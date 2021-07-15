// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2018 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef ACCUMARRY_H
#define ACCUMARRY_H
#include "igl_inline.h"
#include <Eigen/Core>
namespace igl
{
  // ACCUMARRY Like Matlab's accumarray. Accumulate values in V using subscripts
  // in S.
  //
  // Inputs:
  //   S  #S list of subscripts
  //   V  #V list of values
  // Outputs:
  //   A  max(subs)+1 list of accumulated values
  template <
    typename DerivedS,
    typename DerivedV,
    typename DerivedA
    >
  void accumarray(
    const Eigen::MatrixBase<DerivedS> & S,
    const Eigen::MatrixBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedA> & A);
  // Inputs:
  //   S  #S list of subscripts
  //   V  single value used for all
  // Outputs:
  //   A  max(subs)+1 list of accumulated values
  template <
    typename DerivedS,
    typename DerivedA
    >
  void accumarray(
    const Eigen::MatrixBase<DerivedS> & S,
    const typename DerivedA::Scalar V,
    Eigen::PlainObjectBase<DerivedA> & A);
}

#ifndef IGL_STATIC_LIBRARY
#  include "accumarray.cpp"
#endif

#endif
