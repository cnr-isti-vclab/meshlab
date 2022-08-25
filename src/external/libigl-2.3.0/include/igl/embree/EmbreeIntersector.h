// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//               2014 Christian Schüller <schuellchr@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
// igl function interface for Embree2.2
//
// Necessary changes to switch from previous Embree versions:
// * Use igl:Hit instead of embree:Hit (where id0 -> id)
// * For Embree2.2
// * Uncomment #define __USE_RAY_MASK__ in platform.h to enable masking

#ifndef IGL_EMBREE_EMBREE_INTERSECTOR_H
#define IGL_EMBREE_EMBREE_INTERSECTOR_H

#include "../Hit.h"
#include <Eigen/Geometry>
#include <Eigen/Core>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include <iostream>
#include <vector>

#include "EmbreeDevice.h"

namespace igl
{
  namespace embree
  {
    class EmbreeIntersector
    {
    public:
      typedef Eigen::Matrix<float,Eigen::Dynamic,3> PointMatrixType;
      typedef Eigen::Matrix<int,Eigen::Dynamic,3> FaceMatrixType;
    public:
      EmbreeIntersector();
    private:
      // Copying and assignment are not allowed.
      EmbreeIntersector(const EmbreeIntersector & that);
      EmbreeIntersector & operator=(const EmbreeIntersector &);
    public:
      virtual ~EmbreeIntersector();

      // Initialize with a given mesh.
      //
      // Inputs:
      //   V  #V by 3 list of vertex positions
      //   F  #F by 3 list of Oriented triangles
      //   isStatic  scene is optimized for static geometry
      // Side effects:
      //   The first time this is ever called the embree engine is initialized.
      void init(
        const PointMatrixType& V,
        const FaceMatrixType& F,
        bool isStatic = false);

      // Initialize with a given mesh.
      //
      // Inputs:
      //   V  vector of #V by 3 list of vertex positions for each geometry
      //   F  vector of #F by 3 list of Oriented triangles for each geometry
      //   masks  a 32 bit mask to identify active geometries.
      //   isStatic  scene is optimized for static geometry
      // Side effects:
      //   The first time this is ever called the embree engine is initialized.
      void init(
        const std::vector<const PointMatrixType*>& V,
        const std::vector<const FaceMatrixType*>& F,
        const std::vector<int>& masks,
        bool isStatic = false);

      // Deinitialize embree datasctructures for current mesh.  Also called on
      // destruction: no need to call if you just want to init() once and
      // destroy.
      void deinit();

      // Given a ray find the first hit
      //
      // Inputs:
      //   origin     3d origin point of ray
      //   direction  3d (not necessarily normalized) direction vector of ray
      //   tnear      start of ray segment
      //   tfar       end of ray segment
      //   masks      a 32 bit mask to identify active geometries.
      // Output:
      //   hit        information about hit
      // Returns true if and only if there was a hit
      bool intersectRay(
        const Eigen::RowVector3f& origin,
        const Eigen::RowVector3f& direction,
        Hit& hit,
        float tnear = 0,
        float tfar = std::numeric_limits<float>::infinity(),
        int mask = 0xFFFFFFFF) const;

      // Given a ray find the first hit
      // This is a conservative hit test where multiple rays within a small radius
      // will be tested and only the closesest hit is returned.
      //
      // Inputs:
      //   origin     3d origin point of ray
      //   direction  3d (not necessarily normalized) direction vector of ray
      //   tnear      start of ray segment
      //   tfar       end of ray segment
      //   masks      a 32 bit mask to identify active geometries.
      //   geoId      id of geometry mask (default std::numeric_limits<float>::infinity() if no: no masking)
      //   closestHit true for gets closest hit, false for furthest hit
      // Output:
      //   hit        information about hit
      // Returns true if and only if there was a hit
      bool intersectBeam(
        const Eigen::RowVector3f& origin,
        const Eigen::RowVector3f& direction,
        Hit& hit,
        float tnear = 0,
        float tfar = std::numeric_limits<float>::infinity(),
        int mask = 0xFFFFFFFF,
        int geoId = -1,
        bool closestHit = true,
        unsigned int samples = 4) const;

      // Given a ray find all hits in order
      //
      // Inputs:
      //   origin     3d origin point of ray
      //   direction  3d (not necessarily normalized) direction vector of ray
      //   tnear      start of ray segment
      //   tfar       end of ray segment
      //   masks      a 32 bit mask to identify active geometries.
      // Output:
      //   hit        information about hit
      //   num_rays   number of rays shot (at least one)
      // Returns true if and only if there was a hit
      bool intersectRay(
        const Eigen::RowVector3f& origin,
        const Eigen::RowVector3f& direction,
        std::vector<Hit > &hits,
        int& num_rays,
        float tnear = 0,
        float tfar = std::numeric_limits<float>::infinity(),
        int mask = 0xFFFFFFFF) const;

      // Given a ray find the first hit
      //
      // Inputs:
      //   a    3d first end point of segment
      //   ab   3d vector from a to other endpoint b
      // Output:
      //   hit  information about hit
      // Returns true if and only if there was a hit
      bool intersectSegment(
        const Eigen::RowVector3f& a,
        const Eigen::RowVector3f& ab,
        Hit &hit,
        int mask = 0xFFFFFFFF) const;

    private:

      struct Vertex   {float x,y,z,a;};
      struct Triangle {int v0, v1, v2;};

      RTCScene scene;
      unsigned geomID;
      Vertex* vertices;
      Triangle* triangles;
      bool initialized;

      RTCDevice device;

      void createRay(
        RTCRayHit& ray,
        const Eigen::RowVector3f& origin,
        const Eigen::RowVector3f& direction,
        float tnear,
        float tfar,
        int mask) const;
    };
  }
}

#ifndef IGL_STATIC_LIBRARY
#  include "EmbreeIntersector.cpp"
#endif

#endif //EMBREE_INTERSECTOR_H
