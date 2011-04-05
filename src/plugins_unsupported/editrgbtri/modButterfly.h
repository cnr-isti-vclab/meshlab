#ifndef MODBUTTERFLY_H_
#define MODBUTTERFLY_H_

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


#include <vcg/space/point3.h>
#include "rgbInfo.h"
#include <common/meshmodel.h>
#include "rgbPrimitives.h"
#include "topologicalOp.h"

namespace rgbt
{
//! Contain all the function needed to compute the coordinate of the vertexes.
/** Is based on a modified butterfly subdivision adapted for the use on an rgb triangulation */
class ModButterfly
{
    /// The tetrahedral mesh type
    typedef CMeshO TriMeshType;
    /// The face type
    typedef TriMeshType::FaceType FaceType;
    /// The vertex type
    typedef FaceType::VertexType VertexType;
    /// The vertex type pointer
    typedef FaceType::VertexType* VertexPointer;
    /// The vertex iterator type
    typedef TriMeshType::VertexIterator VertexIterator;
    /// The tetra iterator type
    typedef TriMeshType::FaceIterator FaceIterator;
    /// The coordinate type
    typedef FaceType::VertexType::CoordType CoordType;
    /// The scalar type
    typedef TriMeshType::VertexType::ScalarType ScalarType;
    ///the container of tetrahedron type
    typedef TriMeshType::FaceContainer FaceContainer;
    ///the container of vertex type
    typedef TriMeshType::VertContainer VertContainer;
    ///half edge type
    typedef TriMeshType::FaceType::EdgeType EdgeType;
    /// vector of pos
    typedef std::vector<EdgeType> EdgeVec;
    /// Face Pointer
    typedef TriMeshType::FacePointer FacePointer;
    /// Edge defined by Face and Index
    typedef EdgeFI<FacePointer> EdgeFIType;
    /// Topological Operation Class
    typedef TopologicalOp<CMeshO,RgbInfo::VERTEXC,RgbInfo::FACEC > TopologicalOpC;
    /// A Point
    typedef vcg::Point3f Point;
	/// RGB Triangle
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    /// RGB Vertex
    typedef RgbVertex<CMeshO> RgbVertexC;
    /// Pos
    typedef vcg::face::Pos<FaceType> Pos;

public:
	
	/// Compute the initial vertex arity
	static void init(TriMeshType& m, RgbInfo& info);
	
	/// find the 4 vertexes of the stencil
	static void findInitialStencil(RgbTriangleC& t, int EdgeIndex,int level, TopologicalOpC& to, vector<RgbVertexC>* indexes = 0,vector<RgbVertexC>* firstVertexes = 0);
	
    //! Perform an edge split and set the final position of the new vertex
	/* Return true if on the current edge was performed only a topological split
	 * return false if a complete split (with update on rgb Info) was performed. the complete split is performed
	 * during the calculation og the stencil
	 */ 
    static bool doSplit(RgbTriangleC& fp, int EdgeIndex, int level, TopologicalOpC& to , vector<FacePointer> *vfp = 0);
	
    //! Perform an edge collapse
    static void doCollapse(RgbTriangleC& fp, int EdgeIndex, TopologicalOpC& to, Point3<ScalarType> *p = 0, vector<FacePointer> *vfp = 0);

    //! Rotate the pos of the specified angle
    static void rotate(RgbVertexC& v, Pos& pos,int angle);
    
    //! Move to the next point of the same level on the edge where the pos point
    static RgbVertexC move(RgbVertexC& v, Pos& pos, int level);
    
    //! Rotate the pos around v until a border edge is reached
    static void rotateUntilBorder(RgbVertexC& v, Pos& pos);
    
    //! Compute the position of the new vertex using the pattern for extraordinary vertexes.
    /* Pos points to the S0 vertex and it contains the common face incident in S0 and S1
     */
    static Point computeExtraordinary(RgbVertexC& v, Pos& pos);
    
    //! Compute a vector of the coefficient needed by computeExtraordinary
    static void computeExtraordinaryPattern(vector<double>& pattern, int k);
  
    //! Return the base arity (if lvl is 0 the value is stored in the vertex elsewhere return 6)
    static int baseArity(RgbVertexC& v);
    
private:
	/// Auxiliary function for doSplit. It search the vertex in a half of the stencil
	static void findHalfStencil(RgbVertexC& v, Pos& pos, vector<RgbVertexC>& stencil);
};

}

#endif /*MODBUTTERFLY_H_*/
