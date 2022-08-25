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
// CIFXDeviceLight.h
#ifndef CIFX_DEVICE_LIGHT_H
#define CIFX_DEVICE_LIGHT_H

#include "IFXRenderLight.h"

//==============================
// CIFXDeviceLight
//==============================
class CIFXDeviceLight : public IFXRenderLight, public CIFXDeviceBase
{
  U32 m_refCount;
public:
  U32 IFXAPI  AddRef ();
  U32 IFXAPI  Release ();
  IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

  //=================================
  // CIFXDeviceLight methods
  //=================================
  virtual IFXRESULT IFXAPI SetLight(IFXRenderLight& rLight);

  static void IFXAPI SetViewMatrix(const IFXMatrix4x4& mViewMatrix);
  static void IFXAPI Update();

  void IFXAPI InitData();

protected:
  CIFXDeviceLight();
  virtual ~CIFXDeviceLight();

  IFXRESULT IFXAPI SetDefaults();

  virtual IFXRESULT IFXAPI SetHWType(IFXenum eType);
  virtual IFXRESULT IFXAPI SetHWAmbient(const IFXVector4& vColor);
  virtual IFXRESULT IFXAPI SetHWDiffuse(const IFXVector4& vColor);
  virtual IFXRESULT IFXAPI SetHWSpecular(const IFXVector4& vColor);
  virtual IFXRESULT IFXAPI SetHWIntensity(F32 fIntensity);
  virtual IFXRESULT IFXAPI SetHWAttenuation(const IFXVector3& vAttenuation);
  virtual IFXRESULT IFXAPI SetHWSpotDecay(BOOL bEnabled);
  virtual IFXRESULT IFXAPI SetHWSpotInnerAngle(F32 fAngle);
  virtual IFXRESULT IFXAPI SetHWSpotOuterAngle(F32 fAngle);
  virtual IFXRESULT IFXAPI SetHWRange(F32 fRange);
  virtual IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix);

  virtual IFXRESULT IFXAPI SetHWLight();

  IFXRESULT IFXAPI Construct();

  IFXMatrix4x4 m_mLocalViewMatrix;

  static IFXMatrix4x4 ms_mViewMatrix;
  static const IFXVector3 ms_vZero;
  static const IFXVector3 ms_vNegativeZ;
  static const IFXVector3 ms_vPositiveZ;
  static BOOL ms_bViewDirty;

public:
  IFXRESULT IFXAPI Reset();
};
typedef IFXSmartPtr<CIFXDeviceLight> CIFXDeviceLightPtr;

IFXINLINE IFXRESULT CIFXDeviceLight::Reset()
{
  InitData();
  return SetDefaults();
}


#endif // CIFX_DEVICE_LIGHT_H

// END OF FILE
