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
//
//	CIFXShaderLitTextureEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXShaderLitTextureEncoder.
//		The CIFXShaderLitTextureEncoder contains CIFXShaderLitTexture encoding 
//		functionality that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXShaderLitTextureEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXSceneGraph.h"
#include "IFXShaderLitTexture.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"


// constructor
CIFXShaderLitTextureEncoder::CIFXShaderLitTextureEncoder()
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pNextEncoder = NULL;
	m_pObject = NULL;
	m_uEncoderIndex = 0;
	m_uRefCount = 0;
}

// destructor
CIFXShaderLitTextureEncoder::~CIFXShaderLitTextureEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pNextEncoder );
	IFXRELEASE( m_pObject );
}



// IFXUnknown
U32 CIFXShaderLitTextureEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXShaderLitTextureEncoder::Release()
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

IFXRESULT CIFXShaderLitTextureEncoder::QueryInterface( IFXREFIID	interfaceId, 
													   void**		ppInterface )
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
void CIFXShaderLitTextureEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*			pDataBlock = NULL;
	IFXPalette*				pMaterialPalette = NULL;
	IFXSceneGraph*			pSceneGraph = NULL;
	IFXShaderLitTexture*	pShaderLitTexture = NULL;
	IFXPalette*				pTexturePalette = NULL;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXShaderLitTexture, (void**)&pShaderLitTexture ) );
		IFXCHECKX( m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ) );

		// 1. Write the shader name
		m_pBitStream->WriteIFXStringX( rName );

		U32 uFlags = 0;
		if (pShaderLitTexture->GetLightingEnabled()) uFlags |= 0x00000001;
		if (pShaderLitTexture->GetAlphaTestEnabled()) uFlags |= 0x00000002;
		if (pShaderLitTexture->GetRenderMaterial().GetUseVertexColors()) uFlags |= 0x00000004;
		// 2. Write lighting enabled flag
		m_pBitStream->WriteU32X( uFlags );

		// 3. Write Alpha Test Reference
		m_pBitStream->WriteF32X( pShaderLitTexture->GetRenderBlend().GetReference() );

		// 4. Write Alpha Compare Function
		m_pBitStream->WriteU32X( pShaderLitTexture->GetRenderBlend().GetTestFunc() );

		// 5. Write Color Buffer Blend Function
		m_pBitStream->WriteU32X( pShaderLitTexture->GetRenderBlend().GetBlendFunc() );

		// 6. Shader Render Pass enables
		m_pBitStream->WriteU32X( pShaderLitTexture->GetRenderPassFlags() );

		// 7. Write the shader channels
		U32 uChannels = pShaderLitTexture->GetChannels();
		m_pBitStream->WriteU32X( uChannels );

		// 8. Alpha texture channels:
		U32 uAlphaChannels = pShaderLitTexture->GetAlphaTextureChannels();
		m_pBitStream->WriteU32X( uAlphaChannels );

		// 9. Write the material name (optional)
		IFXString sName;

		IFXCHECKX( pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pMaterialPalette ) );
		IFXCHECKX( pMaterialPalette->GetName( pShaderLitTexture->GetMaterialID(), &sName ) );
		m_pBitStream->WriteIFXStringX( sName );

		IFXCHECKX( pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette) );

		// Write out the texture layers
		F32 f32Value			= 0.0;
		IFXMatrix4x4* pMatrix	= NULL;

		U32 i;
		for ( i = 0; i < IFX_MAX_TEXUNITS; i++)
		{
			if (uChannels & (1 << i) )
			{
				// Texture name IFXCHAR
				U32 id = 0;
				IFXCHECKX( pShaderLitTexture->GetTextureID( i, &id ) );
				IFXCHECKX( pTexturePalette->GetName( id, &sName ) );
				m_pBitStream->WriteIFXStringX(sName);

				// Texture intensity F32
				F32 fIntensity = 1;
				IFXCHECKX( pShaderLitTexture->GetTextureIntensity( i, &fIntensity ) );
				m_pBitStream->WriteF32X( fIntensity );

				//---------------------------------------------------
				// Write out the blending parameters for all layers:
				//---------------------------------------------------

				// Get the blend function
				IFXShaderLitTexture::BlendFunction blendFunction;

				IFXCHECKX( pShaderLitTexture->GetBlendFunction( i, &blendFunction ) );
				// Write the texture blend function
				m_pBitStream->WriteU8X((U8)blendFunction);

				// Get the blend source
				IFXShaderLitTexture::BlendSource textureBlendSource;

				IFXCHECKX( pShaderLitTexture->GetBlendSource( i, &textureBlendSource ) );
				// Write the texture blend source
				m_pBitStream->WriteU8X((U8)textureBlendSource);
				
				// Get the blend constant
				IFXCHECKX( pShaderLitTexture->GetBlendConstant( i, &f32Value ) );
				//Write the texture blend constant
				m_pBitStream->WriteF32X(f32Value);

				// Get the texture mode
				IFXShaderLitTexture::TextureMode textureMode;

				IFXCHECKX( pShaderLitTexture->GetTextureMode( i, &textureMode ) );
				// Write the texture texture mode
				m_pBitStream->WriteU8X((U8)textureMode);

				// Write the texture texture transform
				IFXCHECKX( pShaderLitTexture->GetTextureTransform(i, &pMatrix) );

				F32* pElement = pMatrix->Raw();
				U32 ii;
				for ( ii = 0; ii < 16; ii++) 
					m_pBitStream->WriteF32X(*pElement++);
		
				// Write the texture wrap transform
				IFXCHECKX( pShaderLitTexture->GetWrapTransform(i, &pMatrix) );
				
				pElement = pMatrix->Raw();
				for ( ii = 0; ii < 16; ii++) 
					m_pBitStream->WriteF32X(*pElement++);

				// Get the texture repeat value
				U8 textureRepeat;
				IFXCHECKX( pShaderLitTexture->GetTextureRepeat( i, &textureRepeat ) );
				// Write the texture repeat boolean
				m_pBitStream->WriteU8X(textureRepeat);

			} // end if ( IFXSUCCESS(rc) && (uChannels & (1<<i)) )
		} //  end for (i=0; i < IFXRenderLayer::IRL_TEXTURE_NUM_UNITS; i++)

		// Get a data block from the bitstream
		m_pBitStream->GetDataBlockX( pDataBlock );

		// Set the data block type
		pDataBlock->SetBlockTypeX( BlockType_ResourceLitTextureShaderU3D );

		// Set the data block priority
		pDataBlock->SetPriorityX( 0 );

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX( *pDataBlock );

		// clean up
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pMaterialPalette );
		IFXRELEASE( pSceneGraph );
		IFXRELEASE( pShaderLitTexture );
		IFXRELEASE( pTexturePalette );
	}
	catch ( ... )
	{
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pMaterialPalette );
		IFXRELEASE( pSceneGraph );
		IFXRELEASE( pShaderLitTexture );
		IFXRELEASE( pTexturePalette );

		throw;
	}
}

void CIFXShaderLitTextureEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// latch onto the core services object passed in
		IFXRELEASE( m_pCoreServices )
		m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();
		
		// create a bitstream
		IFXRELEASE( m_pBitStream );
		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX,
									   (void**)&m_pBitStream ) );

		m_bInitialized = TRUE;
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		throw;
	}
}

void CIFXShaderLitTextureEncoder::SetObjectX( IFXUnknown& rObject )
{
	try
	{
		// set the object
		IFXRELEASE( m_pObject );		
		m_pObject = &rObject;
		m_pObject->AddRef();
	}
	catch ( ... )
	{
		throw;
	}
}



// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXShaderLitTextureEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXShaderLitTextureEncoder *pComponent = new CIFXShaderLitTextureEncoder;

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
