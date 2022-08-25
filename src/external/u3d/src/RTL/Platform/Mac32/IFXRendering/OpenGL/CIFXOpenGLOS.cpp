//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// CIFXOpenGLOS.cpp
#include "IFXRenderPCHOGL.h"
#include "CIFXOpenGLOS.h"
#include "IFXOSLoader.h"

#include <dlfcn.h>
#include <CoreFoundation/CFString.h>
#include <mach-o/dyld.h>

CIFXOpenGLOS* CIFXOpenGLOS::ms_pSingleton = 0;
static const mach_header* s_phAGL = NULL;
static const mach_header* s_phOpenGL = NULL;
#define AGL_LIBRARY_PATH "/System/Library/Frameworks/AGL.framework/Versions/A/AGL"
#define OPENGL_LIBRARY_PATH "/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL"
#define _MAXIMUM_FUNCNAME_LENGTH  128

//===========================
// Factory Function
//===========================
IFXRESULT CIFXOpenGLOSFactory(IFXREFIID intId, void** ppUnk)
{
  IFXRESULT rc = IFX_OK;

  if(ppUnk)
  {
    CIFXOpenGLOS* pPtr = CIFXOpenGLOS::GetSingleton();
    if(!pPtr)
    {
      pPtr = new CIFXOpenGLOS;
      if(pPtr)
      {
        rc = pPtr->Construct();
        if(IFXFAILURE(rc))
        {
          IFXDELETE(pPtr);
        }
      }
      else
      {
        rc = IFX_E_OUT_OF_MEMORY;
      }
    }

    if(IFXSUCCESS(rc))
    {
      pPtr->AddRef();
      rc = pPtr->QueryInterface(intId, ppUnk);
      pPtr->Release();
    }
  }
  else
  {
    rc = IFX_E_INVALID_POINTER;
  }

  return rc;
}

U32 CIFXOpenGLOS::AddRef()
{
  return ++m_refCount;
}

U32 CIFXOpenGLOS::Release()
{
  if (!(--m_refCount))
  {
    delete this;
    ms_pSingleton = NULL;
    return 0;
  }
  return m_refCount;
}

IFXRESULT CIFXOpenGLOS::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
  IFXRESULT result = IFX_OK;
  if (ppInterface)
  {
    if (interfaceId == IID_IFXOpenGL)
    {
      *(IFXOpenGL**)ppInterface = (IFXOpenGL*) this;
    }
    else
    if (interfaceId == IID_IFXOpenGLOS)
    {
      *(IFXOpenGLOS**)ppInterface = (IFXOpenGLOS*) this;
    }
    else
    if (interfaceId == IID_IFXUnknown)
    {
      *(IFXUnknown**)ppInterface = (IFXUnknown*) this;
    }
    else
    {
      *ppInterface = NULL;
      result = IFX_E_UNSUPPORTED;
    }
    if (IFXSUCCESS(result))
      AddRef();
  }
  else
  {
    result = IFX_E_INVALID_POINTER;
  }
  return result;
}

CIFXOpenGLOS::CIFXOpenGLOS()
{
  IFXASSERTBOX(ms_pSingleton == 0, "Creating more than one instance of CIFXOpenGLOS!!");
  ms_pSingleton = this;
  m_refCount = 0;
}

IFXRESULT CIFXOpenGLOS::Construct()
{
  IFXRESULT rc = CIFXOpenGL::Construct();

  return rc;
}

IFXOGLPROC CIFXOpenGLOS::GetOpenGLFunc(const char* szFuncName)
{
	char newFuncName[_MAXIMUM_FUNCNAME_LENGTH+1];
    int size = (U32)strlen(szFuncName);

    if (size <= _MAXIMUM_FUNCNAME_LENGTH)
    {
        strcpy(newFuncName, "_");
        strcat(newFuncName, szFuncName);
    }
    else
    {
        IFXTRACE_GENERIC(L"%s:%i \"%s%s\" FuncName lenth is more then %i\n",__FILE__, __LINE__, szFuncName, _MAXIMUM_FUNCNAME_LENGTH);
    }

	IFXOGLPROC procAddress = (IFXOGLPROC)NSAddressOfSymbol(NSLookupSymbolInImage(s_phAGL, newFuncName,
															NSLOOKUPSYMBOLINIMAGE_OPTION_BIND));
	if (NULL == procAddress)
	{
		procAddress = (IFXOGLPROC)NSAddressOfSymbol(NSLookupSymbolInImage(s_phOpenGL, newFuncName,
															NSLOOKUPSYMBOLINIMAGE_OPTION_BIND));
	}

	if (NULL == procAddress)
	{
	IFXTRACE_GENERIC(L"GL function [%s] is not allowed\n", szFuncName);
	}

	return procAddress;
}

IFXRESULT CIFXOpenGLOS::LoadOpenGL()
{
  IFXRESULT rc = IFX_OK;

  if(m_bOpenGLLoaded)
  {
    return rc;
  }

  if(0 == s_phOpenGL)
  {
    ClearOpenGLFuncs();

    s_phOpenGL = NSAddImage(OPENGL_LIBRARY_PATH, NSADDIMAGE_OPTION_NONE);

    if (!s_phOpenGL)
    {
      IFXTRACE_GENERIC(L"%s:%i NSAddImage() error %s\n", OPENGL_LIBRARY_PATH);
      rc = IFX_E_UNSUPPORTED;
    }
  }

  if(0 == s_phAGL)
  {
    ClearOpenGLFuncs();

    s_phAGL = NSAddImage(AGL_LIBRARY_PATH, NSADDIMAGE_OPTION_NONE);

    if (!s_phAGL)
    {
      IFXTRACE_GENERIC(L"%s:%i NSAddImage() error %s\n", AGL_LIBRARY_PATH);
      rc = IFX_E_UNSUPPORTED;
    }
  }

  if(s_phOpenGL && s_phAGL && (0 == glEnable))
  {
    GetOpenGLFuncs();

    if(0 == glEnable)
    {
      UnloadOpenGL();
      rc = IFX_E_UNSUPPORTED;
    }
  }

  if(IFXSUCCESS(rc))
  {
    m_bOpenGLLoaded = TRUE;
  }

  return rc;
}

IFXRESULT CIFXOpenGLOS::UnloadOpenGL()
{
  ClearOpenGLFuncs();

  if(s_phOpenGL)
  {
	s_phOpenGL = 0;
  }

  if(s_phAGL)
  {
    s_phAGL = 0;
  }

  m_bOpenGLLoaded = FALSE;

  return IFX_OK;
}


IFXRESULT CIFXOpenGLOS::LoadOSGL()
{
  IFXRESULT rc = LoadOpenGL();

  if (IFXSUCCESS(rc))
  {
    aglCreateContext =       (AGLCreateContext)GetOpenGLFunc("aglCreateContext");
    aglUpdateContext =       (AGLUpdateContext)GetOpenGLFunc("aglUpdateContext");
    aglGetCurrentContext =   (AGLGetCurrentContext)GetOpenGLFunc("aglGetCurrentContext");
    aglGetDrawable =     (AGLGetDrawable)GetOpenGLFunc("aglGetDrawable");
    aglCopyContext =     (AGLCopyContext)GetOpenGLFunc("aglCopyContext");
    aglDestroyContext =      (AGLDestroyContext)GetOpenGLFunc("aglDestroyContext");
    aglSetCurrentContext =   (AGLSetCurrentContext)GetOpenGLFunc("aglSetCurrentContext");
    aglSetDrawable =     (AGLSetDrawable)GetOpenGLFunc("aglSetDrawable");
    aglSetInteger =      (AGLSetInteger)GetOpenGLFunc("aglSetInteger");
    aglChoosePixelFormat =   (AGLChoosePixelFormat)GetOpenGLFunc("aglChoosePixelFormat");
    aglDestroyPixelFormat =  (AGLDestroyPixelFormat)GetOpenGLFunc("aglDestroyPixelFormat");
    aglSetOffScreen =        (AGLSetOffScreen)GetOpenGLFunc("aglSetOffScreen");
    aglEnable =        (AGLEnable)GetOpenGLFunc("aglEnable");
    aglDisable =       (AGLDisable)GetOpenGLFunc("aglDisable");
    aglDescribePixelFormat = (AGLDescribePixelFormat)GetOpenGLFunc("aglDescribePixelFormat");
    aglQueryRendererInfo =   (AGLQueryRendererInfo)GetOpenGLFunc("aglQueryRendererInfo");
    aglDestroyRendererInfo = (AGLDestroyRendererInfo)GetOpenGLFunc("aglDestroyRendererInfo");
    aglNextRendererInfo =    (AGLNextRendererInfo)GetOpenGLFunc("aglNextRendererInfo");
    aglDescribeRenderer =    (AGLDescribeRenderer)GetOpenGLFunc("aglDescribeRenderer");
    aglSwapBuffers =     (AGLSwapBuffers)GetOpenGLFunc("aglSwapBuffers");

    if(0 == aglCreateContext)
    {
      rc = IFX_E_UNSUPPORTED;
    }
  }

  return rc;
}

IFXRESULT CIFXOpenGLOS::LoadExtensions()
{
  IFXRESULT rc = CIFXOpenGL::LoadExtensions();

  return rc;
}

IFXOGLPROC IFXAPI CIFXOpenGLOS::GetOGLExtensionFunc(const char* szFuncName)
{
  return GetOpenGLFunc(szFuncName);
}

// END OF FILE
