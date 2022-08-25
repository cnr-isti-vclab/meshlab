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
//	CIFXDummyModifierEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXDummyModifierEncoder.
//		The CIFXDummyModifierEncoder contains CLOD modifier
//		encoding functionality that is used by the write manager.
//
//*****************************************************************************


#include "CIFXDummyModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"

// constructor
CIFXDummyModifierEncoder::CIFXDummyModifierEncoder() :
	IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXDummyModifierEncoder::~CIFXDummyModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXDummyModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXDummyModifierEncoder::Release()
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

IFXRESULT CIFXDummyModifierEncoder::QueryInterface( IFXREFIID interfaceId,
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
void CIFXDummyModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDummyModifier* pDummyModifier = NULL;
	IFXDataBlockX* pDataBlock = NULL;
	IFXDataBlockQueueX* pDBQueue = NULL;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		BOOL bDone = FALSE;
		IFXCHECKX( m_pObject->QueryInterface( IID_IFXDummyModifier, (void**)&pDummyModifier ) );
		pDBQueue = pDummyModifier->GetDataBlock();

		if( NULL == pDBQueue )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		do
		{
			IFXRELEASE( pDataBlock );
			pDBQueue->GetNextBlockX( pDataBlock, bDone );
			rDataBlockQueue.AppendBlockX(*pDataBlock);

		} while( !bDone );

		// clean up
		IFXRELEASE( pDummyModifier );
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pDBQueue );
	}
	catch ( ... )
	{
		IFXRELEASE( pDummyModifier );
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pDBQueue );

		throw;
	}
}

void CIFXDummyModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
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

void CIFXDummyModifierEncoder::SetObjectX( IFXUnknown& rObject )
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
IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXDummyModifierEncoder *pComponent = new CIFXDummyModifierEncoder;

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
