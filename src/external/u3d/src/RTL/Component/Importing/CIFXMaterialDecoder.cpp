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
	@file	CIFXMaterialDecoder.cpp

			Implementation of the CIFXMaterialDecoder.
			The CIFXMaterialDecoder is used by the CIFXLoadManager to load
			material resources into the scene graph's material resource palette.
			CIFXMaterialDecoder exposes the IFXDecoder interface to the
			CIFXLoadManager for this purpose.
*/

#include "CIFXMaterialDecoder.h"
#include "IFXBitStreamX.h"
#include "IFXCoreCIDs.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"

// Constructor
CIFXMaterialDecoder::CIFXMaterialDecoder() :
IFXDEFINEMEMBER(m_pMaterialResource),
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pDataBlockQueueX)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
}

// Destructor
CIFXMaterialDecoder::~CIFXMaterialDecoder()
{
}

// IFXUnknown
U32 CIFXMaterialDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXMaterialDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXMaterialDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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

// Initialize and get a reference to the core services object
void CIFXMaterialDecoder::InitializeX(const IFXLoadConfig &lc)
{
	// Initialize the data block queue
	IFXRELEASE(m_pDataBlockQueueX);
	IFXCHECKX(IFXCreateComponent( CID_IFXDataBlockQueueX, IID_IFXDataBlockQueueX, (void**)&m_pDataBlockQueueX ));

	// Store the core services pointer
	lc.m_pCoreServices->AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = lc.m_pCoreServices;
	m_uLoadId = lc.m_loadId;
	m_bExternal = lc.m_external;
}

// Provide next block of data to the loader
void CIFXMaterialDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pMaterialResource) {
		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXCHECKX(IFXCreateComponent( CID_IFXMaterialResource, IID_IFXUnknown, (void**)&pObject ));
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
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MATERIAL, sBlockName));

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pSGPalette ));

		U32 uResourceID;
		IFXRESULT resultPaletteFind = pSGPalette->Find( &sBlockName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == resultPaletteFind  ) {
			IFXCHECKX(pSGPalette->Add( &sBlockName, &uResourceID ));
		}
		IFXCHECKX(pSGPalette->SetResourcePtr( uResourceID, pObject ));

		IFXRELEASE( m_pMaterialResource );
		IFXCHECKX(pObject->QueryInterface( IID_IFXMaterialResource, (void**)&m_pMaterialResource));
	}
}

void CIFXMaterialDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pMaterialResource) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) {

			// Determine the block type
			U32 uBlockType = 0;
			pDataBlockX->GetBlockTypeX( uBlockType );

			// Process the data block
			switch ( uBlockType ) {
	  case BlockType_ResourceMaterialU3D:
		  ProcessMaterialBlockX( *pDataBlockX );
		  break;
	  default:
		  break; /// @todo: consider generating error condition here...
			} // end switch (uBlockType)

		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Update the material resource palette
void CIFXMaterialDecoder::ProcessMaterialBlockX(IFXDataBlockX &rDataBlockX)
{
	// Process the material resource data block

	// The material resource block has the following sections:
	// 1.  MaterialName (string)
	// 2.  MaterialAttributes (U32)
	// 3.  Ambient color (3*F32)
	// 4.  Diffuse color (3*F32)
	// 5.  Specular color (3*F32)
	// 6.  Emissive color (3*F32)
	// 7.  Reflectivity (F32)
	// 8.  Opacity (F32)

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pMaterialResource->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	// Create a bitstream component and initialize it to decode the block
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	{
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	U32 uBlockType = 0;
	rDataBlockX.GetBlockTypeX(uBlockType);

	// 1. Decode the material name (Element #1)
	IFXString stringMaterialName;
	pBitStreamX->ReadIFXStringX(stringMaterialName);

	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MATERIAL, stringMaterialName));

	// 2.  MaterialAttributes (U32)
	U32 uAttributes = 0;
	pBitStreamX->ReadU32X(uAttributes);
	uAttributes &= 0x0000003F; // Not using Attributes enum since it could get out of sync with the spec.
	m_pMaterialResource->SetAttributes(uAttributes);

	// Setup the color variable that will be used for decoding the
	// subsequent color fields.  This will ensure that there are no garbage
	// values being stored in the material resource (such as the alpha
	// channel which should be 1.0).
	IFXVector4 vColor( 0.0f, 0.0f, 0.0f, 1.0f );

	// 3.  Ambient color (3*F32)
	pBitStreamX->ReadF32X(vColor.X());
	pBitStreamX->ReadF32X(vColor.Y());
	pBitStreamX->ReadF32X(vColor.Z());
	IFXCHECKX(m_pMaterialResource->SetAmbient(vColor));

	// 4.  Diffuse color (3*F32)
	pBitStreamX->ReadF32X(vColor.X());
	pBitStreamX->ReadF32X(vColor.Y());
	pBitStreamX->ReadF32X(vColor.Z());
	IFXCHECKX(m_pMaterialResource->SetDiffuse(vColor));

	// 5.  Specular color (3*F32)
	pBitStreamX->ReadF32X(vColor.X());
	pBitStreamX->ReadF32X(vColor.Y());
	pBitStreamX->ReadF32X(vColor.Z());
	IFXCHECKX(m_pMaterialResource->SetSpecular(vColor));

	// 6.  Emissive color (3*F32)
	pBitStreamX->ReadF32X(vColor.X());
	pBitStreamX->ReadF32X(vColor.Y());
	pBitStreamX->ReadF32X(vColor.Z());
	IFXCHECKX(m_pMaterialResource->SetEmission(vColor));

	// 7.  Reflectivity (F32)
	F32 fReflectivity;
	pBitStreamX->ReadF32X(fReflectivity);
	IFXCHECKX(m_pMaterialResource->SetReflectivity(fReflectivity));

	// 8.  Opacity (F32)
	F32 fOpacity;
	pBitStreamX->ReadF32X(fOpacity);
	IFXCHECKX(m_pMaterialResource->SetOpacity(fOpacity));
}

IFXRESULT IFXAPI_CALLTYPE CIFXMaterialDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXMaterialDecoder *pComponent = new CIFXMaterialDecoder;

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
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}
