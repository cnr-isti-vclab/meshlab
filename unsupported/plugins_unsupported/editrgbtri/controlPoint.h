#ifndef CONTROLPOINT_H_
#define CONTROLPOINT_H_

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
/** Is based on a loop subdivision adapted for the use on an rgb triangulation */
class ControlPoint
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

public:
	
	//ControlPoint(CMeshO& m, RgbInfo& i, TopologicalOpC& to);
	//virtual ~ControlPoint();
	/// Compute the Pinf of the base vertexes
	static void init(TriMeshType& m, RgbInfo& info);
	
	/// find the 4 vertexes of the stencil
    /**
     * firstVertex return the eventual point that can contribute to the Pinf of the newvertex
     */
	static void findInitialStencil(RgbTriangleC& t, int EdgeIndex,int level, TopologicalOpC& to, vector<RgbVertexC>* indexes = 0,vector<RgbVertexC>* firstVertexes = 0);
	/// Compute and assign PInf (it perform split on the green edge if necessary)
	static void assignPinf(RgbVertexC& v,bool initial = false);
	/// Recompute Pkl with k the minimal level of incident edges
	static Point computePkl(RgbVertexC& v,int kl);
	/// Compute alpha(k)
	static double alpha(int n);
	/// Compute alpha(k,n)
	static double gamma(int n,int k);
	/// compute Pl starting from the stencil
	/** vector must have size 4, first two vertexes are the two that incides
	 * with the edge that has to be splitted
	 */
	static Point computePl(int l, vector<RgbVertexC>& stencil);
	static Point computePlBoundary(int l,vector<RgbVertexC>& stencil);

	/// If necessary update the current vertex position
	/**
	 * It require Pinf for the current vertex
	 */
	static void updateP(RgbVertexC& v);
	/// Add the contibute of p at the vIndex vertexes
	static void addContribute(RgbVertexC& v,Point& p, bool update = true);
	/// Remove the contibute of p at the vIndex vertexes
	static void removeContribute(RgbVertexC& v,Point& p);
	
    //! Perform an edge split with the correct computation of the new vertex coordinates
	/* Return true if on the current edge was performed only a topological split
	 * return false if a complete split (with update on rgb Info) was performed. the complete split is performed
	 * during the calculation og the stencil
	 */ 
    static bool doSplit(RgbTriangleC& fp, int EdgeIndex, int level, TopologicalOpC& to , vector<FacePointer> *vfp = 0, RgbVertexC* vNewInserted = 0, vector<RgbVertexC>* vcont = 0, vector<RgbVertexC>* vupd = 0);
	
    //! Perform an edge collapse
    static void doCollapse(RgbTriangleC& fp, int EdgeIndex, TopologicalOpC& to, Point3<ScalarType> *p = 0, vector<FacePointer> *vfp = 0);
    //! Return the minimal level of incident edge
    static int minimalEdgeLevel(RgbVertexC& v);
    /// Search in VV relation all vertexes that contribute on Pinf of v
    static void searchContribute(RgbVertexC& v, bool update = true);
    /// Search in VV relation the two boundary vertexes that contribute on Pinf of v when v is on the Boundary
    static void searchContributeBoundary(RgbVertexC& v,bool update);
    /// Search in VV relation all vertexes that need contribution from v (is called once per vertex after Pinf become available)
    static void addPinfContributeToVV(RgbVertexC& v);
    
    //static void removeContributeToVV(RgbVertexC& v);
    /// If possible add the contribute of orig to dest. If Execute is equal to false do not perform the operation, only return true if possible false otherwise.
    static bool addContributeIfPossible(RgbVertexC& dest, RgbVertexC& orig, bool execute = true);
    /// Add the vertex orig to the taken list of dest and the vertex dest to the given list of orig
    static void addToLists(RgbVertexC& dest, RgbVertexC& orig);
    /// Remove the vertex orig to the taken list of dest and the vertex dest to the given list of orig
    static void removeFromLists(RgbVertexC& dest, RgbVertexC& orig);
    /// Clear the taken list and clear the given list(removing all the link to other vertexes)
    /**
     * l will contain a copy of the given list of v
     */
    static void listUpdateVertexRemoval(RgbVertexC& v, list<RgbVertexC>& l);
    /// Detach all the link when a vertex is removed (also clear the given and taken lists)  
    static void vertexRemovalUpdate(RgbVertexC& v);
    /// Clear the taken list removing all the links
    static void cleanTakenList(RgbVertexC& v);
    
    //! Count the number of incident edges (not use any relation if vertex level > 0)
    static unsigned int vertexRank(RgbVertexC& v);
};

}

#endif /*CONTROLPOINT_H_*/
