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
/*
@file  CIFXDidRegistry.cpp

	The implementation file of the CIFXDidRegistry singleton component.      */

#include "CIFXDidRegistry.h"



static CIFXDidRegistry* gs_pSingleton = NULL;


CIFXDidRegistry::CIFXDidRegistry()
{
	m_uRefCount      = 0;
	gs_pSingleton    = this;
}


CIFXDidRegistry::~CIFXDidRegistry()
{
	gs_pSingleton = NULL;
}


U32 CIFXDidRegistry::AddRef(void) 
{
	
	return ++m_uRefCount;
}


U32 CIFXDidRegistry::Release(void) 
{
	
	if ( !( --m_uRefCount ) )
	{
		delete this;
		return 0;
	}

	return m_uRefCount;
}


IFXRESULT CIFXDidRegistry::QueryInterface( IFXREFIID interfaceId, 
                                           void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXDidRegistry )
			*ppInterface = ( IFXDidRegistry* ) this;
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


IFXRESULT IFXAPI_CALLTYPE CIFXDidRegistry_Factory(IFXREFIID interfaceId, void **ppInterface)
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// Does the singleton component already exist?
		if ( gs_pSingleton )
		{
			// It does exist, so just QI for the requested interface.
			result = gs_pSingleton->QueryInterface( interfaceId, ppInterface );
		}
		else
		{
			// It doesn't exist, so try to create it.  Note:  The component
			// class sets up gs_pSingleton upon construction and NULLs it
			// upon destruction.
			CIFXDidRegistry *pComponent	= new CIFXDidRegistry;

			if ( pComponent )
			{
				// Perform a temporary AddRef for our usage of the component.
				pComponent->AddRef();

				// Attempt to obtain a pointer to the requested interface.
				result = pComponent->QueryInterface( interfaceId, ppInterface );

				// Perform a Release since our usage of the component is now
				// complete.  Note:  If the QI fails, this will cause the
				// component to be destroyed.
				pComponent->Release();
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXDidRegistry

U32 CIFXDidRegistry::GetDidFlags( IFXREFDID rInDataElement )
{
	U32 *Flags = 0;
	if(m_Didhash.Find(rInDataElement, Flags))
	{
		return *Flags;
	}
		
	return 0;

}
	// IFXDidRegistry


IFXRESULT CIFXDidRegistry::CopyDID( IFXREFDID rInDataElement, IFXREFDID rInTemplate)
{
	IFXRESULT result = IFX_OK;
	IFXASSERT(!(rInDataElement == rInTemplate));
	if(IFXSUCCESS(result))
	{
		U32 *Flags = 0;
		if(m_Didhash.Find(rInTemplate, Flags))
		{
			m_Didhash[rInDataElement] = *Flags;
		}
	}
	
	return result;

}

IFXRESULT CIFXDidRegistry::AddDID( IFXREFDID rInDataElement, U32 in_Flags)
{
	m_Didhash[rInDataElement] = in_Flags;
	
	return IFX_OK;
}
