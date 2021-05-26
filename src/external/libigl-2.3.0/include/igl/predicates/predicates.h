// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2019 Qingnan Zhou <qnzhou@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#pragma once
#ifndef IGL_PREDICATES_PREDICATES_H
#define IGL_PREDICATES_PREDICATES_H

#include <igl/igl_inline.h>
#include <Eigen/Core>

namespace igl {
  namespace predicates {
    enum class Orientation {
      POSITIVE=1, INSIDE=1,
      NEGATIVE=-1, OUTSIDE=-1,
      COLLINEAR=0, COPLANAR=0, COCIRCULAR=0, COSPHERICAL=0, DEGENERATE=0
    };

    // Initialize internal variable used by predciates. Must be called before
    // using exact predicates. It is safe to call this function from multiple
    // threads.
    IGL_INLINE void exactinit();

    // Compute the orientation of the triangle formed by pa, pb, pc.
    //
    // Input:
    //   pa, pb, pc  2D points.
    //
    // Output:
    //   Return POSITIVE if pa, pb, pc are counterclockwise oriented.
    //          NEGATIVE if they are clockwise oriented.
    //          COLLINEAR if they are collinear.
    template<typename Vector2D>
    IGL_INLINE Orientation orient2d(
        const Eigen::MatrixBase<Vector2D>& pa,
        const Eigen::MatrixBase<Vector2D>& pb,
        const Eigen::MatrixBase<Vector2D>& pc);

    // Compute the orientation of the tetrahedron formed by pa, pb, pc, pd.
    //
    // Input:
    //   pa, pb, pc, pd  3D points.
    //
    // Output:
    //   Return POSITIVE if pd is "below" the oriented plane formed by pa, pb and pc.
    //          NEGATIVE if pd is "above" the plane.
    //          COPLANAR if pd is on the plane.
    template<typename Vector3D>
    IGL_INLINE Orientation orient3d(
        const Eigen::MatrixBase<Vector3D>& pa,
        const Eigen::MatrixBase<Vector3D>& pb,
        const Eigen::MatrixBase<Vector3D>& pc,
        const Eigen::MatrixBase<Vector3D>& pd);

    // Decide whether a point is inside/outside/on a circle.
    //
    // Input:
    //   pa, pb, pc  2D points that defines an oriented circle.
    //   pd          2D query point.
    //
    // Output:
    //   Return INSIDE if pd is inside of the circle defined by pa, pb and pc.
    //          OUSIDE if pd is outside of the circle.
    //          COCIRCULAR pd is exactly on the circle.
    template<typename Vector2D>
    IGL_INLINE Orientation incircle(
        const Eigen::MatrixBase<Vector2D>& pa,
        const Eigen::MatrixBase<Vector2D>& pb,
        const Eigen::MatrixBase<Vector2D>& pc,
        const Eigen::MatrixBase<Vector2D>& pd);

    // Decide whether a point is inside/outside/on a sphere.
    //
    // Input:
    //   pa, pb, pc, pd  3D points that defines an oriented sphere.
    //   pe              3D query point.
    //
    // Output:
    //   Return INSIDE if pe is inside of the sphere defined by pa, pb, pc and pd.
    //          OUSIDE if pe is outside of the sphere.
    //          COSPHERICAL pd is exactly on the sphere.
    template<typename Vector3D>
    IGL_INLINE Orientation insphere(
        const Eigen::MatrixBase<Vector3D>& pa,
        const Eigen::MatrixBase<Vector3D>& pb,
        const Eigen::MatrixBase<Vector3D>& pc,
        const Eigen::MatrixBase<Vector3D>& pd,
        const Eigen::MatrixBase<Vector3D>& pe);
  }
}

#ifndef IGL_STATIC_LIBRARY
#  include "predicates.cpp"
#endif

#endif
