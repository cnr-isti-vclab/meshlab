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
U32 g_countActiveRenderingObjects = 0;


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
void operator delete( void* pMemory )
{
	if ( NULL != pMemory )
	{
		if( 0 == g_countActiveRenderingObjects )
		{
			IFXTRACE_GENERIC(L"[IFXCorePluginStatic]: g_countActiveRenderingObjects == 0\n");
		}

		--g_countActiveRenderingObjects;

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
void operator delete[]( void* pMemory )
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
{
	void* p = NULL;

	p = IFXAllocate( byteCount );

	if( NULL != p )
		++g_countActiveRenderingObjects;

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
{
	return operator new( byteCount );
}
