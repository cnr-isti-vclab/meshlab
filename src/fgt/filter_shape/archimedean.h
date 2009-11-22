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

}

template <class MeshType>
void Cuboctahedron(MeshType &in)
{
    //F = 14, V = 12
    /*
        (±1,±1,0)
        (±1,0,±1)
        (0,±1,±1)
    */
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
