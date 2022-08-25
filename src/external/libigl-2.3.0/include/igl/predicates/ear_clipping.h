// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2019 Hanxiao Shen <hanxiao@cs.nyu.edu>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#ifndef IGL_PREDICATES_EAR_CLIPPING_H
#define IGL_PREDICATES_EAR_CLIPPING_H

#include <Eigen/Core>
#include "../igl_inline.h"

namespace igl
{
  namespace predicates
  {

    // Implementation of ear clipping triangulation algorithm for a 2D polygon.
    // https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf
    // If the polygon is simple, all vertices will be clipped and the result mesh is (P,eF)
    // Otherwise, the function will try to clip as many ears as possible.
    //
    // Input:
    // P : n*2, size n 2D polygon
    // RT: n*1, preserved vertices (do not clip) marked as 1, otherwise 0
    // Output:
    // I : size #nP vector, maps index from nP to P, e.g. nP's ith vertex is origianlly I(i) in P
    // eF: clipped ears, in original index of P
    // nP: leftover vertices after clipping

    template <typename DerivedP, typename DerivedRT,
              typename DerivedF, typename DerivedI>
    IGL_INLINE void ear_clipping(
      const Eigen::MatrixBase<DerivedP>& P,
      const Eigen::MatrixBase<DerivedRT>& RT,
      Eigen::PlainObjectBase<DerivedI>& I,
      Eigen::PlainObjectBase<DerivedF>& eF, 
      Eigen::PlainObjectBase<DerivedP>& nP
    );

  }
}

#ifndef IGL_STATIC_LIBRARY
#  include "ear_clipping.cpp"
#endif


#endif
