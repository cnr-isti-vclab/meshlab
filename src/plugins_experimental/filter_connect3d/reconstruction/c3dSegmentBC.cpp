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


void segmentClosedComponents(btree::btree_map<int, SeparatorBoundary> &sbMap, c3dModelData &data)
{
	// collect all open edges with non-open incident triangles
	std::map<IntPair, DEdge> boundaryHEMap;
	determineCCBoundaryEdges(boundaryHEMap, data);
	int labelSB = 0;

	// while boundary edges left
	while (boundaryHEMap.size() > 0)
	{
		// remove any edge from map to start a CC boundary
		std::map<IntPair, DEdge>::iterator mapIter = boundaryHEMap.begin();
		DEdge startHE = mapIter->second;

		// traverse CC boundary until loop closed (next edge in umbrella with an unmarked CC-triangle (or marked as same CC)
		SBElem currElem;
		SeparatorBoundary sbInstance;
		sbInstance.label = labelSB;
		sbMap[labelSB] = sbInstance;
		SeparatorBoundary *sb = &sbMap[labelSB];

		// determine start halftriangle
		DTriangle currHT;
		determineIfHEOpen(startHE, currHT, data);	// already known to be open, just to get triangle and label

		// determine a triangle adjacent to OC space which is unmarked and not open
		determineUnmarkedCCBoundaryHT(startHE, 0, currHT, data);

		// create boundary element
		locateHEInCell(startHE, currHT.first);
		currElem = SBElem(currHT, startHE);
		SBElem startElem = currElem;

		// while boundary is not a loop
		do
		{

			// add edge with incident boundary triangle to boundary
			sb->boundary.push_front(currElem);

			// also add vertex in SB into its boundary vertex set
			DVertex currVH = getHEVertex(currElem.edge());
			sb->sepBVertexSet.insert(currVH);

			{
				// determine next in the (locally manifold) boundary triangulation of the OC-adjacent space
				determineNextCCBoundaryEdge(currElem, labelSB, data);
			}

			// mark the adjacent CC-triangle with CC label, remove edge from set if no unmarked CC-triangles remain
			DEdge currHE = currElem.edge();
			DTriangle currHT = currElem.triangle();
			ht2Triangle(currHT)->setLabel(labelSB);
			bool found = determineUnmarkedCCBoundaryHT(currHE, 0, currHT, data);

			if (!found)
				boundaryHEMap.erase(vIndexPair(currHE));

		} while (currElem.edge() != startElem.edge());

		labelSB++;

	}
}


void determineCCBoundaryEdges(std::map<IntPair, DEdge> &heMap, c3dModelData &data)
{
	for (auto iter = data.dtEdgeList.begin(); iter != data.dtEdgeList.end(); iter++)
	{
		Edge *currEdge = &(*iter);

		if (currEdge->isOpen())
		{
			DEdge currHE = currEdge->dEdge();
			DEdge hePair[2] = { currHE, currHE };
			std::swap(hePair[1].second, hePair[1].third);
			int openTriCount = 0, closedTriCount = 0;
			DTriangle startHT(currHE.first, getHTIndex(currHE));
			DTriangle currHT = startHT;

			// iterate over all existing triangles in RHS orientation
			do
			{
				// determine current halftriangle
				nextRHSHalftriangle(currHE, currHT, data);
				Triangle *currTri = ht2Triangle(currHT);

				if (currTri->exists())
				{
					// count the incident open triangles
					if (currTri->isOpen())
						openTriCount++;
					else
						closedTriCount++;
				}
			}
			while (currHT != startHT);

			if (closedTriCount > 0)
			{
				// insert halfedge
				IntPair pair = vIndexPair(currHE);
				heMap[pair] = currHE;

				// insert opposite halfedge
				std::swap(pair.first, pair.second);
				std::swap(currHE.second, currHE.third);
				heMap[pair] = currHE;
			}
		}
	}
}


int getHTIndex(DEdge he)
{
	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 3; j++)
			if ((he.second == tetraTriVertexIndices[i][j]) && (he.third == tetraTriVertexIndices[i][(j + 1) % 3]))
				return i;

	return 4;
}


void nextRHSHalftriangle(DEdge &he, DTriangle &ht, c3dModelData &data)
{
	// old tetrahedron
	Cell oldCH = ht.first;

	// determine vertex in halftriangle opposite to halfedge, in old tetrahedron
	DVertex oppVH = oldCH->vertex(determineRemainingIndex(he, ht));

	// opposite halftriangle in new tetrahedron
	DTriangle nextHT = data.dt->mirror_facet(ht);

	// new tetrahedron
	Cell newCH = nextHT.first;

	// return next halftriangle in new tetrahedron
	ht.first = newCH;
	ht.second = newCH->index(oppVH);

	// return same halfedge in new tetrahedron
	locateHEInCell(he, newCH);
}


int determineRemainingIndex(DEdge he, DTriangle ht)
{
	int indices[3] = { ht.second, he.second, he.third };

	return determineRemainingIndex(indices);
}


IntPair vIndexPair(DEdge he)
{
	Cell ch = he.first;

	return IntPair(ch->vertex(he.second)->info().index, ch->vertex(he.third)->info().index);
}


bool determineUnmarkedCCBoundaryHT(DEdge startHE, int labelOC, DTriangle &currHT, c3dModelData &data)
{
	Dt::Facet_circulator startFC = data.dt->incident_facets(startHE, currHT);
	Dt::Facet_circulator currFC = startFC;
	DTriangle oppHT;
	bool found = false;

	do
	{
		currHT = *currFC;

		Triangle *currTri = currHT.first->info().triangle(currHT.second);
		oppHT = data.dt->mirror_facet(currHT);
		found = ((currTri->exists() && !currTri->isOpen() && (oppHT.first->info().label == labelOC)) && (currTri->label() == -1));
		currFC++;
	} while ((currFC != startFC) && !found);

	currHT = oppHT;

	return found;
}


DVertex getHEVertex(DEdge he)
{
	return he.first->vertex(he.second);
}


void determineNextCCBoundaryEdge(SBElem &currElem, int labelSB, c3dModelData &data)
{
	// traverse CC triangles in umbrella, until open edge found (may be the original edge) with inside triangle not yet in a CC or in the same one

	SBElem prevElem;
	bool found = false;

	// determine next open edge not already in boundary for that umbrella
	while (!found)
	{

		prevElem = currElem;
		currElem = nextUmbrellaElem(currElem, data);
		Edge *currEdge = he2Edge(currElem.edge());
		int label = ht2Triangle(prevElem.triangle())->label();
		found = (currEdge->isOpen() && ((label == -1) || (label == labelSB)));
	}

	// new boundary element consists of reversed current halfedge and previous halftriangle
	DEdge currHE = heInCell(prevElem.triangle().first, currElem.edge());
	currElem = SBElem(prevElem.triangle(), currHE);
	currElem.reverseHE();
}


SBElem nextUmbrellaElem(SBElem elem, c3dModelData &data)
{
	return nextUmbrellaElemBase(elem, false, data);
}


SBElem nextUmbrellaElemBase(SBElem elem, bool onlyCC, c3dModelData &data)
{
	DTriangle currHT = elem.triangle();
	DEdge currHE = elem.edge();

	int prevIndex = currHE.third;

	// point edge towards remaining vertex in triangle
	currHE.third = determineRemainingIndex(currHE, currHT);

	// determine other triangle in tetrahedron incident to new edge
	currHT.second = prevIndex;

	Triangle *currTri = ht2Triangle(currHT);

	// while not suitable triangle found, navigate to next RHS about that edge
	while (!currTri->exists() || (onlyCC && currTri->isOpen()))
	{
		nextRHSHalftriangle(currHE, currHT, data);
		currTri = ht2Triangle(currHT);
	}

	return SBElem(currHT, currHE);
}


void determineSeparatorBoundaries(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<SBGroup> &sbGroupList, c3dModelData &data)
{
	int i;

	// construct set of SB indices and create their boundary vertex sets
	std::set<int> sbSet;

	for (auto iter = sbMap.begin(); iter != sbMap.end(); iter++)
		sbSet.insert(iter->first);

	// while set not empty, determine boundaries of SBs and which SBs to join
	while (sbSet.size() > 0)
	{
		// remove a SB from the set
		std::set<int>::iterator iter = sbSet.begin();
		int labelSB = *iter;
		sbSet.erase(*iter);

		std::multimap<IntPair, SBElem> elemMMap;	// multimap necessary because halfedges can exist multiply in a SB (same CC incident multiple times at that edge)
		std::set<int> otherLabelSet;
		SeparatorBoundary *sb = &sbMap[labelSB];

		// create SBGroup
		SBGroup sbGroupInstance;
		sbGroupList.push_back(sbGroupInstance);
		SBGroup *sbGroup = &sbGroupList.back();

		// start with a boundary as map of elements, indexed by the pair of vertex indices of the halfedge
		for (std::list<SBElem>::iterator iter = sb->boundary.begin(); iter != sb->boundary.end(); iter++)
		{
			SBElem currElem = *iter;
			IntPair pair = vIndexPair(currElem.edge());
			elemMMap.insert(ElemPair(pair, currElem));
		}

		// get label of OC
		DTriangle anyHT = elemMMap.begin()->second.triangle();
		int labelOC = anyHT.first->info().label;

		// add triangles, until boundary collapses
		while (elemMMap.size() > 0)
		{
			// remove an element from the map
			std::multimap<IntPair, SBElem>::iterator iter = elemMMap.begin();
			SBElem elem = iter->second;
			elemMMap.erase(iter);

			DTriangle currHT = elem.triangle();
			DEdge outsideHE = elem.edge();
			int outsideIndex = getHEIndex(currHT, outsideHE);
			int removedCount = 0;

			// mark triangle as in that SB
			Triangle *currTri = ht2Triangle(currHT);

			// test for label (triangles incident to SB are already labeled)
			int currLabel = currTri->label();
			bool otherSBTouched = ((currLabel != -1) && (currLabel != labelSB));

			if (otherSBTouched)
			{

				assert((sbMap[currLabel].sbGroup == NULL) || (sbMap[currLabel].sbGroup->sbMap.size() == 0));	// make sure that SB does not already belong to another SBGroup

				otherLabelSet.insert(currLabel);	// other SB encountered (label will be overwritten)
			}

			currTri->setLabel(labelSB);

			// add halftriangle on inside of B_e(CC) to enclosing boundary triangle set
			sbGroup->boundaryHTSet.insert(data.dt->mirror_facet(currHT));

			// test remaining two edges
			for (i = 0; i < 3; i++)
				if (i != outsideIndex)
				{
					DEdge currHE = getHTEdge(currHT, i);
					IntPair pair = vIndexPair(currHE);
					std::multimap<IntPair, SBElem>::iterator mapIter = elemMMap.find(pair);

					while ((mapIter != elemMMap.end()) && (mapIter->first == pair) && (mapIter->second.triangle() != currHT))
						mapIter++;

					if ((mapIter != elemMMap.end()) && (mapIter->first == pair))
					{
						// halfedge exists in boundary: remove
						elemMMap.erase(mapIter);
						removedCount++;
					}
					else
					{
						// halfedge does not exist in boundary: add
						bool addEdge = true;

						if (otherSBTouched)
						{
							// test if edge is in other SB, then do not add
							SBElem otherElem(currHT, currHE);
							SeparatorBoundary *otherSB = &sbMap[currLabel];
							std::list<SBElem>::iterator sbIter = otherSB->find(otherElem);
							addEdge = (sbIter == otherSB->boundary.end());
						}

						if (addEdge)
						{
							// determine next halftriangle pointing inside on B_e(CC) - inside that CC, until one reached adjacent to same OC space
							DTriangle oppHT, newHT = currHT;
							DEdge newHE = currHE;
							Triangle *curr2Tri = NULL;

							do
							{
								nextRHSHalftriangle(newHE, newHT, data);
								curr2Tri = ht2Triangle(newHT);
								oppHT = data.dt->mirror_facet(newHT);

							} while (!curr2Tri->exists() || (oppHT.first->info().label != labelOC));	// orig: !x == y

							// flip halftriangle and add
							newHE = heInCell(oppHT.first, currHE);
							std::swap(newHE.second, newHE.third);
							elemMMap.insert(ElemPair(vIndexPair(newHE), SBElem(oppHT, newHE)));

						}
					}
				}

			if (removedCount == 0)
				sbGroup->intBVertexSet.insert(getHTVertex(currHT, outsideIndex));	// is interior vertex in enclosing boundary
		}

		// insert all SBs into SBGroup
		sbGroup->sbMap[labelSB] = sb;
		sb->sbGroup = sbGroup;


		for (std::set<int>::iterator iter = otherLabelSet.begin(); iter != otherLabelSet.end(); iter++)
		{
			SeparatorBoundary *currSB = &sbMap[*iter];
			sbGroup->sbMap[currSB->label] = currSB;
			currSB->sbGroup = sbGroup;

			// remove from sbSet once joined
			sbSet.erase(currSB->label);

			// remove their boundary vertex sets from interior boundary vertex set
			btree::btree_set<DVertex> interiorVertexSet;
			std::insert_iterator<btree::btree_set<DVertex> > iiInterior(interiorVertexSet, interiorVertexSet.begin());
			std::set_difference(sbGroup->intBVertexSet.begin(), sbGroup->intBVertexSet.end(), currSB->sepBVertexSet.begin(), currSB->sepBVertexSet.end(), iiInterior);
			sbGroup->intBVertexSet = interiorVertexSet;

		}

	}
}


int getHEIndex(DTriangle ht, DEdge he)
{

	assert(ht.first == he.first);

	int i, tIndex = ht.second;

	for (i = 0; i < 3; i++)
		if ((he.second == tetraTriVertexIndices[tIndex][(i + 1) % 3]) && (he.third == tetraTriVertexIndices[tIndex][(i + 2) % 3]))
			return i;

	return 3;
}


DVertex getHTVertex(DTriangle ht, int index)
{
	return ht.first->vertex(tetraTriVertexIndices[ht.second][index]);
}


DEdge getHTEdge(DTriangle currHT, int index)
{
	int tIndex = currHT.second;
	DEdge he(currHT.first, tetraTriVertexIndices[tIndex][(index + 1) % 3], tetraTriVertexIndices[tIndex][(index + 2) % 3]);

	return he;
}


void locateOCBs(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<OCBoundary> &ocBoundaries, std::list<CC> &ccList, c3dModelData &data)
{
	std::list<std::list<SBElem> > loops;
	std::set<Edge *> sbLoopSet;
	std::map<Edge *, std::pair<SBElem, SBElem> > sbTreeMap, loopCandidateMap;

	// locate all loops in SBs
	locateSBLoops(sbMap, loops, sbTreeMap, sbLoopSet, data);

	// combine overlapping SB-loops to OCBs
	combineSBLoopsToOCBs(sbMap, loops, ocBoundaries, ccList, data);

	// determine SE which still lack a OCB loop
	locateLoopCandidates(sbTreeMap, sbLoopSet, loopCandidateMap, data);

	// create OCBs from loop candidates
	createOCBsFromLoopCandidates(sbMap, loopCandidateMap, ocBoundaries, ccList, data);
}


/*
 * locate all loops in SBs and determines loop-candidate edges
 * first select all loop-edges (unique in SB), then index them by vertices, and follow loops until all exhausted
 * loop-candidate edges are either in one SB-loop or a SB-tree edge with an incident deflated triangle
 */
void locateSBLoops(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<std::list<SBElem> > &loops,
				   std::map<Edge *, std::pair<SBElem, SBElem> > &sbTreeMap, std::set<Edge *> &sbLoopSet, c3dModelData &data)
{
	int i;

	for (auto mapIter = sbMap.begin(); mapIter != sbMap.end(); mapIter++)
	{
		SeparatorBoundary *currSB = &mapIter->second;

		// split SB edges into loop and tree edges (loop edges are unique in the SB, tree edges exist 2*n times)
		std::map<Edge *, SBElem> loopEdgeMap;
		std::map<Edge *, std::pair<SBElem, SBElem> > treeEdgeMap;

		for (std::list<SBElem>::iterator sbIter = currSB->boundary.begin(); sbIter != currSB->boundary.end(); sbIter++)
		{
			SBElem currElem = *sbIter;
			Edge *currEdge = he2Edge(currElem.edge());
			std::pair<std::map<Edge *, SBElem>::iterator, bool> result = loopEdgeMap.insert(std::pair<Edge *, SBElem>(currEdge, currElem));

			if (!result.second)
			{
				// test if SB-tree edge already in map
				std::map<Edge *, std::pair<SBElem, SBElem> >::iterator treeMapIter = treeEdgeMap.find(currEdge);

				// only insert if not already in there (since if two SB-tree edges incident it cannot be a loop candidate)
				if (treeMapIter == treeEdgeMap.end())
					// already inserted element as first, current as second
					treeEdgeMap.insert(std::pair<Edge *, std::pair<SBElem, SBElem> >(currEdge, std::pair<SBElem, SBElem>(result.first->second, currElem)));

				// edge is contained twice in SB, therefore a SB-tree edge
				loopEdgeMap.erase(result.first);
			}
		}


		// index all edges by their vertices
		std::map<DVertex, std::set<Edge *> > veMap;

		for (std::map<Edge *, SBElem>::iterator map2Iter = loopEdgeMap.begin(); map2Iter != loopEdgeMap.end(); map2Iter++)
		{
			Edge *currEdge = map2Iter->first;

			for (i = 0; i < 2; i++)
				veMap[currEdge->vertex(i)].insert(currEdge);

			// put into SB-loop edge set
			sbLoopSet.insert(currEdge);
		}

		// create loops from the unique edges
		while (loopEdgeMap.size() > 0)
		{
			std::list<SBElem> loop;
			std::map<Edge *, SBElem>::iterator startIter = loopEdgeMap.begin();
			Edge *currEdge = startIter->first;
			SBElem currElem = startIter->second;
			DVertex startVH = currEdge->vertex(0);
			DVertex nextVH = currEdge->vertex(1);
			DVertex currVH = startVH;

			// remove edge from veMap at startVH
			std::map<DVertex, std::set<Edge *> >::iterator veIter = veMap.find(startVH);
			std::set<Edge *> *edgeSet = &veIter->second;
			edgeSet->erase(currEdge);

			do
			{
				// add current edge to loop
				loop.push_back(currElem);
				loopEdgeMap.erase(currEdge);

				// get edgeSet at next vertex
				veIter = veMap.find(nextVH);
				edgeSet = &veIter->second;

				currVH = nextVH;

				if (currVH != startVH)
				{

					// assert that there is always an edge to follow (aside from previous one)
					assert(edgeSet->size() > 1);

					if (edgeSet->size() == 2)
					{
						// determine which edge is the next one
						std::set<Edge *>::iterator edgeIter = edgeSet->begin();

						if (*edgeSet->begin() == currEdge)
							edgeIter++;

						edgeSet->erase(currEdge);	// erase current edge
						currEdge = *edgeIter;
						edgeSet->erase(currEdge);	// erase next edge
						currElem = loopEdgeMap[currEdge];
					}
					else
					{
						// several edges left, choose next outside one
						currElem = locateNextOutsideElem(currElem, nextVH, edgeSet, true, data);
						edgeSet->erase(currEdge);	// erase current edge
						currEdge = he2Edge(currElem.edge());
						edgeSet->erase(currEdge);	// erase next edge
					}

					// follow the next edge
					int nextVIndex = ((currEdge->vertex(0) == currVH) ? 1 : 0);
					nextVH = currEdge->vertex(nextVIndex);
				}
				else
					edgeSet->erase(currEdge);

				// if no incident edges left, erase also the reference vertex in the map
				if (edgeSet->size() == 0)
					veMap.erase(veIter);
			} while (currVH != startVH);

			loops.push_back(loop);
		}


		assert(veMap.size() == 0);	// assert that all edges are used


		// put into SB-tree edge set
		for (std::map<Edge *, std::pair<SBElem, SBElem> >::iterator map2Iter = treeEdgeMap.begin(); map2Iter != treeEdgeMap.end(); map2Iter++)
		{
			Edge *currEdge = map2Iter->first;
			sbTreeMap[currEdge] = map2Iter->second;
		}
	}
}

/*
 * combine overlapping SB-loops to OCBs
 * start with a loop, add all corresponding elements on other OC-side, and continue with remaining elements of their loops, until exhausted
 */
void combineSBLoopsToOCBs(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<std::list<SBElem> > &loops,
						  std::list<OCBoundary> &ocBoundaries, std::list<CC> &ccList, c3dModelData &data)
{

	std::set<std::list<SBElem> *> loopSet;
	std::map<DTriangle, std::list<SBElem> *> htLoopMap;

	for (std::list<std::list<SBElem> >::iterator loopIter = loops.begin(); loopIter != loops.end(); loopIter++)
	{
		// index all elems in the loops by their loop
		std::list<SBElem> *currLoop = &*loopIter;

		for (std::list<SBElem>::iterator iter = currLoop->begin(); iter != currLoop->end(); iter++)
		{
			SBElem currElem = *iter;
			htLoopMap[currElem.triangle()] = currLoop;
		}

		loopSet.insert(currLoop);	// also insert into set of loops
	}

	while (loopSet.size() > 0)
	{

		// create OC boundary
		OCBoundary ocbInstance;
		ocBoundaries.push_back(ocbInstance);
		OCBoundary *ocBoundary = &ocBoundaries.back();

		int side = 0;
		std::set<std::list<SBElem> *> oppLoopSet;
		std::set<Edge *> edgeSet;
		std::set<int> sbLabelSet;

		std::set<std::list<SBElem> *>::iterator loopIter = loopSet.begin();
		std::list<SBElem> *currLoop = *loopIter;
		loopSet.erase(loopIter);

		std::list<SBElem> currElems = *currLoop;	// copy first loop

		// put SB label into set
		int sbLabel = ht2Triangle(currElems.front().triangle())->label();
		sbLabelSet.insert(sbLabel);


		do
		{

			for (std::list<SBElem>::iterator iter = currElems.begin(); iter != currElems.end(); iter++)
			{
				SBElem currElem = *iter;
				ocBoundary->boundary[side].push_back(currElem);
				SBElem oppElem = getCCOppositeHT(currElem, 0, data);
				ocBoundary->boundary[1 - side].push_back(oppElem);
				edgeSet.insert(he2Edge(currElem.edge()));
				std::list<SBElem> *oppLoop = htLoopMap[oppElem.triangle()];
				oppLoopSet.insert(oppLoop);
			}

			// change side on OC
			side = 1 - side;

			// add new elements to list
			currElems.clear();

			for (std::set<std::list<SBElem> *>::iterator loopIter = oppLoopSet.begin(); loopIter != oppLoopSet.end(); loopIter++)
			{
				std::list<SBElem> *currLoop = *loopIter;
				std::set<std::list<SBElem> *>::iterator currLoopIter = loopSet.find(currLoop);

				if (currLoopIter != loopSet.end())
				{
					// add all elements not already in edge set
					for (std::list<SBElem>::iterator iter = currLoop->begin(); iter != currLoop->end(); iter++)
					{
						SBElem currElem = *iter;
						Edge *currEdge = he2Edge(currElem.edge());
						std::pair<std::set<Edge *>::iterator, bool> result = edgeSet.insert(currEdge);

						if (result.second)
							currElems.push_back(currElem);
					}

					// remove loop
					loopSet.erase(currLoopIter);

					// put SB label into set
					int sbLabel = ht2Triangle(currLoop->front().triangle())->label();
					sbLabelSet.insert(sbLabel);

				}
			}

			oppLoopSet.clear();

		} while (currElems.size() > 0);

		// assert that boundary is equal size on both OC-sides
		assert(ocBoundary->boundary[0].size() == ocBoundary->boundary[1].size());


		// assign CC for the SBs
		std::set<CC *> ccSet;

		for (std::set<int>::iterator sbIter = sbLabelSet.begin(); sbIter != sbLabelSet.end(); sbIter++)
		{
			SeparatorBoundary *currSB = &sbMap[*sbIter];

			// locate the CC for the SB
			CC *cc = getCCForSB(currSB);

			if (cc != NULL)
				ccSet.insert(cc);
		}

		// assert that not more than two different CC exist for the joined SBs
		assert(ccSet.size() <= 2);


		// if there are two CCs, join them
		if (ccSet.size() == 2)
			mergeCCs(ccSet);

		CC *cc = NULL;

		if (ccSet.size() == 1)
			cc = *ccSet.begin();

		for (std::set<int>::iterator sbIter = sbLabelSet.begin(); sbIter != sbLabelSet.end(); sbIter++)
		{
			SeparatorBoundary *currSB = &sbMap[*sbIter];

			if (cc == NULL)
				cc = getOrCreateCCForSB(currSB, ccList);

			// reference them both to each other
			cc->outsideSet.insert(currSB);	// assign new SB to CC
			currSB->cc = cc;	// and vice versa
		}

		ocBoundary->ccList.push_back(cc);	// for single-SB loops there is only one CC for the OCB

	}
}



void locateLoopCandidates(std::map<Edge *, std::pair<SBElem, SBElem> > &sbTreeMap, std::set<Edge *> sbLoopSet, std::map<Edge *, 
	std::pair<SBElem, SBElem> > &loopCandidateMap, c3dModelData &data)
{

	for (std::map<Edge *, std::pair<SBElem, SBElem> >::iterator iter = sbTreeMap.begin(); iter != sbTreeMap.end(); iter++)
	{
		Edge *currEdge = iter->first;

		// test if edge is contained in a deflated triangle
		int count = incidentOpenTriangleCount(currEdge, data);

		assert((count == 0) || (count == 1));	// assert that either 1 or 0 incident deflated triangles

		// test if edge is contained in a SB-loop
		if (sbLoopSet.find(currEdge) != sbLoopSet.end())
			count++;

		// is incident to either one deflated triangle, or to one SB-loop -> loop-candidate
		if (count == 1)
			loopCandidateMap[currEdge] = iter->second;
	}

}

/*
 * create OCBs from loop candidates
 * while loop candidate edges remain, follow them on umbrellas at S_0 to next candidate edge, to complete OCBs
 */
void createOCBsFromLoopCandidates(btree::btree_map<int, SeparatorBoundary> &sbMap, std::map<Edge *, std::pair<SBElem, SBElem> > &loopCandidateMap,
								  std::list<OCBoundary> &ocBoundaries, std::list<CC> &ccList, c3dModelData &data)
{
	int i;

	// index all edges by their vertices
	std::map<DVertex, std::set<Edge *> > veMap;

	for (std::map<Edge *, std::pair<SBElem, SBElem> >::iterator iter = loopCandidateMap.begin(); iter != loopCandidateMap.end(); iter++)
	{
		Edge *currEdge = iter->first;

		for (i = 0; i < 2; i++)
			veMap[currEdge->vertex(i)].insert(currEdge);
	}

	// create loops from the unique edges
	while (loopCandidateMap.size() > 0)
	{

		// create OC boundary
		OCBoundary ocbInstance;
		ocBoundaries.push_back(ocbInstance);
		OCBoundary *ocBoundary = &ocBoundaries.back();

		std::map<Edge *, std::pair<SBElem, SBElem> >::iterator startIter = loopCandidateMap.begin();
		Edge *currEdge[2], *startEdge = startIter->first;
		SBElem currElem[2] = { startIter->second.first, startIter->second.second };

		loopCandidateMap.erase(startIter);
		DVertex startVH = startEdge->vertex(0);
		DVertex nextVH = startEdge->vertex(1);
		DVertex currVH = startVH;
		currEdge[0] = startEdge;
		std::set<int> sbLabelSet;

		// remove edge from veMap at startVH
		std::map<DVertex, std::set<Edge *> >::iterator veIter = veMap.find(startVH);
		std::set<Edge *> *edgeSet = &veIter->second;
		edgeSet->erase(startEdge);

		do
		{
			// add current edge to loop (for both sides of OC)
			for (i = 0; i < 2; i++)
			{
				// add element to OCB
				ocBoundary->boundary[i].push_back(currElem[i]);

				// put SB label into set
				int sbLabel = ht2Triangle(currElem[i].triangle())->label();
				sbLabelSet.insert(sbLabel);
			}

			loopCandidateMap.erase(currEdge[0]);

			// remove edge from veMap at nextVH
			veIter = veMap.find(nextVH);
			edgeSet = &veIter->second;
			currVH = nextVH;

			if (currVH != startVH)
			{
				assert(edgeSet->size() > 1);	// assert that there is always an edge to follow (aside from previous one)

				Edge *prevEdge = currEdge[0];

				for (i = 0; i < 2; i++)
				{
					// follow umbrella until one of those edges
					currElem[i] = locateNextOutsideElem(ocBoundary->boundary[i].back(), currVH, edgeSet, false, data);
					currEdge[i] = he2Edge(currElem[i].edge());
				}

				// erase both incident edges at vertex in the map
				edgeSet->erase(prevEdge);
				edgeSet->erase(currEdge[0]);

				// follow the next edge
				int nextVIndex = ((currEdge[0]->vertex(0) == currVH) ? 1 : 0);
				nextVH = currEdge[0]->vertex(nextVIndex);
			}
			else
				edgeSet->erase(currEdge[0]);

			// if only a single edge left, remove it with its reference vertex
			if (edgeSet->size() == 0)
				veMap.erase(veIter);

		} while (currVH != startVH);

		// put unique set of CCs as list into OCB
		std::set<CC *> ccSet;

		for (std::set<int>::iterator iter = sbLabelSet.begin(); iter != sbLabelSet.end(); iter++)
		{
			SeparatorBoundary *currSB = &sbMap[*iter];
			CC *cc = getOrCreateCCForSB(currSB, ccList);

			// reference them both to each other
			cc->outsideSet.insert(currSB);	// assign new SB to CC
			currSB->cc = cc;	// and vice versa

			ccSet.insert(cc);
		}

		ocBoundary->ccList.insert(ocBoundary->ccList.begin(), ccSet.begin(), ccSet.end());



	}

	assert(veMap.size() == 0);	// assert that all edges used

}



SBElem locateNextOutsideElem(SBElem startElem, DVertex currVH, std::set<Edge *> *edgeSet, bool isSBLoop, c3dModelData &data)
{
	DEdge startHE = startElem.edge();
	bool direction = (startHE.first->vertex(startHE.second) == currVH);	// must be reverse: current vertex = start vertex of halfedge

	// if halfedge does not point away from currVH, reverse it
	if (!direction)
		startElem.reverseHE();

	SBElem currElem = startElem;
	SBElem prevElem = locateNextEdgeInSet(currElem, edgeSet, data);

	if (isSBLoop)
	{
		// not hole-adjacent: go to last edge in edgeSet
		SBElem prevPrevElem;

		do
		{
			prevPrevElem = prevElem;	// keep previous elem
			prevElem = locateNextEdgeInSet(currElem, edgeSet, data);

			// if triangle of prevElem is open, restore previous element, since it is not in a CC
			if (ht2Triangle(prevElem.triangle())->isOpen())
				prevElem = prevPrevElem;

		} while (!equalsHE(currElem.edge(), startElem.edge()));

		prevElem = prevPrevElem;
	}
	else
	// test if start edge is reached (hole-adjacent)
	if (equalsHE(currElem.edge(), startElem.edge()))
	{
		// currVH is hole-adjacent, locate an edge of the edgeSet in umbrella which occurs twice successively
		DEdge prevHE;

		do
		{
			prevHE = currElem.edge();
			prevElem = locateNextEdgeInSet(currElem, edgeSet, data);

			// assert that start edge is not reached again
			assert(!equalsHE(currElem.edge(), startHE));

		} while (!equalsHE(currElem.edge(), prevHE));
	}


	// put current edge into previous element
	prevElem.nextHEInHT();

	// change direction of halfedge to match previous one
	if (!direction)
		prevElem.reverseHE();

	assert(!ht2Triangle(prevElem.triangle())->isOpen());

	return prevElem;
}



SBElem getCCOppositeHT(SBElem elem, int labelOC, c3dModelData &data)
{
	SBElem insideElem = getCCInsideHT(elem, labelOC, data);

	// reverse both edge and triangle to be able to locate it in a SB
	DTriangle oppHT = data.dt->mirror_facet(insideElem.triangle());
	SBElem reversedInsideElem(oppHT, heInCell(oppHT.first, insideElem.edge()));
	reversedInsideElem.reverseHE();

	return reversedInsideElem;
}



CC *getCCForSB(SeparatorBoundary *sb)
{
	SBGroup *sbGroup = sb->sbGroup;

	for (auto iter = sbGroup->sbMap.begin(); iter != sbGroup->sbMap.end(); iter++)
	{
		SeparatorBoundary *currSB = iter->second;

		if (currSB->cc != NULL)
			return currSB->cc;
	}

	return NULL;
}


void mergeCCs(std::set<CC *> &ccSet)
{
	// get both CCs and erase second
	CC *cc[2];
	std::set<CC *>::iterator ccIter = ccSet.begin();
	cc[0] = *ccIter;
	ccIter++;
	cc[1] = *ccIter;
	ccSet.erase(ccIter);

	// update references of second CC (to SBs and back)
	for (auto iter = cc[1]->outsideSet.begin(); iter != cc[1]->outsideSet.end(); iter++)
	{
		SeparatorBoundary *currSB = *iter;
		cc[0]->outsideSet.insert(currSB);
		currSB->cc = cc[0];
	}

	// do not erase second CC, but empty it
	cc[1]->outsideSet.clear();
}


CC *getOrCreateCCForSB(SeparatorBoundary *currSB, std::list<CC> &ccList)
{
	CC *cc = getCCForSB(currSB);

	if (cc == NULL)
	{
		// create new CC
		CC ccInstance;
		ccList.push_back(ccInstance);
		cc = &ccList.back();
	}

	// cross-reference SB with CC
	cc->outsideSet.insert(currSB);	// assign new SB to CC
	currSB->cc = cc;	// and vice versa

	return cc;
}


SBElem locateNextEdgeInSet(SBElem &currElem, std::set<Edge *> *edgeSet, c3dModelData &data)
{
	SBElem prevElem;
	Edge *currEdge;
	bool found = false;

	// iterate umbrella in outside orientation until next edge found in edge set
	do
	{
		prevElem = currElem;	// save last element (will be used)

		// advance to next element in umbrella (on other side of OCB)
		currElem = nextUmbrellaElem(currElem, data);
		currEdge = he2Edge(currElem.edge());
		found = (edgeSet->find(currEdge) != edgeSet->end());

	} while (!found);

	return prevElem;
}


bool equalsHE(DEdge he0, DEdge he1)
{
	int i;
	DEdge he[2] = { he0, he1 };
	DVertex vertexPair[2][2];

	for (i = 0; i < 2; i++)
	{
		Cell ch = he[i].first;
		vertexPair[i][0] = ch->vertex(he[i].second);
		vertexPair[i][1] = ch->vertex(he[i].third);
		std::sort(&vertexPair[i][0], &vertexPair[i][0] + 2);
	}

	i = 0;

	while ((i < 2) && (vertexPair[0][i] == vertexPair[1][i]))
		i++;

	return (i == 2);
}


SBElem getCCInsideHT(SBElem elem, int labelOC, c3dModelData &data)
{
	DEdge currHE = elem.edge();
	DTriangle currHT = elem.triangle();
	DTriangle endHT = currHT;

	// rotate about edge inside CC until other incident triangle on its enclosing boundary found
	// assumes that startHT is pointing outward, and then nextRHS goes inside the CC
	do
	{
		nextRHSHalftriangle(currHE, endHT, data);
	} while (data.dt->mirror_facet(endHT).first->info().label != labelOC);

	return SBElem(endHT, heInCell(endHT.first, currHE));
}


void determineCCVertexSet(std::list<CC> &ccList, c3dModelData &data)
{
	int i;

	for (std::list<CC>::iterator ccIter = ccList.begin(); ccIter != ccList.end(); ccIter++)
	{
		CC *cc = &*ccIter;

		std::set<DTriangle> boundaryHTSet;
		getCCBoundaryHTSet(cc, boundaryHTSet);

		int tetrahedraCount = 0;

		// traverse tetrahedra inside CC to determine vertex set:
		// add triangles, until boundary collapses
		while (boundaryHTSet.size() > 0)
		{
			// remove a boundary HT from the set (equal to its tetrahedra from the CC)
			std::set<DTriangle>::iterator htIter = boundaryHTSet.begin();
			DTriangle currHT = *htIter;
			boundaryHTSet.erase(htIter);
			Cell currCH = currHT.first;
			tetrahedraCount++;
			int removedCount = 0;

			// XOR triangles in boundary
			for (i = 0; i < 4; i++)
				if (i != currHT.second)
				{
					DTriangle nextHT = DTriangle(currCH, i);
					std::set<DTriangle>::iterator iter = boundaryHTSet.find(nextHT);

					if (iter != boundaryHTSet.end())
					{
						// remove halftriangle
						boundaryHTSet.erase(iter);
						removedCount++;
					}
					else
					{
						// add halftriangle (inside adjacent tetrahedron)
						DTriangle oppHT = data.dt->mirror_facet(nextHT);
						boundaryHTSet.insert(oppHT);
					}
				}

			// if no halftriangles existed, opposite vertex is interior
			if (removedCount == 0)
				cc->interiorVertexSet.insert(currCH->vertex(currHT.second));
		}
	}
}



void getCCBoundaryHTSet(CC *cc, std::set<DTriangle> &boundaryHTSet)
{
	std::set<SBGroup *> sbGroupSet;
	getSBGroupSetForCC(cc, sbGroupSet);

	for (std::set<SBGroup *>::iterator iter = sbGroupSet.begin(); iter != sbGroupSet.end(); iter++)
	{
		SBGroup *sbGroup = *iter;
		boundaryHTSet.insert(sbGroup->boundaryHTSet.begin(), sbGroup->boundaryHTSet.end());
	}
}


void getSBGroupSetForCC(CC *cc, std::set<SBGroup *> &sbGroupSet)
{

	for (auto iter = cc->outsideSet.begin(); iter != cc->outsideSet.end(); iter++)
	{
		SeparatorBoundary *currSB = *iter;
		sbGroupSet.insert(currSB->sbGroup);

	}

}
