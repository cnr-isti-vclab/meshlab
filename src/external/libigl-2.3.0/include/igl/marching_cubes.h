// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2020 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_MARCHING_CUBES_H
#define IGL_MARCHING_CUBES_H
#include "igl_inline.h"

#include <Eigen/Core>
namespace igl
{
  // marching_cubes( values, points, x_res, y_res, z_res, isovalue, vertices, faces )
  //
  // performs marching cubes reconstruction on a grid defined by values, and
  // points, and generates a mesh defined by vertices and faces
  //
  // Input:
  //   S   nx*ny*nz list of values at each grid corner
  //       i.e. S(x + y*xres + z*xres*yres) for corner (x,y,z)
  //   GV  nx*ny*nz by 3 array of corresponding grid corner vertex locations
  //   nx  resolutions of the grid in x dimension
  //   ny  resolutions of the grid in y dimension
  //   nz  resolutions of the grid in z dimension
  //   isovalue  the isovalue of the surface to reconstruct
  // Output:
  //   V  #V by 3 list of mesh vertex positions
  //   F  #F by 3 list of mesh triangle indices into rows of V
  //
  template <
    typename DerivedS, 
    typename DerivedGV, 
    typename DerivedV, 
    typename DerivedF>
  IGL_INLINE void marching_cubes(
    const Eigen::MatrixBase<DerivedS> & S,
    const Eigen::MatrixBase<DerivedGV> & GV,
    const unsigned nx,
    const unsigned ny,
    const unsigned nz,
    const typename DerivedS::Scalar isovalue,
    Eigen::PlainObjectBase<DerivedV> &V,
    Eigen::PlainObjectBase<DerivedF> &F);
  template <
    typename DerivedS, 
    typename DerivedGV, 
    typename DerivedGI, 
    typename DerivedV, 
    typename DerivedF>
  IGL_INLINE void marching_cubes(
    const Eigen::MatrixBase<DerivedS> & S,
    const Eigen::MatrixBase<DerivedGV> & GV,
    const Eigen::MatrixBase<DerivedGI> & GI,
    const typename DerivedS::Scalar isovalue,
    Eigen::PlainObjectBase<DerivedV> &V,
    Eigen::PlainObjectBase<DerivedF> &F);
}

#ifndef IGL_STATIC_LIBRARY
#  include "marching_cubes.cpp"
#endif

#endif
