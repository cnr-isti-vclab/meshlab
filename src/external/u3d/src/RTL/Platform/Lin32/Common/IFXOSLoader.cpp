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
  @file IFXOSLoader.cpp

      This module contains the Linux specific functions required to
      work with dynamically liked library.
*/

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXOSLoader.h"
#include "IFXDebug.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

//***************************************************************************
//  Local data
//***************************************************************************

static const IFXCHAR* s_pCoreDlName = L"libIFXCore.so";
static const IFXCHAR* s_pOpenGLDlName = L"libGL.so";
static const char* s_pCorePathEnvVar = "U3D_LIBDIR";
#ifdef U3DCorePath
static const char* s_pCorePathDefault = U3DCorePath;
#else
static const char* s_pCorePathDefault = ".";
#endif
#ifdef U3DPluginsPath
#define IFXOSFI_SUBDIR U3DPluginsPath
#else
#define IFXOSFI_SUBDIR "Plugins"
#endif
#define _MAXIMUM_FILENAME_LENGTH  128


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

    if (!handle)
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

    if (!handle)
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
    return IFXLoadLibrary( s_pOpenGLDlName );
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
