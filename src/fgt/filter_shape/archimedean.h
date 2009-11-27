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

 ivp[0]=&*vi;(*vi).P()=CoordType  ( 3, 1, 1); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType  ( 1, 3, 1); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType  ( 1, 1, 3); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType  (-3,-1, 1); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType  (-1,-3, 1); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType  (-1,-1, 3); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType  (-3, 1,-1); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType  (-1, 3,-1); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType  (-1, 1,-3); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType  ( 3,-1,-1); ++vi;
 ivp[10]=&*vi;(*vi).P()=CoordType ( 1,-3,-1); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType ( 1,-1,-3);

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

 ivp[0]=&*vi;(*vi).P()=CoordType  ( 1, 1, 0); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType  ( 1,-1, 0); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType  (-1,-1, 0); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType  (-1, 1, 0); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType  ( 1, 0, 1); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType  (-1, 0, 1); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType  (-1, 0,-1); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType  ( 1, 0,-1); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType  ( 0, 1, 1); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType  ( 0, 1,-1); ++vi;
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
 ivp[10]=&*vi;(*vi).P()=CoordType ( 1, 2, 0); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType ( 1,-2, 0); ++vi;
 ivp[12]=&*vi;(*vi).P()=CoordType (-1, 0, 2); ++vi;
 ivp[13]=&*vi;(*vi).P()=CoordType (-1, 0,-2); ++vi;
 ivp[14]=&*vi;(*vi).P()=CoordType (-1, 2, 0); ++vi;
 ivp[15]=&*vi;(*vi).P()=CoordType (-1,-2, 0); ++vi;
 ivp[16]=&*vi;(*vi).P()=CoordType ( 2, 0, 1); ++vi;
 ivp[17]=&*vi;(*vi).P()=CoordType ( 2, 0,-1); ++vi;
 ivp[18]=&*vi;(*vi).P()=CoordType (-2, 0, 1); ++vi;
 ivp[19]=&*vi;(*vi).P()=CoordType (-2, 0,-1); ++vi;
 ivp[20]=&*vi;(*vi).P()=CoordType ( 2, 1, 0); ++vi;
 ivp[21]=&*vi;(*vi).P()=CoordType ( 2,-1, 0); ++vi;
 ivp[22]=&*vi;(*vi).P()=CoordType (-2, 1, 0); ++vi;
 ivp[23]=&*vi;(*vi).P()=CoordType (-2,-1, 0);

 FaceIterator fi=in.face.begin();
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[10];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[4];  (*fi).V(1)=ivp[5];  (*fi).V(2)=ivp[14];  ++fi;
 (*fi).V(0)=ivp[3];  (*fi).V(1)=ivp[13];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[1];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[17];  (*fi).V(2)=ivp[20];  ++fi;
 (*fi).V(0)=ivp[20];  (*fi).V(1)=ivp[16];  (*fi).V(2)=ivp[21];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[8];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[12];  (*fi).V(2)=ivp[2];  ++fi;
 (*fi).V(0)=ivp[23];  (*fi).V(1)=ivp[18];  (*fi).V(2)=ivp[22];  ++fi;
 (*fi).V(0)=ivp[22];  (*fi).V(1)=ivp[19];  (*fi).V(2)=ivp[23];  ++fi;
 (*fi).V(0)=ivp[11];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[15];  ++fi;
 (*fi).V(0)=ivp[15];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[11];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[12];  (*fi).V(2)=ivp[18];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[18];  (*fi).V(2)=ivp[23];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[23];  (*fi).V(2)=ivp[15];  ++fi;
 (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[15];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[7];  (*fi).V(1)=ivp[15];  (*fi).V(2)=ivp[23];  ++fi;
 (*fi).V(0)=ivp[7];  (*fi).V(1)=ivp[23];  (*fi).V(2)=ivp[19];  ++fi;
 (*fi).V(0)=ivp[7];  (*fi).V(1)=ivp[19];  (*fi).V(2)=ivp[13];  ++fi;
 (*fi).V(0)=ivp[7];  (*fi).V(1)=ivp[13];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[7];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[7];  (*fi).V(2)=ivp[3];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[3];  (*fi).V(2)=ivp[9];  ++fi;
 (*fi).V(0)=ivp[21];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[17];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[0];  (*fi).V(2)=ivp[4];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[14];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[14];  (*fi).V(2)=ivp[22];  ++fi;
 (*fi).V(0)=ivp[12];  (*fi).V(1)=ivp[22];  (*fi).V(2)=ivp[18];  ++fi;
 (*fi).V(0)=ivp[16];  (*fi).V(1)=ivp[20];  (*fi).V(2)=ivp[10];  ++fi;
 (*fi).V(0)=ivp[16];  (*fi).V(1)=ivp[10];  (*fi).V(2)=ivp[4];  ++fi;
 (*fi).V(0)=ivp[16];  (*fi).V(1)=ivp[4];  (*fi).V(2)=ivp[0];  ++fi;
 (*fi).V(0)=ivp[16];  (*fi).V(1)=ivp[0];  (*fi).V(2)=ivp[8];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[20];  (*fi).V(2)=ivp[17];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[17];  (*fi).V(2)=ivp[9];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[9];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[10];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[5];  ++fi;
 (*fi).V(0)=ivp[14];  (*fi).V(1)=ivp[5];  (*fi).V(2)=ivp[1];  ++fi;
 (*fi).V(0)=ivp[14];  (*fi).V(1)=ivp[1];  (*fi).V(2)=ivp[13];  ++fi;
 (*fi).V(0)=ivp[14];  (*fi).V(1)=ivp[13];  (*fi).V(2)=ivp[19];  ++fi;
 (*fi).V(0)=ivp[14];  (*fi).V(1)=ivp[19];  (*fi).V(2)=ivp[22];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[2];  (*fi).V(2)=ivp[6];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[6];  (*fi).V(2)=ivp[11];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[11];  (*fi).V(2)=ivp[21];  ++fi;
 (*fi).V(0)=ivp[8];  (*fi).V(1)=ivp[21];  (*fi).V(2)=ivp[16];

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

 /*
 for(int i=0, x=0; i<2; i++)
     for(int j=0; j<2; j++)
         for(int k=0; k<2; k++) {
            ivp[x++]=&*vi;(*vi).P()=CoordType  ( (-1)^i*1, (-1)^j*1, (-1)^k*E); ++vi;
            ivp[x++]=&*vi;(*vi).P()=CoordType  ( (-1)^i*1, (-1)^j*E, (-1)^k*1); ++vi;
            ivp[x++]=&*vi;(*vi).P()=CoordType  ( (-1)^i*E, (-1)^j*1, (-1)^k*1); ++vi;
         }
 */

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
 ivp[10]=&*vi;(*vi).P()=CoordType (-1,-1, E); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType (-1,-1,-E); ++vi;

 ivp[12]=&*vi;(*vi).P()=CoordType (-1, E, 1); ++vi;
 ivp[13]=&*vi;(*vi).P()=CoordType (-1, E,-1); ++vi;
 ivp[14]=&*vi;(*vi).P()=CoordType (-1,-E, 1); ++vi;
 ivp[15]=&*vi;(*vi).P()=CoordType (-1,-E,-1); ++vi;

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
    // 12 squares, 8 hexagons and 6 octagons
    /* F = 26, V = 48
       Perm(±1, ±(1+√2), ±(1+√8))
    */
}


template <class MeshType>
void Snub_Cube(MeshType &in)
{
    // 32 triangles and 6 squares
    /* F = 38, V = 24
       Perm(±1, ±ξ, ±1/ξ)
       where ξ3+ξ2+ξ=1
    */

}


template <class MeshType>
void Icosidodecahedron(MeshType &in)
{
    // 20 triangles and 12 pentagons
    /* F = 32, V = 30
       (0,0,±τ), (±1/2, ±τ/2, ±(1+τ)/2)
       where τ is the golden ratio, (1+√5)/2
    */

}


template <class MeshType>
void Truncated_Dodecahedron(MeshType &in)
{
    // 20 triangles and 12 decagons
    /* F = 32, V = 60
        (0, ±1/τ, ±(2+τ))
        (±(2+τ), 0, ±1/τ)
        (±1/τ, ±(2+τ), 0)
        (±1/τ, ±τ, ±2τ)
        (±2τ, ±1/τ, ±τ)
        (±τ, ±2τ, ±1/τ)
        (±τ, ±2, ±τ2)
        (±τ2, ±τ, ±2)
        (±2, ±τ2, ±τ)
        where τ = (1+√5)/2 is the golden ratio
    */

}

// (soccer ball, for friends)
template <class MeshType>
void Truncated_Icosahedron(MeshType &in)
{
    // 12 pentagons and 20 hexagons
    /* F = 32, V = 60
        (0,±1,±3φ), (±1,±3φ,0), (±3φ,0,±1)
        (±2,±(1+2φ),±φ), (±(1+2φ),±φ,±2), (±φ,±2,±(1+2φ))
        (±1,±(2+φ),±2φ), (±(2+φ),±2φ,±1), (±2φ,±1,±(2+φ))
        where φ = (1+√5)/2
    */

 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 double G = (1 + sqrt(5))/2;
 //double G3 = 3*G; double G2 = 2*G; double A = 1+G2;

 in.Clear();
 Allocator<MeshType>::AddVertices(in, 60);
 Allocator<MeshType>::AddFaces(in, 12*3+20*4); //116 faces!!!

 int pentagons[5*12]= {
     16,  6, 17, 49, 48,        0, 28, 36, 40, 32,
     39, 43, 35,  3, 31,       55, 53, 21,  9, 23,
     15, 47, 46, 14,  5,       29,  1, 33, 41, 37,
     24, 10, 26, 58, 56,       22,  8, 20, 52, 54,
     42, 38, 30,  2, 34,       19,  7, 18, 50, 51,
     59, 27, 11, 25, 57,       44, 45, 13,  4, 12 };

 int hexagons[20*6]= {
      3,  1, 29, 53, 55, 31,        56, 32, 40, 16, 48, 24,
     24, 48, 49, 25, 11, 10,        2,  0, 32, 56, 58, 34,
     18, 42, 34, 58, 26, 50,        43, 19, 51, 27, 59, 35,
     49, 17, 41, 33, 57, 25,        21, 53, 29, 37, 13, 45,
     52, 20, 44, 12, 36, 28,        35, 59, 57, 33,  1,  3,
     15, 39, 31, 55, 23, 47,        46, 47, 23,  9,  8, 22,
     38, 14, 46, 22, 54, 30,         4, 13, 37, 41, 17,  6,
     12,  4,  6, 16, 40, 36,        54, 52, 28,  0,  2, 30,
      8,  9, 21, 45, 44, 20,         7,  5, 14, 38, 42, 18,
     43, 39, 15,  5,  7, 19,        10, 11, 27, 51, 50, 26 };

 VertexPointer ivp[60];
 VertexIterator vi=in.vert.begin();

 ivp[0]=&*vi;(*vi).P()=CoordType   ( 0, 1, 3*G); ++vi;
 ivp[1]=&*vi;(*vi).P()=CoordType   ( 0, 1,-3*G); ++vi;
 ivp[2]=&*vi;(*vi).P()=CoordType   ( 0,-1, 3*G); ++vi;
 ivp[3]=&*vi;(*vi).P()=CoordType   ( 0,-1,-3*G); ++vi;
 ivp[4]=&*vi;(*vi).P()=CoordType   ( 1, 3*G, 0); ++vi;
 ivp[5]=&*vi;(*vi).P()=CoordType   ( 1,-3*G, 0); ++vi;
 ivp[6]=&*vi;(*vi).P()=CoordType   (-1, 3*G, 0); ++vi;
 ivp[7]=&*vi;(*vi).P()=CoordType   (-1,-3*G, 0); ++vi;
 ivp[8]=&*vi;(*vi).P()=CoordType   ( 3*G, 0, 1); ++vi;
 ivp[9]=&*vi;(*vi).P()=CoordType   ( 3*G, 0,-1); ++vi;
 ivp[10]=&*vi;(*vi).P()=CoordType  (-3*G, 0, 1); ++vi;
 ivp[11]=&*vi;(*vi).P()=CoordType  (-3*G, 0,-1); ++vi;
 ivp[12]=&*vi;(*vi).P()=CoordType  ( 2, 1+2*G, G); ++vi;
 ivp[13]=&*vi;(*vi).P()=CoordType  ( 2, 1+2*G,-G); ++vi;
 ivp[14]=&*vi;(*vi).P()=CoordType  ( 2,-1-2*G, G); ++vi;
 ivp[15]=&*vi;(*vi).P()=CoordType  ( 2,-1-2*G,-G); ++vi;
 ivp[16]=&*vi;(*vi).P()=CoordType  (-2, 1+2*G, G); ++vi;
 ivp[17]=&*vi;(*vi).P()=CoordType  (-2, 1+2*G,-G); ++vi;
 ivp[18]=&*vi;(*vi).P()=CoordType  (-2,-1-2*G, G); ++vi;
 ivp[19]=&*vi;(*vi).P()=CoordType  (-2,-1-2*G,-G); ++vi;
 ivp[20]=&*vi;(*vi).P()=CoordType  ( 1+2*G, G, 2); ++vi;
 ivp[21]=&*vi;(*vi).P()=CoordType  ( 1+2*G, G,-2); ++vi;
 ivp[22]=&*vi;(*vi).P()=CoordType  ( 1+2*G,-G, 2); ++vi;
 ivp[23]=&*vi;(*vi).P()=CoordType  ( 1+2*G,-G,-2); ++vi;
 ivp[24]=&*vi;(*vi).P()=CoordType  (-1-2*G, G, 2); ++vi;
 ivp[25]=&*vi;(*vi).P()=CoordType  (-1-2*G, G,-2); ++vi;
 ivp[26]=&*vi;(*vi).P()=CoordType  (-1-2*G,-G, 2); ++vi;
 ivp[27]=&*vi;(*vi).P()=CoordType  (-1-2*G,-G,-2); ++vi;
 ivp[28]=&*vi;(*vi).P()=CoordType  ( G, 2, 1+2*G); ++vi;
 ivp[29]=&*vi;(*vi).P()=CoordType  ( G, 2,-1-2*G); ++vi;
 ivp[30]=&*vi;(*vi).P()=CoordType  ( G,-2, 1+2*G); ++vi;
 ivp[31]=&*vi;(*vi).P()=CoordType  ( G,-2,-1-2*G); ++vi;
 ivp[32]=&*vi;(*vi).P()=CoordType  (-G, 2, 1+2*G); ++vi;
 ivp[33]=&*vi;(*vi).P()=CoordType  (-G, 2,-1-2*G); ++vi;
 ivp[34]=&*vi;(*vi).P()=CoordType  (-G,-2, 1+2*G); ++vi;
 ivp[35]=&*vi;(*vi).P()=CoordType  (-G,-2,-1-2*G); ++vi;
 ivp[36]=&*vi;(*vi).P()=CoordType  ( 1, 2+G, 2*G); ++vi;
 ivp[37]=&*vi;(*vi).P()=CoordType  ( 1, 2+G,-2*G); ++vi;
 ivp[38]=&*vi;(*vi).P()=CoordType  ( 1,-2-G, 2*G); ++vi;
 ivp[39]=&*vi;(*vi).P()=CoordType  ( 1,-2-G,-2*G); ++vi;
 ivp[40]=&*vi;(*vi).P()=CoordType  (-1, 2+G, 2*G); ++vi;
 ivp[41]=&*vi;(*vi).P()=CoordType  (-1, 2+G,-2*G); ++vi;
 ivp[42]=&*vi;(*vi).P()=CoordType  (-1,-2-G, 2*G); ++vi;
 ivp[43]=&*vi;(*vi).P()=CoordType  (-1,-2-G,-2*G); ++vi;
 ivp[44]=&*vi;(*vi).P()=CoordType  ( 2+G, 2*G, 1); ++vi;
 ivp[45]=&*vi;(*vi).P()=CoordType  ( 2+G, 2*G,-1); ++vi;
 ivp[46]=&*vi;(*vi).P()=CoordType  ( 2+G,-2*G, 1); ++vi;
 ivp[47]=&*vi;(*vi).P()=CoordType  ( 2+G,-2*G,-1); ++vi;
 ivp[48]=&*vi;(*vi).P()=CoordType  (-2-G, 2*G, 1); ++vi;
 ivp[49]=&*vi;(*vi).P()=CoordType  (-2-G, 2*G,-1); ++vi;
 ivp[50]=&*vi;(*vi).P()=CoordType  (-2-G,-2*G, 1); ++vi;
 ivp[51]=&*vi;(*vi).P()=CoordType  (-2-G,-2*G,-1); ++vi;
 ivp[52]=&*vi;(*vi).P()=CoordType  ( 2*G, 1, 2+G); ++vi;
 ivp[53]=&*vi;(*vi).P()=CoordType  ( 2*G, 1,-2-G); ++vi;
 ivp[54]=&*vi;(*vi).P()=CoordType  ( 2*G,-1, 2+G); ++vi;
 ivp[55]=&*vi;(*vi).P()=CoordType  ( 2*G,-1,-2-G); ++vi;
 ivp[56]=&*vi;(*vi).P()=CoordType  (-2*G, 1, 2+G); ++vi;
 ivp[57]=&*vi;(*vi).P()=CoordType  (-2*G, 1,-2-G); ++vi;
 ivp[58]=&*vi;(*vi).P()=CoordType  (-2*G,-1, 2+G); ++vi;
 ivp[59]=&*vi;(*vi).P()=CoordType  (-2*G,-1,-2-G);

 FaceIterator fi=in.face.begin();

 for(int i=0; i<12; i++) {
     (*fi).V(0)=ivp[pentagons[i*5]];  (*fi).V(1)=ivp[pentagons[i*5+1]];  (*fi).V(2)=ivp[pentagons[i*5+2]]; ++fi;
     (*fi).V(0)=ivp[pentagons[i*5]];  (*fi).V(1)=ivp[pentagons[i*5+2]];  (*fi).V(2)=ivp[pentagons[i*5+3]]; ++fi;
     (*fi).V(0)=ivp[pentagons[i*5]];  (*fi).V(1)=ivp[pentagons[i*5+3]];  (*fi).V(2)=ivp[pentagons[i*5+4]]; ++fi;
 }

 for(int i=0; i<20; i++) {
     (*fi).V(0)=ivp[hexagons[i*6]];  (*fi).V(1)=ivp[hexagons[i*6+1]];  (*fi).V(2)=ivp[hexagons[i*6+2]]; ++fi;
     (*fi).V(0)=ivp[hexagons[i*6]];  (*fi).V(1)=ivp[hexagons[i*6+2]];  (*fi).V(2)=ivp[hexagons[i*6+3]]; ++fi;
     (*fi).V(0)=ivp[hexagons[i*6]];  (*fi).V(1)=ivp[hexagons[i*6+3]];  (*fi).V(2)=ivp[hexagons[i*6+4]]; ++fi;
     (*fi).V(0)=ivp[hexagons[i*6]];  (*fi).V(1)=ivp[hexagons[i*6+4]];  (*fi).V(2)=ivp[hexagons[i*6+5]]; ++fi;
 }

  if (in.HasPerFaceFlags()) {
    FaceIterator fi=in.face.begin();
    for (int k=0; k<116; k++) {
      (*fi).SetF(1); fi++;
    }
  }

}

template <class MeshType>
void Rhombicosidodecahedron(MeshType &in)
{
    /*
    */

}

template <class MeshType>
void Truncated_Icosidodecahedron(MeshType &in)
{
    /*
    */

}

template <class MeshType>
void Snub_Dodecahedron(MeshType &in)
{
    /*
    */

}

} // End Namespace TriMesh
} // End Namespace vcg

#endif // __VCGLIB_ARCHIMEDEAN_H
