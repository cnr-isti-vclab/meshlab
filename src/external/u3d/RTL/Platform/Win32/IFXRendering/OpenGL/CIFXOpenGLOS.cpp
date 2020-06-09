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

CIFXOpenGLOS* CIFXOpenGLOS::ms_pSingleton = 0;

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXOpenGLOSFactory(IFXREFIID intId, void** ppUnk)
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

	m_hLibrary = 0;

	return rc;
}

IFXOGLPROC CIFXOpenGLOS::GetOpenGLFunc(CHAR* szFuncName)
{
	return (IFXOGLPROC)IFXGetAddress(m_hLibrary, szFuncName);
}

IFXRESULT CIFXOpenGLOS::LoadOpenGL()
{
	IFXRESULT rc = IFX_OK;

	if(m_bOpenGLLoaded)
	{
		return rc;
	}

	if(0 == m_hLibrary)
	{
		ClearOpenGLFuncs();

		m_hLibrary = IFXLoadOpenGLLibrary();

		IFXASSERTBOX(0 != m_hLibrary, "Couldn't load OpenGL32.dll");
		if(0 == m_hLibrary)
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	if(m_hLibrary && (0 == glEnable))
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

	if(m_hLibrary)
	{
		IFXReleaseLibrary(m_hLibrary);
		m_hLibrary = 0;
	}

	m_bOpenGLLoaded = FALSE;

	return IFX_OK;
}


IFXRESULT CIFXOpenGLOS::LoadOSGL()
{
	IFXRESULT rc = LoadOpenGL();

	if(IFXSUCCESS(rc))
	{
		wglCopyContext =		(WGLCopyContext)		GetOpenGLFunc("wglCopyContext");
		wglCreateContext =		(WGLCreateContext)		GetOpenGLFunc("wglCreateContext");
		wglCreateLayerContext =	(WGLCreateLayerContext)	GetOpenGLFunc("wglCreateLayerContext");
		wglDeleteContext =		(WGLDeleteContext)		GetOpenGLFunc("wglDeleteContext");
		wglGetCurrentContext =	(WGLGetCurrentContext)	GetOpenGLFunc("wglGetCurrentContext");
		wglGetCurrentDC =		(WGLGetCurrentDC)		GetOpenGLFunc("wglGetCurrentDC");
		wglGetProcAddress =		(WGLGetProcAddress)		GetOpenGLFunc("wglGetProcAddress");
		wglMakeCurrent =		(WGLMakeCurrent)		GetOpenGLFunc("wglMakeCurrent");
		wglShareLists =			(WGLShareLists)			GetOpenGLFunc("wglShareLists");
		wglSwapLayerBuffers =	(WGLSwapLayerBuffers)	GetOpenGLFunc("wglSwapLayerBuffers");
	}

	if(wglCreateContext == 0)
	{
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

IFXRESULT CIFXOpenGLOS::LoadExtensions()
{
	IFXRESULT rc = CIFXOpenGL::LoadExtensions();

	if(IFXSUCCESS(rc))
	{
		wglSwapIntervalEXT = NULL;//(WGLSwapIntervalEXT)	GetOGLExtensionFunc("wglSwapIntervalEXT");
	}

	return rc;
}

IFXOGLPROC IFXAPI CIFXOpenGLOS::GetOGLExtensionFunc(CHAR* szFuncName)
{
	return (IFXOGLPROC)wglGetProcAddress(szFuncName);
}

// END OF FILE


