//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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

/** 
	@file CIFXBoneWeightsModifier.cpp
*/

#include "IFXCoreCIDs.h"
#include "CIFXBoneWeightsModifier.h"
#include "IFXBonesManager.h"
#include "IFXModifierDataPacket.h"
#include "IFXVertexMap.h"
#include "IFXDids.h"
#include "IFXAuthorCLODResource.h"
#include "IFXAuthorLineSetResource.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXAutoRelease.h"
#include "IFXModifierChain.h"
#include "IFXSceneGraph.h"
#include "IFXModel.h"

#include "IFXExportingCIDs.h"

static IFXDID* s_pOutputDids[1] =
{
	(IFXGUID*)&DID_IFXBoneWeights
};

static IFXDID* s_pInputDids[1] =
{
	(IFXGUID*)&DID_IFXBonesManager
};

IFXRESULT IFXAPI_CALLTYPE CIFXBoneWeightsModifier_Factory(IFXREFIID interfaceId,
														  void** ppInterface )
{
	IFXRESULT rc = IFX_OK;
	if (ppInterface)
	{
		CIFXBoneWeightsModifier *pPtr = new CIFXBoneWeightsModifier;
		if (pPtr)
		{
			rc = pPtr->Construct();
			if (IFXFAILURE(rc))
				IFXDELETE(pPtr);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if (IFXSUCCESS(rc))
		{
			pPtr->AddRef();
			rc = pPtr->QueryInterface(interfaceId, ppInterface);
			pPtr->Release();
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}
	IFXRETURN(rc);
}

CIFXBoneWeightsModifier::CIFXBoneWeightsModifier()
{
	m_uRefCount = 0;
}

CIFXBoneWeightsModifier::~CIFXBoneWeightsModifier()
{
	if( m_pAuthorBoneWeights )
		delete [] m_pAuthorBoneWeights;
}

IFXRESULT CIFXBoneWeightsModifier::Construct()
{
	IFXRESULT rc = IFX_OK;

	m_BoneWeightsIndex = (U32)-1;
	m_BonesManagerIndex = (U32)-1;
	m_pBonesMgr = NULL;
	m_pBoneWeights = NULL;
	m_LastResolution = 0;
	m_pAuthorBoneWeights = NULL;
	m_uABWSize = 0;

	IFXRETURN(rc);
}


// IFXUnknown
U32 CIFXBoneWeightsModifier::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXBoneWeightsModifier::Release()
{
	if( 1 == m_uRefCount)
	{
		CIFXModifier::PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXBoneWeightsModifier::QueryInterface(IFXREFIID riid, void** ppv)
{
	IFXRESULT rc = IFX_OK;

	if(ppv)
	{
		*ppv = 0;

		if(riid == IID_IFXUnknown)
			*ppv = (IFXUnknown*)this;
		else if(riid == IID_IFXBoneWeightsModifier)
			*ppv = (IFXBoneWeightsModifier*)this;
		else if(riid == IID_IFXSubject)
			*ppv = (IFXSubject*)this;
		else if(riid == IID_IFXModifier)
			*ppv = (IFXModifier*)this;
		else if(riid == IID_IFXMarker)
			*ppv = (IFXMarker*)this;
		else if(riid == IID_IFXMarkerX)
			*ppv = (IFXMarkerX*)this;
		else if (riid == IID_IFXMetaDataX)
			*ppv = (IFXMetaDataX*) this;

		if(*ppv)
			AddRef();
		else
			rc = IFX_E_UNSUPPORTED;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}

// IFXMarkerX
void CIFXBoneWeightsModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXBoneWeightsModifierEncoder, rpEncoderX);
}

// IFXModifier
IFXRESULT CIFXBoneWeightsModifier::GetOutputs( IFXDID**& rpOutOutputs,
											  U32&       rOutNumberOfOutputs,
											  U32*&    rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = 1;
	rpOutOutputDepAttrs = 0;
	rpOutOutputs = s_pOutputDids;

	return IFX_OK;
}

IFXRESULT CIFXBoneWeightsModifier::GetDependencies(IFXGUID*   pInOutputDID,
	IFXGUID**& rppOutInputDependencies,
	U32&       rOutNumberInputDependencies,
	IFXGUID**& rppOutOutputDependencies,
	U32&       rOutNumberOfOutputDependencies,
	U32*&   rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;


	if ( pInOutputDID == &DID_IFXBoneWeights )
	{
		rOutNumberInputDependencies = 1;
		rppOutInputDependencies = (IFXDID**)s_pInputDids;
		rOutNumberOfOutputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXBoneWeightModifier::GetDependencies()"\
			" called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}

IFXRESULT CIFXBoneWeightsModifier::GenerateOutput(U32 inOutputDataElementIndex,
	void*& rpOutData,
	BOOL& rNeedRelease)
{
	IFXRESULT rc = IFX_OK;

	if(inOutputDataElementIndex == m_BoneWeightsIndex)
	{
		if( !boneWeightsAreValid() )
			rc = Initialize();
		if( IFXSUCCESS( rc ) )
			rpOutData = (void*)m_pBoneWeights;
	}
	else
		rc = IFX_E_INVALID_RANGE;

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::SetDataPacket(
	IFXModifierDataPacket* pInInputDataPacket,
	IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT rc = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInDataPacket && pInInputDataPacket)
	{
		pInDataPacket->AddRef();
		m_pModifierDataPacket = pInDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(rc))
	{
		rc = pInDataPacket->GetDataElementIndex(  DID_IFXBoneWeights,
			m_BoneWeightsIndex );
	}


	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::Notify(  IFXModifierMessage eInMessage,
										  void*               pMessageContext )
{
	return IFX_OK;
}

IFXRESULT CIFXBoneWeightsModifier::InitializeMeshMapFromPointSetResource(
	IFXMeshMap **ppMeshMap )
{ 
	IFXRESULT rc = IFX_OK;
	IFXDECLARELOCAL( IFXAuthorPointSetResource, pPS);

	IFXDECLARELOCAL( IFXModifier, pMod );
	IFXDECLARELOCAL( IFXModifier, pMod2 );
	IFXDECLARELOCAL( IFXModifierChain, pMC );
	IFXDECLARELOCAL( IFXModifierChain, pMC2 );
	IFXDECLARELOCAL( IFXUnknown, pUnk );

	rc =  this->QueryInterface( IID_IFXModifier, (void**)&pMod );
	U32 uIdx;

	if( IFXSUCCESS( rc ) )
		rc = pMod->GetModifierChain( &pMC );

	// Get first modifier from the Modifier Chain
	if( IFXSUCCESS( rc ) )
		rc = pMC->GetModifier( 0, pMod2 );

	if  (IFXFAILURE( pMod2->QueryInterface(
										IID_IFXAuthorPointSetResource, 
										(void**)&pPS ) 
										  )
		)
	{
		// 1. Get Node name from NODE Palette
		IFXDECLARELOCAL( IFXPalette, pNodePal );
		IFXDECLARELOCAL( IFXPalette, pGenPal );
		IFXDECLARELOCAL( IFXSceneGraph, pSG );
		IFXDECLARELOCAL( IFXMarker, pMark );
		IFXDECLARELOCAL( IFXModel, pModel );

		rc = pMod2->QueryInterface( IID_IFXMarker, (void**)&pMark );
		if( IFXSUCCESS( rc ) )
			rc = pMark->GetSceneGraph( &pSG );
		rc = pMod2->QueryInterface( IID_IFXModel, (void**)&pModel );

		if( IFXSUCCESS( rc ) )
			uIdx = pModel->GetResourceIndex();

		// 2. We got the name, so found generator in the GENERATOR palette
		if( IFXSUCCESS( rc ) )
			rc = pSG->GetPalette( IFXSceneGraph::GENERATOR, &pGenPal );
		if( IFXSUCCESS( rc ) )
			rc = pGenPal->GetResourcePtr( uIdx, &pUnk );

		// 3. We got pointer to an object, now try to query it to
		// Author LineSet  Resource i-face
		if( IFXSUCCESS( rc ) )
			rc = pUnk->QueryInterface( IID_IFXAuthorPointSetResource, (void**)&pPS );
	}

	// 4. We got it, so use this i-face to initialize its skeleton's
	// bone weights
	if( IFXSUCCESS( rc ) && pPS ) 
	{
		rc = pPS->GetRenderMeshMap( ppMeshMap );
		IFXASSERT( ppMeshMap );
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::InitializeMeshMapFromLineSetResource(
	IFXMeshMap **ppMeshMap )
{ 
	IFXRESULT rc = IFX_OK;
	IFXDECLARELOCAL( IFXAuthorLineSetResource, pLS);
	IFXDECLARELOCAL( IFXModifier, pMod );
	IFXDECLARELOCAL( IFXModifier, pMod2 );
	IFXDECLARELOCAL( IFXModifierChain, pMC );
	IFXDECLARELOCAL( IFXModifierChain, pMC2 );
	IFXDECLARELOCAL( IFXUnknown, pUnk );

	rc =  this->QueryInterface( IID_IFXModifier, (void**)&pMod );
	U32 uIdx;

	if( IFXSUCCESS( rc ) )
		rc = pMod->GetModifierChain( &pMC );

	// Get first modifier from the Modifier Chain
	if( IFXSUCCESS( rc ) )
		rc = pMC->GetModifier( 0, pMod2 );

	if( IFXFAILURE( pMod2->QueryInterface( IID_IFXAuthorLineSetResource,
		(void**)&pLS ) ) )
	{
		// If the first modifier in the MC isn't an Author LineSet Resource
		// then this means that we are in Node Modifier Chain (appended MC)
		// and we need to find Author LineSet Resource in GENERATOR Palette:
		// 1. Get Node name from NODE Palette
		IFXDECLARELOCAL( IFXPalette, pNodePal );
		IFXDECLARELOCAL( IFXPalette, pGenPal );
		IFXDECLARELOCAL( IFXSceneGraph, pSG );
		IFXDECLARELOCAL( IFXMarker, pMark );
		IFXDECLARELOCAL( IFXModel, pModel );

		rc = pMod2->QueryInterface( IID_IFXMarker, (void**)&pMark );
		if( IFXSUCCESS( rc ) )
			rc = pMark->GetSceneGraph( &pSG );
		rc = pMod2->QueryInterface( IID_IFXModel, (void**)&pModel );

		if( IFXSUCCESS( rc ) )
			uIdx = pModel->GetResourceIndex();

		// 2. We got the name, so found generator in the GENERATOR palette
		if( IFXSUCCESS( rc ) )
			rc = pSG->GetPalette( IFXSceneGraph::GENERATOR, &pGenPal );
		if( IFXSUCCESS( rc ) )
			rc = pGenPal->GetResourcePtr( uIdx, &pUnk );

		// 3. We got pointer to an object, now try to query it to
		// Author LineSet  Resource i-face
		if( IFXSUCCESS( rc ) )
			rc = pUnk->QueryInterface( IID_IFXAuthorLineSetResource,
										(void**)&pLS );
	}

	// 4. We got it, so use this i-face to initialize its skeleton's
	// bone weights
	if( IFXSUCCESS( rc ) && pLS ) 
	{
		rc = pLS->GetRenderMeshMap( ppMeshMap );
		IFXASSERT( ppMeshMap );
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::InitializeMeshMapFromCLODResource(
	IFXMeshMap **ppMeshMap )
{ 
	IFXRESULT rc = IFX_OK;
	IFXDECLARELOCAL( IFXAuthorCLODResource, pCLOD);
	IFXDECLARELOCAL( IFXModifier, pMod );
	IFXDECLARELOCAL( IFXModifier, pMod2 );
	IFXDECLARELOCAL( IFXModifierChain, pMC );
	IFXDECLARELOCAL( IFXModifierChain, pMC2 );
	IFXDECLARELOCAL( IFXUnknown, pUnk );

	U32 uIdx;

	rc =  this->QueryInterface( IID_IFXModifier, (void**)&pMod );
	if( IFXSUCCESS( rc ) )
		rc = pMod->GetModifierChain( &pMC );

	// Get first modifier from the Modifier Chain
	if( IFXSUCCESS( rc ) )
		rc = pMC->GetModifier( 0, pMod2 );

	if( IFXFAILURE( pMod2->QueryInterface( IID_IFXAuthorCLODResource,
		(void**)&pCLOD ) ) )
	{
		// If the first modifier in the MC isn't an Author CLOD Resource
		// then this means that we are in Node Modifier Chain (appended MC)
		// and we need to find Author CLOD Resource in GENERATOR Palette:
		// 1. Get Node name from NODE Palette
		IFXDECLARELOCAL( IFXPalette, pNodePal );
		IFXDECLARELOCAL( IFXPalette, pGenPal );
		IFXDECLARELOCAL( IFXSceneGraph, pSG );
		IFXDECLARELOCAL( IFXMarker, pMark );
		IFXDECLARELOCAL( IFXModel, pModel );

		rc = pMod2->QueryInterface( IID_IFXMarker, (void**)&pMark );
		if( IFXSUCCESS( rc ) )
			rc = pMark->GetSceneGraph( &pSG );
		rc = pMod2->QueryInterface( IID_IFXModel, (void**)&pModel );

		if( IFXSUCCESS( rc ) )
			uIdx = pModel->GetResourceIndex();

		// 2. We got the name, so found generator in the GENERATOR palette
		if( IFXSUCCESS( rc ) )
			rc = pSG->GetPalette( IFXSceneGraph::GENERATOR, &pGenPal );
		if( IFXSUCCESS( rc ) )
			rc = pGenPal->GetResourcePtr( uIdx, &pUnk );

		// 3. We got pointer to an object, now try to query it to
		// Author CLOD Resource i-face
		if( IFXSUCCESS( rc ) )
			rc = pUnk->QueryInterface(IID_IFXAuthorCLODResource,
									  (void**)&pCLOD );

	}

	// 3.a if still have not got CLOD - mark rc as object not found 
	if (!IFXSUCCESS( rc ))
		rc = IFX_E_CANNOT_FIND;


	// 4. We got it, so use this i-face to initialize its skeleton's
	// bone weights
	if( IFXSUCCESS( rc ) && pCLOD ) 
	{
		rc = pCLOD->GetRenderMeshMap( ppMeshMap );
		IFXASSERT( ppMeshMap );
	}

	IFXRETURN(rc);
}

/**
	Initialize IFXMesh bone weights based on information for Author Mesh
*/
IFXRESULT CIFXBoneWeightsModifier::Initialize( void )
{
	IFXRESULT rc = IFX_OK;
	IFXDECLARELOCAL( IFXMeshMap, pRMM );
	U32 uIdx;

	if( IFXSUCCESS( rc ) && m_uABWSize > 0 ) 
	{
		rc = InitializeMeshMapFromCLODResource( &pRMM); 
		
		if (rc == IFX_E_CANNOT_FIND) 
		{
			rc = InitializeMeshMapFromLineSetResource( &pRMM); 
			
			if( IFXSUCCESS( rc )) 
			{
				m_resourceType = 1;  // LineSet
			} 
		} 
		else if( IFXSUCCESS( rc )) 
		{
			m_resourceType = 0;  // CLOD 
		}

		if (rc == IFX_E_CANNOT_FIND  || rc == IFX_E_UNSUPPORTED ) 
		{
			rc = InitializeMeshMapFromPointSetResource( &pRMM); 
			if( IFXSUCCESS( rc )) 
			{
				m_resourceType = 2;  // PointSet
			}
		}

		if( IFXSUCCESS( rc )) 
		{
			IFXVertexMap *pVM = NULL;
			if( IFXSUCCESS( rc ) && pRMM )
				pVM = pRMM->GetPositionMap();

			IFXASSERT( pVM );
			if( !pVM )
				rc = IFX_E_NOT_INITIALIZED;

			U32 i, j, uVertIdx = 0, uMeshIdx =0, uTotal;
			U32 uVertCnt = 0, uWeightCnt = 0, uMaxVert = 0, uMeshIdxMax = 0;

			if( IFXSUCCESS( rc ) )
			{
				uTotal = pVM->GetNumMapEntries();
				/// @todo [5/3/2006] check if we need this by looking 
				/// into import/export
				// IFXASSERT( uTotal == m_uABWSize ); 
			}

			// get bones manager from Modifier Chain
			if( IFXSUCCESS( rc ) )
			   rc = m_pInputDataPacket->GetDataElementIndex(DID_IFXBonesManager,
														m_BonesManagerIndex );

			if( IFXSUCCESS( rc ) )
				rc = m_pInputDataPacket->GetDataElement(m_BonesManagerIndex,
														(void**)&m_pBonesMgr );

			if( IFXSUCCESS( rc ))
			{
				if(0 == m_pBonesMgr)
				{
					rc = IFX_E_OUT_OF_MEMORY;
				}
				else
				{
					m_pBonesMgr->ResetUnshared();
				}

				if(IFXSUCCESS(rc))
				{
					IFXCharacter *pCharacter = NULL;
					rc = m_pBonesMgr->GetCharacter(&pCharacter);

					if(IFXSUCCESS(rc))
					{
						m_pBoneWeights = &pCharacter->GetSkin()->
												GetPackVertexWeights(this);

						m_pBoneWeights->Clear();
					}
				}
			}
			else
			{
				rc = IFX_E_UNDEFINED;
			}

			// Now do all required initialization:
			// 1. Count number of render mesh vertices with weights
			// and total number of weights - this is required for
			// pre-allocation
			// 1.a. Do initialization for 0-th mesh and count total number
			// of meshes
			if( IFXSUCCESS( rc ) )
			{
				for( i = 0; i < m_uABWSize; ++i )
				{
					uIdx = m_pAuthorBoneWeights[i].GetVertexIdx();
					if(uIdx!=IFXVertexBoneWeights::m_sVertexIdxUndefinedMarker)
						uTotal = pVM->GetNumVertexCopies( uIdx );
					else // Unitialiazed/unused m_pAuthorBoneWeights[i] 
						uTotal = 0;

					/// uMaxVert, uMeshIdxMax and uWeightCnt calculation
					for( j = 0; j < uTotal; j++ )
					{
						rc = pVM->GetVertexCopy(uIdx, j, &uMeshIdx, &uVertIdx);

						if( IFXSUCCESS( rc ) )
						{
							if( uMeshIdx == 0 )
							{
								if( uVertIdx > uMaxVert )
									uMaxVert = uVertIdx;
								uVertCnt++;
								uWeightCnt += 
									m_pAuthorBoneWeights[i].GetWeightCnt();
							}
							else
							{
								if( uMeshIdx > uMeshIdxMax )
									uMeshIdxMax = uMeshIdx;
							}
						}
						else
						{
							j = uTotal;
							i = m_uABWSize;
						}
					}
				}
			}

			U32 *pVertCnt = NULL;
			U32 *pMaxVert = NULL;
			U32 *pWeightCnt = NULL;

			if( IFXSUCCESS( rc ) )
			{
				// 1.b. Allocate data for total number of 
				// meshes (except 0-th mesh)
				pVertCnt = new U32[ uMeshIdxMax+1 ];
				pMaxVert = new U32[ uMeshIdxMax+1 ];
				pWeightCnt = new U32[ uMeshIdxMax+1 ];

				if( !pVertCnt || !pWeightCnt )
					rc = IFX_E_OUT_OF_MEMORY;

				// Set data to zero
				if( IFXSUCCESS( rc ) )
				{
					for( i = 0; i < uMeshIdxMax+1; i++ )
					{
						pVertCnt[i] = 0;
						pMaxVert[i] = 0;
						pWeightCnt[i] = 0;
					}
				}
			}

			// Initialize 0-th mesh
			if( IFXSUCCESS( rc ) )
			{   
				pVertCnt[0] = uVertCnt;
				pMaxVert[0] = uMaxVert;
				pWeightCnt[0] = uWeightCnt;
				rc = this->SetTotalBoneWeightCount( 0, uVertCnt, uWeightCnt );
			}

			if( IFXSUCCESS( rc ) && uMeshIdxMax > 0 )
			{
				// 1.c. Count number of vertices and weights for meshes 
				// with index > 0 (if any)
				for( i = 0; i < m_uABWSize; i++ )
				{
					uIdx = m_pAuthorBoneWeights[i].GetVertexIdx();
					if (IFXVertexBoneWeights::m_sVertexIdxUndefinedMarker
																!= uIdx )
						uTotal = pVM->GetNumVertexCopies( uIdx );
					else // Unitialiazed/unused m_pAuthorBoneWeights[i] 
						uTotal = 0;

					for( j = 0; j < uTotal; j++ )
					{
						rc = pVM->GetVertexCopy(uIdx, j, &uMeshIdx, &uVertIdx);

						if( IFXSUCCESS( rc ) )
						{
							if( uMeshIdx > 0 )
							{
								if( uVertIdx > pMaxVert[ uMeshIdx ] )
									pMaxVert[ uMeshIdx ] = uVertIdx;
								pVertCnt[ uMeshIdx ]++;
								pWeightCnt[ uMeshIdx ] += 
									m_pAuthorBoneWeights[i].GetWeightCnt();
							}
						}
						else
						{
							j = uTotal;
							i = m_uABWSize;
						}
					}
				}

				for( i = 1; i < uMeshIdxMax+1; i++ )
				{
					if( IFXSUCCESS( rc ) )
						rc = this->SetTotalBoneWeightCount( i, 
											pVertCnt[i], pWeightCnt[i] );
					else
						i = uMeshIdxMax + 1;
				}
			}

			IFXVertexBoneWeights **ppMeshBW = NULL;

			if( IFXSUCCESS( rc ) )
			{
				ppMeshBW = new IFXVertexBoneWeights*[ uMeshIdxMax+1 ];
				if( !ppMeshBW )
					rc = IFX_E_OUT_OF_MEMORY;
				else
				{
					for( i = 0; i < uMeshIdxMax+1; i++ )
						ppMeshBW[i] = NULL;
				}
			}

			if( IFXSUCCESS( rc ) )
			{
				for( i = 0; i < uMeshIdxMax+1; i++ )
				{
					if( IFXSUCCESS( rc ) )
					{
						ppMeshBW[i] = new IFXVertexBoneWeights[pMaxVert[i]+1];
						if( !ppMeshBW[i] )
							rc = IFX_E_OUT_OF_MEMORY;
					}
					else
						i = uMeshIdxMax + 1;
				}
			}

			// 2. Set up data
			U32 *puIDs;
			F32 *pfW;
			U32 uCnt;
			U32 k;
			if( IFXSUCCESS( rc ) )
			{
				for( i = 0; i < m_uABWSize; i++ )
				{
					uIdx = m_pAuthorBoneWeights[i].GetVertexIdx();
					if (IFXVertexBoneWeights::m_sVertexIdxUndefinedMarker
																	!= uIdx)
						uTotal = pVM->GetNumVertexCopies( uIdx );     
					else // Unitialiazed/unused m_pAuthorBoneWeights[i]
						uTotal = 0;

					uCnt = m_pAuthorBoneWeights[i].GetWeightCnt();
					puIDs = m_pAuthorBoneWeights[i].GetBoneIDs();
					pfW = m_pAuthorBoneWeights[i].GetWeights();

					for( j = 0; j < uTotal; j++ )
					{
						if( IFXSUCCESS( rc ) )
						{
						  rc=pVM->GetVertexCopy(uIdx, j, &uMeshIdx, &uVertIdx);

						  ppMeshBW[uMeshIdx][uVertIdx].SetVertexIdx(uVertIdx);
						  ppMeshBW[uMeshIdx][uVertIdx].SetWeightCnt(uCnt);
						  for( k = 0; k < uCnt; k++ )
						  {
							ppMeshBW[uMeshIdx][uVertIdx].SetBoneID(k,puIDs[k]);
							ppMeshBW[uMeshIdx][uVertIdx].SetWeight(k,pfW[k]);
						  }
						}
						else
						{
							j = uTotal;
							i = m_uABWSize;
						}
					}
				}
			}

			F32 sum;
			if( IFXSUCCESS( rc ) )
			{
				for( i = 0; i < uMeshIdxMax+1; i++ )
				{
					for( j = 0; j < pMaxVert[i]+1; j++ )
					{
						if( IFXSUCCESS( rc ) )
						{
							uCnt = ppMeshBW[ i ][ j ].GetWeightCnt();

							if( uCnt )
							{
								puIDs = ppMeshBW[ i ][ j ].GetBoneIDs();
								pfW = ppMeshBW[ i ][ j ].GetWeights();

								if( !pfW || !puIDs )
								{
									rc = IFX_E_NOT_INITIALIZED;
								}
								else
								{
									sum = 0.0f;
									for( k = 0; k < uCnt - 1; k++ )
									{
										sum += pfW[k];
									}

#ifdef _DEBUG
									if( sum > 1.0f )
										IFXASSERT( 0 );
#endif

									if( pfW[ uCnt - 1 ] != (F32)(1.0f - sum) )
									{
										/// @todo this is sometimes true 
										/// because of precision loss
										//IFXASSERT( 0 );
										pfW[ uCnt - 1 ] = 1.0f - sum;
									}

									rc = this->SetBoneWeights( j, i,
										uCnt, puIDs, pfW );
								}
							}
						}
						else
						{
							j = pVertCnt[i];
							i = uMeshIdxMax + 1;
						}
					}
				}
			}

			delete [] pVertCnt;
			delete [] pMaxVert;
			delete [] pWeightCnt;

			if( ppMeshBW )
			{
				for( i = 0; i < uMeshIdxMax+1; i++ )
				{
					delete [] ppMeshBW[i];
				}
			}

			delete [] ppMeshBW;

		}
	}

	if(NULL == m_pBoneWeights)
	{
		rc = IFX_E_UNDEFINED;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::SetTotalBoneWeightCount( U32 uMeshIndex,
														U32 uTotalVertexCount,
														U32 uTotalWeightCount)
{
	IFXRESULT rc = IFX_OK;

	if( !m_pBonesMgr || !boneWeightsAreValid())
		rc = Initialize();

	if(IFXSUCCESS(rc))
	{
		m_totalWeights.ResizeToAtLeast(uMeshIndex+1);
		m_totalWeights[uMeshIndex]=uTotalWeightCount;

		m_pLastVertex.ResizeToAtLeast(uMeshIndex+1);
		m_pLastVertex[uMeshIndex] = NULL;

		m_lastGoodVertID.ResizeToAtLeast(uMeshIndex+1);
		m_lastGoodVertID[uMeshIndex] = 0;

		m_lastReadOffset.ResizeToAtLeast(uMeshIndex+1);
		m_lastReadOffset[uMeshIndex] = 0;

		m_pBoneWeights->ResizeToAtLeast(uMeshIndex+1);

		// check that not already allocated
		if((*m_pBoneWeights)[uMeshIndex].GetNumberVerticesConst() != 0)
			return IFX_E_ALREADY_INITIALIZED;

//          m_pBoneWeights->Clear(uMeshIndex+1);
//      m_pBoneWeights->ResizeToAtLeast(uMeshIndex+1);
		(*m_pBoneWeights)[uMeshIndex].Allocate(uTotalVertexCount,
												uTotalWeightCount);
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::GetTotalBoneWeightCount(U32 meshIndex,
													U32 *pTotalWeightCount)
{
	IFXRESULT rc = IFX_OK;

//  if (!pTotalWeightCount)
//      rc = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS(rc) && !boneWeightsAreValid() )
		rc = Initialize();

	if( IFXSUCCESS(rc) )
	{
		if(meshIndex >= (U32)m_totalWeights.GetNumberElements())
			rc = IFX_E_INVALID_RANGE;
		else
			*pTotalWeightCount = m_totalWeights[meshIndex];
	}

	IFXRETURN(rc);
}


IFXRESULT CIFXBoneWeightsModifier::SetBoneWeights(U32 uVertexIndex,
												  U32 uMeshIndex,
												  U32 uWeightCount,
												  U32 *puBoneIDs,
												  F32 *pfBoneWeights )
{
	IFXRESULT rc = IFX_OK;

	if(uWeightCount<1)
		return IFX_OK;  // failure code might abort caller

	if (!puBoneIDs || !pfBoneWeights)
		rc = IFX_E_INVALID_POINTER;

	if( !boneWeightsAreValid() )
		rc = Initialize();

	if( IFXSUCCESS(rc) )
	{
		if(uMeshIndex >= m_pBoneWeights->GetNumberElements())
			return IFX_E_INVALID_RANGE;
		IFXPackWeights& rPackWeights = m_pBoneWeights->GetElement(uMeshIndex);

		if(uVertexIndex < rPackWeights.GetNumberVerticesConst())
			return IFX_E_ALREADY_INITIALIZED;

		IFXPackVertex *pv = rPackWeights.NextPackVertexForWrite();
		if(!pv)
			return IFX_E_INVALID_RANGE;
		pv->SetVertexIndex(uVertexIndex);
		pv->SetNumWeights(uWeightCount);

		U32 m;
		for(m = 0; m < uWeightCount; ++m)
		{
			IFXPackBoneWeight *const pw = rPackWeights.NextPackBoneWeightForWrite();
			if(!pw)
				return IFX_E_WRITE_FAILED;
			IFXASSERT(NULL != puBoneIDs);
			IFXASSERT(NULL != pfBoneWeights);
			pw->SetBoneIndex(puBoneIDs[m]);
			pw->SetBoneWeight(pfBoneWeights[m]);
		}
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::GotoVertex(U32 uVertexIndex, U32 uMeshIndex)
{
	IFXRESULT rc = IFX_OK;

	if(uMeshIndex >= m_pBoneWeights->GetNumberElements())
		return IFX_E_INVALID_RANGE;

	IFXPackWeights &rPackWeights (m_pBoneWeights->GetElement(uMeshIndex));

	//* reset to start if we've already passed it
	
	if(uVertexIndex < m_lastGoodVertID[uMeshIndex] ||
		m_lastReadOffset[uMeshIndex] !=  rPackWeights.GetReadOffset())
	{
		rPackWeights.RewindForRead();
		m_lastReadOffset[uMeshIndex] = 0;
		m_pLastVertex[uMeshIndex] = NULL;
		m_lastGoodVertID[uMeshIndex] = 0;
	}
	
	//* search ahead for specified vertex
	while(IFXSUCCESS(rc) && (!m_pLastVertex[uMeshIndex] ||
		m_pLastVertex[uMeshIndex]->GetVertexIndex() < uVertexIndex))
	{
		{
			U8 *pLastVertex = (U8*)(m_pLastVertex[uMeshIndex]);
			U8 *pLastRead = (U8*)rPackWeights.GetBufferAddr() +
				m_lastReadOffset[uMeshIndex] - 
				sizeof(IFXPackVertex);

			if(pLastVertex == pLastRead)
			{
				// we are in vertex now, should skip weights block before
				// calling NextPackVertexForRead() 
				U32 const wCount = m_pLastVertex[uMeshIndex]->GetNumWeights();

				rc = rPackWeights.skipWeights(wCount);

				if(IFXSUCCESS(rc))
					m_lastReadOffset[uMeshIndex] = rPackWeights.GetReadOffset();
			}
		}

		m_pLastVertex[uMeshIndex] = rPackWeights.NextPackVertexForRead();
		m_lastReadOffset[uMeshIndex] = rPackWeights.GetReadOffset();

		if(!m_pLastVertex[uMeshIndex])
		{
			m_lastGoodVertID[uMeshIndex] = 0;
			rc = IFX_E_NOT_INITIALIZED;
		}
		else
		{
			m_lastGoodVertID[uMeshIndex] = 
				m_pLastVertex[uMeshIndex]->GetVertexIndex();
		}

		//* if this ain't the one, skip over the associated weights
		if(IFXSUCCESS(rc) && 
			(m_pLastVertex[uMeshIndex]->GetVertexIndex() < uVertexIndex))
		{
			U32 const wCount = m_pLastVertex[uMeshIndex]->GetNumWeights();

			rc = rPackWeights.skipWeights(wCount);

			if(IFXSUCCESS(rc))
				m_lastReadOffset[uMeshIndex] = rPackWeights.GetReadOffset();
		}
		
		if (IFXSUCCESS(rc) 
			&& (m_pLastVertex[uMeshIndex]->GetVertexIndex() > uVertexIndex)) 
		{
			//passed over the one, return error code
			rc = IFX_E_NOT_FOUND;
		}
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::GetBoneWeightCount(U32 uVertexIndex,
													  U32 uMeshIndex,
													  U32 *puWeightCount)
{
	IFXRESULT rc = IFX_OK;

	if (!puWeightCount)
		rc = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS(rc) )
		*puWeightCount=0;

	if( !boneWeightsAreValid() )
		rc = Initialize();

	if( IFXSUCCESS(rc) )
	{
		if((rc = GotoVertex(uVertexIndex,uMeshIndex))!=IFX_OK)
			return IFX_OK;      // real error might abort caller

		if(!m_pLastVertex[uMeshIndex])
			return IFX_OK;

		/// @todo: suspect comparison: why is this <=, not == ?
		if(m_pLastVertex[uMeshIndex]->GetVertexIndex() <= uVertexIndex)
			*puWeightCount=m_pLastVertex[uMeshIndex]->GetNumWeights();
	}

	IFXRETURN(rc);
}

/// copy RT bone data into given buffers
IFXRESULT CIFXBoneWeightsModifier::GetBoneWeights(  U32 uVertexIndex,
												  U32 uMeshIndex,
												  U32 uWeightCount,
												  U32 *puBoneIDs,
												  F32 *pfBoneWeights )
{
	IFXRESULT rc = IFX_OK;
	if (!puBoneIDs && !pfBoneWeights)
		rc = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS(rc) && !boneWeightsAreValid() )
		rc = Initialize();

	if( IFXSUCCESS(rc) )
	{
		if((rc=GotoVertex(uVertexIndex,uMeshIndex))!=IFX_OK)
			IFXRETURN(rc);

		if(!m_pLastVertex[uMeshIndex])
			return IFX_E_INVALID_RANGE;

		if(m_pLastVertex[uMeshIndex]->GetVertexIndex()!=uVertexIndex)
			return IFX_E_CANNOT_FIND;

		if(uMeshIndex >= m_pBoneWeights->GetNumberElements())
			return IFX_E_INVALID_RANGE;
		IFXPackWeights& rPackWeights = m_pBoneWeights->GetElement(uMeshIndex);

		if(m_pLastVertex[uMeshIndex]->GetNumWeights() != uWeightCount)
			return IFX_E_ALREADY_INITIALIZED;

		U32 m;
		for(m = 0; m < uWeightCount; m++)
		{
			IFXPackBoneWeight const *pw = rPackWeights.
											NextPackBoneWeightForRead();
			m_lastReadOffset[uMeshIndex] = rPackWeights.GetReadOffset();
			if(!pw)
				return IFX_E_READ_FAILED;
			puBoneIDs[m] = pw->GetBoneIndex();
			pfBoneWeights[m] = pw->GetBoneWeight();
		}

		m_pLastVertex[uMeshIndex] = NULL;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXBoneWeightsModifier::SetTotalBoneWeightCountForAuthorMesh(
	U32 uTotalVertexCount )
{
	IFXRESULT res = IFX_OK;

	IFXDELETE_ARRAY( m_pAuthorBoneWeights );

	m_pAuthorBoneWeights = new IFXVertexBoneWeights[ uTotalVertexCount ];

	if( NULL != m_pAuthorBoneWeights )
		m_uABWSize = uTotalVertexCount;
	else
		res = IFX_E_OUT_OF_MEMORY;

	IFXRETURN(res);
}

IFXRESULT CIFXBoneWeightsModifier::SetBoneWeightsForAuthorMesh(U32 vertexIndex,
															U32 weightCount,
															U32 *pBoneIDs,
															F32 *pBoneWeights)
{
	IFXRESULT res = IFX_OK;

	if( !m_pAuthorBoneWeights )
		res = IFX_E_NOT_INITIALIZED;
	else if( vertexIndex >= m_uABWSize )
		res = IFX_E_INVALID_RANGE;
	else if( !pBoneIDs || !pBoneWeights )
		res = IFX_E_INVALID_POINTER;

	// check: The sum of all the bone weights at the position should be 1.0
	if( IFXSUCCESS( res ) )
	{
		U32 i;
		F32 sum = 0;

		for (i = 0; i < weightCount; ++i )
			sum += pBoneWeights[i];

		if ( fabs (sum - 1) >= IFX_EPSILON )
			res = IFX_E_INVALID_RANGE;
	}

	if( IFXSUCCESS( res ) )
	{
		m_pAuthorBoneWeights[ vertexIndex ].SetVertexIdx( vertexIndex );
		m_pAuthorBoneWeights[ vertexIndex ].SetWeightCnt( weightCount );

		U32 i;
		for(  i = 0; i < weightCount; i++ )
		{
			m_pAuthorBoneWeights[ vertexIndex ].SetBoneID( i, pBoneIDs[i] );
			m_pAuthorBoneWeights[ vertexIndex ].SetWeight( i, pBoneWeights[i]);
		}
	}

	return res;
}

IFXVertexBoneWeights::IFXVertexBoneWeights()
{
	m_vertexIdx = m_sVertexIdxUndefinedMarker;
	m_weightCnt = 0;
	m_pBoneIDs = NULL;
	m_pWeights = NULL;
}

IFXVertexBoneWeights::~IFXVertexBoneWeights()
{
	IFXDELETE_ARRAY( m_pBoneIDs );
	IFXDELETE_ARRAY( m_pWeights );
}

void IFXVertexBoneWeights::SetWeightCnt( U32 w )
{
	m_weightCnt = w;
	
	IFXDELETE_ARRAY( m_pBoneIDs );
	m_pBoneIDs = new U32[w];

	IFXDELETE_ARRAY( m_pWeights );
	m_pWeights = new F32[w];
}

void IFXVertexBoneWeights::SetBoneID( U32 i, U32 id )
{
	if( i < m_weightCnt )
	{
		m_pBoneIDs[i] = id;
	}
}

void IFXVertexBoneWeights::SetWeight( U32 i, F32 wg )
{
	if( i < m_weightCnt )
	{
		m_pWeights[i] = wg;
	}
}
