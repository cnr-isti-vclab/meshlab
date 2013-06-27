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

#include "c3dInflate.h"



void inflate(int &tetraLabel, std::list<Cell> &inflatedCells, c3dModelData &data)
{
	int i, j;


	// determine list of deflated vertices
	std::vector<DVertex> openVertexSet = collectOpenVertices(data);

    btree::btree_set<Cell> candidateSet;
	btree::btree_multimap<float, Cell> tetraMMap;
	btree::btree_map<Cell, float> tetraReverseMap;

	// for each deflated vertex, determine set of candidate tetrahedra incident to vertex
	for (std::vector<DVertex>::iterator vIter = openVertexSet.begin(); vIter != openVertexSet.end(); vIter++)
		insertAddableTetrahedraForVertex(*vIter, candidateSet, data);

	//is this possible?
	openVertexSet.clear();

	// add all candidates to queue
	addTetrahedraToQueue(candidateSet, tetraMMap, tetraReverseMap, data);

	//copy candidate cells into list for classification
	for(auto cell: candidateSet)
		inflatedCells.push_back(cell);
	

	// while candidates remain
	while (tetraMMap.size() > 0)
	{
		// remove first tetrahedron from queue
		auto mapIter = tetraMMap.begin();
		Cell currCH = mapIter->second;

		tetraMMap.erase(mapIter);
		tetraReverseMap.erase(currCH);

		// add it (label, cell list, triangles)
		currCH->info().label = tetraLabel++;

		for (i = 0; i < 4; i++)
		{
			DTriangle currHT(currCH, i);
			Triangle *currTri = ht2Triangle(currHT);

			if (!currTri->exists())
			{
				currTri->setExists(true);
			}
		}

		// after adding the tetrahedron: re-classify its entities
		std::list<Cell> cells;
		cells.push_back(currCH);
		std::set<Edge *> dummyEdgeSet;
		std::set<DVertex> dummyVertexSet;
		classifyEntitiesForCells(cells, dummyEdgeSet, dummyVertexSet, data);
		candidateSet.clear();

		// for each vertex of the tetrahedron
		for (i = 0; i < 4; i++)
		{
			DVertex currVH = currCH->vertex(i);

			// remove all incident tetrahedra from queue (in case they are candidates)
			std::list<Cell> cells;
			data.dt->incident_cells(currVH, back_inserter(cells));

			for (std::list<Cell>::iterator cellIter = cells.begin(); cellIter != cells.end(); cellIter++)
			{
				Cell curr2CH = *cellIter;
				removeFromQueue(tetraMMap, tetraReverseMap, curr2CH);

				// add as candidate if it is incident to a deflated or non-conforming vertex
				bool isIncident = false;

				for (j = 0; j < 4; j++)
				{
					DVertex curr2VH = curr2CH->vertex(j);

					if (curr2VH->info().isOpen || (curr2VH->info().ncType != NC_CONFORM))
						isIncident = true;
				}

				if (isIncident && isTetrahedronAddable(curr2CH, currVH, data))
					candidateSet.insert(curr2CH);
			}

			// additionally, if vertex is non-conforming or deflated, add candidates (which were not candidates before!)
			if (currVH->info().isOpen || (currVH->info().ncType != NC_CONFORM))
				insertAddableTetrahedraForVertex(currVH, candidateSet, data);
		}

		// add all candidates to queue
		addTetrahedraToQueue(candidateSet, tetraMMap, tetraReverseMap, data);
	}

}



std::vector<DVertex> collectOpenVertices(c3dModelData &data)
{
	std::vector<DVertex> vertexSet;

	for (Dt::Finite_vertices_iterator vhIter = data.dt->finite_vertices_begin(); vhIter != data.dt->finite_vertices_end(); vhIter++)
	{
		DVertex currVH = vhIter;

		if (currVH->info().isOpen)
			vertexSet.push_back(currVH);
	}

	return vertexSet;
}


void insertAddableTetrahedraForVertex(DVertex vh, btree::btree_set<Cell> &cellSet, c3dModelData &data)
{
	std::list<Cell> cells;
	data.dt->incident_cells(vh, back_inserter(cells));

	for (std::list<Cell>::iterator cellIter = cells.begin(); cellIter != cells.end(); cellIter++)
	{
		Cell currCH = *cellIter;

		// test if tetrahedron is in S_0 and contains an existing triangle, which contains >=1 deflated/non-conforming vertex
		if (isTetrahedronAddable(currCH, vh, data))
			cellSet.insert(currCH);
	}
}



void removeFromQueue(btree::btree_multimap<float, Cell> &tetraMMap, btree::btree_map<Cell, float> &tetraReverseMap, Cell ch)
{
	auto iter = tetraReverseMap.find(ch);

	// if found, remove
	if (iter != tetraReverseMap.end())
	{
		// remove corresponding item in tetraMMap
		auto iter2 = tetraMMap.find(iter->second);

		assert(iter2 != tetraMMap.end());	// assert that found


		// since identical keys possible, iterate to correct secondary value
		while ((iter2->first == iter->second) && (iter2->second != ch))
			iter2++;

		assert(iter2->second == ch);	// assert that exact element found


		tetraMMap.erase(iter2);

		//from reverse map (unique key)
		tetraReverseMap.erase(iter);
	}
}


void addTetrahedraToQueue(const btree::btree_set<Cell> &candidateSet, btree::btree_multimap<float, Cell> &tetraMMap, btree::btree_map<Cell, float> &tetraReverseMap, c3dModelData &data)
{
	// for each candidate
	for (auto cellIter = candidateSet.begin(); cellIter != candidateSet.end(); cellIter++)
	{
		Cell currCH = *cellIter;

		// calculate criterion for tetrahedron and add to queue
		float deltaCrit = getCriterionForNonExistingTrianglesInTetrahedron(currCH, data);
		tetraMMap.insert(std::pair<float, Cell>(deltaCrit, currCH));
		tetraReverseMap[currCH] = deltaCrit;
	}
}
