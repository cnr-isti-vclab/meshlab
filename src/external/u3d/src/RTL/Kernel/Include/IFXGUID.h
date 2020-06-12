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
	@file	IFXGUID.h

			This header defines the core IFXCOM functionality.

	@note	Interface and component templates are available for use when
			creating new ones.  Also available is a guidelines document that
			describes both create and usage rules.
*/


#ifndef IFXGUID_H
#define IFXGUID_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "IFXAPI.h"

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************
//---------------------------------------------------------------------------

/**
	This is the IFXCOM version of a globally unique identifier.  It's
	defined the same as a Microsoft COM GUID except C doesn't supported it.
*/
struct IFXGUID
{
	U32	A;
	U16	B,
		C;
	U8	D[ 8 ];
};

//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of a component identifier.  It's
	defined the same as a Microsoft COM CLSID except C doesn't supported it.
*/
typedef IFXGUID IFXCID;

//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of an interface identifier.  It's
	defined the same as a Microsoft COM IID except C doesn't supported it.
*/
typedef IFXGUID IFXIID;

//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of an DataElement identifier.  It's
	defined the same as a Microsoft COM IID except C doesn't supported it.
*/
typedef IFXGUID IFXDID; 

//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of an plug-in identifier.  It's
	defined the same as a Microsoft COM IID except C doesn't supported it.
*/
typedef IFXGUID IFXPID; 


//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of a component identifier reference.
	It's defined the same as a Microsoft COM REFCLSID except C doesn't
	supported it.
*/
typedef const IFXCID&	IFXREFCID;

//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of an interface identifier reference.
	It's defined the same as a Microsoft COM REFIID except C doesn't support
	it.
*/
typedef const IFXIID&	IFXREFIID;


//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of a generic identifier reference.
*/
typedef const IFXGUID&	IFXREFGUID;


//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of a DataElement identifier reference.
*/
typedef const IFXGUID&  IFXREFDID;

//---------------------------------------------------------------------------
/**
	This is the IFXCOM version of a plug-in identifier reference.
*/
typedef const IFXGUID&  IFXREFPID;

#undef IFXDEFINE_GUID

#ifndef IFX_INIT_GUID
#define IFXDEFINE_GUID( name, a0, b0, c0, d0, d1, d2, d3, d4, d5, d6, d7 )	\
		extern "C" const IFXGUID name
#else
#define IFXDEFINE_GUID( name, a0, b0, c0, d0, d1, d2, d3, d4, d5, d6, d7 )	\
		extern "C" const IFXGUID name = { a0, b0, c0, { d0, d1, d2, d3, d4, d5, d6, d7 } }
#endif


//***************************************************************************
//	Interfaces and interface identifiers
//***************************************************************************


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Inline functions
//***************************************************************************

// remove MS warning: unreferenced inline function has been removed
#ifdef _MSC_VER
#pragma warning( disable : 4514 )
#endif

//---------------------------------------------------------------------------
/**
	This is an equivalence operator for comparing IFXGUIDs.
*/
inline BOOL IFXAPI operator==( const IFXGUID& guidA, const IFXGUID& guidB )
{
   return (
      ((U32) guidA.A) == ((U32) guidB.A) &&
      ((U16) guidA.B) == ((U16) guidB.B) &&
      ((U32) guidA.C) == ((U32) guidB.C) &&
      ((U32) guidA.D[0]) == ((U32) guidB.D[0]) &&
      ((U32) guidA.D[1]) == ((U32) guidB.D[1]) &&
      ((U32) guidA.D[2]) == ((U32) guidB.D[2]) &&
      ((U32) guidA.D[3]) == ((U32) guidB.D[3]) &&
      ((U32) guidA.D[4]) == ((U32) guidB.D[4]) &&
      ((U32) guidA.D[5]) == ((U32) guidB.D[5]) &&
      ((U32) guidA.D[6]) == ((U32) guidB.D[6]) &&
      ((U32) guidA.D[7]) == ((U32) guidB.D[7]));
}

#endif

