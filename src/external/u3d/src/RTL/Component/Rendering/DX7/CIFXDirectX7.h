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
// CIFXDirectX7.h
#ifndef CIFX_DIRECTX7_H
#define CIFX_DIRECTX7_H

#include "IFXDirectX7.h"

class CIFXDirectX7 : virtual public IFXDirectX7, virtual public IFXUnknown
{
  U32 m_refCount;
public:
  U32 IFXAPI  AddRef ();
  U32 IFXAPI  Release ();
  IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

  friend IFXRESULT IFXAPI_CALLTYPE CIFXDirectX7Factory(IFXREFIID intId, void** ppUnk);

  //=================================
  // IFXDirectX7 methods
  //=================================
  virtual IFXRESULT IFXAPI LoadDX7();

protected:
  CIFXDirectX7();
  virtual ~CIFXDirectX7();

  IFXRESULT IFXAPI Construct();

  HMODULE m_hDDrawLib;

  static CIFXDirectX7* IFXAPI GetSingleton();
  static CIFXDirectX7* ms_pSingleton;
};

#endif // CIFX_DIRECTX7_H

// END OF FILE
