//
// Created by Sandra Alfaro on 24/04/18.
//

#ifndef SUPER4PCS_FUNCTORSUPER4PCS_H
#define SUPER4PCS_FUNCTORSUPER4PCS_H


#include <vector>
#include "gr/shared.h"
#include "gr/algorithms/pairCreationFunctor.h"

#ifdef SUPER4PCS_USE_CHEALPIX
#include "gr/accelerators/normalHealSet.h"
#else
#include "gr/accelerators/normalset.h"
#include "gr/accelerators/utils.h"

#endif

#include <fstream>
#include <array>
#include <time.h>

namespace gr {

    /// Processing functor for the computation of the Super4PCS algorithm
    /// \see Match4pcsBase
    /// \tparam PairFilterFunctor filters pairs of points during the exploration.
    ///         Must implement PairFilterConcept
    template <typename PointType, typename PointFilterFunctor, typename Options>
    struct FunctorSuper4PCS {
    public :
        using BaseCoordinates = typename Traits4pcs<PointType>::Coordinates;
        using Scalar      = typename PointType::Scalar;
        using PairsVector = std::vector< std::pair<int, int> >;
        using VectorType  = typename PointType::VectorType;
        using OptionType  = Options;
        using PairCreationFunctorType = PairCreationFunctor<PointType, Scalar, PointFilterFunctor, OptionType>;


    private :
        std::vector<PointType> &mySampled_Q_3D_;
        BaseCoordinates &myBase_3D_;

        mutable PairCreationFunctorType pcfunctor_;


    public :
        inline FunctorSuper4PCS (std::vector<PointType> &sampled_Q_3D_,
                               BaseCoordinates& base_3D_,
                               const OptionType& options)
                                : pcfunctor_ (options,mySampled_Q_3D_)
                                ,mySampled_Q_3D_(sampled_Q_3D_)
                                ,myBase_3D_(base_3D_){}

        /// Initializes the data structures and needed values before the match
        /// computation.
        inline void Initialize() {
            pcfunctor_.synch3DContent();
        }


        /// Constructs pairs of points in Q, corresponding to a single pair in the
        /// in basein P.
        /// @param [in] pair_distance The distance between the pairs in P that we have
        /// to match in the pairs we select from Q.
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

            using namespace gr::Accelerators::PairExtraction;

            pcfunctor_.pairs = pairs;

            pairs->clear();
            pairs->reserve(2 * pcfunctor_.points.size());

            pcfunctor_.pair_distance         = pair_distance;
            pcfunctor_.pair_distance_epsilon = pair_distance_epsilon;
            pcfunctor_.pair_normals_angle    = pair_normals_angle;
            pcfunctor_.setRadius(pair_distance);
            pcfunctor_.setBase(base_point1, base_point2, myBase_3D_);

#ifdef MULTISCALE
            BruteForceFunctor
  <typename PairCreationFunctorType::Primitive, typename PairCreationFunctorType::Point, 3, Scalar> interFunctor;
#else
            IntersectionFunctor
                    <typename PairCreationFunctorType::Primitive,
                     typename PairCreationFunctorType::Point, 3, Scalar> interFunctor;
#endif

            Scalar eps = pcfunctor_.getNormalizedEpsilon(pair_distance_epsilon);

            interFunctor.process(pcfunctor_.primitives,
                                 pcfunctor_.points,
                                 eps,
                                 50,
                                 pcfunctor_);
        }

        /// Finds congruent candidates in the set Q, given the invariants and threshold
        /// distances. Returns true if a non empty set can be found, false otherwise.
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
                const std::vector<std::pair<int, int>>& First_pairs,
                const std::vector<std::pair<int, int>>& Second_pairs,
               typename Traits4pcs<PointType>::Set* quadrilaterals) const {

            typedef typename PairCreationFunctorType::Point Point;

#ifdef SUPER4PCS_USE_CHEALPIX
            typedef gr::IndexedNormalHealSet IndexedNormalSet3D;
#else
            typedef  gr::IndexedNormalSet
                    < Point,   //! \brief Point type used internally
                            3,       //! \brief Nb dimension
                            7,       //! \brief Nb cells/dim normal
                            Scalar>  //! \brief Scalar type
                    IndexedNormalSet3D;
#endif


            if (quadrilaterals == nullptr) return false;

            quadrilaterals->clear();

            // Compute the angle formed by the two vectors of the basis
            const Scalar alpha =
                    (myBase_3D_[1]->pos() - myBase_3D_[0]->pos()).normalized().dot(
                            (myBase_3D_[3]->pos() - myBase_3D_[2]->pos()).normalized());

            // 1. Datastructure construction
            const Scalar eps = pcfunctor_.getNormalizedEpsilon(distance_threshold2);

            IndexedNormalSet3D nset (eps);

            for (size_t i = 0; i <  First_pairs.size(); ++i) {
                const Point& p1 = pcfunctor_.points[First_pairs[i].first];
                const Point& p2 = pcfunctor_.points[First_pairs[i].second];
                const Point  n  = (p2 - p1).normalized();

                nset.addElement((p1+ typename Point::Scalar(invariant1) * (p2 - p1)).eval(), n, i);
            }


            std::set< std::pair<unsigned int, unsigned int > > comb;

            unsigned int j = 0;
            std::vector<unsigned int> nei;
            // 2. Query time
            for (unsigned int i = 0; i < Second_pairs.size(); ++i) {
                const Point& p1 = pcfunctor_.points[Second_pairs[i].first];
                const Point& p2 = pcfunctor_.points[Second_pairs[i].second];

                const VectorType& pq1 = mySampled_Q_3D_[Second_pairs[i].first].pos();
                const VectorType& pq2 = mySampled_Q_3D_[Second_pairs[i].second].pos();

                nei.clear();

                const Point      query  =  p1 + invariant2 * ( p2 - p1 );
                const VectorType queryQ = pq1 + invariant2 * (pq2 - pq1);

                const Point queryn = (p2 - p1).normalized();

                nset.getNeighbors( query, queryn, alpha, nei);


                VectorType invPoint;
                //const Scalar distance_threshold2s = distance_threshold2 * distance_threshold2;
                for (unsigned int k = 0; k != nei.size(); k++){
                    const int id = nei[k];

                    const VectorType& pp1 = mySampled_Q_3D_[First_pairs[id].first].pos();
                    const VectorType& pp2 = mySampled_Q_3D_[First_pairs[id].second].pos();

                    invPoint = pp1 + (pp2 - pp1) * invariant1;

                    // use also distance_threshold2 for inv 1 and 2 in 4PCS
                    if ((queryQ-invPoint).squaredNorm() <= distance_threshold2){
                        comb.emplace(id, i);
                    }
                }
            }

            for (std::set< std::pair<unsigned int, unsigned int>>::const_iterator it = comb.cbegin();
                    it != comb.cend(); it++) {
                const unsigned int & id = (*it).first;
                const unsigned int & i  = (*it).second;

                quadrilaterals->push_back( {First_pairs[id].first, First_pairs[id].second,
                                             Second_pairs[i].first,  Second_pairs[i].second });
            }

            return quadrilaterals->size() != 0;
        }

    };
}

#endif //SUPER4PCS_FUNCTORSUPER4PCS_H
