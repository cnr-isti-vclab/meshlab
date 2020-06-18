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
	@file	CIFXViewNodeEncoder.cpp

		Implementation of the CIFXViewNodeEncoder.
		The CIFXViewNodeEncoder contains view node encoding functionality
		that is used by the write manager.
*/


#include "CIFXViewNodeEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXView.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"


// constructor
CIFXViewNodeEncoder::CIFXViewNodeEncoder()
{
	m_bInitialized = FALSE;
	m_uRefCount = 0;
}

// destructor
CIFXViewNodeEncoder::~CIFXViewNodeEncoder()
{
}

// IFXUnknown
U32 CIFXViewNodeEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXViewNodeEncoder::Release()
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

IFXRESULT CIFXViewNodeEncoder::QueryInterface( IFXREFIID  interfaceId,
											  void**   ppInterface )
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
void CIFXViewNodeEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*  pDataBlock = NULL;
	IFXPalette*     pTexPalette = NULL;
	IFXView*    pView = NULL;
	U32       attributes = 0;

	try
	{
		// The view node block has the following sections:
		//
		// Node data
		//
		// 1.1. View node name (IFXString)
		// 1.2. Parent node name (IFXString)
		// 1.3. UserPropertyList (IFXString)
		// 2. Local matrix (16*F32)
		//
		// View specific data
		//
		// 3. View resource name (IFXString)
		//
		// 4. Attributes (U32)
		//
		// 5. View Clip Planes & Projection
		//    5.1 View Near Clip (F32)
		//    5.2 View Far Clip (F32)
		//    5.3 View Projection (F32)
		//      or
		//    5.3 Ortho height (F32)
		//
		// 6. Viewport
		//    6.1 Size (F32 * 2)
		//    6.2 Position (F32 * 2)
		//
		//
		// 7. Number of backdrops (U32)
		// For Each Backdrop:
		//    7.0 Texture Name (IFXString)
		//    7.1 Texture blend (F32)
		//    7.2 Backdrop rotation (F32)
		//    7.3, 7.4 Backdrop location (F32 * 2)
		//    7.5, 7.6 Backdrop regpoint (I32 * 2)
		//    7.7, 7.8 Backdrop scale (F32 * 2)
		//
		// 7. Number of overlays (U32)
		// For Each Overlay:
		//    7.0 Texture Name (IFXString)
		//    7.1 Texture blend (F32)
		//    7.2 Overlay rotation (F32)
		//    7.3, 7.4 Overlay location (F32 * 2)
		//    7.5, 7.6 Overlay regpoint (I32 * 2)
		//    7.7, 7.8 Overlay scale (F32 * 2)
		//

		// use node base class to encode shared node data (i.e. data that all node
		// types possess)

		// check for initialization
		if ( FALSE == CIFXViewNodeEncoder::m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		if( units <= 0.0f )
			throw IFXException( IFX_E_INVALID_RANGE );

		// get palettes
		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexPalette ));

		// get the view (node) interface of the node
		if ( NULL == m_pNode )
			throw IFXException( IFX_E_CANNOT_FIND );

		// 1. View node name (IFXString)
		// 2. Parent node name (IFXString)
		// 3. UserPropertyList (IFXString)
		// 4. Local matrix (16*F32)
		CIFXNodeBaseEncoder::CommonNodeEncodeU3D( rName, units );

		IFXCHECKX( m_pNode->QueryInterface( IID_IFXView, (void**)&pView ) );

		if ( NULL == pView )
			throw IFXException( IFX_E_UNSUPPORTED );

		// get resource name
		IFXString sName;

		U32 uViewResourceIndex = 0;
		pView->GetViewResourceID( &uViewResourceIndex );
		IFXDECLARELOCAL( IFXPalette, pViewResourcePalette );

		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pViewResourcePalette ) );

		if ( uViewResourceIndex != (U32)(-1) )
		{
			IFXCHECKX( pViewResourcePalette->GetName( uViewResourceIndex,
				&sName) );
		}
		else
		{
			IFXCHECKX( sName.Assign(L"") ); // make it a null string
		}

		// 3. Resource name (IFXString)
		m_pBitStream->WriteIFXStringX( sName );

		// 4. Attributes (U32)
		/*
		Attributes:
		0x00000000: default values (3-point perspective projection and screen position units in screen pixels)
		0x00000001: screen position units: percentage of screen dimension
		0x00000002: projection mode: ortho
		0x00000004: projection mode: 2-point perspective
		0x00000008: projection mode: 1-point perspective
		*/
		attributes = pView->GetAttributes();
		m_pBitStream->WriteU32X( attributes );

		// 5. View Clip Planes & Projection
		//    5.1 View Near Clip (F32)
		m_pBitStream->WriteF32X(pView->GetNearClip()/(F32)units);

		//    5.2 View Far Clip (F32)
		F32 fTemp = pView->GetFarClip();
		if( FLT_MAX * (F32)units < fTemp )
			fTemp = FLT_MAX * (F32)units;
		m_pBitStream->WriteF32X(fTemp/(F32)units);

		const U32 attributesMasked = attributes & ~IFX_PERCENTDIMEN; // mask screen position unit

		if( IFX_PERSPECTIVE3 == attributesMasked ) // 3-point perspective, default
		{
			IFXCHECKX(pView->GetProjection(&fTemp));
			m_pBitStream->WriteF32X(fTemp);
		}
		else if ( IFX_ORTHOGRAPHIC == attributesMasked ) // ortho
		{
			IFXCHECKX(pView->GetOrthoHeight(&fTemp));
			m_pBitStream->WriteF32X(fTemp);
		}
		else if( IFX_PERSPECTIVE2 == attributesMasked ) // 2-point perspective
		{
			IFXCHECKX(pView->GetProjection(&fTemp));
			// Since we don't have support for this type of projection
			// then just use old value and write it three times to
			// match the spec
			m_pBitStream->WriteF32X(fTemp);
			m_pBitStream->WriteF32X(fTemp);
			m_pBitStream->WriteF32X(fTemp);
		}
		else if( IFX_PERSPECTIVE1 == attributesMasked ) // 1-point perspective
		{
			IFXCHECKX(pView->GetProjection(&fTemp));
			// Since we don't have support for this type of projection
			// then just use old value and write it three times to
			// match the spec
			m_pBitStream->WriteF32X(fTemp);
			m_pBitStream->WriteF32X(fTemp);
			m_pBitStream->WriteF32X(fTemp);
		}

		// 6. Viewport
		IFXF32Rect rcViewport;
		IFXCHECKX(pView->GetViewport(rcViewport));

		//    6.1 Size (F32 * 2)
		m_pBitStream->WriteF32X(rcViewport.m_Width);
		m_pBitStream->WriteF32X(rcViewport.m_Height);

		//    6.2 Position (F32 * 2)
		m_pBitStream->WriteF32X(rcViewport.m_X);
		m_pBitStream->WriteF32X(rcViewport.m_Y);

		U32 uNumLayers = 0;
		// 7. Number of backdrops
		IFXCHECKX(pView->GetLayerCount(IFX_VIEW_BACKDROP, uNumLayers));
		m_pBitStream->WriteU32X(uNumLayers);

		// For Each Backdrop:
		U32 i;
		for(i = 0; i < uNumLayers; i++)
		{
			IFXString tempString;

			IFXViewLayer layer;
			IFXCHECKX(pView->GetLayer(IFX_VIEW_BACKDROP, i, layer));

			//    7.0 Texture Name (IFXString)
			IFXCHECKX( pTexPalette->GetName( layer.m_uTextureId, &tempString ) );
			m_pBitStream->WriteIFXStringX(tempString);

			//    7.1 Texture blend (F32)
			m_pBitStream->WriteF32X(layer.m_fBlend);

			//    7.2 Backdrop rotation (F32)
			m_pBitStream->WriteF32X(layer.m_fRotation);

			//    7.3, 7.4 Backdrop location (F32 * 2)
			m_pBitStream->WriteF32X((F32)layer.m_iLocX);
			m_pBitStream->WriteF32X((F32)layer.m_iLocY);

			//    7.5, 7.6 Backdrop regpoint (I32 * 2)
			m_pBitStream->WriteI32X(layer.m_iRegX);
			m_pBitStream->WriteI32X(layer.m_iRegY);

			//    7.7, 7.8 Backdrop scale (F32 * 2)
			m_pBitStream->WriteF32X(layer.m_vScale.X());
			m_pBitStream->WriteF32X(layer.m_vScale.Y());
		}

		// 7. Number of overlays
		IFXCHECKX(pView->GetLayerCount(IFX_VIEW_OVERLAY, uNumLayers));
		m_pBitStream->WriteU32X(uNumLayers);

		// For Each Overlay:
		for(i = 0; i < uNumLayers; i++)
		{
			IFXString tempString;

			IFXViewLayer layer;
			IFXCHECKX(pView->GetLayer(IFX_VIEW_OVERLAY, i, layer));

			//    7.0 Texture Name (IFXString)
			IFXCHECKX( pTexPalette->GetName( layer.m_uTextureId, &tempString ) );
			m_pBitStream->WriteIFXStringX(tempString);

			//    7.1 Texture blend (F32)
			m_pBitStream->WriteF32X(layer.m_fBlend);

			//    7.2 Backdrop rotation (F32)
			m_pBitStream->WriteF32X(layer.m_fRotation);

			//    7.3, 7.4 Backdrop location (F32 * 2)
			m_pBitStream->WriteF32X((F32)layer.m_iLocX);
			m_pBitStream->WriteF32X((F32)layer.m_iLocY);

			//    7.5, 7.6 Backdrop regpoint (I32 * 2)
			m_pBitStream->WriteI32X(layer.m_iRegX);
			m_pBitStream->WriteI32X(layer.m_iRegY);

			//    7.7, 7.8 Backdrop scale (F32 * 2)
			m_pBitStream->WriteF32X(layer.m_vScale.X());
			m_pBitStream->WriteF32X(layer.m_vScale.Y());
		}

		// Get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// Set the data block type
		pDataBlock->SetBlockTypeX( BlockType_NodeViewU3D );

		// Set the Priority on the Datablock
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pNode->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX( *pDataBlock );

		// clean up:
		IFXRELEASE( pTexPalette );
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pView );
	}
	catch ( ... )
	{
		// release IFXCOM objects
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pTexPalette );
		IFXRELEASE( pView );

		throw;
	}
}

void CIFXViewNodeEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{
		// initialize base class(es)
		CIFXNodeBaseEncoder::Initialize( rCoreServices );

		// initialize locally
		if ( TRUE == CIFXNodeBaseEncoder::m_bInitialized )
			CIFXViewNodeEncoder::m_bInitialized = TRUE;
		else
			CIFXViewNodeEncoder::m_bInitialized = FALSE;
	}
	catch ( ... )
	{
		throw;
	}
}

void CIFXViewNodeEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXNode* pNode = NULL;

	try
	{
		// get the IFXNode interface
		IFXCHECKX( rObject.QueryInterface( IID_IFXNode, (void**)&pNode ) );

		if ( NULL == pNode )
			throw IFXException( IFX_E_UNSUPPORTED );

		// set the node on the CIFXNodeBaseEncoder base class
		CIFXNodeBaseEncoder::SetNode( *pNode );

		// clean up
		IFXRELEASE( pNode );
	}
	catch ( ... )
	{
		IFXRELEASE( pNode );

		throw;
	}
}

// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXViewNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXViewNodeEncoder *pComponent = new CIFXViewNodeEncoder;

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
