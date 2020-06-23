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

#ifndef _OPENGR_ALGO_CSE_
#define _OPENGR_ALGO_CSE_

#include <vector>

#ifdef OpenGR_USE_OPENMP
#include <omp.h>
#endif

#include "gr/shared.h"
#include "gr/algorithms/matchBase.h"

#ifdef TEST_GLOBAL_TIMINGS
#   include "gr/utils/timer.h"
#endif

namespace gr{


template < class Derived, class TBase>
class CongruentSetExplorationOptions : public  TBase
{
public:
    using Scalar = typename TBase::Scalar;
    
    inline bool configureOverlap(Scalar overlap_, Scalar terminate_threshold_ = Scalar(1)) {
        if(terminate_threshold_ < overlap_) return false;
        overlap_estimation = overlap_;
        terminate_threshold = terminate_threshold_;
        return true;
    }
    inline Scalar getTerminateThreshold() const { return terminate_threshold; }
    inline Scalar getOverlapEstimation()  const { return overlap_estimation; }
private:
    /// Threshold on the value of the target function (LCP, see the paper).
    /// It is used to terminate the process once we reached this value.
    Scalar terminate_threshold = 1.0;
    /// Estimated overlap between P and Q. This is the fraction of points in P that
    /// may have corresponding point in Q. It's being used to estimate the number
    /// of RANSAC iterations needed to guarantee small failure probability.
    Scalar overlap_estimation = 0.2;
};


/// \brief Base class for Congruent Sec Exploration algorithms
/// \tparam _Traits Defines properties of the Base used to build the congruent set.
template <typename _Traits,
          typename _PointType,
          typename _TransformVisitor,
          typename _PairFilteringFunctor, /// <\brief Must implements PairFilterConcept
          template < class, class > class ... OptExts >
class CongruentSetExplorationBase : public MatchBase<_PointType, _TransformVisitor, OptExts ..., CongruentSetExplorationOptions> {

public:
    using Traits = _Traits;
    using TransformVisitor = _TransformVisitor;
    using CongruentBaseType = typename Traits::Base;
    using Set = typename Traits::Set;
    using Coordinates = typename Traits::Coordinates;
    using PairFilteringFunctor = _PairFilteringFunctor;

    using MatchBaseType = MatchBase<_PointType, _TransformVisitor, OptExts ..., CongruentSetExplorationOptions>;
    using PosMutablePoint = typename MatchBaseType::PosMutablePoint;
    using OptionsType = typename MatchBaseType::OptionsType;

    using PairsVector =  std::vector< std::pair<int, int> >;
    using Scalar = typename MatchBaseType::Scalar;
    using VectorType = typename MatchBaseType::VectorType;
    using MatrixType = typename MatchBaseType::MatrixType;
    static constexpr Scalar kLargeNumber = 1e9;
    static constexpr Scalar distance_factor = 2.0;

    using LogLevel = typename MatchBaseType::LogLevel;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW


    CongruentSetExplorationBase(const OptionsType& options
            , const Utils::Logger &logger
    );

    virtual ~CongruentSetExplorationBase();

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
                                 const Sampler<_PointType>& sampler,
                                 TransformVisitor& v);

    /// Tries to compute an inital base from P
    /// @param [out] base The base, if found. Initial value is not used. Modified as 
    /// the computed base if the return value is true.
    /// @return true if a base is found an initialized, false otherwise
    virtual bool initBase (CongruentBaseType &base) = 0;
    
protected:
    /// Number of trials. Every trial picks random base from P.
    int number_of_trials_;
    /// The points in the base (indices to P). It is being updated in every
    /// RANSAC iteration.
    CongruentBaseType base_;
    /// The current congruent 4 points from Q. Every RANSAC iteration the
    /// algorithm examines a set of such congruent 4-points from Q and retains
    /// the best from them (the one that realizes the best LCP).
    CongruentBaseType current_congruent_;
    /// The 3D points of the base.
    Coordinates base_3D_;
    /// The best LCP (Largest Common Point) fraction so far.
    Scalar best_LCP_;
    /// Current trial.
    int current_trial_;

#ifdef OpenGR_USE_OPENMP
    /// number of threads used to verify the congruent set
    const int omp_nthread_congruent_;
#endif

#ifdef TEST_GLOBAL_TIMINGS

    mutable Scalar totalTime;
    mutable Scalar kdTreeTime;
    mutable Scalar verifyTime;

    using Timer = gr::Utils::Timer;

#endif

protected :
    /// Performs n RANSAC iterations, each one of them containing base selection,
    /// finding congruent sets and verification. Returns true if the process can be
    /// terminated (the target LCP was obtained or the maximum number of trials has
    /// been reached), false otherwise.
    bool Perform_N_steps(int n,
                         Eigen::Ref<MatrixType> transformation,
                         TransformVisitor& v);
    /// Tries one base and finds the best transformation for this base.
    /// Returns true if the achieved LCP is greater than terminate_threshold_,
    /// else otherwise.
    bool TryOneBase(TransformVisitor &v);

    /// Loop over the set of congruent 4-points and test the compatibility with the
    /// input base.
    /// \param [out] Nb Number of quads corresponding to valid configurations
    bool TryCongruentSet(CongruentBaseType& base, Set& set, TransformVisitor &v,size_t &nbCongruent);

    const CongruentBaseType& base3D() const { return base_3D_; }

    /// Find all the congruent set similar to the base in the second 3D model (Q).
    /// It could be with a 3 point base or a 4 point base.
    /// \param base use to find the similar points congruent in Q.
    /// \param congruent_set a set of all point congruent found in Q.
    virtual bool generateCongruents (CongruentBaseType& base,Set& congruent_set) = 0;

    /// For each randomly picked base, verifies the computed transformation by
    /// computing the number of points that this transformation brings near points
    /// in Q. Returns the current LCP. R is the rotation matrix, (tx,ty,tz) is
    /// the translation vector and (cx,cy,cz) is the center of transformation.template <class MatrixDerived>
    Scalar Verify(const Eigen::Ref<const MatrixType> & mat) const;

}; /// class MatchBaseType
} /// namespace Super4PCS
#include "congruentSetExplorationBase.hpp"

#endif // _OPENGR_ALGO_CSE_
