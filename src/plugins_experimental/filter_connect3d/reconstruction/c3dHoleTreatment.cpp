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
#include "c3dSegmentBC.h"
#include "c3dSculpture.h"

#include "c3dHoleTreatment.h"



std::list<OCBoundary *> testForHolesInOCBs(std::list<OCBoundary> &ocBoundaries, c3dModelData &data)
{
	std::list<OCBoundary *> holeOCBs;

	for (std::list<OCBoundary>::iterator ocbIter = ocBoundaries.begin(); ocbIter != ocBoundaries.end(); ocbIter++)
	{
		OCBoundary *currOCB = &*ocbIter;

		bool holeDetected = detectIfOCBIsOpen(currOCB->boundary[0], data);

		if (holeDetected)
		{
			holeOCBs.push_back(currOCB);

		}
	}

	return holeOCBs;
}


bool detectIfOCBIsOpen(std::list<SBElem> &elemList, c3dModelData &data)
{
	int i;

	// create the set of all elements from the OCB, indexed by the pair of vertex indices of the halfedge, as well as the original edge set
	std::multimap<IntPair, SBElem> elemMMap;	// multimap necessary because halfedges can exist multiply in a SB (same CC incident multiple times at that edge)
	std::set<IntPair> originalHESet;

	// only need to test for the given side of the OCB
	for (std::list<SBElem>::iterator elemIter = elemList.begin(); elemIter != elemList.end(); elemIter++)
	{
		SBElem currElem = *elemIter;
		DEdge currHE = currElem.edge();
		IntPair pair = vIndexPair(currHE);
		elemMMap.insert(ElemPair(pair, currElem));
		std::swap(pair.first, pair.second);
		originalHESet.insert(pair);
	}

	// while elements left in the boundary
	bool holeDetected = false;

	while ((elemMMap.size() > 0) && !holeDetected)
	{

		// remove an element from the boundary
		std::multimap<IntPair, SBElem>::iterator mmapIter = elemMMap.begin();
		SBElem currElem = mmapIter->second;
		elemMMap.erase(mmapIter);

		DTriangle currHT = currElem.triangle();
		DEdge outsideHE = currElem.edge();
		int outsideIndex = getHEIndex(currHT, outsideHE);

		// XOR its edges in the boundary
		i = 0;

		while ((i < 3) && !holeDetected)
		{
			if (i != outsideIndex)
			{
				DEdge currHE = getHTEdge(currHT, i);
				IntPair pair = vIndexPair(currHE);

				// test if triangle connects to original OCB, with same orientation (=triangle strip through hole)
				if (originalHESet.find(pair) != originalHESet.end())
				{
					holeDetected = true;
				}
				else
				{
					// test if edge is in original edge boundary, or else in other OCB (simplified here as test for open edge)
					bool isRemovable = false;
					std::multimap<IntPair, SBElem>::iterator mapIter = elemMMap.find(pair);

					while ((mapIter != elemMMap.end()) && (mapIter->first == pair) && (mapIter->second.triangle() != currHT))
						mapIter++;

					// halfedge exists in boundary: remove
					if (mapIter != elemMMap.end())
					{
						elemMMap.erase(mapIter);
						isRemovable = true;
					}

					// edge is also removed when hitting other OCB (indicated by open edge) - also can handle trees of open edges inside the CC set, since boundary at both sides for the triangulation
					if (!isRemovable)
						isRemovable = he2Edge(currHE)->isOpen();

					if (isRemovable)
					{

					}
					else
					{
						// halfedge does not exist in boundary: add

						// determine next inside triangle on CC boundary
						DTriangle oppHT, newHT = currHT;
						DEdge newHE = currHE;
						Triangle *newTri = NULL;

						do
						{
							nextFacingHalftriangle(newHE, newHT, data);

							newTri = ht2Triangle(newHT);
							oppHT = data.dt->mirror_facet(newHT);
						} while (!newTri->exists());

						// flip halftriangle and add
						newHE = heInCell(oppHT.first, currHE);
						std::swap(newHE.second, newHE.third);
						elemMMap.insert(ElemPair(vIndexPair(newHE), SBElem(oppHT, newHE)));
					}
				}
			}

			i++;
		}
	}

	return holeDetected;
}



void nextFacingHalftriangle(DEdge &he, DTriangle &ht, c3dModelData &data)
{
	// old tetrahedron
	Cell oldCH = ht.first;

	// determine vertex in halftriangle opposite to halfedge, in old tetrahedron
	DVertex oppVH = oldCH->vertex(determineRemainingIndex(he, ht));

	// determine next halftriangle in old tetrahedron
	ht.second = oldCH->index(oppVH);

	// opposite halftriangle in new tetrahedron
	ht = data.dt->mirror_facet(ht);

	// new tetrahedron
	Cell newCH = ht.first;

	// return same halfedge in new tetrahedron
	locateHEInCell(he, newCH);
}


std::list<OCBoundary *> classifyOCBWithHull(std::list<OCBoundary *> &openOCBs, c3dModelData &data)
{
	std::list<OCBoundary *> holeOCBs;

	// collect set of edges of open OCBs
	std::set<Edge *> openOCBEdgeSet;

	for (std::list<OCBoundary *>::iterator ocbIter = openOCBs.begin(); ocbIter != openOCBs.end(); ocbIter++)
	{
		OCBoundary *currOCB = *ocbIter;

		for (std::list<SBElem>::iterator elemIter = currOCB->boundary[0].begin(); elemIter != currOCB->boundary[0].end(); elemIter++)
		{
			SBElem currElem = *elemIter;
			Edge *currEdge = he2Edge(currElem.edge());
			openOCBEdgeSet.insert(currEdge);
		}
	}

	for (std::list<OCBoundary *>::iterator ocbIter = openOCBs.begin(); ocbIter != openOCBs.end(); ocbIter++)
	{
		OCBoundary *currOCB = *ocbIter;


		// merge vertex sets of all CCs for OCB
		std::set<DVertex> vertexSet;
		getCCVertexSetForOCB(currOCB, vertexSet);

		// test if OC has interior vertices
		bool isDeflated = !testOCHasInteriorVertices(currOCB, openOCBEdgeSet, data);

		if (!isDeflated)
		{
			holeOCBs.push_back(currOCB);
		}
	}

	return holeOCBs;
}



void getCCVertexSetForOCB(OCBoundary *currOCB, std::set<DVertex> &vertexSet)
{
	std::set<SBGroup *> sbGroupSet;

	for (std::list<CC *>::iterator ccIter = currOCB->ccList.begin(); ccIter != currOCB->ccList.end(); ccIter++)
	{
		CC *currCC = *ccIter;

		vertexSet.insert(currCC->interiorVertexSet.begin(), currCC->interiorVertexSet.end());

		for (auto sbIter = currCC->outsideSet.begin(); sbIter != currCC->outsideSet.end(); sbIter++)
		{
			SeparatorBoundary *currSB = *sbIter;
			SBGroup *sbGroup = currSB->sbGroup;

			for (auto sb2Iter = sbGroup->sbMap.begin(); sb2Iter != sbGroup->sbMap.end(); sb2Iter++)
			{
				SeparatorBoundary *curr2SB = sb2Iter->second;
				vertexSet.insert(curr2SB->sepBVertexSet.begin(), curr2SB->sepBVertexSet.end());

			}

			sbGroupSet.insert(sbGroup);
		}

	}

	for (std::set<SBGroup *>::iterator sbgIter = sbGroupSet.begin(); sbgIter != sbGroupSet.end(); sbgIter++)
	{
		SBGroup *sbGroup = *sbgIter;
		vertexSet.insert(sbGroup->intBVertexSet.begin(), sbGroup->intBVertexSet.end());

	}
}



bool testOCHasInteriorVertices(OCBoundary *currOCB, std::set<Edge *> &openOCBEdgeSet, c3dModelData &data)
{
	int i;

	// determine vertex set of open OCBs
	std::set<DVertex> openOCBVertexSet;

	for (std::set<Edge *>::iterator iter = openOCBEdgeSet.begin(); iter != openOCBEdgeSet.end(); iter++)
	{
		Edge *currEdge = *iter;

		for (i = 0; i < 2; i++)
			openOCBVertexSet.insert(currEdge->vertex(i));
	}

	// for each element in OCB, get next triangle on boundary if deflated, as element in edge-boundary
	std::map<Edge *, DTriangle> edgeBoundary;

	for (std::list<SBElem>::iterator iter = currOCB->boundary[0].begin(); iter != currOCB->boundary[0].end(); iter++)
	{
		SBElem currElem = *iter;
		DEdge currHE = currElem.edge();
		DTriangle currHT = currElem.triangle();
		Edge *currEdge = he2Edge(currHE);
		DTriangle oppHT = data.dt->mirror_facet(currHT);	// face outside CC to S_0
		locateHEInCell(currHE, oppHT.first);
		SBElem oppElem(oppHT, currHE);

		// determine opposite boundary triangle adjacent to edge
		SBElem nextElem = nextExistingTriangle(oppElem, data);
		DTriangle nextHT = data.dt->mirror_facet(nextElem.triangle());
		Triangle *nextTri = ht2Triangle(nextHT);

		if (nextTri->isOpen())
		{
			// add edge with deflated triangle inside OC to edge boundary
			edgeBoundary[currEdge] = nextHT;
		}
	}

	// while edge-boundary not empty
	while (edgeBoundary.size() > 0)
	{
		// remove an element from edge-boundary
		std::map<Edge *, DTriangle>::iterator htIter = edgeBoundary.begin();
		Edge *currEdge = htIter->first;
		DTriangle currHT[2];
		currHT[0] = htIter->second;
		Triangle *currTri = ht2Triangle(currHT[0]);

		// get opposite vertex
		DVertex nextVH = currTri->vertex(currTri->index(currEdge));

		// test if vertex is not in any open OCB
		if (openOCBVertexSet.find(nextVH) == openOCBVertexSet.end())
		{

			return true;
		}

		edgeBoundary.erase(htIter);

		// XOR its new edges in edge-boundary
		for (i = 0; i < 3; i++)
		{
			Edge *nextEdge = currTri->edge(i);

			if (currEdge != nextEdge)
			{
				if (edgeBoundary.erase(nextEdge) == 0)
				{
					// edge did not exist in edge boundary, determine its opposite triangle
					DEdge nextHE = nextEdge->dEdge();
					locateHEInCell(nextHE, currHT[0].first);
					SBElem currElem(currHT[0], nextHE);
					SBElem nextElem = nextExistingTriangle(currElem, data);
					DTriangle nextHT = data.dt->mirror_facet(nextElem.triangle());

					// if next edge not contained in another open OCB
					if (openOCBEdgeSet.find(nextEdge) == openOCBEdgeSet.end())
					{
						// insert edge OC triangle
						edgeBoundary[nextEdge] = nextHT;

					}
				}

			}
		}
	}

	return false;
}



SBElem nextExistingTriangle(SBElem elem, c3dModelData &data)
{
	DTriangle currHT = elem.triangle();
	DEdge currHE = elem.edge();
	Triangle *currTri = ht2Triangle(currHT);

	// while not suitable triangle found, navigate to next RHS about that edge
	do
	{
		nextRHSHalftriangle(currHE, currHT, data);
		currTri = ht2Triangle(currHT);
	}
	while (!currTri->exists());

	return SBElem(currHT, currHE);
}

/*
 * Identify the triangles that belong to the boundary (similar to detectIfOCBIsOpen)
 */
std::set<DTriangle> findBoundaryTriangles(OCBoundary *ocb, c3dModelData &data)
{
	int i;
	std::set<DTriangle> boundaryTriangles;

	// create the set of all elements from the OCB, indexed by the pair of vertex indices of the halfedge, as well as the original edge set
	std::multimap<IntPair, SBElem> elemMMap;	// multimap necessary because halfedges can exist multiply in a SB (same CC incident multiple times at that edge)
	std::set<IntPair> originalHESet;

	// only need to test for the given side of the OCB
	for (std::list<SBElem>::iterator elemIter = ocb->boundary[0].begin(); elemIter != ocb->boundary[0].end(); elemIter++)
	{
		SBElem currElem = *elemIter;
		DEdge currHE = currElem.edge();
		IntPair pair = vIndexPair(currHE);
		elemMMap.insert(ElemPair(pair, currElem));
		std::swap(pair.first, pair.second);
		originalHESet.insert(pair);
	}

	// while elements left in the boundary
	while ((elemMMap.size() > 0))
	{

		// remove an element from the boundary
		std::multimap<IntPair, SBElem>::iterator mmapIter = elemMMap.begin();
		SBElem currElem = mmapIter->second;
		elemMMap.erase(mmapIter);

		DTriangle currHT = currElem.triangle();

		boundaryTriangles.insert(currHT);

		DEdge outsideHE = currElem.edge();
		int outsideIndex = getHEIndex(currHT, outsideHE);

		// XOR its edges in the boundary
		i = 0;

		while ((i < 3))
		{
			if (i != outsideIndex)
			{
				DEdge currHE = getHTEdge(currHT, i);
				IntPair pair = vIndexPair(currHE);

				// test if triangle connects to original OCB, with same orientation (=triangle strip through hole)
				if (originalHESet.find(pair) != originalHESet.end())
				{
					//holeDetected = true;
				}
				else
				{
					// test if edge is in original edge boundary, or else in other OCB (simplified here as test for open edge)
					bool isRemovable = false;
					std::multimap<IntPair, SBElem>::iterator mapIter = elemMMap.find(pair);

					while ((mapIter != elemMMap.end()) && (mapIter->first == pair) && (mapIter->second.triangle() != currHT))
						mapIter++;

					// halfedge exists in boundary: remove
					if (mapIter != elemMMap.end())
					{
						elemMMap.erase(mapIter);
						isRemovable = true;
					}

					// edge is also removed when hitting other OCB (indicated by open edge) - also can handle trees of open edges inside the CC set, since boundary at both sides for the triangulation
					if (!isRemovable)
						isRemovable = he2Edge(currHE)->isOpen();

					if (isRemovable)
					{

					}
					else
					{
						// halfedge does not exist in boundary: add

						// determine next inside triangle on CC boundary
						DTriangle oppHT, newHT = currHT;
						DEdge newHE = currHE;
						Triangle *newTri = NULL;

						do
						{
							nextFacingHalftriangle(newHE, newHT, data);

							newTri = ht2Triangle(newHT);
							//sometimes endless loop!!
							oppHT = data.dt->mirror_facet(newHT);
						} while (!newTri->exists());

						// flip halftriangle and add
						newHE = heInCell(oppHT.first, currHE);
						std::swap(newHE.second, newHE.third);
						elemMMap.insert(ElemPair(vIndexPair(newHE), SBElem(oppHT, newHE)));
					}
				}
			}

			i++;
		}
	}

	std::set<DTriangle> existingTris;

	for(auto t: boundaryTriangles)
	{
		if(ht2Triangle(t)->exists() && ht2Triangle(t)->isFinite(data.dt.get()))
			existingTris.insert(t);
	}

	//debug only, the above can be removed
	assert(existingTris.size() == boundaryTriangles.size());

	return boundaryTriangles;
}

/*
 * Search for edges with only one adjacent triangle
 */
std::set<Edge *> getBoundaryEdges(std::set<DTriangle> &boundaryTriangles, c3dModelData &data)
{
	std::set<Edge *> boundaryEdges;

	for(std::set<DTriangle>::const_iterator triIter = boundaryTriangles.begin(); triIter != boundaryTriangles.end(); ++triIter)
	{
		Triangle *t = ht2Triangle(*triIter);

		for(size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
		{
			Edge *e = t->edge(edgeIndex);
			assert(e->isFinite(data.dt.get()));
			Dt::Facet_circulator firstTri = data.dt->incident_facets(e->dEdge());
			Dt::Facet_circulator tempTri = firstTri;
			size_t numberOfIncidentTriangles = 0;
			do
			{
				Triangle *t = ht2Triangle(*tempTri);
				if(t->exists() && t->isFinite(data.dt.get()))
					++numberOfIncidentTriangles;

				++tempTri;

			} while(tempTri != firstTri);

			if(numberOfIncidentTriangles == 1)
			{
				boundaryEdges.insert(e);
			}
		}

	}

	return boundaryEdges;
}

/*
 * create triangulation for boundaryEdges (adapted from createHull)
 * TODO: rewrite createHull functions and merge them!
 */
std::set<Triangle *> createHullFromBoundaryEdges(std::set<Edge *> &boundaryEdges, c3dModelData &data)
{
	int i;

	// for all vertices, put all incident triangles into candidate set
	std::set<Triangle *> candTriangleSet, hullTriangleSet;

	std::set<DVertex> vertexSet;
	for(auto edgeP: boundaryEdges)
	{
		vertexSet.insert(edgeP->vertex(0));
		vertexSet.insert(edgeP->vertex(1));
	}


	for (std::set<DVertex>::iterator iter = vertexSet.begin(); iter != vertexSet.end(); iter++)
	{
		DVertex currVH = *iter;
		std::list<DTriangle> triangles;
		data.dt->incident_facets(currVH, back_inserter(triangles));

		for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
		{
			DTriangle currHT = *htIter;
			Triangle *currTri = ht2Triangle(currHT);

			// then select those with all vertices in vertexSet
			i = 0;

			while ((i < 3) && (vertexSet.find(currTri->vertex(i)) != vertexSet.end()))
				i++;

			if (i == 3)
				candTriangleSet.insert(currTri);
		}
	}


	// equals previous createHull() result:
	hullTriangleSet = candTriangleSet;

	// remove all triangles with external edges (1 incident triangles) from hull set (recursively):

	// determine all added triangles, of those all with external edges
	std::list<Triangle *> hullAddedTriangleList;
	std::set<Triangle *> externalTriangleSet;

	for (std::set<Triangle *>::iterator iter = hullTriangleSet.begin(); iter != hullTriangleSet.end(); iter++)
	{
		Triangle *currTri = *iter;

		// determine new triangles in hull and add that set to triangulation
		if (!currTri->exists())
		{
			hullAddedTriangleList.push_back(currTri);
			currTri->setExists(true);

			if (triangleHasExternalEdge(currTri, data))
				externalTriangleSet.insert(currTri);
		}
	}

	// while triangles with external edges left
	while (externalTriangleSet.size() > 0)
	{
		// remove triangle from set
		std::set<Triangle *>::iterator iter = externalTriangleSet.begin();
		Triangle *currTri = *iter;
		externalTriangleSet.erase(iter);

		// test if it still exists and has an external edge
		if (currTri->exists() && triangleHasExternalEdge(currTri, data))
		{
			// if so, remove from triangulation, from hull set
			currTri->setExists(false);
			hullTriangleSet.erase(currTri);

			// put incident triangles to its other edges into set
			for (i = 0; i < 3; i++)
			{
				Edge *currEdge = currTri->edge(i);
				DEdge currHE = currEdge->dEdge();
				Dt::Facet_circulator startFC = data.dt->incident_facets(currHE);
				Dt::Facet_circulator currFC = startFC;

				// iterate over all incident triangles to that edge
				do
				{
					DTriangle currHT = *currFC;
					Triangle *edgeCurrTri = ht2Triangle(currHT);

					if (edgeCurrTri->exists() && (edgeCurrTri != currTri) && triangleHasExternalEdge(currTri, data))
						externalTriangleSet.insert(edgeCurrTri);

					currFC++;
				} while (currFC != startFC);
			}
		}
	}

	// restore triangulation: remove the added set
	for (std::list<Triangle *>::iterator iter = hullAddedTriangleList.begin(); iter != hullAddedTriangleList.end(); iter++)
		(*iter)->setExists(false);

	return hullTriangleSet;
}

bool checkHoleCover(std::set<Triangle *> &hullTriangles, std::set<Edge *> &boundaryEdges)
{
	//find edges in cover that are not in the boundary edges set
	std::set<Edge *> newBEdges, allNewBoundaryEdges;

	for(auto coverTriangle: hullTriangles)
	{
		bool isInCover[3];
		size_t edgesInCover = 0;
		for(size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
		{
			Edge *e = coverTriangle->edge(edgeIndex);
			if(boundaryEdges.find(e) != boundaryEdges.end())
			{
				++edgesInCover;
				isInCover[edgeIndex] = true;
			}
			else
				isInCover[edgeIndex] = false;
			
		}


		for(size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
		{
			if(edgesInCover == 2)
			{
				if(isInCover[edgeIndex] == false)
				{
					newBEdges.insert(coverTriangle->edge(edgeIndex));
				}
			}
			else
			{

			}
		}
		
		
	}

	for(auto e: newBEdges)
	{
		if(boundaryEdges.find(e) != boundaryEdges.end())
			assert(false); //there should be only new edges in this set!
	}

	
}


void fillHoles(std::list<OCBoundary *> &holeBoundaries, std::map<int, Cell> &intSpaceMap, c3dModelData &data)
{
	for(auto ocBoundary : holeBoundaries)
	{
		static int foo = 0;
		++foo;
		if(foo != 3)
			continue;

		//get triangles
		auto boundaryTris = findBoundaryTriangles(ocBoundary, data);

		for(auto dt: boundaryTris)
		{
			data.colorTriangles.insert(ht2Triangle(dt));
		}


		std::map<Triangle *, bool> triBackup;

		//sculpture Triangleset
		sculptureTriangleSetHeuristic(boundaryTris, triBackup, data);


		//reset edges..
		for(auto reset: triBackup)
		{
			reset.first->setExists(reset.second);
		}


		return;

		//find boundaryEdges
		auto boundaryEdges = getBoundaryEdges(boundaryTris, data);

		//reset edges..
		//for(auto reset: triBackup)
		//{
		//	reset.first->setExists(reset.second);
		//}

		//determine hole-triangulation:
		//auto coverTriangles = createHullFromBoundaryEdges(boundaryEdges, data);

		//check if the cover of the hole is watertight
		//checkHoleCover(coverTriangles, boundaryEdges);

	}
}

void createHullOCBoundary(std::list<OCBoundary *> &holeOCBs, c3dModelData &data)
{
	for (std::list<OCBoundary *>::iterator ocbIter = holeOCBs.begin(); ocbIter != holeOCBs.end(); ocbIter++)
	{
		OCBoundary *currOCB = *ocbIter;


		// merge vertex sets of all CCs for OCB
		std::set<DVertex> vertexSet;
		getCCVertexSetForOCB(currOCB, vertexSet);

		// create hull for that vertex set
		std::set<Triangle *> boundaryTriangleSet;

		for (std::list<CC *>::iterator iter = currOCB->ccList.begin(); iter != currOCB->ccList.end(); iter++)
		{
			std::set<DTriangle> boundaryHTSet;
			getCCBoundaryHTSet(*iter, boundaryHTSet);

			for (std::set<DTriangle>::iterator iter = boundaryHTSet.begin(); iter != boundaryHTSet.end(); iter++)
				boundaryTriangleSet.insert(ht2Triangle(*iter));
		}

		std::set<Triangle *> hullTriangleSet = createHull(vertexSet, boundaryTriangleSet, data);

		// add the triangles of the hull to the triangulation
		int addedTriangleCount = 0;

		for (std::set<Triangle *>::iterator triIter = hullTriangleSet.begin(); triIter != hullTriangleSet.end(); triIter++)
		{
			Triangle *currTri = *triIter;

			if (!currTri->exists())
				addedTriangleCount++;

			currTri->setExists(true);
		}

	}

}


/*
 * create hull for vertex set
 */
std::set<Triangle *> createHull(std::set<DVertex> &vertexSet, std::set<Triangle *> &boundaryTriangleSet, c3dModelData &data)
{
	int i;

	// determine set of edges for set of CC incident to OCB
	std::set<Edge *> boundaryEdgeSet;

	for (std::set<Triangle *>::iterator iter = boundaryTriangleSet.begin(); iter != boundaryTriangleSet.end(); iter++)
	{
		Triangle *currTri = *iter;

		for (i = 0; i < 3; i++)
			boundaryEdgeSet.insert(currTri->edge(i));
	}

	// for all vertices, put all incident triangles into candidate set
	std::set<Triangle *> candTriangleSet, hullTriangleSet;

	for (std::set<DVertex>::iterator iter = vertexSet.begin(); iter != vertexSet.end(); iter++)
	{
		DVertex currVH = *iter;
		std::list<DTriangle> triangles;
		data.dt->incident_facets(currVH, back_inserter(triangles));

		for (std::list<DTriangle>::iterator htIter = triangles.begin(); htIter != triangles.end(); htIter++)
		{
			DTriangle currHT = *htIter;
			Triangle *currTri = ht2Triangle(currHT);

			// then select those with all vertices in vertexSet
			i = 0;

			while ((i < 3) && (vertexSet.find(currTri->vertex(i)) != vertexSet.end()))
				i++;

			if (i == 3)
				candTriangleSet.insert(currTri);
		}
	}


	// equals previous createHull() result:
	hullTriangleSet = candTriangleSet;

	// remove all triangles with external edges (1 incident triangles) from hull set (recursively):

	// determine all added triangles, of those all with external edges
	std::list<Triangle *> hullAddedTriangleList;
	std::set<Triangle *> externalTriangleSet;

	for (std::set<Triangle *>::iterator iter = hullTriangleSet.begin(); iter != hullTriangleSet.end(); iter++)
	{
		Triangle *currTri = *iter;

		// determine new triangles in hull and add that set to triangulation
		if (!currTri->exists())
		{
			hullAddedTriangleList.push_back(currTri);
			currTri->setExists(true);

			if (triangleHasExternalEdge(currTri, data))
				externalTriangleSet.insert(currTri);
		}
	}

	// while triangles with external edges left
	while (externalTriangleSet.size() > 0)
	{
		// remove triangle from set
		std::set<Triangle *>::iterator iter = externalTriangleSet.begin();
		Triangle *currTri = *iter;
		externalTriangleSet.erase(iter);

		// test if it still exists and has an external edge
		if (currTri->exists() && triangleHasExternalEdge(currTri, data))
		{
			// if so, remove from triangulation, from hull set
			currTri->setExists(false);
			hullTriangleSet.erase(currTri);

			// put incident triangles to its other edges into set
			for (i = 0; i < 3; i++)
			{
				Edge *currEdge = currTri->edge(i);
				DEdge currHE = currEdge->dEdge();
				Dt::Facet_circulator startFC = data.dt->incident_facets(currHE);
				Dt::Facet_circulator currFC = startFC;

				// iterate over all incident triangles to that edge
				do
				{
					DTriangle currHT = *currFC;
					Triangle *edgeCurrTri = ht2Triangle(currHT);

					if (edgeCurrTri->exists() && (edgeCurrTri != currTri) && triangleHasExternalEdge(currTri, data))
						externalTriangleSet.insert(edgeCurrTri);

					currFC++;
				} while (currFC != startFC);
			}
		}
	}

	// restore triangulation: remove the added set
	for (std::list<Triangle *>::iterator iter = hullAddedTriangleList.begin(); iter != hullAddedTriangleList.end(); iter++)
		(*iter)->setExists(false);

	return hullTriangleSet;
}


bool triangleHasExternalEdge(Triangle *triangle, c3dModelData &data)
{
	int i = 0;

	while ((i < 3) && (countEdgeTriangles(triangle->edge(i), data) != 1))
		i++;

	return (i < 3);
}

/*
 * determines incident existing triangle count per edge
 */
int countEdgeTriangles(Edge *edge, c3dModelData &data)
{
	DEdge he = edge->dEdge();
	Dt::Facet_circulator startFC = data.dt->incident_facets(he);
	Dt::Facet_circulator currFC = startFC;
	int incidentTriangleCount = 0;

	do
	{
		DTriangle currDT = *currFC;
		Triangle *currTri = ht2Triangle(currDT);

		if (currTri->exists())
			incidentTriangleCount++;

		currFC++;
	} while (currFC != startFC);

	return incidentTriangleCount;
}
