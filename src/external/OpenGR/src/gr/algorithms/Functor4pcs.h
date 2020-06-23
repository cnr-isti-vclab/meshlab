//
// Created by Sandra Alfaro on 24/04/18.
//

#ifndef OPENGR_FUNCTOR4PCS_H
#define OPENGR_FUNCTOR4PCS_H

#include <vector>
#include "gr/shared.h"


namespace gr {
    /// Processing functor for the computation of the 4PCS algorithm
    /// \see Match4pcsBase
    /// \tparam PairFilterFunctor filters pairs of points during the exploration.
    ///         Must implement PairFilterConcept
    template <typename PointType, typename PairFilterFunctor, typename Options>
    struct Functor4PCS {
    public :
        using BaseCoordinates = typename Traits4pcs<PointType>::Coordinates;
        using Scalar      = typename PointType::Scalar;
        using PairsVector = std::vector< std::pair<int, int> >;
        using VectorType  = typename PointType::VectorType;
        using OptionType  = Options;


    private :
        OptionType myOptions_;
        std::vector<PointType>& mySampled_Q_3D_;
        BaseCoordinates &myBase_3D_;


    public :
        inline Functor4PCS(std::vector<PointType> &sampled_Q_3D_,
                         BaseCoordinates& base_3D_,
                         const OptionType &options)
                        :mySampled_Q_3D_(sampled_Q_3D_)
                        ,myBase_3D_(base_3D_)
                        ,myOptions_ (options) {}

        /// Initializes the data structures and needed values before the match
        /// computation.
        inline void Initialize() {}

        /// Finds congruent candidates in the set Q, given the invariants and threshold distances.
        /// Returns true if a non empty set can be found, false otherwise.
        /// @param invariant1 [in] The first invariant corresponding to the set P_pairs
        /// of pairs, previously extracted from Q.
        /// @param invariant2 [in] The second invariant corresponding to the set
        /// Q_pairs of pairs, previously extracted from Q.
        /// @param [in] distance_threshold1 The distance for verification.
        /// @param [in] distance_threshold2 The distance for matching middle points due
        /// to the invariants (See the paper for e1, e2).
        /// @param [in] First_pairs The first set of pairs found in Q.
        /// @param [in] Second_pairs The second set of pairs found in Q.
        /// @param [out] quadrilaterals The set of congruent quadrilateral. In fact,
        /// it's a super set from which we extract the real congruent set.
        inline bool FindCongruentQuadrilaterals(
                                         Scalar invariant1,
                                         Scalar invariant2,
                                         Scalar /*distance_threshold1*/,
                                         Scalar distance_threshold2,
                                         const std::vector <std::pair<int, int>> &First_pairs,
                                         const std::vector <std::pair<int, int>> &Second_pairs,
                                         typename Traits4pcs<PointType>::Set* quadrilaterals) const {
            using RangeQuery = typename gr::KdTree<Scalar>::template RangeQuery<>;

            if (quadrilaterals == nullptr) return false;

            size_t number_of_points = 2 * First_pairs.size();

            // We need a temporary kdtree to store the new points corresponding to
            // invariants in the First_pairs and then query them (for range search) for all
            // the new points corresponding to the invariants in Second_pairs.
            quadrilaterals->clear();

            gr::KdTree<Scalar> kdtree(number_of_points);

            // Build the kdtree tree using the invariants on First_pairs.
            for (size_t i = 0; i < First_pairs.size(); ++i) {
                const VectorType &p1 = mySampled_Q_3D_[First_pairs[i].first].pos();
                const VectorType &p2 = mySampled_Q_3D_[First_pairs[i].second].pos();
                kdtree.add(p1 + invariant1 * (p2 - p1));
            }
            kdtree.finalize();

            //Point3D invRes;
            // Query the Kdtree for all the points corresponding to the invariants in Second_pairs.
            for (size_t i = 0; i < Second_pairs.size(); ++i) {
                const VectorType &p1 = mySampled_Q_3D_[Second_pairs[i].first].pos();
                const VectorType &p2 = mySampled_Q_3D_[Second_pairs[i].second].pos();

                RangeQuery query;
                query.queryPoint = p1 + invariant2 * (p2 - p1);
                query.sqdist = distance_threshold2;

                kdtree.doQueryDistProcessIndices(query,
                                                 [quadrilaterals, i, &First_pairs, &Second_pairs](int id) {
                                                     quadrilaterals->push_back(
                                                             { First_pairs[id].first,
                                                               First_pairs[id].second,
                                                               Second_pairs[i].first,
                                                               Second_pairs[i].second });
                                                 });
            }

            return quadrilaterals->size() != 0;
        }

        /// Constructs pairs of points in Q, corresponding to a single pair in the
        /// in basein P.
        /// @param [in] pair_distance The distance between the pairs in P that we have
        /// to match in the pairs we select from Q.
        /// @param [in] pair_normal_distance The angle between the normals of the pair
        /// in P.
        /// @param [in] pair_distance_epsilon Tolerance on the pair distance. We allow
        /// candidate pair in Q to have distance of
        /// pair_distance+-pair_distance_epsilon.
        /// @param [in] base_point1 The index of the first point in P.
        /// @param [in] base_point2 The index of the second point in P.
        /// @param [out] pairs A set of pairs in Q that match the pair in P with
        /// respect to distance and normals, up to the given tolerance.
       inline void ExtractPairs(Scalar pair_distance,
                                Scalar pair_normals_angle,
                                Scalar pair_distance_epsilon,
                                int base_point1,
                                int base_point2,
                                PairsVector* pairs) const {
            if (pairs == nullptr) return;

            pairs->clear();
            pairs->reserve(2 * mySampled_Q_3D_.size());

            PairFilterFunctor fun;

            // Go over all ordered pairs in Q.
            for (size_t j = 0; j < mySampled_Q_3D_.size(); ++j) {
                const PointType& p = mySampled_Q_3D_[j];
                for (size_t i = j + 1; i < mySampled_Q_3D_.size(); ++i) {
                    const PointType& q = mySampled_Q_3D_[i];
#ifndef MULTISCALE
                    // Compute the distance and two normal angles to ensure working with
                    // wrong orientation. We want to verify that the angle between the
                    // normals is close to the angle between normals in the base. This can be
                    // checked independent of the full rotation angles which are not yet
                    // defined by segment matching alone..
                    const Scalar distance = (q.pos() - p.pos()).norm();
                    if (std::abs(distance - pair_distance) > pair_distance_epsilon) continue;
#endif

                    std::pair<bool,bool> res = fun(p,q, pair_normals_angle, *myBase_3D_[base_point1],*myBase_3D_[base_point2], myOptions_);
                    if (res.first)
                        pairs->emplace_back(i, j);
                    if (res.second)
                        pairs->emplace_back(j, i);
                }
            }
        }

     };
}


#endif //OPENGR_FUNCTOR4PCS_H
