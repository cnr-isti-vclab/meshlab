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
// CIFXDeviceTexUnit.h
#ifndef CIFX_DEVICE_TEXUNIT_H
#define CIFX_DEVICE_TEXUNIT_H

#include "IFXRenderTexUnit.h"

//==============================
// CIFXDeviceTexUnit
//==============================
class CIFXDeviceTexUnit : public IFXRenderTexUnit, public CIFXDeviceBase
{
  U32 m_refCount;
public:
  U32 IFXAPI  AddRef ();
  U32 IFXAPI  Release ();
  IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

  //=================================
  // CIFXDeviceTexUnit methods
  //=================================
  IFXRESULT IFXAPI SetTexUnit(IFXRenderTexUnit& s);

  IFXRESULT IFXAPI SetTexCoordSet(U32 uTCSet);
  U32 IFXAPI GetTexCoordSet() { return m_uTexCoordSet; }

  IFXRESULT IFXAPI SetTextureId(U32 uTexId);

  void IFXAPI InitData();

protected:
  CIFXDeviceTexUnit();
  virtual ~CIFXDeviceTexUnit();

  IFXRESULT IFXAPI SetDefaults();

  virtual IFXRESULT IFXAPI SetHWRGBInput0 (IFXenum eInput);
  virtual IFXRESULT IFXAPI SetHWRGBInput1 (IFXenum eInput);
  virtual IFXRESULT IFXAPI SetHWRGBInterpolatorSource(IFXenum eInput);
  virtual IFXRESULT IFXAPI SetHWRGBBlendFunc (IFXenum eFunc);
  virtual IFXRESULT IFXAPI SetHWAlphaInput0 (IFXenum eInput);
  virtual IFXRESULT IFXAPI SetHWAlphaInput1 (IFXenum eInput);
  virtual IFXRESULT IFXAPI SetHWAlphaInterpolatorSource(IFXenum eInput);
  virtual IFXRESULT IFXAPI SetHWAlphaFunc (IFXenum eFunc);
  virtual IFXRESULT IFXAPI SetHWTextureId (U32 uTexId);
  virtual IFXRESULT IFXAPI SetHWTexCoordGen (IFXenum eGenMode);
  virtual IFXRESULT IFXAPI SetHWTexRepeatU(BOOL bEnabled);
  virtual IFXRESULT IFXAPI SetHWTexRepeatV(BOOL bEnabled);
  virtual IFXRESULT IFXAPI SetHWTexRepeatW(BOOL bEnabled);
  virtual IFXRESULT IFXAPI SetHWTextureTransform (const IFXMatrix4x4 &mTexture);
  virtual IFXRESULT IFXAPI SetHWTextureCoordinateSet(I32 iTexCoordSet);

  virtual IFXRESULT IFXAPI SetHWTexCoordSet(U32 uTCSet);

  virtual IFXRESULT IFXAPI SetHWTexUnit();

  IFXRESULT IFXAPI Construct();

  U32 m_uTexCoordSet;

public:
  IFXRESULT IFXAPI Reset();
};
typedef IFXSmartPtr<CIFXDeviceTexUnit> CIFXDeviceTexUnitPtr;

//=================================
// CIFXDeviceTexUnit methods
//=================================
IFXINLINE CIFXDeviceTexUnit::CIFXDeviceTexUnit()
{
  // EMPTY
}

IFXINLINE CIFXDeviceTexUnit::~CIFXDeviceTexUnit()
{
  // EMPTY
}

IFXINLINE IFXRESULT CIFXDeviceTexUnit::Construct()
{
  IFXRESULT rc = IFX_OK;

  IFXRenderTexUnit::InitData();
  CIFXDeviceBase::InitData();
  InitData();

  return rc;
}

IFXINLINE void CIFXDeviceTexUnit::InitData()
{
  m_uTexCoordSet = 0;
}

IFXINLINE IFXRESULT CIFXDeviceTexUnit::Reset()
{
  IFXRenderTexUnit::InitData();
  CIFXDeviceBase::InitData();
  CIFXDeviceTexUnit::InitData();
  return SetDefaults();
}


#endif // CIFX_DEVICE_TEXUNIT_H
