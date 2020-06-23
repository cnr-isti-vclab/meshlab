# Using Your Point Type {#using-your-point-type}

OpenGR provides gr::PointConcept, which it relies on while operating with the point samples internally. While gr::PointConcept yields the least requirements for any point type that could be used as input to OpenGR, additional features could be introduced to be used as you extend the library, e.g., to employ different point filters while comparing two point instances considering additional features.

When an existing external point type is going to be used as input to OpenGR, gr::PointConcept can be implemented to behave as a wrapper (i.e. adapter) for the external point type. In this case, two types will be in question: the external point type whose instances are available to be passed to OpenGR, and the point type whose implementation is available to work as an adapter of the external point type to provide required interface to OpenGR. For registration, at the sampling stage, points are reinstantiated of the point adapter type which is given as template point type parameter. The requirement with gr::PointConcept for a constructor, which takes an instance of the external point type as input, is handy when the given point adapter type is not the same as the type of the input point instances. In other words, this constructor is used to reinstantiate the point sample with the given point adapter type at the sampling stage, so that, the provided interface could be used internally through the adapter.

In the other case, if the type of the input point instances provides the required interface, the type of input point instances could be passed as the template type parameter. This way, the required constructor turns out to be the copy constructor of the point type. Therefore, the sample points are copied using the copy constructor of the point type at the sampling stage. Although this allows to avoid implementing an additional adapter type, one could consider using an mapper adapter if the copy constructor causes memory duplicates of the underlying point sample data.

Internally, OpenGR uses Eigen for computation, vector representation and vectorization. Therefore, it expects gr::PointConcept::VectorType to be compatible with Eigen::DenseBase, as apparent in gr::PointConcept. To use existing point types without requiring any memory duplication while complying with this requirement, Eigen::Map could be used to map data of existing point types as an Eigen matrix or vector.

For convenience, OpenGR provides an implementation for gr::PointConcept: gr::Point3D. IO methods for gr::Point3D is also provided with the library to use the library on supported point cloud files without needing to implement the point concept and the IO methods. 

Following examples show two simple point type classes demonstrating possible point types external to OpenGR, and the adapters for those point types that allow OpenGR to work on the instances of them without needing any duplication of data while complying with the required interface of the point type concept: gr::PointConcept.

### Example: Using an external point type: `extlib1::PointType1`

\anchor extlib1-pointtype
\snippet ExtPointBinding/ext/point_extlib1.hpp extlib1::PointType1

Assuming that we have an external point type `extlib1::PointType1` as defined above, 
which is used by some library ExtLib1, it is sufficient to implement the below point
adapter `extlib1::PointAdapter`, and pass the type of the point adapter as the point
type while instantiating any template class or method of OpenGR. OpenGR will wrap
your point type with the adapter, and use the interface provided by the adapter
for its computations.

\anchor extlib1-pointadapter
\snippet ExtPointBinding/ext/pointadapter_extlib1.hpp extlib1::PointAdapter

Note that some members of the point adapter `extlib1::PointAdapter` is not required
by default, but could be useful when used with point filters that make use of those
members. For example, the methods `normal()` and `color()` are handful for point
filters that use normal and color attributes of points while comparing two points,
such as gr::AdaptivePointFilter. 

The `ExtPointBinding` demo demonstrates the external point binding through point
adapters by using the above external point type `extlib1::PointType1` together
with another external point type `extlib2::PointType2`.

#### Registration using Super4PCS
In this section, registration of points of an example external point type is briefly
discussed by referring to the point adapter shown. This example is extracted from
the demo `ExtPointBinding`, for which complete code could be found.

Let's say we have two `std::vector` instances that contain two point clouds with
point type \ref extlib1-pointtype "extlib1::PointType1", which we would like to compute a registration
transform using Super4PCS algorithm:

\snippet ExtPointBinding/external_point_binding_test.cc Creating a vector of extlib1::PointType1

In such case, we need to use \ref extlib1-pointadapter "extlib1::PointAdapter" as our point type to allow
OpenGR retrive the required attributes of our points of type \ref extlib1-pointtype "extlib1::PointType1"
by wrapping them with \ref extlib1-pointadapter "extlib1::PointAdapter" on-the-fly. Therefore, the type of
the adapter is used to instantiate the matcher type:
\snippet ExtPointBinding/external_point_binding_test.cc Point adapter and matcher type

Also, the sampler type is instantiated using the point adapter type as sampler needs
to retrieve attributes of the point as well:
\snippet ExtPointBinding/external_point_binding_test.cc Sampler type

And, the rest for the registration transformation computation is as regular, instantiating
the matcher and computing the registration transformation. Notice that the instances
of the external point type are directly passed to the matcher, without requiring
to make any type conversions:
\snippet ExtPointBinding/external_point_binding_test.cc Matcher instantiation and computation

### Example: Using another external point type: `extlib2::PointType2`
\snippet ExtPointBinding/ext/point_extlib2.hpp extlib2::PointType2

Here, a different external point type, `extlib2::PointType2`, is shown, for which
the point binding to OpenGR could be done using the below adapter type, `extlib2::PointAdapter`,
in an efficient manner thanks to Eigen maps.

\snippet ExtPointBinding/ext/pointadapter_extlib2.hpp extlib2::PointAdapter

For any external point type, when a proper point adapter is defined, which could
be done with little effort in an efficient manner for most cases, OpenGR could be
utilized as shown.