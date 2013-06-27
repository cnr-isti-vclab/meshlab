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
 * test OC boundaries for holes (try to create a spanning triangulation)
 */
std::list<OCBoundary *> testForHolesInOCBs(std::list<OCBoundary> &ocBoundaries, c3dModelData &data);


/*
 * detect if OCB is open
 */
bool detectIfOCBIsOpen(std::list<SBElem> &elemList, c3dModelData &data);


/*
 * returns next halftriangle about that halfedge (and changes halfedge to the one in the new tetrahedron)
 * in the direction which the halftriangle faces (inside the tetrahedron)
 */
void nextFacingHalftriangle(DEdge &he, DTriangle &ht, c3dModelData &data);


/*
 * classify the given OCBs (they are all open) as either with hole, or as deflated
 * return list with OCBs which have holes (not deflated)
 */
std::list<OCBoundary *> classifyOCBWithHull(std::list<OCBoundary *> &openOCBs, c3dModelData &data);


/*
 * determine vertex set for CC set adjacent to OCB (also interior vertices)
 */
void getCCVertexSetForOCB(OCBoundary *currOCB, std::set<DVertex> &vertexSet);


/*
 * test if OC has interior vertices
 */
bool testOCHasInteriorVertices(OCBoundary *currOCB, std::set<Edge *> &openOCBEdgeSet, c3dModelData &data);


/*
 * return next existing triangle for SBElem (in the orientation the halftriangle is facing)
 */
SBElem nextExistingTriangle(SBElem elem, c3dModelData &data);


/*
 * create hulls for the given OC boundaries (they all have holes)
 */
void createHullOCBoundary(std::list<OCBoundary *> &holeOCBs, c3dModelData &data);


/*
 * fill the holes of the OCBoundaries 
 */
void fillHoles(std::list<OCBoundary *> &holeBoundaries, std::map<int, Cell> &intSpaceMap, c3dModelData &data);


/*
 * create hull for vertex set
 */
std::set<Triangle *> createHull(std::set<DVertex> &vertexSet, std::set<Triangle *> &boundaryTriangleSet, c3dModelData &data);


/*
 * determines if triangle has an external edge (is an edge with 1 incident triangle)
 */
bool triangleHasExternalEdge(Triangle *triangle, c3dModelData &data);


/*
 * determines incident existing triangle count per edge
 */
int countEdgeTriangles(Edge *edge, c3dModelData &data);