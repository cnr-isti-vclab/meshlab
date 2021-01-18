//
// Created by Sandra Alfaro on 24/05/18.
//

#include <vector>
#include <chrono>
#include <atomic>
#include <Eigen/Geometry>                 // MatrixBase.homogeneous()
#include <Eigen/SVD>
#include <Eigen/Core>                     // Transform.computeRotationScaling()


#ifdef OpenGR_USE_OPENMP
#include <omp.h>
#endif

#include "../shared.h"
#include "../sampling.h"
#include "../accelerators/kdtree.h"
#include "../utils/logger.h"
#include "match4pcsBase.h"

#ifdef TEST_GLOBAL_TIMINGS
#   include "gr/utils/timer.h"
#endif


namespace gr {
    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::Match4pcsBase (const OptionsType& options
            , const Utils::Logger& logger)
            : MatchBaseType(options,logger)
            , fun_(MatchBaseType::sampled_Q_3D_,MatchBaseType::base_3D_,MatchBaseType::options_)
    {
    }

    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::~Match4pcsBase() {}

    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::TryQuadrilateral(
        typename PointType::Scalar &invariant1,
        typename PointType::Scalar &invariant2,
        int &id1, int &id2, int &id3, int &id4) {

        Scalar min_distance = std::numeric_limits<Scalar>::max();
        int best1, best2, best3, best4;
        best1 = best2 = best3 = best4 = -1;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i == j) continue;
                int k = 0;
                while (k == i || k == j) k++;
                int l = 0;
                while (l == i || l == j || l == k) l++;
                Scalar local_invariant1;
                Scalar local_invariant2;
                // Compute the closest points on both segments, the corresponding
                // invariants and the distance between the closest points.
                Scalar segment_distance = distSegmentToSegment(
                        MatchBaseType::base_3D_[i]->pos(), MatchBaseType::base_3D_[j]->pos(),
                        MatchBaseType::base_3D_[k]->pos(), MatchBaseType::base_3D_[l]->pos(),
                        local_invariant1, local_invariant2);
                // Retail the smallest distance and the best order so far.
                if (segment_distance < min_distance) {
                    min_distance = segment_distance;
                    best1 = i;
                    best2 = j;
                    best3 = k;
                    best4 = l;
                    invariant1 = local_invariant1;
                    invariant2 = local_invariant2;
                }
            }
        }

        if(best1 < 0 || best2 < 0 || best3 < 0 || best4 < 0 ) return false;

        Coordinates tmp = MatchBaseType::base_3D_;
        MatchBaseType::base_3D_[0] = tmp[best1];
        MatchBaseType::base_3D_[1] = tmp[best2];
        MatchBaseType::base_3D_[2] = tmp[best3];
        MatchBaseType::base_3D_[3] = tmp[best4];

        CongruentBaseType tmpId = {id1, id2, id3, id4};
        id1 = tmpId[best1];
        id2 = tmpId[best2];
        id3 = tmpId[best3];
        id4 = tmpId[best4];

        return true;
    }

    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::SelectQuadrilateral(
        Scalar &invariant1,
        Scalar &invariant2,
        int& base1, int& base2, int& base3, int& base4)  {

        const Scalar kBaseTooSmall (0.2);
        int current_trial = 0;

        // Try fix number of times.
        while (current_trial < MatchBaseType::kNumberOfDiameterTrials) {
            // Select a triangle if possible. otherwise fail.
            if (!MatchBaseType::SelectRandomTriangle(base1, base2, base3)){
                return false;
            }

            const auto& b0 = *(MatchBaseType::base_3D_[0] = &MatchBaseType::sampled_P_3D_[base1]);
            const auto& b1 = *(MatchBaseType::base_3D_[1] = &MatchBaseType::sampled_P_3D_[base2]);
            const auto& b2 = *(MatchBaseType::base_3D_[2] = &MatchBaseType::sampled_P_3D_[base3]);

            // The 4th point will be a one that is close to be planar to the other 3
            // while still not too close to them.
            const double x1 = b0.pos()(0);
            const double y1 = b0.pos()(1);
            const double z1 = b0.pos()(2);
            const double x2 = b1.pos()(0);
            const double y2 = b1.pos()(1);
            const double z2 = b1.pos()(2);
            const double x3 = b2.pos()(0);
            const double y3 = b2.pos()(1);
            const double z3 = b2.pos()(2);

            // Fit a plan.
            Scalar denom = (-x3 * y2 * z1 + x2 * y3 * z1 + x3 * y1 * z2 - x1 * y3 * z2 -
                            x2 * y1 * z3 + x1 * y2 * z3);

            if (denom != 0) {
                Scalar A =
                        (-y2 * z1 + y3 * z1 + y1 * z2 - y3 * z2 - y1 * z3 + y2 * z3) / denom;
                Scalar B =
                        (x2 * z1 - x3 * z1 - x1 * z2 + x3 * z2 + x1 * z3 - x2 * z3) / denom;
                Scalar C =
                        (-x2 * y1 + x3 * y1 + x1 * y2 - x3 * y2 - x1 * y3 + x2 * y3) / denom;
                base4 = -1;
                Scalar best_distance = std::numeric_limits<Scalar>::max();
                // Go over all points in P.
                const Scalar too_small = std::pow(MatchBaseType::max_base_diameter_ * kBaseTooSmall, 2);
                for (unsigned int i = 0; i < MatchBaseType::sampled_P_3D_.size(); ++i) {
                    const auto &p = MatchBaseType::sampled_P_3D_[i];
                    if ((p.pos() - b0.pos()).squaredNorm() >= too_small &&
                        (p.pos() - b1.pos()).squaredNorm() >= too_small &&
                        (p.pos() - b2.pos()).squaredNorm() >= too_small) {
                        // Not too close to any of the first 3.
                        const Scalar distance =
                                std::abs(A * p.pos()(0) + B * p.pos()(1) + C * p.pos()(2) - 1.0);
                        // Search for the most planar.
                        if (distance < best_distance) {
                            best_distance = distance;
                            base4 = int(i);
                        }
                    }
                }
                // If we have a good one we can quit.
                if (base4 != -1) {
                    MatchBaseType::base_3D_[3] = &MatchBaseType::sampled_P_3D_[base4];
                    if(TryQuadrilateral(invariant1, invariant2, base1, base2, base3, base4))
                        return true;
                }
            }
            current_trial++;
        }

        // We failed to find good enough base..
        return false;
    }

    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    // Initialize all internal data structures and data members.
    void Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::Initialize() {
        fun_.Initialize();
    }


    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::generateCongruents (
        CongruentBaseType &base, Set& congruent_quads) {
//      std::cout << "------------------" << std::endl;
        Scalar invariant1, invariant2;

        if(!initBase(base, invariant1, invariant2)) return false;

//        std::cout << "Found a new base !" << std::endl;
        const auto& b0 = *MatchBaseType::base_3D_[0];
        const auto& b1 = *MatchBaseType::base_3D_[1];
        const auto& b2 = *MatchBaseType::base_3D_[2];
        const auto& b3 = *MatchBaseType::base_3D_[3];

        // Computes distance between pairs.
        const Scalar distance1 = (b0.pos()- b1.pos()).norm();
        const Scalar distance2 = (b2.pos()- b3.pos()).norm();

        std::vector<std::pair<int, int>> pairs1, pairs2;

        // Compute normal angles.
        const Scalar normal_angle1 = (b0.normal() - b1.normal()).norm();
        const Scalar normal_angle2 = (b2.normal() - b3.normal()).norm();

        fun_.ExtractPairs(distance1, normal_angle1, MatchBaseType::distance_factor * MatchBaseType::options_.delta, 0, 1, &pairs1);
        fun_.ExtractPairs(distance2, normal_angle2, MatchBaseType::distance_factor * MatchBaseType::options_.delta, 2, 3, &pairs2);


//        std::cout << "Pair set 1 has " << pairs1.size() << " elements" << std::endl;
//        std::cout << "Pair set 2 has " << pairs2.size() << " elements" << std::endl;

//  Log<LogLevel::Verbose>( "Pair creation ouput: ", pairs1.size(), " - ", pairs2.size());

        if (pairs1.size() == 0 || pairs2.size() == 0) {
            return false;
        }

        if (!fun_.FindCongruentQuadrilaterals(invariant1, invariant2,
                                         MatchBaseType::distance_factor * MatchBaseType::options_.delta,
                                         MatchBaseType::distance_factor * MatchBaseType::options_.delta,
                                         pairs1,
                                         pairs2,
                                         &congruent_quads)) {
            return false;
        }

        return true;
    }

    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::initBase (CongruentBaseType &base)
    {
        Scalar invariant1, invariant2; // dummy

        return initBase(base, invariant1, invariant2);
    }

    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::initBase (CongruentBaseType &base, Scalar& invariant1, Scalar& invariant2)
    {
        #ifdef STATIC_BASE
        static bool first_time = true;

        if (first_time){
            std::cerr << "Warning: Running with static base" << std::endl;
            base[0] = 0;
            base[1] = 3;
            base[2] = 1;
            base[3] = 4;

            MatchBaseType::base_3D_[0] = &MatchBaseType::sampled_P_3D_ [base[0]];
            MatchBaseType::base_3D_[1] = &MatchBaseType::sampled_P_3D_ [base[1]];
            MatchBaseType::base_3D_[2] = &MatchBaseType::sampled_P_3D_ [base[2]];
            MatchBaseType::base_3D_[3] = &MatchBaseType::sampled_P_3D_ [base[3]];
            TryQuadrilateral(invariant1, invariant2, base[0], base[1], base[2], base[3]);

            first_time = false;
        }
        else
            return false;

        #else
            if (!SelectQuadrilateral(invariant1, invariant2, base[0], base[1], base[2], base[3])) {
                // std::cout << "Skipping wrong base" << std::endl;
                return false;
            }
        #endif

        return true;
    }


    template <template <typename, typename, typename> typename _Functor,
              typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    typename Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::Scalar
    Match4pcsBase<_Functor, PointType, TransformVisitor, PairFilteringFunctor, PFO>::distSegmentToSegment(
        const VectorType& p1, const VectorType& p2,
        const VectorType& q1, const VectorType& q2,
        Scalar& invariant1, Scalar& invariant2) {

        static const Scalar kSmallNumber = 0.0001;
        VectorType u = p2 - p1;
        VectorType v = q2 - q1;
        VectorType w = p1 - q1;
        Scalar a = u.dot(u);
        Scalar b = u.dot(v);
        Scalar c = v.dot(v);
        Scalar d = u.dot(w);
        Scalar e = v.dot(w);
        Scalar f = a * c - b * b;
        // s1,s2 and t1,t2 are the parametric representation of the intersection.
        // they will be the invariants at the end of this simple computation.
        Scalar s1 = 0.0;
        Scalar s2 = f;
        Scalar t1 = 0.0;
        Scalar t2 = f;

        if (f < kSmallNumber) {
            s1 = 0.0;
            s2 = 1.0;
            t1 = e;
            t2 = c;
        } else {
            s1 = (b * e - c * d);
            t1 = (a * e - b * d);
            if (s1 < 0.0) {
                s1 = 0.0;
                t1 = e;
                t2 = c;
            } else if (s1 > s2) {
                s1 = s2;
                t1 = e + b;
                t2 = c;
            }
        }

        if (t1 < 0.0) {
            t1 = 0.0;
            if (-d < 0.0)
                s1 = 0.0;
            else if (-d > a)
                s1 = s2;
            else {
                s1 = -d;
                s2 = a;
            }
        } else if (t1 > t2) {
            t1 = t2;
            if ((-d + b) < 0.0)
                s1 = 0;
            else if ((-d + b) > a)
                s1 = s2;
            else {
                s1 = (-d + b);
                s2 = a;
            }
        }
        invariant1 = (std::abs(s1) < kSmallNumber ? 0.0 : s1 / s2);
        invariant2 = (std::abs(t1) < kSmallNumber ? 0.0 : t1 / t2);

        return ( w + (invariant1 * u) - (invariant2 * v)).norm();
    }
}

