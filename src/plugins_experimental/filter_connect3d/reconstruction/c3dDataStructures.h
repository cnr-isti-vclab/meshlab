/*
 * Connect3D - Reconstruction of water-tight triangulated meshes from unstructured point clouds
 *
 * please credit the following article: Stefan Ohrhallinger, Sudhir Mudur and Michael Wimmer:
 * 'Minimizing Edge Length to Connect Sparsely Sampled Unstructured Point Sets',
 * Shape Modeling International 2013, published in Computers & Graphics Journal, 2013.
 *
 * Copyright (C) 2013 Stefan Ohrhallinger, Daniel Prieler
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once


#define SQR(a) ((a)*(a))
#define MIN(a, b) (((a) < (b)) ? a : b)
#define MAX(a, b) (((a) > (b)) ? a : b)


#define COUT_CELL(c) (c)->vertex(0)->info().index << "/" << (c)->vertex(1)->info().index << "/" << (c)->vertex(2)->info().index << "/" << (c)->vertex(3)->info().index
#define COUT_HT(ht) ((ht).first)->vertex(tetraTriVertexIndices[(ht).second][0])->info().index << "/" << ((ht).first)->vertex(tetraTriVertexIndices[(ht).second][1])->info().index << "/" << ((ht).first)->vertex(tetraTriVertexIndices[(ht).second][2])->info().index
#define COUT_HE(he) ((he).first)->vertex((he).second)->info().index << "-" << ((he).first)->vertex((he).third)->info().index
#define IS_FINITE(c) (!dt->is_infinite((c)->vertex(0)) && !dt->is_infinite((c)->vertex(1)) && !dt->is_infinite((c)->vertex(2)) && !dt->is_infinite((c)->vertex(3)))


typedef std::pair<int, int> IntPair;
typedef std::pair<IntPair, int> IntTriple;


class Triangle;
struct Tetrahedron;
class Vector3D;

typedef enum { NC_CONFORM, NC_VERTEXCONNECTED, NC_ISOLATED, NC_NONPLANAR } NCType;

// indices of edges start at lowest vertex index of triangle, ccw at halftriangles inside tetrahedron
const int tetraTriVertexIndices[4][3] = { { 1, 3, 2 }, { 0, 2, 3 }, { 0, 3, 1 }, { 0, 1, 2 } };
const int tetraTriEdgeIndices[4][3] = { { 5, 1, 4 }, { 5, 3, 2 }, { 4, 0, 3 }, { 1, 2, 0 } };	// guarantees that edges with same index as vertex in triangle are opposed
const int tetraEdgeVertexIndices[6][2] = { { 0, 1 }, { 1, 2 }, { 0, 2 }, { 0, 3 }, { 1, 3 }, { 2, 3 } };	// indexes edge index to (sorted) vertex pair


struct Vertex
{
	int index;
	NCType ncType;
	bool isOpen;

	Vertex()
	{
		index = -1;
		isOpen = false;
		ncType = NC_CONFORM;
	}

	inline bool isNC()
	{
		return (ncType != NC_CONFORM);
	}
};

struct Tetrahedron
{
	int label;
	Triangle *triangles[4];

	Tetrahedron()
	{
		label = -1;

		for (int i = 0; i < 4; i++)
			triangles[i] = nullptr;
	}

	inline void setTriangle(int index, Triangle *t)
	{
		triangles[index] = t;
	}

	inline Triangle *triangle(int index)
	{
		return triangles[index];
	}
};

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_3<Vertex, K> Vb;
typedef CGAL::Triangulation_cell_base_with_info_3<Tetrahedron, K> Cb;
typedef CGAL::Triangulation_data_structure_3<Vb, Cb> Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds> Dt;
typedef Dt::Cell_handle Cell;
typedef Dt::Facet DTriangle;
typedef Dt::Edge DEdge;
typedef Dt::Vertex_handle DVertex;
typedef Dt::Point Point;

struct Edge;

/*
 * returns triangle for halftriangle
 */
Triangle *ht2Triangle(DTriangle ht);

/*
 * returns edge for halfedge
 */
Edge *he2Edge(DEdge he);

/*
 * determines halfedge in other tetrahedron
 */
DEdge heInCell(Cell cell, DVertex v0, DVertex v1);

/*
 * determines halfedge in other tetrahedron
 */
DEdge heInCell(Cell cell, DEdge he);

/*
 * tests if tetrahedron consists of these vertices' indices
 */
bool cellEquals(Cell ch, int *vArray);

/*
 * locate and reference given halfedge in new cell
 * condition: vertices of halfedge must be contained in new cell
 */
void locateHEInCell(DEdge &he, Cell newCH);

/*
 * locate and reference given halfedge in halftriangle
 * condition: vertices of halfedge must be contained in halftriangle
 */
void locateHEInHT(DEdge &he, DTriangle ht);

/*
 * returns remaining index in tetrahedon
 * condition: all indices are in [0..3]
 */
int determineRemainingIndex(int *indices);


struct Edge
{
	Cell _cell;
	int _cellIndex[2];
	bool _isOpen, _isNC;

	Edge() {_isOpen = false; _isNC = false; _cellIndex[0] = 0; _cellIndex[1] = 0;}

	Edge(Cell cell, int index0, int index1);

	/*
	 * return a Delaunay halfedge for this edge
	 */
	DEdge dEdge();

	/*
	 * return Delaunay vertex
	 */
	const DVertex vertex(int index);

	/*
	 * return index of Delaunay vertex
	 * returns 2 if not in edge
	 */
	int index(DVertex vh);

	/*
	 * returns if edge is finite
	 */
	bool isFinite(Dt *delaunayTri);

	/*
	 * set edge classification
	 */
	void setOpen(bool isOpen);

	/*
	 * get edge classification
	 */
	bool isOpen();

	/*
	 * set edge classification
	 */
	void setNC(bool isNC);

	/*
	 * get edge classification
	 */
	bool isNC();

	/*
	 * return if edge exists (in triangulation), by having incident triangles
	 */
	bool exists(Dt *delaunayTri);

	/*
	 * output edge as v0-v1
	 */
	friend std::ostream& operator<<(std::ostream& os, const Edge &edge);

	/*
	 * return index as int pair
	 */
	IntPair index();

	/*
	 * test if it contains these vertices
	 */
	bool equals(int v0, int v1);

	/*
	 * return length of edge
	 */
	float length(Vector3D *vertexData);

};





class Triangle
{
private:
	bool _exists;
	bool _isOpen;
	int _label;	// label of CC
	Edge *_edge[3];
	Cell _cell;
	int _indexInCell;	// index of its opposite vertex in cell

public:
	Triangle(Cell cell, int index);

	/*
	 * return index of triangle in cell
	 */
    int indexInCell();

	/*
	 * return index of given vertex (equal to index of opposite edge)
	 * returns 3 if not in triangle
	 */
	int index(DVertex currVertex);

	/*
	 * return index of given vertex (equal to index of opposite edge)
	 * returns 3 if not in triangle
	 */
	int index(Cell cell, DVertex currVertex);

	/*
	 * set existence state of triangle
	 */
	void setExists(bool exists);

	/*
	 * return if triangle exists
	 */
	bool exists();

	/*
	 * return Delaunay vertex
	 */
	const DVertex vertex(int index);

	/*
	 * return Delaunay vertex
	 */
	const DVertex vertex(Cell cell, int index);

	/*
	 * set edge at given index
	 */
	void setEdge(int index, Edge *edge);

	/*
	 * get edge for given index
	 * NOTE: triangle is not oriented
	 */
	Edge *edge(int index);

	/*
	 * return index of edge in triangle [0..2], 3 if it is not contained in triangle
	 */
	int index(Edge *edge);

	/*
	 * get edge for given index, for orientation of triangle in the cell
	 */
	Edge *edge(Cell cell, int index);

	/*
	 * returns if triangle is finite
	 */
    bool isFinite(Dt *delaunayTri);

	/*
	 * set edge classification
	 */
	void setOpen(bool isOpen);

	/*
	 * get edge classification
	 */
	bool isOpen();

	/*
	 * set label
	 */
	void setLabel(int label);

	/*
	 * get label
	 */
	int label();

	/*
	 * returns one adjacent cell
	 */
	const Cell cell();

	/*
	 * return index as int triple
	 */
	const IntTriple index();

	/*
	 * returns a halftriangle for the triangle
	 */
	DTriangle halftriangle();

	/*
	 * output triangle as v0/v1/v2
	 */
	friend std::ostream& operator<<(std::ostream& os, const Triangle &triangle);


	/*
	 * test if triangle contains exactly these vertex indices
	 */
	bool equals(int *vArray);
};


/*
 * encapsulates a halftriangle and one of its halfedges
 */
class SBElem
{
private:
	Cell ch;
	int vIndex, eIndex[2];

public:
	SBElem();
	SBElem(DTriangle ht, DEdge he);

	/*
	 * return Delaunay halfedge
	 */
	DEdge edge() const;

	/*
	 * return Delaunay halftriangle
	 */
	DTriangle triangle();

    /*
     * return cell of the SB element
     */
    Cell getCell();


	bool equals(SBElem elem);

	/*
	 * determine other (next) incident edge of triangle
	 */
	void nextEdgeInHT();

	/*
	 * reverse orientation of halfedge, by swapping its vertex indices
	 */
	void reverseHE();

	/*
	 * reverse orientation of halftriangle
	 */
	void reverseHT(Dt *delaunayTri);

	/*
	 * switch halfedge to next one in order in triangle
	 */
	void nextHEInHT();
};


typedef struct std::pair<IntPair, SBElem> ElemPair;

struct SeparatorBoundary;

typedef struct
{
	btree::btree_map<int, SeparatorBoundary *> sbMap;
	btree::btree_set<DVertex> intBVertexSet;	// set of vertices interior to the (triangle set) boundary of the region of the B_e(CC) limited by the SBGroup
	btree::btree_set<DTriangle> boundaryHTSet;	// set of triangles in B_e(CC), for this side of the separator boundary (the cover)
} SBGroup;

struct CC;

struct SeparatorBoundary
{
	int label;
	std::list<SBElem> boundary;
	std::set<DVertex> sepBVertexSet;	// set of vertices in SB
	SBGroup *sbGroup;	// group of joined SBs (may consist of only one SB)
	CC *cc;
	bool isHoleAdjacent;

	SeparatorBoundary()
	{
		label = -1;
		sbGroup = NULL;
		cc = NULL;
		isHoleAdjacent = false;
	}

	/*
	 * returns iterator to element in boundary
	 */
	std::list<SBElem>::iterator find(SBElem elem);
};

struct CC
{
	btree::btree_set<SeparatorBoundary *> /*insideSet, */outsideSet;	// only outside used - ?
	btree::btree_set<DVertex> interiorVertexSet;
};

typedef struct
{
	std::list<CC *> ccList;
	std::list<SBElem> boundary[2];	// list, but elements are not ordered
} OCBoundary;

typedef struct
{
	std::set<OCBoundary *> ocbSet;
} OC;


typedef struct
{
	bool isDC;	// bounds a DC (else a PC)
	bool isGenus0;	// can only be false if a PC
	std::list<SBElem> boundary[2];
	std::set<Edge *> treeEdgeSet;	// tree edges (part of EB if bounding a PC)
} EdgeBoundary;

typedef struct
{
	std::list<EdgeBoundary> ebs;
	std::set<DTriangle> htSet[2], boundaryHTSet;
	bool sideInwards[2];
} MC;


struct c3dModelData
{
	std::unique_ptr<Dt> dt;
	std::vector<Triangle> dtTriangles;
	std::vector<Edge> dtEdgeList;

	std::set<Triangle *> colorTriangles;

	std::unique_ptr<Vector3D []> vertices;

	size_t numVertices;
	int criterionType;

	c3dModelData();
	~c3dModelData();

};

