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

/* algo headers*/
#include "c3dConstructBC.h"
#include "c3dConstructDG.h"
#include "c3dHoleTreatment.h"
#include "c3dInflate.h"
#include "c3dMakeConformBC.h"
#include "c3dMeshFairing.h"
#include "c3dSculpture.h"
#include "c3dSegmentBC.h"
#include "c3dShared.h"

#include "connect3d.h"


Connect3D::Connect3D()
{
}

Connect3D::~Connect3D()
{
}

std::string Connect3D::info()
{
	return std::string("Connect3D - Reconstruction of water-tight triangulated meshes from unstructured point clouds\n(C) 2013 Stefan Ohrhallinger, Daniel Prieler, Version 1.0 (2013-05-28)");
}

/*
 * reconstruct triangulation from point set
 */
std::vector<Triangle *> Connect3D::reconstruct(c3dModelData &modelData, ProgressCallback *p)
{
	
	// steps 1-8 are labeled as in fig. 5.3 of thesis
	// STEP 1: create 3d delaunay triangulation
	p(4, "Construct Delaunay Triangulation");
	construct3DDelaunayTriangulation(modelData);
	p(15, "Creating Aggregate Datastructures");
	createAggregateDataStructure(modelData);

	// STEP 2: create boundary complex
	p(20, "Creating Boundary Complex");
	createBoundaryComplex(modelData);

	int labelTetra = 0;
	p(25, "Label Contiguous Spaces and classifying all entities");
	labelContiguousSpaces(labelTetra, modelData);
	classifyEntitiesAll(modelData);

	// STEP 3: make entities conforming
	p(40, "Fixing non-conforming entities");
	fixNonConformingEntities(labelTetra, modelData);	// see chapter 5.5.2 & 5.7 in thesis

	// STEP 4: identify boundaries containing potential hull holes
	btree::btree_map<int, SeparatorBoundary> sbMap;
	std::list<OCBoundary> ocBoundaries;
	std::list<CC> ccList;
	std::list<SBGroup> sbGroupList;

	// see chapter 5.5.4 in thesis
	p(50, "Locate closed and open components");
	segmentClosedComponents(sbMap, modelData); //last occurrence of LIST
	determineSeparatorBoundaries(sbMap, sbGroupList, modelData);
	locateOCBs(sbMap, ocBoundaries, ccList, modelData);

	determineCCVertexSet(ccList, modelData);
	
	// STEP 5: detect and cover hull holes
	std::list<OCBoundary *> openOCBs = testForHolesInOCBs(ocBoundaries, modelData);	// see chapter 5.6.1 in thesis: test if there is actually a hole or water-tight inside boundary
	std::list<OCBoundary *> holeOCBs = classifyOCBWithHull(openOCBs, modelData);	// see chapter 5.6.2 in thesis: classify OCBs as open or not (=need to be filled)

	p(60, "Fill holes");

	std::map<int, Cell> spaceMap;

	if (holeOCBs.size() > 0)
	{
		//fillHoles(holeOCBs, spaceMap, modelData); 
		createHullOCBoundary(holeOCBs, modelData);	// see chapter 5.6.3 in thesis: fill hull hole: create hull for OCBs with holes to fill them (old algorithm, only works for holes containing disks)
		
		// fix the non-conforming entities created by the hulls - necessary?
		resetAllEntities(modelData);
		labelTetra = 0;
		labelContiguousSpaces(labelTetra, modelData);

		classifyEntitiesAll(modelData);
	}

	sbMap.clear();
	ocBoundaries.clear();
	ccList.clear();
	sbGroupList.clear();


	std::list<Cell> inflatedCells;
	// STEP 6: inflate
	p(70, "Inflating");
	inflate(labelTetra, inflatedCells, modelData);	// see chapter 5.2 in thesis: globally inflate the remaining OCBs (deflated)

	// STEP 7: sculpture
	labelTetra = 0;
	resetAllTetraLabels(modelData);
	labelContiguousSpaces(labelTetra, modelData);
	classifyEntitiesForCells(inflatedCells, modelData);

	inflatedCells.clear();

	
	std::list<Cell> manipulatedCells;
	p(80, "Sculpturing");
	sculptureWallsHeuristic(spaceMap, manipulatedCells, modelData);	// see chapter 5.3.1-5.3.3 in thesis


	labelTetra = 0;
	resetAllTetraLabels(modelData);
	labelContiguousSpaces(labelTetra, modelData);
	classifyEntitiesForCells(manipulatedCells, modelData);

	manipulatedCells.clear();

	p(90, "pop NBCs");
	popNBCs(spaceMap, manipulatedCells, modelData);	// see chapter 5.3.4 in thesis: pop bubbles

	labelTetra = 0;
	resetAllTetraLabels(modelData);
	labelContiguousSpaces(labelTetra, modelData);

	classifyEntitiesForCells(manipulatedCells, modelData);
	manipulatedCells.clear();
	isBoundaryManifold(modelData);	// test to show in debug output if/how many non-manifold vertices (interior)

	// STEP 8: smooth boundary, see chapter 5.4 in thesis
	p(95, "smooth boundary");
	smoothBoundary(modelData);


	//labelTetra = 0;
	//resetAllTetraLabels(modelData);
	//labelContiguousSpaces(labelTetra, modelData);


	size_t numTris = 0;
	for(auto t: modelData.dtTriangles)
		if(t.exists() == true)
			++numTris;
	
	std::vector<Triangle *> surface;
	surface.reserve(numTris);
	for(size_t triIndex = 0; triIndex < modelData.dtTriangles.size(); ++triIndex)
	{
		if(modelData.dtTriangles[triIndex].exists() == true)
		{
			surface.push_back(&modelData.dtTriangles[triIndex]);
		}
	}	

	return surface;
}

