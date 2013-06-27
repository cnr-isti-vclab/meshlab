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
 * see chapter 5.2 in thesis: globally inflate the remaining OCBs (deflated)
 */
void inflate(int &tetraLabel, std::list<Cell> &inflatedCells, c3dModelData &data);


/*
 * collects sets of open trees (vertices + edges)
 */
std::vector<DVertex> collectOpenVertices(c3dModelData &data);


/*
 * determine addable tetrahedra incident to that vertex
 */
void insertAddableTetrahedraForVertex(DVertex vh, btree::btree_set<Cell> &cellSet, c3dModelData &data);

/*
 * remove tetrahedron from queue
 */
void removeFromQueue(btree::btree_multimap<float, Cell> &tetraMMap, btree::btree_map<Cell, float> &tetraReverseMap, Cell ch);

/*
 * add all tetrahedra candidates to queue, sorted with their criterion
 */
void addTetrahedraToQueue(const btree::btree_set<Cell> &candidateSet, btree::btree_multimap<float, Cell> &tetraMMap, btree::btree_map<Cell, float> &tetraReverseMap, c3dModelData &data);
