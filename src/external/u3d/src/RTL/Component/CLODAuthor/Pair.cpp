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
#include <stdio.h>
#include <float.h>

#include "Pair.h"
#include "Face.h"
#include "Vertex.h"
#include "Primitives.h"
#include "CostMap.h"

float Pair::cosMaxNormalChange;

Pair::Pair()
{
	v1 = NULL;
	v2 = NULL;

	cost = 0.0;
	weight = 0.0;
	materialBoundary = FALSE;
	m_pHashNext = NULL;

	// We'll create boundary quadrics only as needed:
	m_pQuadric = NULL;
}


Pair::~Pair()
{
	if (m_pQuadric)
	{
		delete m_pQuadric;
	}
}

void Pair::Setup(Vertex *vert1, Vertex *vert2)
{
	IFXASSERT(vert1 != vert2);
	if(vert1 < vert2)
	{
		v1 = vert1;
		v2 = vert2;
	}
	else
	{
		v2 = vert1;
		v1 = vert2;
	}
	v1->AddPair (this);
	v2->AddPair (this);
}


// this adds the faces in fs to the faces on the pair.
void Pair::AddFaces(FacePtrSet*fs)
{
	SmallPtrSet_Union(fs, &m_Faces);
}

BOOL Pair::isZero( IV3D *v1 )
{
	if( fabs(v1->x) < LEN_EPSILON && fabs(v1->y) < LEN_EPSILON && fabs(v1->z) < LEN_EPSILON ) 
		return TRUE; 
	else 
		return FALSE; 
}

// Check Compare before and after normals of affected faces.
// If a normal changes more by more than pi/2 (90 deg), then
// we will disallow this contraction and terminate early.
BOOL Pair::normalFlips(SmallPtrSet& updatedFaces, SmallPtrSet& rvFaces, BOOL& smallNormalChange)
{
	Vertex* keepVertex = getContractTarget();
	Vertex* removeVertex = NULL;
	if (keepVertex == v1)
		removeVertex = v2;
	else
		removeVertex = v1;

	// Compute adjusted faces:
	rvFaces.Clear();
	updatedFaces.Clear();

	removeVertex->computeFaceSet(rvFaces); // problem here, rvFaces different
	SmallPtrSet_Difference(&rvFaces, &m_Faces, &updatedFaces);

	Vertex *v1, *v2, *v3;
	U32 SetCtx = 0;
	Face* face = (Face*)updatedFaces.Begin(SetCtx);

	smallNormalChange = TRUE;
	F32 dotThresh = 0.966f;  // about 15 degrees
	F32 worstDot=1;
	BOOL result = FALSE;
	IV3D u, v, oldNormal, newNormal;
	F32 dot;

	while(face && !result)
	{
		if( !result )
		{
			v1 = face->a->getCommonVertex(face->b);
			v2 = face->b->getCommonVertex(face->c);
			v3 = face->c->getCommonVertex(face->a);

			// Compute the normal - we could grab this data from the equation of the plane (A, B, C, D),
			// but then we'd have to keep A, B, C, D around in the faces:
			subtract3D ((IV3D*)&v2->v, (IV3D*)&v1->v, (IV3D*)&u);
			if( isZero(&u) )
			{
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		if( !result )
		{
			normalize3D ((IV3D*)&u);
			subtract3D ((IV3D*)&v3->v, (IV3D*)&v1->v, (IV3D*)&v);
			if( isZero(&v) )
			{
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		if( !result )
		{
			normalize3D ((IV3D*)&v);
			crossprod ((IV3D*)&u, (IV3D*)&v, &oldNormal);
			if( isZero(&oldNormal) )
			{
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		if( !result )
			normalize3D (&oldNormal);

		// Recompute the normal:

		if( !result )
		{
			// Install the keep vertex into this temp version of the adjusted face:
			if (v1 == removeVertex) v1 = keepVertex;
			if (v2 == removeVertex) v2 = keepVertex;
			if (v3 == removeVertex) v3 = keepVertex;

			subtract3D ((IV3D*)&v2->v, (IV3D*)&v1->v, (IV3D*)&u);
			if( isZero(&u) )
			{
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		if( !result )
		{
			normalize3D ((IV3D*)&u);
			subtract3D ((IV3D*)&v3->v, (IV3D*)&v1->v, (IV3D*)&v);
			if( isZero(&v) )
			{
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		if( !result )
		{
			normalize3D ((IV3D*)&v);
			crossprod ((IV3D*)&u, (IV3D*)&v, (IV3D*)&newNormal);
			if( isZero(&newNormal) )
			{
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		if( !result )
		{
			normalize3D ((IV3D*)&newNormal);
			dot = dotProduct3D ((IV3D*)&oldNormal, (IV3D*)&newNormal);
			// Did the normal flip?:
			if (dot < cosMaxNormalChange)
			{
				// Mark it flipped:
				if(cost < NORMAL_FLIP_COST)
					cost = NORMAL_FLIP_COST;
				result = TRUE;
			}
		}

		// test
		if( !result )
		{
			if(dot < worstDot)
				worstDot = dot;
			face = (Face*)updatedFaces.Next(SetCtx);
		}
	}

	if(worstDot < dotThresh && !result)
	{
		smallNormalChange = FALSE;
	}
	return result;
}




void Pair::markMaterialBoundaries(IFXAuthorCLODMesh* imesh)
{
	materialBoundary = FALSE;

	if (m_Faces.Size() < 2)
		return;

	// For the moment, we just test the two face case:
	U32 SetCtx = 0;
	Face* pFace = (Face*)m_Faces.Begin(SetCtx);
	int index1 = pFace->index;

	U32 matID1 = imesh->GetFaceMaterial(index1,&matID1);
	pFace = (Face*)m_Faces.Next(SetCtx);
	int index2 = pFace->index;
	U32 matID2 = imesh->GetFaceMaterial(index2,&matID2);

	// If matID are equivalent and texture are continuous then no boundary.
	if ((matID2 == matID1) && imesh->GetMaxMeshDesc()->NumTexCoords && continuousTexCoords(imesh))
		return;

	// We have a material or texture coord boundary, mark it:
	if(matID2 != matID1)
	{
		materialBoundary = TRUE;
		v1->setBoundary();
		v2->setBoundary();
		return;
	}

	v1->setTextureBoundary();
	v2->setTextureBoundary();

}

// Returns true if the texture coordinates are continuous
// across this edge pair.
BOOL Pair::continuousTexCoords(IFXAuthorCLODMesh* imesh)
{
	// map from slot index to next slot index that is clockwise from current.
	const unsigned char clockwise[3] = {1, 2, 0};

	if (m_Faces.Size() < 2)
		return TRUE;

	if (m_Faces.Size() == 2)
	{
		int f1Side, f2Side; // side a = 0, b = 1, c = 2
		// get the two faces that share this edge.
		U32 SetCtx = 0;
		Face *f1 = (Face*)m_Faces.Begin(SetCtx);
		Face *f2 = (Face*)m_Faces.Next(SetCtx);

		// determine which side of face 1 points to this edge (pair).
		if(f1->a == this)
			f1Side = 0;
		else
			if(f1->b == this)
				f1Side = 1;
			else
				f1Side = 2;
		// which side of f2
		if(f2->a == this)
			f2Side = 0;
		else
			if(f2->b == this)
				f2Side = 1;
			else
				f2Side = 2;


		if(texCoordsEqual(f1, f1Side, f2, clockwise[f2Side], imesh) &&
			texCoordsEqual(f1, clockwise[f1Side], f2, f2Side, imesh))
			return TRUE;
		else
			return FALSE;
	}

	/// @todo: add code to handle non manifold cases.
	return TRUE;
}

BOOL Pair::texCoordsEqual(Face *f1, int slot1, Face* f2, int slot2, IFXAuthorCLODMesh* imesh)
{
	U32 t1,t2,mat,layer,d,numLayers,numDimensions;

	IFXAuthorFace *pTexFaces;
	IFXAuthorMaterial *pAuthorMeshMats;
	IFXVector4 *tx1, *tx2, *pTexCoords;

	//handle multiple texture layers
	imesh->GetFaceMaterial(f1->index, &mat);  // at this point we know that both faces have the same material.
	imesh->GetMaterials(&pAuthorMeshMats);

	imesh->GetTexCoords(&pTexCoords);
	numLayers = pAuthorMeshMats[mat].m_uNumTextureLayers;
	for(layer = 0; layer < numLayers; layer++)
	{
		imesh->GetTexFaces(layer, &pTexFaces);
		t1 = pTexFaces[f1->index].corner[slot1];
		t2 = pTexFaces[f2->index].corner[slot2];

		tx1 = &pTexCoords[t1];
		tx2 = &pTexCoords[t2];

		//handle any number of dimensions
		numDimensions = pAuthorMeshMats[mat].m_uTexCoordDimensions[layer];
		for(d = 0; d < numDimensions; d++)
			if (tx1->Value(d) != tx2->Value(d))
				return FALSE;
	}
	return TRUE;
}

Vertex* Pair::getContractTarget()
{
	if(weight == 0.0)
		return v1;
	else
		return v2;
}

// vert should be 1 or 2 to indicate which vertex we contract to (the vertex that is kept).
void Pair::setContractTarget(int vert)
{
	if(vert == 1)
		weight = 0.0;
	else
		weight = 1.0;
}

void Pair::ReplaceVertex (Vertex *removeVertex, Vertex *keepVertex)
{
	if (v1 == removeVertex)
		v1 = keepVertex;
	if (v2 == removeVertex)
		v2 = keepVertex;

	IFXASSERT(v1 != v2);
	if(v1>v2)
	{
		removeVertex = v1;
		v1 = v2;
		v2 = removeVertex;
	}
	keepVertex->AddPair(this);
}

void Pair::Merge (Pair *pair)
{
	U32 SetCtx = 0;
	FacePtrSet *fset = pair->GetFaces();
	Face *pFace = (Face*)fset->Begin(SetCtx);
	while(pFace)
	{
		pFace->ReplacePair (pair, this);
		pFace = (Face*)fset->Next(SetCtx);
	}

	AddFaces (fset);
}

void Pair::deleteBoundaryQuadric()
{
	delete m_pQuadric;
	m_pQuadric = NULL;
}


void Pair::computeQuadric (Matrix4x4* pQuadric)
{
	pQuadric->reset();
	(*pQuadric) += v1->Quadric();
	(*pQuadric) += v2->Quadric();
}

void Pair::computeBoundaryQuadric()
{
	// We'd need to compute a quadric which is perpendicular to
	// the one face attached to this pair.
	float A, B, C, D;
	U32 SetCtx = 0;
	Face *face = (Face *) m_Faces.Begin(SetCtx);

	face->planeEquation (A, B, C, D);
	IV3D normal, perpNormal, pairVector;

	normal.x = A; normal.y = B; normal.z = C;
	normalize3D (&normal);

	subtract3D ((IV3D*)&v1->v, (IV3D*)&v2->v, (IV3D*)&pairVector);

	normalize3D ((IV3D*)&pairVector);
	crossprod ((IV3D*)&normal, (IV3D*)&pairVector, (IV3D*)&perpNormal);

	// Form an equation of a plane perpendicular to the pair and its 1 face:
	A = perpNormal.x;
	B = perpNormal.y;
	C = perpNormal.z;
	// Solve for D:
	//converting all float type data to double to improve calculation accuracy.
	D = (float)(-((double)A*(double)v1->v.X()) - ((double)B*(double)v1->v.Y()) - ((double)C*(double)v1->v.Z()));

	float q[16]; // q is row major:
	q[0]=A*A;   q[1]=A*B;   q[2]=A*C;   q[3]=A*D;
	q[4]=A*B;   q[5]=B*B;   q[6]=B*C;   q[7]=B*D;
	q[8]=A*C;   q[9]=B*C;   q[10]=C*C;    q[11]=C*D;
	q[12]=A*D;    q[13]=B*D;    q[14]=C*D;    q[15]=D*D;

	m_pQuadric = new Matrix4x4(q);

	// Weight this quadric..."We *really* want to hang on to the boundaries...":
	(*m_pQuadric) *= BOUNDARY_QUADRIC_WEIGHT;
}


void Pair::computeCost(Matrix4x4* pQuadric)
{
	// computeCost assumes the Quadric for this pair is up to date.

	// Now we compute the costs of contracting to v1 versus contracting to v2:
	// cost = vtranspose * Q * v
	float v1cost = (float) fabs(v1->vec1x4() * (*pQuadric) * v1->vec4x1());
	float v2cost = (float) fabs(v2->vec1x4() * (*pQuadric) * v2->vec4x1());

	// regular quadric only mode

	// if v1 is in the base mesh then don't want to contract it into v2
	// so set the v2 cost very high
	if(v1->getBase())
		v2cost = BASE_VERTEX_COST;
	if(v2->getBase())
		v1cost = BASE_VERTEX_COST;

	// Test for boundary conditions:

	// if v1 is a boundary vertex and v2 is not boundary and it's not a base then keep v1.

	if((v1->getBoundary() && !v2->getBoundary() && !v2->getBase()) ||
		(v1->getTextureBoundary() && !v2->getTextureBoundary() && !v2->getBase()))
	{
		cost = v1cost;
		weight = 0.0f;
	}
	else if ((v2->getBoundary() && !v1->getBoundary() && !v1->getBase()) ||
		(v2->getTextureBoundary() && !v1->getTextureBoundary() && !v1->getBase()))
	{
		cost = v2cost;
		weight = 1.0f;
	}
	else
	{
		// collapse as normal, according to lowest cost.
		// Contraction target will be the contraction with least cost:
		if (v1cost < v2cost)
		{
			cost = v1cost;
			weight = 0.0f;
		}
		else
		{
			cost = v2cost;
			weight = 1.0f;
		}
	}
}

void Pair::init()
{
	// Boundary quadrics only needed for Vertex::init()...so clean up some space:
	if (m_pQuadric)
	{
		delete m_pQuadric;
	}

	// We'll use this instance of the matrix for pair cost computations:
	m_pQuadric = NULL;

	Matrix4x4 Quadric;
	computeQuadric(&Quadric);
	computeCost(&Quadric);
}
