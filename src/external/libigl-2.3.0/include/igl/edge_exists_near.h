// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2018 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_EDGE_EXISTS_NEAR_H
#define IGL_EDGE_EXISTS_NEAR_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <vector>
namespace igl
{
  // Does edge (a,b) exist in the edges of all faces incident on
  // existing unique edge uei.
  //
  // Inputs:
  //   uE  #uE by 2 list of unique undirected edges
  //   EMAP #F*3 list of indices into uE, mapping each directed edge to unique
  //     undirected edge
  //   uE2E  #uE list of lists of indices into E of coexisting edges
  //   E  #F*3 by 2 list of half-edges
  //   a  1st end-point of query edge
  //   b  2nd end-point of query edge
  //   uei  index into uE/uE2E of unique edge
  // Returns true if edge exists near uei.
  //
  // See also: unique_edge_map
  template <
    typename DeriveduE,
    typename DerivedEMAP,
    typename uE2EType,
    typename Index>
  IGL_INLINE bool edge_exists_near(
    const Eigen::MatrixBase<DeriveduE> & uE,
    const Eigen::MatrixBase<DerivedEMAP> & EMAP,
    const std::vector<std::vector< uE2EType> > & uE2E,
    const Index & a,
    const Index & b,
    const Index & uei);
}
#ifndef IGL_STATIC_LIBRARY
#  include "edge_exists_near.cpp"
#endif
#endif

