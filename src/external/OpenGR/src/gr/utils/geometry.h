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
// Authors: Nicolas Mellado
//
// An implementation of the Super 4-points Congruent Sets (Super 4PCS)
// algorithm presented in:
//
// Super 4PCS: Fast Global Pointcloud Registration via Smart Indexing
// Nicolas Mellado, Dror Aiger, Niloy J. Mitra
// Symposium on Geometry Processing 2014.
//
// Data acquisition in large-scale scenes regularly involves accumulating
// information across multiple scans. A common approach is to locally align scan
// pairs using Iterative Closest Point (ICP) algorithm (or its variants), but
// requires static scenes and small motion between scan pairs. This prevents
// accumulating data across multiple scan sessions and/or different acquisition
// modalities (e.g., stereo, depth scans). Alternatively, one can use a global
// registration algorithm allowing scans to be in arbitrary initial poses. The
// state-of-the-art global registration algorithm, 4PCS, however has a quadratic
// time complexity in the number of data points. This vastly limits its
// applicability to acquisition of large environments. We present Super 4PCS for
// global pointcloud registration that is optimal, i.e., runs in linear time (in
// the number of data points) and is also output sensitive in the complexity of
// the alignment problem based on the (unknown) overlap across scan pairs.
// Technically, we map the algorithm as an 'instance problem' and solve it
// efficiently using a smart indexing data organization. The algorithm is
// simple, memory-efficient, and fast. We demonstrate that Super 4PCS results in
// significant speedup over alternative approaches and allows unstructured
// efficient acquisition of scenes at scales previously not possible. Complete
// source code and datasets are available for research use at
// http://geometry.cs.ucl.ac.uk/projects/2014/super4PCS/.

#ifndef _OPENGR_UTILS_GEOMETRY_H_
#define _OPENGR_UTILS_GEOMETRY_H_

#include "gr/utils/disablewarnings.h"
#include "Eigen/Core"

namespace gr{
namespace Utils{


template <typename PointContainer, typename VecContainer>
static inline void CleanInvalidNormals( PointContainer &v,
                                        VecContainer &normals){
  using Point = typename PointContainer::value_type;
  using Vector = typename VecContainer::value_type;
  if (v.size() == normals.size()){
    typename PointContainer::iterator itV = v.begin();
    typename VecContainer::iterator itN = normals.begin();

    unsigned int nb = 0;
    for( ; itV != v.end(); itV++, itN++){

      if ((*itV).normal().squaredNorm() < 0.01){
        (*itN) = {0., 0., 0.};
        (*itV).set_normal({0., 0., 0.});
        nb++;
      }else{
        (*itN).normalize();
        (*itV).normalize();
      }
    }

    if (nb != 0){
      std::cout << "Found " << nb << " vertices with invalid normals" << std::endl;
    }
  }
}

template <typename PointContainer>
static inline void TransformPointCloud( PointContainer& v,
                                        Eigen::Ref<Eigen::Matrix<typename PointContainer::value_type::Scalar, 4, 4>> tr){
  using Scalar = typename PointContainer::value_type::Scalar;
  auto tr3x3 = tr.template block<3,3>(0,0);
  for (auto& vertex : v){
      vertex.pos() = (tr * vertex.pos().homogeneous()).template head<3>();
      vertex.set_normal(tr3x3 * vertex.normal());
    }
}

} // namespace Utils
} // namespace Super4PCS

#endif // _UTILS_GEOMETRY_H_
