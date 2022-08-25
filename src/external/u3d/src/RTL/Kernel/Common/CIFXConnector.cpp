//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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
	@file	CIFXConnector.cpp

			This module defines the CIFXConnector component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "CIFXConnector.h"
#include "IFXDebug.h"


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXConnector::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXConnector::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXConnector::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXConnection )
		{
			*ppInterface = ( IFXConnection* ) this;
		}
		else if ( interfaceId == IID_IFXConnectionServer || 
				  interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXConnectionServer* ) this;
		}
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXConnector::GetReference(
							IFXREFIID	rInterfaceId,
							void**		ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( m_pServerUnknown )
	{
		result = m_pServerUnknown->QueryInterface( 
										rInterfaceId,
										ppInterface );
	}
	else
		result = IFX_E_CONNECTION_SEVERED;

	return result;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXConnector::SetReference( IFXUnknown* pUnknown )
{
	// Keep a weak reference to the specified server's IFXUnknown interface. 
	// It's legal and appropriate to specify a NULL interface pointer 
	// because that's how a server disconnects itself from a connection it 
	// established with its clients.  Note:  Since we're only keeping a 
	// weak reference, there's no need to worry about the previous contents 
	// of m_pServerUnknown.
	m_pServerUnknown = pUnknown;

	return IFX_OK;
}


//***************************************************************************
//	Private methods
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the component constructor.  It doesn't do anything significant.
*/
CIFXConnector::CIFXConnector() :
	m_refCount( 0 ),
	m_pServerUnknown( NULL )
{
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXConnector::~CIFXConnector()
{
	// Note:  Because m_pServerUnknown is a weak interface reference it 
	// doesn't need to be released.  However, if it is non-NULL at this 
	// point then the connector is not being properly used.  To catch this 
	// sort of usage and design problem we'll do an assert on debug builds.
	IFXASSERT( !m_pServerUnknown );
}


//***************************************************************************
//	Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the CIFXConnector component factory function.  It's used by
	IFXCreateComponent to create an instance of the component and hand back
	a pointer to a supported interface that's requested.

	@note	This component can be instaniated multiple times.

	@param	interfaceId	Reference to the interface id that's supported by the
						component that will be handed back.
	@param	ppInterface	Pointer to the interface pointer that will be
						initialized upon successful creation of the
						component.

	@return	Upon success, IFX_OK is returned.  Otherwise, one of the
			following failures are returned:  IFX_E_COMPONENT,
			IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or
			IFX_E_UNSUPPORTED.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXConnector_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXConnector component.
		CIFXConnector	*pComponent	= new CIFXConnector;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			if ( IFXSUCCESS( result ) )
				result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the RobustConstructor or QI fails,
			// this will cause the component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


