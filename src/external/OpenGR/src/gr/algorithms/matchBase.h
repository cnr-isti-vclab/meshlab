// Copyright 2017 Nicolas Mellado
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -------------------------------------------------------------------------- //
//
// Authors: Dror Aiger, Yoni Weill, Nicolas Mellado
//
// This file is part of the implementation of the 4-points Congruent Sets (4PCS)
// algorithm presented in:
//
// 4-points Congruent Sets for Robust Surface Registration
// Dror Aiger, Niloy J. Mitra, Daniel Cohen-Or
// ACM SIGGRAPH 2008 and ACM Transaction of Graphics.
//
// Given two sets of points in 3-space, P and Q, the algorithm applies RANSAC
// in roughly O(n^2) time instead of O(n^3) for standard RANSAC, using an
// efficient method based on invariants, to find the set of all 4-points in Q
// that can be matched by rigid transformation to a given set of 4-points in P
// called a base. This avoids the need to examine all sets of 3-points in Q
// against any base of 3-points in P as in standard RANSAC.
// The algorithm can use colors and normals to speed-up the matching
// and to improve the quality. It can be easily extended to affine/similarity
// transformation but then the speed-up is smaller because of the large number
// of congruent sets. The algorithm can also limit the range of transformations
// when the application knows something on the initial pose but this is not
// necessary in general (though can speed the runtime significantly).

// Home page of the 4PCS project (containing the paper, presentations and a
// demo): http://graphics.stanford.edu/~niloy/research/fpcs/fpcs_sig_08.html
// Use google search on "4-points congruent sets" to see many related papers
// and applications.

#ifndef _OPENGR_ALGO_MATCH_BASE_
#define _OPENGR_ALGO_MATCH_BASE_

#include <vector>

#ifdef OpenGR_USE_OPENMP
#include <omp.h>
#endif

#include "gr/shared.h"
#include "gr/sampling.h"
#include "gr/accelerators/kdtree.h"
#include "gr/utils/logger.h"
#include "gr/utils/crtp.h"

#ifdef TEST_GLOBAL_TIMINGS
#   include "gr/utils/timer.h"
#endif

namespace gr{

struct DummyTransformVisitor {
    template <typename Derived>
    inline void operator() (float, float, const Eigen::MatrixBase<Derived>&) const {}
    constexpr bool needsGlobalTransformation() const { return false; }
};

/// \brief Abstract class for registration algorithms
template <typename PointType, typename _TransformVisitor = DummyTransformVisitor,
          template < class, class > typename ... OptExts>
class MatchBase {

public:
    using Scalar = typename PointType::Scalar;
    using VectorType = typename PointType::VectorType;
    using MatrixType = Eigen::Matrix<Scalar, 4, 4>;
    using LogLevel = Utils::LogLevel;
    using TransformVisitor = _TransformVisitor;

    template < class Derived, class TBase>
    class Options : public TBase
    {
    public:
        using Scalar = typename PointType::Scalar;

        /// Distance threshold used to compute the LCP
        /// \todo Move to DistanceMeasure
        Scalar delta       = Scalar(5.0);
        /// The number of points in the sample. We sample this number of points
        /// uniformly from P and Q.
        size_t sample_size = 200;
        /// Maximum time we allow the computation to take. This makes the algorithm
        /// an ANY TIME algorithm that can be stopped at any time, producing the best
        /// solution so far.
        /// \warning Max. computation time must be handled in child classes
        int max_time_seconds = 60;
        /// use a constant default seed by default
        unsigned int randomSeed = std::mt19937::default_seed;

        /// Constraints about transformations

        /// Maximum rotation angle. Set negative to ignore
        Scalar max_angle = -1;
        /// Maximum translation distance. Set negative to ignore
        Scalar max_translation_distance = -1;
        // \FIXME std::pair <Scalar, Scalar> scale_range;
    };

    using OptionsType = gr::Utils::CRTP < OptExts ... , Options >;

    /// A convenience class used to wrap (any) PointType to allow mutation of position
    /// of point samples for internal computations. 
    struct PosMutablePoint : public PointType
    {
        using VectorType = typename PointType::VectorType;
        
        private:
            VectorType posCopy;
        
        public:
            template<typename ExternalType>
            PosMutablePoint(const ExternalType& i) 
                : PointType(i), posCopy(PointType(i).pos()) { }

            inline VectorType & pos() { return posCopy; }

            inline VectorType pos() const { return posCopy; }
    };

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    MatchBase(const OptionsType& options, const Utils::Logger &logger);

    virtual ~MatchBase();

    /// Read access to the sampled clouds used for the registration
    const std::vector<PosMutablePoint>& getFirstSampled() const {
        return sampled_P_3D_;
    }

    /// Read access to the sampled clouds used for the registration
    const std::vector<PosMutablePoint>& getSecondSampled() const {
        return sampled_Q_3D_;
    }


#ifdef PARSED_BY_DOXYGEN
    /// Computes an approximation of the best LCP (directional) from Q to P
    /// and the rigid transformation that realizes it. The input sets may or may
    /// not contain normal information for any point.
    /// @param [in] P The first input set.
    /// @param [in] Q The second input set.
    /// @param [out] transformation Rigid transformation matrix (4x4) that brings
    /// Q to the (approximate) optimal LCP. Initial value is considered as a guess
    /// @return the computed LCP measure as a fraction of the size of P ([0..1]).
/*    template <typename Sampler>
    Scalar ComputeTransformation(const std::vector<Point3D>& P,
                                 const std::vector<Point3D>& Q,
                                 Eigen::Ref<MatrixType> transformation,
                                 const Sampler& sampler,
                                 TransformVisitor& v) {}
*/
    /// Computes an approximation of the best LCP (directional) from Q to P
    /// and the rigid transformation that realizes it. The input sets may or may
    /// not contain normal information for any point.
    /// @param [in] P The first input set.
    /// @param [in] Q The second input set.
    /// @param [out] transformation Rigid transformation matrix (4x4) that brings
    /// Q to the (approximate) optimal LCP. Initial value is considered as a guess
    /// @return the computed LCP measure as a fraction of the size of P ([0..1]).
    template <typename InputRange1,
              typename InputRange2,
              template<typename> typename Sampler>
    Scalar ComputeTransformation(const InputRange1& P,
                                 const InputRange2& Q,
                                 Eigen::Ref<MatrixType> transformation,
                                 const Sampler<PointType>& sampler,
                                 TransformVisitor& v) {}


#endif

protected:
    /// Maximum base diameter. It is computed automatically from the diameter of
    /// P and the estimated overlap and used to limit the distance between the
    /// points in the base in P so that the probability to have all points in
    /// the base as inliers is increased.
    Scalar max_base_diameter_;
    /// The diameter of P.
    Scalar P_diameter_;
    /// Mean distance between points and their nearest neighbor in the set P.
    /// Used to normalize the "delta" which is given in terms of this distance.
    Scalar P_mean_distance_;
    /// The transformation matrix by wich we transform Q to P
    Eigen::Matrix<Scalar, 4, 4> transform_;
    /// Sampled P (3D coordinates).
    std::vector<PosMutablePoint> sampled_P_3D_;
    /// Sampled Q (3D coordinates).
    std::vector<PosMutablePoint> sampled_Q_3D_;
    /// The centroid of P.
    VectorType centroid_P_;
    /// The centroid of Q.
    VectorType centroid_Q_;
    VectorType qcentroid1_;
    VectorType qcentroid2_;
    /// KdTree used to compute the LCP
    KdTree<Scalar> kd_tree_;
    std::mt19937 randomGenerator_;
    const Utils::Logger &logger_;

    OptionsType options_;

    /// \todo Rationnalize use and name of this variable
    static constexpr int kNumberOfDiameterTrials = 1000;

protected :
    template <Utils::LogLevel level, typename...Args>
    inline void Log(Args...args) const { logger_.Log<level>(args...); }


    /// Computes the mean distance between points in Q and their nearest neighbor.
    /// We need this for normalization of the user delta (See the paper) to the
    /// "scale" of the set.
    Scalar MeanDistance() const;


    /// Selects a random triangle in the set P (then we add another point to keep the
    /// base as planar as possible). We apply a simple heuristic that works in most
    /// practical cases. The idea is to accept maximum distance, computed by the
    /// estimated overlap, multiplied by the diameter of P, and try to have
    /// a triangle with all three edges close to this distance. Wide triangles helps
    /// to make the transformation robust while too large triangles makes the
    /// probability of having all points in the inliers small so we try to trade-off.
    bool SelectRandomTriangle(int& base1, int& base2, int& base3);

    /// Computes the best rigid transformation between three corresponding pairs.
    /// The transformation is characterized by rotation matrix, translation vector
    /// and a center about which we rotate. The set of pairs is potentially being
    /// updated by the best permutation of the second set. Returns the RMS of the
    /// fit. The method is being called with n points but it applies the fit for
    /// only 3 after the best permutation is selected in the second set (see
    /// bellow). This is done because the solution for planar points is much
    /// simpler.
    /// The method is the closed-form solution by Horn:
    /// people.csail.mit.edu/bkph/papers/Absolute_Orientation.pdf
    /// \tparam Coordinates Struct with operator[](int i) ->i\in[0:2]
    template <typename Coordinates>
    bool ComputeRigidTransformation(const Coordinates& ref,
                                    const Coordinates& candidate,
                                    const Eigen::Matrix<Scalar, 3, 1>& centroid1,
                                    Eigen::Matrix<Scalar, 3, 1> centroid2,
                                    Eigen::Ref<MatrixType> transform,
                                    Scalar& rms_,
                                    bool computeScale ) const;

    /// Initializes the data structures and needed values before the match
    /// computation.
    /// This method is called once the internal state of the Base class as been
    /// set.
    virtual void Initialize() { }

    /// Initializes the internal state of the Base class
    /// @param P The first input set.
    /// @param Q The second input set.
    /// @param sampler The sampler used to sample the input sets.
    template <typename InputRange1, typename InputRange2, template<typename> typename Sampler>
    void init(const InputRange1& P,
              const InputRange2& Q,
              const Sampler<PointType>& sampler);

private:

    void initKdTree();

}; /// class MatchBase
} /// namespace Super4PCS
#include "matchBase.hpp"

#endif // _OPENGR_ALGO_MATCH_BASE_
