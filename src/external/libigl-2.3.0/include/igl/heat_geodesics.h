// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2018 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_HEAT_GEODESICS_H
#define IGL_HEAT_GEODESICS_H
#include "igl_inline.h"
#include "min_quad_with_fixed.h"
#include <Eigen/Sparse>
#include <Eigen/Sparse>
namespace igl
{
  template <typename Scalar>
  struct HeatGeodesicsData
  {
    // Gradient and Divergence operators
    Eigen::SparseMatrix<Scalar> Grad,Div;
    // Number of gradient components
    int ng;
    // List of boundary vertex indices
    Eigen::VectorXi b;
    // Solvers for Dirichet, Neumann problems
    min_quad_with_fixed_data<Scalar> Dirichlet,Neumann,Poisson;
    bool use_intrinsic_delaunay = false;
  };
  // Precompute factorized solvers for computing a fast approximation of
  // geodesic distances on a mesh (V,F). [Crane et al. 2013]
  //
  // Inputs:
  //   V  #V by dim list of mesh vertex positions
  //   F  #F by 3 list of mesh face indices into V
  // Outputs:
  //   data  precomputation data (see heat_geodesics_solve)
  template < typename DerivedV, typename DerivedF, typename Scalar >
  IGL_INLINE bool heat_geodesics_precompute(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    HeatGeodesicsData<Scalar> & data);
  // Inputs:
  //   t  "heat" parameter (smaller --> more accurate, less stable)
  template < typename DerivedV, typename DerivedF, typename Scalar >
  IGL_INLINE bool heat_geodesics_precompute(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const Scalar t,
    HeatGeodesicsData<Scalar> & data);
  // Compute fast approximate geodesic distances using precomputed data from a
  // set of selected source vertices (gamma)
  //
  // Inputs: 
  //   data  precomputation data (see heat_geodesics_precompute)
  //   gamma  #gamma list of indices into V of source vertices
  // Outputs:
  //   D  #V list of distances to gamma 
  template < typename Scalar, typename Derivedgamma, typename DerivedD>
  IGL_INLINE void heat_geodesics_solve(
    const HeatGeodesicsData<Scalar> & data,
    const Eigen::MatrixBase<Derivedgamma> & gamma,
    Eigen::PlainObjectBase<DerivedD> & D);
}

#ifndef IGL_STATIC_LIBRARY
#include "heat_geodesics.cpp"
#endif

#endif
