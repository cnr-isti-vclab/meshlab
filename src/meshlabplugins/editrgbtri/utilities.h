#ifndef UTILITIES_H_
#define UTILITIES_H_

/****************************************************************************
* Rgb Triangulations Plugin                                                 *
*                                                                           *
* Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
* Copyright(C) 2007                                                         *
* DISI - Department of Computer Science                                     *
* University of Genova                                                      *
*                                                                           *
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
****************************************************************************/


#include <meshlab/meshmodel.h>
#include <assert.h>
#include "topologicalOp.h"
#include <vcg/space/point3.h>

namespace rgbt
{


//! Utilities used by the GUI
class Utilities
{

    
public:
	Utilities();
	virtual ~Utilities();
	
	typedef CMeshO::FacePointer FacePointer;
	typedef CMeshO::VertexPointer VertexPointer;
	
    typedef EdgeFI<FacePointer> EdgeFIType;
    typedef Point3<float> PointType;
    
	// Select face in relation FF with fp
	static void HighlightFF(FacePointer fp);
	// Select face in relation VF with vp using a VFIterator
	static void HighlightVF(VertexPointer vp);
    // Select face in relation VF with vp using a Pos
    static void HighlightVFPos(FacePointer fp, int i);
	
    // Edge Collapse (only topological)
    static void doCollapse(CMeshO &m, EdgeFIType & c, const PointType &p);
    // Edge Split (only topological)
    static void doSplit(CMeshO &m, EdgeFIType & c, const PointType &p);

    // Edge Collapse
    static void doRgbCollapse(CMeshO &m, EdgeFIType & c, const PointType &p);
    // Edge Split
    static void doRgbSplit(CMeshO &m, EdgeFIType & c, const PointType &p);
    
    // Extract the common edge between 2 triangles
    static bool commonEdge(CMeshO::FacePointer fp1, CMeshO::FacePointer fp2, Utilities::EdgeFIType* edge = 0);
    // Extract the common vertex
    static bool commonVertex(vector<FacePointer> fc, Utilities::EdgeFIType* vert = 0);
};

}

#endif /*UTILITIES_H_*/
