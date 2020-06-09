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
//	CIFXCLODModifierEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXCLODModifierEncoder.
//		The CIFXCLODModifierEncoder contains CLOD modifier
//		encoding functionality that is used by the write manager.
//
//*****************************************************************************


#include "CIFXCLODModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXCLODModifier.h"

// constructor
CIFXCLODModifierEncoder::CIFXCLODModifierEncoder() :
	IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXCLODModifierEncoder::~CIFXCLODModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXCLODModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXCLODModifierEncoder::Release()
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

IFXRESULT CIFXCLODModifierEncoder::QueryInterface( IFXREFIID interfaceId,
												  void**	ppInterface )
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
void CIFXCLODModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXCLODModifier* pCLODModifier = NULL;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXCLODModifier, (void**)&pCLODModifier ) );

		// The following elements are common to all Modifier blocks
		// and are encoded in EncodeCommonElements():
			// 1. ModelGenerator Name
			// 2. Index for modifier chain

		// 1. Name
		m_pBitStream->WriteIFXStringX( rName );

		// 2. Chain index
		U32 uTemp = 0;
		IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
		m_pBitStream->WriteU32X( uTemp );

		// 3. CLOD Modifier Attributes
		// 0x00000000: Default attributes ( automatic LOD control is disabled )
		// 0x00000001: Automatic level of detail control based on screen space method
		BOOL bTemp;
		IFXCHECKX( pCLODModifier->GetCLODScreenSpaceControllerState( &bTemp ) );
		m_pBitStream->WriteU32X( (U32)bTemp );

		// 4. CLOD Screen space bias
		F32 fTemp;
		IFXCHECKX( pCLODModifier->GetLODBias( &fTemp ) );
		m_pBitStream->WriteF32X( fTemp );

		// 5. CLOD Modifier level
		IFXCHECKX( pCLODModifier->GetCLODLevel( &fTemp ) );
		m_pBitStream->WriteF32X( fTemp );

		//-------------------------------------------------------------------------
		// Done with CLODModifier specific parameters.
		//-------------------------------------------------------------------------
		IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

		// Get a data block from the bitstream
		m_pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX(BlockType_ModifierCLODU3D);

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX(*pDataBlock);

		// clean up
		IFXRELEASE( pCLODModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( pCLODModifier );

		throw;
	}
}

void CIFXCLODModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
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
		IFXRELEASE( m_pBitStream );

		throw;
	}
}

void CIFXCLODModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;

	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();

		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

		pModifier->AddRef();
		IFXRELEASE( m_pModifier );
		m_pModifier = pModifier;

		IFXRELEASE( pModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter
		IFXRELEASE( pModifier );

		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXCLODModifierEncoder *pComponent = new CIFXCLODModifierEncoder;

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
