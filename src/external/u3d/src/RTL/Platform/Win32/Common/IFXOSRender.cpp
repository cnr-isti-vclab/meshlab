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

#include <windows.h>

#include "IFXOSLoader.h"
#include "IFXOSRender.h"
#include "IFXRenderServices.h"


typedef BOOL (CALLBACK* MONENUMPROC)(HMON, HDC, LPRECT, LPARAM);

typedef BOOL (WINAPI* GMI)(HMON, LPMONITORINFOEX);
typedef BOOL (WINAPI* EDM)(HDC, LPCRECT, MONENUMPROC, LPARAM);
typedef BOOL (WINAPI* EDD)(PVOID, DWORD, PDISPLAY_DEVICE, DWORD);

struct MonitorEnumData
{
	U32 uMonNum;
	U32 uNumDevices;
	IFXString* pStrings;
	IFXRect* pScreen; ///< Resolution of monitor. Part of IFXMonitor
	U32* pDeviceNum; ///< Video adapter device id. Part of IFXMonitor
};

IFXRESULT IFXGetMonitorInfo(void*, LPMONITORINFOEX);

static BOOL CALLBACK MonitorEnum(HMON hMon, HDC hDC, LPRECT prcRect, LPARAM lParam)
{
	MonitorEnumData* pMonitorEnumData = (MonitorEnumData*)lParam;
	IFXRect* pScreen = &pMonitorEnumData->pScreen[pMonitorEnumData->uMonNum];
	U32* pDeviceNum = &pMonitorEnumData->pDeviceNum[pMonitorEnumData->uMonNum];
	
	MONITORINFOEX miMon;
	memset(&miMon, 0, sizeof(MONITORINFOEX));
	miMon.cbSize = sizeof(MONITORINFOEX);
	IFXGetMonitorInfo( hMon, &miMon );

	U32 i;
	for( i = 0; i < pMonitorEnumData->uNumDevices; i++)
	{
		IFXString pMonDevice;
		pMonDevice.Assign(miMon.szDevice);
		if(pMonDevice == pMonitorEnumData->pStrings[i])
		{
			*pDeviceNum = i;
			break;
		}
	}
//	IFXASSERTBOX(i < pMonitorEnumData->uNumDevices, "Monitor doesn't appear to be attached to anything!");

	pScreen->Set( miMon.rcMonitor.left, miMon.rcMonitor.top, miMon.rcMonitor.right - miMon.rcMonitor.left,
			miMon.rcMonitor.bottom - miMon.rcMonitor.top );
	(pMonitorEnumData->uMonNum)++;
	
	return TRUE;
}

extern "C"
int IFXAPI_CALLTYPE IFXGetNumMonitors()
{
	return GetSystemMetrics(SM_CMONITORS);
}

extern "C"
int IFXAPI_CALLTYPE IFXGetColorBufferDepth()
{
	U32 uBitsPerPixel = 0;
	HDC hDC = GetDC(NULL);
	uBitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(NULL, hDC);
	
	return uBitsPerPixel;
}

extern "C"
BOOL IFXAPI_CALLTYPE IFXClientToScreen(IFXHANDLE hWnd, I32* pX, I32* pY)
{
	POINT p;
	p.x = *pX;
	p.y = *pY;
	BOOL result = ClientToScreen( (HWND)hWnd, &p );
	*pX = p.x;
	*pY = p.y;
	return result;
}

extern "C"
BOOL IFXAPI_CALLTYPE IFXGetClientRect(IFXHANDLE hWnd, IFXRect* pRect)
{
	RECT clientRect;
	BOOL result = GetClientRect( (HWND)hWnd, &clientRect );
	pRect->m_X = clientRect.left;
	pRect->m_Y = clientRect.top;
	pRect->m_Width = clientRect.right - pRect->m_X;
	pRect->m_Height = clientRect.bottom - pRect->m_Y;
	return result;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayDevices(void* pDevice, U32 devNum, PIFXDISPLAY_DEVICE pDisplayDevice, U32 flags, U32* resDevice)
{
	IFXRESULT result = IFX_OK;
	// Load the user32.dll library
	IFXHANDLE hUserLib = IFXLoadLibrary(L"USER32");

	EDD EnumDisplayDevices = NULL;

	if( 0 != hUserLib )
    {
#ifdef UNICODE		
	    EnumDisplayDevices = (EDD)IFXGetAddress(hUserLib, "EnumDisplayDevicesW");
#else
	    EnumDisplayDevices = (EDD)IFXGetAddress(hUserLib, "EnumDisplayDevicesA");
#endif
    }
	else
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS( result ) )
	{
		if( EnumDisplayDevices )
			*resDevice = EnumDisplayDevices( pDevice, devNum, (PDISPLAY_DEVICE)pDisplayDevice, flags );
		else
			result = IFX_E_NOT_DONE;
	}
	
	IFXReleaseLibrary(hUserLib);
	return result;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayMonitors( IFXHANDLE hDC,
								U32 uMonNum, U32 uNumDevices, IFXString* pStrings, IFXRect*& pScreenArr, U32*& pDeviceArr )
{
	IFXRESULT result = IFX_OK;
	// Load the user32.dll library
	IFXHANDLE hUserLib = IFXLoadLibrary(L"USER32");

	EDM EnumDisplayMonitors = NULL;

	if( 0 != hUserLib) 
	    EnumDisplayMonitors = (EDM)IFXGetAddress(hUserLib, "EnumDisplayMonitors");
	else
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS( result ) )
	{
		MonitorEnumData param;
		param.uMonNum = uMonNum;
		param.uNumDevices = uNumDevices;
		param.pStrings = pStrings;
		param.pScreen = pScreenArr;
		param.pDeviceNum = pDeviceArr;
		EnumDisplayMonitors( (HDC)hDC, NULL, MonitorEnum, (LPARAM) &param );
	}
	
	IFXReleaseLibrary(hUserLib);
	return result;
}

IFXRESULT IFXGetMonitorInfo( void* hMon, LPMONITORINFOEX miMon )
{
	IFXRESULT result = IFX_OK;
	// Load the user32.dll library
	IFXHANDLE hUserLib = IFXLoadLibrary(L"USER32");

	GMI GetMonitorInfo = NULL;

	if( 0 != hUserLib) 
#ifdef UNICODE		
	    GetMonitorInfo = (GMI)IFXGetAddress(hUserLib, "GetMonitorInfoW");
#else
	    GetMonitorInfo = (GMI)IFXGetAddress(hUserLib, "GetMonitorInfoA");
#endif
	else
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS( result ) )
		GetMonitorInfo( (HMON)hMon, miMon );
	
	IFXReleaseLibrary(hUserLib);
	return result;
}



static ATOM gs_aClass = 0;
U32 gs_uNumInUse = 0;

LRESULT CALLBACK IFXWinMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUseRenderWindowClassWin()
{
	gs_uNumInUse++;

	if(0 == gs_aClass)
	{
		WNDCLASSEX wc = 
		{
			sizeof(WNDCLASSEX),
				CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
				IFXWinMsgProc,
				0,
				0,
				GetModuleHandle(NULL),
				NULL,
				NULL,
				NULL,
				NULL,
				IFX_RENDER_WINDOW_CLASS,
				NULL
		};
		gs_aClass = RegisterClassEx(&wc);
	}

	return IFX_OK;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUnuseRenderWindowClassWin()
{
	gs_uNumInUse--;
	if(gs_aClass && (0 == gs_uNumInUse))
	{
		UnregisterClass(IFX_RENDER_WINDOW_CLASS, GetModuleHandle(NULL));
		gs_aClass = 0;
	}

	return IFX_OK;
}

//=============================
// Message Processing function
//=============================
LRESULT CALLBACK IFXWinMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT p;

	switch (uMsg) 
	{

	case WM_ERASEBKGND:// fixes white background...
		return 1;
		break;

	case WM_PAINT:// Don't update, updates will be forced.
		ValidateRect(hWnd, NULL);
		return 1;
		break;

		// Pass clicks to parent window.
	case WM_LBUTTONDBLCLK:	// 0x203
	case WM_RBUTTONDBLCLK:	// 0x206
	case WM_MBUTTONDBLCLK:	// 0x209

	case WM_NCLBUTTONDOWN:	// 0x0a1
	case WM_NCRBUTTONDOWN:	// 0x0a4
	case WM_NCMBUTTONDOWN:	// 0x0a7
	case WM_LBUTTONDOWN:	// 0x201
	case WM_RBUTTONDOWN:	// 0x204
	case WM_MBUTTONDOWN:	// 0x207

	case WM_NCLBUTTONUP:	// 0x0a2
	case WM_NCRBUTTONUP:	// 0x0a5
	case WM_NCMBUTTONUP:	// 0x0a8
	case WM_LBUTTONUP:		// 0x202
	case WM_RBUTTONUP:		// 0x205
	case WM_MBUTTONUP:		// 0x208
		{
			HWND hWndMyParent = GetParent( hWnd );
			if ( hWndMyParent )
			{
				p.x = LOWORD( lParam );
				p.y = HIWORD( lParam );
				ClientToScreen( hWnd, &p );
				ScreenToClient( hWndMyParent, &p );
				lParam = MAKELPARAM( p.x, p.y );

				HRESULT hr = 0;
				hr = PostMessage( hWndMyParent, uMsg, wParam, lParam );

				return hr;
			}
			else break;
		}

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
