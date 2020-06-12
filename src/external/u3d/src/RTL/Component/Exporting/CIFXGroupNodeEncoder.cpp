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
//	CIFXGroupNodeEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXGroupNodeEncoder.
//		The CIFXGroupNodeEncoder contains group node encoding functionality 
//		that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXGroupNodeEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"


// constructor
CIFXGroupNodeEncoder::CIFXGroupNodeEncoder()
{
	m_bInitialized = FALSE;
	m_uRefCount = 0;
}

// destructor
CIFXGroupNodeEncoder::~CIFXGroupNodeEncoder()
{
}


// IFXUnknown
U32 CIFXGroupNodeEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXGroupNodeEncoder::Release()
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

IFXRESULT CIFXGroupNodeEncoder::QueryInterface( IFXREFIID	interfaceId, 
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
void CIFXGroupNodeEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*	pDataBlock = NULL;

	try
	{
		// check for initialization
		if ( FALSE == CIFXGroupNodeEncoder::m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		// use node base class to encode shared node data (i.e. data that all node
		// types possess)
		CIFXNodeBaseEncoder::CommonNodeEncodeU3D( rName, units );

		// get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// set the data block type
		pDataBlock->SetBlockTypeX( BlockType_NodeGroupU3D );

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
	}
	catch ( ... )
	{
		IFXRELEASE( pDataBlock );

		throw;
	}
}

void CIFXGroupNodeEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// initialize base class(es)
		CIFXNodeBaseEncoder::Initialize( rCoreServices );

		// initialize locally
		if ( TRUE == CIFXNodeBaseEncoder::m_bInitialized )
			CIFXGroupNodeEncoder::m_bInitialized = TRUE;
		else
			CIFXGroupNodeEncoder::m_bInitialized = FALSE;
	}
	catch ( ... )
	{
		throw;
	}
}

void CIFXGroupNodeEncoder::SetObjectX( IFXUnknown& rObject )
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
	}
	catch ( ... )
	{
		IFXRELEASE( pNode );
		
		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXGroupNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXGroupNodeEncoder *pComponent = new CIFXGroupNodeEncoder;

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

