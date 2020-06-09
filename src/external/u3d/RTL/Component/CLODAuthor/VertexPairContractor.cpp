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
#include <float.h>
#include "Pair.h"
#include "VertexPairContractor.h"
#include "ContractionRecorder.h"
#include "PairFinder.h"
#include "CostMap.h"
#include <memory.h>

#define VTXIDX(x) ((x) - m_pVertices)

/// @todo: tune this value
#define CLOD_FLT_EPSILON	(FLT_EPSILON * 100.0f)

VertexPairContractor::VertexPairContractor()
{	
	mesh			= NULL;	
	m_pVertices		= NULL;
	m_NumVerts		= 0;
	m_pFaces		= NULL;
	m_NumFaces		= 0;
	recorder		= NULL; 
	m_pPairHeap		= NULL;
	m_pPairHash		= NULL;
	vertsRemoved	= 0;
	killMe			= 0;
	m_rejectionCount = 0; 
	m_removeCount	= 0;
	m_keptVertex	= NULL;
	baseVerts = NULL;
	m_pParams		= NULL;
	meshDamage		= NULL;
}


VertexPairContractor::~VertexPairContractor()
{	
	delete m_pPairHeap;
	m_pPairHeap		= NULL;
	delete m_pPairHash;
	m_pPairHash		= NULL;
	delete[] m_pVertices;
	m_pVertices		= NULL;
	delete[] m_pFaces;
	m_pFaces		= NULL;
	delete recorder;
	recorder		= NULL; 
	if(baseVerts) 
	{
		delete [] baseVerts;
		baseVerts = NULL;
	}
}


IFXRESULT VertexPairContractor::init(CLODGenerator::Params *p, BOOL& res)
{
	IFXRESULT result = IFX_OK;
	mesh = p->pMesh;
	m_NumVerts		= mesh->GetMaxMeshDesc()->NumPositions;
	m_NumFacesIn    = mesh->GetMaxMeshDesc()->NumFaces;
	m_pVertices		= new Vertex[m_NumVerts];
	m_pPairHash		= new PairHash( m_NumVerts, m_pVertices );
	recorder		= new ContractionRecorder(p);

	U32 i;

	res = TRUE;

	if( !m_pVertices || !m_pPairHash || !recorder )
	{
		if( m_pVertices )
			delete [] m_pVertices;
		if( m_pPairHash )
			delete m_pPairHash;
		if( recorder )
			delete recorder;

		result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		m_pParams = p;
		numBaseVerts = p->numBaseVertices;
		if( numBaseVerts > 0 )
		{
			baseVerts = new int[numBaseVerts];
			if( NULL != baseVerts )
				memcpy(baseVerts, p->baseVertices, numBaseVerts*sizeof(int));
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		normalsMode = p->normalsMode;
		normalsCreaseAngle = p->normalsCreaseAngle;
		meshDamage = p->meshDamage;

		// Create all the vpc vertices:

		/// @todo tune this
		// preallocate a fixed amount of pairs a guess
		m_pPairHash->Initialize(m_NumFacesIn *2);

		IFXVector3 *pInMeshVerts;
		mesh->GetPositions(&pInMeshVerts);

		for(i = 0; i < m_NumVerts; i++) 
		{
			m_pVertices[i].v = pInMeshVerts[i];
		}

		// mark the base vertices
		for(i=0; i<numBaseVerts; i++)
		{
			m_pVertices[baseVerts[i]].setBase();
		}

		// for each face in the author mesh create the vpc pairs and the vpc face
		m_NumFaces = 0;
		m_pFaces = new Face[m_NumFacesIn];

		if( !m_pFaces )
			result = IFX_E_OUT_OF_MEMORY;
	}

	U32 a,b,c;  //vert indicies
	Pair *pa, *pb, *pc;

	if( IFXSUCCESS( result ) )
	{
		float area;
		IFXAuthorFace *pFace;
		mesh->GetPositionFaces(&pFace);

		for(i=0; i < m_NumFacesIn; i++) 
		{	
			a = pFace[i].VertexA();
			b = pFace[i].VertexB();
			c = pFace[i].VertexC();

			if((area = triangleAreaSlow(
				(IV3D*) &m_pVertices[a].v, (IV3D*)&m_pVertices[b].v, (IV3D*)&m_pVertices[c].v)) > 
				(CLOD_FLT_EPSILON*CLOD_FLT_EPSILON)) 
			{
				Face *f;
				// pairs is the pair hash table, which is used to eliminate duplicates
				pa = m_pPairHash->AddPair(m_pVertices+a, m_pVertices+b);
				pb = m_pPairHash->AddPair(m_pVertices+b, m_pVertices+c);
				pc = m_pPairHash->AddPair(m_pVertices+c, m_pVertices+a);

				// when face is created it sets it's own index to faceCount.
				f = m_pFaces + m_NumFaces;
				f->Set(pa, pb, pc, i);
				m_NumFaces++;
			}
		}

		if(m_pParams->mergeThresh >= 0.0f)
		{
			PairFinder *pf = new PairFinder(this, m_pParams);

			if( !pf )
			{
				result = IFX_E_OUT_OF_MEMORY;
			}
			else
			{
				pf->findPairs();
				delete pf;
			}
		}
	}

	Pair* pPair;

	if( IFXSUCCESS( result ) )
	{
		if(killMe)	// see if a kill request came in during pair finding phase.
			res = FALSE;
	}

	if( IFXSUCCESS( result ) && res )
	{
		// Initialize vertices for pair cost computation.
		// Set boundary flag if vertex is attached to a boundary edge
		for (i = 0; i < m_NumVerts; i++) 
		{
			m_pVertices[i].initBoundary();
			m_pVertices[i].init();
		}

		// Mark material boundaries:
		pPair = m_pPairHash->Begin();

		// init the static member
		pPair->setMaxNormalChange(p->maxNormalChange);

		while( pPair ) 
		{
			pPair->markMaterialBoundaries(mesh);
			pPair = m_pPairHash->Next();
		}

		m_pPairHeap	= new PairHeap(m_pPairHash->Size());
		if( !m_pPairHeap )
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) && res )
	{
		pPair = m_pPairHash->Begin();
		while(pPair) 
		{
			pPair->init();
			pPair->setHeapPosition(m_pPairHeap->Insert(pPair->getCost(), pPair));
			pPair = m_pPairHash->Next();
		}

		// Finish initializing the recorder object:
		recorder->init (m_pParams, m_pVertices, m_pFaces);
	}

	return result;	
}


BOOL VertexPairContractor::contractNextPair()
{
	if (!m_pPairHeap->Size()) 
	{
		FacePtrSet deleted;
		SmallPtrSet adjusted;
		if(m_keptVertex == NULL) 
			return FALSE; // we have been called again even after returning FALSE.

		// No more pairs, but we need to record the removal of the last vertex.
		recorder->record(m_keptVertex, m_keptVertex, &deleted, &adjusted);
		m_keptVertex = NULL;
		return FALSE;
	}

	U32 SetCtx = 0;

	Pair *pair = m_pPairHeap->Pop(); // get next pair to contract
	pair->setHeapPosition(0);

	// We won't contract pairs that flip normals:
	BOOL smallNormalChange;
	if (pair->normalFlips(m_UpdateFaces, m_rvFaces, smallNormalChange) 
		&& (m_rejectionCount < m_pPairHeap->Size()))
	{
		pair->setHeapPosition(m_pPairHeap->Insert(pair->getCost(), pair));
		m_rejectionCount++;
		return TRUE;
	}

	// Reset rejection flag:
	m_rejectionCount = 0;

	m_keptVertex = pair->getContractTarget(); // pair->contract(recorder);
	Vertex* removeVertex = pair->otherVertex(m_keptVertex);

	//Record the contraction:
	recorder->record(m_keptVertex, removeVertex, pair->GetFaces(), &m_UpdateFaces);

	// 2) Remove this pair from its vertices and faces:
	m_keptVertex->RemovePair(pair);
	removeVertex->RemovePair(pair);
	m_pPairHash->Remove(pair);

	// 3) Remove the faces from their pairs:
	FacePtrSet* pFaceSet = pair->GetFaces();
	Face* pFace = (Face*)pFaceSet->Begin(SetCtx);
	while(pFace)
	{
		pFace->remove(); // remove face from all pairs
		pFace = (Face*)pFaceSet->Begin(SetCtx); // weird iteration because previous call has latered the set
	}
	m_pPairHash->DeletePair(pair);

	// Merge Pairs And delete pairs with no reason for being
	// 4) walk remove Vertex pairs
	SmallPtrSet* pRemovePairSet = removeVertex->GetPairSet();
	Pair* rp = (Pair*)pRemovePairSet->Begin(SetCtx);
	Vertex* ov = NULL;
	// All of pairs in this vertex now belong to the keep Vertex, replace the vertex in the pair:
	while(rp)
	{
		ov = rp->otherVertex(removeVertex);
		Pair* existPair = m_pPairHash->Find(m_keptVertex, ov);
		if(existPair)
		{
			existPair->Merge(rp);
			ov->RemovePair(rp);
			m_pPairHeap->Remove(rp->getHeapPosition());
			m_pPairHash->Delete(rp);
		}
		else
		{
			m_pPairHash->Remove(rp);
			rp->ReplaceVertex(removeVertex, m_keptVertex);
			m_pPairHash->Insert(rp);

		}
		rp = (Pair*)pRemovePairSet->Next(SetCtx);
	}
	pRemovePairSet->Clear();

	// 5 Topology is now complete 
	recorder->recordAttribChanges(m_keptVertex, &m_UpdateFaces, smallNormalChange);

	// 6 test the keep vertex for easy removal
	vertsRemoved++;	 // keep track of this for error metric clients.
	if(m_keptVertex->getNumPairs() == 0)
	{	
		// vertex is isolated remove it.
		FacePtrSet deleted;
		SmallPtrSet adjusted;

		// This vertex is no longer connected to any thing, 
		// must record the removal of it.
		recorder->record(m_keptVertex, m_keptVertex, &deleted, &adjusted);
		m_keptVertex = NULL;
	}
	else
	{
		// Update costs on all pairs attached to the keepVertex:
		// Integrate with loop below
		m_keptVertex->updatePairCosts (removeVertex);


		SmallPtrSet* kvPairs = m_keptVertex->GetPairSet();
		rp = (Pair*) kvPairs->Begin(SetCtx);
		while(rp)
		{
			// If a pair has no faces then it's a candidate for immediate removal.
			if( rp->NumFaces() == 0 )
			{
				// Figure out if the pair should really be removed.
				// We dont want to remove the virtual pairs that are created by vertex merging,  
				// and we don't want to remove any base vertices.
				// If vertex is not a base vertex and it has no faces then it should be removed.

				// Note! UMF may have problems with base vertices!
				// We might want to ignore base vertice in this situation (no faces attached).
				if( !rp->getv1()->getBase() && ! rp->getv1()->HasFaces())
				{
					rp->setContractTarget(2);  // Set direction of collapse to v2 (thus removing v1).
					rp->setCost(0.0);  // This should make the collapse happen right away.
				}
				else
				{
					if( !rp->getv2()->getBase() &&  ! rp->getv2()->HasFaces() )
					{
						rp->setContractTarget(1);
						rp->setCost(0.0);
					}
				}
			}
			m_pPairHeap->Reinsert(rp->getHeapPosition(), rp->getCost());
			rp = (Pair*) kvPairs->Next(SetCtx);
		}

	}

	// call the  callback function that is used to monitor progress.
	if (m_pParams->progressCallback && !(m_removeCount % m_pParams->progressFrequency)) 
	{
		(*m_pParams->progressCallback)(m_pPairHeap->Size(), m_pParams->pProgressCallBackUserData);
		m_removeCount = 0;
		// see if they decided to kill us.
		if(killMe == TRUE) 
			return FALSE;
	}
	m_removeCount++;

	return TRUE;
}


BOOL VertexPairContractor::contractAll( IFXProgressCallback *pPCB )
{	
	// Contract pairs until the pair m_pPairHeap is empty.
    U32 uSize = m_pPairHeap->Size();
    F32 fStep = (F32)( uSize / 100.0f );
    F32 fSum = 0.0f;
    U32 uCount = 0;
    if( pPCB )
    {
        pPCB->InitializeProgress( 100.0f );
    }

	while(contractNextPair()) 
	{
        uCount++;
        if( pPCB && uCount > (U32)fSum )
        {
            fSum += fStep;
            if( fSum/fStep < 100.0f )
                pPCB->UpdateProgress( fSum/fStep );
            fStep = (F32)( m_pPairHeap->Size() / 100.0f );
        }
    };

	if(killMe)
		return FALSE;

	recorder->reOrderIndices();

	return TRUE;
}


