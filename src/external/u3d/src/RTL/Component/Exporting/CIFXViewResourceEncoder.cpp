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
	@file	CIFXViewResourceEncoder.cpp

			Implementation of the CIFXViewResourceEncoder.
			The CIFXViewResourceEncoder contains view node encoding functionality 
			that is used by the write manager.
*/

#include "CIFXViewResourceEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXView.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"

// constructor
CIFXViewResourceEncoder::CIFXViewResourceEncoder()
{
	m_bInitialized = FALSE;
	m_uRefCount = 0;
	m_pViewRes = NULL;
}

// destructor
CIFXViewResourceEncoder::~CIFXViewResourceEncoder()
{
	IFXRELEASE( m_pViewRes );
}

// IFXUnknown
U32 CIFXViewResourceEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXViewResourceEncoder::Release()
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

IFXRESULT CIFXViewResourceEncoder::QueryInterface( IFXREFIID	interfaceId, 
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
//-----------------------------------------------------------------------------
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
// use node base class to encode shared node data (i.e. data that all node
// types possess)
//
void CIFXViewResourceEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*	pDataBlock = NULL;
	IFXPalette*		pNodePalette = NULL;
	BOOL			bTemp = 0;

	try
	{
		// check for initialization
		if ( FALSE == CIFXViewResourceEncoder::m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		if( units <= 0.0f )
			throw IFXException( IFX_E_INVALID_RANGE );

		// get palettes
		IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pNodePalette ));

		// get the view (node) interface of the node
		if ( NULL == m_pViewRes )
			throw IFXException( IFX_E_CANNOT_FIND );

		// 1. View resource name
		m_pBitStream->WriteIFXStringX( rName );

		// 2. Number of Render Passes (U8)
		U32 i = 0;
		U32 uNumPasses = 0;
		uNumPasses = m_pViewRes->GetNumRenderPasses();
		m_pBitStream->WriteU32X(uNumPasses);

		// For Each Render Pass:
		for(i = 0; i < uNumPasses; i++)
		{
			IFXString tempString;

			IFXCHECKX(m_pViewRes->SetCurrentRenderPass(i));

			//    2.1 RootNode name (IFXString)
			U32 nodeIndex = 0, nodeInstance = 0;

			IFXCHECKX( m_pViewRes->GetRootNode( &nodeIndex, &nodeInstance ) ); //  no addref occurs in this call
			IFXCHECKX( pNodePalette->GetName( nodeIndex, &tempString ) );
			m_pBitStream->WriteIFXStringX( tempString );

			//	2.2. Render Attributes (U32)
			U32 uRenderAttributes = 0x00000000;

			//    Fog Enabled
			IFXCHECKX(m_pViewRes->GetFogEnableValue(&bTemp));
			uRenderAttributes |= ((U8)(bTemp != 0)) * 0x00000001;

			m_pBitStream->WriteU32X( uRenderAttributes );

			//    2.3.1 Fog Mode (U32)
			m_pBitStream->WriteU32X((U32)m_pViewRes->GetRenderFog().GetMode());

			//    2.3.2 - 2.3.5 Fog Color (IFXVector4)
			const F32* pfColor = m_pViewRes->GetRenderFog().GetColor().RawConst();
			m_pBitStream->WriteF32X(pfColor[0]);
			m_pBitStream->WriteF32X(pfColor[1]);
			m_pBitStream->WriteF32X(pfColor[2]);
			m_pBitStream->WriteF32X(pfColor[3]);

			//    2.3.6 Fog Near Value (F32)
			m_pBitStream->WriteF32X(m_pViewRes->GetRenderFog().GetLinearNear()/(F32)units);

			//    2.3.7 Fog Far Value (F32)
			m_pBitStream->WriteF32X(m_pViewRes->GetRenderFog().GetFar()/(F32)units);
		}

		// Get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// Set the data block type
		pDataBlock->SetBlockTypeX( BlockType_ResourceViewU3D );
		
		// Set the Priority on the Datablock
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pViewRes->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX( *pDataBlock );

		// clean up:
		IFXRELEASE( pNodePalette );
		IFXRELEASE( pDataBlock );
	}
	catch ( ... )
	{
		// release IFXCOM objects
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pNodePalette );

		throw;
	}
}

void CIFXViewResourceEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// initialize base class(es)
		CIFXNodeBaseEncoder::Initialize( rCoreServices );

		// initialize locally
		if ( TRUE == CIFXNodeBaseEncoder::m_bInitialized )
			CIFXViewResourceEncoder::m_bInitialized = TRUE;
		else
			CIFXViewResourceEncoder::m_bInitialized = FALSE;
	}
	catch ( ... )
	{
		throw;
	}
}

void CIFXViewResourceEncoder::SetObjectX( IFXUnknown& rObject )
{
	try
	{
		IFXCHECKX( rObject.QueryInterface( IID_IFXViewResource, (void**)&m_pViewRes ) );

		if ( NULL == m_pViewRes )
			throw IFXException( IFX_E_UNSUPPORTED );
	}
	catch ( ... )
	{
		IFXRELEASE( m_pViewRes );
		
		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXViewResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXViewResourceEncoder *pComponent = new CIFXViewResourceEncoder;

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
