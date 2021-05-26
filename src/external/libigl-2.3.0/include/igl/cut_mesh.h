// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2019 Hanxiao Shen <hanxiao@cims.nyu.edu>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_CUT_MESH_H
#define IGL_CUT_MESH_H
#include "igl_inline.h"

#include <Eigen/Core>

namespace igl
{
  // Given a mesh and a list of edges that are to be cut, the function
  // generates a new disk-topology mesh that has the cuts at its boundary.
  //
  //
  // Known issues: Assumes mesh is edge-manifold.
  //
  // Inputs:
  //   V  #V by 3 list of the vertex positions
  //   F  #F by 3 list of the faces
  //   cuts  #F by 3 list of boolean flags, indicating the edges that need to
  //     be cut (has 1 at the face edges that are to be cut, 0 otherwise)
  // Outputs:
  //   Vn  #V by 3 list of the vertex positions of the cut mesh. This matrix
  //     will be similar to the original vertices except some rows will be
  //     duplicated.
  //   Fn  #F by 3 list of the faces of the cut mesh(must be triangles). This
  //     matrix will be similar to the original face matrix except some indices
  //     will be redirected to point to the newly duplicated vertices.
  //   I   #V by 1 list of the map between Vn to original V index.

  // In place mesh cut
  template <typename DerivedV, typename DerivedF, typename DerivedC, typename DerivedI>
  IGL_INLINE void cut_mesh(
    Eigen::PlainObjectBase<DerivedV>& V,
    Eigen::PlainObjectBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedC>& cuts,
    Eigen::PlainObjectBase<DerivedI>& I
  );
  
  template <typename DerivedV, typename DerivedF, typename DerivedFF, typename DerivedFFi, typename DerivedC, typename DerivedI>
  IGL_INLINE void cut_mesh(
    Eigen::PlainObjectBase<DerivedV>& V,
    Eigen::PlainObjectBase<DerivedF>& F,
    Eigen::MatrixBase<DerivedFF>& FF,
    Eigen::MatrixBase<DerivedFFi>& FFi,
    const Eigen::MatrixBase<DerivedC>& C,
    Eigen::PlainObjectBase<DerivedI>& I
  );

  template <typename DerivedV, typename DerivedF, typename DerivedC>
  IGL_INLINE void cut_mesh(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedC>& cuts,
    Eigen::PlainObjectBase<DerivedV>& Vn,
    Eigen::PlainObjectBase<DerivedF>& Fn
  );

  template <typename DerivedV, typename DerivedF, typename DerivedC, typename DerivedI>
  IGL_INLINE void cut_mesh(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedC>& cuts,
    Eigen::PlainObjectBase<DerivedV>& Vn,
    Eigen::PlainObjectBase<DerivedF>& Fn,
    Eigen::PlainObjectBase<DerivedI>& I
  );


  
}


#ifndef IGL_STATIC_LIBRARY
#include "cut_mesh.cpp"
#endif


#endif
