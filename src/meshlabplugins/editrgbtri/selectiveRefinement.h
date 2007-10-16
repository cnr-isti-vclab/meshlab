#ifndef SELECTIVEREFINEMENT_H_
#define SELECTIVEREFINEMENT_H_

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
#include <queue>
#include <meshlab/meshmodel.h>
#include "rgbPrimitives.h"
#include "topologicalOp.h"
#include <vcg/space/box3.h> 

namespace rgbt
{
/// Contain a refining operation and a priority.
/**
 * A refining operation can contain only one index if point to a vertex, two otherwise */
struct RefOp
{
	/// RGB Triangle
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    /// RGB Vertex
    typedef RgbVertex<CMeshO> RgbVertexC;
	
    RefOp() {}
    /// Buind a new RefOp, reverse indicate how must be sorted by the operator<
    RefOp(int v1,int v2,double priority,bool reverse = true) // Costructor for refining (default is really important)
    {
    	this->v1 = v1;
    	this->v2 = v2;
    	this->priority = priority;
    	this->reverse = reverse;
    }
    /// Buind a new RefOp, reverse indicate how must be sorted by the operator<
    RefOp(int v1,double priority,bool reverse = false) // Costructor for coarsening (default is really important)
    {
    	this->v1 = v1;
    	this->priority = priority;
    	this->reverse = reverse;
    }
    
	double priority;
	int v1;
	int v2;
	bool reverse;
	/// Compare two refop by them priority
	friend bool operator<(const RefOp& x, const RefOp& y) 
	{
		assert(x.reverse == y.reverse);
		if (!x.reverse)
			return (x.priority > y.priority);
		else
			return (x.priority < y.priority);
	}
	
};
/// Class that implement two algorithm for selective refinement
/**
 * The simple algorithm is useful for debug
 * The complex algorithm is the same proposed on the paper
 */
class SelectiveRefinement
{

    /// The tetrahedral mesh type
    typedef CMeshO TriMeshType;
    /// The face type
    typedef TriMeshType::FaceType FaceType;
    /// The face pointer
    typedef FaceType* FacePointer;
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
	
	/// RGB Triangle
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    /// RGB Vertex
    typedef RgbVertex<CMeshO> RgbVertexC;
    /// Topological Operation Class
    typedef TopologicalOp<CMeshO,RgbInfo::VERTEXC,RgbInfo::FACEC > TopologicalOpC;
    /// A Point
    typedef vcg::Point3f Point;

public:
	SelectiveRefinement(CMeshO& m, RgbInfo& i, TopologicalOpC& to);
	virtual ~SelectiveRefinement();
	/// Start the process
	void start(bool interactive, bool simpleAlgo);
	/// Execute a single step
	bool step();
	/// Stop the execution
	void stop();
	/// Perform a step using the Simple Algorithm
	bool stepSimple();
	/// Perform a step using the Complex Algorithm
	bool stepComplex();
	/// Max level of edge inside the box
	int maxEdgeLevelInBox;
	/// Max level of edge outside the box
	int minEdgeLevel;
	/// Max lenght of edge inside the box
	double maxEdgeLenghtInBox;
	/// Max lenght of edge outside the box
	double maxEdgeLenght;
	/// Max number of triangles in the mesh
	int maxTriangles;
	
private:
	/// Init procedure for the simple algorithm
	void init_queues();
	/// Init procedure for the complex algorithm
	void init_queuesComplex();
	/// Init procedure (common)
	void init();
	/// Auxiliary funcion for stepComplex
	void stepComplex_aux(vector<RgbTriangleC>& vt);
	/// Compute the lenght of an edge
	double edgeLenght(RgbTriangleC& t, int index);
	/// Return if the edge need more refinement (used by simple algorithm)
	bool edgeNeedRefinement(RgbTriangleC& t, int EdgeIndex);
	/// Return if the vertex can be removed
	bool vertexCanBeRemoved(RgbTriangleC& t, int VertexIndex);
	/// Compute the priority of an edge (simple)
	double calculatePriorityEdge(RgbTriangleC& t, int EdgeIndex);
	/// Compute the priority of a vertex (simple)
	double calculatePriorityVertex(RgbTriangleC& t, int VertexIndex);
	/// Compute the priority of an edge (complex)
	double calculatePriorityEdgeComplex(RgbTriangleC& t, int EdgeIndex);
	/// Compute the priority of a vertex (complex)
	double calculatePriorityVertexComplex(RgbTriangleC& t, int VertexIndex);
	/// Check if an edge is valid (the two vertex are internal)
	bool IsValidEdge(int v1,int v2, RgbTriangleC* t = 0, int* ti = 0);
	/// Check if a vertex is valid
	bool IsValidVertex(int v, RgbTriangleC* t = 0, int* ti = 0);
	/// Check if p is inside box
	bool isInBox(Point& p);
	/// Check if the edge is completely inside the box
	bool isInBox(RgbTriangleC& t, int EdgeIndex);
	/// Check if the vertex is inside the box
	bool isInBoxV(RgbTriangleC& t, int EdgeIndex);
	/// Extract the lenght of the minimum edge incident at v
	double minEdge(RgbVertexC& v);
	/// Extract the lenght of the maximum edge incident at v
	double maxEdge(RgbVertexC& v);
	/// Extract the lenght of all the edges incident at v
	void VE(RgbVertexC& v, vector<double>& vv);
	/// Get the coordinates of the point
	Point& extractCoord(RgbTriangleC& t, int VertexIndex);
	/// Priority queue that contain refinement operation
	priority_queue<RefOp> refinementQueue;
	/// Priority queue that contain coarsening operation
	priority_queue<RefOp> coarseningQueue;
	/// Indicate if a start has been called not followed by a stop
	bool isRunning;
	/// Temporaty flag used by simple algorithm
	bool isCompletedCoarsening;
	/// Indicate the kind of algorithm used
	bool simpleAlgorithm;
	/// Box used during the refinement
	Box3f box;
	/// Topological object used
	TopologicalOpC* to;
	/// Mesh
	CMeshO* m;
	/// Rgb Information
	RgbInfo* info; 
};

}

#endif /*SELECTIVEREFINEMENT_H_*/
