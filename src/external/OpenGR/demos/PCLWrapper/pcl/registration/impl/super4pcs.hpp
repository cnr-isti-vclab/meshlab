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

#ifndef PCL_REGISTRATION_OPENGR_HPP_
#define PCL_REGISTRATION_OPENGR_HPP_

#include <pcl/io/ply_io.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace pcl {
template <typename PointSource, typename PointTarget> void
pcl::Super4PCS<PointSource, PointTarget>::computeTransformation (PointCloudSource &output, const Eigen::Matrix4f& guess)
{
  using namespace gr;

  // Initialize results
  final_transformation_ = guess;

  constexpr ::gr::Utils::LogLevel loglvl = ::gr::Utils::Verbose;

  gr::Utils::Logger logger(loglvl);
  MatcherType matcher(options_, logger);

  SamplerType sampler;
  TransformVisitor visitor;

  std::vector<gr::Point3D<float> > set1, set2;

  // init Super4PCS point cloud internal structure
  auto fillPointSet = [] (const PointCloudSource& m, std::vector<gr::Point3D<float> >& out) {
      out.clear();
      out.reserve(m.size());

      // TODO: copy other point-wise information, if any
      for(size_t i = 0; i< m.size(); i++){
          const auto& point = m[i];
          out.emplace_back(point.x, point.y, point.z);
      }
  };
  fillPointSet(*target_, set1);
  fillPointSet(*input_, set2);;

  float score = matcher.ComputeTransformation(set1, set2, final_transformation_, sampler, visitor);

  transformPointCloud (*input_, output, final_transformation_);

  pcl::console::print_highlight ("Final score: %f\n", fitness_score_);

  converged_ = true;
}
}

#endif

