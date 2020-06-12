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
// CIFXDirectX8.h
#ifndef CIFX_DIRECTX8_H
#define CIFX_DIRECTX8_H

#include "IFXDirectX8.h"

class CIFXDirectX8 : virtual public IFXDirectX8, virtual public IFXUnknown
{
  U32 m_refCount;
public:
  U32 IFXAPI  AddRef ();
  U32 IFXAPI  Release ();
  IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

  friend IFXRESULT IFXAPI_CALLTYPE CIFXDirectX8Factory(IFXREFIID intId, void** ppUnk);

  //=================================
  // IFXDirectX8 methods
  //=================================
  virtual IFXRESULT IFXAPI LoadDX8();

protected:
  CIFXDirectX8();
  virtual ~CIFXDirectX8();

  IFXRESULT IFXAPI Construct();

  HMODULE m_hD3D8Lib;

  static CIFXDirectX8* IFXAPI GetSingleton();
  static CIFXDirectX8* ms_pSingleton;
};

#endif // CIFX_DIRECTX8_H

// END OF FILE
