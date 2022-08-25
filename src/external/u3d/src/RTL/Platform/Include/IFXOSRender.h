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
  @file IFXOSRender.h

      This module contains platform specific render related functionality
      and data.

  @note Do not put any OS specific includes or types in this header!
*/

#ifndef IFXOSRENDER_H
#define IFXOSRENDER_H

#include "IFXResult.h"
#include "IFXRenderHelpers.h"
#include "IFXString.h"

#if defined( RENDERING ) && ( defined( LINUX ) || defined( __linux__ ) )
#include <GL/glx.h>
#endif

#ifndef SM_CMONITORS
  #define SM_CMONITORS 80
#endif // SM_CMONITORS

#ifndef HMON_DECLARED
  #define HMON_DECLARED
  typedef void* HMON;
#endif

typedef struct IFXDISPLAY_DEVICE
{
  U32  cb;
  IFXCHAR  DeviceName[32];
  IFXCHAR  DeviceString[128];
  U32  StateFlags;
} IFXDISPLAY_DEVICE, *PIFXDISPLAY_DEVICE;

#define DISPLAY_DEVICE_ATTACHED_TO_DESKTOP  0x00000001
#define DISPLAY_DEVICE_MULTI_DRIVER     0x00000002
#define DISPLAY_DEVICE_PRIMARY_DEVICE   0x00000004
#define DISPLAY_DEVICE_MIRRORING_DRIVER   0x00000008
#define DISPLAY_DEVICE_VGA          0x00000010

extern "C"
int IFXAPI_CALLTYPE IFXGetNumMonitors();

extern "C"
int IFXAPI_CALLTYPE IFXGetColorBufferDepth();

extern "C"
BOOL IFXAPI_CALLTYPE IFXClientToScreen(IFXHANDLE hWnd, I32* pX, I32* pY);

extern "C"
BOOL IFXAPI_CALLTYPE IFXGetClientRect(IFXHANDLE hWnd, IFXRect* pRect);

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayDevices(void*, U32, PIFXDISPLAY_DEVICE, U32, U32*);

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayMonitors( IFXHANDLE hDC,
                U32 uMonNum, U32 uNumDevices, IFXString* pStrings, IFXRect*& pScreenArr, U32*& pDeviceArr );

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUseRenderWindowClassWin();

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUnuseRenderWindowClassWin();

#define IFX_RENDER_WINDOW_CLASS L"IFXRenderWindowClass"

#endif
