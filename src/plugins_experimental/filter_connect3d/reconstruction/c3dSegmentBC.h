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


struct SeparatorBoundary;

/*
 * determine boundaries between closed and open components in RC
 */
void segmentClosedComponents(btree::btree_map<int, SeparatorBoundary> &sbMap, c3dModelData &data);


/*
 * determine (triangle set) boundaries of the SBs, detect if they are in a multiply connected SB group and assign them to one
 */
void determineSeparatorBoundaries(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<SBGroup> &sbGroupList, c3dModelData &data);

/*
 * locate OCBs (in single or multiple SBs)
 */
void locateOCBs(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<OCBoundary> &ocBoundaries, std::list<CC> &ccList, c3dModelData &data);


/*
 * determine vertex sets for CCs
 */
void determineCCVertexSet(std::list<CC> &ccList, c3dModelData &data);

/*
 * get boundary HT set for CC
 */
void getCCBoundaryHTSet(CC *cc, std::set<DTriangle> &boundaryHTSet);


/*
 * get SB group set for CC
 */
void getSBGroupSetForCC(CC *cc, std::set<SBGroup *> &sbGroupSet);


/*
 * locate all loops in SBs and determines loop-candidate edges
 * first select all loop-edges (unique in SB), then index them by vertices, and follow loops until all exhausted
 * loop-candidate edges are either in one SB-loop or a SB-tree edge with an incident deflated triangle
 */
void locateSBLoops(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<std::list<SBElem> > &loops,
		std::map<Edge *, std::pair<SBElem, SBElem> > &sbTreeMap, std::set<Edge *> &sbLoopSet, c3dModelData &data);


/*
 * combine overlapping SB-loops to OCBs
 * start with a loop, add all corresponding elements on other OC-side, and continue with remaining elements of their loops, until exhausted
 */
void combineSBLoopsToOCBs(btree::btree_map<int, SeparatorBoundary> &sbMap, std::list<std::list<SBElem> > &loops,
		std::list<OCBoundary> &ocBoundaries, std::list<CC> &ccList, c3dModelData &data);


/*
 * determine SE which still lack a OCB loop
 */
void locateLoopCandidates(std::map<Edge *, std::pair<SBElem, SBElem> > &sbTreeMap, std::set<Edge *> sbLoopSet, 
						  std::map<Edge *, std::pair<SBElem, SBElem> > &loopCandidateMap, c3dModelData &data);


/*
 * create OCBs from loop candidates
 * while loop candidate edges remain, follow them on umbrellas at S_0 to next candidate edge, to complete OCBs
 */
void createOCBsFromLoopCandidates(btree::btree_map<int, SeparatorBoundary> &sbMap, std::map<Edge *, std::pair<SBElem, SBElem> > &loopCandidateMap,
		std::list<OCBoundary> &ocBoundaries, std::list<CC> &ccList, c3dModelData &data);


/*
 * locate next element in OCB
 */
SBElem locateNextOutsideElem(SBElem startElem, DVertex currVH, std::set<Edge *> *edgeSet, bool isSBLoop, c3dModelData &data);


/*
 * returns the element in B_e(CC) incident to that edge, on the other side of a tetrahedra fan inside that CC
 */
SBElem getCCOppositeHT(SBElem elem, int labelOC, c3dModelData &data);


/*
 * return CC for SB if one has been assigned for any in its SBGroup
 */
CC *getCCForSB(SeparatorBoundary *sb);


/*
 * merge the two CCs in the set
 */
void mergeCCs(std::set<CC *> &ccSet);


/*
 * locate CC, or create it if it does not yet exist (and add it to CC list)
 */
CC *getOrCreateCCForSB(SeparatorBoundary *currSB, std::list<CC> &ccList);


/*
 * traverse to next element in umbrella with an edge from set
 * return the element before that one
 */
SBElem locateNextEdgeInSet(SBElem &currElem, std::set<Edge *> *edgeSet, c3dModelData &data);


/*
 * compare halfedges if they reference to the same edge (= same vertex pair)
 */
bool equalsHE(DEdge he0, DEdge he1);


/*
 * returns other element of CC incident to that edge, adjacent to the OC space
 */
SBElem getCCInsideHT(SBElem elem, int labelOC, c3dModelData &data);

/*
 * returns the index of the halfedge in the given halftriangle
 */
int getHEIndex(DTriangle ht, DEdge he);


/*
 * return halfedge with given index for the halftriangle
 */
DEdge getHTEdge(DTriangle currHT, int index);


/*
 * return vertex with given index for the halftriangle
 */
DVertex getHTVertex(DTriangle ht, int index);


/*
 * determine boundary edges for CCs (both halfedges for each)
 */
void determineCCBoundaryEdges(std::map<IntPair, DEdge> &heMap, c3dModelData &data);


/*
 * returns the index of the (incident) halftriangle in tetrahedron for the halfedge with same orientation
 */
int getHTIndex(DEdge he);


/*
 * returns next RHS halftriangle about that halfedge (and changes halfedge to the one in the new tetrahedron)
 * against the direction which the halftriangle faces (inside the tetrahedron)
 */
void nextRHSHalftriangle(DEdge &he, DTriangle &ht, c3dModelData &data);


/*
 * returns index in tetrahedon not in halfedge, or halftriangle
 * condition: both entities are contained in the same tetrahedron
 */
int determineRemainingIndex(DEdge he, DTriangle ht);

/*
 * return index pair for vertices in halfedge
 */
IntPair vIndexPair(DEdge he);


/*
 * determine a triangle adjacent to OC space which is unmarked and not open, starts from given one
 * returns true if one exists
 */
bool determineUnmarkedCCBoundaryHT(DEdge startHE, int labelOC, DTriangle &currHT, c3dModelData &data);


/*
 * return vertex with given index for the halfedge
 */
DVertex getHEVertex(DEdge he);


/*
 * determine next boundary element in SB on B_e(CC)
 */
void determineNextCCBoundaryEdge(SBElem &currElem, int labelSB, c3dModelData &data);


/*
 * return next existing halftriangle (with edge) in umbrella
 */
SBElem nextUmbrellaElem(SBElem elem, c3dModelData &data);


/*
 * return next existing halftriangle (with edge) in umbrella (using flag onlyCC)
 */
SBElem nextUmbrellaElemBase(SBElem elem, bool onlyCC, c3dModelData &data);