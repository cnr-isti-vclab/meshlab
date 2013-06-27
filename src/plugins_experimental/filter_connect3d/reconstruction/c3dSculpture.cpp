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
#include "c3dSegmentBC.h"
#include "c3dHoleTreatment.h"
#include "c3dMakeConformBC.h"

#include "c3dSculpture.h"


void sculptureWallsHeuristic(std::map<int, Cell> &intSpaceMap, std::list<Cell> &manipulatedCells, c3dModelData &data)
{
    int i;
    btree::btree_set<Cell> uniqueManipulatedCells;

	std::map<int, Cell> allSpaceMap;
	determineSpaceMap(allSpaceMap, intSpaceMap, data);

	// classify (and remove certain) triangles
	std::set<DTriangle> boundaryHTSet;
	getBoundaryHTSet(allSpaceMap, boundaryHTSet, data);

	// put tetrahedra in priority queue which are not in outside space but have 1 or 2 triangles adjacent to it
	// sort queue by 1-triangle tetrahedra, then 2-triangle ones; among them by delta-rho
	std::multimap<std::pair<bool, float>, std::pair<Cell, int> > tetraMMap;	// keep tetrahedra+incident space sorted, 'exposing' before 'edge-flip', then by value (non-unique!)
	std::map<std::pair<Cell, int> , std::pair<bool, float> > tetraReverseMap;	// to look up float value for tetrahedron

	for (std::set<DTriangle>::iterator iter = boundaryHTSet.begin(); iter != boundaryHTSet.end(); iter++)
	{
		DTriangle currHT = *iter;
		Cell currCH = currHT.first;
		DTriangle oppHT = data.dt->mirror_facet(currHT);
		int spaceLabel = oppHT.first->info().label;

		// evaluate eligibility for removal
		int boundaryTriCount;
		bool inBoundary[4];

		if (isTetrahedronRemovable(currCH, spaceLabel, boundaryTriCount, inBoundary, data))
        {
			addTetrahedronToPQ(currCH, spaceLabel, inBoundary, tetraMMap, tetraReverseMap, data);
            uniqueManipulatedCells.insert(currCH);
        }
	}

	// sculpture until no more tetrahedra candidates
	while (tetraReverseMap.size() > 0)
	{
		// remove first tetrahedron from queue
		std::multimap<std::pair<bool, float>, std::pair<Cell, int> >::iterator iter = tetraMMap.begin();
		std::pair<Cell, int> cellPair = iter->second;
		Cell currCH = cellPair.first;
		int spaceLabel = cellPair.second;

		// remove tetrahedron from boundary: update label, boundaryHTSet
		bool inBoundary[4];
		int boundaryTriCount = trianglesInBoundary(currCH, spaceLabel, inBoundary, data);

		// eliminate those exposing candidates which make the result worse
		if ((boundaryTriCount == 2) && (iter->first.second > 0.0))
			removeTetrahedronFromPQ(currCH, spaceLabel, tetraMMap, tetraReverseMap);
		else
		{

			// also remove other candidates with that tetrahedron (to be added to another space)
			for (std::map<int, Cell>::iterator sIter = allSpaceMap.begin(); sIter != allSpaceMap.end(); sIter++)
			{
				int currLabel = sIter->first;

//				if (currLabel != spaceLabel)
					removeTetrahedronFromPQ(currCH, currLabel, tetraMMap, tetraReverseMap);
			}

			currCH->info().label = spaceLabel;

			for (i = 0; i < 4; i++)
			{
				DTriangle currHT(currCH, i);
				Triangle *currTri = ht2Triangle(currHT);

				// toggle triangle in boundary
				currTri->setExists(!inBoundary[i]);


				// update boundaryHTSet
				if (inBoundary[i])
				{
					boundaryHTSet.erase(currHT);
				}
				else
					boundaryHTSet.insert(data.dt->mirror_facet(currHT));	// opposite half-triangle
			}

			currCH->info().label = spaceLabel;

			// re-evaluate all possibly affected tetrahedra: sharing an exposed vertex, edge or triangle with the removed tetrahedron
			std::set<Cell> adjacentTetraSet;

			for (i = 0; i < 4; i++)
			{
				DVertex currVH = currCH->vertex(i);
				std::list<Cell> cells;
				data.dt->incident_cells(currVH, back_inserter(cells));

				for (std::list<Cell>::iterator iter = cells.begin(); iter != cells.end(); iter++)
				{
					Cell curr2CH = *iter;

					// all tetrahedra not in {S} are evaluated as candidates
					if (is_Finite(data, curr2CH) && (allSpaceMap.find(curr2CH->info().label) == allSpaceMap.end()))
						adjacentTetraSet.insert(curr2CH);
				}
			}

			// update/insert them in the queue
			for (std::set<Cell>::iterator cellIter = adjacentTetraSet.begin(); cellIter != adjacentTetraSet.end(); cellIter++)
			{
				Cell curr2CH = *cellIter;
				std::set<int> tetraSpaceSet;

				for (i = 0; i < 4; i++)
				{
					DTriangle curr2HT(curr2CH, i);
					DTriangle opp2HT = data.dt->mirror_facet(curr2HT);
					int spaceHT = opp2HT.first->info().label;

					if (allSpaceMap.find(spaceHT) != allSpaceMap.end())
						tetraSpaceSet.insert(spaceHT);
				}

				for (std::set<int>::iterator sIter = tetraSpaceSet.begin(); sIter != tetraSpaceSet.end(); sIter++)
				{
					int currSpaceLabel = *sIter;
					removeTetrahedronFromPQ(curr2CH, currSpaceLabel, tetraMMap, tetraReverseMap);

					// evaluate eligibility for removal
					int boundaryTriCount;
					bool inBoundary[4];

					if (isTetrahedronRemovable(curr2CH, currSpaceLabel, boundaryTriCount, inBoundary, data))
                    {
						addTetrahedronToPQ(curr2CH, currSpaceLabel, inBoundary, tetraMMap, tetraReverseMap, data);
                        uniqueManipulatedCells.insert(curr2CH);
                    }
				}
			}
		}
	}

    for(auto cell: uniqueManipulatedCells)
    {
        manipulatedCells.push_back(cell);
    }
}


bool isCellRemovable(Cell ch, int boundaryLabel, int &boundaryTriCount, bool *inBoundary, c3dModelData &data)
{
	assert(boundaryLabel == 0);

	// determine number of triangles in current cover
	boundaryTriCount = trianglesInBoundary(ch, boundaryLabel, inBoundary, data);

	if(boundaryTriCount == 0)
		return false;

	return true;

	//TODO: this check needs to go to the updateBoundarySet function
	///*
	// * Ensure that all 4 vertices stay connected, if the current cell is removed:
	// * Check if there are incident existing triangles, that are not part of the
	// * current Cell.
	// */
	//int conformVertices = 0;
	//for(size_t vIndex = 0; vIndex < 4; ++vIndex)
	//{
	//	DVertex v = ch->vertex(vIndex);
	//	std::list<Cell> incidentCells;
	//	data.dt->finite_incident_cells(v, std::back_inserter(incidentCells));
	//	
	//	if(incidentCells.size() < 2)
	//		assert(false);

	//	std::list<DTriangle> adjacentTris;
	//	data.dt->finite_incident_facets(v, std::back_inserter(adjacentTris));

	//	for(auto adjacentT: adjacentTris)
	//	{
	//		DTriangle mirrorTri = data.dt->mirror_facet(adjacentT);

	//		assert(mirrorTri.first != adjacentT.first);
	//		assert(ht2Triangle(adjacentT)->exists() == ht2Triangle(mirrorTri)->exists());

	//		if(adjacentT.first != ch ||	mirrorTri.first != ch)
	//		{
	//			Triangle *t = ht2Triangle(adjacentT);

	//			if(t->exists())
	//			{
	//				++conformVertices;
	//				break;
	//			}
	//		}
	//	}

	//}

	//if(conformVertices == 4)
	//	return true;

	//return false;
}

/*
 * Update the boundary set: check if a vertex will get disconnected, if the triangles of the PQ
 * are removed.
 */
void updateBoundarySet(Cell c, bool *inBoundary, IntPair optTriangles, std::set<DTriangle> &boundaryHTSet, std::map<Triangle *, bool> &triResetMap, c3dModelData &data)
{

	for(size_t triIndex = 0; triIndex < 4; ++triIndex)
	{
		DTriangle currHT(c, triIndex);
		Triangle *currTri = ht2Triangle(currHT);

		triResetMap[currTri] = currTri->exists();

		if(optTriangles.first == triIndex || optTriangles.second == triIndex)
		{
			currTri->setExists(false);
			boundaryHTSet.erase(currHT);
		}
		else
		{
			currTri->setExists(true);
			boundaryHTSet.insert(data.dt->mirror_facet(currHT));
		}
	}

	size_t existingTris = 0;
	for(size_t i = 0; i < 4; ++i)
	{

		if(c->info().triangles[i]->exists() == true)
			++existingTris;
	}

	assert(existingTris > 1);

}

/*
 * Helper function to fill a Pair of ints, which is initialized with -1. Inserts the integer i 
 * to the first position which is != -1
 */
bool addToIntPair(IntPair &p, int i)
{
	if(std::get<0>(p) == -1)
		p.first = i;
	else if (std::get<1>(p) == -1)
		p.second = i;
	else
		return false;

	return true;
}

/*
 * This array stores the vertex indices of the edges of all 4 triangles of a Tetrahedron. The order is defined by 
 * http://www.cgal.org/Manual/3.2/doc_html/cgal_manual/Triangulation_3/Chapter_main.html
 * cellTriEdges[IndexOfTriangleInCell][IndexOfEdgeInTriangle][IndexOfVertexInEdge]
 */
int cellTriEdges[4][3][2] = {
	{{1,3},{3,2},{2,1}}, {{0,2},{2,3},{3,0}}, {{0,3},{3,1},{1,0}}, {{0,1},{1,2},{2,0}}
};

//finds those 2 unique indices which are not present (in the range 0-3)
void getOtherIndicesOf4(const int indices[2], int missingIndices[2])
{
	int writtenNumbers = 0; 

	for(int i = 0; i < 4; ++i)
	{
		if(indices[0] != i && indices[1] != i)
		{
			missingIndices[writtenNumbers] = i;
			++writtenNumbers;
		}
	}

	assert(writtenNumbers == 2);
}

/*
 * Find the longest edge: As a precondition the 2 adjacent triangles of the edge must be inside the
 * boundary. The edge (the indices of the endpoints of the edge) + the length of the edge is returned
 */
std::pair<float, IntPair> getLongestEdgeInBoundary(Cell c, bool *inBoundary, c3dModelData &data)
{
	std::array<std::pair<float, IntPair>, 12> edges;
	size_t insertedEdges = 0;

	for(size_t i = 0; i < 12; ++i)
	{
		edges.at(i).first = std::numeric_limits<float>::min();
		edges.at(i).second = std::make_pair(-1, -1);
	}

	for(size_t triIndex = 0; triIndex < 4; ++triIndex)
	{
		for(size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
		{
			int vIndices[2] = {cellTriEdges[triIndex][edgeIndex][0], cellTriEdges[triIndex][edgeIndex][1]}; 
			int triangles[2];
			getOtherIndicesOf4(vIndices, triangles);

			if(inBoundary[triangles[0]] && inBoundary[triangles[1]])
			{
				DEdge e(c, vIndices[0], vIndices[1]);
				float length = he2Edge(e)->length(data.vertices.get());
				edges[insertedEdges] = std::make_pair(length, std::make_pair(vIndices[0], vIndices[1]));
				++insertedEdges;
			}
		}
	}


	std::sort(edges.begin(), edges.end());

	return std::get<11>(edges);
}


/*
 * Adds a tetrahedron to the priority queue. (both forward and reverse map). If the given cell is already in the PQ, nothing is added.
 */
void addCellToPQ(Cell cell, bool *inBoundary, std::multimap<std::pair<float, IntPair>, Cell > &tetraMMap, std::map<Cell, std::pair<float, IntPair> > &tetraReverseMap,
				 c3dModelData &data)
{
	size_t numBTriangles = 0;
	for(size_t i = 0; i < 4; ++i)
		if(inBoundary[i] == true)
			++numBTriangles;

	//indicate the triangles that can be removed with the current tetrahedron
	IntPair bestTriIndices = std::make_pair(-1, -1);
	float crit = std::numeric_limits<float>::min();

	/* 
	 * Determine which triangles should be used for sculpturing: 
	 * max 2 Triangles that create the best criterion (longest edge in boundary)
	 */
	//for one or two boundary triangles: just take 
	if(numBTriangles < 3)
	{
		for(size_t i = 0; i < 4; ++i)
		{
			if(inBoundary[i] == true)
			{
				bool assertion = addToIntPair(bestTriIndices, i);
				assert(assertion);
				crit = getTetrahedronLongestBoundaryEdgeCriterion(cell, inBoundary, data);
			}
		}
	}
	else
	{
		std::pair<float, IntPair> longestEdge = getLongestEdgeInBoundary(cell, inBoundary, data);
		crit = longestEdge.first;
		bestTriIndices = longestEdge.second;
	}

	//negative criterion, because PQ is sorted ascendingly -> longest should be at the beginning
	std::pair<float, IntPair> floatTriPair = std::make_pair( - crit, bestTriIndices);

	//insert only, if the current cell is not already in the PQ
	auto result = tetraReverseMap.insert(std::make_pair(cell, floatTriPair));
	if(result.second == true)
		tetraMMap.insert(std::make_pair(floatTriPair, cell));
	
}

/*
 * Removes the given cell from the PQ, if it is contained in it.
 */
void removeCellFromPQ(Cell cell, std::multimap<std::pair<float, IntPair>, Cell > &tetraMMap, std::map<Cell, std::pair<float, IntPair> > &tetraReverseMap)
{

	auto reverseIterator = tetraReverseMap.find(cell);

	if(reverseIterator != tetraReverseMap.end())
	{
		auto mmReturnPair = tetraMMap.equal_range(reverseIterator->second);

		bool found = false;

		for(auto it = mmReturnPair.first; it != mmReturnPair.second; ++it)
		{
			if(it->second == cell)
			{
				tetraMMap.erase(it);
				found = true;
				break;
			}
		}

		assert(found);

		tetraReverseMap.erase(reverseIterator);
	}

}

/*
 * Sculpturing based on "sculptureWallsHeuristic". This function doesn't consider the whole triangulation but starts with the given 
 * triangleset. The boundary is defined as those Halftriangles, which have an opposite Halftriangle with label 0.
 */
void sculptureTriangleSetHeuristic(std::set<DTriangle> &triangleSet, std::map<Triangle *, bool> &triResetMap, c3dModelData &data)
{
	// classify (and remove certain) triangles
	std::set<DTriangle> boundaryHTSet;

	for(auto halfTri: triangleSet)
	{
		Triangle *currTri = ht2Triangle(halfTri);
		if (currTri->isFinite(data.dt.get()) && currTri->exists())
		{
			if(halfTri.first->info().label == 0)
				boundaryHTSet.insert(data.dt->mirror_facet(halfTri));
			else
				boundaryHTSet.insert(halfTri);
		}
	}

	// put tetrahedra in priority queue which are not in outside space but have 1 or 2 triangles adjacent to it
	// sort queue by 1-triangle tetrahedra, then 2-triangle ones; among them by delta-rho

	std::multimap<std::pair<float, IntPair>, Cell > tetraMMap;
	std::map<Cell, std::pair<float, IntPair> > tetraReverseMap;


	for (std::set<DTriangle>::iterator iter = boundaryHTSet.begin(); iter != boundaryHTSet.end(); iter++)
	{
		DTriangle currHT = *iter;
		Cell currCH = currHT.first;
		DTriangle oppHT = data.dt->mirror_facet(currHT);
		int spaceLabel = oppHT.first->info().label;

		assert(spaceLabel == 0); //all outside triangles should belong to space 0

		// evaluate eligibility for removal
		int boundaryTriCount;
		bool inBoundary[4];

		if (isCellRemovable(currCH, spaceLabel, boundaryTriCount, inBoundary, data))
        {
			addCellToPQ(currCH, inBoundary, tetraMMap, tetraReverseMap, data);
        }

	}

	// sculpture until no more tetrahedra candidates
	while (tetraReverseMap.size() > 0)
	{
		// remove first tetrahedron from queue
		auto iter = tetraMMap.begin();
		std::pair<float, IntPair> cellCritPair = iter->first;
		Cell currCH = iter->second;
		IntPair triIndices = cellCritPair.second;

		// remove tetrahedron from boundary: update label, boundaryHTSet
		bool inBoundary[4];
		trianglesInBoundary(currCH, 0, inBoundary, data);

		removeCellFromPQ(currCH, tetraMMap, tetraReverseMap);
			
		currCH->info().label = 0;

		updateBoundarySet(currCH, inBoundary, triIndices, boundaryHTSet, triResetMap, data);

		// re-evaluate all possibly affected tetrahedra: sharing an exposed vertex, edge or triangle with the removed tetrahedron
		std::set<Cell> adjacentTetraSet;

		for (size_t i = 0; i < 4; i++)
		{
			DVertex currVH = currCH->vertex(i);
			std::list<Cell> cells;
			data.dt->incident_cells(currVH, back_inserter(cells));

			for (std::list<Cell>::iterator iter = cells.begin(); iter != cells.end(); iter++)
			{
				Cell curr2CH = *iter;
				if (is_Finite(data, curr2CH) && curr2CH->info().label != 0)
					adjacentTetraSet.insert(curr2CH);
			}
		}

		// update/insert them in the queue
		for (std::set<Cell>::iterator cellIter = adjacentTetraSet.begin(); cellIter != adjacentTetraSet.end(); cellIter++)
		{
			Cell curr2CH = *cellIter;

			removeCellFromPQ(curr2CH, tetraMMap, tetraReverseMap);

			// evaluate eligibility for removal
			int boundaryTriCount;
			bool inBoundary[4];

			if (isCellRemovable(curr2CH, 0, boundaryTriCount, inBoundary, data))
            {
				addCellToPQ(curr2CH, inBoundary, tetraMMap, tetraReverseMap, data);
            }
				
		}
		
	}

	triangleSet.clear();
	//filter out non-existing triangles
	for(auto hT: boundaryHTSet)
	{
		if(ht2Triangle(hT)->exists() == true)
			triangleSet.insert(hT);
	}

}


void determineSpaceMap(std::map<int, Cell> &allSpaceMap, std::map<int, Cell> &intSpaceMap, c3dModelData &data)
{
	int i;
	Cell s0Ch;

	// determine set of vertices with exactly 1 umbrella: collect their incident triangles and their spaces (with an example tetrahedron for later re-assigning of space label)
	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;
		std::list<DTriangle> triangles;

		// test if vertex has exactly one single umbrella
		if (isVertexManifold(currVH, triangles, data))
		{
			// test if that umbrella (one triangle) is contained in S_0
			DTriangle startHT = triangles.front();
			DTriangle oppHT = data.dt->mirror_facet(startHT);
			Cell ch[2] = { startHT.first, oppHT.first };
			int label[2];

			for (i = 0; i < 2; i++)
				label[i] = ch[i]->info().label;

			// since no deflated triangles exist, at most one side of triangle can be in space S_0: add opposite one
			for (i = 0; i < 2; i++)
				if (label[i] == 0)
				{
					s0Ch = ch[i];
					intSpaceMap[label[1 - i]] = ch[1 - i];
				}
		}
	}

	// copy interior spaces and add S_0 for all spaces
	allSpaceMap = intSpaceMap;
	allSpaceMap[0] = s0Ch;
}



bool isVertexManifold(DVertex currVH, std::list<DTriangle> &existingTriangles, c3dModelData &data)
{
	// collect all incident triangles
	std::list<DTriangle> triangles;
	data.dt->incident_facets(currVH, back_inserter(triangles));

	// select the existing ones
	for (std::list<DTriangle>::iterator iter = triangles.begin(); iter != triangles.end(); iter++)
		if (ht2Triangle(*iter)->exists())
			existingTriangles.push_back(*iter);

	if (existingTriangles.size() == 0)
		return false;
	else
	{
		// start with one triangle
		DTriangle startHT = existingTriangles.front();

		// locate halfedge in the halftriangle outgoing from that vertex
		Cell startCH = startHT.first;
		int vIndex0 = startCH->index(currVH);
		int vIndex1 = nextCCWInHT(startHT, vIndex0);
		DEdge startHE = DEdge(startCH, vIndex0, vIndex1);

		// construct SBElem from triangle and edge
		SBElem startElem(startHT, startHE);
		SBElem currElem = startElem;

		// traverse umbrella until start element reached or any already visited triangle
		std::set<Triangle *> triangleSet;
		bool visited = false;

		do
		{
			Triangle *currTri = ht2Triangle(currElem.triangle());
			visited = (triangleSet.find(currTri) != triangleSet.end());
			triangleSet.insert(currTri);

			currElem = nextUmbrellaElem(currElem, data);
		} while (!currElem.equals(startElem) && !visited);

		if (visited || (triangleSet.size() < existingTriangles.size()))
			return false;
	}

	return true;
}


int nextCCWInHT(DTriangle currHT, int vIndex)
{
	int i = 0;

	while (tetraTriVertexIndices[currHT.second][i] != vIndex)
		i++;

	return tetraTriVertexIndices[currHT.second][(i + 1) % 3];
}



bool hasNBCInteriorVertices(std::map<int, Cell> &allSpaceMap, c3dModelData &data)
{
	int interiorVertexCount = 0;

	// for all vertices
	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;
		std::list<DTriangle> triangles;
		data.dt->incident_facets(currVH, back_inserter(triangles));
		bool isNBCInterior = true;

		for (std::list<DTriangle>::iterator iter = triangles.begin(); iter != triangles.end(); iter++)
		{
			DTriangle currHT = *iter;
			Triangle *currTri = ht2Triangle(currHT);

			if (currTri->exists())
			{
				DTriangle oppHT = data.dt->mirror_facet(currHT);

				if ((allSpaceMap.find(currHT.first->info().label) != allSpaceMap.end()) &&
					(allSpaceMap.find(oppHT.first->info().label) != allSpaceMap.end()))
					isNBCInterior = false;
			}
		}

		if (isNBCInterior)
		{
			interiorVertexCount++;
		}
	}

	return (interiorVertexCount == 0);
}


bool hasInteriorVertices(c3dModelData &data)
{
	int interiorVertexCount = 0;

	// for all vertices
	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;
		std::list<DTriangle> triangles;
		data.dt->incident_facets(currVH, back_inserter(triangles));
		bool isInterior = true;

		for (std::list<DTriangle>::iterator iter = triangles.begin(); iter != triangles.end(); iter++)
		{
			Triangle *currTri = ht2Triangle(*iter);

			if (currTri->exists())
				isInterior = false;
		}

		if (isInterior)
		{
			interiorVertexCount++;
		}
	}

	return (interiorVertexCount == 0);
}


/*
 * get set of boundary halftriangles for the given {S}
 * NOTE: removes all non-boundary triangles
 */
void getBoundaryHTSet(std::map<int, Cell> &allSpaceMap, std::set<DTriangle> &boundaryHTSet, c3dModelData &data)
{
	int i;

	// iterate all existing triangles, if incident to no space in set -> set inexisting, if exactly one space -> insert HT facing away from that space
	for (std::vector<Triangle>::iterator iter = data.dtTriangles.begin(); iter != data.dtTriangles.end(); iter++)
	{
		Triangle *currTri = &*iter;
		DTriangle boundaryHT;
		int spaceIndex = -1, spaceCount = 0;

		if (currTri->isFinite(data.dt.get()) && currTri->exists())
		{
			DTriangle currHT = currTri->halftriangle();
			DTriangle ht[2] = { currHT, data.dt->mirror_facet(currHT) };

			for (i = 0; i < 2; i++)
				if (allSpaceMap.find(ht[i].first->info().label) != allSpaceMap.end())
				{
					spaceCount++;
					spaceIndex = i;
				}

			if (spaceCount == 0)
				currTri->setExists(false);
			else
			if (spaceCount == 1)
			{
				boundaryHTSet.insert(ht[1 - spaceIndex]);
			}
			else
			{
				// spaceCount == 2
			}
		}
	}
}


bool isTetrahedronRemovable(Cell ch, int boundaryLabel, int &boundaryTriCount, bool *inBoundary, c3dModelData &data)
{
	bool removable = false;

	// determine number of triangles in current cover
	boundaryTriCount = trianglesInBoundary(ch, boundaryLabel, inBoundary, data);


	if (boundaryTriCount == 2)
	{
		// verify that edge between the two non-boundary triangles is not in that space
		Edge *nonBoundaryEdge = getNonBoundaryEdge(ch, inBoundary);

		// verify that nonBoundaryEdge is not contained in that space
		removable = !isEdgeIncidentToSpace(nonBoundaryEdge, boundaryLabel, data);
	}
	else
	if (boundaryTriCount == 1)
	{
		// verify that vertex opposite to the single boundary triangle is not contained in that space
		DVertex oppVH = getNonBoundaryVertex(ch, inBoundary);
		removable = !isVertexIncidentToSpace(oppVH, boundaryLabel, data);
	}

	// test if smallest triangle in tetrahedron would be removed
	int smallestTriIndex = determineLeastCRTriangleInTetrahedron(ch, data);

	if (inBoundary[smallestTriIndex])
		removable = false;

	return removable;
}


int trianglesInBoundary(Cell ch, int label, bool *inBoundary, c3dModelData &data)
{
	int i, boundaryTriCount = 0;

	for (i = 0; i < 4; i++)
	{
		DTriangle currHT(ch, i);
		Cell oppCH = data.dt->mirror_facet(currHT).first;
		inBoundary[i] = (oppCH->info().label == label);

		if (inBoundary[i])
			boundaryTriCount++;
	}

	return boundaryTriCount;
}


Edge *getNonBoundaryEdge(Cell ch, bool *inBoundary)
{

	// determine edge between the two non-boundary triangles
	int i, triIndex = 0;

	// point to first non-boundary triangle
	while (inBoundary[triIndex])
		triIndex++;

	Triangle *boundaryTri0 = ch->info().triangle(triIndex);
	std::set<Edge *> edgeSet;

	for (i = 0; i < 3; i++)
		edgeSet.insert(boundaryTri0->edge(i));

	triIndex++;

	// point to second non-boundary triangle
	while (inBoundary[triIndex])
		triIndex++;

	Triangle *boundaryTri1 = ch->info().triangle(triIndex);

	i = 0;

	while ((i < 3) && (edgeSet.find(boundaryTri1->edge(i)) == edgeSet.end()))
		i++;

	return boundaryTri1->edge(i);
}


bool isEdgeIncidentToSpace(Edge *edge, int label, c3dModelData &data)
{
	// check all its incident tetrahedra if in boundary shell
	Dt::Facet_circulator startFC = data.dt->incident_facets(edge->dEdge());
	Dt::Facet_circulator currFC = startFC;

	do
	{
		DTriangle currDT = *currFC;
		Cell currCH = currDT.first;

		if (is_Finite(data, currCH) && (currCH->info().label == label))
			return true;

		currFC++;
	} while (currFC != startFC);

	return false;
}


DVertex getNonBoundaryVertex(Cell ch, bool *inBoundary)
{
	int vIndex = 0;

	while (!inBoundary[vIndex])
		vIndex++;

	return ch->vertex(vIndex);
}


bool isVertexIncidentToSpace(DVertex vh, int label, c3dModelData &data)
{
	std::list<Cell> cells;
	data.dt->incident_cells(vh, back_inserter(cells));

	for (std::list<Cell>::iterator cellIter = cells.begin(); cellIter != cells.end(); cellIter++)
	{
		Cell currCH = *cellIter;

		if (currCH->info().label == label)
			return true;
	}

	return false;
}


void addTetrahedronToPQ(Cell ch, int spaceLabel, bool *inBoundary, std::multimap<std::pair<bool, float>, std::pair<Cell, int> > &tetraMMap, 
						std::map<std::pair<Cell, int>, std::pair<bool, float> > &tetraReverseMap, c3dModelData &data)
{
	float crit = getTetrahedronLongestBoundaryEdgeCriterion(ch, inBoundary, data);
	std::pair<bool, float> critPair = std::pair<bool, float>(false, -crit);
	std::pair<Cell, int> cellPair = std::pair<Cell, int>(ch, spaceLabel);
	std::pair<std::map<std::pair<Cell, int> , std::pair<bool, float> >::iterator, bool> result = tetraReverseMap.insert(std::pair<std::pair<Cell, int> , std::pair<bool, float> >(cellPair, critPair));

	if (result.second)	// only insert if not already exists (from other boundary triangle)
	{
		tetraMMap.insert(std::pair<std::pair<bool, float>, std::pair<Cell, int> >(critPair, cellPair));

	}
}


float getTetrahedronLongestBoundaryEdgeCriterion(Cell ch, bool *inBoundary, c3dModelData &data)
{
	int i;

	// criterion: longest edge
	float maxCrit = -std::numeric_limits<float>::max();

	for (i = 0; i < 4; i++)
		if (inBoundary[i])
		{
			DTriangle currHT(ch, i);
			float crit = static_cast<float>(getCriterionForTriangle(ht2Triangle(currHT), data));

			if (crit > maxCrit)
				maxCrit = crit;
		}

	return maxCrit;
}



void removeTetrahedronFromPQ(Cell cell, int spaceLabel, std::multimap<std::pair<bool, float>, std::pair<Cell, int> > &tetraMMap,
		std::map<std::pair<Cell, int>, std::pair<bool, float> > &tetraReverseMap)
{
	std::pair<Cell, int> cellPair = std::pair<Cell, int>(cell, spaceLabel);
	std::map<std::pair<Cell, int> , std::pair<bool, float> >::iterator iter = tetraReverseMap.find(cellPair);

	// remove if found, in any case (may be not anymore eligible)
	if (iter != tetraReverseMap.end())
	{

		// remove corresponding item in tetraMMap
		std::multimap<std::pair<bool, float>, std::pair<Cell, int> >::iterator iter2 = tetraMMap.find(iter->second);

		std::pair<bool, float> iterSecond = iter->second; 

		tetraReverseMap.erase(iter);

		assert(iter2 != tetraMMap.end());

		while ((iter2->first == iterSecond) && (iter2->second != cellPair))
		{

			iter2++;
		}

		assert(iter2->second == cellPair);	// assert that exact element found

		tetraMMap.erase(iter2);
	}
}



int determineLeastCRTriangleInTetrahedron(Cell ch, c3dModelData &data)
{
	int i, minTriIndex = -1;
	float minCR = std::numeric_limits<float>::max();

	int backupCriterionType = data.criterionType;
	data.criterionType = 0;

	for (i = 0; i < 4; i++)
	{
		float cr = static_cast<float>(getCriterionForTriangle(ch->info().triangle(i), data));

		if (cr < minCR)
		{
			minCR = cr;
			minTriIndex = i;
		}
	}

	data.criterionType = backupCriterionType;

	return minTriIndex;
}



void popNBCs(std::map<int, Cell> oldIntSpaceMap, std::list<Cell> &manipulatedCells, c3dModelData &data)
{
	int i;

    btree::btree_set<Cell> uniqueManipulatedCells;

	// convert old space map to new one (tetrahedra have been relabeled)
	std::set<int> intSpaceSet, allSpaceSet;

	for (std::map<int, Cell>::iterator iter = oldIntSpaceMap.begin(); iter != oldIntSpaceMap.end(); iter++)
	{
		Cell currCH = iter->second;
		intSpaceSet.insert(currCH->info().label);
	}

	// create set of all spaces together with S_0
	allSpaceSet = intSpaceSet;
	allSpaceSet.insert(0);

	// determine subsets of face-connected tetrahedra which are not in {S} as NBCs, indexed by their space label
	std::map<int, std::set<Cell> > nbcMap;

	for (Dt::Cell_iterator cellIter = data.dt->cells_begin(); cellIter != data.dt->cells_end(); cellIter++)
	{
		Cell currCH = cellIter;
		int label = currCH->info().label;

		if (is_Finite(data, currCH) && (allSpaceSet.find(label) == allSpaceSet.end()))
			nbcMap[label].insert(currCH);
	}

	// for each NBC
	for (std::map<int, std::set<Cell> >::iterator nbcIter = nbcMap.begin(); nbcIter != nbcMap.end(); nbcIter++)
	{
		// collect triangles contained in S_0, and triangles contained in S_{int}
		std::set<Triangle *> triangleSet[2];
		std::set<Cell> *cellSet = &nbcIter->second;

		for (std::set<Cell>::iterator cellIter = cellSet->begin(); cellIter != cellSet->end(); cellIter++)
		{
			Cell currCH = *cellIter;

			for (i = 0; i < 4; i++)
			{
				DTriangle currHT(currCH, i);
				Triangle *currTri = ht2Triangle(currHT);

				if (currTri->exists())
				{
					DTriangle oppHT = data.dt->mirror_facet(currHT);
					int label = oppHT.first->info().label;

					if (label != 0)
					{
						// assert that triangles are either in S_0 or S_{int}
						assert(intSpaceSet.find(label) != intSpaceSet.end());

						label = 1;
					}
						triangleSet[label].insert(ht2Triangle(oppHT));
				}
			}
		}

		// test if there exist triangles contained in S_0
		if (triangleSet[0].size() > 0)
		{
			int removeIndex = -1;

			// test if triangles in S_{int} are edge-connected
			if ((triangleSet[1].size() > 0) && isEdgeConnected(triangleSet[1]))
			{
				if (!containsWall(triangleSet[0], data))
				{
					if (containsWall(triangleSet[1], data))
						removeIndex = 0;	// select triangles in S_0 for removal
					else
					{
						// calculate curvature for removal of both triangle sets
						float curvature[2];

						for (i = 0; i < 2; i++)
							curvature[i] = calculateKForTriangleSet(nbcIter->first, triangleSet[i], data);

						// remove triangles in set with higher curvature
						removeIndex = ((curvature[0] > curvature[1]) ? 0 : 1);
					}
				}
			}

			// remove triangles from selected triangle set (only if in S_0 boundary, else will be taken care of later)
			if (removeIndex == 0)
			{
				for (std::set<Triangle *>::iterator tIter = triangleSet[removeIndex].begin(); tIter != triangleSet[removeIndex].end(); tIter++)
				{
					Triangle *currTri = *tIter;
					currTri->setExists(false);
                    uniqueManipulatedCells.insert(currTri->cell());
				}

				// since NBC merged into S_0, update label for all its tetrahedra
				for (std::set<Cell>::iterator cellIter = cellSet->begin(); cellIter != cellSet->end(); cellIter++)
				{
					Cell currCH = *cellIter;
					currCH->info().label = 0;
                    uniqueManipulatedCells.insert(currCH);
				}
			}
		}

	}

	// remove interior triangles
	for (std::vector<Triangle>::iterator iter = data.dtTriangles.begin(); iter != data.dtTriangles.end(); iter++)
	{
		Triangle *currTri = &*iter;

		if (currTri->exists())
		{
			// test if triangle is on the boundary of S_0
			DTriangle currHT = currTri->halftriangle();

			if ((currHT.first->info().label != 0) && (data.dt->mirror_facet(currHT).first->info().label != 0))
            {
				currTri->setExists(false);
                uniqueManipulatedCells.insert(currTri->cell());
            }
		}
	}

    for(auto cell: uniqueManipulatedCells)
    {
        manipulatedCells.push_back(cell);
    }

}


bool isEdgeConnected(std::set<Triangle *> &triangleSet)
{
	int i;

	// initialize edge set with edges of a start triangle
	std::set<Edge *> edgeSet;
	std::list<Triangle *> triangles;
	triangles.insert(triangles.begin(), triangleSet.begin(), triangleSet.end());
	std::list<Triangle *>::iterator triIter = triangles.begin();
	Triangle *currTri = *triIter;
	triangles.erase(triIter);

	for (i = 0; i < 3; i++)
		edgeSet.insert(currTri->edge(i));

	// while triangles share edges with that edge set, add those and remove from triangle set
	bool changed = true;

	while ((triangles.size() > 0) && changed)
	{
		changed = false;
		std::list<Triangle *>::iterator triIter = triangles.begin();

		while (triIter != triangles.end())
		{
			currTri = *triIter;

			// test if triangle shares edge with edge set
			i = 0;

			while ((i < 3) && (edgeSet.find(currTri->edge(i)) == edgeSet.end()))
				i++;

			if (i < 3)
			{
				// remove triangle and add its edges to edge set
				for (i = 0; i < 3; i++)
					edgeSet.insert(currTri->edge(i));

				changed = true;

				// make sure iterator position remains valid
				triIter = triangles.erase(triIter);
			}
			else
				triIter++;
		}
	}

	// is edge-connected if all triangles share edges with the edge set
	return (triangles.size() == 0);
}


bool containsWall(std::set<Triangle *> &triangleSet, c3dModelData &data)
{
	// determine if triangle set has any interior vertices (not incident to any other existing triangles), then it contains a wall
	int i, interiorVertexCount = 0;
	std::set<DVertex> vertexSet;

	for (std::set<Triangle *>::iterator tIter = triangleSet.begin(); tIter != triangleSet.end(); tIter++)
	{
		Triangle *currTri = *tIter;

		for (i = 0; i < 3; i++)
			vertexSet.insert(currTri->vertex(i));
	}

	for (std::set<DVertex>::iterator vIter = vertexSet.begin(); vIter != vertexSet.end(); vIter++)
	{
		DVertex currVH = *vIter;

		std::list<DTriangle> triangles;
		data.dt->incident_facets(currVH, back_inserter(triangles));
		bool isInterior = true;

		for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
		{
			Triangle *currTri = ht2Triangle(*htIter);

			if (currTri->exists() && (triangleSet.find(currTri) == triangleSet.end()))
				isInterior = false;
		}

		if (isInterior)
			interiorVertexCount++;
	}

	return (interiorVertexCount > 0);
}



float calculateKForTriangleSet(int nbcLabel, std::set<Triangle *> &triangleSet, c3dModelData &data)
{
	int i;

	// put all edges of cover triangles into map with its halftriangle
	std::map<Edge *, Triangle *> edgeMap;

	for (std::set<Triangle *>::iterator tIter = triangleSet.begin(); tIter != triangleSet.end(); tIter++)
	{
		Triangle *currTri = *tIter;

		for (i = 0; i < 3; i++)
			edgeMap[currTri->edge(i)] = currTri;
	}

	// for each edge, get halftriangle in S_i in {S}, then its other halftriangle, and calculate their sum(K)
	float sumK = 0.0;

	for (std::map<Edge *, Triangle *>::iterator mapIter = edgeMap.begin(); mapIter != edgeMap.end(); mapIter++)
	{
		Edge *currEdge = mapIter->first;
		Triangle *currTri = mapIter->second;
		DTriangle currHT = currTri->halftriangle();

		if (currHT.first->info().label != nbcLabel)
			currHT = data.dt->mirror_facet(currHT);

		// determine other boundary triangle in S_i for that edge
		DEdge currHE = currEdge->dEdge();
		locateHEInCell(currHE, currHT.first);
		SBElem currElem(currHT, currHE);
		SBElem nextElem = nextExistingTriangle(currElem, data);
		Triangle *nextTri = ht2Triangle(nextElem.triangle());
		float k = calculateCurvature(currEdge, currTri, nextTri, data);
		sumK += k;

	}

	return sumK;
}


float calculateCurvature(int v0, int v1, int v2, int v3, c3dModelData &data)
{
	// 0 degrees = no curvature/flat, 180 degrees completely bent over
	float angle = static_cast<float>(/*180.0 - */acosSafe(dihedralAngle(data.vertices[v0], data.vertices[v1], 
			data.vertices[v2], data.vertices[v3]))/M_PI*180.0f);
	float len = Vector3D::distance(data.vertices[v1], data.vertices[v2]);
	float curv = angle*len;

	return curv;
}


float calculateCurvature(Edge *edge, Triangle *t0, Triangle *t1, c3dModelData &data)
{
	int i;
	DVertex t0VH, t1VH;
	DVertex edgeVH0 = edge->vertex(0);
	DVertex edgeVH1 = edge->vertex(1);

	for (i = 0; i < 3; i++)
	{
		DVertex currVH = t0->vertex(i);

		if ((currVH != edgeVH0) && (currVH != edgeVH1))
			t0VH = currVH;

		currVH = t1->vertex(i);

		if ((currVH != edgeVH0) && (currVH != edgeVH1))
			t1VH = currVH;
	}

	return calculateCurvature(t0VH->info().index, edgeVH0->info().index, edgeVH1->info().index, t1VH->info().index, data);
}



bool isBoundaryManifold(c3dModelData &data)
{
	int nonManifoldVertexCount = 0, manifoldVertexCount = 0;

	// for all vertices
	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;
		std::list<DTriangle> triangles;

		if (!isVertexManifold(currVH, triangles, data))
		{
			nonManifoldVertexCount++;
		}
		else
		{
			manifoldVertexCount++;
		}
	}

	return (nonManifoldVertexCount == 0);
}



float calculateCurvature(Triangle *t0, Triangle *t1, c3dModelData &data)
{
	return calculateCurvature(sharedEdge(t0, t1), t0, t1, data);
}
