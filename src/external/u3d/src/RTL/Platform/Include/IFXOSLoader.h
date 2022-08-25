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
	@file	IFXOSLoader.h

			This module provides all required functionality to work with dynamic
			libraries and their analogs.

	@note	Do not put any OS specific includes or types in this header!
*/

#ifndef IFXOSLoader_H
#define IFXOSLoader_H

#include "IFXAPI.h"
#include "IFXResult.h"
#include "IFXString.h"

/**
	Pointer to a function obtained from the dynamically linked library.
*/
typedef IFXRESULT (*IFXOSFUNC)();

/**
	Load the dynamically linked library with path specified.

	@param  pFileName	Pointer to a wide-character name of the dynamically 
						linked library.

	@return Upon success, it returns the handle for loaded library. 
			Otherwise, it will return NULL.
*/ 
extern "C"
IFXHANDLE IFXAPI IFXLoadLibrary( const IFXCHAR* pFileName );

/**
	Load the dynamically linked IFXCore library.

	@return Upon success, it returns the handle for loaded IFXCore library. 
			Otherwise, it will return NULL.
*/ 
extern "C"
IFXHANDLE IFXAPI IFXLoadCoreLibrary();

/**
	Load the dynamically linked OpenGL library.

	@return Upon success, it returns the handle for loaded OpenGL library. 
			Otherwise, it will return NULL.
*/ 
extern "C"
IFXHANDLE IFXAPI IFXLoadOpenGLLibrary();

/**
	Retrieve the fully qualified path for the IFXCore library.

	@param  pPath Path for IFXCore.

	@return Upon success, it returns the value IFX_OK. 
			Otherwise, it will return the value IFX_E_UNDEFINED.
*/
extern "C"
IFXRESULT IFXAPI IFXOSGetCoreLibraryPath( IFXString* pPath );

/**
	Obtain the address of a function from the dynamically linked library.

	@param  handle Handle for loaded library.
	@param  pFuncName Pointer to a name of the obrained function.

	@return Upon success, it returns the pointer of function. 
			Otherwise, it will return NULL.
*/
extern "C"
IFXOSFUNC IFXAPI IFXGetAddress( const IFXHANDLE handle, const char* pFuncName );

/**
	Unloads the dynamically linked library.

	@return Upon success, it returns the value IFX_OK. Otherwise, it will
			return the value IFX_E_NOT_DONE.
*/
extern "C"
IFXRESULT IFXAPI IFXReleaseLibrary( const IFXHANDLE handle );

/**
	Parameters
		filename 
			Filename. 
		mode 
			Type of access permitted. 

	Return a pointer to the open file. A null pointer value indicates an error.
*/
extern "C"
FILE* IFXAPI IFXOSFileOpen(const wchar_t *filename, const wchar_t *mode);

#endif

