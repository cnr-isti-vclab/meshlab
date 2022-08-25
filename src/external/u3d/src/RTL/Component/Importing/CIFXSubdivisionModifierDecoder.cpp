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
	@file	CIFXSubdivisionModifierDecoder.cpp

			The CIFXSubdivisionModifierDecoder is used by the CIFXLoadManager to load
			surface subdivision (SDS) modifiers into the scene graph.
*/

#include "IFXCoreCIDs.h"
#include "IFXSubdivModifier.h"
#include "CIFXSubdivisionModifierDecoder.h"
#include "IFXCheckX.h"
#include "IFXBitStreamX.h"

CIFXSubdivisionModifierDecoder::CIFXSubdivisionModifierDecoder()
{
}

CIFXSubdivisionModifierDecoder::~CIFXSubdivisionModifierDecoder()
{
}

U32 CIFXSubdivisionModifierDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXSubdivisionModifierDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXSubdivisionModifierDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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

	return rc;
}

// Initialize and get a reference to the core services object
void CIFXSubdivisionModifierDecoder::InitializeX(const IFXLoadConfig &lc)
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXSubdivisionModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXSubdivModifier);
}

void CIFXSubdivisionModifierDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
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
			ProcessSubdivisionModifierBlockX(*pDataBlockX);
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXSubdivisionModifierDecoder::ProcessSubdivisionModifierBlockX( IFXDataBlockX &rDataBlockX )
{
	// get the required interfaces of the object
	IFXDECLARELOCAL(IFXSubdivModifier,pSubdivisionModifier);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXSubdivModifier, (void**)&pSubdivisionModifier ));

	// The following elements are common to all Modifier blocks
	// and are decoded in DecodeCommonElements():
		// 1. ModelName
		// 2. Index for modifier chain

	// SDS Modifiers have the following parameters:
		// 1. Enabled, Adaptive
		// 2. Depth
		// 3. Tension
		// 4. Error

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	// Create the bitstream for reading from the data block
	IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
	{
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	// 1. Model Name
	pBitStreamX->ReadIFXStringX( m_stringObjectName);
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

	// 2. Index for modifier chain
	pBitStreamX->ReadU32X( m_uChainPosition );

	// 1. Enable
	U32 uTemp = 0;
	BOOL bTemp;
	pBitStreamX->ReadU32X( uTemp );
	bTemp = (uTemp & 0x00000001) ? TRUE : FALSE;
	IFXCHECKX(pSubdivisionModifier->SetEnable( bTemp ));
	bTemp = (uTemp & 0x00000002) ? TRUE : FALSE;
	IFXCHECKX(pSubdivisionModifier->SetAdaptive( bTemp ));

	// 2. Depth
	pBitStreamX->ReadU32X( uTemp );
	IFXCHECKX(pSubdivisionModifier->SetDepth( uTemp ));

	// 3. Tension
	F32  fTemp = 0.0f;
	pBitStreamX->ReadF32X( fTemp );
	IFXCHECKX(pSubdivisionModifier->SetTension( fTemp ));

	// 4. Error
	pBitStreamX->ReadF32X(fTemp );
	IFXCHECKX(pSubdivisionModifier->SetError( fTemp ));
}

IFXRESULT IFXAPI_CALLTYPE CIFXSubdivisionModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXSubdivisionModifierDecoder *pComponent = new CIFXSubdivisionModifierDecoder;

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
