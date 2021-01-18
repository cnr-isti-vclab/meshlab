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
//	IFXSharedUnitAllocator.cpp
//
//	DESCRIPTION
//		Source code for module IFXSharedUnitAllocator.
//
//	NOTES
//
//***************************************************************************
#include "IFXSharedUnitAllocator.h"

// Constructor for IFXSharedUnitAllocator
IFXSharedUnitAllocator::IFXSharedUnitAllocator()
{
	m_uNumGrownSegments = 0;
	m_firstSegmentSize  = 0;
    m_growSegmentSize   = 0;
	m_refCountOffset    = 0;
	m_allocListSize     = 0;
	m_freeListSize      = 0;
 	m_numGrowUnits      = 0;
	m_unitSize          = 0;
	m_numUnits          = 0;
 
    m_pHeap = NULL;
    m_pFree = NULL;
    m_pEnd  = NULL;
}

// Destructor for IFXSharedUnitAllocator
IFXSharedUnitAllocator::~IFXSharedUnitAllocator()
{
    Destroy();
}


IFXRESULT IFXSharedUnitAllocator::Initialize(U32 unitSize, U32 numUnits, U32 numGrowUnits)
{
	IFXASSERT((unitSize & 0x00000003) == 0x00000000);	// Warn about alignment issues

	m_unitSize       = unitSize + sizeof(U8*);
	m_numUnits       = numUnits;
	m_refCountOffset = m_unitSize - sizeof(U8*);
	m_freeListSize   = m_numUnits;
	m_numGrowUnits   = numGrowUnits;

	if( m_numGrowUnits == 0 ) m_numGrowUnits = numUnits;

    // Compute segment sizes:
	m_firstSegmentSize = m_unitSize * m_numUnits;
    m_growSegmentSize = m_numGrowUnits * m_unitSize;

	if( !(m_pHeap = new U8[m_firstSegmentSize + sizeof(U8*)]) )
		return IFX_E_OUT_OF_MEMORY;

	m_pFree = m_pHeap;
	m_pEnd  = m_pHeap + m_firstSegmentSize;

    // Set next segment pointer to Null for now:
    U8 **ppNextSegmentPtr = (U8 **) m_pEnd;
    *ppNextSegmentPtr = NULL;

	ThreadMemory(m_pHeap);

	return IFX_OK;
}




IFXRESULT IFXSharedUnitAllocator::Destroy()
{
	// Delete allocated memory - 2 Steps
	//    Step 1: Delete each "grown" memory chunk
	//    Step 2: Delete initially allocated chunk
	
	IFXRESULT result = IFX_OK;

    if( m_pHeap )
	{
		// Delete first segment:
		U8  *pNextSegment = * (U8 **)(m_pHeap + m_firstSegmentSize);
		if (m_pHeap) IFXDELETE(m_pHeap);
		m_pHeap = NULL;

		U32 i;

		// Now delete grown segments if they exist:
		for (i = 0; i < m_uNumGrownSegments; i++)
		{
			if (pNextSegment)
			{
				U8 *pCurrentSegment = pNextSegment;
				pNextSegment = * (U8 **)(pCurrentSegment + m_growSegmentSize);

				IFXDELETE(pCurrentSegment);
			}
		}

		m_uNumGrownSegments = 0;
		m_pFree = NULL;
		m_pEnd  = NULL;
	}
	else result = IFX_E_UNDEFINED;
	return result;
}


IFXRESULT IFXSharedUnitAllocator::ThreadMemory(U8 *pPtr)
{
	IFXRESULT result = IFX_OK;

	if( pPtr )
	{
		U8 *tmpPtr;

		for(; pPtr < m_pEnd; )
		{
			// Get Next Block Offset
			tmpPtr = pPtr + m_unitSize;

			// Put NextBlock address into first 4 bytes of pPtr
		   *((U8 **) pPtr) = (U8 *)tmpPtr;

			pPtr += m_refCountOffset;
		   *((U8 **)pPtr) = 0;
			pPtr += sizeof(U8*);
		}
	}
	else result = IFX_E_INVALID_POINTER;
	return result;
}


U8 * IFXSharedUnitAllocator::Grow()
{
	// Allocate new memory
    U8  *pNewSegment = (U8 *)(new U8[m_growSegmentSize + sizeof(U8*)]);
    if (!pNewSegment) return NULL;

    // Increment grown segment count and free list unit count:
    m_uNumGrownSegments++;
    m_freeListSize += m_numGrowUnits;

	// Make m_pEnd point to new segment address
    *((U8 **)m_pEnd) = pNewSegment;
        
    m_pFree = pNewSegment;
    m_pEnd  = pNewSegment + m_growSegmentSize;

    // Set last segment pointer to Null:
    U8 **ppNextSegmentPtr = (U8 **) m_pEnd;
    *ppNextSegmentPtr = NULL;

	// Thread the new segment in:
	ThreadMemory(m_pFree);

	return m_pFree;
}
