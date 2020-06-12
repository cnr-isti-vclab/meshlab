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
	@file	IFXMemory.cpp

			This module defines a memory abstraction layer.  It's used to funnel
			memory related services (allocation, deallocation and reallocation)
			through a common point that can be controlled for tracking or porting
			purposes.  It also overloads the default C++ new, new[], delete and
			delete[] operators so that the memory abstraction layer is used.

	@note	Eventually at some point, add to the debug version, code to report 
			total memory usage, amount of current memory allocated, etc.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include <stdlib.h>
#include "IFXMemory.h"

//***************************************************************************
//	Global data
//***************************************************************************

IFXAllocateFunction		*gs_pAllocateFunction	= malloc;
IFXDeallocateFunction	*gs_pDeallocateFunction	= free;
IFXReallocateFunction	*gs_pReallocateFunction	= realloc;

//***************************************************************************
//	Global functions
//***************************************************************************

//---------------------------------------------------------------------------
extern "C"
void* IFXAPI IFXAllocate( size_t byteCount )
{
	return gs_pAllocateFunction( byteCount );
}

//---------------------------------------------------------------------------
extern "C"
void IFXAPI IFXDeallocate( void* pMemory )
{
	gs_pDeallocateFunction( pMemory );
}

//---------------------------------------------------------------------------
extern "C"
void* IFXAPI IFXReallocate( void* pMemory, size_t byteCount )
{
	return gs_pReallocateFunction( pMemory, byteCount );
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXGetMemoryFunctions(    
                            IFXAllocateFunction**	ppAllocateFunction,
							IFXDeallocateFunction**	ppDeallocateFunction,
							IFXReallocateFunction**	ppReallocateFunction )
{
	if ( ppAllocateFunction )
		*ppAllocateFunction = gs_pAllocateFunction;

	if ( ppDeallocateFunction )
		*ppDeallocateFunction = gs_pDeallocateFunction;

	if ( ppReallocateFunction )
		*ppReallocateFunction = gs_pReallocateFunction;

	return IFX_OK;
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXSetMemoryFunctions(    
                            IFXAllocateFunction*	pAllocateFunction,
			                IFXDeallocateFunction*	pDeallocateFunction,
			                IFXReallocateFunction*	pReallocateFunction )
{
	IFXRESULT result = IFX_OK;

	if ( pAllocateFunction && pDeallocateFunction && pReallocateFunction )
	{
		gs_pAllocateFunction	= pAllocateFunction;
		gs_pDeallocateFunction	= pDeallocateFunction;
		gs_pReallocateFunction	= pReallocateFunction;
	}
	else if ( !pAllocateFunction && !pDeallocateFunction && !pReallocateFunction )
	{
		gs_pAllocateFunction	= malloc;
		gs_pDeallocateFunction	= free;
		gs_pReallocateFunction	= realloc;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

