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
#ifndef PAIR_DOT_H
#define PAIR_DOT_H
#include "Vertex.h"
#include "Primitives.h"
#include "Face.h"
#include "FacePtrSet.h"
#include "SmallPtrSet.h"
#include "QEConstants.h"
#include "IFXAuthorCLODMesh.h"

#define LEN_EPSILON FLT_EPSILON*5.0f // use it for defining low threshold for normals' length

class Pair
{
private:
	UPTR HeapContext;
	static float cosMaxNormalChange;
	BOOL continuousTexCoords(IFXAuthorCLODMesh*);
	BOOL texCoordsEqual(Face *face1, int slot1, Face* face2, int slot2, IFXAuthorCLODMesh*);
	inline BOOL isZero( IV3D *v1 ); ///< check if this vector is zero one

protected:
	Vertex *v1,*v2; ///< indicies of vertices that make up pair.
	float weight; ///< 0.0 means contract to v1, 1.0 means contract to v2. set by subclass
	float cost; ///< error cost of contracting this pair, this will be set by the subclass
	BOOL materialBoundary;

	FacePtrSet m_Faces; ///< set of faces using this pair.

public:
	Pair* m_pHashNext;

	Pair();
	~Pair();

	void Setup(Vertex *vert1, Vertex *vert2);
	void init(); ///< must call this

	inline Vertex* getv1()          { return v1; };
	inline Vertex* getv2()          { return v2; };

	inline FacePtrSet* GetFaces()  { return &m_Faces; };
	inline U32 NumFaces()     { return m_Faces.Size(); };

	void AddFaces(FacePtrSet* fs);

	inline void AddFace(Face *face) { m_Faces.Insert(face); };
	inline void RemoveFace(Face *f) { m_Faces.Remove(f); };

	inline float getCost()          { return cost;};
	inline void  setCost(float c) { cost = c; };

	Vertex* getContractTarget();  ///< returns remaining vert
	void    setContractTarget(int vert);  ///< designate the vertex to contract to (vertex to keep).

	void computeCost(Matrix4x4*); ///< all pair types must have this

	BOOL normalFlips(SmallPtrSet&, SmallPtrSet&, BOOL& smallNormalChange);


	inline BOOL boundary ()
	{ 
		if ((m_Faces.Size() == 1) || materialBoundary)
			return TRUE;
		return FALSE;
	};
	inline BOOL isEdge()
	{
		if (m_Faces.Size() > 0)
			return TRUE;
		return FALSE;
	};
	void ReplaceVertex (Vertex *removeVertex, Vertex *keepVertex);

	void markMaterialBoundaries(IFXAuthorCLODMesh*);

	// For heap maintainance:
	inline UPTR getHeapPosition() { return HeapContext; };
	inline void setHeapPosition (UPTR i) { HeapContext = i; };

	inline Vertex* getCommonVertex(Pair *p)  // assumes must be one in common
	{ 
		if(v1 == p->v1) return v1;
		if(v1 == p->v2) return v1;
		return v2;
	};

	void Merge(Pair *pair);

	inline Vertex* otherVertex(Vertex *v)
	{
		if(v==v1)
			return v2;
		return v1;
	};

	BOOL IsEqual(Vertex* in_v1, Vertex* in_v2)
	{
		IFXASSERT(in_v1<in_v2);
		return v1 == in_v1 && v2 == in_v2;
	};
	void setMaxNormalChange(float f) { cosMaxNormalChange = (float) cos(f*(3.14159/180.0)); };

private:
	/// @todo: - may have space and speed savings moving this out for the vertex init
	Matrix4x4 *m_pQuadric;
public:

	inline Matrix4x4& BoundaryQuadric()
	{ 
		if (!m_pQuadric)
			computeBoundaryQuadric();
		return (*m_pQuadric);
	};

	void computeQuadric(Matrix4x4*);
	void computeBoundaryQuadric();
	void deleteBoundaryQuadric();
};


#endif // PAIR_DOT_H
