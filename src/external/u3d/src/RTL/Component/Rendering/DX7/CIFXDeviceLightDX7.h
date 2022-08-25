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
// CIFXDeviceLightDX7.h
#ifndef CIFX_DEVICE_LIGHTDX7_H
#define CIFX_DEVICE_LIGHTDX7_H

#include "CIFXDeviceLight.h"

//==============================
// CIFXDeviceLight
//==============================
class CIFXDeviceLightDX7 : public CIFXDeviceLight
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightDX7Factory(IFXREFIID intId, void** ppUnk);

	//=================================
	// CIFXDeviceLightDX7 methods
	//=================================
	CIFXDeviceLightDX7();
	~CIFXDeviceLightDX7();

	IFXRESULT IFXAPI Initialize(U32 uIndex, CIFXRenderDeviceDX7Ptr& spDevice);
	
protected:

	IFXRESULT IFXAPI SetHWEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWType(IFXenum eType);
	IFXRESULT IFXAPI SetHWAmbient(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWDiffuse(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWSpecular(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWIntensity(F32 fIntensity);
	IFXRESULT IFXAPI SetHWAttenuation(const IFXVector3& vAttenuation);
	IFXRESULT IFXAPI SetHWSpotDecay(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWSpotInnerAngle(F32 fAngle);
	IFXRESULT IFXAPI SetHWSpotOuterAngle(F32 fAngle);
	IFXRESULT IFXAPI SetHWRange(F32 fRange);
	IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix);
	
	IFXRESULT IFXAPI SetHWLight();
	void IFXAPI InitData();
	
	IFXRESULT IFXAPI Construct();
	
	CIFXRenderDeviceDX7* m_pDevice;
	
	D3DLIGHT7			m_Light;
	U32					m_uIndex;

	F32					m_fPhi;
	F32					m_fTheta;
};
typedef IFXSmartPtr<CIFXDeviceLightDX7> CIFXDeviceLightDX7Ptr;

#endif // CIFX_DEVICE_LIGHT_H
