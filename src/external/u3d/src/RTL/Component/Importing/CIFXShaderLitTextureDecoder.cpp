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
	@file	CIFXShaderLitTextureDecoder2.cpp

			Implementation of the CIFXShaderLitTextureDecoder.
			The CIFXShaderLitTextureDecoder is used by the CIFXLoadManager to load
			shader resources into the scene graph's shader resource palette.
			CIFXMaterialLoader exposes the IFXDecoderX interface to the
			CIFXLoadManager for this purpose.
*/

#include "CIFXShaderLitTextureDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"

// Constructor
CIFXShaderLitTextureDecoder::CIFXShaderLitTextureDecoder() :
	IFXDEFINEMEMBER(m_pShaderLitTexture),
	IFXDEFINEMEMBER(m_pCoreServices),
	IFXDEFINEMEMBER(m_pDataBlockQueueX),
	IFXDEFINEMEMBER(m_pBitStreamX)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
	m_eShaderMode = IFX_SHADER_STANDARD;
}

// Destructor
CIFXShaderLitTextureDecoder::~CIFXShaderLitTextureDecoder()
{

}

// IFXUnknown
U32 CIFXShaderLitTextureDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXShaderLitTextureDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXShaderLitTextureDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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
void CIFXShaderLitTextureDecoder::InitializeX(const IFXLoadConfig &lc)
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
void CIFXShaderLitTextureDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pShaderLitTexture) {
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);

		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXCHECKX(IFXCreateComponent( CID_IFXShaderLitTexture, IID_IFXUnknown, (void**)&pObject ));
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
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::SHADER, sBlockName));

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::SHADER, &pSGPalette ));

		U32 uResourceID;
		IFXRESULT iResultPaletteFind = pSGPalette->Find( &sBlockName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) {
			IFXCHECKX(pSGPalette->Add( &sBlockName, &uResourceID ));
		}
		IFXCHECKX(pSGPalette->SetResourcePtr( uResourceID, pObject ));

		IFXRELEASE( m_pShaderLitTexture );
		IFXCHECKX(pObject->QueryInterface( IID_IFXShaderLitTexture, (void**)&m_pShaderLitTexture));
	}
}

void CIFXShaderLitTextureDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pShaderLitTexture) {
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
			case BlockType_ResourceLitTextureShaderU3D:
				DecodeShaderLitTextureU3DX( *pDataBlockX );
				break;
			default:
				IFXCHECKX(IFX_E_UNSUPPORTED);
				break;
			} // end switch (uBlockType)
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Private methods

void CIFXShaderLitTextureDecoder::DecodeShaderLitTextureU3DX(IFXDataBlockX &rDataBlockX)
{
	U32 id = 0;

	// Get the scene graph
	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph, (void**)&pSceneGraph));

	// Get the shader resource palette
	IFXDECLARELOCAL(IFXPalette,pShaderPalette);
	IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::SHADER, &pShaderPalette));

	// Get the material resource palette
	IFXDECLARELOCAL(IFXPalette,pMaterialResourcePalette);
	IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pMaterialResourcePalette));

	// Get the texture resource palette
	IFXDECLARELOCAL(IFXPalette,pTextureResourcePalette);
	IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTextureResourcePalette));

	// Create the bitstream and set up
	{
		IFXRELEASE(m_pBitStreamX);
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
		m_pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	// 1. Decode the shader name
	IFXString sName;
	m_pBitStreamX->ReadIFXStringX(sName);
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::SHADER, sName));

	U32 uVal = 0;
	F32 fVal = 0;

	// 2. Shader attributes U32
	m_pBitStreamX->ReadU32X( uVal );
	IFXCHECKX(m_pShaderLitTexture->SetLightingEnabled( uVal & 0x00000001 ));
	IFXCHECKX(m_pShaderLitTexture->SetAlphaTestEnabled( (uVal & 0x00000002) >> 1 ));
	IFXCHECKX(m_pShaderLitTexture->GetRenderMaterial().SetUseVertexColors( (uVal & 0x00000004) >> 2 ));

	// 3. Alpha Test Reference
	m_pBitStreamX->ReadF32X( fVal );
	IFXCHECKX(m_pShaderLitTexture->GetRenderBlend().SetReference( fVal ));

	// 4. Alpha Test Compare Function
	m_pBitStreamX->ReadU32X( uVal );
	IFXCHECKX(m_pShaderLitTexture->GetRenderBlend().SetTestFunc( (IFXenum)uVal ));

	// 5. Color Buffer Blend Function
	m_pBitStreamX->ReadU32X( uVal );
	IFXCHECKX(m_pShaderLitTexture->GetRenderBlend().SetBlendFunc( (IFXenum)uVal ));

	// 6. Shader Render Pass Enables
	m_pBitStreamX->ReadU32X( uVal );
	// Setting this value does not return an IFXRESULT
	m_pShaderLitTexture->SetRenderPassFlags( uVal );

	// 7. Decode the shader channels
	U32 uChannels = 0;
	m_pBitStreamX->ReadU32X( uChannels );
	IFXCHECKX(m_pShaderLitTexture->SetChannels( uChannels ));

	// 8. Alpha texture channels U32
	m_pBitStreamX->ReadU32X( uVal );
	IFXCHECKX(m_pShaderLitTexture->SetAlphaTextureChannels( uVal ));

	// 9. Decode the material name (optional)
	m_pBitStreamX->ReadIFXStringX(sName);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MATERIAL, sName));

	// 9.1 Look for a material with the same name in the material palette
	IFXRESULT iFindResult = pMaterialResourcePalette->Find(&sName, &id);
	if (IFXFAILURE(iFindResult)) {
		// If not found, add the material to the palette
		IFXCHECKX(pMaterialResourcePalette->Add(&sName, &id));
	}
	/* Should not need to create the material; adding to the palette is enough */

	// 9.2 Link to the material
	IFXCHECKX(m_pShaderLitTexture->SetMaterialID(id));

	// Texture layers:
	U8				u8Value	 = 0 ;
	IFXMatrix4x4	matrix ;

	U32 i;
	for ( i = 0; i < IFX_MAX_TEXUNITS; i++) {
		
		if (uChannels & (1 << i) ) {

			// Decode the texture name (optional)
			m_pBitStreamX->ReadIFXStringX(sName);
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::TEXTURE, sName));

			// Look for a texture with the same name in the texture palette
			IFXRESULT iFindResult = pTextureResourcePalette->Find(&sName, &id);
			if (IFXFAILURE(iFindResult)) {
				// Add the texture name to the palette if not found
				IFXCHECKX(pTextureResourcePalette->Add(&sName, &id));
			}
			// Should not need to create the texture object; adding to the palette is enough

			// Link to the texture
			IFXCHECKX(m_pShaderLitTexture->SetTextureID( i, id ));

			// Decode the texture intensity
			m_pBitStreamX->ReadF32X( fVal );
			IFXCHECKX(m_pShaderLitTexture->SetTextureIntensity( i, fVal ));

			// Decode the texture blend function
			m_pBitStreamX->ReadU8X(u8Value);
			IFXCHECKX(m_pShaderLitTexture->SetBlendFunction( i, (IFXShaderLitTexture::BlendFunction)u8Value ));

			// Decode the texture blend source
			m_pBitStreamX->ReadU8X(u8Value);
			IFXCHECKX(m_pShaderLitTexture->SetBlendSource( i, (IFXShaderLitTexture::BlendSource)u8Value ));

			// Decode the texture blend constant
			m_pBitStreamX->ReadF32X(fVal);
			IFXCHECKX(m_pShaderLitTexture->SetBlendConstant( i, fVal ));

			// Decode the texture texture mode
			m_pBitStreamX->ReadU8X(u8Value);
			IFXCHECKX(m_pShaderLitTexture->SetTextureMode( i, (IFXShaderLitTexture::TextureMode)u8Value ));

			// Decode the texture texture transform
			F32* pElement = matrix.Raw();

			U32 ii;
			for ( ii = 0; ii < 16 ; ii++)
				m_pBitStreamX->ReadF32X(pElement[ii]);

			IFXCHECKX(m_pShaderLitTexture->SetTextureTransform(i, &matrix));

			// Decode the texture wrap transform
			pElement = matrix.Raw();

			for ( ii = 0; ii < 16 ; ii++)
				m_pBitStreamX->ReadF32X(pElement[ii]);

			IFXCHECKX(m_pShaderLitTexture->SetWrapTransform(i, &matrix));

			// Decode the texture repeat value
			m_pBitStreamX->ReadU8X(u8Value);
			IFXCHECKX(m_pShaderLitTexture->SetTextureRepeat( i, u8Value ));
		}
	}
}

IFXRESULT IFXAPI_CALLTYPE CIFXShaderLitTextureDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXLoadManager component.
		CIFXShaderLitTextureDecoder *pComponent = new CIFXShaderLitTextureDecoder;

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
