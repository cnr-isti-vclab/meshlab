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
// IFXDirectX7.h
#ifndef IFX_DIRECTX7_H
#define IFX_DIRECTX7_H

#include "IFXUnknown.h"

#ifdef WIN32

#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION 0x700
#endif // DIRECT3D_VERSION

#ifndef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x0700
#endif // DIRECTDRAW_VERSION

#ifndef D3D_OVERLOADS
#define D3D_OVERLOADS
#endif // D3D_OVERLOADS

#include "ddraw.h"
#include "d3d.h"

#if (DIRECT3D_VERSION < 0x0800)
// Inclusion of this header file allow code to use DX8 types with the DX7
// types which are the same with different names.
#include "DX7asDX8.h"
#endif

/**
 *  Function pointer type declaration for DirectDrawEnumerateEx.
 */
typedef HRESULT(WINAPI * PFNDirectDrawEnumerateEx)(LPDDENUMCALLBACKEX pCallBack, LPVOID pvData, DWORD dwFlags);
/**
 *  Function pointer type declaration for DirectDrawCreateEx.
 */
typedef HRESULT (WINAPI * PFNDirectDrawCreateEx)( GUID*, LPVOID*, REFIID, IUnknown* );

// {29442A44-F01E-4241-9C76-A83137A8AF3F}
IFXDEFINE_GUID(IID_IFXDirectX7,
0x29442a44, 0xf01e, 0x4241, 0x9c, 0x76, 0xa8, 0x31, 0x37, 0xa8, 0xaf, 0x3f);

/**
 *  The implementation of this class handles the loading and unloading
 *  of the DirectX7 DDraw library, and querying for a function pointers to
 *  DirectDrawEnumerateEx and DirectDrawCreateEx.  These are the only entry
 *  points in ddraw.dll that needs to be used to fully use DX7 D3D.
 */
class IFXDirectX7 : virtual public IFXUnknown
{
public:
  /**
   *  Function pointer for ddraw.dll entry point DirectDrawEnumerateEx.
   */
  PFNDirectDrawEnumerateEx m_DirectDrawEnumerateEx;
  /**
   *  Function pointer for ddraw.dll entry point DirectDrawCreateEx.
   */
  PFNDirectDrawCreateEx m_DirectDrawCreateEx;

  /**
   *  Loads the ddraww.dll and sets the function pointers for
   *  DirectDrawEnumerateEx and DirectDrawCreateEx.
   *
   *  @return An IFXRESULT code.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_UNSUPPORTED The DDRAW.dll failed to load, or the functions
   *                "DirectDrawEnumerateEx" or "DirectDrawCreateEx"
   *                failed to be retrieved.
   */
  virtual IFXRESULT IFXAPI LoadDX7()=0;
protected:
  IFXDirectX7()
  {
    m_DirectDrawEnumerateEx = 0;
    m_DirectDrawCreateEx = 0;
  }
  virtual ~IFXDirectX7() {}
};
/**
 *  Smart pointer type declaration for IFXDirectX7.
 */
typedef IFXSmartPtr<IFXDirectX7> IFXDirectX7Ptr;

#endif // WIN32

#endif // IFX_DIRECTX7_H

// END OF FILE
