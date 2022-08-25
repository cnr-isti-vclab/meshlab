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
#include "CIFXDecoderChainX.h"
#include "IFXCheckX.h"
#include <memory.h>

// constants
const U32 CIFXDecoderChainX::m_uArrayGrowthModifier = 1; 
// We grow by a factor of one each time because this reflects the behavior of
// the modifier chain, that is, the end of this chain must be the same as the end
// of the modifier chain. It is also estimated the mode average size of chains = 1.

// constructor
CIFXDecoderChainX::CIFXDecoderChainX()
{
	m_ppDecoderArray = NULL;
	m_uArraySize = 0;
	m_uDecoderCount = 0;
	m_uRefCount = 0;
}

// destructor
CIFXDecoderChainX::~CIFXDecoderChainX()
{
	ReleaseArrayDecoders();
	IFXDELETE_ARRAY( m_ppDecoderArray );
}

// factory function
IFXRESULT IFXAPI_CALLTYPE CIFXDecoderChainX_Factory( IFXREFIID	interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXDecoderChainX *pComponent = new CIFXDecoderChainX;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN( rc );
}



// IFXUnknown
U32 CIFXDecoderChainX::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXDecoderChainX::Release()
{
	if ( 1 == m_uRefCount)  
	{
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXDecoderChainX::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if (IID_IFXDecoderChainX == interfaceId)
		{
			*ppInterface = (IFXDecoderChainX*)this;
			AddRef();
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
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}


// IFXDecoderChainX
void CIFXDecoderChainX::AddDecoderX( IFXDecoderX& rDecoderX, U32 uIndex )
{
	// check for initialization
	IFXASSERT(m_ppDecoderArray);

	// if no index is specified, then add the element to the end of the chain
	if ( END_OF_CHAIN == uIndex ) {
		uIndex = m_uDecoderCount;
	}

	// check for allocated space and expand if necessary
	if (uIndex >= m_uArraySize  ) 
	{
		IFXDecoderX** ppTemp = m_ppDecoderArray;
		m_ppDecoderArray = new IFXDecoderX*[ uIndex + m_uArrayGrowthModifier ];
	
		if ( NULL == m_ppDecoderArray ) {
			m_ppDecoderArray = ppTemp;
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}

		U32 uOldArraySize = m_uArraySize;
		m_uArraySize = uIndex + m_uArrayGrowthModifier;
		memset( m_ppDecoderArray, 0, m_uArraySize * sizeof(IFXDecoderX*) );
		memcpy( m_ppDecoderArray, ppTemp, uOldArraySize * sizeof(IFXDecoderX*) );

		IFXDELETE_ARRAY( ppTemp );
	}

	// add the new component to the chain
	rDecoderX.AddRef();
	IFXRELEASE(m_ppDecoderArray[ uIndex ]);
	m_ppDecoderArray[ uIndex ] = &rDecoderX;
	m_uDecoderCount++;
}

void CIFXDecoderChainX::InitializeX()
{
	// initialize array size to size of CIFXDecoderChainX::m_uArrayGrowthModifier
	if ( m_ppDecoderArray )
	{
		ReleaseArrayDecoders();
		IFXDELETE_ARRAY( m_ppDecoderArray );
	}

	m_uArraySize = m_uArrayGrowthModifier;
	m_ppDecoderArray = new IFXDecoderX*[ m_uArraySize ];
	if ( NULL == m_ppDecoderArray ) {
		IFXCHECKX(IFX_E_OUT_OF_MEMORY);
	}

	m_uDecoderCount = 0;
	memset( m_ppDecoderArray, 0, (m_uArraySize * sizeof(IFXDecoderX*)) );
}

void CIFXDecoderChainX::GetDecoderCountX( U32& ruDecoderCount )
{
	ruDecoderCount = m_uDecoderCount;
}

void CIFXDecoderChainX::GetDecoderX( U32 uIndex, IFXDecoderX*& rpOutDecoderX )
{
	IFXASSERT(m_ppDecoderArray);

	if ( uIndex < m_uArraySize  ) {
		rpOutDecoderX = m_ppDecoderArray[ uIndex ];
		if(rpOutDecoderX) {
			rpOutDecoderX->AddRef();
		}
	} else {
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}
}

void CIFXDecoderChainX::RemoveDecoderX( U32 uIndex )
{
	IFXASSERT(m_ppDecoderArray);

	if ( uIndex < m_uArraySize  ) {
		if( m_ppDecoderArray[uIndex]) {
			m_uDecoderCount--;
		}
		IFXRELEASE(m_ppDecoderArray[uIndex]);
	} else {
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}
}


// private members
void CIFXDecoderChainX::ReleaseArrayDecoders()
{
	IFXASSERT(m_ppDecoderArray);

	U32 i = 0;
	for ( i = 0; i < m_uArraySize ; i++ ) {
		IFXRELEASE(m_ppDecoderArray[i]);
	}
}
