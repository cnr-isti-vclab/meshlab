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
//
//  CIFXSimpleObject.cpp
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************
//*****************************************************************************
//	Includes
//*****************************************************************************
#include "CIFXSimpleObject.h"
#include "IFXCoreCIDs.h"

//*****************************************************************************
//	Defines
//***************************************************************************** 

//*****************************************************************************
//	Constants
//***************************************************************************** 

//*****************************************************************************
//	Enumerations
//*****************************************************************************

//*****************************************************************************
//	Global data
//*****************************************************************************

//*****************************************************************************
//	Local data
//*****************************************************************************

//*****************************************************************************
//	Classes, structures and types
//*****************************************************************************

//-----------------------------------------------------------------------------
// CIFXSimpleObject::CIFXSimpleObject
//
// Constructor
//-----------------------------------------------------------------------------

CIFXSimpleObject::CIFXSimpleObject()
{
	// IFXUnknown attributes...
	m_uRefCount = 0; 
}


//-----------------------------------------------------------------------------
// CIFXSimpleObject::~CIFXSimpleObject
//
// Destructor
//-----------------------------------------------------------------------------

CIFXSimpleObject::~CIFXSimpleObject()
{
	CIFXSubject::PreDestruct();
}


IFXRESULT IFXAPI_CALLTYPE CIFXSimpleObject_Factory(IFXREFIID iid, void** ppv)
{
	IFXRESULT result = IFX_OK;

	if( ppv )	
	{
		// Create the CIFXBitStream component.
		CIFXSimpleObject *pComponent = new CIFXSimpleObject;

		if ( pComponent ) 
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( iid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} 
		else 
			result = IFX_E_OUT_OF_MEMORY;
	} 
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown Interface...
U32 CIFXSimpleObject::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXSimpleObject::Release()
{
	if (--m_uRefCount) 
		return m_uRefCount;

	delete this;
	return 0;
}


IFXRESULT CIFXSimpleObject::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXUnknown )
		{
			*ppv = (IFXUnknown*)this;
			AddRef();
		}
		else if ( riid == IID_IFXSubject )
		{
			*ppv = (IFXSubject*)this;
			AddRef();
		}
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXSubject Interface

IFXRESULT CIFXSimpleObject::Attach( IFXObserver* pInObserver, 
									U32 uInIntrestBits, 
									IFXREFIID rIType, 
									U32 shiftBits )
{
	return CIFXSubject::Attach(pInObserver, uInIntrestBits, rIType, shiftBits);
}


// removes an observer 
IFXRESULT CIFXSimpleObject::Detach(IFXObserver* pObs)
{
	return CIFXSubject::Detach(pObs);
}


// the observers will get update calls when the 
// subject state the observer is interested in changes
void CIFXSimpleObject::PostChanges(U32 changedBits)
{
	CIFXSubject::PostChanges(changedBits);
}


void CIFXSimpleObject::PreDestruct()
{
	CIFXSubject::PreDestruct();
}
