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
// CIFXDeviceTexUnitDX7.h
#ifndef CIFX_DEVICE_TEXUNITDX7_H
#define CIFX_DEVICE_TEXUNITDX7_H

#include "CIFXDeviceTexUnit.h"

//==============================
// CIFXDeviceTexUnitDX7
//==============================
class CIFXDeviceTexUnitDX7 : public CIFXDeviceTexUnit
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitDX7Factory(IFXREFIID intId, void** ppUnk);

	//====================================
	// CIFXDeviceTexUnitDX7 methods
	//====================================
	IFXRESULT IFXAPI Initialize(U32 uTexUnitNum, CIFXRenderDeviceDX7Ptr& spDevice, BOOL bHW = FALSE);

	virtual IFXRESULT IFXAPI SetHWTexCoordGen(IFXenum eGenMode);

protected:
	CIFXDeviceTexUnitDX7();
	virtual ~CIFXDeviceTexUnitDX7();

	U32 IFXAPI GetTexUnitFunc(IFXenum eFunc, IFXenum eInterpSrc) const;
	U32	IFXAPI GetTexUnitInput(IFXenum eInput) const;
	
	virtual IFXRESULT IFXAPI SetHWEnabled(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWRGBInput0 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWRGBInput1 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWRGBInterpolatorSource(IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWRGBBlendFunc (IFXenum eFunc);
	virtual IFXRESULT IFXAPI SetHWAlphaInput0 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWAlphaInput1 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWAlphaInterpolatorSource(IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWAlphaFunc (IFXenum eFunc);
	virtual IFXRESULT IFXAPI SetHWTextureId (U32 uTexId);
	virtual IFXRESULT IFXAPI SetHWTextureTransform(const IFXMatrix4x4& mTrans);

	virtual IFXRESULT IFXAPI SetHWTexCoordSet(U32 uTCSet);
	
	virtual IFXRESULT IFXAPI SetHWTexRepeatU(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatV(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatW(BOOL bEnabled);

	IFXRESULT IFXAPI SetHWMinFilter(IFXenum eMinFilter);
	IFXRESULT IFXAPI SetHWMagFilter(IFXenum eMagFilter);
	IFXRESULT IFXAPI SetHWMipMode(IFXenum eMipMode);

	DWORD IFXAPI GetD3DFilterMode(IFXenum eFilter);
	
	CIFXDeviceTextureDX7* IFXAPI GetTexture(U32 uTexId);

	virtual IFXRESULT IFXAPI SetHWTexUnit();
	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	CIFXRenderDeviceDX7* m_pDevice;
	U32 m_uTexUnitNum;
	U32 m_uTexGenMode;
	IFXMatrix4x4 m_mTexGenMatrix;
	BOOL m_bCubeMap;

	IFXenum m_eTexMipMode;
	IFXenum m_eTexMinFilter;
	IFXenum m_eTexMagFilter;
	
	BOOL m_bNeedValidation;

	D3DDEVICEDESC7 m_Caps;
};
typedef IFXSmartPtr<CIFXDeviceTexUnitDX7> CIFXDeviceTexUnitDX7Ptr;

#endif // CIFX_DEVICE_TEXUNITDX7_H
