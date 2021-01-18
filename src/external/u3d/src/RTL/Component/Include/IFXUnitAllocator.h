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

/**
	@file	IFXUnitAllocator.h

			This module defines the IFXUnitAllocator class.  It provides
			a "fast" alternative to the standard NEW and DELETE operators provided
			by C++.  The allocator doles out "unitSize" chunks of memory, manages a
			freelist, and grows by "growSize" when the current memory has been 
			exhausted.

	@note	Since IFXUnitAllocator::Allocate() does not call new(),
			it does not call any constructors.  If your pointers are to
			classes, you will presumably have to supply and call you own
			function to initialize the class.
*/

#ifndef IFXUNITALLOCATOR_DOT_H
#define IFXUNITALLOCATOR_DOT_H

#include "IFXDataTypes.h"
#include "IFXResult.h"

/**
	This class provides a fast alternative to the standard NEW and DELETE 
	operators provided by C++.  The allocator doles out "unitSize" chunks of memory, 
	manages a freelist, and grows by "growSize" when the current memory has been 
	exhausted.
*/
class IFXUnitAllocator
{
public:

	IFXUnitAllocator();
	~IFXUnitAllocator();

	IFXRESULT  Initialize(U32 unitSize, U32 numUnits, U32 numGrowUnits);
	void       Deallocate(U8* pPtr);
	U32        SizeList();
	U32        NumAllocatedUnits();
	U32		   NumFreeUnits();
	U8        *Allocate();

private:

	U8  *m_pHeap;             ///< Pointer to allocated memory chunk
	U8	*m_pFree;             ///< Pointer to first element of free list
	U8	*m_pEnd;			  ///< Always points to the "next" pointer at end of LAST chuck

	U32  m_unitSize;          ///< Size in bytes of each "Unit"
	U32	 m_numUnits;          ///< Number of units in initial allocation
	U32	 m_numGrowUnits;      ///< Grow by this many "Units" when memory is exhausted
    U32  m_firstSegmentSize;  ///< Size in buytes of first segment
    U32  m_growSegmentSize;   ///< Size in bytes of grow segment
	U32	 m_freeListSize;      ///< Num objects on free list
	U32	 m_allocListSize;     ///< Num allocated objects
    U32  m_uNumGrownSegments; ///< Number of segments grown dynamically

	IFXRESULT  ThreadMemory(U8 *pPtr);
    IFXRESULT  Destroy();
	U8        *Grow();
};


IFXINLINE U32 IFXUnitAllocator::SizeList()
{ 
	return (m_allocListSize + m_freeListSize); 
}

IFXINLINE U32 IFXUnitAllocator::NumAllocatedUnits()
{ 
	return (m_allocListSize); 
}

IFXINLINE U32 IFXUnitAllocator::NumFreeUnits()
{ 
	return (m_freeListSize); 
}

IFXINLINE U8 *IFXUnitAllocator::Allocate()
{
	U8 *pPtr = m_pFree;

	if( m_freeListSize == 0 )
	{
		// Memory exhausted - Need to allocate new memory
		if( !(pPtr = Grow()) ) return NULL;
	}

	// Retrieve the next free block from first 4 bytes of pPtr
	m_pFree = (U8 *) *((U8**) pPtr);

	m_freeListSize --;
	m_allocListSize ++;

	return pPtr;
}


IFXINLINE void IFXUnitAllocator::Deallocate(U8 *pPtr)
{
	if( m_freeListSize == 0 )
	{
		// If FreeList is empty, simply point m_pFree to newly freed node.
		m_pFree = pPtr;
	}
	else
	{
		// If FreeList is empty, point newly freed block to what m_pFree
		// was pointing to and set m_pFree to the new node.
		// Put NextBlock address into first 4 bytes of pPtr
		U8 *tmpPtr = m_pFree;

		*((U8**) pPtr) = (U8*)tmpPtr;	
		m_pFree = pPtr;
	}

	m_freeListSize++;
	m_allocListSize--;
}


#endif
