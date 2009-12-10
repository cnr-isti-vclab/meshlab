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

#ifndef __VCGLIB_KNOT_H
#define __VCGLIB_KNOT_H

#include <vcg/math/base.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/update/flag.h>

namespace vcg {
namespace tri {

// Preliminar, it only creates the vertexes
// It needs some kind of interface to convert from vertexes to a 3D tube
template <class MeshType>
void Torus_knot(MeshType &in, int p, int q, float prec)
{
 typedef typename MeshType::CoordType CoordType;
 typedef typename MeshType::VertexPointer  VertexPointer;
 typedef typename MeshType::VertexIterator VertexIterator;
 typedef typename MeshType::FaceIterator   FaceIterator;

 int n_vert = 2*M_PI*p/prec+1;
 std::cout << "Vertexes: " << n_vert << std::endl;

 in.Clear();
 Allocator<MeshType>::AddVertices(in, n_vert);
 Allocator<MeshType>::AddFaces(in, 0);

 VertexPointer ivp[n_vert];
 VertexIterator vi=in.vert.begin();
 
 double i, x, y, z;
 int j=0;
 for(i=0; i<=2*M_PI*p; i+=prec) {
	 x = (2 + cos(q*i/p))*cos(i);
         y = (2 + cos(q*i/p))*sin(i);
	 z = sin(q*i/p);
	 ivp[j]=&*vi; vi->P()=CoordType(x, y, z);
	 vi++; j++;
 }

}

} // End Namespace TriMesh
} // End Namespace vcg

#endif // __VCGLIB_KNOT_H
