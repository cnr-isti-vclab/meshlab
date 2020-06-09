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
//	CIFXModelNodeEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXModelNodeEncoder.
//		The CIFXModelNodeEncoder contains model node encoding functionality 
//		that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXModelNodeEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXModel.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"


// constructor
CIFXModelNodeEncoder::CIFXModelNodeEncoder()
{
	m_bInitialized = FALSE;
	m_uRefCount = 0;
}

// destructor
CIFXModelNodeEncoder::~CIFXModelNodeEncoder()
{
}



// IFXUnknown
U32 CIFXModelNodeEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXModelNodeEncoder::Release()
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

IFXRESULT CIFXModelNodeEncoder::QueryInterface( IFXREFIID	interfaceId, 
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



// IFXEncoder
void CIFXModelNodeEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*	pDataBlock = NULL;
	IFXModel*		pModel = NULL;
	IFXPalette*		pModelResourcePalette = NULL;
	IFXResourceClient* pRsrcClient = NULL;

	try
	{
		// use node base class to encode shared node data (i.e. data that all node
		// types possess)

		// check for initialization
		if ( FALSE == CIFXModelNodeEncoder::m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		if ( NULL == m_pNode )
			throw IFXException( IFX_E_CANNOT_FIND );

		CIFXNodeBaseEncoder::CommonNodeEncodeU3D( rName, units );

		IFXCHECKX( m_pNode->QueryInterface( IID_IFXModel, (void**)&pModel ) );

		if ( NULL == pModel )
			throw IFXException( IFX_E_UNSUPPORTED );

		// Encode the model resource name
		U32 uModelResourceIndex = 0;
	
		IFXCHECKX( pModel->QueryInterface(IID_IFXResourceClient, (void**)&pRsrcClient ));
		uModelResourceIndex  = pRsrcClient->GetResourceIndex();
		IFXRELEASE(pRsrcClient);

		// nodes don't necessarily have valid resourceIDs, they
		// don't have to have an associated resource, so we need to handle
		// not being able to get a resource name.
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pModelResourcePalette ) );

		IFXString modelResourceName;

		if ( uModelResourceIndex != (U32)(-1) )
		{
			IFXCHECKX( pModelResourcePalette->GetName( uModelResourceIndex, 
													   &modelResourceName) );
		}
		else
		{
			IFXCHECKX( modelResourceName.Assign(L"") );	// make it a null string
		}
		m_pBitStream->WriteIFXStringX(modelResourceName);

		// Model attributes (U32)
		m_pBitStream->WriteU32X( pModel->GetVisibility() & 0x00000003 );

		// get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// set the data block type
		pDataBlock->SetBlockTypeX( BlockType_NodeModelU3D );

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
		IFXRELEASE( pModel );
		IFXRELEASE( pModelResourcePalette );
	}
	catch ( ... )
	{
		// release IFXCOM objects
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pModel );
		IFXRELEASE( pModelResourcePalette );
		IFXRELEASE( pRsrcClient );
		
		throw;
	}
}

void CIFXModelNodeEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// initialize base class(es)
		CIFXNodeBaseEncoder::Initialize( rCoreServices );

		// initialize locally
		if ( TRUE == CIFXNodeBaseEncoder::m_bInitialized )
			CIFXModelNodeEncoder::m_bInitialized = TRUE;
		else
			CIFXModelNodeEncoder::m_bInitialized = FALSE;
	}
	catch ( ... )
	{
		throw;
	}
}

void CIFXModelNodeEncoder::SetObjectX( IFXUnknown& rObject )
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
IFXRESULT IFXAPI_CALLTYPE CIFXModelNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXModelNodeEncoder *pComponent = new CIFXModelNodeEncoder;

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
