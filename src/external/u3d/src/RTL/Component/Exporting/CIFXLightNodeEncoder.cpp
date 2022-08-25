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
//	CIFXLightNodeEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXLightNodeEncoder.
//		The CIFXLightNodeEncoder contains light node encoding functionality 
//		that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXLightNodeEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXLight.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"


// constructor
CIFXLightNodeEncoder::CIFXLightNodeEncoder()
{
	m_bInitialized = FALSE;
	m_uRefCount = 0;
}

// destructor
CIFXLightNodeEncoder::~CIFXLightNodeEncoder()
{
}



// IFXUnknown
U32 CIFXLightNodeEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXLightNodeEncoder::Release()
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

IFXRESULT CIFXLightNodeEncoder::QueryInterface( IFXREFIID	interfaceId, 
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
void CIFXLightNodeEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// initialize base class(es)
		CIFXNodeBaseEncoder::Initialize( rCoreServices );

		// initialize locally
		if ( TRUE == CIFXNodeBaseEncoder::m_bInitialized )
			CIFXLightNodeEncoder::m_bInitialized = TRUE;
		else
			CIFXLightNodeEncoder::m_bInitialized = FALSE;

		return;
	}

	catch ( ... )
	{
		throw;
	}
}

void CIFXLightNodeEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*	pDataBlock = NULL;
	IFXLight*		pLight = NULL;

	try
	{
		
		// use node base class to encode shared node data (i.e. data that all node
		// types possess)

		// check for initialization
		if ( FALSE == CIFXLightNodeEncoder::m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		if ( NULL == m_pNode )
			throw IFXException( IFX_E_CANNOT_FIND );

		CIFXNodeBaseEncoder::CommonNodeEncodeU3D( rName, units );

		m_pNode->QueryInterface( IID_IFXLight, (void**)&pLight );

		// get resource name
		IFXString sName;

		U32 uLightResourceIndex = 0;
		pLight->GetLightResourceID( &uLightResourceIndex );
		IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
	
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette ) );

		if ( uLightResourceIndex != (U32)(-1) )
		{
			IFXCHECKX( pLightResourcePalette->GetName( uLightResourceIndex, 
													   &sName) );
		}
		else
		{
			IFXCHECKX( sName.Assign(L"") );	// make it a null string
		}

		// write resource name
		m_pBitStream->WriteIFXStringX( sName );

		// get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// set the data block type
		pDataBlock->SetBlockTypeX( BlockType_NodeLightU3D );

		// set the priority on the datablock
		pDataBlock->SetPriorityX( 0 ); // returns void

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pNode->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX( *pDataBlock );

		// clean up
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pLight );

		return;
	}

	catch ( ... )
	{
		// release IFXCOM objects
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pLight );

		throw;
	}
}

void CIFXLightNodeEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXNode* pNode = NULL;


	try
	{
		// get the IFXNode interface
		IFXCHECKX( rObject.QueryInterface( IID_IFXNode, (void**)&pNode ) );

		if ( NULL == pNode )
			throw IFXException( IFX_E_INVALID_POINTER );

		// set the node on the CIFXNodeBaseEncoder base class
		CIFXNodeBaseEncoder::SetNode( *pNode );

		// clean up
		IFXRELEASE( pNode );

		return;
	}

	catch ( ... )
	{
		IFXRELEASE( pNode );
		
		throw;
	}
}






// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXLightNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXLightNodeEncoder *pComponent = new CIFXLightNodeEncoder;

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


