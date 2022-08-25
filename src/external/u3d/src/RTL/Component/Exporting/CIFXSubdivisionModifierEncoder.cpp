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
//  CIFXSubdivisionModifierEncoder.cpp
//
//  DESCRIPTION:
//    Implementation of the CIFXSubdivisionModifierEncoder.
//    The CIFXSubdivisionModifierEncoder contains SDS (surface subdivision) modifier
//    encoding functionality that is used by the write manager.
//
//*****************************************************************************


#include "CIFXSubdivisionModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXSubdivModifier.h"

// constructor
CIFXSubdivisionModifierEncoder::CIFXSubdivisionModifierEncoder() :
IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXSubdivisionModifierEncoder::~CIFXSubdivisionModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXSubdivisionModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXSubdivisionModifierEncoder::Release()
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

IFXRESULT CIFXSubdivisionModifierEncoder::QueryInterface( IFXREFIID interfaceId,
														 void**  ppInterface )
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
void CIFXSubdivisionModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXSubdivModifier* pSubdivisionModifier = NULL;

	F32  fTemp = 0.0f;
	BOOL bTemp = FALSE;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );


		IFXCHECKX( m_pObject->QueryInterface( IID_IFXSubdivModifier,
			(void**)&pSubdivisionModifier ) );

		// The following elements are common to all Modifier blocks
		// and are encoded in EncodeCommonElements():
		// 1. ModelGenerator Name
		// 2. Index for modifier chain

		//    CIFXModifierBaseEncoder::SetName( rName );

		m_pBitStream->WriteIFXStringX( rName );

		U32 uTemp = 0;
		IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
		m_pBitStream->WriteU32X( uTemp );

		// SDS Modifiers have the following parameters:
		// 1. Enabled, Adaptive
		// 2. Depth
		// 3. Tension
		// 4. Error

		/// @todo: IFXSubdivModifier should be changed to use
		///     BOOL and not 'bool' types.

		// 1. Enable
		U32 uFlags = 0;
		IFXCHECKX( pSubdivisionModifier->GetEnable( &bTemp ) );
		if (bTemp) uFlags |= 0x00000001;
		IFXCHECKX( pSubdivisionModifier->GetAdaptive( &bTemp ) );
		if (bTemp) uFlags |= 0x00000002;
		m_pBitStream->WriteU32X( uFlags );

		// 2. Depth
		IFXCHECKX( pSubdivisionModifier->GetDepth( &uTemp ) );
		m_pBitStream->WriteU32X( uTemp );

		// 3. Tension
		IFXCHECKX( pSubdivisionModifier->GetTension( &fTemp ) );
		m_pBitStream->WriteF32X( fTemp );

		// 4. Error
		IFXCHECKX( pSubdivisionModifier->GetError( &fTemp ) );
		m_pBitStream->WriteF32X( fTemp );

		//-------------------------------------------------------------------------
		// Done with Subdiv specific parameters.
		//-------------------------------------------------------------------------
		//    CommonDataBlockFinish( *m_pBitStream, BlockType_ModifierSubdivisionU3D, 0, rDataBlockQueue );
		IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

		// Get a data block from the bitstream
		m_pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX(BlockType_ModifierSubdivisionU3D);

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
		IFXRELEASE( pSubdivisionModifier );
	}

	catch ( ... )
	{
		IFXRELEASE( pSubdivisionModifier );

		throw;
	}
}

void CIFXSubdivisionModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
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

void CIFXSubdivisionModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;

	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();

		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

		//    CIFXModifierBaseEncoder::SetModifier( *pModifier );
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
IFXRESULT IFXAPI_CALLTYPE CIFXSubdivisionModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXSubdivisionModifierEncoder *pComponent = new CIFXSubdivisionModifierEncoder;

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
