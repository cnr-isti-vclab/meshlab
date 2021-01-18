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
//	CIFXShadingModifierDecoder.cpp
//

#include "IFXCoreCIDs.h"
#include "IFXShadingModifier.h"
#include "CIFXShadingModifierDecoder.h"
#include "IFXCheckX.h"
#include "IFXBitStreamX.h"

CIFXShadingModifierDecoder::CIFXShadingModifierDecoder()
{
}

CIFXShadingModifierDecoder::~CIFXShadingModifierDecoder()
{
}

U32 CIFXShadingModifierDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXShadingModifierDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXShadingModifierDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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
void CIFXShadingModifierDecoder::InitializeX(const IFXLoadConfig &lc)
{
	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the decoder
void CIFXShadingModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXShadingModifier);
}

void CIFXShadingModifierDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
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
			ProcessShadingModifierBlockX(*pDataBlockX);
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXShadingModifierDecoder::ProcessShadingModifierBlockX( IFXDataBlockX &rDataBlockX )
{
	// get the required interfaces of the object
	IFXDECLARELOCAL(IFXShadingModifier,pShadingModifier);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXShadingModifier, (void**)&pShadingModifier ));

	// The following elements are common to all Modifier blocks 
	// and are decoded in DecodeCommonModifierElementsX():
		// 1. ModelGenerator Name (IFXString)
		// 2. Index for modifier chain (U32)

	// Shading Modifiers have the following parameters:
		// 1. Shading Attributes (U32)
		// 2. Shader List Count (U32)
		//		2.1 Shader Count (U32)
		//			2.1.1 Shader Name (IFXString)

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

	U32 uShadingAttributes;
	U32 uShaderCnt;
	U32 uShaderListCnt;
	IFXString sShaderName;
	IFXDECLARELOCAL( IFXPalette, pShaderPalette );
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
	U32 uShaderID;

	IFXCHECKX( m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void **)&pSceneGraph ) );
	IFXCHECKX( pSceneGraph->GetPalette( IFXSceneGraph::SHADER, &pShaderPalette ) );

	// read shading attributes
	pBitStreamX->ReadU32X( uShadingAttributes );
	IFXCHECKX( pShadingModifier->SetAttributes( uShadingAttributes ) );

	// read shading lists count
	pBitStreamX->ReadU32X( uShaderListCnt );

	U32 i;
	for(  i = 0; i < uShaderListCnt; i++ )
	{
		IFXDECLARELOCAL( IFXShaderList, pShaderList );
		IFXCHECKX( IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList) );

		// read shaders count
		pBitStreamX->ReadU32X( uShaderCnt );
		IFXCHECKX( pShaderList->Allocate( uShaderCnt ) );

		U32 j;
		for(  j = 0; j < uShaderCnt; j++ )
		{
			// read shader name
			pBitStreamX->ReadIFXStringX( sShaderName );
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::SHADER, sShaderName));
			IFXRESULT rc = pShaderPalette->Find( sShaderName, &uShaderID );

			// If the shading resource was not found
			if (IFXFAILURE(rc))	
			{
				IFXUnknown* pUnk = NULL;
				// create a shading resource and add the shading resource to the palette
				rc = pShaderPalette->Add( &sShaderName, &uShaderID );
				if (IFXSUCCESS(rc)) {
					IFXCHECKX( IFXCreateComponent( CID_IFXShaderLitTexture, IID_IFXUnknown, (void**)&pUnk) );
					IFXDECLARELOCAL( IFXMarker, pMarker );
					IFXCHECKX(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
					IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
					IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
					IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
					IFXCHECKX( pShaderPalette->SetResourcePtr( uShaderID, pUnk ) );
					pMarker->SetExternalFlag(m_bExternal);
					pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);
				}
				IFXRELEASE(pUnk);
			}

			IFXCHECKX( pShaderList->SetShader( j, uShaderID ) );
		}

		IFXCHECKX( pShadingModifier->SetElementShaderList( i, pShaderList ) );
	}
}

IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXShadingModifierDecoder *pComponent = new CIFXShadingModifierDecoder;

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
