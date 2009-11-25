/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2009                                                \/)\/    *
* Pablo Castellano García-Saavedra                                /\/|      *
*                                                                    |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef __VCGLIB_ARCHIMEDEAN_H
#define __VCGLIB_ARCHIMEDEAN_H

#include <vcg/math/base.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/update/flag.h>

namespace vcg {
namespace tri {

template <class MeshType>
void Truncated_Tetrahedron(MeshType &in)
{
    // 12 vertices y 8 caras
    /* (+3,+1,+1), (+1,+3,+1), (+1,+1,+3)
    (−3,−1,+1), (−1,−3,+1), (−1,−1,+3)
    (−3,+1,−1), (−1,+3,−1), (−1,+1,−3)
    (+3,−1,−1), (+1,−3,−1), (+1,−1,−3)
    */

 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 in.Clear();
 Allocator<MeshType>::AddVertices(in, 12);
 Allocator<MeshType>::AddFaces(in, 4*4+4); //20 triangles

 VertexPointer ivp[12];
 VertexIterator vi=in.vert.begin();

 ivp[0]=&*vi;(*vi).P()=CoordType ( 3, 1, 1); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType ( 1, 3, 1); ++vi;
 ivp[2] =&*vi;(*vi).P()=CoordType ( 1, 1, 3); ++vi;
 ivp[3] =&*vi;(*vi).P()=CoordType (-3,-1, 1); ++vi;
 ivp[4] =&*vi;(*vi).P()=CoordType (-1,-3, 1); ++vi;
 ivp[5] =&*vi;(*vi).P()=CoordType (-1,-1, 3); ++vi;
 ivp[6] =&*vi;(*vi).P()=CoordType (-3, 1,-1); ++vi;
 ivp[7] =&*vi;(*vi).P()=CoordType (-1, 3,-1); ++vi;
 ivp[8] =&*vi;(*vi).P()=CoordType (-1, 1,-3); ++vi;
 ivp[9] =&*vi;(*vi).P()=CoordType ( 3,-1,-1); ++vi;
 ivp[10] =&*vi;(*vi).P()=CoordType ( 1,-3,-1); ++vi;
 ivp[11] =&*vi;(*vi).P()=CoordType ( 1,-1,-3);

 FaceIterator fi=in.face.begin();
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[8];  ++fi;
 (*fi).V(0)=ivp[11]; (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[9];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[5];  ++fi;    //triangle1
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[2];  ++fi;    //triangle2
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[7];  ++fi;    //triangle3
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[10]; (*fi).V(2)=ivp[11]; ++fi;    //triangle4
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[5];  (*fi).V(2)=ivp[4];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[10]; ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[10];  (*fi).V(2)=ivp[9]; ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[3];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[10]; (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[11]; ++fi;
 (*fi).V(0)=ivp[10]; (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[8];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[6]; (*fi).V(2)=ivp[10];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[10]; (*fi).V(2)=ivp[4];

  if (in.HasPerFaceFlags()) {
    FaceIterator fi=in.face.begin();
    for (int k=0; k<20; k++) {
      (*fi).SetF(1); fi++;
    }
  }

}

template <class MeshType>
void Cuboctahedron(MeshType &in)
{
    //F = 14, V = 12
    //8 triangles and 6 squares
    /*
        (±1,±1,0)
        (±1,0,±1)
        (0,±1,±1)
    */

 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 in.Clear();
 Allocator<MeshType>::AddVertices(in, 12);
 Allocator<MeshType>::AddFaces(in, 6*2+8); //20 triangles

 VertexPointer ivp[12];

 VertexIterator vi=in.vert.begin();

 ivp[0]=&*vi;(*vi).P()=CoordType ( 1, 1, 0); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType ( 1,-1, 0); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType (-1,-1, 0); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType (-1, 1, 0); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType ( 1, 0, 1); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType (-1, 0, 1); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType (-1, 0,-1); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType ( 1, 0,-1); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType ( 0, 1, 1); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType ( 0, 1,-1); ++vi;
 ivp[10]=&*vi;(*vi).P()=CoordType ( 0,-1, 1); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType ( 0,-1,-1);

 FaceIterator fi=in.face.begin();
 (*fi).V(0)=ivp[1];  (*fi).V(1)=ivp[10];  (*fi).V(2)=ivp[2];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[0];  (*fi).V(2)=ivp[9];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[5];  (*fi).V(2)=ivp[2];  ++fi;
 (*fi).V(0)=ivp[5];  (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[1];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[10];  ++fi;
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[0];  (*fi).V(2)=ivp[8];  ++fi;
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[11];  (*fi).V(1)=ivp[2];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[7];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[1];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[6];  (*fi).V(1)=ivp[2];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[7];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[7];

  if (in.HasPerFaceFlags()) {
    FaceIterator fi=in.face.begin();
    for (int k=0; k<20; k++) {
      (*fi).SetF(1); fi++;
    }
  }

}

template <class MeshType>
void Truncated_Cube(MeshType &in)
{
    // F = 14, V = 24
    /*  (±ξ, ±1, ±1),
        (±1, ±ξ, ±1),
        (±1, ±1, ±ξ)
        where ξ = √2 - 1
    */

}

template <class MeshType>
void Truncated_Octahedron(MeshType &in)
{
    /* F = 14, V = 24
       Perm(0, ±1, ±2)
    */
}

template <class MeshType>
void Rhombicuboctahedron(MeshType &in)
{
    /* F = 26, V = 24
       Perm(±1, ±1, ±(1+√2))
    */
}

template <class MeshType>
void Truncated_cuboctahedron(MeshType &in)
{
    /* F = 26, V = 48
       Perm(±1, ±(1+√2), ±(1+√8))
    */
}

/****************************************
TODO:
    snub cube
    icosidodecahedron
    truncated dodecahedron
    truncated icosahedron (soccer ball)
    rhombicosidodecahedron
    truncated icosidodecahedron
    snub dodecahedron
**************************************************/

} // End Namespace TriMesh
} // End Namespace vcg

#endif // __VCGLIB_ARCHIMEDEAN_H
