#ifndef IGL_DUAL_CONTOURING_H
#define IGL_DUAL_CONTOURING_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <functional>
namespace igl
{
  // Dual contouring to extract a pure quad mesh from differentiable implicit
  // function using a dense grid.
  //
  // Inputs:
  //   f  function returning >0 outside, <0 inside and =0 on the surface
  //   f_grad  function returning ∇f/‖∇f‖
  //   min_corner  position of primal grid vertex at minimum corner
  //   max_corner  position of primal grid vertex at maximum corner
  //   nx  number of vertices on x side of primal grid
  //   ny  number of vertices on y side of primal grid
  //   nz  number of vertices on z side of primal grid
  //   constrained  whether to force dual vertices to lie strictly inside
  //     corresponding primal cell (prevents self-intersections at cost of
  //     surface quality; marginally slower)
  //   triangles  whether to output four triangles instead of one quad per
  //     crossing edge (quad mesh usually looks fine)
  //   root_finding  whether to use root finding to identify crossing point on
  //     each edge (improves quality a lot at cost of performance). If false,
  //     use linear interpolation.
  // Outputs:
  //   V  #V by 3 list of outputs vertex positions
  //   Q  #Q by 4 (or 3 if triangles=true) face indices into rows of V
  template <
    typename DerivedV,
    typename DerivedQ>
  IGL_INLINE void dual_contouring(
    const std::function<
      typename DerivedV::Scalar(const Eigen::Matrix<typename DerivedV::Scalar,1,3> &)> & f,
    const std::function<
      Eigen::Matrix<typename DerivedV::Scalar,1,3>(
        const Eigen::Matrix<typename DerivedV::Scalar,1,3> &)> & f_grad,
    const Eigen::Matrix<typename DerivedV::Scalar,1,3> & min_corner,
    const Eigen::Matrix<typename DerivedV::Scalar,1,3> & max_corner,
    const int nx,
    const int ny,
    const int nz,
    const bool constrained,
    const bool triangles,
    const bool root_finding,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedQ> & Q);
  // Inputs:
  //   Gf  nx*ny*nz list of function values so that Gf(k) = f(GV.row(k)) (only
  //   needs to be accurate near f=0 and correct sign elsewhere)
  //   GV  nx*ny*nz list of grid positions so that the x,y,z grid position is at
  //     GV.row(x+nx*(y+z*ny))
  template <
    typename DerivedGf,
    typename DerivedGV,
    typename DerivedV,
    typename DerivedQ>
  IGL_INLINE void dual_contouring(
    const std::function<
      typename DerivedV::Scalar(const Eigen::Matrix<typename DerivedV::Scalar,1,3> &)> & f,
    const std::function<
      Eigen::Matrix<typename DerivedV::Scalar,1,3>(
        const Eigen::Matrix<typename DerivedV::Scalar,1,3> &)> & f_grad,
    const Eigen::MatrixBase<DerivedGf> & Gf,
    const Eigen::MatrixBase<DerivedGV> & GV,
    const int nx,
    const int ny,
    const int nz,
    const bool constrained,
    const bool triangles,
    const bool root_finding,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedQ> & Q);
  // Sparse voxel grid
  //
  // Gf  #GV list of corresponding f values. If using root finding then only the
  //   sign needs to be correct.
  template <
    typename DerivedGf,
    typename DerivedGV,
    typename DerivedGI,
    typename DerivedV,
    typename DerivedQ>
  IGL_INLINE void dual_contouring(
    const std::function<typename DerivedV::Scalar(const Eigen::Matrix<typename DerivedV::Scalar,1,3> &)> & f,
    const std::function<Eigen::Matrix<typename DerivedV::Scalar,1,3>(const Eigen::Matrix<typename DerivedV::Scalar,1,3> &)> & f_grad,
    const Eigen::Matrix<typename DerivedV::Scalar,1,3> & step,
    const Eigen::MatrixBase<DerivedGf> & Gf,
    const Eigen::MatrixBase<DerivedGV> & GV,
    const Eigen::MatrixBase<DerivedGI> & GI,
    const bool constrained,
    const bool triangles,
    const bool root_finding,
    Eigen::PlainObjectBase<DerivedV> & V,
    Eigen::PlainObjectBase<DerivedQ> & Q);
}

#ifndef IGL_STATIC_LIBRARY
#  include "dual_contouring.cpp"
#endif
#endif
