#ifndef IGL_SCREEN_SPACE_SELECTION_H
#define IGL_SCREEN_SPACE_SELECTION_H

#include "igl/igl_inline.h"
#include <Eigen/Core>
#include <vector>
// Forward declaration
namespace igl { template <typename DerivedV, int DIM> class AABB; }

namespace igl
{
  // Given a mesh, a camera  determine which points are inside of a given 2D
  // screen space polygon **culling points based on self-occlusion.**
  //
  // Inputs:
  //   V  #V by 3 list of mesh vertex positions
  //   F  #F by 3 list of mesh triangle indices into rows of V
  //   tree  precomputed bounding volume heirarchy
  //   model  4 by 4 camera model-view matrix
  //   proj  4 by 4 camera projection matrix (perspective or orthoraphic)
  //   viewport  4-vector containing camera viewport
  //   L  #L by 2 list of 2D polygon vertices (in order)
  // Outputs:
  //   W  #V by 1 list of winding numbers (|W|>0.5 indicates inside)
  //   and_visible  #V by 1 list of visibility values (only correct for vertices
  //     with |W|>0.5)
  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedM,
    typename DerivedN,
    typename DerivedO,
    typename Ltype,
    typename DerivedW,
    typename Deriveda>
  IGL_INLINE void screen_space_selection(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedF> & F,
    const igl::AABB<DerivedV, 3> & tree,
    const Eigen::MatrixBase<DerivedM>& model,
    const Eigen::MatrixBase<DerivedN>& proj,
    const Eigen::MatrixBase<DerivedO>& viewport,
    const std::vector<Eigen::Matrix<Ltype,1,2> > & L,
    Eigen::PlainObjectBase<DerivedW> & W,
    Eigen::PlainObjectBase<Deriveda> & and_visible);
  // Given a mesh, a camera  determine which points are inside of a given 2D
  // screen space polygon
  //
  // Inputs:
  //   V  #V by 3 list of mesh vertex positions
  //   model  4 by 4 camera model-view matrix
  //   proj  4 by 4 camera projection matrix (perspective or orthoraphic)
  //   viewport  4-vector containing camera viewport
  //   L  #L by 2 list of 2D polygon vertices (in order)
  // Outputs:
  //   W  #V by 1 list of winding numbers (|W|>0.5 indicates inside)
  template <
    typename DerivedV,
    typename DerivedM,
    typename DerivedN,
    typename DerivedO,
    typename Ltype,
    typename DerivedW>
  IGL_INLINE void screen_space_selection(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedM>& model,
    const Eigen::MatrixBase<DerivedN>& proj,
    const Eigen::MatrixBase<DerivedO>& viewport,
    const std::vector<Eigen::Matrix<Ltype,1,2> > & L,
    Eigen::PlainObjectBase<DerivedW> & W);
  // Given a mesh, a camera  determine which points are inside of a given 2D
  // screen space polygon
  //
  // Inputs:
  //   V  #V by 3 list of mesh vertex positions
  //   model  4 by 4 camera model-view matrix
  //   proj  4 by 4 camera projection matrix (perspective or orthoraphic)
  //   viewport  4-vector containing camera viewport
  //   P  #P by 2 list of screen space polygon vertices
  //   E  #E by 2 list of screen space edges as indices into rows of P
  // Outputs:
  //   W  #V by 1 list of winding numbers (|W|>0.5 indicates inside)
  template <
    typename DerivedV,
    typename DerivedM,
    typename DerivedN,
    typename DerivedO,
    typename DerivedP,
    typename DerivedE,
    typename DerivedW>
  IGL_INLINE void screen_space_selection(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedM>& model,
    const Eigen::MatrixBase<DerivedN>& proj,
    const Eigen::MatrixBase<DerivedO>& viewport,
    const Eigen::MatrixBase<DerivedP> & P,
    const Eigen::MatrixBase<DerivedE> & E,
    Eigen::PlainObjectBase<DerivedW> & W);
}

#ifndef IGL_STATIC_LIBRARY
#include "screen_space_selection.cpp"
#endif
  
#endif
