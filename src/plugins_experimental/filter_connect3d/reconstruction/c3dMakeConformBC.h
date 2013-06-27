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
 * fix non-conforming vertices and edges
 * vertex: not manifold in OC, or single vertex SB (inside CC or connecting CCs)
 * edge: >2 deflated triangles/CCs incident
 * NOTE: assumes that edges and triangles have been classified as open (and their vertices as being contained in such an open entity, but not by themselves!)
 */
void fixNonConformingEntities(int &tetraLabel, c3dModelData &data);


/*
 * add hull to edge (for vertex set of incident triangles, or all of DG neighborhood)
 */
void edgeAddHull(Edge *edge, bool addAllIncident, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data);


/*
 * add local hull (or all incident tetrahedra if addAllIncident=true)
 */
void vertexAddHull(DVertex vh, bool addAllIncident, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data);


/*
 * add limited hull to edge
 * there exist >2 deflated triangles + CCs, and as many sub-spaces in S_0: add all tetrahedra to all but 2 subspaces in S_0
 * returns true, since it is always guaranteed to make the vertex conforming
 */
bool edgeAddHullHeuristic(Edge *edge, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data);


/*
 * add limited hull to vertex (if non-conforming)
 * return false if not successful -> add all incident tetrahedra as workaround
 */
bool vertexAddHullHeuristic(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data);


/*
 * add limited hull to connected vertex
 * add incident tetrahedra in S_0 containing a S_0/non-S_0 triangle, sorted by criterion of added triangles, until conforming
 * returns true to indicate that vertex is now conforming (is guaranteed)
 */
bool vertexAddHullHeuristicConnected(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data);

/*
 * add limited hull to connected vertex
 * add incident tetrahedra in S_0 containing a S_0/non-S_0 triangle, sorted by criterion of added triangles, until conforming
 * returns true to indicate that vertex is now conforming (is guaranteed)
 */
bool vertexAddHullHeuristicNonPlanar(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, c3dModelData &data);


/*
 * add limited hull to non-conforming vertex (type=true: vertex-connected, false: non-planar)
 * add incident tetrahedra in S_0 containing a S_0/non-S_0 triangle and open entities, sorted by criterion of added triangles, until conforming
 * returns true to indicate that vertex is now conforming (is guaranteed)
 */
bool vertexAddHullHeuristicGeneric(DVertex vh, std::list<Cell> &addedCells, int &tetraLabel, NCType ncType, c3dModelData &data);


/*
 * returns whether tetrahedron can be added at vertex
 * tetrahedron is finite, in S_0 and contains a non-S_0 triangle incident to the given vertex
 */
bool isTetrahedronAddable(Cell ch, DVertex vh, c3dModelData &data);


/*
 * determine sum of criterion for non-existing triangles in that tetrahedron
 */
float getCriterionForNonExistingTrianglesInTetrahedron(Cell ch, c3dModelData &data);


/*
 * determine shared edge between the two triangles
 * PRE-CONDITION: triangles must share an edge
 */
Edge *sharedEdge(Triangle *t0, Triangle *t1);
