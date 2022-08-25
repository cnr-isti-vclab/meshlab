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
#include "IFXFastAllocator.h"

IFXFastAllocator::IFXFastAllocator()
{
	m_initialSize = 0;
	m_growSize = 0;
	m_pHeap = NULL;
	m_pFree = NULL;
	m_pEnd = NULL;
}


IFXFastAllocator::~IFXFastAllocator()
{
	FreeAll();
}


IFXRESULT IFXFastAllocator::Initialize(U32 initialSize, U32 growSize)
{
	IFXASSERT(m_pHeap == NULL);

	m_initialSize = initialSize;
	m_growSize = growSize;
	m_pFree = new U8[initialSize + sizeof(U8*)];
	if (!m_pFree)
		return IFX_E_OUT_OF_MEMORY;

	m_pHeap = m_pFree;
	m_pEnd = m_pFree + initialSize;
	*((U8**)(m_pEnd)) = NULL;
	return IFX_OK;
}


void IFXFastAllocator::FreeAll()
{
	if (m_pHeap)
	{
		U8* pChunk = *((U8**)(m_pHeap + m_initialSize));
		delete m_pHeap;
		m_pHeap = NULL;

		U8* pNextChunk;
		while (pChunk)
		{
//			IFXASSERT(_msize(pChunk) == int(m_growSize)+sizeof(U8*));
			pNextChunk = *((U8**)(pChunk + m_growSize));
			delete pChunk;
			pChunk = pNextChunk;
		}
	}
}


U8* IFXFastAllocator::GrowThenAllocate(U32 size)
{
	IFXASSERT(size < m_growSize);

	m_pFree = new U8[m_growSize + sizeof(U8*)];
	if (!m_pFree)
		return NULL;

	*((U8**)(m_pEnd)) = m_pFree;
	m_pEnd = m_pFree + m_growSize;
	*((U8**)(m_pEnd)) = NULL;

	// this is guaranteed
	U8 *pPtr = m_pFree;
	m_pFree += size;
	return pPtr;
}
