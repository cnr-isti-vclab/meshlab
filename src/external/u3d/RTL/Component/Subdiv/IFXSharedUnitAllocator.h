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
//	IFXSharedUnitAllocator.h
//
//	DESCRIPTION
//		This module defines the IFXSharedUnitAllocator class.  It provides
//	an "fast" alternative to the standard NEW and DELETE operators provided
//	by C++.  The allocator doles out "unitSize" chunks of memory, manages a
//	freelist, grows by "growSize" when the current memory has been exhausted,
//	and provides a per object reference counter.
//
//	NOTES
//
//***************************************************************************

#ifndef IFXSHAREDUNITALLOCATOR_DOT_H
#define IFXSHAREDUNITALLOCATOR_DOT_H

#include <memory.h>
#include "IFXResult.h"
#include "IFXDataTypes.h"
#include "IFXDebug.h"

#define  ALREADYDEALLOCATED   0xffffffff;
/**This module defines the IFXSharedUnitAllocator class. It provides
a fast alternative to the standard NEW and DELETE operators in C++.  
The allocator doles out "unitSize" chunks of memory, manages a
freelist, grows by "growSize" when the current memory has been exhausted,
and provides a reference counter for each object.*/
class IFXSharedUnitAllocator
{

public:

	IFXSharedUnitAllocator();
	~IFXSharedUnitAllocator();

	IFXRESULT  Initialize(U32 unitSize, U32 numUnits, U32 numGrowUnits);
	
	U8      *Allocate();
	void    Deallocate(U8* pPtr);
	U32     IncRefCount(U8 *pPtr);
	U32     SizeList();
	U32		NumAllocatedUnits();
	U32		NumFreeUnits();

private:

	U8  *m_pHeap;             // Pointer to allocated memory chunk
	U8	*m_pFree;             // Pointer to first element of free list
	U8	*m_pEnd;			  // Always points to the "next" pointer at end of LAST chuck

	U32  m_unitSize;          // Size in bytes of each "Unit"
	U32	 m_numUnits;          // Number of units in initial allocation
	U32	 m_numGrowUnits;      // Grow by this many "Units" when memory is exhausted
	U32	 m_refCountOffset;    // Offset in bytes to the ref count
    U32  m_firstSegmentSize;  // Size in buytes of first segment
    U32  m_growSegmentSize;   // Size in bytes of grow segment
	U32	 m_freeListSize;      // Num objects on free list
	U32	 m_allocListSize;     // Num allocated objects
    U32  m_uNumGrownSegments; // Number of segments grown dynamically

	// Private member functions
	IFXRESULT  ThreadMemory(U8 *pPtr);
    IFXRESULT  Destroy();
	U32        DecRefCount(U8 *pPtr);
	U8        *Grow();
};


IFXINLINE U32 IFXSharedUnitAllocator::SizeList()
{ 
	return (m_allocListSize + m_freeListSize); 
}

IFXINLINE U32 IFXSharedUnitAllocator::NumAllocatedUnits()
{ 
	return (m_allocListSize); 
}

IFXINLINE U32 IFXSharedUnitAllocator::NumFreeUnits()
{ 
	return (m_freeListSize); 
}

IFXINLINE U32 IFXSharedUnitAllocator::IncRefCount(U8* pPtr)
{
	// Offset the data pointer to the reference counting storage word:
	pPtr += m_refCountOffset;
   
	// Retrieve the refcount:
	U32 uRefCount = *pPtr;

	// Increment it:
	uRefCount++;

	// Put it back:
	*pPtr = uRefCount;

   return uRefCount;
}


IFXINLINE U32 IFXSharedUnitAllocator::DecRefCount(U8* pPtr)
{
	// OffSet pPtr to refCount position:
	pPtr += m_refCountOffset;

	// Retrieve the reference count:
	U32 uRefCount = *pPtr;

	if( uRefCount == 0 ) 
		return ALREADYDEALLOCATED;

	// decrementing ref count when nothing is
	// allocated is BAD!
	IFXASSERT (m_allocListSize);

	// Decrement it:
	uRefCount--;
	
	// Put the updated reference count back:
	*pPtr = uRefCount;

	return uRefCount;
}


IFXINLINE U8 *IFXSharedUnitAllocator::Allocate()
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

	IncRefCount(pPtr);

	return pPtr;
}


IFXINLINE void IFXSharedUnitAllocator::Deallocate(U8 *pPtr)
{


	U32 refCount = DecRefCount(pPtr);

	if( refCount == 0 )
	{
		// Deallocating a unit when no units are
		// allocated is BAD! 
		IFXASSERT (m_allocListSize);

		if( m_freeListSize == 0 )
		{
			// FreeList is empty, simply point m_pFree to newly freed node
			m_pFree = pPtr;
		}
		else
		{
			// FreeList is !empty, point newly freed block to what m_pFree
			// was pointing to and set m_pFree to the new node
			// Put NextBlock address into first 4 bytes of pPtr
			U8 *tmpPtr = m_pFree;

			//*((NextFreeBlock *) pPtr) = (NextFreeBlock)tmpPtr;	
			*((U8**) pPtr) = (U8*)tmpPtr;	
			m_pFree = pPtr;
		}

#ifdef _DEBUG
		U32 uPreviousAllocListSize = m_allocListSize;
#endif
		m_allocListSize --;
		m_freeListSize ++;

		// Guard against allocated list size underflow:
		IFXASSERT(m_allocListSize < uPreviousAllocListSize);
	}
}

#endif
