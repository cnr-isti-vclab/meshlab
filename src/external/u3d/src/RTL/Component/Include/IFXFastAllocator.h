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
	@file	IFXFastAllocator.h

			This header defines the IFXFastAllocator class and its functionality.
*/

#ifndef IFXFastAllocator_h
#define IFXFastAllocator_h

#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXDebug.h"

/**
	This is the main interface and definition for IFXFastAllocator.
*/
class IFXFastAllocator
{
public:
	IFXFastAllocator();
	virtual ~IFXFastAllocator();

	/**
		This method initializes an IFXFastAllocator object.

		@param	initialSize		A U32 representing initial memory allocated
								to the object.
		@param	growSize		A U32 representing the memory allocated to the 
								object once the inital memory is exhausted.

		@return	IFXRESULT
		
		  - @b IFX_OK				-	No error.\n
		  - @b IFX_E_OUT_OF_MEMORY	-	Memory allocation failure.
	*/	
	IFXRESULT Initialize(U32 initialSize, U32 growSize);

	/**
		This method frees all memory associated with the IFXFastAllocator object.

		@return	void
	*/	
	void FreeAll();

	/**
		This method allocates a new chuck of memory and returns a pointer to it.

		@param	size	A U32 representing the requested memory size.

		@return	U8 pointer to the allocated memory.
	*/	
	U8* Allocate(U32 size);

private:
	U8* GrowThenAllocate(U32 size);

	U32 m_initialSize;
	U32 m_growSize;
	U8* m_pHeap;            // Pointer to first allocated memory chunk
	U8* m_pFree;            // Pointer to available memory 
	U8* m_pEnd;				// Pointer to next allocated chunk
};




inline U8* IFXFastAllocator::Allocate(U32 size)
{
	IFXASSERT((size & 0x00000003) == 0x00000000);	// Warn about alignment issues
	U8 *pPtr = m_pFree;
	m_pFree += size;
	if (m_pFree <= m_pEnd)
		return pPtr;
	else
		return GrowThenAllocate(size);
}

#endif
