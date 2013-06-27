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

#include "DisjointSets.h"
#include "c3dDataStructures.h"

#include "c3dShared.h"

#include "c3dConstructBC.h"


void createBoundaryComplex(c3dModelData &data)
{
	int i, insertedTriangleCount = 0;

	// initialize a tree for each vertex
	int vTreeCount = data.dt->number_of_vertices();
	DisjointSets dSet(data.numVertices);

	// get sorted triangle map
	btree::btree_multimap<double, Triangle *> triMMap;
	getSortedTriangleMap(triMMap, data);

	btree::btree_set<Edge *> externalEdgeSet;
	btree::btree_set<Triangle *> rejectedTriangleSet;

	// while more than one vertex tree or still external edges left
	while ((triMMap.size() > 0) && ((vTreeCount > 1) || (externalEdgeSet.size() > 0)))
	{
		// remove minimal triangle from set
		auto triIter = triMMap.begin();
		Triangle *triangle = triIter->second;
		int triIndex[3];

		for (i = 0; i < 3; i++)
			triIndex[i] = triangle->vertex(i)->info().index;

		triMMap.erase(triIter);

		// check if that triangle connects two yet unconnected trees
		int root[3], root01;

		for (i = 0; i < 2; i++)
			root[i] = dSet.FindSet(triIndex[i]);

		bool inserted = false;

		// if vertex #0 and #1 not in same tree, connect them and insert triangle into EMST
		if (root[0] != root[1])
		{
			dSet.Union(root[0], root[1]);
			inserted = true;
			vTreeCount--;
		}

		root01 = dSet.FindSet(root[0]);
		root[2] = dSet.FindSet(triIndex[2]);	// can have been incorporated into root0 or root1

		// same for joined tree and vertex #2 (triangle will not appear twice in set)
		if (root01 != root[2])
		{
			dSet.Union(root01, root[2]);
			inserted = true;
			vTreeCount--;
		}

		// check if triangle connects to an external edge
		std::list<Edge *> newNonExtEdges, newExtEdges;

		for (i = 0; i < 3; i++)
		{
			Edge *edge = triangle->edge(i);
			DEdge dEdge = edge->dEdge();
			Dt::Facet_circulator startFC = data.dt->incident_facets(dEdge);
			Dt::Facet_circulator currFC = startFC;
			int edgeTriCount = 0;

			do
			{
				DTriangle currDT = *currFC;
				Triangle *currTri = ht2Triangle(currDT);

				if ((currTri != triangle) && currTri->exists())
					edgeTriCount++;

				currFC++;
			} while (currFC != startFC);

			if (edgeTriCount == 0)
				newExtEdges.push_back(edge);
			else
			if (edgeTriCount == 1)
			{
				newNonExtEdges.push_back(edge);
				inserted = true;
			}
		}

		if (inserted)
		{
			insertedTriangleCount++;
			triangle->setExists(true);

			for (std::list<Edge *>::iterator edgeIter = newNonExtEdges.begin(); edgeIter != newNonExtEdges.end(); edgeIter++)
				externalEdgeSet.erase(*edgeIter);

			for (std::list<Edge *>::iterator edgeIter = newExtEdges.begin(); edgeIter != newExtEdges.end(); edgeIter++)
			{
				Edge *edge = *edgeIter;
				externalEdgeSet.insert(edge);

				// re-insert triangles in evaluation queue if they are adjacent to a new external edge
				DEdge dEdge = edge->dEdge();
				Dt::Facet_circulator startFC = data.dt->incident_facets(dEdge);
				Dt::Facet_circulator currFC = startFC;

				do
				{
					DTriangle currDT = *currFC;
					Triangle *currTri = ht2Triangle(currDT);

					// if located in rejected set
					if ((currTri != triangle) && (rejectedTriangleSet.find(currTri) != rejectedTriangleSet.end()))
					{
						// move from there to evaluation queue
						//std::set<Triangle *>::iterator tri2Iter = rejectedTriangleSet.find(currTri);
						auto tri2Iter = rejectedTriangleSet.find(currTri);
						double crit = getCriterionForTriangle(currTri, data);
						triMMap.insert(std::pair<double, Triangle *>(crit, currTri));
						rejectedTriangleSet.erase(tri2Iter);
					}

					currFC++;
				} while (currFC != startFC);
			}
		}
		else
			rejectedTriangleSet.insert(triangle);
	}


}


/*
 * calculate criterion for triangles and return sorted map with its references
 */
void getSortedTriangleMap(btree::btree_multimap<double, Triangle *> &triMMap, c3dModelData &data)
{
	// sort all triangles in delaunay graph by ascending triangle circumradius
	for (std::vector<Triangle>::iterator triIter = data.dtTriangles.begin(); triIter != data.dtTriangles.end(); triIter++)
	{
		Triangle *triangle = &*triIter;

		if (triangle->isFinite(data.dt.get()))
		{
			double crit = getCriterionForTriangle(triangle, data);
			triMMap.insert(std::pair<double, Triangle *>(crit, triangle));
		}
	}
}

