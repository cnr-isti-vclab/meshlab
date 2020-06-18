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

#include <AGL/agl.h>

#define MAX_DISPLAYS 100

extern "C"
int IFXAPI_CALLTYPE IFXGetNumMonitors()
{
  ///@toodo: needs to be developed for MacOSX
  return 1;
}

extern "C"
int IFXAPI_CALLTYPE IFXGetColorBufferDepth()
{
  CGDirectDisplayID mainDisplayID = CGMainDisplayID();

  int pixelSize = (int)CGDisplayBitsPerPixel(mainDisplayID);

  return pixelSize;
}
extern "C"
BOOL IFXAPI_CALLTYPE IFXClientToScreen(IFXHANDLE hWnd, I32* pX, I32* pY)
{
  ///@todo: develop for MacOSX
  return 0;
}
extern "C"
BOOL IFXAPI_CALLTYPE IFXGetClientRect(IFXHANDLE hWnd, IFXRect* pRect)
{
  ///@todo: develop for MacOSX
  return 0;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXEnumDisplayDevices(void* pDevice, U32 devNum, PIFXDISPLAY_DEVICE pDisplayDevice, U32 flags, U32* resDevice)
{
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
  CGDisplayCount maxDisplays = MAX_DISPLAYS;
  CGDirectDisplayID displays[MAX_DISPLAYS];
  CGDisplayCount displayCount;
  CGRect cgRect;

  CGGetActiveDisplayList( maxDisplays,
              displays,
              &displayCount );


  for(U32 i = 0; i < uNumDevices; i++)
  {
    cgRect = CGDisplayBounds(displays[i]);

    pDeviceArr[i] = i;
    pScreenArr[i].Set(  cgRect.origin.x,
              cgRect.origin.y,
              cgRect.size.width,
              cgRect.size.height );
  }

  return result;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUseRenderWindowClassWin()
{
  //is not used in MacOSX
  return IFX_OK;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXUnuseRenderWindowClassWin()
{
  return IFX_OK;
}
