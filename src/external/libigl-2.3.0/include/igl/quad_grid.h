// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2019 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_QUAD_GRID_H
#define IGL_QUAD_GRID_H

#include <igl/igl_inline.h>
#include <Eigen/Core>

namespace igl
{
  // Generate a quad mesh over a regular grid.
  //
  // Inputs:
  //   nx  number of vertices in the x direction
  //   ny  number of vertices in the y direction
  // Outputs:
  //   V  nx*ny by 2 list of vertex positions
  //   Q  (nx-1)*(ny-1) by 4 list of quad indices into V
  //   E  (nx-1)*ny+(ny-1)*nx by 2 list of undirected quad edge indices into V
  //
  //   See also: grid, triangulated_grid
  template<
    typename DerivedV,
    typename DerivedQ,
    typename DerivedE>
  IGL_INLINE void quad_grid(
    const int nx,
    const int ny,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedQ> & Q,
    Eigen::PlainObjectBase<DerivedE> & E);
  template<
    typename DerivedQ,
    typename DerivedE>
  IGL_INLINE void quad_grid(
    const int nx,
    const int ny,
    Eigen::PlainObjectBase<DerivedQ> & Q,
    Eigen::PlainObjectBase<DerivedE> & E);
}

#ifndef IGL_STATIC_LIBRARY
#  include "quad_grid.cpp"
#endif
#endif
