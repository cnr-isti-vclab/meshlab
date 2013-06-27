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
#include "c3dMath.h"

#include "c3dShared.h"



double getCriterionForTriangle(Triangle *triangle, c3dModelData &data)
{
	int i, triIndex[3];

	for (i = 0; i < 3; i++)
		triIndex[i] = triangle->vertex(i)->info().index;

	if (data.criterionType == 0)
	{
		// criterion: circumradius
		Vector3D v0 = data.vertices[triIndex[0]];
		Vector3D cc;
		float r = calcCircumcenterRadiusDet(v0, data.vertices[triIndex[1]], data.vertices[triIndex[2]], cc);

		return r;
	}
	else
	if (data.criterionType == 1)
	{
		// criterion: longest edge in triangle (prefers also acute angles, small triangles)
		float edge[3];

		for (i = 0; i < 3; i++)
			edge[i] = Vector3D::distance(data.vertices[triIndex[i]], data.vertices[triIndex[(i + 1) % 3]]);

		std::sort(edge, edge + 3);	// determine longest edge

		return edge[2];
	}
	else
	if (data.criterionType == 2)
	{
		// criterion: calculate triangle area
		return calcTriangleArea(data.vertices[triIndex[0]], data.vertices[triIndex[1]], data.vertices[triIndex[2]]);
	}
	else
	if (data.criterionType == 3)
	{
		// criterion: calculate triangle aspect ratio
		return calcTriangleAspectRatio(data.vertices[triIndex[0]], data.vertices[triIndex[1]], data.vertices[triIndex[2]]);
	}

	// dummy
	return 0.0;
}


bool is_Finite(c3dModelData &data, Cell c)
{
	return (!data.dt->is_infinite((c)->vertex(0)) && !data.dt->is_infinite((c)->vertex(1)) && !data.dt->is_infinite((c)->vertex(2)) && !data.dt->is_infinite((c)->vertex(3)));
}


void labelContiguousSpaces(int &tetraLabel, c3dModelData &data)
{
	int i;	// initialize label

	// initial set of all unmarked tetrahedra, including infinite ones
	btree::btree_set<Cell> unmarkedCellSet;

	for (Dt::Cell_iterator cellIter = data.dt->cells_begin(); cellIter != data.dt->cells_end(); cellIter++)
		unmarkedCellSet.insert(cellIter);

	bool start = true;

	// while unmarked tetrahedra remain
	while (unmarkedCellSet.size() > 0)
	{
		// remove one from set
		Cell currCH;

		if (start)
		{
			// ensure that infinite space has label 0
			currCH = data.dt->infinite_cell();
			unmarkedCellSet.erase(currCH);
			start = false;
		}
		else
		{
			//std::set<Cell>::iterator iter = unmarkedCellSet.begin();
			auto iter = unmarkedCellSet.begin();
			currCH = *iter;
			unmarkedCellSet.erase(iter);
		}

		// initialize stack with it
		std::stack<Cell> cellStack;
		cellStack.push(currCH);

		// while adjacent tetrahedra exist in shell
		while (cellStack.size() > 0)
		{
			// pop one from stack
			currCH = cellStack.top();
			cellStack.pop();

			// test if not already traversed
			if (currCH->info().label == -1)
			{
				// mark and remove from set
				currCH->info().label = tetraLabel;
				unmarkedCellSet.erase(currCH);

				// push all its neighbor tetrahedra in shell on stack
				for (i = 0; i < 4; i++)
				{
					Triangle *currTri = currCH->info().triangle(i);

					if (!currTri->exists())
					{
						Cell oppCH = currCH->neighbor(i);

						if (oppCH->info().label == -1)
							cellStack.push(oppCH);
					}
				}

			}
		}

		tetraLabel++;	// increase label for next shell
	}
}

bool classifyEntitiesAll(c3dModelData &data)
{
	//copied from classifyEntities..
	int i;

	// reset edges + vertices flags
	for (std::vector<Edge>::iterator iter = data.dtEdgeList.begin(); iter != data.dtEdgeList.end(); iter++)
	{
		if(!iter->isFinite(data.dt.get()))
			continue;

		iter->setOpen(false);
		iter->setNC(false);
	}

	for (Dt::Finite_vertices_iterator iter = data.dt->finite_vertices_begin(); iter != data.dt->finite_vertices_end(); iter++)
	{
		DVertex currVH = iter;
		currVH->info().isOpen = false;
		currVH->info().ncType = NC_CONFORM;
	}

	// triangles: if both adjacent tetrahedra are in the same space, then it is open
	for (std::vector<Triangle>::iterator currTri = data.dtTriangles.begin(); currTri != data.dtTriangles.end(); currTri++)
	{
		if(!currTri->isFinite(data.dt.get()))
			continue;

		if (currTri->isFinite(data.dt.get()) && currTri->exists())
		{
			Cell currCH = currTri->cell();
			Cell oppCH = currCH->neighbor(currTri->indexInCell());

			if ((currCH->info().label == 0) && (currCH->info().label == oppCH->info().label))
			{
				// mark triangle and its edges and vertices as open
				currTri->setOpen(true);

				for (i = 0; i < 3; i++)
				{
					currTri->edge(i)->setOpen(true);
					currTri->vertex(i)->info().isOpen = true;
				}

			}
			else
				currTri->setOpen(false);
		}
	}

	// edges: open if either in an open triangle, or has >= 2 incident non-face-connected sub-spaces in S_0
	for (std::vector<Edge>::iterator currEdge = data.dtEdgeList.begin(); currEdge != data.dtEdgeList.end(); currEdge++)
	{
		if(!currEdge->isFinite(data.dt.get()))
			continue;

		if (currEdge->isFinite(data.dt.get()) && currEdge->exists(data.dt.get()))
		{
			if (!currEdge->isOpen())
			{
				DTriangle dummyHT;
				bool isOpen = determineIfEdgeOpen(&*currEdge, dummyHT, data);

				if (isOpen)
				{
					// mark edge and its vertices as open
					currEdge->setOpen(true);

					for (i = 0; i < 2; i++)
						currEdge->vertex(i)->info().isOpen = true;

				}
			}

			if (currEdge->isOpen())
				if (isEdgeNonConforming(&*currEdge, data))
				{
					currEdge->setNC(true);
				}
		}
	}

	// vertices: create set of incident sub-spaces separated by incident umbrellas in RC, indexed by their space label
	// if any sub-space occurs twice, the vertex is open
	for (Dt::Finite_vertices_iterator iter = data.dt->finite_vertices_begin(); iter != data.dt->finite_vertices_end(); iter++)
	{
		DVertex currVH = iter;

		// first classify if vertex-connected as such
		if (isVertexConnectedOC(currVH, data))
		{
			currVH->info().isOpen = true;
			currVH->info().ncType = NC_VERTEXCONNECTED;
		}
		else
		{
			if (!currVH->info().isOpen)
			{
				if (isVertexInOpenEdgeOrTriangle(currVH, data))
					currVH->info().isOpen = true;
				else
				if (isVertexDeflated(currVH, data))
				{
					// no incident deflated entities, but itself deflated -> isolated vertex
					currVH->info().isOpen = true;
					currVH->info().ncType = NC_ISOLATED;
				}
			}

			// deflated but not vertex-connected or isolated: test if planar
			if (currVH->info().isOpen && !isOpenVertexManifold(currVH, data))
			{
				currVH->info().ncType = NC_NONPLANAR;
			}
		}
	}

	return true;
}



void classifyEntities(std::vector<Triangle *> &triangleSet, std::vector<Edge *> &edgeSet, std::vector<DVertex> &vertexSet, c3dModelData &data)
{
	int i;

	// reset edges + vertices flags
	for (std::vector<Edge *>::iterator iter = edgeSet.begin(); iter != edgeSet.end(); iter++)
	{
		Edge *currEdge = *iter;
		currEdge->setOpen(false);
		currEdge->setNC(false);
	}

	for (std::vector<DVertex>::iterator iter = vertexSet.begin(); iter != vertexSet.end(); iter++)
	{
		DVertex currVH = *iter;
		currVH->info().isOpen = false;
		currVH->info().ncType = NC_CONFORM;
	}

	// triangles: if both adjacent tetrahedra are in the same space, then it is open
	for (std::vector<Triangle *>::iterator iter = triangleSet.begin(); iter != triangleSet.end(); iter++)
	{
		Triangle *currTri = *iter;

		if (currTri->isFinite(data.dt.get()) && currTri->exists())
		{
			Cell currCH = currTri->cell();
			Cell oppCH = currCH->neighbor(currTri->indexInCell());

			if ((currCH->info().label == 0) && (currCH->info().label == oppCH->info().label))
			{
				// mark triangle and its edges and vertices as open
				currTri->setOpen(true);

				for (i = 0; i < 3; i++)
				{
					currTri->edge(i)->setOpen(true);
					currTri->vertex(i)->info().isOpen = true;
				}

			}
			else
				currTri->setOpen(false);
		}
	}

	// edges: open if either in an open triangle, or has >= 2 incident non-face-connected sub-spaces in S_0
	for (std::vector<Edge *>::iterator iter = edgeSet.begin(); iter != edgeSet.end(); iter++)
	{
		Edge *currEdge = *iter;

		if (currEdge->isFinite(data.dt.get()) && currEdge->exists(data.dt.get()))
		{
			if (!currEdge->isOpen())
			{
				DTriangle dummyHT;
				bool isOpen = determineIfEdgeOpen(currEdge, dummyHT, data);

				if (isOpen)
				{
					// mark edge and its vertices as open
					currEdge->setOpen(true);

					for (i = 0; i < 2; i++)
						currEdge->vertex(i)->info().isOpen = true;

				}
			}

			if (currEdge->isOpen())
				if (isEdgeNonConforming(currEdge, data))
				{
					currEdge->setNC(true);
				}
		}
	}

	// vertices: create set of incident sub-spaces separated by incident umbrellas in RC, indexed by their space label
	// if any sub-space occurs twice, the vertex is open
	for (std::vector<DVertex>::iterator iter = vertexSet.begin(); iter != vertexSet.end(); iter++)
	{
		DVertex currVH = *iter;
		
		// first classify if vertex-connected as such
		if (isVertexConnectedOC(currVH, data))
		{
			currVH->info().isOpen = true;
			currVH->info().ncType = NC_VERTEXCONNECTED;
		}
		else
		{
			if (!currVH->info().isOpen)
			{
				if (isVertexInOpenEdgeOrTriangle(currVH, data))
					currVH->info().isOpen = true;
				else
				if (isVertexDeflated(currVH, data))
				{
					// no incident deflated entities, but itself deflated -> isolated vertex
					currVH->info().isOpen = true;
					currVH->info().ncType = NC_ISOLATED;
				}
			}

			// deflated but not vertex-connected or isolated: test if planar
			if (currVH->info().isOpen && !isOpenVertexManifold(currVH, data))
			{
				currVH->info().ncType = NC_NONPLANAR;
			}
		}

	}

}


void classifyEntitiesForOCBs(std::list<OCBoundary *> &holeOCBs, c3dModelData &data)
{
    std::list<Cell> ocbCells;

    //find all cells of the OCB
    for (auto ocIterator = holeOCBs.begin(); ocIterator != holeOCBs.end(); ocIterator++)
    {
        for(auto sb : (*ocIterator)->boundary[0])
        {
            ocbCells.push_back(sb.getCell());
        }

        for(auto sb : (*ocIterator)->boundary[1])
        {
            ocbCells.push_back(sb.getCell());
        }
    }

    classifyEntitiesForCells(ocbCells, data);
}

bool determineIfEdgeOpen(Edge *edge, DTriangle &ht, c3dModelData &data)
{
	DEdge he = edge->dEdge();

	return determineIfHEOpen(he, ht, data);
}


bool determineIfHEOpen(DEdge he, DTriangle &ht, c3dModelData &data)
{
	bool isOpen = false, s0Visited = false;
	Dt::Facet_circulator startFC = data.dt->incident_facets(he);

	// locate first incident triangle in RC
	while (!ht2Triangle(*startFC)->exists())
		startFC++;

	// traverse incident spaces
	Dt::Facet_circulator currFC = startFC;

	do
	{
		DTriangle currHT = *currFC;
		Triangle *currTri = ht2Triangle(currHT);

		if (currTri->exists())
		{
			if (currHT.first->info().label == 0)
			{
				if (s0Visited)
				{
					isOpen = true;	// S_0 encountered twice -> open edge
					ht = currHT;
				}
				else
					s0Visited = true;
			}
		}

		currFC++;
	} while ((currFC != startFC) && !isOpen);

	return isOpen;
}


bool isEdgeNonConforming(Edge *edge, c3dModelData &data)
{
	if (!edge->isFinite(data.dt.get()) || !edge->exists(data.dt.get()) || !edge->isOpen())
		return false;
/*
	if (edge->equals(56, 450) || edge->equals(261, 67) || edge->equals(296, 83) || edge->equals(155, 139) || edge->equals(281, 320))
		cout << "";
*/
	// determine count of incident open triangles
	int incidentCount = incidentOpenTriangleCount(edge, data);

	// start by positioning at the end of a CC
	DEdge he = edge->dEdge();
	DTriangle endHT, startHT = *data.dt->incident_facets(he);
	int labelOC = 0;	// only use exterior open space
	bool hasCC = locateIncidentCC(he, labelOC, startHT, endHT, data);

	if (!hasCC)
		return false;

	DTriangle firstHT = endHT;
	startHT = firstHT;

	do
	{
		// locate next incident CC
		startHT = endHT;
		locateIncidentCC(he, labelOC, startHT, endHT, data);
		incidentCount++;
	}
	while (firstHT != endHT);

	return (incidentCount != 2);
}


int incidentOpenTriangleCount(Edge *edge, c3dModelData &data)
{
	int count = 0;

	// iterate over all incident triangles
	Dt::Facet_circulator startFC = data.dt->incident_facets(edge->dEdge());
	Dt::Facet_circulator currFC = startFC;
	Triangle *currTri;

	do
	{
		DTriangle currHT = *currFC;
		currTri = ht2Triangle(currHT);

		if ((currTri->isOpen() && (currHT.first->info().label == 0)))
			count++;

		currFC++;
	} while (currFC != startFC);

	return count;
}

bool locateIncidentCC(DEdge he, int labelOC, DTriangle &startHT, DTriangle &endHT, c3dModelData &data)
{
	// rotate about he in forward orientation of startHT
	// navigate to first triangle which is followed by tetrahedra in CC
	Dt::Facet_circulator currFC = data.dt->incident_facets(he, startHT);
	bool noIncidentCC = false;

	while (!noIncidentCC && (data.dt->mirror_facet(*currFC).first->info().label == labelOC))
	{
		currFC++;
		noIncidentCC = (*currFC == startHT);
	}

	if (noIncidentCC)
		return false;

	Dt::Facet_circulator startFC = currFC;
	startHT = *currFC;

	// navigate to first triangle which is followed by tetrahedra not in CC
	currFC = data.dt->incident_facets(he, startHT);

	do
	{
		currFC++;
	} while ((currFC != startFC) && (data.dt->mirror_facet(*currFC).first->info().label != labelOC));

	if (currFC == startFC)
		return false;

	endHT = *currFC;

	return true;
}


bool isVertexConnectedOC(DVertex currVH, c3dModelData &data)
{
	int i;

	// determine set of all triangles //incident to S_0
	std::list<DTriangle> triangles;
	data.dt->incident_facets(currVH, back_inserter(triangles));
	std::list<Triangle *> unconnectedTriangles;

	for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
	{
		DTriangle currHT = *htIter;
		Triangle *currTri = ht2Triangle(currHT);

		if (currTri->exists())
			unconnectedTriangles.push_back(currTri);
	}

	// test if adjacent to S_0
	if (unconnectedTriangles.size() == 0)
		return false;

	// initialize edge-connected set with start triangle
	std::set<Edge *> edgeSet;
	Triangle *startTri = unconnectedTriangles.front();
	unconnectedTriangles.erase(unconnectedTriangles.begin());
	int oppEdgeIndex = startTri->index(currVH);

	for (i = 0; i < 2; i++)
		edgeSet.insert(startTri->edge((oppEdgeIndex + 1 + i) % 3));

	// while triangles added to connected set
	bool changed = true;

	while (changed)
	{
		changed = false;

		// move all edge-connected triangles to that set
		std::list<Triangle *>::iterator iter = unconnectedTriangles.begin();

		while (iter != unconnectedTriangles.end())
		{
			Triangle *currTri = *iter;
			int oppEdgeIndex = currTri->index(currVH);
			bool isConnected = false;
			Edge *edge[2];

			for (i = 0; i < 2; i++)
			{
				edge[i] = currTri->edge((oppEdgeIndex + 1 + i) % 3);

				if (edgeSet.find(edge[i]) != edgeSet.end())
					isConnected = true;
			}

			if (isConnected)
			{
				// move triangle to connected set
				iter = unconnectedTriangles.erase(iter);

				for (i = 0; i < 2; i++)
					edgeSet.insert(edge[i]);

				changed = true;
			}
			else
				iter++;
		}
	}

	// if not all triangles are in the edge-connected set, the vertex is only vertex-connected
	return (unconnectedTriangles.size() > 0);
}


bool isVertexInOpenEdgeOrTriangle(DVertex vh, c3dModelData &data)
{
	std::list<DEdge> hEdges;
	data.dt->incident_edges(vh, back_inserter(hEdges));

	for (std::list<DEdge>::iterator iter = hEdges.begin(); iter != hEdges.end(); iter++)
	{
		Edge *currEdge = he2Edge(*iter);

		if (currEdge->isOpen())
			return true;
	}

	return false;
}


bool isVertexDeflated(DVertex currVH, c3dModelData &data)
{
	std::set<DTriangle> boundaryHTSet[2];
	bool s0Visited = false;

	// start with any tetrahedron incident to the vertex
	std::list<Cell> cells;
	data.dt->incident_cells(currVH, std::back_inserter(cells));
	Cell cell = *cells.begin();

	// initialize halftriangle boundary with its halftriangles incident to the vertex
	updateBoundary(currVH, cell, boundaryHTSet, data);

	if (cell->info().label == 0)
		s0Visited = true;

	// while not all incident tetrahedra traversed (or same space encountered twice)
	while (((boundaryHTSet[0].size() > 0) || (boundaryHTSet[1].size() > 0))/* && !isOpen*/)
	{
		// while boundary contains non-existing halftriangles
		while (boundaryHTSet[1].size() > 0)
		{
			// remove a non-existing halftriangle from boundary
			std::set<DTriangle>::iterator htIter = boundaryHTSet[1].begin();
			DTriangle currHT = *htIter;

			// XOR the tetrahedron's other vertex-incident halftriangles in boundary
			Cell currCH = currHT.first;
			updateBoundary(currVH, currCH, boundaryHTSet, data);
		}

		if (boundaryHTSet[0].size() > 0)
		{
			// remove an existing half-triangle from boundary
			std::set<DTriangle>::iterator htIter = boundaryHTSet[0].begin();
			DTriangle currHT = *htIter;

			// XOR the tetrahedron's other vertex-incident halftriangles in boundary
			Cell currCH = currHT.first;
			updateBoundary(currVH, currCH, boundaryHTSet, data);

			if (currCH->info().label == 0)
			{
				if (s0Visited)
					return true;	// S_0 encountered twice -> vertex is open
				else
					s0Visited = true;
			}
		}
	}

	return false;
}


void updateBoundary(DVertex vh, Cell ch, std::set<DTriangle> *boundaryHTSet, c3dModelData &data)
{
	int i;

	for (i = 0; i < 4; i++)
		if (ch->vertex(i) != vh)
		{
			DTriangle currHT(ch, i);
			Triangle *currTri = ht2Triangle(currHT);
			int setIndex = (currTri->exists() ? 0 : 1);
			std::set<DTriangle>::iterator htIter = boundaryHTSet[setIndex].find(currHT);

			if (htIter != boundaryHTSet[setIndex].end())
			{
				boundaryHTSet[setIndex].erase(htIter);
			}
			else
			{
				DTriangle oppHT = data.dt->mirror_facet(currHT);
				boundaryHTSet[setIndex].insert(oppHT);

			}

		}
}


bool isOpenVertexManifold(DVertex openVH, c3dModelData &data)
{
	int i, j;

	// iterate through incident triangles to get the sets of open edges and triangles
	std::list<DTriangle> triangles;
	data.dt->incident_facets(openVH, back_inserter(triangles));
	std::set<Edge *> openEdgeSet;
	std::set<Triangle *> openTriangleSet;

	for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
	{
		DTriangle currHT = *htIter;
		Triangle *currTri = ht2Triangle(currHT);

		if (currTri->isOpen())
			openTriangleSet.insert(currTri);

		int vertexIndex = currTri->index(openVH);

		for (i = 0; i < 2; i++)
		{
			Edge *currEdge = currTri->edge((vertexIndex + 1 + i) % 3);

			if (currEdge->isOpen())
				openEdgeSet.insert(currEdge);
		}
	}

	// only if open edges or triangles found, test vertex if conforming (any of its umbrellas contained in S_0 must contain them all)
	if ((openEdgeSet.size() > 0) || (openTriangleSet.size() > 0))
	{
		// get set of vertex-incident tetrahedra in S_0
		std::list<Cell> cells;
		data.dt->incident_cells(openVH, back_inserter(cells));
		std::set<Cell> s0CellSet;

		for (std::list<Cell>::iterator iter = cells.begin(); iter != cells.end(); iter++)
		{
			Cell cell = *iter;

			if (cell->info().label == 0)
				s0CellSet.insert(cell);
		}

		// traverse up to two sub-spaces' umbrellas to test if they include all deflated entities
		j = 0;

		while ((j < 2) && (s0CellSet.size() > 0))
		{
			// traverse a sub-space in S_0, from any of its tetrahedra
			std::set<Edge *> openEdgeTestSet = openEdgeSet;
			std::set<Triangle *> openTriangleTestSet = openTriangleSet;
			Cell cell = *s0CellSet.begin();
			std::set<DTriangle> boundaryHTSet[2];

			// initialize halftriangle boundary with its halftriangles incident to the vertex
			updateBoundary(openVH, cell, boundaryHTSet, data);
			s0CellSet.erase(cell);

			// while boundary contains non-existing halftriangles, traverse sub-space of S_0
			while (boundaryHTSet[1].size() > 0)
			{
				// remove a non-existing halftriangle from boundary
				std::set<DTriangle>::iterator htIter = boundaryHTSet[1].begin();
				DTriangle currHT = *htIter;

				// XOR the tetrahedron's other vertex-incident halftriangles in boundary
				Cell currCH = currHT.first;

				for (i = 0; i < 4; i++)
					if (currCH->vertex(i) != openVH)
					{
						DTriangle currHT(currCH, i);
						Triangle *currTri = ht2Triangle(currHT);
						int setIndex = (currTri->exists() ? 0 : 1);
						std::set<DTriangle>::iterator htIter = boundaryHTSet[setIndex].find(currHT);

						if (htIter != boundaryHTSet[setIndex].end())
						{
							// do not erase existing triangles from boundary (= deflated ones)
							if (setIndex == 1)
							{
								boundaryHTSet[setIndex].erase(htIter);
							}
						}
						else
						{
							DTriangle oppHT = data.dt->mirror_facet(currHT);
							boundaryHTSet[setIndex].insert(oppHT);
						}
					}

				s0CellSet.erase(currCH);
			}

			// test all triangles in boundary of sub-space if they contain deflated entities
			for (std::set<DTriangle>::iterator htIter = boundaryHTSet[0].begin(); htIter != boundaryHTSet[0].end(); htIter++)
			{
				// remove an existing half-triangle from boundary
				DTriangle currHT = *htIter;

				// test if existing triangle contains deflated entities
				Triangle *currTri = ht2Triangle(currHT);

				// if so, erase them from test list
				if (currTri->isOpen())
					openTriangleTestSet.erase(currTri);

				int oppIndex = currTri->index(openVH);

				for (i = 0; i < 3; i++)
					if (i != oppIndex)
					{
						Edge *currEdge = currTri->edge(i);

						if (currEdge->isOpen())
							openEdgeTestSet.erase(currEdge);
					}
			}

			// if not all deflated entities traversed -> non-conforming
			if ((openEdgeTestSet.size() > 0) || (openTriangleTestSet.size() > 0))
				return false;

			j++;
		}

		// if still tetrahedra left, there are >2 sub-spaces -> non-conforming
		if (s0CellSet.size() > 0)
			return false;
	}

	return true;
}


/*
 * reset all entities (vertices, edges, triangles) open/closed state to original (closed)
 * also set vertices to non-hole-adjacent
 * reset tetrahedra labels
 */
void resetAllEntities(c3dModelData &data)
{
	resetAllTetraLabels(data);

	for (std::vector<Triangle>::iterator iter = data.dtTriangles.begin(); iter != data.dtTriangles.end(); iter++)
	{
		Triangle *currTri = &*iter;
		currTri->setOpen(false);
		currTri->setLabel(-1);
	}

	for (auto iter = data.dtEdgeList.begin(); iter != data.dtEdgeList.end(); iter++)
	{
		Edge *currEdge = &(*iter);
		currEdge->setOpen(false);
	}

	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;
		currVH->info().isOpen = false;
	}
}


void resetAllTetraLabels(c3dModelData &data)
{
	for (Dt::Cell_iterator cellIter = data.dt->cells_begin(); cellIter != data.dt->cells_end(); cellIter++)
	{
		Cell currCH = cellIter;
		currCH->info().label = -1;
	}
}


void debugOutputVertex(int vIndex, c3dModelData &data)
{
	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;

		if (currVH->info().index == vIndex)
		{

			std::list<DTriangle> triangles;
			data.dt->incident_facets(currVH, back_inserter(triangles));

			for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
			{
				DTriangle currHT = *htIter;
				Triangle *currTri = ht2Triangle(currHT);


				if (currTri->exists())
					std::cout << "triangle " << COUT_HT(currHT) << ": " << (currTri->isOpen() ? "open" : "closed")<< std::endl;
				else
					std::cout << "triangle " << COUT_HT(currHT) << ": " << (currTri->isFinite(data.dt.get()) ? "finite" : "infinite")<< std::endl;

			}

		}
	}
}
