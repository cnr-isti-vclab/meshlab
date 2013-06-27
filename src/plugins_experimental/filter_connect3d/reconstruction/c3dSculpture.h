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

struct c3dModelData;

/*
 * sculpture tetrahedra from walls globally by heuristic (sorted by delta-rho criterion)
 */
void sculptureWallsHeuristic(std::map<int, Cell> &intSpaceMap, std::list<Cell> &manipulatedCells, c3dModelData &data);


/*
 * sculpture tetrahedra from a given triangleset by heuristic (sorted by delta-rho criterion)
 * TODO: merge with WallsHeuristic!
 */
void sculptureTriangleSetHeuristic(std::set<DTriangle> &triangleSet, std::map<Triangle *, bool> &triResetMap, c3dModelData &data);


/*
 * determine interior spaces: incident to single-umbrella vertices which are also in S_0, together with a representative contained tetrahedron
 * also return all spaces: interior spaces + S_0
 */
void determineSpaceMap(std::map<int, Cell> &allSpaceMap, std::map<int, Cell> &intSpaceMap, c3dModelData &data);


/*
 * test if vertex is manifold: has exactly one umbrella of existing triangles
 * return list of existing triangles (must be initialized empty)
 */
bool isVertexManifold(DVertex currVH, std::list<DTriangle> &existingTriangles, c3dModelData &data);


/*
 * return index of next ccw vertex in that halftriangle to the one with vIndex (both in range 0..3)
 */
int nextCCWInHT(DTriangle currHT, int vIndex);


/*
 * get set of boundary halftriangles for the given {S}
 * NOTE: removes all non-boundary triangles
 */
void getBoundaryHTSet(std::map<int, Cell> &allSpaceMap, std::set<DTriangle> &boundaryHTSet, c3dModelData &data);


/*
 * tests if tetrahedron for is removable (to sculpturing rules) from the given space
 * returns also count and indices of the triangles which are in the boundary of that space
 */
bool isTetrahedronRemovable(Cell ch, int boundaryLabel, int &boundaryTriCount, bool *inBoundary, c3dModelData &data);


/*
 * determine how many (and which) triangles of tetrahedron are in current boundary
 * returns triangle count in boundary and modifies 'inBoundary'
 */
int trianglesInBoundary(Cell ch, int label, bool *inBoundary, c3dModelData &data);


/*
 * returns the non-boundary edge for a tetrahedron with 2 triangles on boundary
 * CONDITION: exactly 2 triangles on boundary
 */
Edge *getNonBoundaryEdge(Cell ch, bool *inBoundary);


/*
 * tests if edge is incident to the space with the given label
 */
bool isEdgeIncidentToSpace(Edge *edge, int label, c3dModelData &data);


/*
 * returns the non-boundary vertex for a tetrahedron with 1 triangle on boundary
 * CONDITION: exactly 1 triangle on boundary
 */
DVertex getNonBoundaryVertex(Cell ch, bool *inBoundary);


/*
 * tests if vertex is incident to the space with the given label
 */
bool isVertexIncidentToSpace(DVertex vh, int label, c3dModelData &data);



/*
 * add tetrahedron to queue (both queue and reverse map)
 */
void addTetrahedronToPQ(Cell ch, int spaceLabel, bool *inBoundary, std::multimap<std::pair<bool, float>, std::pair<Cell, int> > &tetraMMap, 
						std::map<std::pair<Cell, int>, std::pair<bool, float> > &tetraReverseMap, c3dModelData &data);


/*
 * returns criterion for tetrahedron: longest boundary edge in tetrahedron
 * pre-condition: either 1 or 2 triangles of tetrahedron in boundary
 */
float getTetrahedronLongestBoundaryEdgeCriterion(Cell ch, bool *inBoundary, c3dModelData &data);


/*
 * remove tetrahedron from queue (both queue and reverse map)
 */
void removeTetrahedronFromPQ(Cell cell, int spaceLabel, std::multimap<std::pair<bool, float>, std::pair<Cell, int> > &tetraMMap,
		std::map<std::pair<Cell, int>, std::pair<bool, float> > &tetraReverseMap);


/*
 * determine the triangle with least circumradius in tetrahedron
 */
int determineLeastCRTriangleInTetrahedron(Cell ch, c3dModelData &data);


/*
 * pop the NBC bubbles:
 * - identify spaces not in {S} - NBCs, and for each:
 *   - determine how many of its covers has interior vertices
 *   - if all, select all but S_0 cover; if all but one, select that one; if none, select all but one with least criterion
 *   - remove select cover(s)
 */
void popNBCs(std::map<int, Cell> oldIntSpaceMap, std::list<Cell> &manipulatedCells, c3dModelData &data);


/*
 * determines if triangle set is edge-connected
 * PRE-CONDITION: set must not be empty
 */
bool isEdgeConnected(std::set<Triangle *> &triangleSet);


/*
 * determines if triangle set contains any walls
 */
bool containsWall(std::set<Triangle *> &triangleSet, c3dModelData &data);


/*
 * calculate curvature for triangle set (not edge-connected)
 * sum up curvature for all its edges (by its containing boundary triangles)
 */
float calculateKForTriangleSet(int nbcLabel, std::set<Triangle *> &triangleSet, c3dModelData &data);


/*
 * calculate curvature for edge between the two given triangles
 * PRE-CONDITION: triangles are incident to edge
 */
float calculateCurvature(Edge *edge, Triangle *t0, Triangle *t1, c3dModelData &data);


/*
 * calculates curvature for edge v1-v2 with the opposite vertices of the adjacent triangles v0 and v3 respectively
 */
float calculateCurvature(int v0, int v1, int v2, int v3, c3dModelData &data);


/*
 * calculate curvature for edge between the two given triangles
 * PRE-CONDITION: triangles share an edge
 */
float calculateCurvature(Triangle *t0, Triangle *t1, c3dModelData &data);


/*
 * DEBUG: verify that all vertices are manifold, that is they have exactly one umbrella
 * NOTE: by locally testing entities, it does not verify if the triangulation is a single connected component
 */
bool isBoundaryManifold(c3dModelData &data);


/*
 * DEBUG: verify that no vertices are NBC interior - that is, not in any BC triangles
 */
bool hasNBCInteriorVertices(std::map<int, Cell> &allSpaceMap, c3dModelData &data);


/*
 * DEBUG: verify that no vertices are interior - that is disconnected from any triangles
 */
bool hasInteriorVertices(c3dModelData &data);
