// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_EDGES_H
#define IGL_EDGES_H
#include "igl_inline.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace igl
{
  // Constructs a list of unique edges represented in a given mesh (V,F)
  //
  // Inputs:
  //   F  #F by 3 list of mesh faces (must be triangles)
  //   or
  //   T  #T x 4  matrix of indices of tet corners
  // Outputs:
  //   E #E by 2 list of edges in no particular order
  //
  // See also: adjacency_matrix
  template <typename DerivedF, typename DerivedE>
  IGL_INLINE void edges(
    const Eigen::MatrixBase<DerivedF> & F, 
    Eigen::PlainObjectBase<DerivedE> & E);
  // Constructs a list of unique edges represented in a given polygon mesh.
  //
  // Inputs:
  //   I  #I vectorized list of polygon corner indices into rows of some matrix V
  //   C  #polygons+1 list of cumulative polygon sizes so that C(i+1)-C(i) =
  //     size of the ith polygon, and so I(C(i)) through I(C(i+1)-1) are the
  //     indices of the ith polygon
  // Outputs:
  //   E #E by 2 list of edges in no particular order
  template <typename DerivedI, typename DerivedC, typename DerivedE>
  IGL_INLINE void edges(
    const Eigen::MatrixBase<DerivedI> & I,
    const Eigen::MatrixBase<DerivedC> & C,
    Eigen::PlainObjectBase<DerivedE> & E);
  // Inputs:
  //   A  #V by #V symmetric adjacency matrix
  // Outputs:
  //   E  #E by 2 list of edges in no particular order
  template <typename T, typename DerivedE>
  IGL_INLINE void edges(
    const Eigen::SparseMatrix<T> & A,
    Eigen::PlainObjectBase<DerivedE> & E);
}

#ifndef IGL_STATIC_LIBRARY
#  include "edges.cpp"
#endif

#endif
