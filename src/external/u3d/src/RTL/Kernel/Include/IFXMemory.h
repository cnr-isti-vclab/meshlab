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
	@file	IFXMemory.h

			This module defines a memory abstraction layer.  It's used to funnel
			memory related services (allocation, deallocation and reallocation)
			through a common point that can be controlled for tracking or porting
			purposes.  It also overloads the default C++ new, new[], delete and
			delete[] operators so that the memory abstraction layer is used.

	@todo	Clarify NULL and 0 argument handling.  Delete must accept NULL.  
			Free should also, but it may not be supported everywhere
			it is not clear if new of array size 0 is absolutely permitted
			malloc(0) should be valid, but it may not be supported everywhere
			These issues should be clarified and implemented.
*/

#ifndef IFXMemory_h
#define IFXMemory_h

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "IFXDebug.h"
#include "IFXResult.h"

//***************************************************************************
//  Classes, structures and types
//***************************************************************************

extern "C"
{
	typedef void*	( IFXAllocateFunction )( size_t byteCount );
	typedef void	( IFXDeallocateFunction )( void* pMemory );
	typedef void*	( IFXReallocateFunction )( void* pMemory, size_t byteCount );
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************

//---------------------------------------------------------------------------
/**
	This function is used to allocate a block of memory.  If successful, a
	pointer to the memory block is returned.  Otherwise, enough memory isn't
	available and NULL is returned.
*/
extern "C"
void* IFXAPI IFXAllocate( size_t byteCount );

//---------------------------------------------------------------------------
/**
	This function is used to deallocate a block of memory previously
	allocated with the IFXDeallocate function.
*/
extern "C"
void IFXAPI IFXDeallocate( void* pMemory );

//---------------------------------------------------------------------------
/**
	This function is used to reallocate a block of memory previously
	allocated by the IFXAllocate function using the new byte count.  If
	successful, a pointer to the reallocated memory block is returned.  It
	may not be the same pointer passed to the function, however the contents
	of the memory block will mirror the original memory block (they will be
	truncated if a smaller block is requested or undefined after the original
	data ends if a larger block is requested).  Otherwise, enough memory
	isn't available and NULL is returned.
*/
extern "C"
void* IFXAPI IFXReallocate( void* pMemory, size_t byteCount );

//---------------------------------------------------------------------------
/**
	This function is used to get pointers to the three main memory functions.
	Specify NULL for any function pointer that is not desired.  IFX_OK is
	always returned.
*/
extern "C"
IFXRESULT IFXAPI IFXGetMemoryFunctions(
					IFXAllocateFunction**	ppAllocateFunction,
					IFXDeallocateFunction**	ppDeallocateFunction,
					IFXReallocateFunction**	ppReallocateFunction );

//---------------------------------------------------------------------------
/**
	This function is used to replace the three main memory functions.
	Specify NULL for all three function pointers to restore the default
	functions.  Upon success, IFX_OK is returned.  Otherwise,
	IFX_E_INVALID_POINTER is returned.
*/
extern "C"
IFXRESULT IFXAPI IFXSetMemoryFunctions(
					IFXAllocateFunction*	pAllocateFunction,
					IFXDeallocateFunction*	pDeallocateFunction,
					IFXReallocateFunction*	pReallocateFunction );

#endif
