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
#include "c3dHoleTreatment.h"
#include "c3dSculpture.h"
#include "c3dMakeConformBC.h"

#include "c3dMeshFairing.h"



float calculateTotalCurvature(c3dModelData &data)
{
	int i;

	// construct map of all edges in boundary, with adjacent triangle
	//std::map<Edge *, Triangle *> edgeTriangleMap;
	btree::btree_map<Edge *, Triangle *> edgeTriangleMap;

	for (std::vector<Triangle>::iterator iter = data.dtTriangles.begin(); iter != data.dtTriangles.end(); iter++)
	{
		Triangle *currTri = &*iter;

		if (currTri->exists())
		{
			// for each existing triangle: put edge with itself in map
			for (i = 0; i < 3; i++)
				edgeTriangleMap[currTri->edge(i)] = currTri;
		}
	}

	// determine curvature for each edge in the map
	float sumK = 0.0;

	for (auto mapIter = edgeTriangleMap.begin(); mapIter != edgeTriangleMap.end(); mapIter++)
	{
		Edge *currEdge = mapIter->first;
		Triangle *currTri = mapIter->second;

		// determine other boundary triangle adjacent to edge
		DEdge currHE = currEdge->dEdge();
		DTriangle currHT;
		currHT = currTri->halftriangle();
		locateHEInCell(currHE, currHT.first);
		SBElem currElem(currHT, currHE);
		SBElem nextElem = nextExistingTriangle(currElem, data);
		DTriangle nextHT = nextElem.triangle();
		Triangle *nextTri = ht2Triangle(nextHT);

		sumK += calculateCurvature(currEdge, currTri, nextTri, data);
	}

	return sumK;
}



void smoothBoundary(c3dModelData &data)
{
	int i, j;

	// construct map of all edges in boundary, with adjacent triangle
	//std::map<Edge *, Triangle *> edgeTriangleMap;
	btree::btree_map<Edge *, Triangle *> edgeTriangleMap;

	for (std::vector<Triangle>::iterator iter = data.dtTriangles.begin(); iter != data.dtTriangles.end(); iter++)
	{
		Triangle *currTri = &*iter;

		if (currTri->exists())
		{
			// for each existing triangle: put edge with itself in map
			for (i = 0; i < 3; i++)
				edgeTriangleMap[currTri->edge(i)] = currTri;
		}
	}

	// while edge set not empty, test if edge-flip reduces the curvature
	while (edgeTriangleMap.size() > 0)
	{
		auto mapIter = edgeTriangleMap.begin();
		Edge *currEdge = mapIter->first;
		Triangle *newTri[2], *currTri[2] = { mapIter->second, nullptr };
		edgeTriangleMap.erase(mapIter);

		// determine other boundary triangle adjacent to edge
		DEdge currHE = currEdge->dEdge();
		DTriangle currHT[2];
		currHT[0] = currTri[0]->halftriangle();
		locateHEInCell(currHE, currHT[0].first);
		SBElem currElem(currHT[0], currHE);
		SBElem nextElem = nextExistingTriangle(currElem, data);
		currHT[1] = nextElem.triangle();
		currTri[1] = ht2Triangle(currHT[1]);

		// determine if the two boundary triangles are contained in a tetrahedron
		Cell ch[2][2];

		// determine all tetrahedra adjacent to the two boundary triangles
		for (i = 0; i < 2; i++)
		{
			ch[i][0] = currHT[i].first;
			ch[i][1] = data.dt->mirror_facet(currHT[i]).first;
		}

		Cell currCH = NULL;

		// test if any two tetrahedra of these triangles are identical -> contained in same tetrahedron
		for (j = 0; j < 2; j++)
			for (i = 0; i < 2; i++)
				if (ch[0][j] == ch[1][i])
					currCH = ch[0][j];

		if (currCH != NULL)
		{

			// determine which triangles of tetrahedron are in the boundary
			bool inBoundary[4];
			int newIndex = 0;

			for (i = 0; i < 4; i++)
			{
				DTriangle curr2HT(currCH, i);
				Triangle *curr2Tri = ht2Triangle(curr2HT);
				inBoundary[i] = ((curr2Tri == currTri[0]) || (curr2Tri == currTri[1]));

				if (!inBoundary[i])
					newTri[newIndex++] = curr2Tri;
			}

			// the remaining two triangles must not be in the boundary (i.e. as in a corner vertex with 3 triangles in the tetrahedron)
			if (!newTri[0]->exists() && !newTri[1]->exists())
			{
				// the edge-to-be-exposed must also not be in the boundary (or deflated edge would be created)
				Edge *newEdge = getNonBoundaryEdge(currCH, inBoundary);

				// test all its incident tetrahedra if in S_0
				Dt::Cell_circulator start2CH = data.dt->incident_cells(newEdge->dEdge());
				Dt::Cell_circulator curr2CH = start2CH;
				bool isEdgeExposed = false;

				do
				{
					isEdgeExposed = (curr2CH->info().label == 0);
					curr2CH++;
				} while ((curr2CH != start2CH) && !isEdgeExposed);

				if (!isEdgeExposed)
				{
					// compare curvature for the triangle pairs
					float deltaK = getTetrahedronCurvatureCriterion(currCH, inBoundary, data);

					if (deltaK < 0.0)
					{
						// toggle triangles
						for (i = 0; i < 4; i++)
						{
							Triangle *curr2Tri = currCH->info().triangle(i);
							curr2Tri->setExists(!curr2Tri->exists());
						}

						// put affected edges into edge map

						for (j = 0; j < 2; j++)
							for (i = 0; i < 3; i++)
							{
								Edge *curr2Edge = newTri[j]->edge(i);

								// each edge of new triangle pair, except their shared edge
								if (curr2Edge != newEdge)
								{
									edgeTriangleMap[curr2Edge] = newTri[j];
								}
							}
					}
				}
			}
		}
	}
}



float getTetrahedronCurvatureCriterion(Cell ch, bool *inBoundary, c3dModelData &data)
{
	int i;

	// criterion: delta of curvature (K)
	// edge-flip tetrahedron: subtract K for 5 edges before, and add K for 5 remaining edges
	// exposing tetrahedron: subtract K for 3 edges before, and add K for 6 remaining edges
	int inBoundaryCount = 0;

	for (i = 0; i < 4; i++)
		if (inBoundary[i])
			inBoundaryCount++;

	float deltaK = 0.0;

	if (inBoundaryCount == 1)
	{
		DVertex nonBoundaryVH = getNonBoundaryVertex(ch, inBoundary);
		int oppTriIndex = ch->index(nonBoundaryVH);
		DTriangle baseHT(ch, oppTriIndex);
		Triangle *baseTri = ht2Triangle(baseHT);
		Triangle *insideTri[3];
		int insideIndex = 0;

		for (i = 0; i < 4; i++)
			if (i != oppTriIndex)
			{
				Triangle *currTri = ht2Triangle(DTriangle(ch, i));
				Edge *currEdge = sharedEdge(baseTri, currTri);

				// determine other boundary triangle
				DEdge currHE = currEdge->dEdge();
				locateHEInCell(currHE, baseHT.first);
				SBElem insideElem(baseHT, currHE);
				SBElem outsideElem = nextExistingTriangle(insideElem, data);
				Triangle *outsideTri = ht2Triangle(outsideElem.triangle());


				// subtract K of edge with removable triangle
				deltaK -= calculateCurvature(currEdge, baseTri, outsideTri, data);

				// add K of edge with new inside triangle
				deltaK += calculateCurvature(currEdge, currTri, outsideTri, data);

				insideTri[insideIndex++] = currTri;
			}

		for (i = 0; i < 3; i++)
			deltaK += calculateCurvature(insideTri[i], insideTri[(i + 1) % 3], data);
	}
	else
	if (inBoundaryCount == 2)
	{
		// determine the pair of removable and exposable triangles
		int indexBase = 0, indexInside = 0;
		DTriangle baseHT[2];
		Triangle *baseTri[2], *insideTri[2];

		for (i = 0; i < 4; i++)
		{
			if (inBoundary[i])
			{
				baseHT[indexBase] = DTriangle(ch, i);
				baseTri[indexBase] = ht2Triangle(baseHT[indexBase]);
				indexBase++;
			}
			else
				insideTri[indexInside++] = ht2Triangle(DTriangle(ch, i));
		}

		// subtract K of edge between removable triangles
		deltaK -= calculateCurvature(baseTri[0], baseTri[1], data);

		// add K of edge between exposable triangles
		deltaK += calculateCurvature(insideTri[0], insideTri[1], data);

		// get the four outside triangles
		for (int j = 0; j < 2; j++)
			for (i = 0; i < 2; i++)
			{
				Edge *currEdge = sharedEdge(baseTri[j], insideTri[i]);

				// determine other boundary triangle
				DEdge currHE = currEdge->dEdge();
				locateHEInCell(currHE, baseHT[j].first);
				SBElem insideElem(baseHT[j], currHE);
				SBElem outsideElem = nextExistingTriangle(insideElem, data);
				Triangle *outsideTri = ht2Triangle(outsideElem.triangle());

				// subtract K of edge with removable triangle
				deltaK -= calculateCurvature(currEdge, baseTri[j], outsideTri, data);

				// add K of edge with new inside triangle
				deltaK += calculateCurvature(currEdge, insideTri[i], outsideTri, data);
			}
	}
	else
		assert(false);	// pre-condition violated

	return deltaK;
}
