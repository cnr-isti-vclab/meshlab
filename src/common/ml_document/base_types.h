/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005 - 2020                                          \/)\/   *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
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

#ifndef MESHLAB_BASE_TYPES_H
#define MESHLAB_BASE_TYPES_H

#include <vcg/complex/complex.h>

#ifndef MESHLAB_SCALAR
#error "Fatal compilation error: MESHLAB_SCALAR must be defined"
#endif

typedef MESHLAB_SCALAR Scalarm;
typedef vcg::Point2<MESHLAB_SCALAR>     Point2m;
typedef vcg::Point3<MESHLAB_SCALAR>     Point3m;
typedef vcg::Point4<MESHLAB_SCALAR>     Point4m;
typedef vcg::Plane3<MESHLAB_SCALAR>     Plane3m;
typedef vcg::Segment2<MESHLAB_SCALAR>   Segment2m;
typedef vcg::Segment3<MESHLAB_SCALAR>   Segment3m;
typedef vcg::Box3<MESHLAB_SCALAR>       Box3m;
typedef vcg::Matrix44<MESHLAB_SCALAR>   Matrix44m;
typedef vcg::Matrix33<MESHLAB_SCALAR>   Matrix33m;
typedef vcg::Shot<MESHLAB_SCALAR>       Shotm;
typedef vcg::Similarity<MESHLAB_SCALAR> Similaritym;

template<typename T>
struct MeshLabScalarTest
{
};

template<>
struct MeshLabScalarTest<float>
{
	static const char* floatingPointPrecision() { return "fp"; }
	static const char* floatingPointPrecisionIOToken() { return "%f"; }
	static bool doublePrecision() { return false; }
};

template<>
struct MeshLabScalarTest<double>
{
	static const char* floatingPointPrecision() { return "dp"; }
	static const char* floatingPointPrecisionIOToken() { return "%lf"; }
	static bool doublePrecision() { return true; }
};

#endif // MESHLAB_BASE_TYPES_H
