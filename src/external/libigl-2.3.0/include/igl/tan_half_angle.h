// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2018 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_TAN_HALF_ANGLE_H
#define IGL_TAN_HALF_ANGLE_H
#include "igl_inline.h"
namespace igl
{
  // TAN_HALF_ANGLE Compute the tangent of half of the angle opposite the side
  // with length a, in a triangle with side lengths (a,b,c).
  //
  // Inputs:
  //   a  scalar edge length of first side of triangle
  //   b  scalar edge length of second side of triangle
  //   c  scalar edge length of third side of triangle
  // Returns tangent of half of the angle opposite side with length a
  //
  // See also: is_intrinsic_delaunay
  template < typename Scalar>
  IGL_INLINE Scalar tan_half_angle(
    const Scalar & a,
    const Scalar & b,
    const Scalar & c);
}

#ifndef IGL_STATIC_LIBRARY
#  include "tan_half_angle.cpp"
#endif

#endif

