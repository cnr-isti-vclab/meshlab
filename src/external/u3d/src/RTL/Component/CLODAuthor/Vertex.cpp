//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#include "Vertex.h"
#include "Pair.h"


Vertex::Vertex() 
{
	clearBase();
	clearBoundary();
	clearTextureBoundary();
}

// This is a boundary vertex if there is a pair attached that has
// only one face.
void Vertex::initBoundary()
{
	U32 SetCtx = 0;
	Pair* pPair = (Pair*)m_Pairs.Begin(SetCtx);
	while(pPair && pPair->NumFaces() !=1)
	{
		pPair = (Pair*)m_Pairs.Next(SetCtx);
	}
	if(pPair) 
		setBoundary();
	else
		clearBoundary();
}


// update to use set operators, union of pair face sets
void Vertex::computeFaceSet(SmallPtrSet& faces)
{
	if (m_Pairs.Size() == 0) 
		return;

	U32 SetCtx = 0;
	Pair* pPair = (Pair*)m_Pairs.Begin(SetCtx);
	while(pPair)
	{	
		SmallPtrSet_Union(pPair->GetFaces(), &faces);
		pPair = (Pair*)m_Pairs.Next(SetCtx);
	}
}


// Determine if the vertex contributes to any faces
BOOL Vertex::HasFaces()
{
	if (m_Pairs.Size() == 0) 
		return FALSE;

	U32 SetCtx = 0;
	Pair* pPair = (Pair*)m_Pairs.Begin(SetCtx);
	while(pPair)
	{	
		if(pPair->NumFaces() != 0)
		{
			return TRUE;
		}
		pPair = (Pair*)m_Pairs.Next(SetCtx);
	}
	return FALSE;
}


void Vertex::init()
{
	computeQuadric();
	setMultiplyVectors();
}

void Vertex::updatePairCosts(Vertex *removed)
{
	// This method assumes that the geometry has just been updated.
	// This method will now recompute the quadrics and cost for each pair.

	// 1)  First update the quadric for this vertex:
	// Garland and Heckbert suggest it's simply: q'(v1') = q(v1) +  q(v2);
	Vertex* r = removed;
	quadric += r->Quadric();

	// 2) Now compute a new quadric, contraction target, and cost for each pair:
	U32 SetCtx = 0;
	Pair* pPair = (Pair*)m_Pairs.Begin(SetCtx);
	Matrix4x4 Quadric;
	while(pPair)
	{
		pPair->computeQuadric(&Quadric);
		pPair->computeCost(&Quadric);
		pPair = (Pair*)m_Pairs.Next(SetCtx);
	}
}

void Vertex::setMultiplyVectors()
{
	IV3D t;
	t.x = v.X();
	t.y = v.Y();
	t.z = v.Z();
	v4x1.set(t);
	v1x4.set(t);
}


void Vertex::computeQuadric()
{
	// 1) Set the quadric to zero:
	quadric.reset();

	// 2) Create a temporary set of the faces attached to this vertex:
	SmallPtrSet faces;
	computeFaceSet (faces);
	if (faces.Size() == 0)
	{
		return;
	}

	// 3) Accumulate Quadrics from each face:
	U32 SetCtx = 0;
	Face* pFace = (Face*) faces.Begin(SetCtx);
	while(pFace)
	{
		// If the face has not yet had it's quadric computed, compute it now:
		if (!pFace->isCurrent())
		{
			pFace->computeQuadric();
		}
		quadric += pFace->Quadric();
		pFace = (Face*)faces.Next(SetCtx);
	}

	// 4) Accumulate boundary quadrics from each pair that is a boundary:
	// I store the boundary quadric temporarily in the pair, then delete 
	// it during pair::init().Every boundary quadric should be used twice, 
	// once for each vertex attached to the pair.

	Pair* pPair = (Pair*) m_Pairs.Begin(SetCtx);
	while(pPair)
	{
		// If pair is a boundary, accumulate its boundary quadric:
		if (pPair->boundary())
		{
			quadric += pPair->BoundaryQuadric();
		}
		pPair = (Pair*) m_Pairs.Next(SetCtx);
	}
	
	// We should now have an updated quadric for this vertex.
}
