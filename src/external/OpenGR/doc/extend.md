# Extending the library {#extend}

## Add your own matcher class
OpenGR provides a generic base class gr::MatchBase defining the interface for registration algorithms.
Assuming you want to add a new registration algorithm, and depending on the genericity you want to provide to the users, you can consider three different scenarios, from the simplest to the most advanced.
Scenario 2 is probably describing the most usual case.

### 1. Simple extension without genericity constraint or custom option
In order to write your own registration algorithm, you need to inherit gr::MatchBase, and implement the initialization and processing methods:
```{.cpp}
namespace gr {
template <typename _TransformVisitor >
class MatchTestSimple : public gr::MatchBase<_TransformVisitor> {
public:
  using MatchBaseType = gr::MatchBase<_TransformVisitor>;
  using OptionsType   = typename MatchBaseType::OptionsType;
  using Scalar        = typename MatchBaseType::Scalar;
  using MatrixType    = typename MatchBaseType::MatrixType;

  MatchTestSimple (const OptionsType& options, const gr::Utils::Logger& logger)
    :MatchBaseType(options, logger) { }

  // Initializes the data structures and needed values before the match computation.
  // This method is called once the internal state of the Base class as been set.
  void Initialize(const std::vector<Point3D>& P,
                  const std::vector<Point3D>& Q) override { /* ... */}

  // Computes an approximation of the best LCP (directional) from Q to P
  // and the rigid transformation that realizes it.
  // See gr::MatchBase::ComputeTransformation for more details
  Scalar ComputeTransformation(const std::vector<Point3D>& P,
                               std::vector<Point3D>* Q,
                               Eigen::Ref<MatrixType> transformation,
                               const Sampler& sampler,
                               _TransformVisitor& v) override { return 0; }
};
}
```

### 2. Simple extension with custom options
gr::MatchBase holds a protected field `OptionsType options_;` that is used to specify parameters sets to the algorithms.
In case your algorithm needs to be configured by some parameter values, `OptionsType` needs to be extended with your own parameter set.

Suppose that `MatchTestSimple` needs two parameters `value` and `point`. First, you need to declare these parameters in a structure:
```{.cpp}
namespace gr {
template < class Derived, class TBase>
struct MatchTestSimpleOptions : TBase
{
  typename TBase::Scalar value;
  typename gr::Point3D   point;
};
}
```

Then, you need to extend `OptionsType` with `MatchTestSimpleOptions`.
Technically, `OptionsType` is defined as an aggregate () between gr::MatchBase::Options and any other custom extensions passed as template parameter.
This is automatically done by inhering gr::MatchBase as follow:
```{.cpp}
namespace gr {
template <typename _TransformVisitor>
class MatchTestSimple : public gr::MatchBase<_TransformVisitor, MatchTestSimpleOptions> { /* ... */ };
}
```
Then, the parameter values can be accessed as `options_.value` and `options_.point` in `MatchTestSimple`.

### 3. Generic new matcher type
In case you plan to add a new type of matcher with several implementations, you might need to provide a fully transparent propagation mechanism for the algorithm variants options.
The mechanism presented in this section is exactly the one we used to implement gr::CongruentSetExplorationBase, which is inherited by gr::Match3pcs and gr::Match4pcsBase.
```{.cpp}
template <typename _TransformVisitor,
          template < class, class > typename ... OptExts >
class MyCustomMatcherBase : public gr::MatchBase<_TransformVisitor , OptExts ...  , MyCustomBaseOptions> { /* ... */ };
}
```
and one of its child classes implementing the variants
```{.cpp}
namespace gr {
template <typename _TransformVisitor >
class Variant1 : public MyCustomMatcherBase<_TransformVisitor, Variant1Options> { /* ... */ };
}
```


## Add your own point filter
The generic matcher type gr::CongruentSetExplorationBase allows filtering pair of points during exploration, which is done with point filters that are attachable while instantiation of matcher class, effective at inherited gr::Match3pcs and gr::Match4pcsBase types. If you plan to use your filtering logic considering the features of your point samples, you could implement gr::PairFilterConcept and attach it to your preferred matcher of descendent type of gr::CongruentSetExplorationBase while type instantiation.

OpenGR provides gr::AdaptivePointFilter, an implementation of gr::PairFilterConcept, which is a point filter that filters pairs of points using their normal, color and max motion features when available. For using arbitrary attributes of your point type in your point filter, check @ref using-your-point-type page.