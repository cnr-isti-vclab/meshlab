#ifndef BEZIER_H
#define BEZIER_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <vector>
namespace igl
{
  // Evaluate a polynomial Bezier Curve.
  //
  // Inputs:
  //   V  #V by dim list of Bezier control points
  //   t  evaluation parameter within [0,1]
  // Outputs:
  //   P  1 by dim output point 
  template <typename DerivedV, typename DerivedP>
  IGL_INLINE void bezier(
    const Eigen::MatrixBase<DerivedV> & V,
    const typename DerivedV::Scalar t,
    Eigen::PlainObjectBase<DerivedP> & P);
  // Evaluate a polynomial Bezier Curve.
  //
  // Inputs:
  //   V  #V by dim list of Bezier control points
  //   T  #T evaluation parameters within [0,1]
  // Outputs:
  //   P  #T  by dim output points
  template <typename DerivedV, typename DerivedT, typename DerivedP>
  IGL_INLINE void bezier(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedT> & T,
    Eigen::PlainObjectBase<DerivedP> & P);
  // Evaluate a polynomial Bezier spline with a fixed parameter set for each
  // sub-curve
  //
  // Inputs:
  //   spline #curves list of lists of Bezier control points
  //   T  #T evaluation parameters within [0,1] to use for each spline
  // Outputs:
  //   P  #curves*#T  by dim output points
  template <typename VMat, typename DerivedT, typename DerivedP>
  IGL_INLINE void bezier(
    const std::vector<VMat> & spline,
    const Eigen::MatrixBase<DerivedT> & T,
    Eigen::PlainObjectBase<DerivedP> & P);
}

#ifndef IGL_STATIC_LIBRARY
#  include "bezier.cpp"
#endif

#endif 
