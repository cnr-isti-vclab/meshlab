// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2020 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef FIT_CUBIC_BEZIER_H
#define FIT_CUBIC_BEZIER_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <vector>
namespace igl
{
  // Fit a cubic bezier spline (G1 continuous) to an ordered list of input
  // points in any dimension, according to "An algorithm for automatically
  // fitting digitized curves" [Schneider 1990].
  //
  // Inputs:
  //   d  #d by dim list of points along a curve to be fit with a cubic bezier
  //     spline (should probably be roughly uniformly spaced). If d(0)==d(end),
  //     then will treat as a closed curve.
  //   error  maximum squared distance allowed
  // Output:
  //   cubics #cubics list of 4 by dim lists of cubic control points
  IGL_INLINE void fit_cubic_bezier(
    const Eigen::MatrixXd & d,
    const double error,
    std::vector<Eigen::MatrixXd> & cubics);
  // Recursive helper function. 
  //
  // Inputs:
  //    first  index of first point in d of substring
  //    last  index of last point in d of substring
  //    tHat1  tangent to use at beginning of spline
  //    tHat2  tangent to use at end of spline
  //    error  see above
  //    force_split  whether to force a split (i.e., force a recursive call)
  //    cubics  running list of cubics so far
  // Outputs
  //    cubics  running list of cubics so far (new cubics appended)
  IGL_INLINE void fit_cubic_bezier_substring(
    const Eigen::MatrixXd & d,
    const int first,
    const int last,
    const Eigen::RowVectorXd & tHat1,
    const Eigen::RowVectorXd & tHat2,
    const double error,
    const bool force_split, 
    std::vector<Eigen::MatrixXd> & cubics);
}

#ifndef IGL_STATIC_LIBRARY
#include "fit_cubic_bezier.cpp"
#endif

#endif 
