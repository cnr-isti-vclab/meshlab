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
// IFXDirectX8.h
#ifndef IFX_DIRECTX8_H
#define IFX_DIRECTX8_H

#include "IFXUnknown.h"

#ifdef WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION         0x0800
#endif  //DIRECT3D_VERSION

#include <windows.h>

#include "d3d8.h"


/**
 *  Function pointer type for Direct3DCreate8.
 */
//typedef IDirect3D8* (WINAPI * PFNDirect3DCreate8)(UINT);
typedef LPDIRECT3D8 (WINAPI * PFNDirect3DCreate8)(UINT);

// {896A7ECF-C918-4039-8202-8AAFC19D184F}
IFXDEFINE_GUID(IID_IFXDirectX8,
0x896a7ecf, 0xc918, 0x4039, 0x82, 0x2, 0x8a, 0xaf, 0xc1, 0x9d, 0x18, 0x4f);

/**
 *  The implementation of this class handles the loading and unloading
 *  of the DirectX8 D3D library, and querying for a function pointer to
 *  Direct3DCreate8.  This is the only entry point in d3d8.dll that needs
 *  to be used to use DX8 D3D.
 *
 *  We also use a DirectDraw7 object for implementing VSync.  This is much
 *  easier (and seems to work better) than using DX8's VSync routines.
 *  Therefore, this object also loads the DDraw.dll and retrieves a function
 *  pointer to DirectDrawCreateEx.
 */
class IFXDirectX8 : virtual public IFXUnknown
{
public:
  /**
   *  Function pointer for the d3d8.dll entry point Direct3DCreate8.
   */
  PFNDirect3DCreate8 Direct3DCreate8;

  /**
   *  Loads the d3d8.dll and sets the function pointer for
   *  Direct3DCreate8.
   *
   *  @return An IFXRESULT code.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_UNSUPPORTED The D3D8.dll failed to load, or the function
   *                "Direct3DCreate8" failed to be retrieved.
   */
  virtual IFXRESULT IFXAPI LoadDX8()=0;
protected:
  IFXDirectX8() {Direct3DCreate8 = 0;}
  virtual ~IFXDirectX8() {}
};

/**
 *  Smart pointer type declaration for IFXDirectX8.
 */
typedef IFXSmartPtr<IFXDirectX8> IFXDirectX8Ptr;

#endif // WIN32

#endif // IFX_DIRECTX8_H
