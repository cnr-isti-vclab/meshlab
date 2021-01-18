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
	@file	CIFXViewResourceDecoder.cpp

			Implementation of the CIFXViewResourceDecoder.
			The CIFXViewResourceDecoder is used by the CIFXLoadManager to load
			view nodes into the scenegraph. CIFXViewResourceDecoder exposes the
			IFXDecoderX interface to the CIFXLoadManager for this purpose.
*/

#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXView.h"
#include "CIFXViewResourceDecoder.h"
#include "IFXCheckX.h"

// Constructor
CIFXViewResourceDecoder::CIFXViewResourceDecoder() :
IFXDEFINEMEMBER(m_pViewRes)
{
	m_units = 1.0f;
}

// Destructor
CIFXViewResourceDecoder::~CIFXViewResourceDecoder()
{
}

// IFXUnknown
U32 CIFXViewResourceDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXViewResourceDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXViewResourceDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
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
//-----------------------------------------------------------------------------
// LoadViewResourceBlock(IFXDataBlock *pDataBlock)
//
// The view resource block has the following sections:
//
// 1. View resource name (IFXString)
//
// 2. Number of Render Passes (U32)
//
// For Each Render Pass:
//    2.1 RootNode name (IFXString)
//    2.2 Render Attributes (U32)
//    Fog:
//        2.3.1 Fog Mode (U32)
//        2.3.2 - 2.3.5 Fog Color (IFXVector4)
//        2.3.6 Fog Near Value (F32)
//        2.3.7 Fog Far Value (F32)
//-----------------------------------------------------------------------------
void CIFXViewResourceDecoder::LoadViewResourceBlockX(IFXDataBlockX &rDataBlockX)
{
	U32   uTemp   = 0 ;
	U8      u8Temp      = 0 ;
	F32   fTemp   = 0.0 ;
	IFXVector4 vTemp(0,0,0,0);

	IFXRELEASE(m_pBitStreamX);
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
	m_pBitStreamX->SetDataBlockX( rDataBlockX );

	// 1. View resource name
	IFXString sName;
	m_pBitStreamX->ReadIFXStringX( sName );

	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);

	// Get the scene graph
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
	IFXCHECKX( m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSceneGraph) );

	IFXDECLARELOCAL(IFXPalette,pNodePalette);
	IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pNodePalette ));

	// 2. Number of Render Passes (U32)
	U32 uNumPasses = 0;
	U32 i = 0;
	m_pBitStreamX->ReadU32X( uNumPasses );
	IFXCHECKX(m_pViewRes->SetNumRenderPasses( uNumPasses ));

	// For Each Render Pass:
	for(i = 0; i < (U32)uNumPasses; i++) {
		IFXCHECKX(m_pViewRes->SetCurrentRenderPass( i ));

		//    2.1 RootNode name (IFXString)
		IFXString stringRootNodeName;
		m_pBitStreamX->ReadIFXStringX(stringRootNodeName);
		IFXString sWorldName;
		IFXCHECKX(pNameMap->GetWorldAlias(m_uLoadId, sWorldName));
		if (stringRootNodeName.IsEmpty())
			stringRootNodeName = sWorldName;
		else if (stringRootNodeName != sWorldName) /// @todo: leave only else
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, stringRootNodeName));

		U32 uRootId = 0;
		// Look for the root node in the node palette
		IFXRESULT iFindResult = pNodePalette->Find(&stringRootNodeName, &uRootId);
		if (IFXFAILURE(iFindResult)) {
			// if the root node wasn't in the palette already,
			// then add the name, create the node, add the node to the palette
			IFXDECLARELOCAL(IFXNode, pRootNode);
			IFXCHECKX(pNodePalette->Add(&stringRootNodeName, &uRootId));
			IFXCHECKX(IFXCreateComponent(CID_IFXGroup,IID_IFXNode,(void**)&pRootNode));
			IFXCHECKX(pRootNode->SetSceneGraph(pSceneGraph));
			pRootNode->SetExternalFlag(m_bExternal);
			pRootNode->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

			IFXDECLARELOCAL(IFXUnknown,pUnk);
			IFXCHECKX(pRootNode->QueryInterface(IID_IFXUnknown,(void**)&pUnk));
			IFXCHECKX(pNodePalette->SetResourcePtr(uRootId,pUnk));
		}

		IFXCHECKX(m_pViewRes->SetRootNode(uRootId, 0));

		U32 uRenderAttributes = 0x00000000;

		//    2.2 Render Attributes (U32)
		m_pBitStreamX->ReadU32X( uRenderAttributes );

		//    Fog Enabled
		u8Temp = uRenderAttributes & 0x00000001;
		IFXCHECKX(m_pViewRes->FogEnable( (BOOL) u8Temp ));

		//    2.3.1 Fog Mode (U32)
		m_pBitStreamX->ReadU32X( uTemp );
		IFXCHECKX(m_pViewRes->GetRenderFog().SetMode( (IFXenum)uTemp ));

		//    2.3.2 - 2.3.5 Fog Color (IFXVector4)
		m_pBitStreamX->ReadF32X( vTemp.R() );
		m_pBitStreamX->ReadF32X( vTemp.G() );
		m_pBitStreamX->ReadF32X( vTemp.B() );
		m_pBitStreamX->ReadF32X( vTemp.A() );
		IFXCHECKX(m_pViewRes->GetRenderFog().SetColor( vTemp ));

		//    2.3.6 Fog Near Value (F32)
		m_pBitStreamX->ReadF32X( fTemp );
		IFXCHECKX(m_pViewRes->GetRenderFog().SetLinearNear( fTemp*(F32)m_units ));

		//    2.3.7 Fog Far Value (F32)
		m_pBitStreamX->ReadF32X( fTemp );
		IFXCHECKX(m_pViewRes->GetRenderFog().SetFar( fTemp*(F32)m_units ));
	} // For each render pass
}


void CIFXViewResourceDecoder::InitializeX(const IFXLoadConfig &lc)
{
	if( lc.m_units > 0.0f )
			m_units = lc.m_units;
	CIFXNodeBaseDecoder::InitializeX(lc);
}


void CIFXViewResourceDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	// create resource
	{
		// Get name
		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		IFXString sName;
		pBitStreamX->ReadIFXStringX( sName );

		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::VIEW, sName));

		// Get the scene graph
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		IFXCHECKX( m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pSceneGraph) );

		// Get the view resource palette
		IFXDECLARELOCAL( IFXPalette, pViewResourcePalette );
		IFXCHECKX( pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pViewResourcePalette) );

		// 1.1 Look for a view with the same name in the view resource palette
		U32 id=0;
		IFXRESULT rc = pViewResourcePalette->Find(&sName, &id);
		if(IFXFAILURE(rc))
		{
			// Add the name if it was not found
			IFXCHECKX( pViewResourcePalette->Add(sName, &id) );
		}

		// 1.1.1a If the view exists, Use it
		rc = pViewResourcePalette->GetResourcePtr(id, IID_IFXViewResource, (void**)&m_pViewRes);

		if (IFXFAILURE(rc)) // 1.1.1b Else, create it, and add it to the palette.
		{
			IFXCHECKX( IFXCreateComponent( CID_IFXViewResource, IID_IFXViewResource, (void**)&m_pViewRes ) );
			IFXDECLARELOCAL( IFXUnknown, pUnk );
			IFXCHECKX( m_pViewRes->QueryInterface(IID_IFXUnknown, (void**)&pUnk) );

			IFXDECLARELOCAL( IFXMarker, pMarker );
			IFXCHECKX(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
			IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
			IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
			IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
			pMarker->SetExternalFlag(m_bExternal);
			pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

			IFXCHECKX( pViewResourcePalette->SetResourcePtr(id, pUnk) );
			IFXRELEASE( pUnk );
		}
	}
}

// Update node palette or scene graph
void CIFXViewResourceDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if ( NULL == m_pCoreServices || m_pViewRes == NULL )
		IFXCHECKX(IFX_E_NOT_INITIALIZED);

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) {
			LoadViewResourceBlockX( *pDataBlockX );
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXViewResourceDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXLoadManager component.
		CIFXViewResourceDecoder *pComponent = new CIFXViewResourceDecoder;

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
