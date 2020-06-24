//
// Created by Sandra Alfaro on 30/05/18.
//

#include <vector>
#include <atomic>
#include <chrono>
#include "gr/shared.h"
#include "gr/sampling.h"
#include "gr/utils/logger.h"
#include "match3pcs.h"

namespace gr {

    template <typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    Match3pcs<PointType, TransformVisitor, PairFilteringFunctor, PFO>::
    Match3pcs(const Match3pcs<PointType, TransformVisitor, PairFilteringFunctor, PFO>::OptionsType &options,
                         const gr::Utils::Logger &logger)
        : MatchBaseType(options,logger)
    {
    }

    template <typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    Match3pcs<PointType, TransformVisitor, PairFilteringFunctor, PFO>::~Match3pcs() {}

    template <typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match3pcs<PointType, TransformVisitor, PairFilteringFunctor, PFO>::generateCongruents (CongruentBaseType &base, Set& congruent_set) {

        //Find base in P (random triangle)
        if(!initBase(base)) return false;

        // Computes distance between points.
        const Scalar d1 = (MatchBaseType::base_3D_[0]->pos()- MatchBaseType::base_3D_[1]->pos()).norm();
        const Scalar d2 = (MatchBaseType::base_3D_[0]->pos()- MatchBaseType::base_3D_[2]->pos()).norm();
        const Scalar d3 = (MatchBaseType::base_3D_[1]->pos()- MatchBaseType::base_3D_[2]->pos()).norm();

       /*
        // Compute normal angles.
        const Scalar normal_angle_AB;
        const Scalar normal_angle_AC;
        const Scalar normal_angle_BC;
       */

        PairFilteringFunctor fun;

        // Find all 3pcs in Q
        for (int i=0; i<MatchBaseType::sampled_Q_3D_.size(); ++i) {
            const PosMutablePoint& a = MatchBaseType::sampled_Q_3D_[i];
            for (int j=i+1; j<MatchBaseType::sampled_Q_3D_.size(); ++j) {
                const PosMutablePoint& b = MatchBaseType::sampled_Q_3D_[j];
                const Scalar dAB = (b.pos() - a.pos()).norm();
                if (std::abs(dAB - d1) > MatchBaseType::distance_factor * MatchBaseType::options_.delta) continue;
                for (int k=j+1; k<MatchBaseType::sampled_Q_3D_.size(); ++k) {
                    const PosMutablePoint& c = MatchBaseType::sampled_Q_3D_[k];
                    const Scalar dAC = (c.pos() - a.pos()).norm();
                    const Scalar dBC = (c.pos() - b.pos()).norm();
                    if (std::abs(dAC - d2) > MatchBaseType::distance_factor * MatchBaseType::options_.delta) continue;
                    if (std::abs(dBC - d3) > MatchBaseType::distance_factor * MatchBaseType::options_.delta) continue;

                    congruent_set.push_back({i,j,k});
                }
            }
        }

        //TODO add filter points
        /* //Change the normal_angle
         // Compute normal angles.
        const Scalar normal_angle1 = (base_3D_[0]->normal() - base_3D_[1]->normal()).norm();
        const Scalar normal_angle2 = (base_3D_[2]->normal() - base_3D_[3]->normal()).norm();

          PointFilterFunctor fun(myOptions_, myBase_3D_);
                    std::pair<bool,bool> res = fun(p,q, pair_normals_angle, base_point1,base_point2);
                    if (res.first)
                        pairs->emplace_back(i, j);
                    if (res.second)
                        pairs->emplace_back(j, i);
        */
        return congruent_set.size()!=0;
    }


    template <typename PointType,
              typename TransformVisitor,
              typename PairFilteringFunctor,
              template < class, class > typename PFO>
    bool Match3pcs<PointType, TransformVisitor, PairFilteringFunctor, PFO>::initBase (CongruentBaseType &base)
    {
        if (!MatchBaseType::SelectRandomTriangle(base[0], base[1], base[2]))
            return false;

        MatchBaseType::base_3D_ [0] = &MatchBaseType::sampled_P_3D_[base[0]];
        MatchBaseType::base_3D_ [1] = &MatchBaseType::sampled_P_3D_[base[1]];
        MatchBaseType::base_3D_ [2] = &MatchBaseType::sampled_P_3D_[base[2]];

        return true;
    }
}
