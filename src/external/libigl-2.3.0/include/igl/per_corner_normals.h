// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_PER_CORNER_NORMALS_H
#define IGL_PER_CORNER_NORMALS_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <vector>

namespace igl
{
  // Compute vertex normals via vertex position list, face list
  // Inputs:
  //   V  #V by 3 eigen Matrix of mesh vertex 3D positions
  //   F  #F by 3 eigen Matrix of face (triangle) indices
  //   corner_threshold  threshold in degrees on sharp angles
  // Output:
  //   CN  #F*3 by 3 eigen Matrix of mesh vertex 3D normals, where the normal
  //     for corner F(i,j) is at CN(i*3+j,:) 
  template <typename DerivedV, typename DerivedF, typename DerivedCN>
  IGL_INLINE void per_corner_normals(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const double corner_threshold,
    Eigen::PlainObjectBase<DerivedCN> & CN);
  // Other Inputs:
  //   FN  #F by 3 eigen Matrix of face normals
  template <
    typename DerivedV, 
    typename DerivedF, 
    typename DerivedFN, 
    typename DerivedCN>
  IGL_INLINE void per_corner_normals(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedFN>& FN,
    const double corner_threshold,
    Eigen::PlainObjectBase<DerivedCN> & CN);
  // Other Inputs:
  //   VF  map from vertices to list of incident faces
  template <
    typename DerivedV, 
    typename DerivedF, 
    typename DerivedFN, 
    typename IndexType,
    typename DerivedCN>
  IGL_INLINE void per_corner_normals(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedFN>& FN,
    const std::vector<std::vector<IndexType> >& VF,
    const double corner_threshold,
    Eigen::PlainObjectBase<DerivedCN> & CN);
  // Inputs:
  //   V  #V by 3 list of mesh vertex positions
  //   I  #I vectorized list of polygon corner indices into rows of some matrix V
  //   C  #polygons+1 list of cumulative polygon sizes so that C(i+1)-C(i) = size of
  //     the ith polygon, and so I(C(i)) through I(C(i+1)-1) are the indices of
  //     the ith polygon
  //   corner_threshold  threshold in degrees on sharp angles
  // Outputs:
  //   N  #I by 3 list of per corner normals
  //   VV  #I+#polygons by 3 list of auxiliary triangle mesh vertex positions
  //   FF  #I by 3 list of triangle indices into rows of VV
  //   J  #I list of indices into original polygons
  //   NN  #FF by 3 list of normals for each auxiliary triangle
  template <
    typename DerivedV, 
    typename DerivedI, 
    typename DerivedC, 
    typename DerivedN,
    typename DerivedVV,
    typename DerivedFF,
    typename DerivedJ,
    typename DerivedNN>
  IGL_INLINE void per_corner_normals(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedI> & I,
    const Eigen::MatrixBase<DerivedC> & C,
    const typename DerivedV::Scalar corner_threshold,
    Eigen::PlainObjectBase<DerivedN>  & N,
    Eigen::PlainObjectBase<DerivedVV> & VV,
    Eigen::PlainObjectBase<DerivedFF> & FF,
    Eigen::PlainObjectBase<DerivedJ>  & J,
    Eigen::PlainObjectBase<DerivedNN> & NN);
}

#ifndef IGL_STATIC_LIBRARY
#  include "per_corner_normals.cpp"
#endif

#endif
