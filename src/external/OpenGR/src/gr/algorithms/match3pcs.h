//
// Created by Sandra Alfaro on 30/05/18.
//

#ifndef OPENGR_MATCH3PCSBASE_H
#define OPENGR_MATCH3PCSBASE_H

#include <vector>
#include "gr/shared.h"
#include "gr/sampling.h"
#include "gr/algorithms/congruentSetExplorationBase.h"
#include "matchBase.h"

#ifdef TEST_GLOBAL_TIMINGS
#   include "gr/utils/timer.h"
#endif

namespace gr {
    template <typename PointType>
    struct Traits3pcs {
        static constexpr int size() { return 3; }
        using Base = std::array<int,3>; 
        using Set = std::vector<Base>;
        using Coordinates = std::array<const PointType*, 3>;
    };

    /// Class for the computation of the 3PCS algorithm.
    template <typename _PointType,
              typename _TransformVisitor,
              typename _PairFilteringFunctor,  /// <\brief Must implements PairFilterConcept
              template < class, class > typename PairFilteringOptions >
    class Match3pcs : public CongruentSetExplorationBase<Traits3pcs<typename MatchBase<_PointType, _TransformVisitor, PairFilteringOptions, CongruentSetExplorationOptions>::PosMutablePoint>, _PointType, _TransformVisitor, _PairFilteringFunctor, PairFilteringOptions> {
    public:
      using PosMutablePoint      = typename MatchBase<_PointType, _TransformVisitor, PairFilteringOptions, CongruentSetExplorationOptions>::PosMutablePoint;
      using Traits               = Traits3pcs<PosMutablePoint>;
      using PairFilteringFunctor = _PairFilteringFunctor;
      using TransformVisitor     = _TransformVisitor;

      using CongruentBaseType    = typename Traits::Base;
      using Set                  = typename Traits::Set;
      using Coordinates          = typename Traits::Coordinates;

      using MatchBaseType = CongruentSetExplorationBase<Traits3pcs<PosMutablePoint>, _PointType, _TransformVisitor, _PairFilteringFunctor, PairFilteringOptions>;

      using OptionsType = typename MatchBaseType::OptionsType;
      using Scalar      = typename MatchBaseType::Scalar;

        Match3pcs (const OptionsType& options
                , const Utils::Logger& logger);

        virtual ~Match3pcs();

        /// Find all the congruent set similar to the base in the second 3D model (Q).
        /// It could be with a 3 point base or a 4 point base.
        /// \param base use to find the similar points congruent in Q.
        /// \param congruent_set a set of all point congruent found in Q.
        bool generateCongruents (CongruentBaseType& base, Set& congruent_quads) override;

        /// Tries to compute an inital base from P
        /// @param [out] base The base, if found. Initial value is not used. Modified as 
        /// the computed base if the return value is true.
        /// @return true if a base is found an initialized, false otherwise
        bool initBase(CongruentBaseType &base) override;
    };
}

#include "match3pcs.hpp"
#endif //OPENGR_MATCH3PCSBASE_H
