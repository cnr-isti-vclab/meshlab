// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2020 Oded Stein <oded.stein@columbia.edu>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_CR_VECTOR_LAPLACIAN_H
#define IGL_CR_VECTOR_LAPLACIAN_H

#include "igl_inline.h"

#include <Eigen/Core>
#include <Eigen/Sparse>


namespace igl
{
  // Computes the CR vector Laplacian matrix.
  // See Oded Stein, Max Wardetzky, Alec Jacobson, Eitan Grinspun, 2020.
  //  "A Simple Discretization of the Vector Dirichlet Energy"
  //
  // Inputs:
  //  V, F: input mesh
  //  E: a mapping from each halfedge to each edge, as computed with
  //     orient_halfedges.
  //     will be computed if not provided.
  //  oE: the orientation of each halfedge compared to the orientation of the
  //      actual edge, as computed with orient_halfedges.
  //      will be computed if not provided.
  //
  // Outputs:
  //  L: computed Laplacian matrix
  //  E, oE: these are computed if they are not present, as described above

  template <typename DerivedV, typename DerivedF, typename DerivedE,
  typename DerivedOE, typename ScalarL>
  IGL_INLINE void
  cr_vector_laplacian(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedE>& E,
    const Eigen::MatrixBase<DerivedOE>& oE,
    Eigen::SparseMatrix<ScalarL>& L);

  template <typename DerivedV, typename DerivedF, typename DerivedE,
  typename DerivedOE, typename ScalarL>
  IGL_INLINE void
  cr_vector_laplacian(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<DerivedE>& E,
    Eigen::PlainObjectBase<DerivedOE>& oE,
    Eigen::SparseMatrix<ScalarL>& L);


  // Version that uses intrinsic quantities as input
  //
  // Inputs:
  //  F: input mesh connectivity
  //  l_sq: squared edge lengths of each halfedge
  //  dA: double area of each face
  //  E: a mapping from each halfedge to each edge.
  //  oE: the orientation of each halfedge compared to the orientation of the
  //      actual edge.
  //
  // Outputs:
  //  L: computed Laplacian matrix

  template <typename DerivedF, typename DerivedL_sq, typename DerivedE,
  typename DerivedOE, typename ScalarL>
  IGL_INLINE void
  cr_vector_laplacian_intrinsic(
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedL_sq>& l_sq,
    const Eigen::MatrixBase<DerivedE>& E,
    const Eigen::MatrixBase<DerivedOE>& oE,
    Eigen::SparseMatrix<ScalarL>& L);

  template <typename DerivedF, typename DerivedL_sq, typename DeriveddA,
  typename DerivedE, typename DerivedOE, typename ScalarL>
  IGL_INLINE void
  cr_vector_laplacian_intrinsic(
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedL_sq>& l_sq,
    const Eigen::MatrixBase<DeriveddA>& dA,
    const Eigen::MatrixBase<DerivedE>& E,
    const Eigen::MatrixBase<DerivedOE>& oE,
    Eigen::SparseMatrix<ScalarL>& L);


}


#ifndef IGL_STATIC_LIBRARY
#  include "cr_vector_laplacian.cpp"
#endif

#endif
