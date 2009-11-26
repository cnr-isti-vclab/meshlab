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
    // 8 triangles and 6 octagons
    /*  Perm(±ξ, ±1, ±1)
        where ξ = √2 - 1
    */

 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 double E = sqrt(2) - 1;

 in.Clear();
 Allocator<MeshType>::AddVertices(in, 24);
 Allocator<MeshType>::AddFaces(in, 6*6+8); //44 faces

 VertexPointer ivp[24];
 VertexIterator vi=in.vert.begin();

 ivp[0]=&*vi;(*vi).P()=CoordType  ( E, 1, 1); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType  ( E, 1,-1); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType  ( E,-1, 1); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType  ( E,-1,-1); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType  (-E, 1, 1); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType  (-E, 1,-1); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType  (-E,-1, 1); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType  (-E,-1,-1); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType  ( 1, E, 1); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType  ( 1, E,-1); ++vi;
 ivp[10]=&*vi;(*vi).P()=CoordType ( 1,-E, 1); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType ( 1,-E,-1); ++vi;
 ivp[12]=&*vi;(*vi).P()=CoordType (-1, E, 1); ++vi;
 ivp[13]=&*vi;(*vi).P()=CoordType (-1, E,-1); ++vi;
 ivp[14]=&*vi;(*vi).P()=CoordType (-1,-E, 1); ++vi;
 ivp[15]=&*vi;(*vi).P()=CoordType (-1,-E,-1); ++vi;
 ivp[16]=&*vi;(*vi).P()=CoordType ( 1, 1, E); ++vi;
 ivp[17]=&*vi;(*vi).P()=CoordType ( 1, 1,-E); ++vi;
 ivp[18]=&*vi;(*vi).P()=CoordType ( 1,-1, E); ++vi;
 ivp[19]=&*vi;(*vi).P()=CoordType ( 1,-1,-E); ++vi;
 ivp[20]=&*vi;(*vi).P()=CoordType (-1, 1, E); ++vi;
 ivp[21]=&*vi;(*vi).P()=CoordType (-1, 1,-E); ++vi;
 ivp[22]=&*vi;(*vi).P()=CoordType (-1,-1, E); ++vi;
 ivp[23]=&*vi;(*vi).P()=CoordType (-1,-1,-E);

 FaceIterator fi=in.face.begin();
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[20];  (*fi).V(2)=ivp[12];  ++fi;    // triangles
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[16];  ++fi;     // ..
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[17];  ++fi;     // ..
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[5];  (*fi).V(2)=ivp[13];  ++fi;    // ..
 (*fi).V(0)=ivp[23];  (*fi).V(1)=ivp[15];  (*fi).V(2)=ivp[7];  ++fi;    // ..
 (*fi).V(0)=ivp[6];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[22];  ++fi;    // ..
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[2];  (*fi).V(2)=ivp[18];  ++fi;    // ..
 (*fi).V(0)=ivp[11];  (*fi).V(1)=ivp[19];  (*fi).V(2)=ivp[3];  ++fi;    // end of triangles
 (*fi).V(0)=ivp[5];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[13];  ++fi;
 (*fi).V(0)=ivp[1];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[13];  ++fi;
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[15];  (*fi).V(2)=ivp[13];  ++fi;
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[15];  ++fi;
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[3];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[9];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[5];  (*fi).V(1)=ivp[21];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[17];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[16];  (*fi).V(2)=ivp[17];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[0];  (*fi).V(2)=ivp[16];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[20];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[20];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[17];  (*fi).V(2)=ivp[16];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[17];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[8];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[10];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[19];  (*fi).V(2)=ivp[9];  ++fi;
 (*fi).V(0)=ivp[19];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[9];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[3];  (*fi).V(2)=ivp[19];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[23];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[22];  (*fi).V(2)=ivp[23];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[22];  ++fi;
 (*fi).V(0)=ivp[18];  (*fi).V(1)=ivp[2];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[4];  ++fi;
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[8];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[10];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[2];  ++fi;
 (*fi).V(0)=ivp[14];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[2];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[22];  (*fi).V(2)=ivp[14];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[23];  (*fi).V(2)=ivp[22];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[15];  (*fi).V(2)=ivp[23];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[13];  (*fi).V(2)=ivp[15];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[21];  (*fi).V(2)=ivp[13];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[20];  (*fi).V(2)=ivp[21];

  if (in.HasPerFaceFlags()) {
    FaceIterator fi=in.face.begin();
    for (int k=0; k<44; k++) {
      (*fi).SetF(1); fi++;
    }
  }

}

template <class MeshType>
void Truncated_Octahedron(MeshType &in)
{
    // 6 squares and 8 hexagons
    /* F = 14, V = 24
       Perm(0, ±1, ±2)
    */

 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 in.Clear();
 Allocator<MeshType>::AddVertices(in, 24);
 Allocator<MeshType>::AddFaces(in, 6*2+8*4); // 44 faces

 VertexPointer ivp[24];
 VertexIterator vi=in.vert.begin();

 ivp[0]=&*vi;(*vi).P()=CoordType  ( 0, 1, 2); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType  ( 0, 1,-2); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType  ( 0,-1, 2); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType  ( 0,-1,-2); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType  ( 0, 2, 1); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType  ( 0, 2,-1); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType  ( 0,-2, 1); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType  ( 0,-2,-1); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType  ( 1, 0, 2); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType  ( 1, 0,-2); ++vi;
 ivp[10]=&*vi;(*vi).P()=CoordType  ( 1, 2, 0); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType  ( 1,-2, 0); ++vi;
 ivp[12]=&*vi;(*vi).P()=CoordType  (-1, 0, 2); ++vi;
 ivp[13]=&*vi;(*vi).P()=CoordType  (-1, 0,-2); ++vi;
 ivp[14]=&*vi;(*vi).P()=CoordType  (-1, 2, 0); ++vi;
 ivp[15]=&*vi;(*vi).P()=CoordType  (-1,-2, 0); ++vi;
 ivp[16]=&*vi;(*vi).P()=CoordType  ( 2, 0, 1); ++vi;
 ivp[17]=&*vi;(*vi).P()=CoordType  ( 2, 0,-1); ++vi;
 ivp[18]=&*vi;(*vi).P()=CoordType  (-2, 0, 1); ++vi;
 ivp[19]=&*vi;(*vi).P()=CoordType  (-2, 0,-1); ++vi;
 ivp[20]=&*vi;(*vi).P()=CoordType  ( 2, 1, 0); ++vi;
 ivp[21]=&*vi;(*vi).P()=CoordType  ( 2,-1, 0); ++vi;
 ivp[22]=&*vi;(*vi).P()=CoordType  (-2, 1, 0); ++vi;
 ivp[23]=&*vi;(*vi).P()=CoordType  (-2,-1, 0);

 FaceIterator fi=in.face.begin();
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];

  if (in.HasPerFaceFlags()) {
    FaceIterator fi=in.face.begin();
    for (int k=0; k<44; k++) {
      (*fi).SetF(1); fi++;
    }
  }

}

template <class MeshType>
void Rhombicuboctahedron(MeshType &in)
{
    // 8 triangles and 18 squares
    /* F = 26, V = 24
       Perm(±1, ±1, ±(1+√2))
    */

 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 double E = 1 + sqrt(2);

 in.Clear();
 Allocator<MeshType>::AddVertices(in, 24);
 Allocator<MeshType>::AddFaces(in, 18*2+8); //44 faces

 VertexPointer ivp[24];
 VertexIterator vi=in.vert.begin();

 ivp[0]=&*vi;(*vi).P()=CoordType  ( 1, 1, E); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType  ( 1, 1,-E); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType  ( 1,-1, E); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType  ( 1,-1,-E); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType  ( 1, E, 1); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType  ( 1, E,-1); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType  ( 1,-E, 1); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType  ( 1,-E,-1); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType  (-1, 1, E); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType  (-1, 1,-E); ++vi;
 ivp[10]=&*vi;(*vi).P()=CoordType  (-1,-1, E); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType  (-1,-1,-E); ++vi;
 ivp[12]=&*vi;(*vi).P()=CoordType  (-1, E, 1); ++vi;
 ivp[13]=&*vi;(*vi).P()=CoordType  (-1, E,-1); ++vi;
 ivp[14]=&*vi;(*vi).P()=CoordType  (-1,-E, 1); ++vi;
 ivp[15]=&*vi;(*vi).P()=CoordType  (-1,-E,-1); ++vi;
 ivp[16]=&*vi;(*vi).P()=CoordType ( E, 1, 1); ++vi;
 ivp[17]=&*vi;(*vi).P()=CoordType ( E, 1,-1); ++vi;
 ivp[18]=&*vi;(*vi).P()=CoordType ( E,-1, 1); ++vi;
 ivp[19]=&*vi;(*vi).P()=CoordType ( E,-1,-1); ++vi;
 ivp[20]=&*vi;(*vi).P()=CoordType (-E, 1, 1); ++vi;
 ivp[21]=&*vi;(*vi).P()=CoordType (-E, 1,-1); ++vi;
 ivp[22]=&*vi;(*vi).P()=CoordType (-E,-1, 1); ++vi;
 ivp[23]=&*vi;(*vi).P()=CoordType (-E,-1,-1);

 FaceIterator fi=in.face.begin();
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[1];

  if (in.HasPerFaceFlags()) {
    FaceIterator fi=in.face.begin();
    for (int k=0; k<44; k++) {
      (*fi).SetF(1); fi++;
    }
  }
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
