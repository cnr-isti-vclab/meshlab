// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2018 Zhongshi Jiang <jiangzs@nyu.edu>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_MAPPING_ENERGY_WITH_JACOBIANS_H
#define IGL_MAPPING_ENERGY_WITH_JACOBIANS_H

#include "igl_inline.h"
#include <Eigen/Dense>
#include "MappingEnergyType.h"

namespace igl
{
   // compute the rotation-invariant energy of a mapping (represented in Jacobians and areas)
   // Input:
   // Ji: #F by 4 (9 if 3D) entries of jacobians
   // areas: #F by 1 face areas
   // slim_energy: energy type as in igl::MappingEnergyType
   // exp_factor: see igl::MappingEnergyType
   //
   // Output:
   // energy value
   IGL_INLINE double mapping_energy_with_jacobians(const Eigen::MatrixXd &Ji, 
                                                  const Eigen::VectorXd &areas, 
                                                  igl::MappingEnergyType slim_energy, 
                                                  double exp_factor);
  
}
#ifndef IGL_STATIC_LIBRARY
#  include "mapping_energy_with_jacobians.cpp"
#endif

#endif