// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2014 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_SIGNED_DISTANCE_H
#define IGL_SIGNED_DISTANCE_H

#include "igl_inline.h"
#include "AABB.h"
#include "WindingNumberAABB.h"
#include "fast_winding_number.h"
#include <Eigen/Core>
#include <vector>
namespace igl
{
  enum SignedDistanceType
  {
    // Use fast pseudo-normal test [Bærentzen & Aanæs 2005]
    SIGNED_DISTANCE_TYPE_PSEUDONORMAL         = 0,
    // Use winding number [Jacobson, Kavan Sorking-Hornug 2013]
    SIGNED_DISTANCE_TYPE_WINDING_NUMBER       = 1,
    SIGNED_DISTANCE_TYPE_DEFAULT              = 2,
    SIGNED_DISTANCE_TYPE_UNSIGNED             = 3,
    // Use Fast winding number [Barill, Dickson, Schmidt, Levin, Jacobson 2018]
    SIGNED_DISTANCE_TYPE_FAST_WINDING_NUMBER  = 4,
    NUM_SIGNED_DISTANCE_TYPE                  = 5
  };
  // Computes signed distance to a mesh
  //
  // Inputs:
  //   P  #P by 3 list of query point positions
  //   V  #V by 3 list of vertex positions
  //   F  #F by ss list of triangle indices, ss should be 3 unless sign_type ==
  //     SIGNED_DISTANCE_TYPE_UNSIGNED
  //   sign_type  method for computing distance _sign_ S
  //   lower_bound  lower bound of distances needed {std::numeric_limits::min}
  //   upper_bound  lower bound of distances needed {std::numeric_limits::max}
  // Outputs:
  //   S  #P list of smallest signed distances
  //   I  #P list of facet indices corresponding to smallest distances
  //   C  #P by 3 list of closest points
  //   N  #P by 3 list of closest normals (only set if
  //     sign_type=SIGNED_DISTANCE_TYPE_PSEUDONORMAL)
  //
  // Known bugs: This only computes distances to triangles. So unreferenced
  // vertices and degenerate triangles are ignored.
  template <
    typename DerivedP,
    typename DerivedV,
    typename DerivedF,
    typename DerivedS,
    typename DerivedI,
    typename DerivedC,
    typename DerivedN>
  IGL_INLINE void signed_distance(
    const Eigen::MatrixBase<DerivedP> & P,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const SignedDistanceType sign_type,
    const typename DerivedV::Scalar lower_bound,
    const typename DerivedV::Scalar upper_bound,
    Eigen::PlainObjectBase<DerivedS> & S,
    Eigen::PlainObjectBase<DerivedI> & I,
    Eigen::PlainObjectBase<DerivedC> & C,
    Eigen::PlainObjectBase<DerivedN> & N);
  // Computes signed distance to a mesh, with default bounds
  //
  // Inputs:
  //   P  #P by 3 list of query point positions
  //   V  #V by 3 list of vertex positions
  //   F  #F by ss list of triangle indices, ss should be 3 unless sign_type ==
  //     SIGNED_DISTANCE_TYPE_UNSIGNED
  //   sign_type  method for computing distance _sign_ S
  //   lower_bound  lower bound of distances needed {std::numeric_limits::min}
  //   upper_bound  lower bound of distances needed {std::numeric_limits::max}
  // Outputs:
  //   S  #P list of smallest signed distances
  //   I  #P list of facet indices corresponding to smallest distances
  //   C  #P by 3 list of closest points
  //   N  #P by 3 list of closest normals (only set if
  //     sign_type=SIGNED_DISTANCE_TYPE_PSEUDONORMAL)
  template <
    typename DerivedP,
    typename DerivedV,
    typename DerivedF,
    typename DerivedS,
    typename DerivedI,
    typename DerivedC,
    typename DerivedN>
  IGL_INLINE void signed_distance(
    const Eigen::MatrixBase<DerivedP> & P,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const SignedDistanceType sign_type,
    Eigen::PlainObjectBase<DerivedS> & S,
    Eigen::PlainObjectBase<DerivedI> & I,
    Eigen::PlainObjectBase<DerivedC> & C,
    Eigen::PlainObjectBase<DerivedN> & N);
  // Computes signed distance to mesh using pseudonormal with precomputed AABB tree and edge/vertice normals
  //
  // Inputs:
  //   tree  AABB acceleration tree (see AABB.h)
  //   F  #F by 3 list of triangle indices
  //   FN  #F by 3 list of triangle normals 
  //   VN  #V by 3 list of vertex normals (ANGLE WEIGHTING)
  //   EN  #E by 3 list of edge normals (UNIFORM WEIGHTING)
  //   EMAP  #F*3 mapping edges in F to E
  //   q  Query point
  // Returns signed distance to mesh
  //
  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedFN,
    typename DerivedVN,
    typename DerivedEN,
    typename DerivedEMAP,
    typename Derivedq>
  IGL_INLINE typename DerivedV::Scalar signed_distance_pseudonormal(
    const AABB<DerivedV,3> & tree,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const Eigen::MatrixBase<DerivedFN> & FN,
    const Eigen::MatrixBase<DerivedVN> & VN,
    const Eigen::MatrixBase<DerivedEN> & EN,
    const Eigen::MatrixBase<DerivedEMAP> & EMAP,
    const Eigen::MatrixBase<Derivedq> & q);
  template <
    typename DerivedP,
    typename DerivedV,
    typename DerivedF,
    typename DerivedFN,
    typename DerivedVN,
    typename DerivedEN,
    typename DerivedEMAP,
    typename DerivedS,
    typename DerivedI,
    typename DerivedC,
    typename DerivedN>
  IGL_INLINE void signed_distance_pseudonormal(
    const Eigen::MatrixBase<DerivedP> & P,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const AABB<DerivedV,3> & tree,
    const Eigen::MatrixBase<DerivedFN> & FN,
    const Eigen::MatrixBase<DerivedVN> & VN,
    const Eigen::MatrixBase<DerivedEN> & EN,
    const Eigen::MatrixBase<DerivedEMAP> & EMAP,
    Eigen::PlainObjectBase<DerivedS> & S,
    Eigen::PlainObjectBase<DerivedI> & I,
    Eigen::PlainObjectBase<DerivedC> & C,
    Eigen::PlainObjectBase<DerivedN> & N);
  // Outputs:
  //   s  sign
  //   sqrd  squared distance
  //   i  closest primitive
  //   c  closest point
  //   n  normal
  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedFN,
    typename DerivedVN,
    typename DerivedEN,
    typename DerivedEMAP,
    typename Derivedq,
    typename Scalar,
    typename Derivedc,
    typename Derivedn>
  IGL_INLINE void signed_distance_pseudonormal(
    const AABB<DerivedV,3> & tree,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const Eigen::MatrixBase<DerivedFN> & FN,
    const Eigen::MatrixBase<DerivedVN> & VN,
    const Eigen::MatrixBase<DerivedEN> & EN,
    const Eigen::MatrixBase<DerivedEMAP> & EMAP,
    const Eigen::MatrixBase<Derivedq> & q,
    Scalar & s,
    Scalar & sqrd,
    int & i,
    Eigen::PlainObjectBase<Derivedc> & c,
    Eigen::PlainObjectBase<Derivedn> & n);
  template <
    typename DerivedV,
    typename DerivedE,
    typename DerivedEN,
    typename DerivedVN,
    typename Derivedq,
    typename Scalar,
    typename Derivedc,
    typename Derivedn>
  IGL_INLINE void signed_distance_pseudonormal(
    const AABB<DerivedV,2> & tree,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedE> & E,
    const Eigen::MatrixBase<DerivedEN> & EN,
    const Eigen::MatrixBase<DerivedVN> & VN,
    const Eigen::MatrixBase<Derivedq> & q,
    Scalar & s,
    Scalar & sqrd,
    int & i,
    Eigen::PlainObjectBase<Derivedc> & c,
    Eigen::PlainObjectBase<Derivedn> & n);
  // Inputs:
  //   tree  AABB acceleration tree (see cgal/point_mesh_squared_distance.h)
  //   hier  Winding number evaluation hierarchy
  //   q  Query point
  // Returns signed distance to mesh
  template <
    typename DerivedV,
    typename DerivedF,
    typename Derivedq>
  IGL_INLINE typename DerivedV::Scalar signed_distance_winding_number(
    const AABB<DerivedV,3> & tree,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const igl::WindingNumberAABB<Derivedq,DerivedV,DerivedF> & hier,
    const Eigen::MatrixBase<Derivedq> & q);
  // Outputs:
  //   s  sign
  //   sqrd  squared distance
  //   pp  closest point and primitve
  template <
    typename DerivedV,
    typename DerivedF,
    typename Derivedq,
    typename Scalar,
    typename Derivedc>
  IGL_INLINE void signed_distance_winding_number(
    const AABB<DerivedV,3> & tree,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const igl::WindingNumberAABB<Derivedq,DerivedV,DerivedF> & hier,
    const Eigen::MatrixBase<Derivedq> & q,
    Scalar & s,
    Scalar & sqrd,
    int & i,
    Eigen::PlainObjectBase<Derivedc> & c);
  template <
    typename DerivedV,
    typename DerivedF,
    typename Derivedq,
    typename Scalar,
    typename Derivedc>
  IGL_INLINE void signed_distance_winding_number(
    const AABB<DerivedV,2> & tree,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const Eigen::MatrixBase<Derivedq> & q,
    Scalar & s,
    Scalar & sqrd,
    int & i,
    Eigen::PlainObjectBase<Derivedc> & c);


  // Calculates signed distance at query points P, using fast winding number
  //   for sign.
  //
  // Usage:
  //     VectorXd S;  
  //     VectorXd V, P; //where V is mesh vertices, P are query points
  //     VectorXi F;  
  //     igl::FastWindingNumberBVH fwn_bvh;
  //     igl::fast_winding_number(V.cast<float>(), F, 2, fwn_bvh);
  //     igl::signed_distance_fast_winding_number(P,V,F,tree,fwn_bvh,S);
  //
  // Inputs:
  //   P  #P by 3 list of query point positions
  //   V  #V by 3 list of triangle indices
  //   F  #F by 3 list of triangle normals 
  //   tree  AABB acceleration tree (see AABB.h)
  //   bvh fast winding precomputation (see Fast_Winding_Number.h)   
  // Outputs:
  //   S  #P list of signed distances of each point in P
  template <
    typename DerivedP,
    typename DerivedV,
    typename DerivedF,
    typename DerivedS>
  IGL_INLINE void signed_distance_fast_winding_number(
    const Eigen::MatrixBase<DerivedP> & P,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const AABB<DerivedV,3> & tree,
    const igl::FastWindingNumberBVH & fwn_bvh,
    Eigen::PlainObjectBase<DerivedS> & S
  );

  // Calculates signed distance at query point q, using fast winding number
  //   for sign.
  //
  // Inputs:
  //   tree  AABB acceleration tree (see AABB.h)
  //   V  #V by 3 list of triangle indices
  //   F  #F by 3 list of triangle normals 
  //   bvh fast winding precomputation (see Fast_Winding_Number.h)   
  //   q  1 by 3 list of query point positions
  // Outputs:
  //   S  #P list of signed distances of each point in P
  template <
    typename Derivedq,
    typename DerivedV,
    typename DerivedF>
  IGL_INLINE typename DerivedV::Scalar signed_distance_fast_winding_number(
    const Eigen::MatrixBase<Derivedq> & q,
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const AABB<DerivedV,3> & tree,
    const igl::FastWindingNumberBVH & fwn_bvh
  );
}

#ifndef IGL_STATIC_LIBRARY
#  include "signed_distance.cpp"
#endif

#endif
