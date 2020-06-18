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
	@file	IFXAPI.h

			This module defines a variety of macros used for exporting 
			public functions and interface methods.  These include IFXAPI, 
			IFXAPI_CALLTYPE, IFXAPI_FASTCALLTYPE and IFXAPI_STDCALLTYPE.
*/

#ifndef IFXAPI_H
#define IFXAPI_H


//***************************************************************************
//	Defines
//***************************************************************************


//---------------------------------------------------------------------------
/**
	Defines the fast calling convention that can vary with compiler.
*/
#if defined( WIN32 )
	#if defined ( _MSC_VER ) && ( _MSC_VER >= 1020 )
		#define IFXAPI_FASTCALLTYPE	__fastcall
	#else
		#define IFXAPI_FASTCALLTYPE
    #endif
#else
	#define IFXAPI_FASTCALLTYPE
#endif

//---------------------------------------------------------------------------
/**
	Defines the standard calling convention that can vary with compiler.
*/
#if defined( WIN32 )
	#if defined ( _MSC_VER ) && ( _MSC_VER >= 1020 )
		#define IFXAPI_STDCALLTYPE	__stdcall
	#else
		#define IFXAPI_STDCALLTYPE
    #endif
#else
	#define IFXAPI_STDCALLTYPE
#endif

//---------------------------------------------------------------------------
/**
	Defines the calling convention for exported functions and interface 
	methods. It can vary with compiler.
*/
#if defined( WIN32 )
	#if defined ( _MSC_VER ) && ( _MSC_VER >= 1020 )
		#define IFXAPI_CALLTYPE		IFXAPI_FASTCALLTYPE
	#else
		#define IFXAPI_CALLTYPE
    #endif
#else
	#define IFXAPI_CALLTYPE
#endif

//---------------------------------------------------------------------------
/**
	Defines the macro used in the prototype declaration and implementation 
	of exported functions and interface methods.
*/
#define IFXAPI	IFXAPI_CALLTYPE


#endif


