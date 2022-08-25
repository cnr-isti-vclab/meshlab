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

#ifdef WIN32
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

#else
/**
  @file IFXOSLoader.cpp

      This module contains the MacOSX specific functions required to
      work with dynamically liked library.
*/

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXOSLoader.h"
#include "IFXDebug.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

//***************************************************************************
//  Local data
//***************************************************************************

static const IFXCHAR* s_pCoreDlName = L"libIFXCore.so";
static const char* s_pCorePathEnvVar = "U3D_LIBDIR";
static const char* s_pCorePathDefault = U3DCorePath;
#define IFXOSFI_SUBDIR U3DPluginsPath
#define _MAXIMUM_FILENAME_LENGTH  256


//***************************************************************************
//  Global functions
//***************************************************************************

extern "C"
IFXHANDLE IFXAPI IFXLoadLibrary( const IFXCHAR* pFileName )
{
  IFXHANDLE handle = NULL;

  if (NULL != pFileName)
  {
    char fileName[_MAXIMUM_FILENAME_LENGTH];
    size_t size = wcstombs(NULL, pFileName, 0);

    if (size < _MAXIMUM_FILENAME_LENGTH)
    {
    wcstombs(fileName, pFileName, size);
    fileName[size] = 0;

    handle = dlopen(fileName, RTLD_LAZY | RTLD_GLOBAL );

    if (!handle &&
      ( 	!wcscmp( pFileName, L"libIFXExporting.so"  ) || 
		!wcscmp( pFileName, L"libIFXImporting.so"  ) || 
		!wcscmp( pFileName, L"libIFXScheduling.so" ) || 
		!wcscmp( pFileName, L"libIFXRendering.so"  ) ))
    {
        char newFileName[_MAXIMUM_FILENAME_LENGTH];

        if (size+1+strlen(IFXOSFI_SUBDIR) < _MAXIMUM_FILENAME_LENGTH)
        {
          strcat(newFileName, IFXOSFI_SUBDIR "/");
          strcat(newFileName, fileName);

          handle = dlopen(newFileName, RTLD_LAZY | RTLD_GLOBAL );
        }
        else
        {
          IFXTRACE_GENERIC(L"%s:%i \"" IFXOSFI_SUBDIR "/%s\" FileName lenth is more then %i\n",__FILE__, __LINE__, fileName, _MAXIMUM_FILENAME_LENGTH);
        }
    }

	if (!handle &&
		( 	!wcscmp( pFileName, s_pCoreDlName )          ||
			!wcscmp( pFileName, L"libIFXExporting.so"  ) || 
			!wcscmp( pFileName, L"libIFXImporting.so"  ) || 
			!wcscmp( pFileName, L"libIFXScheduling.so" ) || 
			!wcscmp( pFileName, L"libIFXRendering.so"  ) ))
    {
      const char* pCorePath = getenv( s_pCorePathEnvVar );
      if( NULL == pCorePath ) pCorePath = s_pCorePathDefault;

        char newFileName[_MAXIMUM_FILENAME_LENGTH];
        size += strlen(pCorePath);

        if (size+1 < _MAXIMUM_FILENAME_LENGTH)
        {
          strcpy(newFileName, pCorePath);
          strcat(newFileName, "/");
          strcat(newFileName, fileName);

          handle = dlopen(newFileName, RTLD_LAZY | RTLD_GLOBAL );
        }
        else
        {
          IFXTRACE_GENERIC(L"%s:%i \"%s/%s\" FileName lenth is more then %i\n",__FILE__, __LINE__, pCorePath, fileName, _MAXIMUM_FILENAME_LENGTH);
        }
    }

    if (!handle &&
		( 	!wcscmp( pFileName, L"libIFXExporting.so"  ) || 
			!wcscmp( pFileName, L"libIFXImporting.so"  ) || 
			!wcscmp( pFileName, L"libIFXScheduling.so" ) || 
			!wcscmp( pFileName, L"libIFXRendering.so"  ) ))
    {
      const char* pCorePath = getenv( s_pCorePathEnvVar );
      if( NULL == pCorePath ) pCorePath = s_pCorePathDefault;

        char newFileName[_MAXIMUM_FILENAME_LENGTH];

        if (size+2+strlen(IFXOSFI_SUBDIR) < _MAXIMUM_FILENAME_LENGTH)
        {
          strcpy(newFileName, pCorePath);
          strcat(newFileName, "/" IFXOSFI_SUBDIR "/");
          strcat(newFileName, fileName);

          handle = dlopen(newFileName, RTLD_LAZY | RTLD_GLOBAL );
        }
        else
        {
          IFXTRACE_GENERIC(L"%s:%i \"%s/" IFXOSFI_SUBDIR "/%s\" FileName lenth is more then %i\n",__FILE__, __LINE__, pCorePath, fileName, _MAXIMUM_FILENAME_LENGTH);
        }
    }

    if (!handle)
    {
      IFXTRACE_GENERIC(L"%s:%i dlopen() error %s\n",__FILE__, __LINE__, dlerror());
    }
  }
    else
    {
      IFXTRACE_GENERIC(L"%s:%i \"%s\" FileName lenth is more then %i\n",__FILE__, __LINE__, pFileName, _MAXIMUM_FILENAME_LENGTH);
    }
  }
  else
  {
    IFXTRACE_GENERIC(L"%s:%i IFX_E_INVALID_POINTER", __FILE__, __LINE__);
  }

  return handle;
}

extern "C"
IFXHANDLE IFXAPI IFXLoadCoreLibrary()
{
    return IFXLoadLibrary( s_pCoreDlName );
}

extern "C"
IFXHANDLE IFXAPI IFXLoadOpenGLLibrary()
{
    return NULL;
}

extern "C"
IFXRESULT IFXAPI IFXOSGetCoreLibraryPath( IFXString* pPath )
{
  IFXRESULT result = IFX_OK;

  if( NULL != pPath )
  {
    U8* pCorePath = (U8*)getenv( s_pCorePathEnvVar );

    if( NULL != pCorePath )
      result = pPath->Assign( (const U8*)pCorePath );
    else
      result = pPath->Assign( (const U8*)s_pCorePathDefault );
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}

extern "C"
IFXOSFUNC IFXAPI IFXGetAddress( const IFXHANDLE handle, const char* pFuncName )
{
    return (IFXOSFUNC)dlsym( handle, pFuncName );
}

extern "C"
IFXRESULT IFXAPI IFXReleaseLibrary( const IFXHANDLE handle )
{
    IFXRESULT result = IFX_OK;

    if( 0 != dlclose( handle ) )
    {
        result = IFX_E_NOT_DONE;
    }

    return result;
}

extern "C"
FILE* IFXAPI IFXOSFileOpen(const wchar_t *filename, const wchar_t *mode)
{
  FILE  *pFile = NULL;
  char  *mb_fname = NULL;
  char  *mb_mode = NULL;
  size_t  size;
  size_t  res;

  size = wcstombs(NULL, filename, 0);

  if (size != (size_t)-1)
  {
    mb_fname = (char*)malloc((size+1)*sizeof(char));

    if (mb_fname != NULL)
    {
      res = wcstombs(mb_fname, filename, size);

      if (res != (size_t)-1)
      {
        mb_fname[size]='\0';
        size = wcstombs(NULL, mode, 0);

        if (size != (size_t)-1)
        {
          mb_mode = (char*)malloc((size+1)*sizeof(char));

          if (mb_mode != NULL)
          {
            res = wcstombs(mb_mode, mode, size);

            if (res != (size_t)-1)
            {
              mb_mode[size]='\0';
#ifdef STDIO_HACK
              if (!strcmp(mb_fname,"stdout") || (!strcmp(mb_fname,"-") && *mb_mode=='w')) {
                pFile = stdout;
              } else if (!strcmp(mb_fname,"stdin") || (!strcmp(mb_fname,"-") && *mb_mode=='r')) {
                pFile = stdin;
              } else {
                pFile = fopen(mb_fname, mb_mode);
              }
#else
              pFile = fopen(mb_fname, mb_mode);
#endif
            }

            free(mb_mode);
          }
        }
      }

      free(mb_fname);
    }
  }

  return pFile;
}

#endif //WIN32
