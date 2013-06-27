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

#include "precompiled.h"

#include "c3dDataStructures.h"



Edge::Edge(Cell cell, int index0, int index1)
{
	_isOpen = false;
	_isNC = false;
	_cell = cell;
	_cellIndex[0] = index0;
	_cellIndex[1] = index1;
}

/*
	* return a Delaunay halfedge for this edge
	*/
DEdge Edge::dEdge()
{
	return DEdge(_cell, _cellIndex[0], _cellIndex[1]);
}

/*
	* return Delaunay vertex
	*/
const DVertex Edge::vertex(int index)
{
	return _cell->vertex(_cellIndex[index]);
}

/*
	* return index of Delaunay vertex
	* returns 2 if not in edge
	*/
int Edge::index(DVertex vh)
{
	int i = 0;

	while ((i < 2) && (vertex(i) != vh))
		i++;

	return i;
}

/*
	* returns if edge is finite
	*/
bool Edge::isFinite(Dt *delaunayTri)
{
	int i = 0;

	while ((i < 2) && !delaunayTri->is_infinite(vertex(i)))
		i++;

	return (i == 2);
}

/*
	* set edge classification
	*/
void Edge::setOpen(bool isOpen)
{
	_isOpen = isOpen;
}

/*
	* get edge classification
	*/
bool Edge::isOpen()
{
	return _isOpen;
}

/*
	* set edge classification
	*/
void Edge::setNC(bool isNC)
{
	_isNC = isNC;
}

/*
	* get edge classification
	*/
bool Edge::isNC()
{
	return _isNC;
}


/*
	* return if edge exists (in triangulation), by having incident triangles
	*/
bool Edge::exists(Dt *delaunayTri)
{
	// iterate over all incident triangles, or until an existing one found
	Dt::Facet_circulator startFC = delaunayTri->incident_facets(dEdge());
	Dt::Facet_circulator currFC = startFC;
	Triangle *currTri;

	do
	{
		DTriangle currDT = *currFC;
		currTri = ht2Triangle(currDT);
		currFC++;
	} while ((currFC != startFC) && !currTri->exists());

	return currTri->exists();
}

/*
	* output edge as v0-v1
	*/
std::ostream& operator<<(std::ostream& os, const Edge &edge)
{
	os << edge._cell->vertex(edge._cellIndex[0])->info().index << "-" << edge._cell->vertex(edge._cellIndex[1])->info().index;

	return os;
}

/*
* return index as int pair
*/
IntPair Edge::index()
{
	return IntPair(vertex(0)->info().index, vertex(1)->info().index);
}

/*
* test if it contains these vertices
*/
bool Edge::equals(int v0, int v1)
{
	IntPair pair = index();

	return ((pair.first == v0) && (pair.second == v1)) || ((pair.first == v1) && (pair.second == v0));
}

/*
	* return length of edge
	*/
float Edge::length(Vector3D *vertexData)
{
	return Vector3D::distance(vertexData[vertex(0)->info().index], vertexData[vertex(1)->info().index]);
}



Triangle::Triangle(Cell cell, int index)
{
	_exists = false;
	_isOpen = false;
	_label = -1;

	for (int i = 0; i < 3; i++)
		_edge[i] = NULL;

	_cell = cell;
	_indexInCell = index;
}

/*
	* return index of triangle in cell
	*/
int Triangle::indexInCell()
{
	return _indexInCell;
}

/*
	* return index of given vertex (equal to index of opposite edge)
	* returns 3 if not in triangle
	*/
int Triangle::index(DVertex currVertex)
{
	int i = 0;

	while ((i < 3) && (vertex(i) != currVertex))
		i++;

	return i;
}

/*
	* return index of given vertex (equal to index of opposite edge)
	* returns 3 if not in triangle
	*/
int Triangle::index(Cell cell, DVertex currVertex)
{
	int vIndex = index(currVertex);

	if (vIndex != 3)
	{
		// test if in opposite cell
		if (cell != _cell)
			vIndex = 2 - vIndex;	// if so, swap orientation
	}

	return vIndex;
}

/*
	* set existence state of triangle
	*/
void Triangle::setExists(bool exists)
{
	_exists = exists;
}

/*
	* return if triangle exists
	*/
bool Triangle::exists()
{
	return _exists;
}

/*
	* return Delaunay vertex
	*/
const DVertex Triangle::vertex(int index)
{
	int vIndex = tetraTriVertexIndices[indexInCell()][index];

	return cell()->vertex(vIndex);
}

/*
	* return Delaunay vertex
	*/
const DVertex Triangle::vertex(Cell cell, int index)
{
	// test if in opposite cell
	if (cell != _cell)
		index = 2 - index;	// if so, swap orientation

	return vertex(index);
}

/*
	* set edge at given index
	*/
void Triangle::setEdge(int index, Edge *edge)
{
	_edge[index] = edge;
}

/*
	* get edge for given index
	* NOTE: triangle is not oriented
	*/
Edge *Triangle::edge(int index)
{
	return _edge[index];
}

/*
	* return index of edge in triangle [0..2], 3 if it is not contained in triangle
	*/
int Triangle::index(Edge *edge)
{
	int edgeIndex = 0;

	while ((edgeIndex < 3) && (_edge[edgeIndex] != edge))
		edgeIndex++;

	return edgeIndex;
}

/*
	* get edge for given index, for orientation of triangle in the cell
	*/
Edge *Triangle::edge(Cell cell, int index)
{
	// test if triangle in opposite cell
	if (cell != _cell)
		index = 2 - index;	// if so, swap orientation

	return edge(index);
}

/*
	* returns if triangle is finite
	*/
bool Triangle::isFinite(Dt *delaunayTri)
{
	int i = 0;

	while ((i < 3) && !delaunayTri->is_infinite(vertex(i)))
		i++;

	return (i == 3);
}

/*
	* set edge classification
	*/
void Triangle::setOpen(bool isOpen)
{
	_isOpen = isOpen;
}

/*
	* get edge classification
	*/
bool Triangle::isOpen()
{
	return _isOpen;
}

/*
	* set label
	*/
void Triangle::setLabel(int label)
{
	_label = label;
}

/*
	* get label
	*/
int Triangle::label()
{
	return _label;
}

/*
	* returns one adjacent cell
	*/
const Cell Triangle::cell()
{
	return _cell;
}

/*
	* return index as int triple
	*/
const IntTriple Triangle::index()
{
	return IntTriple(IntPair(vertex(0)->info().index, vertex(1)->info().index), vertex(2)->info().index);
}

/*
	* returns a halftriangle for the triangle
	*/
DTriangle Triangle::halftriangle()
{
	return DTriangle(cell(), indexInCell());
}


/*
	* output triangle as v0/v1/v2
	*/
std::ostream& operator<<(std::ostream& os, const Triangle &triangle)
{
	int i;

	for (i = 0; i < 3; i++)
	{
		os << triangle._cell->vertex(tetraTriVertexIndices[triangle._indexInCell][i])->info().index;

		if (i < 2)
			os << "/";
	}

	return os;
}

/*
	* test if triangle contains exactly these vertex indices
	*/
bool Triangle::equals(int *vArray)
{
	int i, _vArray[3];

	for (i = 0; i < 3; i++)
		_vArray[i] = cell()->vertex(tetraTriVertexIndices[indexInCell()][i])->info().index;

	std::sort(_vArray, _vArray + 3);
	std::sort(vArray, vArray + 3);
	i = 0;

	while ((i < 3) && (vArray[i] == _vArray[i]))
		i++;

	return (i == 3);
}



SBElem::SBElem()
{
}

SBElem::SBElem(DTriangle ht, DEdge he)
{
	ch = ht.first;
	vIndex = ht.second;
	eIndex[0] = he.second;
	eIndex[1] = he.third;

	assert(ch == he.first);
	assert(eIndex[0] != vIndex);
	assert(eIndex[1] != vIndex);

}

Cell SBElem::getCell()
{
    return ch;
}

/*
	* return Delaunay halfedge
	*/
DEdge SBElem::edge() const
{
	return DEdge(ch, eIndex[0], eIndex[1]);
}

/*
	* return Delaunay halftriangle
	*/
DTriangle SBElem::triangle()
{
	return DTriangle(ch, vIndex);
}

bool SBElem::equals(SBElem elem)
{
	return ((ch == elem.ch) && (vIndex == elem.vIndex) && (eIndex[0] == elem.eIndex[0]) && (eIndex[1] == elem.eIndex[1]));
}

/*
	* determine other (next) incident edge of triangle
	*/
void SBElem::nextEdgeInHT()
{
	int indices[3] = { vIndex, eIndex[0], eIndex[1] };
	eIndex[1] = determineRemainingIndex(indices);
}

/*
	* reverse orientation of halfedge, by swapping its vertex indices
	*/
void SBElem::reverseHE()
{
	std::swap(eIndex[0], eIndex[1]);
}

/*
	* reverse orientation of halftriangle
	*/
void SBElem::reverseHT(Dt *delaunayTri)
{
	DTriangle ht = triangle();
	DEdge he = edge();
	DTriangle oppHT = delaunayTri->mirror_facet(ht);
	ch = oppHT.first;
	vIndex = oppHT.second;
	locateHEInCell(he, ch);
	eIndex[0] = he.second;
	eIndex[1] = he.third;
}

/*
	* switch halfedge to next one in order in triangle
	*/
void SBElem::nextHEInHT()
{
	nextEdgeInHT();
	reverseHE();
}





//----------------- Non-class functions -------------------//


/*
 * returns triangle for halftriangle
 */
Triangle *ht2Triangle(DTriangle ht)
{
	return ht.first->info().triangle(ht.second);
}

/*
 * returns edge for halfedge
 */
Edge *he2Edge(DEdge he)
{
	int i;
	Cell cell = he.first;
	Tetrahedron *tetra = &cell->info();

	// locate a triangle in tetrahedron which contains both edge vertices
	int triIndex = 0;
	int vIndex[2];
	bool found = false;

	while (!found)
	{
		found = true;
		i = 0;

		while ((i < 2) && found)
		{
			DVertex currVH = cell->vertex(((i == 0) ? he.second : he.third));
			vIndex[i] = tetra->triangle(triIndex)->index(currVH);

			if (vIndex[i] == 3)
				found = false;

			i++;
		}

		if (!found)
			triIndex++;
	}

	// determine index of vertex in triangle which is not in edge
	bool vInEdge[3] = { false, false, false };

	for (i = 0; i < 2; i++)
		vInEdge[vIndex[i]] = true;

	i = 0;

	while ((i < 3) && (vInEdge[i]))
		i++;

	// opposite vertex index is edge index
	return tetra->triangle(triIndex)->edge(i);
}

/*
 * determines halfedge in other tetrahedron
 */
DEdge heInCell(Cell cell, DVertex v0, DVertex v1)
{
	return DEdge(cell, cell->index(v0), cell->index(v1));
}

/*
 * determines halfedge in other tetrahedron
 */
DEdge heInCell(Cell cell, DEdge he)
{
	Cell heCell = he.first;

	return DEdge(cell, cell->index(heCell->vertex(he.second)), cell->index(heCell->vertex(he.third)));
}

/*
 * tests if tetrahedron consists of these vertices' indices
 */
bool cellEquals(Cell ch, int *vArray)
{
	int i, _vArray[4];

	for (i = 0; i < 4; i++)
		_vArray[i] = ch->vertex(i)->info().index;

	std::sort(_vArray, _vArray + 4);
	std::sort(vArray, vArray + 4);
	i = 0;

	while ((i < 4) && (vArray[i] == _vArray[i]))
		i++;

	return (i == 4);
}

/*
 * locate and reference given halfedge in new cell
 * condition: vertices of halfedge must be contained in new cell
 */
void locateHEInCell(DEdge &he, Cell newCH)
{
	Cell oldCH = he.first;
	he.first = newCH;
	he.second = newCH->index(oldCH->vertex(he.second));
	he.third = newCH->index(oldCH->vertex(he.third));
}

/*
 * locate and reference given halfedge in halftriangle
 * condition: vertices of halfedge must be contained in halftriangle
 */
void locateHEInHT(DEdge &he, DTriangle ht)
{
	Cell oldCH = he.first;
	Cell newCH = ht.first;
	he.first = newCH;
	he.second = newCH->index(oldCH->vertex(he.second));
	he.third = newCH->index(oldCH->vertex(he.third));
	int nextIndex = 0;

	while (tetraTriVertexIndices[ht.second][nextIndex] != he.second)
		nextIndex++;

	// test if halfedge vertices are in same order as in halftriangle
	if (tetraTriVertexIndices[ht.second][(nextIndex + 1) % 3] != he.third)
		std::swap(he.second, he.third);
}

/*
 * returns remaining index in tetrahedon
 * condition: all indices are in [0..3]
 */
int determineRemainingIndex(int *indices)
{
	int i;

	// set all referenced indices
	bool vRef[4] = { false, false, false, false };

	for (i = 0; i < 3; i++)
		vRef[indices[i]] = true;

	// locate the remaining one
	i = 0;

	while ((i < 4) && vRef[i])
		i++;

	return i;
}


/*
	* returns iterator to element in boundary
	*/
std::list<SBElem>::iterator SeparatorBoundary::find(SBElem elem)
{
	std::list<SBElem>::iterator elemIter = boundary.begin();

	while ((elemIter != boundary.end()) && !elem.equals(*elemIter))
	{
		elemIter++;
	}

	return elemIter;
}

c3dModelData::c3dModelData() : 	
	numVertices (0),
	criterionType(1) // criterion: longest edge in triangle
{}

c3dModelData::~c3dModelData()
{
	if(this->dt.get() != nullptr)
		this->dt->clear();

	this->dtTriangles.clear();
	this->dtEdgeList.clear();
}
