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

class Triangle;
struct c3dModelData;

bool is_Finite(c3dModelData &data, Cell c);


/*
 * calculate criterion for triangle (minimizing)
 */
double getCriterionForTriangle(Triangle *triangle, c3dModelData &data);


/*
 * label contiguous spaces
 */
void labelContiguousSpaces(int &tetraLabel, c3dModelData &data);


/*
 * get sets of entities for all and return whether any are open or non-conform
 */
bool classifyEntitiesAll(c3dModelData &data);


/*
 * classify entities as open/closed and for open edges+vertices, whether they are non-conform
 */
void classifyEntities(std::vector<Triangle *> &triangleSet, std::vector<Edge *> &edgeSet, std::vector<DVertex> &vertexSet, c3dModelData &data);


/*
 * classify entities for cells: as open/closed and for open edges+vertices, whether they are non-conform
 * return a tuple of the vertices, edges and triangles contained in the cells
 */
std::tuple<std::vector<DVertex>,  std::vector<Edge *>, std::vector<Triangle *> >
classifyEntitiesForCells(std::list<Cell> &cells, c3dModelData &data);

/*
 * classify entities for cells: as open/closed and for open edges+vertices, whether they are non-conform, return sets of new non conforming vertices and edges
 */
void classifyEntitiesForCells(std::list<Cell> &cells, std::set<Edge *> &newNCEdgeSet, std::set<DVertex> &newNCVertexSet, c3dModelData &data);


/*
 * classify entities for all OC-boundaries: as open/closed and for open edges+vertices, whether they are non-conform
 */
void classifyEntitiesForOCBs(std::list<OCBoundary *> &holeOCBs, c3dModelData &data);


/*
 * determine if edge is open and sets halftriangle to one in that adjacent S_0 if it is
 */
bool determineIfEdgeOpen(Edge *edge, DTriangle &ht, c3dModelData &data);

/*
 * determine if halfedge is open and sets halftriangle to one in that adjacent S_0 if it is
 */
bool determineIfHEOpen(DEdge he, DTriangle &ht, c3dModelData &data);


/*
 * tests if (open) edge is conforming:
 * edge is incident to a deflated triangle and one SB-loop, or two SB-loops
 */
bool isEdgeNonConforming(Edge *edge, c3dModelData &data);


/*
 * determines how many open triangles - incident to space #0 - are incident to that edge
 */
int incidentOpenTriangleCount(Edge *edge, c3dModelData &data);


/*
 * locate the next incident CC in ccw orientation about the given edge
 * requires an incident open tetrahedron
 * returns true if a CC located and then sets startHT and endHT
 * set startHT to the next triangle with CC tetrahedra following
 * return endHT to the next triangle after startHT with open tetrahedra following
 */
bool locateIncidentCC(DEdge he, int labelOC, DTriangle &startHT, DTriangle &endHT, c3dModelData &data);


/*
 * determines if vertex is incident to two or more only vertex-connected tetrahedra complexes (single vertex OC) - not edge-, or face-connected
 * if set of incident triangles is not a single edge-connected set
 */
bool isVertexConnectedOC(DVertex currVH, c3dModelData &data);


/*
 * determine if vertex has incident open edge (implies open triangle)
 */
bool isVertexInOpenEdgeOrTriangle(DVertex vh, c3dModelData &data);

/*
 * determines if vertex is open if not incident to open edge or triangle: traverse spaces and test if S_0 is encountered twice
 * NOTE: need only test this if not already incident to open edge or triangle
 */
bool isVertexDeflated(DVertex currVH, c3dModelData &data);


/*
 * at vertex: add incident tetrahedron to incident halftriangle boundary by XORing its incident triangles in the existing/non-existing sets
 */
void updateBoundary(DVertex vh, Cell ch, std::set<DTriangle> *boundaryHTSet, c3dModelData &data);

/*
 * determines whether an open vertex is in a manifold OC (locally, for vertices incident by edges in DG)
 */
bool isOpenVertexManifold(DVertex openVH, c3dModelData &data);


/*
 * reset all entities (vertices, edges, triangles) open/closed state to original (closed)
 * also set vertices to non-hole-adjacent
 * reset tetrahedra labels
 */
void resetAllEntities(c3dModelData &data);

/*
 * reset the labels of all tetras
 */
void resetAllTetraLabels(c3dModelData &data);



/*
 * DEBUG: output all existing triangles at vertex, and whether open or closed
 */
void debugOutputVertex(int vIndex, c3dModelData &data);


