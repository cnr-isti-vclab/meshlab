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
	@file	CIFXViewDecoder.cpp

			Implementation of the CIFXViewDecoder.
			The CIFXViewDecoder is used by the CIFXLoadManager to load
			view nodes into the scenegraph. CIFXViewDecoder exposes the
			IFXDecoderX interface to the CIFXLoadManager for this purpose.
*/

#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXView.h"
#include "CIFXViewDecoder.h"
#include "IFXCheckX.h"

// Constructor
CIFXViewDecoder::CIFXViewDecoder()
{
	m_units = 1.0f;
}

// Destructor
CIFXViewDecoder::~CIFXViewDecoder()
{
}

// IFXUnknown
U32 CIFXViewDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXViewDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXViewDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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

// IFXDecoderX

void CIFXViewDecoder::LoadViewNodeBlockU3DX(IFXDataBlockX &rDataBlockX)
{
	U32   attributes = 0 ;
	F32   fTemp   = 0.0 ;
	U32 i;

	{
		// CommonNodeRead() takes care of:
		// 1.1 View node name (IFXString)
		// 1.2 Parent node name (IFXString)
		// 1.3. UserPropertyList (IFXString)
		// 2. Local matrix (16*F32)
		if (CommonNodeReadU3DX(rDataBlockX) == FALSE) return;

		if( m_unitScale > 0.0f && m_unitScale != 1.0f )
		{
			IFXMatrix4x4 matrix;

			for( i = 0; i < m_pNode->GetNumberOfParents(); ++i )
			{
				matrix = m_pNode->GetMatrix(i);

				// Apply units scaling factor to TM's translation.
				matrix[12] *= (F32)m_unitScale;
				matrix[13] *= (F32)m_unitScale;
				matrix[14] *= (F32)m_unitScale;

				IFXCHECKX(m_pNode->SetMatrix(i, &matrix));
			}
		}

		IFXDECLARELOCAL(IFXView,pViewNode);
		IFXCHECKX(m_pNode->QueryInterface(IID_IFXView,(void**)&pViewNode));

		IFXDECLARELOCAL(IFXPalette,pTexPalette);
		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexPalette ));

		// 3. View resource name (IFXString)
		IFXString sResName;
		m_pBitStreamX->ReadIFXStringX( sResName );

		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::VIEW, sResName));

		// 4. Attributes (U32)
		m_pBitStreamX->ReadU32X( attributes );
		pViewNode->SetAttributes( attributes ); // this will set projection mode

		// 5. View Clip Planes & Projection
		//    5.1 View Near Clip (F32)
		m_pBitStreamX->ReadF32X( fTemp );
		IFXCHECKX(pViewNode->SetNearClip( fTemp*(F32)m_units ));

		//    5.2 View Far Clip (F32)
		m_pBitStreamX->ReadF32X( fTemp );
		if( FLT_MAX / (F32)m_units < fTemp )
			fTemp = FLT_MAX / (F32)m_units;
		IFXCHECKX(pViewNode->SetFarClip( fTemp*(F32)m_units ));

		const U32 attributesMasked = attributes & ~IFX_PERCENTDIMEN; // mask screen position unit

		m_pBitStreamX->ReadF32X( fTemp );

		if( IFX_PERSPECTIVE3 == attributesMasked ) // 3-point perspective
		{
			pViewNode->SetProjection( fTemp );
		}
		else if ( IFX_ORTHOGRAPHIC == attributesMasked ) // ortho
		{
			pViewNode->SetOrthoHeight( fTemp );
		}
		else if( IFX_PERSPECTIVE2 == attributesMasked ) // 2-point perspective
		{
			// Since we don't have support for this type of projection
			// then just use old value and read it three times to
			// match the spec
			m_pBitStreamX->ReadF32X( fTemp );
			m_pBitStreamX->ReadF32X( fTemp );
			pViewNode->SetProjection( fTemp );
		}
		else if( IFX_PERSPECTIVE1 == attributesMasked ) // 1-point perspective
		{
			// Since we don't have support for this type of projection
			// then just use old value and read it three times to
			// match the spec
			m_pBitStreamX->ReadF32X( fTemp );
			m_pBitStreamX->ReadF32X( fTemp );
			pViewNode->SetProjection( fTemp );
		}

		// 6. Viewport
		{
			IFXF32Rect rcView;

			//    6.1 Size (F32 * 2)
			m_pBitStreamX->ReadF32X( rcView.m_Width );
			m_pBitStreamX->ReadF32X( rcView.m_Height );

			//    6.2 Position (F32 * 2)
			m_pBitStreamX->ReadF32X( rcView.m_X );
			m_pBitStreamX->ReadF32X( rcView.m_Y );

			IFXCHECKX(pViewNode->SetViewport( rcView ));
		}

		// 7. Number of backdrops
		U32 uNumChannels = 0;
		m_pBitStreamX->ReadU32X( uNumChannels );

		for(i = 0; i < uNumChannels; i++) 
		{
			IFXViewLayer layer;
			IFXString sTexName;
			I32 iTemp = 0;

			// For Each Backdrop:
			//    7.0 Texture Name (IFXString)
			m_pBitStreamX->ReadIFXStringX( sTexName );
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::TEXTURE, sTexName));

			// Look for a texture with the same name in the texture palette
			IFXRESULT iFindResult = pTexPalette->Find(&sTexName, &layer.m_uTextureId);
			if (IFXFAILURE(iFindResult)) 
			{
				// Add the texture name to the palette if not found
				IFXCHECKX(pTexPalette->Add(&sTexName, &layer.m_uTextureId));
			}
			// Should not need to create the texture object; adding to the palette is enough

			//    7.1 Texture blend (F32)
			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_fBlend = fTemp;

			//    7.2 Backdrop rotation (F32)
			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_fRotation = fTemp;

			//    7.3, 7.4 Backdrop location (F32 * 2)
			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_iLocX = fTemp;

			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_iLocY = fTemp;

			//    7.5, 7.6 Backdrop regpoint (I32 * 2)
			m_pBitStreamX->ReadI32X( iTemp );
			layer.m_iRegX = iTemp;

			m_pBitStreamX->ReadI32X( iTemp );
			layer.m_iRegY = iTemp;

			//    7.7, 7.8 Backdrop scale (F32 * 2)
			m_pBitStreamX->ReadF32X( layer.m_vScale.X() );
			m_pBitStreamX->ReadF32X( layer.m_vScale.Y() );

			IFXRect viewport;
			viewport.Set(0,0,1,1);
			IFXCHECKX(pViewNode->AddLayer(IFX_VIEW_BACKDROP, layer, &viewport, 1, 1 ));
		}

		// 7. Number of overlays
		uNumChannels = 0;
		m_pBitStreamX->ReadU32X( uNumChannels );

		for(i = 0; i < uNumChannels; i++) 
		{
			IFXViewLayer layer;
			IFXString sTexName;
			I32 iTemp = 0;

			// For Each Overlay:
			//    7.0 Texture Name (IFXString)
			m_pBitStreamX->ReadIFXStringX( sTexName );
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::TEXTURE, sTexName));

			// Look for a texture with the same name in the texture palette
			IFXRESULT iFindResult = pTexPalette->Find(&sTexName, &layer.m_uTextureId);
			if (IFXFAILURE(iFindResult)) 
			{
				// Add the texture name to the palette if not found
				IFXCHECKX(pTexPalette->Add(&sTexName, &layer.m_uTextureId));
			}
			// Should not need to create the texture object; adding to the palette is enough

			//    7.1 Texture blend (F32)
			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_fBlend = fTemp;

			//    7.2 Overlay rotation (F32)
			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_fRotation = fTemp;

			//    7.3, 7.4 Overlay location (F32 * 2)
			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_iLocX = fTemp;

			m_pBitStreamX->ReadF32X( fTemp );
			layer.m_iLocY = fTemp;

			//    7.5, 7.6 Overlay regpoint (I32 * 2)
			m_pBitStreamX->ReadI32X( iTemp );
			layer.m_iRegX = iTemp;

			m_pBitStreamX->ReadI32X( iTemp );
			layer.m_iRegY = iTemp;

			//    7.7, 7.8 Overlay scale (F32 * 2)
			m_pBitStreamX->ReadF32X( layer.m_vScale.X() );
			m_pBitStreamX->ReadF32X( layer.m_vScale.Y() );

			IFXRect viewport;
			viewport.Set(0,0,1,1);
			IFXCHECKX(pViewNode->AddLayer(IFX_VIEW_OVERLAY, layer, &viewport, 1, 1 ));
		}

		// try to find view resource

		// Get the view resource palette
		IFXDECLARELOCAL( IFXPalette, pViewResourcePalette );
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pViewResourcePalette ));

		U32 uViewResourceId = 0;
		// Look for the view resource in the view resource palette
		IFXRESULT rc = pViewResourcePalette->Find( &sResName, &uViewResourceId );

		// If the view resource was not found
		if (IFXFAILURE(rc))
		{
			IFXViewResource* pViewResource = NULL;
			// create a view resource and add the view resource to the palette
			rc = pViewResourcePalette->Add( &sResName, &uViewResourceId );

			if (IFXSUCCESS(rc))
			{
				IFXCHECKX( IFXCreateComponent( CID_IFXViewResource, IID_IFXViewResource, (void**)&pViewResource ) );
				IFXDECLARELOCAL( IFXUnknown, pUnk );
				IFXCHECKX( pViewResource->QueryInterface( IID_IFXUnknown, (void**)&pUnk ) );

				IFXDECLARELOCAL( IFXMarker, pMarker );
				IFXCHECKX(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
				IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
				IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
				IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
				pMarker->SetExternalFlag(m_bExternal);
				pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

				IFXCHECKX( pViewResourcePalette->SetResourcePtr( uViewResourceId, pUnk ) );
				IFXRELEASE(pUnk);
			}
			IFXRELEASE(pViewResource);
		}

		// Link the view node to the view resource pallete entry
		IFXDECLARELOCAL( IFXView, pView );
		IFXCHECKX( m_pNode->QueryInterface(IID_IFXView,(void**)&pView) );
		IFXCHECKX( pView->SetViewResourceID(uViewResourceId) );
		IFXRELEASE(pView);
	}
}

//-----------------------------------------------------------------------------

void CIFXViewDecoder::InitializeX(const IFXLoadConfig &lc)
{
	if( lc.m_units > 0.0f )
		m_units = lc.m_units;
	CIFXNodeBaseDecoder::InitializeX(lc);
}


void CIFXViewDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXView);
}

// Update node palette or scene graph
void CIFXViewDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	CheckInitializedX();

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) {
			// Determine the block type
			LoadViewNodeBlockU3DX( *pDataBlockX );
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXViewDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXLoadManager component.
		CIFXViewDecoder *pComponent = new CIFXViewDecoder;

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

	return rc;
}
