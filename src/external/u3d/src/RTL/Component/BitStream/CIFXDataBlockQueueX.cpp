//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
// CIFXDataBlockQueueX.cpp
//
// DESCRIPTION:
//     Declaration of CIFXDataBlockQueueX class implementation.
// CIFXDataBlockQueueX is used to hold a list of pointers to IFXDataBlock.
//
//*****************************************************************************

#include "IFXCheckX.h"
#include "CIFXDataBlockQueueX.h"

const U32 CIFXDataBlockQueueX::m_uInitialListSize = 10;
const U32 CIFXDataBlockQueueX::m_uIncrementListSize = 100;

CIFXDataBlockQueueX::CIFXDataBlockQueueX()
{
	m_uRefCount = 0;
	m_ppDataBlockList = NULL;
	m_uDataBlockListSize = 0;
	m_uDataBlockCount = 0;
	m_uDataBlockCurrent = 0;
}

CIFXDataBlockQueueX::~CIFXDataBlockQueueX()
{
	ClearX();
}

// Clear out the data block list, releasing the contents
void CIFXDataBlockQueueX::ClearX()
{
	IFXDataBlockX **ppTemp = m_ppDataBlockList;
	U32 uCurrent = m_uDataBlockCurrent;
	U32 uCount = m_uDataBlockCount;
	U32 i;

	m_ppDataBlockList = NULL;
	m_uDataBlockListSize = 0;
	m_uDataBlockCurrent = 0;
	m_uDataBlockCount = 0;

	if (NULL != ppTemp) {
		for(i = uCurrent; i < uCount; i++) {
			IFXRELEASE(ppTemp[i]);
		}
		
		delete [] ppTemp;
		ppTemp = NULL;
	}
}

// Get the next data block from the list (removing it from the list)
void CIFXDataBlockQueueX::GetNextBlockX(IFXDataBlockX*& rpDataBlockX, BOOL& rbDone)
{
	// Check for list is not empty
	if (NULL == m_ppDataBlockList || m_uDataBlockCurrent >= m_uDataBlockCount) {
		rpDataBlockX = NULL;
		rbDone = TRUE;
	} else {
		// Give the caller the reference to the next block
		rpDataBlockX = m_ppDataBlockList[m_uDataBlockCurrent];
		m_ppDataBlockList[m_uDataBlockCurrent] = NULL;
		m_uDataBlockCurrent++;
		
		rbDone = (m_uDataBlockCurrent >= m_uDataBlockCount);
	}
}

// Peek at the next block in the list without removing it from the list
void CIFXDataBlockQueueX::PeekNextBlockX(IFXDataBlockX*& rpDataBlockX)
{
	// Check for list is not empty
	if (NULL == m_ppDataBlockList || m_uDataBlockCurrent >= m_uDataBlockCount) {
		rpDataBlockX = NULL;
	} else {
		// Give the caller the reference to the next block
		rpDataBlockX = m_ppDataBlockList[m_uDataBlockCurrent];
		if(rpDataBlockX)
			rpDataBlockX->AddRef();
	}
}

// Add a data block to the end of the list
void CIFXDataBlockQueueX::AppendBlockX(IFXDataBlockX& rDataBlockX)
{
	// If the list does not exist, create the list
	if(NULL == m_ppDataBlockList) {
		m_ppDataBlockList = new IFXDataBlockX* [m_uInitialListSize];
		if(NULL == m_ppDataBlockList) {
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}
		
		m_uDataBlockListSize = m_uInitialListSize;
		
		U32 i;
		for(i=0;i<m_uDataBlockListSize;i++) {
			m_ppDataBlockList[i] = NULL;
		}
	}
	
	// If the list is too small, expand it
	if(m_uDataBlockCount >= m_uDataBlockListSize) {
		IFXDataBlockX** ppNewList = NULL;
		ppNewList = new IFXDataBlockX* [m_uDataBlockListSize + m_uIncrementListSize];
		if (NULL == ppNewList) {
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}
		
		m_uDataBlockListSize += m_uIncrementListSize;
		
		U32 i;
		for(i=0;i<m_uDataBlockCount;i++) {
			ppNewList[i] = m_ppDataBlockList[i];
		}
		for(i=m_uDataBlockCount;i<m_uDataBlockListSize;i++) {
			ppNewList[i] = NULL;
		}
		
		delete [] m_ppDataBlockList;
		
		m_ppDataBlockList = ppNewList;
	}
	
	// Put the block on the list
	rDataBlockX.AddRef();
	m_ppDataBlockList[m_uDataBlockCount] = &rDataBlockX;
	m_uDataBlockCount++;
}

// Get a copy of the data block list
void CIFXDataBlockQueueX::CopyX(IFXDataBlockQueueX*& rpDataBlockQueueX)
{
	IFXDECLARELOCAL(CIFXDataBlockQueueX,pNew);

	// Create a new CIFXDataBlockQueueX
	pNew = new CIFXDataBlockQueueX;
	if(NULL == pNew) {
		IFXCHECKX(IFX_E_OUT_OF_MEMORY);
	} 
	pNew->AddRef();
	
	// Copy the integers (do not copy the ref count)
	pNew->m_uDataBlockListSize = this->m_uDataBlockListSize;
	pNew->m_uDataBlockCount = this->m_uDataBlockCount;
	pNew->m_uDataBlockCurrent = this->m_uDataBlockCurrent;
	
	// Allocate the new array
	if(m_uDataBlockListSize > 0) {
		pNew->m_ppDataBlockList = new IFXDataBlockX* [m_uDataBlockListSize];
		if(NULL == pNew->m_ppDataBlockList) {
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}
	}
	
	// Copy data block pointers to the new array
	U32 i = 0;
	for(i=0;i<m_uDataBlockListSize;i++) {
		pNew->m_ppDataBlockList[i] = this->m_ppDataBlockList[i];
		if(NULL != pNew->m_ppDataBlockList[i]) {
			pNew->m_ppDataBlockList[i]->AddRef();
		}
	}
	
	// Assign the new queue to the output.
	rpDataBlockQueueX = (IFXDataBlockQueueX*) pNew;
	pNew->AddRef();
	//IFXRELEASE(pNew);
}

U32 CIFXDataBlockQueueX::AddRef() 
{
	return ++m_uRefCount;
}

U32 CIFXDataBlockQueueX::Release() 
{
	if ( 1 == m_uRefCount )
	{
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXDataBlockQueueX::QueryInterface(IFXREFIID interfaceId, void** ppInterface) 
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if (IID_IFXUnknown == interfaceId) {
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		} else if (IID_IFXDataBlockQueueX == interfaceId) {
			*ppInterface = ( IFXDataBlockQueueX* ) this;
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

    // Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXDataBlockQueueX_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXDataBlockQueueX component.
		CIFXDataBlockQueueX *pComponent = new CIFXDataBlockQueueX;

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

	IFXRETURN(rc);
}
