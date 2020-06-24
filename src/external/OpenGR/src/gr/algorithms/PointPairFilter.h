//
// Created by Sandra Alfaro on 26/04/18.
//

#ifndef OPENGR_FUNCTORFEATUREPOINTTEST_H
#define OPENGR_FUNCTORFEATUREPOINTTEST_H

#include <gr/shared.h>
#include <vector>

namespace gr {

#ifdef PARSED_BY_DOXYGEN
struct PairFilterConcept {

    template < class Derived, class TBase>
    struct Options : public TBase { };
    
    /// Verify that the 2 points found in Q are similar to 2 of the points in the base.
    /// Return a pair of bool, according of the right addition of the pair (p,q) or (q,p) in the congruent set.
    template <typename PointType, typename WantedOptionsAndMore>
    inline std::pair<bool,bool> operator() (const PointType& /*p*/,
                                            const PointType& /*q*/,
                                            typename PointType::Scalar /*pair_normals_angle*/,
                                            const PointType& /*b0*/,
                                            const PointType& /*b1*/,
                                            const WantedOptionsAndMore& /*options*/) { }
};
#endif

    /// \brief Functor used in n-pcs algorithm to filters pairs of points according
    ///        to the exploration basis,
    /// \tparam
    /// \implements PairFilterConcept
    ///
    struct DummyPointFilter {
    template < class Derived, class TBase>
    struct Options : public TBase {
      bool dummyFilteringResponse;
      enum { IS_DUMMYPOINTFILTER_OPTIONS = true };
    };
    template <typename PointType, typename WantedOptionsAndMore>
    inline std::pair<bool,bool> operator() (const PointType& /*p*/,
                                            const PointType& /*q*/,
                                            typename PointType::Scalar /*pair_normals_angle*/,
                                            const PointType& /*b0*/,
                                            const PointType& /*b1*/,
                                            const WantedOptionsAndMore &options) {
        return std::make_pair(options.dummyFilteringResponse, options.dummyFilteringResponse);
    }
    };

    /// \brief Functor used in n-pcs algorithm to filters pairs of points according
    ///        to the exploration basis. Uses normal, colors and max motion when
    ///        available
    ///
    /// \implements PairFilterConcept
    ///
    struct AdaptivePointFilter {
      template < class Derived, class TBase>
      struct Options : public TBase {
        using Scalar = typename TBase::Scalar;

        /// Maximum normal difference.
        Scalar max_normal_difference = -1;
        /// Maximum color RGB distance between corresponding vertices. Set negative to ignore
        Scalar max_color_distance = -1;

        enum { IS_ADAPTIVEPOINTFILTER_OPTIONS = true };
      };

        /// Verify that the 2 points found in Q are similar to 2 of the points in the base.
        /// A filter by point feature : normal, distance, translation distance, angle and color.
        /// Return a pair of bool, according of the right addition of the pair (p,q) or (q,p) in the congruent set.
        template <typename PointType, typename WantedOptionsAndMore>
        inline std::pair<bool,bool> operator() (const PointType& p,
                                                const PointType& q,
                                                typename PointType::Scalar pair_normals_angle,
                                                const PointType& b0,
                                                const PointType& b1,
                                                const WantedOptionsAndMore &options) {
            static_assert( WantedOptionsAndMore::IS_ADAPTIVEPOINTFILTER_OPTIONS,
                           "Options passed to AdaptivePointFilter must inherit AdaptivePointFilter::Options" );
            using Scalar      = typename PointType::Scalar;
            using PairsVector = std::vector< std::pair<int, int> >;
            using VectorType  = typename PointType::VectorType;


            std::pair<bool,bool> res;
            res.first = false;
            res.second = false;

            VectorType segment1 = (b1.pos() - b0.pos()).normalized();

            if ( options.max_normal_difference > 0 &&
                 q.normal().squaredNorm() > 0 &&
                 p.normal().squaredNorm() > 0) {
                const Scalar norm_threshold =
                        0.5 * options.max_normal_difference * M_PI / 180.0;
                const double first_normal_angle = (q.normal() - p.normal()).norm();
                const double second_normal_angle = (q.normal() + p.normal()).norm();
                // Take the smaller normal distance.
                const Scalar first_norm_distance =
                        std::min(std::abs(first_normal_angle - pair_normals_angle),
                                 std::abs(second_normal_angle - pair_normals_angle));
                // Verify appropriate angle between normals and distance.

                if (first_norm_distance > norm_threshold) return res;
            }
            // Verify restriction on the rotation angle, translation and colors.
            if (options.max_color_distance > 0) {
                const bool use_rgb = (p.rgb()[0] >= 0 && q.rgb()[0] >= 0 &&
                                      b0.rgb()[0] >= 0 &&
                                      b1.rgb()[0] >= 0);
                bool color_good = (p.rgb() - b0.rgb()).norm() <
                                  options.max_color_distance &&
                                  (q.rgb() - b1.rgb()).norm() <
                                  options.max_color_distance;

                if (use_rgb && ! color_good) return res;
            }

            if (options.max_translation_distance > 0) {
                const bool dist_good = (p.pos() - b0.pos()).norm() <
                                       options.max_translation_distance &&
                                       (q.pos() - b1.pos()).norm() <
                                       options.max_translation_distance;
                if (! dist_good) return res;
            }

            // need cleaning here
            if (options.max_angle > 0){
                VectorType segment2 = (q.pos() - p.pos()).normalized();
                if (std::acos(segment1.dot(segment2)) <= options.max_angle * M_PI / 180.0) {
                    res.second = true;
                }

                if (std::acos(segment1.dot(- segment2)) <= options.max_angle * M_PI / 180.0) {
                    // Add ordered pair.
                    res.first = true;
                }
            }else {
                res.first = true;
                res.second = true;
            }
            return res;
        }
    };
}

#endif //OPENGR_FUNCTORFEATUREPOINTTEST_H
