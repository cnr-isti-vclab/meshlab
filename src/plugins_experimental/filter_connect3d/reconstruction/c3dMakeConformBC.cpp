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
#include "c3dShared.h"

#include "c3dMakeConformBC.h"



void fixNonConformingEntities(int &tetraLabel, c3dModelData &data)
{
	// determine set of non-conforming vertices
	std::set<DVertex> ncVertexSet;

	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;

		if (currVH->info().isNC())
			ncVertexSet.insert(currVH);
	}

	// determine set of non-conforming edges
	std::set<Edge *> ncEdgeSet;

	for (auto iter = data.dtEdgeList.begin(); iter != data.dtEdgeList.end(); iter++)
	{
		Edge *currEdge = &(*iter);

		if (currEdge->isNC())
			ncEdgeSet.insert(currEdge);
	}

	std::set<Edge *> newNCEdgeSet;
	std::set<DVertex> newNCVertexSet;

	bool changed = true;

	// while non-conforming entities left
	while (changed && ((ncVertexSet.size() > 0) || (ncEdgeSet.size() > 0)))
	{
		// until no more changes made
		while (changed && ((ncVertexSet.size() > 0) || (ncEdgeSet.size() > 0)))
		{
			changed = false;

			// handle edges
			for (std::set<Edge *>::iterator iter = ncEdgeSet.begin(); iter != ncEdgeSet.end(); iter++)
			{
				Edge *currEdge = *iter;

				if (currEdge->isNC())
				{
					// add hull for incident triangles
					std::list<Cell> cells;
					edgeAddHull(currEdge, false, cells, tetraLabel, data);

					if (cells.size() > 0)
					{
						// update classification of affected entities and test neighborhood (all incident tetrahedra) for new non-conforming vertices/edges
						classifyEntitiesForCells(cells, newNCEdgeSet, newNCVertexSet, data);
						changed = true;
					}
					else
					if (currEdge->isNC())
						newNCEdgeSet.insert(currEdge);
				}
			}

			// handle vertices
			for (std::set<DVertex>::iterator iter = ncVertexSet.begin(); iter != ncVertexSet.end(); iter++)
			{
				DVertex currVH = *iter;

				if (currVH->info().isNC())
				{
					// add hull for incident triangles
					std::list<Cell> cells;
					vertexAddHull(currVH, false, cells, tetraLabel, data);

					if (cells.size() > 0)
					{
						// update classification of affected entities and test neighborhood (all incident tetrahedra) for new non-conforming vertices/edges
						classifyEntitiesForCells(cells, newNCEdgeSet, newNCVertexSet, data);
						changed = true;
					}
					else
					if (currVH->info().isNC())
						newNCVertexSet.insert(currVH);
				}
			}


			// add newly found non-conforming entities
			ncEdgeSet = newNCEdgeSet;
			ncVertexSet = newNCVertexSet;
			newNCEdgeSet.clear();
			newNCVertexSet.clear();
		}

		// handle edges
		for (std::set<Edge *>::iterator iter = ncEdgeSet.begin(); iter != ncEdgeSet.end(); iter++)
		{
			Edge *currEdge = *iter;

			// add hull for entire local neighborhood
			std::list<Cell> cells;

			if (!edgeAddHullHeuristic(currEdge, cells, tetraLabel, data))	// try adding only limited set of tetrahedra instead of all
				edgeAddHull(currEdge, true, cells, tetraLabel, data);

			if (cells.size() > 0)
			{
				// update classification of affected entities and test neighborhood (all incident tetrahedra) for new non-conforming vertices/edges
				classifyEntitiesForCells(cells, newNCEdgeSet, newNCVertexSet, data);
				changed = true;
			}
		}

		// handle vertices
		for (std::set<DVertex>::iterator iter = ncVertexSet.begin(); iter != ncVertexSet.end(); iter++)
		{
			DVertex currVH = *iter;

			// add hull for entire local neighborhood
			std::list<Cell> cells;

			if (!vertexAddHullHeuristic(currVH, cells, tetraLabel, data))	// try adding only limited set of tetrahedra instead of all
				vertexAddHull(currVH, true, cells, tetraLabel, data);

			if (cells.size() > 0)
			{
				// update classification of affected entities and test neighborhood (all incident tetrahedra) for new non-conforming vertices/edges
				classifyEntitiesForCells(cells, newNCEdgeSet, newNCVertexSet, data);
				changed = true;
			}
		}

		ncEdgeSet = newNCEdgeSet;
		ncVertexSet = newNCVertexSet;
		newNCEdgeSet.clear();
		newNCVertexSet.clear();
	}

}


void edgeAddHull(Edge *edge, bool addAllIncident, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data)
{
	int i;

	// iterate over all tetrahedra and test them if to add
	Dt::Cell_circulator startCH = data.dt->incident_cells(edge->dEdge());
	Dt::Cell_circulator currCH = startCH;
	bool start = true;

	while (start || (startCH != currCH))
	{
		start = false;

		if (is_Finite(data, currCH))
		{
			Tetrahedron *currTetra = &currCH->info();
			bool addIncident = addAllIncident;

			if (!addIncident)
			{
				// test if both triangles of the tetrahedron incident to the edge are adjacent to S_0:
				// condition: tetrahedron is in S_0 and both triangles exist
				if (currTetra->label == 0)
				{
					addIncident = true;

					for (i = 0; i < 4; i++)
					{
						Triangle *currTri = currTetra->triangle(i);

						// test if triangle is incident to edge
						if (currTri->index(edge) != 3)
						{
							if (!currTri->exists())
								addIncident = false;
						}
					}
				}
			}

			if (addIncident)
			{
				// only add if still in exterior space
				if (currTetra->label == 0)
				{
					currTetra->label = tetraLabel++;

					addedCells.push_back(currCH);

					// add all triangles of tetrahedron
					for (i = 0; i < 4; i++)
					{
						if (!currTetra->triangle(i)->exists())
						{
							currTetra->triangle(i)->setExists(true);
						}
					}
				}
			}
		}

		currCH++;
	}
}

std::tuple<std::vector<DVertex>,  std::vector<Edge *>, std::vector<Triangle *> >
classifyEntitiesForCells(std::list<Cell> &cells, c3dModelData &data)
{
    std::vector<Triangle *> triangleSet;
    std::vector<Edge *> edgeSet;
    std::vector<DVertex> vertexSet;

    triangleSet.reserve(cells.size() * 4);
    edgeSet.reserve(cells.size() * 6);
    vertexSet.reserve(cells.size() * 4);

    // iterate over all tetrahedra
    for (std::list<Cell>::iterator iter = cells.begin(); iter != cells.end(); iter++)
    {
        Cell currCH = *iter;

        Tetrahedron *currTetra = &currCH->info();

        // for each triangle in added tetrahedra: is closed
        for (size_t i = 0; i < 4; i++)
            triangleSet.push_back(currTetra->triangle(i));

        // add all edges to set
        for (size_t i = 0; i < 6; i++)
        {
            DEdge he(currCH, tetraEdgeVertexIndices[i][0], tetraEdgeVertexIndices[i][1]);
            edgeSet.push_back(he2Edge(he));
        }

        // add all vertices to set
        for (size_t i = 0; i < 4; i++)
            vertexSet.push_back(currCH->vertex(i));
    }

    classifyEntities(triangleSet, edgeSet, vertexSet, data);

    return std::make_tuple(vertexSet, edgeSet, triangleSet);
}

void classifyEntitiesForCells(std::list<Cell> &cells, std::set<Edge *> &newNCEdgeSet, std::set<DVertex> &newNCVertexSet, c3dModelData &data)
{
    auto cellEntities = classifyEntitiesForCells(cells, data);

    // insert new non-conforming entities
    for (std::vector<Edge *>::iterator iter = std::get<1>(cellEntities).begin(); iter != std::get<1>(cellEntities).end(); iter++)
	{
		Edge *currEdge = *iter;

		if (currEdge->isNC())
			newNCEdgeSet.insert(currEdge);
	}

    for (std::vector<DVertex>::iterator iter = std::get<0>(cellEntities).begin(); iter != std::get<0>(cellEntities).end(); iter++)
	{
		DVertex currVH = *iter;

		if (currVH->info().isNC())
			newNCVertexSet.insert(currVH);
	}
}


void vertexAddHull(DVertex vh, bool addAllIncident, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data)
{
	int i;

	std::list<DTriangle> triangles;
	data.dt->incident_facets(vh, back_inserter(triangles));
	std::set<DVertex> htVertexSet;

	for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
	{
		DTriangle ht = *htIter;

		// test if triangle exists
		if (ht2Triangle(ht)->exists())
		{
			// add the three vertices of the triangle
			for (i = 0; i < 4; i++)
				if (i != ht.second)
					htVertexSet.insert(ht.first->vertex(i));
		}
	}

	std::list<Cell> cells;
	data.dt->incident_cells(vh, back_inserter(cells));

	for (std::list<Cell>::iterator cellIter = cells.begin(); cellIter != cells.end(); cellIter++)
	{
		Cell currCH = *cellIter;

		if (is_Finite(data, currCH))
		{

			// test if all vertices of tetrahedron are in that set
			i = 0;

			while ((i < 4) && (htVertexSet.find(currCH->vertex(i)) != htVertexSet.end()))
				i++;

			// if addAllIncident, add all finite incident tetrahedra instead of those in set
			if (addAllIncident || (i == 4))
			{
				// only add if still in exterior space
				if (currCH->info().label == 0)
				{
					currCH->info().label = tetraLabel++;

					addedCells.push_back(currCH);

					// set all triangles of tetrahedron
					for (i = 0; i < 4; i++)
					{
						Triangle *currTri = currCH->info().triangle(i);

						if (!currTri->exists())
						{
							currTri->setExists(true);
						}
					}

				}
			}
		}
	}
}

//This is just a hack, s.t. the compiler does not have to deal with too long (>4096) name decorators..
struct CellSetStruct
{
	std::set<Cell> cellSet;

	friend bool operator< (const CellSetStruct &cS1, const CellSetStruct &cS2);

};

bool operator< (const CellSetStruct &cS1, const CellSetStruct &cS2)
{
	return cS1.cellSet < cS2.cellSet;
}

bool edgeAddHullHeuristic(Edge *edge, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data)
{
	int i;


	// determine tetrahedra for each sub-space in S_0
	//std::set<std::set<Cell> > subSpaceSet;
	std::set<CellSetStruct> subSpaceSet;


	Dt::Facet_circulator currFC = data.dt->incident_facets(edge->dEdge());
	DTriangle oppHT;

	while ((data.dt->mirror_facet(*currFC).first->info().label != 0) || (currFC->first->info().label == 0))
	{
		currFC++;
	}

	Dt::Facet_circulator startFC = currFC;
	Triangle *nextTri = NULL;

	do
	{
		//std::set<Cell> cellSet;
		CellSetStruct cellSet;

		// traverse subset in S_0 (until other space or existing triangle reached)
		do
		{
//			DTriangle currHT = *currFC;
			oppHT = data.dt->mirror_facet(*currFC);
			cellSet.cellSet.insert(oppHT.first);

			currFC++;
			nextTri = ht2Triangle(*currFC);
		}
		while ((oppHT.first->info().label == 0) && (!nextTri->exists()));

		subSpaceSet.insert(cellSet);

		// circulate to next subset in S_0 (through other spaces, but not if separated by an existing triangle)
		while (data.dt->mirror_facet(*currFC).first->info().label != 0)
		{
			currFC++;
		}

	} while (currFC != startFC);

	assert(subSpaceSet.size() > 2);	// assert that >2 sub-spaces


	// for each sub-space, determine triangles of tetrahedra to be added and calculate their criteria sum -> select sub-space with minimal value
	float minSumCrit = std::numeric_limits<float>::max();
	std::set<CellSetStruct>::iterator minIter;
	std::set<Triangle *> minTriangleSet;

	for (std::set<CellSetStruct>::iterator setIter = subSpaceSet.begin(); setIter != subSpaceSet.end(); setIter++)
	{
		// determine all triangles to be added from the tetrahedra set
		std::set<Triangle *> addedTriangleSet;
		bool containsInfinite = false;

		for (std::set<Cell>::iterator cellIter = setIter->cellSet.begin(); cellIter != setIter->cellSet.end(); cellIter++)
		{
			Cell currCH = *cellIter;

			if (!is_Finite(data, currCH))
				containsInfinite = true;

			for (i = 0; i < 4; i++)
			{
				DTriangle currHT(currCH, i);
				Triangle *currTri = ht2Triangle(currHT);

				if (!currTri->exists())
					addedTriangleSet.insert(currTri);
			}
		}

		if (!containsInfinite)
		{
			float sumCrit = 0.0;

			// calculate sum of criteria for triangles
			for (std::set<Triangle *>::iterator triIter = addedTriangleSet.begin(); triIter != addedTriangleSet.end(); triIter++)
				sumCrit += static_cast<float>(getCriterionForTriangle(*triIter, data));

			if (sumCrit < minSumCrit)
			{
				minSumCrit = sumCrit;
				minIter = setIter;
				minTriangleSet = addedTriangleSet;
			}
		}
	}

	// add tetrahedra for selected sub-space
	addedCells.insert(addedCells.begin(), minIter->cellSet.begin(), minIter->cellSet.end());

	for (std::list<Cell>::iterator iter = addedCells.begin(); iter != addedCells.end(); iter++)
		(*iter)->info().label = tetraLabel++;

	for (std::set<Triangle *>::iterator triIter = minTriangleSet.begin(); triIter != minTriangleSet.end(); triIter++)
		(*triIter)->setExists(true);

	return true;
}


/*
 * add limited hull to vertex (if non-conforming)
 * return false if not successful -> add all incident tetrahedra as workaround
 */
bool vertexAddHullHeuristic(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data)
{
	if (vh->info().ncType == NC_VERTEXCONNECTED)
	{
		return vertexAddHullHeuristicConnected(vh, addedCells, tetraLabel, data);
	}
	else

	if (vh->info().ncType == NC_NONPLANAR)
	{
		return vertexAddHullHeuristicNonPlanar(vh, addedCells, tetraLabel, data);
	}
	else
		return true;	// already conforming

}


/*
 * add limited hull to connected vertex
 * add incident tetrahedra in S_0 containing a S_0/non-S_0 triangle, sorted by criterion of added triangles, until conforming
 * returns true to indicate that vertex is now conforming (is guaranteed)
 */
bool vertexAddHullHeuristicConnected(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data)
{
	return vertexAddHullHeuristicGeneric(vh, addedCells, tetraLabel, NC_VERTEXCONNECTED, data);
}

/*
 * add limited hull to connected vertex
 * add incident tetrahedra in S_0 containing a S_0/non-S_0 triangle, sorted by criterion of added triangles, until conforming
 * returns true to indicate that vertex is now conforming (is guaranteed)
 */
bool vertexAddHullHeuristicNonPlanar(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data)
{
	return vertexAddHullHeuristicGeneric(vh, addedCells, tetraLabel, NC_NONPLANAR, data);
}


/*
 * add limited hull to non-conforming vertex (type=true: vertex-connected, false: non-planar)
 * add incident tetrahedra in S_0 containing a S_0/non-S_0 triangle and open entities, sorted by criterion of added triangles, until conforming
 * returns true to indicate that vertex is now conforming (is guaranteed)
 */
bool vertexAddHullHeuristicGeneric(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, NCType ncType, c3dModelData &data)
{
	int i;

	// create queue of tetrahedra candidates, sorted by sum of criterion of their added triangles
	std::list<Cell> cells;
	data.dt->incident_cells(vh, back_inserter(cells));
	std::multimap<float, Cell> tetraMMap;
	std::map<Cell, float> tetraReverseMap;

	for (std::list<Cell>::iterator cellIter = cells.begin(); cellIter != cells.end(); cellIter++)
	{
		Cell currCH = *cellIter;

		// test if tetrahedron in S_0
		if (isTetrahedronAddable(currCH, vh, data))
		{
			float deltaCrit = getCriterionForNonExistingTrianglesInTetrahedron(currCH, data);
			tetraMMap.insert(std::pair<float, Cell>(deltaCrit, currCH));
			tetraReverseMap[currCH] = deltaCrit;

		}
	}

	// while candidates remain
	while (tetraMMap.size() > 0)
	{
		// remove first tetrahedron from queue
		std::multimap<float, Cell>::iterator mapIter = tetraMMap.begin();
		Cell currCH = mapIter->second;

		tetraMMap.erase(mapIter);
		tetraReverseMap.erase(currCH);

		// add it (label, cell list, triangles)
		currCH->info().label = tetraLabel++;
		addedCells.push_back(currCH);

		for (i = 0; i < 4; i++)
		{
			DTriangle currHT(currCH, i);
			Triangle *currTri = ht2Triangle(currHT);

			if (!currTri->exists())
				currTri->setExists(true);
		}

		// test if vertex is already conforming
		std::list<Cell> cells;
		cells.push_back(currCH);
		std::set<Edge *> dummyEdgeSet;
		std::set<DVertex> dummyVertexSet;
		classifyEntitiesForCells(cells, dummyEdgeSet, dummyVertexSet, data);

		if (ncType == NC_VERTEXCONNECTED)
		{
			if (!isVertexConnectedOC(vh, data))
				return true;
		}
		else
		if (ncType == NC_NONPLANAR)
		{
			if (isOpenVertexManifold(vh, data))
				return true;
		}
		else
		if (ncType == NC_ISOLATED)
		{
			if (!isVertexDeflated(vh, data))
				return true;
		}

		// update all affected tetrahedra in the queue
		int vIndex = currCH->index(vh);
		Triangle *incidentTri[3];

		for (i = 0; i < 3; i++)
		{
			DTriangle currHT(currCH, (vIndex + 1 + i) % 4);
			incidentTri[i] = ht2Triangle(currHT);
		}

		std::set<Cell> affectedTetraSet;

		// for each edge of the added tetrahedron incident to the vertex
		for (i = 0; i < 3; i++)
		{
			// add all tetrahedra in S_0 to affected set
			Edge *currEdge = sharedEdge(incidentTri[i], incidentTri[(i + 1) % 3]);
			Dt::Cell_circulator start2CH = data.dt->incident_cells(currEdge->dEdge());
			Dt::Cell_circulator curr2CH = start2CH;

			do
			{
				if (curr2CH->info().label == 0)
					affectedTetraSet.insert(curr2CH);

				curr2CH++;
			} while (curr2CH != start2CH);
		}

		for (std::set<Cell>::iterator iter = affectedTetraSet.begin(); iter != affectedTetraSet.end(); iter++)
		{
			Cell curr2CH = *iter;
			std::map<Cell, float>::iterator mapIter = tetraReverseMap.find(curr2CH);

			// remove if found, in any case (may be not anymore eligible)
			if (mapIter != tetraReverseMap.end())
			{
				tetraReverseMap.erase(mapIter);

				// remove corresponding item in tetraMMap
				std::multimap<float, Cell>::iterator iter2 = tetraMMap.find(mapIter->second);

				assert(iter2 != tetraMMap.end());


				while ((iter2->first == mapIter->second) && (iter2->second != curr2CH))
					iter2++;

				assert(iter2->second == curr2CH);	// assert that exact element found

				tetraMMap.erase(iter2);
			}

			// test if tetrahedron in S_0 and contains a (boundary) triangle incident to vertex and in non-S_0
			if (isTetrahedronAddable(curr2CH, vh, data))
			{
				float deltaCrit = getCriterionForNonExistingTrianglesInTetrahedron(curr2CH, data);
				std::pair<std::map<Cell, float>::iterator, bool> result = tetraReverseMap.insert(std::pair<Cell, float>(curr2CH, deltaCrit));

				if (result.second)	// only insert if not already exists
					tetraMMap.insert(std::pair<float, Cell>(deltaCrit, curr2CH));
			}
		}
	}

	assert(false);	// assert that conforming

	return false;	// dummy
}


bool isTetrahedronAddable(Cell ch, DVertex vh, c3dModelData &data)
{
	int i;

	// test if tetrahedron is in finite S_0
	if (is_Finite(data, ch) && (ch->info().label == 0))
	{
		// test if tetrahedron also contains triangle of RC
		bool adjacent = false;

		for (i = 0; i < 4; i++)
			if (ch->vertex(i) != vh)
			{
				DTriangle currHT(ch, i);
				Triangle *currTri = ht2Triangle(currHT);

				if (currTri->exists())
					adjacent = true;
			}

		return adjacent;
	}
	else
		return false;
}



float getCriterionForNonExistingTrianglesInTetrahedron(Cell ch, c3dModelData &data)
{
	int i;
	float deltaCrit = 0.0;

	for (i = 0; i < 4; i++)
	{
		DTriangle currHT(ch, i);
		Triangle *currTri = ht2Triangle(currHT);

		if (!currTri->exists())
			deltaCrit += static_cast<float>(getCriterionForTriangle(currTri, data));
	}

	return deltaCrit;
}



Edge *sharedEdge(Triangle *t0, Triangle *t1)
{
	// compare all edges with each other if identical
	int i = 0;

	while (i < 3)
	{
		Edge *currEdge = t0->edge(i);
		int j = 0;

		while (j < 3)
		{
			if (currEdge == t1->edge(j))
				return currEdge;

			j++;
		}

		i++;
	}

	assert(false);
	return NULL;	// dummy
}

