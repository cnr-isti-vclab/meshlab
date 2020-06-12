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
	@file	CIFXTextureDecoder.cpp

			The CIFXTextureDecoder is used by the CIFXLoadManager to load
			texture resources into the scene graph. CIFXTextureDecoder
			exposes an IFXDecoderX interface to the CIFXLoadManager for this
			purpose.
*/

#include "IFXCoreCIDs.h"
#include "IFXSceneGraph.h"
#include "IFXBitStreamX.h"
#include "CIFXTextureDecoder.h"
#include "IFXCheckX.h"

CIFXTextureDecoder::CIFXTextureDecoder() :
	IFXDEFINEMEMBER(m_pDecoderX),
	IFXDEFINEMEMBER(m_pCoreServices)
{
	m_uRefCount	= 0;
	m_uLoadId = 0;
}

CIFXTextureDecoder::~CIFXTextureDecoder()
{
}

U32 CIFXTextureDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXTextureDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXTextureDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface)
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
void CIFXTextureDecoder::InitializeX(const IFXLoadConfig &lc)
{
	// Store the core services pointer
	lc.m_pCoreServices->AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = lc.m_pCoreServices;
	m_uLoadId = lc.m_loadId;
	m_bExternal = lc.m_external;
}

// Provide next block of data to the loader
void CIFXTextureDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if (NULL == m_pDecoderX) {
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);

		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXCHECKX(IFXCreateComponent( CID_IFXTextureObject, IID_IFXUnknown, (void**)&pObject ));
		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXCHECKX(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);
		pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		IFXString sBlockName;
		pBitStreamX->ReadIFXStringX( sBlockName );
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::TEXTURE, sBlockName));

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pSGPalette ));

		U32 uResourceID;
		IFXRESULT iResultPaletteFind = pSGPalette->Find( &sBlockName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) {
			IFXCHECKX(pSGPalette->Add( &sBlockName, &uResourceID ));
		}
		IFXCHECKX(pSGPalette->SetResourcePtr( uResourceID, pObject ));

		IFXRELEASE(m_pDecoderX);
		IFXCHECKX(pObject->QueryInterface(IID_IFXDecoderX,(void**)&m_pDecoderX));
	}
	m_pDecoderX->PutNextBlockX(rDataBlockX);
}

void CIFXTextureDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pDecoderX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDecoderX->TransferX(rWarningPartialTransfer);
}

IFXRESULT IFXAPI_CALLTYPE CIFXTextureDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXTextureDecoder *pComponent = new CIFXTextureDecoder;

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
