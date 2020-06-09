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
@file  CIFXLightSet.cpp

	The implementation file of the CIFXLightSet component.      */

#include "CIFXLightSet.h"
#include "IFXSpatial.h"


CIFXLightSet::CIFXLightSet()
{
	m_uRefCount      = 0;

	m_pLights       = NULL;
	m_uCount         = 0;
	m_uAllocated     = 0;
}


CIFXLightSet::~CIFXLightSet()
{
	IFXDeallocate(m_pLights);
}


U32 CIFXLightSet::AddRef(void) 
{
	
	return ++m_uRefCount;
}


U32 CIFXLightSet::Release(void) 
{
	
	if ( !( --m_uRefCount ) )
	{
		delete this;
		return 0;
	}

	return m_uRefCount;
}


IFXRESULT CIFXLightSet::QueryInterface( IFXREFIID interfaceId, 
                                           void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXLightSet )
			*ppInterface = ( IFXLightSet* ) this;
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


IFXRESULT IFXAPI_CALLTYPE CIFXLightSet_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXLightSet *pLightSet = new CIFXLightSet;

		if ( pLightSet )
		{
			// Perform a temporary AddRef for our usage of the component.
			pLightSet->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pLightSet->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pLightSet->Release();
		}
		else 
			result = IFX_E_OUT_OF_MEMORY;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXLightSet
IFXRESULT CIFXLightSet::Set( U32 uInCount, SPATIALINSTANCE_LIST** ppInLights )
{
	IFXRESULT result = IFX_OK;

	if ( uInCount > m_uAllocated )
	{
		IFXDeallocate( m_pLights );
		m_pLights = (IFXSpatialInstance*)IFXAllocate( uInCount*sizeof(IFXSpatialInstance) );
		m_uAllocated = uInCount;
	}

	if ( m_uAllocated == 0 )
	{
		m_pLights = (IFXSpatialInstance*)IFXAllocate( 8*sizeof(IFXSpatialInstance) );
		m_uAllocated = 8;
	}

	if ( m_pLights )
	{
		U32 i;
		for ( i = 0; i < uInCount; i++) {
			m_pLights[i] = (*(*ppInLights))[i];
		}
		m_uCount = uInCount;
	}
	else 
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}



IFXRESULT CIFXLightSet::Append( U32 uInCount, SPATIALINSTANCE_LIST** ppInLights )
{
	IFXRESULT result = IFX_OK;


	if ( m_uAllocated == 0 )
	{
		m_pLights = (IFXSpatialInstance*)IFXAllocate( 8*sizeof(IFXSpatialInstance) );
		m_uAllocated = 8;
	}

	if ( m_uCount + uInCount > m_uAllocated )
	{
		m_pLights = (IFXSpatialInstance*)IFXReallocate(m_pLights,(m_uCount+uInCount)*sizeof(IFXSpatialInstance) );
		m_uAllocated = m_uCount + uInCount;
	}

	
	if ( m_pLights )
	{
		U32 i;
		for ( i = 0; i < uInCount; i++) {
			m_pLights[m_uCount+i] = (*(*ppInLights))[i];
		}
		m_uCount += uInCount;
	}
	else 
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}



IFXRESULT CIFXLightSet::GetLight( U32 uLightIndex, IFXLight*& rpOutLight, U32& lightInstance ) const
{
	IFXRESULT result = IFX_OK;

	if ( uLightIndex < m_uCount ) {
		if (NULL != m_pLights[uLightIndex].m_pSpatial)
		{
			lightInstance = m_pLights[uLightIndex].m_Instance;
			result = m_pLights[uLightIndex].m_pSpatial->QueryInterface( IID_IFXLight, (void**)&rpOutLight );
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	} else 
		result = IFX_E_INVALID_RANGE;

	return result;
}
