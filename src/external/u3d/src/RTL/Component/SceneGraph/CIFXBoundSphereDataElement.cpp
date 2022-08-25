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
@file  CIFXBoundSphereDataElement.cpp                                                         */


#include "CIFXBoundSphereDataElement.h"


CIFXBoundSphereDataElement::CIFXBoundSphereDataElement()
{
	m_uRefCount = 0;
  	m_vBoundingSphere.Set(0.0f,0.0f,0.0f,0.0f);
  	m_uRenderableDataElementIndex = (U32)-1;
}


CIFXBoundSphereDataElement::~CIFXBoundSphereDataElement()
{
}


IFXRESULT IFXAPI_CALLTYPE CIFXBoundSphereDataElement_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXBoundSphereDataElement *pBS = new CIFXBoundSphereDataElement;

		if ( pBS )
		{
			// Perform a temporary AddRef for our usage of the component.
			pBS->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pBS->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pBS->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXBoundSphereDataElement::AddRef()
{
	
	return ++m_uRefCount;
}


U32 CIFXBoundSphereDataElement::Release()
{
	
	if (m_uRefCount == 1)
	{
		delete this ;
		return 0 ;
	}
	else return (--m_uRefCount);
}


IFXRESULT CIFXBoundSphereDataElement::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXBoundSphereDataElement )
			*ppInterface = ( IFXBoundSphereDataElement* ) this;			
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
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


