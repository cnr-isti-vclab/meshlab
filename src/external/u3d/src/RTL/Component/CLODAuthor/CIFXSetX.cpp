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
/**
@file CIFXSetX.cpp
	Implementation of CIFXSetX class.  
	This class is used by the progressive geometry compression and decompression.
*/
//*****************************************************************************

#include "CIFXSetX.h"
#include "IFXCheckX.h"
#include <memory.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
//	Constants
//-----------------------------------------------------------------------------
const U32 CIFXSetX::m_uArrayGrowthSize = 10;

//-----------------------------------------------------------------------------
//	Internal methods: (this is used for qsort/bsearch compare)
//-----------------------------------------------------------------------------
static int CompareU32Descending(const void *left, const void *right)
{
	return (int) (*(U32*)right) - (*(U32*)left);
}

//-----------------------------------------------------------------------------
//	Public methods
//-----------------------------------------------------------------------------


// IFXUnknown
U32 CIFXSetX::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXSetX::Release()
{
	if ( !( --m_uRefCount )) {
		delete this;
		return 0;
	}
	return m_uRefCount;
}

IFXRESULT CIFXSetX::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;
	if(ppInterface) {
		if (IID_IFXUnknown == interfaceId) {
			*ppInterface = (IFXUnknown*) this;
			this->AddRef();
		} else if (IID_IFXSetX == interfaceId) {
			*ppInterface = (IFXSetX*) this;
			this->AddRef();
		} else {
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}


// IFXSetX
void CIFXSetX::AddX(U32 uMember)
{
	U32* pm = NULL;

	if(NULL != m_puMemberArray && 0 < m_uMemberCount )
	{
		// Look for the element in the set
		pm = (U32*) bsearch(&uMember,m_puMemberArray,m_uMemberCount,sizeof(U32),CompareU32Descending);
	}

	// If the element is not already a member of the set
	if(NULL == pm) 
	{
		// expand the member array if necessary
		if(m_uMemberCount == m_uArraySize) 
		{
			U32* puTemp = m_puMemberArray;
			m_puMemberArray = new U32[m_uArraySize + m_uArrayGrowthSize];
			if(NULL == m_puMemberArray) 
			{
				m_puMemberArray = puTemp;
				IFXCHECKX(IFX_E_OUT_OF_MEMORY);
			}
			m_uArraySize += m_uArrayGrowthSize;
			memcpy(m_puMemberArray,puTemp,m_uMemberCount*sizeof(U32));
			memset(m_puMemberArray+m_uMemberCount,0,m_uArrayGrowthSize*sizeof(U32));
			IFXDELETE_ARRAY(puTemp);
		}

		// Add the new member to the set
		m_puMemberArray[m_uMemberCount] = uMember;
		m_uMemberCount++;

		// re-sort the set
		qsort(m_puMemberArray,m_uMemberCount,sizeof(U32),CompareU32Descending);
	}
}

void CIFXSetX::RemoveX(U32 uMember)
{
	U32* pm = NULL;

	if(NULL != m_puMemberArray && 0 < m_uMemberCount )
	{
		// Look for the element in the set
		pm = (U32*) bsearch(&uMember,m_puMemberArray,m_uMemberCount,sizeof(U32),CompareU32Descending);
	}

	// If the element is a member of the set
	if(NULL != pm) 
	{
		// Zero out the member 
		*pm = 0;
		// re-sort the set
		qsort(m_puMemberArray,m_uMemberCount,sizeof(U32),CompareU32Descending);
		// and reduce the member count
		m_uMemberCount--;
	}
}

void CIFXSetX::GetSizeX(U32& ruSize)
{
	ruSize = m_uMemberCount;
}

void CIFXSetX::GetMemberX(U32 uIndex, U32& ruMember)
{
	if(uIndex < m_uMemberCount) 
	{
		ruMember = m_puMemberArray[uIndex];
	} 
	else 
	{
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}
}

void CIFXSetX::GetIndexX(U32 uMember, BOOL& rbIsMember, U32& ruIndex)
{
	U32* pm = NULL;

	if(NULL != m_puMemberArray && 0 < m_uMemberCount )
	{
		// Look for the element in the set
		pm = (U32*) bsearch(&uMember,m_puMemberArray,m_uMemberCount,sizeof(U32),CompareU32Descending);
	}

	// If the element is a member of the set
	if(NULL != pm) 
	{
		rbIsMember = TRUE;
		ruIndex = (U32) (pm - m_puMemberArray);
	} 
	else 
	{
		rbIsMember = FALSE;
		ruIndex = (U32) -1;
	}
}


//-----------------------------------------------------------------------------
// Factory function
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXSetX_Factory(IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 	
	{
		// Create the CIFXSetX component.
		CIFXSetX *pComponent = new CIFXSetX;

		if ( pComponent ) 
		{
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} 
		else 
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

//-----------------------------------------------------------------------------
//	Private methods
//-----------------------------------------------------------------------------
CIFXSetX::CIFXSetX()
{
	m_uRefCount = 0;
	m_puMemberArray = NULL;
	m_uArraySize = 0;
	m_uMemberCount = 0;
}

CIFXSetX::~CIFXSetX()
{
	IFXDELETE_ARRAY(m_puMemberArray);
}
