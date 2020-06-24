/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
 *  Copyright (c) 2012-, Open Perception, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 *
 */

#ifndef PCL_REGISTRATION_OPENGR_H_
#define PCL_REGISTRATION_OPENGR_H_

#include <pcl/registration/registration.h>
#include <pcl/registration/transformation_estimation_svd.h>

#include <gr/shared.h>
#include <gr/sampling.h>
#include <gr/utils/logger.h>
#include <gr/algorithms/PointPairFilter.h>
#include <gr/algorithms/match4pcsBase.h>
#include <gr/algorithms/FunctorSuper4pcs.h>

namespace pcl
{
  /** \brief Pose estimation and alignment class using Super4CS routine.
   *
   * This class is a wrapper to use the OpenGR library in PCL.
   * For more info, see Super4PCS library repository: https://github.com/STORM-IRIT/OpenGR
   *
   * If you use this in academic work, please cite:
   *
   * ```
   *    N. Mellado, D. Aiger, N. J. Mitra
   *    SUPER 4PCS: Fast Global Pointcloud Registration via Smart Indexing.
   *    Computer Graphics Forum, Proceedings of SGP 2014, 2014.
   * ```
   *
   * and
   * ```
   *  @MISC{openGR,
   *    author = {Nicolas Mellado and others},
   *    title = {OpenGR: A C++ library for 3D Global Registration},
   *    howpublished = {https://storm-irit.github.io/OpenGR/},
   *    year = {2017}
   *   }
   * ```
   *
   * \author Nicolas Mellado (nmellado0@gmail.com)
   * \ingroup registration
   */
  template <typename PointSource, typename PointTarget>
  class Super4PCS : public Registration<PointSource, PointTarget>
  {
    public:
      typedef typename Registration<PointSource, PointTarget>::Matrix4 Matrix4;

      using Registration<PointSource, PointTarget>::reg_name_;
      using Registration<PointSource, PointTarget>::getClassName;
      using Registration<PointSource, PointTarget>::input_;
      using Registration<PointSource, PointTarget>::target_;
      using Registration<PointSource, PointTarget>::transformation_estimation_;
      using Registration<PointSource, PointTarget>::final_transformation_;
      using Registration<PointSource, PointTarget>::converged_;

      typedef typename Registration<PointSource, PointTarget>::PointCloudSource PointCloudSource;
      typedef typename PointCloudSource::Ptr PointCloudSourcePtr;
      typedef typename PointCloudSource::ConstPtr PointCloudSourceConstPtr;

      typedef typename Registration<PointSource, PointTarget>::PointCloudTarget PointCloudTarget;

      typedef PointIndices::Ptr PointIndicesPtr;
      typedef PointIndices::ConstPtr PointIndicesConstPtr;


      struct TransformVisitor {
          template <typename Derived>
          inline void operator()(
                  float fraction,
                  float best_LCP,
                  const Eigen::MatrixBase<Derived>& /*transformation*/) const {
            if(fraction >= 0)
              {
                printf("done: %d%c best: %f                  \r",
                       static_cast<int>(fraction * 100), '%', best_LCP);
                fflush(stdout);
              }
          }
          constexpr bool needsGlobalTransformation() const { return false; }
      };

      using PointType     = gr::Point3D<float>;
      using SamplerType   = gr::UniformDistSampler<PointType>;
      using MatcherType   = gr::Match4pcsBase<gr::FunctorSuper4PCS, PointType, TransformVisitor, gr::AdaptivePointFilter, gr::AdaptivePointFilter::Options>;
      using OptionType    = typename MatcherType::OptionsType;

      OptionType options_;

      /** \brief Constructor */
      Super4PCS ()
      {
        reg_name_ = "Super4PCS";
        transformation_estimation_.reset (new pcl::registration::TransformationEstimationSVD<PointSource, PointTarget>);
      }

      /** \brief Destructor */
      virtual ~Super4PCS ()
      {
      }

      /** \brief Get the fitness score of alignment. Range from 0-1, higher is better.
       */
      inline float getFitnessScore() const { return fitness_score_; }

    protected:

      float fitness_score_;

      /** \brief Rigid transformation computation method.
        * \param output the transformed input point cloud dataset using the rigid transformation found
        * \param guess The computed transformation
        */
      void
      computeTransformation (PointCloudSource &output, const Eigen::Matrix4f& guess);

  };
}

#endif

#include <pcl/registration/impl/super4pcs.hpp>

