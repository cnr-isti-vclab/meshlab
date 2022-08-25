//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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
	@file	CIFXTaskCallback.cpp

			This module defines the CIFXTaskCallback component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "CIFXTaskCallback.h"


//***************************************************************************
//	Public methods
//***************************************************************************


//---------------------------------------------------------------------------
U32 CIFXTaskCallback::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXTaskCallback::Release()
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
IFXRESULT CIFXTaskCallback::QueryInterface( 
								IFXREFIID	interfaceId, 
								void**		ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXTask )
		{
			*ppInterface = ( IFXTask* ) this;
		}
		else if ( interfaceId == IID_IFXTaskCallback )
		{
			*ppInterface = ( IFXTaskCallback* ) this;
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
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
IFXRESULT CIFXTaskCallback::Execute( 
								IFXTaskData*	pTaskData )
{
	IFXRESULT	result	= IFX_OK;

	if ( m_pCallbackFunction )
		result = m_pCallbackFunction( pTaskData );

	return result;
}

//---------------------------------------------------------------------------
IFXTaskCallbackFunction* CIFXTaskCallback::GetFunction() const
{
	return m_pCallbackFunction;
}

//---------------------------------------------------------------------------
void CIFXTaskCallback::SetFunction(
							IFXTaskCallbackFunction*	pCallbackFunction )
{
	m_pCallbackFunction = pCallbackFunction;
}


//***************************************************************************
//	Private methods
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the component constructor.
*/
CIFXTaskCallback::CIFXTaskCallback() :
	m_refCount( 0 ), 
	m_pCallbackFunction( NULL )
{
}

//---------------------------------------------------------------------------
/**
	This is the component destructor.  After all references to the component
	are released, it performs any necessary cleanup activities.
*/
CIFXTaskCallback::~CIFXTaskCallback()
{
}

//***************************************************************************
//	Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the CIFXTaskCallback component factory function.  It's used by
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
IFXRESULT IFXAPI_CALLTYPE CIFXTaskCallback_Factory( 
			IFXREFIID	interfaceId, 
			void**		ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXTaskCallback component.
		CIFXTaskCallback	*pComponent	= new CIFXTaskCallback;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If QI fails, this will cause the component 
			// to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


