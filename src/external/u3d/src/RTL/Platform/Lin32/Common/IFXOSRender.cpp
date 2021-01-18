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

#include <dlfcn.h>
#include <wchar.h>
#include "IFXOSLoader.h"
#include "IFXOSRender.h"

typedef Display* (*XOD)(const char *);
typedef void (*XCD)(Display *);

extern "C"
int IFXAPI_CALLTYPE IFXGetNumMonitors()
{
	///@toodo: needs to be developed for Linux
	return 1;
}

extern "C"
int IFXAPI_CALLTYPE IFXGetColorBufferDepth()
{
	///@todo: develop for Linux
	return 0;
}
extern "C"
BOOL IFXAPI_CALLTYPE IFXClientToScreen(IFXHANDLE hWnd, I32* pX, I32* pY)
{
	///@todo: develop for Linux
	return 0;
}
extern "C"
BOOL IFXAPI_CALLTYPE IFXGetClientRect(IFXHANDLE hWnd, IFXRect* pRect)
{
	///@todo: develop for Linux
	return 0;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayDevices(void* pDevice, U32 devNum, PIFXDISPLAY_DEVICE pDisplayDevice, U32 flags, U32* resDevice)
{
	///@toodo: needs to be developed for Linux

	if (devNum == 0) 
	{
		*resDevice = 1;
		pDisplayDevice->StateFlags = 0 | DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
		wcscpy(pDisplayDevice->DeviceName, L":0.0");
	}
	else
		*resDevice = 0;

	return IFX_OK;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayMonitors( IFXHANDLE hDC,
								U32 uMonNum, U32 uNumDevices, IFXString* pStrings, IFXRect*& pScreenArr, U32*& pDeviceArr )
{
	IFXRESULT result = IFX_OK;
	Display* dpy = (Display*)hDC;
	IFXHANDLE handle;
	XOD xOpenDisplay = NULL;
	XCD xCloseDisplay = NULL;

	handle = IFXLoadLibrary(L"libX11.so");
	if (!handle) 
	{
		result = IFX_E_SUBSYSTEM_FAILURE;
	}
	if (IFXSUCCESS(result)) 
	{
		char* error;
		xOpenDisplay = (XOD)IFXGetAddress(handle, "XOpenDisplay");
		if ((error = dlerror()) != NULL) 
		{
			result = IFX_E_SUBSYSTEM_FAILURE;
		}
		xCloseDisplay = (XCD)IFXGetAddress(handle, "XCloseDisplay");
		if ((error = dlerror()) != NULL) 
		{
			result = IFX_E_SUBSYSTEM_FAILURE;
		}
	}


	if (IFXSUCCESS(result)) 
	{
		if (NULL == dpy) 
		{
			dpy = xOpenDisplay(NULL);
		}
		if (NULL == dpy)
			result = IFX_E_SUBSYSTEM_FAILURE;
    }

	if (IFXSUCCESS(result)) 
	{
	/**@todo: investigate and implement monitors enumeration
		if (XineramaIsActive(dpy)) 
		{
			XineramaScreenInfo *pScreens;
			pScreens = XineramaQueryScreens(dpy, (int*)&uMonNum);
	
			XFree(pScreens);
		}
		else*/
		{
			uMonNum = 1;
		}
	
		///@todo: improve it to get size of all of the possible screens
		pScreenArr[0].m_X = 0;
		pScreenArr[0].m_Y = 0;
		pScreenArr[0].m_Width = DisplayWidth (dpy, DefaultScreen (dpy));
		pScreenArr[0].m_Height = DisplayHeight (dpy, DefaultScreen (dpy));
	
		pDeviceArr[0] = 0;
	
	}

	if (NULL == hDC && NULL != dpy) 
	{
		xCloseDisplay(dpy);
	}

	IFXReleaseLibrary(handle);

	return result;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUseRenderWindowClassWin()
{
	//is not used in Linux
	return IFX_OK;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUnuseRenderWindowClassWin()
{
	return IFX_OK;
}

