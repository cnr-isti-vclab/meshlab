// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>, Olga Diamanti <olga.diam@gmail.com>, Kevin Walliman <wkevin@student.ethz.ch>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_COMISO_MIQ_H
#define IGL_COMISO_MIQ_H
#include "../../igl_inline.h"
#include <Eigen/Core>
#include <vector>

namespace igl
{
  namespace copyleft
  {
  namespace comiso
  {
    // Global seamless parametrization aligned with a given per-face Jacobian (PD1, PD2).
    // The algorithm is based on
    // "Mixed-Integer Quadrangulation" by D. Bommes, H. Zimmer, L. Kobbelt
    // ACM SIGGRAPH 2009, Article No. 77 (http://dl.acm.org/citation.cfm?id=1531383)
    // We thank Nico Pietroni for providing a reference implementation of MIQ
    // on which our code is based.

    //  Limitations:
    //  -  Due to the way of handling of hardFeatures the algorithm  may fail in difficult cases.
    //  -  Meshes with boundaries are not hendled properly i.e., jagged edges along the boundary are possible

    // Input:
    // V                 #V by 3 list of mesh vertex 3D positions
    // F                 #F by 3 list of faces indices in V
    // PD1               #V by 3 first line of the Jacobian per triangle
    // PD2               #V by 3 second line of the Jacobian per triangle
    //                   (optional, if empty it will be a vector in the tangent plane orthogonal to PD1)
    // gradientSize      global scaling for the gradient (controls the quads resolution)
    // stiffness         weight for the stiffness iterations (Reserved but not used!)
    // directRound       greedily round all integer variables at once (greatly improves optimization speed but lowers quality)
    // iter              stiffness iterations (0 = no stiffness)
    // localIter         number of local iterations for the integer rounding
    // doRound           enables the integer rounding (disabling it could be useful for debugging)
    // singularityRound  set true/false to decide if the singularities' coordinates should be rounded to the nearest integers
    // roundVertices     id of additional vertices that should be snapped to integer coordinates
    // hardFeatures      #H by 2 list of pairs of vertices that belongs to edges that should be snapped to integer coordinates
    //
    // Output:
    // UV                 #UV by 2 list of vertices in 2D
    // FUV                #FUV by 3 list of face indices in UV
    //
    template <typename DerivedV, typename DerivedF, typename DerivedU>
    IGL_INLINE void miq(
      const Eigen::PlainObjectBase<DerivedV> &V,
      const Eigen::PlainObjectBase<DerivedF> &F,
      const Eigen::PlainObjectBase<DerivedV> &PD1,
      const Eigen::PlainObjectBase<DerivedV> &PD2,
      Eigen::PlainObjectBase<DerivedU> &UV,
      Eigen::PlainObjectBase<DerivedF> &FUV,
      double gradientSize = 30.0,
      double stiffness = 5.0,
      bool directRound = false,
      unsigned int iter = 5,
      unsigned int localIter = 5,
      bool doRound = true,
      bool singularityRound = true,
      const std::vector<int> &roundVertices = std::vector<int>(),
      const std::vector<std::vector<int>> &hardFeatures = std::vector<std::vector<int> >());

    // Helper function that allows to directly provided pre-combed bisectors for an already cut mesh

    // Input:
    // V                  #V by 3 list of mesh vertex 3D positions
    // F                  #F by 3 list of faces indices in V

    // Additional Input:
    // PD1_combed         #F by 3 first combed Jacobian
    // PD2_combed         #F by 3 second combed Jacobian
    // mismatch             #F by 3 list of per-corner integer PI/2 rotations
    // singular           #V list of flag that denotes if a vertex is singular or not
    // seams              #F by 3 list of per-corner flag that denotes seams

    // Input:
    // gradientSize       global scaling for the gradient (controls the quads resolution)
    // stiffness          weight for the stiffness iterations (Reserved but not used!)
    // directRound        greedily round all integer variables at once (greatly improves optimization speed but lowers quality)
    // iter               stiffness iterations (0 = no stiffness)
    // localIter          number of local iterations for the integer rounding
    // doRound            enables the integer rounding (disabling it could be useful for debugging)
    // singularityRound   set true/false to decide if the singularities' coordinates should be rounded to the nearest integers
    // roundVertices      id of additional vertices that should be snapped to integer coordinates
    // hardFeatures       #H by 2 list of pairs of vertices that belongs to edges that should be snapped to integer coordinates

    // Output:
    // UV                 #UV by 2 list of vertices in 2D
    // FUV                #FUV by 3 list of face indices in UV
    //
    template <typename DerivedV, typename DerivedF, typename DerivedU>
    IGL_INLINE void miq(
      const Eigen::PlainObjectBase<DerivedV> &V,
      const Eigen::PlainObjectBase<DerivedF> &F,
      const Eigen::PlainObjectBase<DerivedV> &PD1_combed,
      const Eigen::PlainObjectBase<DerivedV> &PD2_combed,
      const Eigen::Matrix<int, Eigen::Dynamic, 3> &mismatch,
      const Eigen::Matrix<int, Eigen::Dynamic, 1> &singular,
      const Eigen::Matrix<int, Eigen::Dynamic, 3> &seams,
      Eigen::PlainObjectBase<DerivedU> &UV,
      Eigen::PlainObjectBase<DerivedF> &FUV,
      double gradientSize = 30.0,
      double stiffness = 5.0,
      bool directRound = false,
      unsigned int iter = 5,
      unsigned int localIter = 5,
      bool doRound = true,
      bool singularityRound = true,
      const std::vector<int> &roundVertices = std::vector<int>(),
      const std::vector<std::vector<int>> &hardFeatures = std::vector<std::vector<int> >());
  };
};
};
#ifndef IGL_STATIC_LIBRARY
#include "miq.cpp"
#endif

#endif
