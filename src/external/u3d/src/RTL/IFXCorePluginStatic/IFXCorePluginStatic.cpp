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
	@file	IFXCorePluginStatic.cpp

			This module overrides the default C++ memory operators.
			It is part of IFXCorePluginStatic library which defines common
			functionality for all extension plug-ins.
*/


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXMemory.h"
#include <new>


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

/**
	Count of active objects.
	This counter is used by IFXPluginCanUnloadNow.
	If this counter equals 0 it means there is no active objects allocated by 
	plug-in and plug-in can be successfully unloaded.
*/
U32 g_countActiveObjects = 0;
// const size_t maxObjects = 1024;
// size_t numObjects = 0;
// void* objects[maxObjects];


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


/**
	Overloaded global memory operator.

	@note	Every plug-in DL should be statically linked to the IFXCore
			dynamic library and can use IFXAllocate, IFXDeallocate and
			IFXReallocate from it.  Memory operators are overloaded here for
			the plug-in DL by using IFXAllocate and IFXDeallocate.  Thus
			plug-in DL uses the same memory functions as IFXCore uses.
*/
void operator delete( void* pMemory ) throw()
{
	if ( NULL != pMemory )
	{
		if( 0 == g_countActiveObjects )
		{
			IFXTRACE_GENERIC(L"[IFXCorePluginStatic]: g_countActiveObjects == 0\n");
		}

//		for (size_t i=0; i<numObjects; i++)
//			if (objects[i] == pMemory)
//				objects[i] = NULL;
		
		--g_countActiveObjects;

		IFXDeallocate( pMemory );
	}
}

/**
	Overloaded global memory operator.

	@note	Every plug-in DL should be statically linked to the IFXCore
			dynamic library and can use IFXAllocate, IFXDeallocate and
			IFXReallocate from it.  Memory operators are overloaded here for
			the plug-in DL by using IFXAllocate and IFXDeallocate.  Thus
			plug-in DL uses the same memory functions as IFXCore uses.
*/
void operator delete[]( void* pMemory ) throw()
{
	operator delete( pMemory );
}

/**
	Overloaded global memory operator.

	@note	Every plug-in DL should be statically linked to the IFXCore
			dynamic library and can use IFXAllocate, IFXDeallocate and
			IFXReallocate from it.  Memory operators are overloaded here for
			the plug-in DL by using IFXAllocate and IFXDeallocate.  Thus
			plug-in DL uses the same memory functions as IFXCore uses.
*/
void* operator new( size_t byteCount )
#ifndef _MSC_VER
	throw(std::bad_alloc)
#endif
{
	void* p = NULL;

	p = IFXAllocate( byteCount );

//	if (numObjects>412 && numObjects<418)
//		fprintf(stderr,"check\n");
//	if (numObjects < maxObjects)
//		objects[numObjects++] = p;

	if( NULL != p )
		++g_countActiveObjects;

	return p;
}

/**
	Overloaded global memory operator.

	@note	Every plug-in DL should be statically linked to the IFXCore
			dynamic library and can use IFXAllocate, IFXDeallocate and
			IFXReallocate from it.  Memory operators are overloaded here for
			the plug-in DL by using IFXAllocate and IFXDeallocate.  Thus
			plug-in DL uses the same memory functions as IFXCore uses.
*/
void* operator new[]( size_t byteCount )
#ifndef _MSC_VER
	throw(std::bad_alloc)
#endif
{
	return operator new( byteCount );
}
