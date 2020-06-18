//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
	@file	CIFXBoneWeightsModifierEncoder.cpp

			Implementation of the CIFXBoneWeightsModifierEncoder.
			The CIFXBoneWeightsModifierEncoder contains Bone Weight Modifier 
			modifier encoding functionality that is used by the write manager.
*/


#include "CIFXBoneWeightsModifierEncoder.h"
#include "IFXACContext.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXBoneWeightsModifier.h"
#include "IFXModifierChain.h"
#include "IFXSceneGraph.h"
#include "IFXAuthorCLODResource.h"
#include "IFXAuthorLineSetResource.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXModel.h"

#include "IFXVertexMap.h"
#include "IFXMeshGroup.h"
#include "IFXMeshMap.h"

void GetVertexMapContainerX( 
						IFXMeshMap *pMeshDesc, 
   						U32* pMeshIdx, 
   						U32* pVertIdx,
   						U32 uPosCnt );

// constructor
CIFXBoneWeightsModifierEncoder::CIFXBoneWeightsModifierEncoder() :
IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXBoneWeightsModifierEncoder::~CIFXBoneWeightsModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXBoneWeightsModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXBoneWeightsModifierEncoder::Release()
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

IFXRESULT CIFXBoneWeightsModifierEncoder::QueryInterface( IFXREFIID interfaceId,
														 void**  ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}


// IFXEncoderX
void CIFXBoneWeightsModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXBoneWeightsModifier* pBoneWeightsModifier = NULL;
	U32 *pMeshIdx = NULL;
	U32 *pVertIdx = NULL;
	U32 *puBoneIDs = NULL;
	F32 *pfWeights = NULL;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXBoneWeightsModifier, (void**)&pBoneWeightsModifier ) );

		// The following elements are common to all Modifier blocks
		// and are encoded in EncodeCommonElements():
		// 1. ModelGenerator Name
		// 2. Index for modifier chain

		m_pBitStream->WriteIFXStringX( rName );

		U32 uTemp = 0;
		IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
		m_pBitStream->WriteU32X( uTemp );

		

		IFXDECLARELOCAL( IFXAuthorCLODResource, pCLOD);
		IFXDECLARELOCAL( IFXAuthorLineSetResource, pLines );
		IFXDECLARELOCAL( IFXAuthorPointSetResource, pPoints );
		IFXDECLARELOCAL( IFXAuthorCLODMesh, pMesh );
		IFXDECLARELOCAL( IFXAuthorLineSet, pAuthorLineSet );
		IFXDECLARELOCAL( IFXAuthorPointSet, pAuthorPointSet );
		IFXDECLARELOCAL( IFXPalette, pSGPalette );
		IFXDECLARELOCAL( IFXSceneGraph, pSG );
		IFXDECLARELOCAL( IFXModifierChain, pMC );
		IFXDECLARELOCAL( IFXModifier, pMod );
		IFXDECLARELOCAL( IFXModel, pModel );

		IFXCHECKX( m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSG ) );
		IFXCHECKX( pSG->GetPalette( IFXSceneGraph::GENERATOR, &pSGPalette ) );

		IFXCHECKX( m_pModifier->GetModifierChain( &pMC ) );
		IFXCHECKX( pMC->GetModifier( 0, pMod ) );

		/// @todo: It's better to get Render Mesh Map thru Modifier Chain
		if( IFXSUCCESS( pMod->QueryInterface( IID_IFXModel, (void**)&pModel ) ) )
		{
			U32 uResourceID = 0;
			// this means that we are in Node Mod Chain
			uResourceID = pModel->GetResourceIndex();
			IFXRELEASE( pMod );
			IFXCHECKX( pSGPalette->GetResourcePtr( uResourceID, IID_IFXModifier, (void**)&pMod ) );
		}

		if( IFXFAILURE( pMod->QueryInterface( IID_IFXAuthorCLODResource, (void**)&pCLOD ) ) )
		{			     
			if( IFXFAILURE( pMod->QueryInterface(IID_IFXAuthorLineSetResource,(void**)&pLines) ) )
			{
				if( IFXSUCCESS( pMod->QueryInterface(IID_IFXAuthorPointSetResource,(void**)&pPoints) ) )
				{
					uTemp = 0x00000004; // PointSet Attribute
				}
				else 
				{
					throw IFXException( IFX_E_CANNOT_FIND );
				}
			}
			else
			{
				uTemp = 0x00000002; // LineSet Attribute
			}
		}
		else
		{
			uTemp = 0x00000001; // CLOD Attribute
		}

		IFXDECLARELOCAL( IFXMeshMap, pRMM );
		IFXDECLARELOCAL( IFXMeshGroup, pMG );
		U32 uPosCnt = 0;

		if( uTemp == 0x00000001 ) // AuthorCLODRes
		{
			IFXASSERT( pCLOD );
			IFXCHECKX( pCLOD->GetAuthorMesh( pMesh ) );

			const IFXAuthorMeshDesc *pMeshDesc = pMesh->GetMeshDesc();
			IFXASSERT( pMeshDesc );
		
			IFXCHECKX( pCLOD->GetRenderMeshMap( &pRMM ) );
			IFXCHECKX( pCLOD->GetMeshGroup( &pMG ) );
			
			uPosCnt = pMeshDesc->NumPositions;
		}
		else if ( uTemp == 0x00000002 ) // LineSet
		{
			IFXASSERT( pLines );
			
			IFXCHECKX(pLines->GetAuthorLineSet(pAuthorLineSet));
			const IFXAuthorLineSetDesc *pLineSetDesc = pAuthorLineSet->GetLineSetDesc();
			IFXASSERT( pLineSetDesc );
            			
			IFXCHECKX( pLines->GetRenderMeshMap( &pRMM ) );
			IFXCHECKX( pLines->GetMeshGroup( &pMG ) );

			uPosCnt = pLineSetDesc->m_numPositions;
		}
		else if( uTemp == 0x00000004 ) // PointSet
		{
			IFXASSERT( pPoints );
			IFXCHECKX(pPoints->GetAuthorPointSet(pAuthorPointSet));
			const IFXAuthorPointSetDesc *pPointSetDesc = pAuthorPointSet->GetPointSetDesc();
			IFXASSERT( pPointSetDesc );

			IFXCHECKX( pPoints->GetRenderMeshMap( &pRMM ) );
			IFXCHECKX( pPoints->GetMeshGroup( &pMG ) );

			uPosCnt = pPointSetDesc->m_numPositions;
		}
		else
		{
			IFXASSERT(FALSE);
		}

		IFXASSERT( pMG->GetNumMeshes() );

		pMeshIdx = new U32[ uPosCnt ];
		pVertIdx = new U32[ uPosCnt ];

		if( pMeshIdx == NULL || pVertIdx == NULL )
		{
			throw IFXException( IFX_E_OUT_OF_MEMORY );
		}

		GetVertexMapContainerX( pRMM, pMeshIdx, pVertIdx, uPosCnt );

		

		// 3. Attributes
		m_pBitStream->WriteU32X( uTemp );

		U32 uQualityFactor = 1000;
		F32 fQuantBoneWeight = (F32)pow( 1.0022294514890519310704865897552, uQualityFactor + 1741.0 );
		F32 fInvQuant= 1.0f/fQuantBoneWeight;

		// 4. Inverse Quant
		m_pBitStream->WriteF32X( fInvQuant );

		U32 uNumWeightsTotal = 0;
		IFXCHECKX( pBoneWeightsModifier->GetTotalBoneWeightCount( 0, &uNumWeightsTotal ) );

		// 5. Position Count
		m_pBitStream->WriteU32X( uPosCnt );

		U32 i, j, uWeightCnt;

		// 6. Position Bone Weight List
		for( i = 0; i < uPosCnt; i++ )
		{
			IFXCHECKX( pBoneWeightsModifier->GetBoneWeightCount( pVertIdx[i], pMeshIdx[i], &uWeightCnt ) );

			// 6.1. Bone Weight Count (U32)
			m_pBitStream->WriteCompressedU32X( uACContextBoneWeightCount, uWeightCnt );

			if( uWeightCnt > 0 )
			{
				puBoneIDs = new U32[ uWeightCnt ];
				pfWeights = new F32[ uWeightCnt ];
				if( puBoneIDs == NULL || pfWeights == NULL )
				{
					throw IFXException( IFX_E_OUT_OF_MEMORY );
				}

				IFXCHECKX( pBoneWeightsModifier->GetBoneWeights( pVertIdx[i], pMeshIdx[i], uWeightCnt, puBoneIDs, pfWeights ) );

				// 6.2. Bone Indices
				for( j = 0; j < uWeightCnt; j++ )
				{
					m_pBitStream->WriteCompressedU32X( uACContextBoneWeightBoneID, puBoneIDs[j] );
				}

				// 6.3. Quantized Bone Weights
				// NOTE: Assumed that sum of all weights is 1.0f. Last weight won't be written
				// into a file basing on this assumption
				for( j = 0; j < (uWeightCnt - 1); j++ )
				{
					m_pBitStream->WriteCompressedU32X( uACContextBoneWeightBoneWeight, (U32)(pfWeights[j] / fInvQuant) );
				}

				delete [] puBoneIDs;
				delete [] pfWeights;
			}
		}

		delete [] pMeshIdx;
		delete [] pVertIdx;

		//-------------------------------------------------------------------------
		// Done with BoneWeights specific parameters.
		//-------------------------------------------------------------------------
		IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

		// Get a data block from the bitstream
		m_pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX(BlockType_ModifierBoneWeightsU3D);

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX(*pDataBlock);

		// clean up
		IFXRELEASE( pBoneWeightsModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( pBoneWeightsModifier );

		delete [] pMeshIdx;
		delete [] pVertIdx;
		delete [] puBoneIDs;
		delete [] pfWeights;

		throw;
	}
}

void CIFXBoneWeightsModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{
		// latch onto the core services object passed in
		IFXRELEASE( m_pCoreServices )
			m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();

		// create a bitstream
		IFXRELEASE( m_pBitStream );
		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamCompressedX,(void**)&m_pBitStream ) );
		U32 uProfile;
		m_pCoreServices->GetProfile(uProfile);
		m_pBitStream->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

		m_bInitialized = TRUE;
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		IFXRELEASE( m_pBitStream );

		throw;
	}
}

void CIFXBoneWeightsModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;

	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();

		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

		pModifier->AddRef();
		IFXRELEASE( m_pModifier );
		m_pModifier = pModifier;

		IFXRELEASE( pModifier );
	}
	catch (...)
	{
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter
		IFXRELEASE( pModifier );

		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXBoneWeightsModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXBoneWeightsModifierEncoder *pComponent = new CIFXBoneWeightsModifierEncoder;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			rc = IFX_E_OUT_OF_MEMORY;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN( rc );
}


// local function
void GetVertexMapContainerX(
							IFXMeshMap* pRMM, 
							U32* pMeshIdx, 
							U32* pVertIdx,
							U32 uPosCnt )
{
	U32 uNum2 = 0;
	IFXVertexMap* pRVM = pRMM->GetPositionMap();

	if( pRVM )
	{
		uNum2 = pRVM->GetNumMapEntries();

		IFXASSERT( uPosCnt == uNum2 );

		if (uNum2 <= 0) 
		{
			IFXCHECKX( IFX_E_NOT_INITIALIZED );
		}

		U32 f, uMeshIdx = 0, uVertIdx = 0;
		for( f = 0; f < uPosCnt; f++ )
		{
			// Get Mesh Index and Vertex Index for first copy of this author mesh
			// (since all copies should have the same weight)
			IFXCHECKX( pRVM->GetVertexCopy( f, 0, &uMeshIdx, &uVertIdx ) );
			pMeshIdx[f] = uMeshIdx;
			pVertIdx[f] = uVertIdx;
		}
	}
	else
	{
		IFXCHECKX( IFX_E_NOT_INITIALIZED );
	}
}
