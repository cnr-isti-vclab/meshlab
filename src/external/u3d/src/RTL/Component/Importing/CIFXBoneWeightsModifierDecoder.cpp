//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file CIFXBoneWeightsModifierDecoder.cpp
*/

#include "IFXCoreCIDs.h"
#include "CIFXBoneWeightsModifierDecoder.h"
#include "IFXBoneWeightsModifier.h"
#include "IFXCheckX.h"

CIFXBoneWeightsModifierDecoder::CIFXBoneWeightsModifierDecoder()
{
}

CIFXBoneWeightsModifierDecoder::~CIFXBoneWeightsModifierDecoder()
{
}

U32 CIFXBoneWeightsModifierDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXBoneWeightsModifierDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXBoneWeightsModifierDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = ( IFXDecoderX* ) this;
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
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

// Initialize and get a reference to the core services object
void CIFXBoneWeightsModifierDecoder::InitializeX(const IFXLoadConfig &lc)
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXBoneWeightsModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXBoneWeightsModifier);
}

void CIFXBoneWeightsModifierDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pObject) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL bDone = FALSE;
	while ( IFX_OK == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) {
			// Process the data block
			ProcessBoneWeightsModifierBlockX(*pDataBlockX);
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXBoneWeightsModifierDecoder::ProcessBoneWeightsModifierBlockX( IFXDataBlockX &rDataBlockX )
{
	// get the required interfaces of the object
	IFXDECLARELOCAL(IFXBoneWeightsModifier, pBoneWeightsModifier);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXBoneWeightsModifier, (void**)&pBoneWeightsModifier ));

	// The following elements are common to all Modifier blocks
	// and are decoded in DecodeCommonElements():
		// 1. ModelName
		// 2. Index for modifier chain

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	IFXDECLARELOCAL( IFXBitStreamCompressedX, pBitStreamX );

	// Create the bitstream for reading from the data block
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamCompressedX, (void**)&pBitStreamX ));
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	U32 uProfile;
	pNameMap->GetProfile(m_uLoadId, uProfile);
	pBitStreamX->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

	pBitStreamX->SetDataBlockX( rDataBlockX );

	// 1. Model Name
	pBitStreamX->ReadIFXStringX( m_stringObjectName);
	IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

	// 2. Index for modifier chain
	pBitStreamX->ReadU32X( m_uChainPosition );

	U32 uTemp, uPosCount, i, j, uWeightCnt;
	F32 fTemp;
	U32 uCnt = 0;

	// 3. Attributes (U32)
	pBitStreamX->ReadU32X( uTemp );

	// 4. Inverse Quant (F32)
	F32 fInvQuant = 1.0f;
	pBitStreamX->ReadF32X( fInvQuant );

	// 5. Position Count (U32)
	pBitStreamX->ReadU32X( uPosCount );

	IFXCHECKX( pBoneWeightsModifier->SetTotalBoneWeightCountForAuthorMesh( uPosCount ) );
	U32 *pBoneIDs = NULL;
	F32 *pWeights = NULL;

	// 6. Weight List
	for( i = 0; i < uPosCount; i++ )
	{
		// 6.1. Bone Weights Count
		pBitStreamX->ReadCompressedU32X( uACContextBoneWeightCount, uWeightCnt );

		if( uWeightCnt > 0 )
		{
			pBoneIDs = new U32[ uWeightCnt ];
			pWeights = new F32[ uWeightCnt ];
			uCnt += uWeightCnt;

			// 6.2. Bone Indeces
			for( j = 0; j < uWeightCnt; j++ )
			{
				pBitStreamX->ReadCompressedU32X( uACContextBoneWeightBoneID, uTemp );
				pBoneIDs[j] = uTemp;
			}

			// 6.3. Quantized Bone Weights
			// NOTE: Assumed that sum of all weights is 1.0f. Last weight won't be readed
			// into a file basing on this assumption
			F32 fSum = 0.0f;
			fTemp = 0.0f;
			for( j = 0; j < (uWeightCnt - 1); j++ )
			{
				pBitStreamX->ReadCompressedU32X( uACContextBoneWeightBoneWeight, uTemp );
				fTemp = uTemp * fInvQuant;
				pWeights[j] = fTemp;
				fSum += fTemp;
			}
			pWeights[ uWeightCnt - 1 ] = 1.0f - fSum;

			IFXCHECKX( pBoneWeightsModifier->SetBoneWeightsForAuthorMesh( i, uWeightCnt, pBoneIDs, pWeights ) );

			IFXDELETE_ARRAY(pWeights);
			IFXDELETE_ARRAY(pBoneIDs);
		}
		else
		{
			// Not calling this is OK, the method shouldn't called with NULL parameters. Doing nothing is OK.
			//IFXCHECKX( pBoneWeightsModifier->SetBoneWeightsForAuthorMesh( i, 0, NULL, NULL ) );
		}
	}
}

IFXRESULT IFXAPI_CALLTYPE CIFXBoneWeightsModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXBoneWeightsModifierDecoder *pComponent = new CIFXBoneWeightsModifierDecoder;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}
