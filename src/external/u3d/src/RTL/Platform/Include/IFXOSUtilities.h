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
	@file	IFXOSUtilities.h

			This header defines an abstraction layer for general OS specific
			utilities.  The implementation of these utilities resides in a source
			file named after the OS.  To easily identify functions belonging to
			this module, all function names have been prefixed with "IFXOS_".

	@note	To port to another operating system, rewrite the all of the IFXOS*.h
			services.
			Do not put any OS specific includes or types in this header!

*/

#ifndef IFXOSUtilities_h
#define IFXOSUtilities_h

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDebug.h"

//***************************************************************************
//	Enumerations
//***************************************************************************

enum EIFXCPUFeature 
{ 
	IFXCPUFeature_MMX, 
	IFXCPUFeature_SSE, 
	IFXCPUFeature_SSE2, 
	IFXCPUFeature_SSE3 
};

//***************************************************************************
//  Global function prototypes
//***************************************************************************

/**
	This function retrieves the language identifier of the system locale.
*/
extern "C"
U16	IFXAPI_CALLTYPE IFXGetSystemDefaultLangID(void);

/**
*/
extern "C"
BOOL IFXAPI_CALLTYPE IFXOSCheckCPUFeature(EIFXCPUFeature feature);

/**
	This function is used to initialize any OS-specific subsystems. It
	must be called during system startup, before any of the other IFXOS
	functions are called.
*/
extern "C"
void IFXAPI_CALLTYPE IFXOSInitialize( void );

/**
	This function must be called before shutting the system down, to
	release any OS-specific resources.
*/
extern "C"
void IFXAPI_CALLTYPE IFXOSUninitialize( void );

/**
	This function is used to get the current system time in milliseconds.
*/
extern "C"
U32 IFXAPI_CALLTYPE IFXOSGetTime( void );

/**
	This function is used to put the current thread to sleep for the
	specified number of milliseconds (at a minimum).
    
	@note	A value of zero causes the thread to relinquish the remainder
			of its time slice to any other thread of equal priority that is
			ready to run. If there are no other threads of equal priority
			ready to run, the function returns immediately, and the thread
			continues execution.
*/
extern "C"
void IFXAPI_CALLTYPE IFXOSSleep( U32 milliseconds );

extern "C"
U32 IFXAPI_CALLTYPE IFXOSControlFP( U32 value, U32 mask );

#ifdef _DEBUG

/**
	This function is used to get the current debug level.
*/
extern "C"
IFXDebugLevel IFXAPI_CALLTYPE IFXOSGetDebugLevel( void );

/**
	This function is used to display a debug message to the standard output
	debug text window.
*/
extern "C"
void IFXAPI_CALLTYPE IFXOSOutputDebugString( const IFXCHAR* pMessage );

#endif

/**
	Returns the the required size of the destination UTF-8 string in bytes 
	excluding null terminator.

	@param	pWideCharString		Null-terminated wide-char string.

	@param	pUtf8StringSize		Size of required destination UTF-8 buffer.

	@return Upon success, IFX_OK is returned.  Otherwise, one of the
	following failures are returned: IFX_E_UNDEFINED, IFX_E_INVALID_POINTER.
*/
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSGetUtf8StrSize( 
									const IFXCHAR* pWideCharString, 
									U32* pUtf8StringSize );

/**
	Converts a wide-char string to a UTF-8 string.

	@param	pWideCharString		Null-terminated wide-char string.

	@param	pUtf8String			Destination UTF-8 buffer.

	@param	utf8StringSize		Size of destination UTF-8 buffer.

	@return Upon success, IFX_OK is returned.  Otherwise, one of the
	following failures are returned: IFX_E_BAD_PARAM, IFX_E_UNDEFINED, 
	IFX_E_INVALID_POINTER.
*/
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSConvertWideCharStrToUtf8( 
									const IFXCHAR* pWideCharString,
									U8* pUtf8String, 
									U32 utf8StringSize );

/**
	Returns the the required size of the destination wide-char string in bytes
	excluding null terminator.

	@param	pUtf8String				Null-terminated UTF-8 string.

	@param	pWideCharStringSize		Size of required destination wide-char buffer.

	@return Upon success, IFX_OK is returned.  Otherwise, one of the
	following failures are returned: IFX_E_UNDEFINED, IFX_E_INVALID_POINTER.
*/
extern "C"
IFXRESULT IFXAPI_CALLTYPE	IFXOSGetWideCharStrSize( 
									const U8* pUtf8String, 
									U32* pWideCharStringSize );

/**
	Converts a UTF-8 string to a wide-char string.

	@param	pUtf8String				Null-terminated UTF-8 string.

	@param	pWideCharString			Destination wide-char buffer.

	@param	wideCharStringSize		Size of destination wide-char buffer.

	@return Upon success, IFX_OK is returned.  Otherwise, one of the
	following failures are returned: IFX_E_BAD_PARAM, IFX_E_UNDEFINED, 
	IFX_E_INVALID_POINTER.
*/
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSConvertUtf8StrToWideChar( 
									const U8* pUtf8String, 
									IFXCHAR* pWideCharString, 
									U32 wideCharStringSize );

/**
	Performs debug output (message box on Windows, console output
	on Linux).

	@param	pMessage				Description of event.

	@param	pFile					The file in which the event occured.

	@param	lineNum					Line number in the file.

	@param	pExpression				Source code of the event.

	@return Can return different values depending on user's decision on
	the way to handle the event (for example, on Windows it can be
	Abort, Retry or Ignore).
*/
extern "C"
U32 IFXAPI_CALLTYPE IFXOSDebugOutput( 
									IFXCHAR* pMessage,
									IFXCHAR* pFile,
									U32 lineNum,
									IFXCHAR* pExpression );

/**
	Set recommended locale for using unicode data.

	@return Upon success, IFX_OK is returned.
	Otherwise IFX_E_UNDEFINED is returned.
*/
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXSetDefaultLocale( void );

#endif
