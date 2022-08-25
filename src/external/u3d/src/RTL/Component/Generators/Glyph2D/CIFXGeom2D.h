//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	CIFXGeom2D.h

			This header defines some basic 2D geometry structures.

	@note	This code in this module is a modification of the code from the 
			article "Incremental Delaunay Triangulation" by Dani Lischinski 
			(danix@graphics.cornell.edu) in "Graphics Gems IV", Academic 
			Press, 1994, p. 47-59, code: p. 51-58.  The actual source is 
			available from www.acm.org/pubs/tog/GraphicsGems/gemsiv.zip 
			in the "delaunay" folder.  The associated website's root address 
			is www.graphicsgems.org.  It states that the Graphics Gems code 
			can be used without restrictions.
*/


#ifndef CIFXGEOM2D_H
#define CIFXGEOM2D_H


#include "predicates.h"


struct SIFXPoint2d {
public:
	REAL x, y;
	I32 index;
	SIFXPoint2d()					{ x = 0; y = 0; index=-1;}
	SIFXPoint2d(REAL a, REAL b)		{ x = a; y = b; index=-1;}
	SIFXPoint2d(const SIFXPoint2d& p)	{ *this = p; }
};


inline int operator==(const SIFXPoint2d& p, const SIFXPoint2d& q) 
{
	return (q.x==p.x && q.y==p.y);
}

struct SIFXTriangle
{
	SIFXPoint2d a;
	SIFXPoint2d b;
	SIFXPoint2d c;
};


struct SIFXIndexTriangle
{
	U32 ia;
	U32 ib;
	U32 ic;
};

struct SIFXEdge
{
	SIFXPoint2d a;
	SIFXPoint2d b;
};

#endif
