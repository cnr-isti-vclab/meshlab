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

//
//	CIFXDummyModifierDecoder.cpp
//

#include "IFXCoreCIDs.h"
#include "CIFXDummyModifierDecoder.h"
#include "IFXCheckX.h"
#include "IFXBlockTypes.h"

CIFXDummyModifierDecoder::CIFXDummyModifierDecoder()
{
}

CIFXDummyModifierDecoder::~CIFXDummyModifierDecoder()
{
}

U32 CIFXDummyModifierDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXDummyModifierDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXDummyModifierDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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
void CIFXDummyModifierDecoder::InitializeX(const IFXLoadConfig &lc)
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXDummyModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	U32 uBlockType;
	rDataBlockX.GetBlockTypeX(uBlockType);
	if( uBlockType != BlockType_FileNewObjectTypeU3D )
		CreateObjectX(rDataBlockX, CID_IFXDummyModifier);
}

void CIFXDummyModifierDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pObject) 
	{
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	IFXDECLARELOCAL(IFXDummyModifier,pDummyModifier);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXDummyModifier, (void**)&pDummyModifier ));

	IFXCHECKX( pDummyModifier->SetDataBlock(m_pDataBlockQueueX) );

	rWarningPartialTransfer = IFX_OK;
}

IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXDummyModifierDecoder *pComponent = new CIFXDummyModifierDecoder;

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
