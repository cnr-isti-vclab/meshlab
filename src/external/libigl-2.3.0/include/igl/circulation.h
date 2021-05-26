// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_CIRCULATION_H
#define IGL_CIRCULATION_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <vector>

namespace igl
{
  // Return list of faces around the end point of an edge. Assumes
  // data-structures are built from an edge-manifold **closed** mesh.
  //
  // Inputs:
  //   e  index into E of edge to circulate
  //   ccw  whether to _continue_ in ccw direction of edge (circulate around
  //     E(e,1))
  //   EMAP #F*3 list of indices into E, mapping each directed edge to unique
  //     unique edge in E
  //   EF  #E by 2 list of edge flaps, EF(e,0)=f means e=(i-->j) is the edge of
  //     F(f,:) opposite the vth corner, where EI(e,0)=v. Similarly EF(e,1) "
  //     e=(j->i)
  //   EI  #E by 2 list of edge flap corners (see above).
  // Returns list of faces touched by circulation (in cyclically order).
  //   
  // See also: edge_flaps
  IGL_INLINE std::vector<int> circulation(
    const int e,
    const bool ccw,
    const Eigen::VectorXi & EMAP,
    const Eigen::MatrixXi & EF,
    const Eigen::MatrixXi & EI);
  // Wrapper with VectorXi output.
  IGL_INLINE void circulation(
    const int e,
    const bool ccw,
    const Eigen::VectorXi & EMAP,
    const Eigen::MatrixXi & EF,
    const Eigen::MatrixXi & EI,
    Eigen::VectorXi & vN);
  // Outputs:
  ////   Ne  2*#Nf list of indices into E of "next" rim-spoke-rim-spoke-...
  //   Nv  #Nv list of "next" vertex indices
  //   Nf  #Nf list of face indices
  IGL_INLINE void circulation(
    const int e,
    const bool ccw,
    const Eigen::MatrixXi & F,
    const Eigen::VectorXi & EMAP,
    const Eigen::MatrixXi & EF,
    const Eigen::MatrixXi & EI,
    /*std::vector<int> & Ne,*/
    std::vector<int> & Nv,
    std::vector<int> & Nf);
}

#ifndef IGL_STATIC_LIBRARY
#  include "circulation.cpp"
#endif
#endif
