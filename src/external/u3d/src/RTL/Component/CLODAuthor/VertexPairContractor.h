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
#ifndef VERTEXPAIRCONTRACTOR_DOT_H
#define VERTEXPAIRCONTRACTOR_DOT_H
#include "Primitives.h"
#include "Vertex.h" 
#include "Face.h" 
#include "Pair.h" 
#include "ContractionRecorder.h"
#include "PairHash.h"
#include "IFXFastHeap.h"
#include "CLODGenerator.h" 
#include "IFXProgressCallback.h"

typedef IFXFastHeap<float, Pair*> PairHeap;

class VertexPairContractor
{
public:
	int metric2Start;
	int vertsRemoved; //number of vertices removed so far.

	VertexPairContractor();
	virtual ~VertexPairContractor();
	
	virtual IFXRESULT IFXAPI  init( CLODGenerator::Params *p, BOOL& res ); ///< must call this after creating VPC subclass instance.
	inline U32 size() { return m_pPairHeap->Size(); };
	
	inline void kill() { killMe = TRUE; }; ///< should be called from progress call back if you want to terminate.
	inline BOOL getKill() { return killMe; };
	
	// Contraction operators:
	BOOL contractNextPair();
    BOOL contractAll( IFXProgressCallback *pPCB );

	Vertex* getVertices() {  return m_pVertices; };
	int getNumVerts() { return m_NumVerts; };

	// for use by the PairFinder
	void AddPair(Vertex* v1, Vertex* v2)
	{
		m_pPairHash->AddPair(v1, v2);
	};
	
protected:
	// The source mesh
	IFXAuthorCLODMesh*		mesh;

	// Pair info
	PairHeap*   m_pPairHeap; ///< pair container sorted by contraction cost.
	PairHash*	m_pPairHash; ///< pair hash bucket table

	// Vertex Info
	Vertex*	m_pVertices;
	U32		m_NumVerts;

	// Face Info
	Face*	m_pFaces;	///< ptr to array of face pointers.
	int		m_NumFaces;	///< some face get thrown away, this is how many real faces

private:
	CLODGenerator::Params *m_pParams;
	ContractionRecorder *recorder;

	BOOL  killMe;

	U32	  numBaseVerts;
	int   *baseVerts;
	U32		m_NumFacesIn;   ///< num face in orginal input mesh

	CLODGenerator::NormalsMode		normalsMode;	///< Indicates how normals should be maintained and updated.
	float			normalsCreaseAngle; ///< If PerFacePerVertex normals are used, normalCreaseAngle indicates
										///< a angle (deg) threshold for sharing a vertex normal b/t two faces.            

	float *meshDamage; ///< array of floats passed in by user, CLODgen will record mesh damage in.
	void findUnconnectedPairs(float withinDistance);

	// Computational variable to replace old static, so each instance has it's own state. 
	SmallPtrSet m_UpdateFaces;
	SmallPtrSet m_rvFaces;

	U32 m_rejectionCount;
	Vertex* m_keptVertex;  
	int m_removeCount;
};

#endif // VERTEXPAIRCONTRACTOR_DOT_H
