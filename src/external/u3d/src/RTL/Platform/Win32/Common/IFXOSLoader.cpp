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
	@file	IFXOSLoader.cpp

			This module contains the Windows specific functions required to
			work with dynamically liked library.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXOSLoader.h"
#include <wtypes.h>

#if __MINGW32__
#define swprintf _snwprintf
#endif

//***************************************************************************
//	Local data
//***************************************************************************

static const IFXCHAR* s_pCoreDlName = L"IFXCore.dll";
static const IFXCHAR* s_pOpenGLDlName = L"OPENGL32";

const U32 MAXPATHLEN = 512;

//***************************************************************************
//	Global functions
//***************************************************************************

extern "C"
IFXHANDLE IFXAPI IFXLoadLibrary( const IFXCHAR* pFileName )
{
    HINSTANCE hinst = LoadLibrary( pFileName );

	if (!hinst && 
		( 	!wcscmp( pFileName, L"IFXExporting.dll"  ) || 
			!wcscmp( pFileName, L"IFXImporting.dll"  ) || 
			!wcscmp( pFileName, L"IFXScheduling.dll" ) || 
			!wcscmp( pFileName, L"IFXRendering.dll"  ) ) )
	{
		wchar_t fileName[MAXPATHLEN];
		swprintf( fileName, MAXPATHLEN, L"Plugins\\%ls", pFileName );
		hinst = LoadLibrary( fileName );
	}

	if (!hinst &&
		(	!wcscmp( pFileName, s_pCoreDlName )        ||
		 	!wcscmp( pFileName, L"IFXExporting.dll"  ) ||
			!wcscmp( pFileName, L"IFXImporting.dll"  ) ||
			!wcscmp( pFileName, L"IFXScheduling.dll" ) ||
			!wcscmp( pFileName, L"IFXRendering.dll"  ) ))
	{
		wchar_t fileName[MAXPATHLEN];
		wchar_t corePath[MAXPATHLEN];
		if(GetEnvironmentVariable(L"U3D_LIBDIR", corePath, MAXPATHLEN))
		{
			swprintf( fileName, MAXPATHLEN, L"%ls\\%ls", corePath, pFileName );
			hinst = LoadLibrary( fileName );
		}
	}

	if (!hinst &&
		( 	!wcscmp( pFileName, L"IFXExporting.dll"  ) || 
			!wcscmp( pFileName, L"IFXImporting.dll"  ) || 
			!wcscmp( pFileName, L"IFXScheduling.dll" ) || 
			!wcscmp( pFileName, L"IFXRendering.dll"  ) ) )
	{
		wchar_t fileName[MAXPATHLEN];
		wchar_t corePath[MAXPATHLEN];
		if(GetEnvironmentVariable(L"U3D_LIBDIR", corePath, MAXPATHLEN))
		{
			swprintf( fileName, MAXPATHLEN, L"%ls\\Plugins\\%ls", corePath, pFileName );
			hinst = LoadLibrary( fileName );
		}
	}

	return (IFXHANDLE)hinst;
}

extern "C"
IFXHANDLE IFXAPI IFXLoadCoreLibrary()
{
    return IFXLoadLibrary( s_pCoreDlName );
}

extern "C"
IFXHANDLE IFXAPI IFXLoadOpenGLLibrary()
{
    return IFXLoadLibrary( s_pOpenGLDlName );
}

extern "C"
IFXRESULT IFXAPI IFXOSGetCoreLibraryPath( IFXString* pPath )
{
    IFXRESULT result = IFX_OK;

	IFXCHAR pluginLocation[MAXPATHLEN] = {0};

	HMODULE handle = GetModuleHandle( s_pCoreDlName );
	if( NULL != handle )
	{
		if( 0 == GetModuleFileName( handle, pluginLocation, MAXPATHLEN ) )
		{
			result = IFX_E_UNDEFINED;
		}
	}
	else
		result = IFX_E_INVALID_FILE;

	// remove IFXCore.dll from the end
	if( IFXSUCCESS( result ))
	{
		IFXString fullPath( pluginLocation ), pPath;
		U32 index = 0;

		fullPath.FindSubstring( s_pCoreDlName, &index );
		fullPath.Substring( &pPath, 0, index );
	}

    return result;
}

extern "C"
IFXOSFUNC IFXAPI IFXGetAddress( const IFXHANDLE handle, const char* pFuncName )
{
    return (IFXOSFUNC)GetProcAddress( (HINSTANCE)handle, pFuncName );
}

extern "C"
IFXRESULT IFXAPI IFXReleaseLibrary( const IFXHANDLE handle )
{
    IFXRESULT result = IFX_OK;

    if( 0 == FreeLibrary( (HINSTANCE)handle ) )
    {
        result = IFX_E_NOT_DONE;
    }

    return result;
}

extern "C"
FILE* IFXAPI IFXOSFileOpen(const wchar_t *filename, const wchar_t *mode)
{
	FILE *pFile = NULL;

#ifdef STDIO_HACK
	if (!wcscmp(filename,"stdin") || (!wcscmp(filename,"-")  && *mode=='r')) {
		pFile = stdin;
	} else if (!wcscmp(filename,"stdout") || (!wcscmp(filename,"-")  && *mode=='w')) {
		pFile = stdout;
		_setmode(_fileno(stdout),_O_BINARY);
	} else {
		pFile = _wfopen(filename, mode);
	}
#else
	pFile = _wfopen(filename, mode);
#endif

	return pFile;
}
