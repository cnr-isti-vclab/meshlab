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
	@file	CIFXLightResourceDecoder.cpp

			Implementation of the CIFXLightResourceDecoder.
			The CIFXLightResourceDecoder is used by the CIFXLoadManager to load
			light nodes into the scenegraph. CIFXLightResourceDecoder exposes the
			IFXDecoderX interface to the	CIFXLoadManager for this purpose.
*/

#include "IFXCoreCIDs.h"
#include "CIFXLightResourceDecoder.h"
#include "IFXLight.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"

// Constructor
CIFXLightResourceDecoder::CIFXLightResourceDecoder()
{
	m_pLightRes = NULL;
}

// Destructor
CIFXLightResourceDecoder::~CIFXLightResourceDecoder()
{
	IFXRELEASE( m_pLightRes );
}

// IFXUnknown
U32 CIFXLightResourceDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXLightResourceDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXLightResourceDecoder::QueryInterface( IFXREFIID	interfaceId,
										    void**		ppInterface )
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

// IFXDecoderX
void CIFXLightResourceDecoder::InitializeX(const IFXLoadConfig &lc)
{
	CIFXNodeBaseDecoder::InitializeX(lc);
}

void CIFXLightResourceDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	// create resource

	// Get name
	IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
	pBitStreamX->SetDataBlockX( rDataBlockX );

	IFXString sName;
	pBitStreamX->ReadIFXStringX( sName );

	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::LIGHT, sName));

	// Get the scene graph
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
	IFXCHECKX( m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSceneGraph) );

	// Get the light resource palette
	IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
	IFXCHECKX( pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette) );

	// 1.1 Look for a light with the same name in the light resource palette
	U32 id=0;
	IFXRESULT rc = pLightResourcePalette->Find(sName, &id);
	if(IFXFAILURE(rc)) 
	{
		// Add the name if it was not found
		IFXCHECKX( pLightResourcePalette->Add(sName, &id) );
	}

	// 1.1.1a If the light exists, Use it
	rc = pLightResourcePalette->GetResourcePtr(id, IID_IFXLightResource, (void**)&m_pLightRes);

	if (IFXFAILURE(rc)) // 1.1.1b Else, create it, and add it to the palette.
	{
		IFXCHECKX( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&m_pLightRes ) );
		IFXDECLARELOCAL( IFXUnknown, pUnk );
		IFXCHECKX( m_pLightRes->QueryInterface(IID_IFXUnknown, (void**)&pUnk) );

		IFXDECLARELOCAL( IFXMarker, pMarker );
		IFXCHECKX(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);
		pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

		IFXCHECKX( pLightResourcePalette->SetResourcePtr(id, pUnk) );
		IFXRELEASE( pUnk );
	}
}

void CIFXLightResourceDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if ( NULL == m_pCoreServices || m_pLightRes == NULL )
		IFXCHECKX(IFX_E_NOT_INITIALIZED);

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) 
	{
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX ) 
		{
			U32 uBlockType = 0;
			pDataBlockX->GetBlockTypeX( uBlockType );

			if( uBlockType == BlockType_ResourceLightU3D ) 
			{
				IFXRELEASE(m_pBitStreamX);
				IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
				m_pBitStreamX->SetDataBlockX( *pDataBlockX );

				IFXString sName;
				U8 u8Value;
				U32 uValue;
				m_pBitStreamX->ReadIFXStringX( sName );

				// U32 LightAttributes
				m_pBitStreamX->ReadU32X(uValue);
				m_pLightRes->SetAttributes((U8)uValue);

				// U8 LightType
				m_pBitStreamX->ReadU8X(u8Value);
				m_pLightRes->SetType((IFXLightResource::LightType)(u8Value));

				// Color (3*F32)
				IFXVector4 vColor;
				m_pBitStreamX->ReadF32X(vColor.R());
				m_pBitStreamX->ReadF32X(vColor.G());
				m_pBitStreamX->ReadF32X(vColor.B());
				m_pBitStreamX->ReadF32X(vColor.A());
				m_pLightRes->SetColor(vColor);

				// (F32*3) attenuation
				F32 fAttenuation[3];
				m_pBitStreamX->ReadF32X(fAttenuation[0]);
				m_pBitStreamX->ReadF32X(fAttenuation[1]);
				m_pBitStreamX->ReadF32X(fAttenuation[2]);
				IFXCHECKX(m_pLightRes->SetAttenuation(fAttenuation));

				// F32 Spot angle
				F32 fSpotAngle;
				m_pBitStreamX->ReadF32X(fSpotAngle);
				m_pLightRes->SetSpotAngle(fSpotAngle);

				// Intensity
				F32 fIntensity = 1.0f;
				m_pBitStreamX->ReadF32X(fIntensity);
				IFXCHECKX(m_pLightRes->SetIntensity(fIntensity));
			}
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXLightResourceDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXLightResourceDecoder *pComponent = new CIFXLightResourceDecoder;

		if ( pComponent ) {
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}
