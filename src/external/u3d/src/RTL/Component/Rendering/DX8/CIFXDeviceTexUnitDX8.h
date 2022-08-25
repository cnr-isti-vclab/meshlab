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
// CIFXDeviceTexUnitDX8.h
#ifndef CIFX_DEVICE_TEXUNITDX8_H
#define CIFX_DEVICE_TEXUNITDX8_H

#include "CIFXDeviceTexUnit.h"

//==============================
// CIFXDeviceTexUnitDX8
//==============================
class CIFXDeviceTexUnitDX8 : public CIFXDeviceTexUnit
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitDX8Factory(IFXREFIID intId, void** ppUnk);

	//====================================
	// CIFXDeviceTexUnitDX8 methods
	//====================================
	IFXRESULT IFXAPI Initialize(U32 uTexUnitNum, CIFXRenderDeviceDX8Ptr& spDevice, BOOL bHW = FALSE);
	
	void IFXAPI InitData();

	virtual IFXRESULT IFXAPI SetHWTexCoordGen(IFXenum eGenMode);

protected:
	CIFXDeviceTexUnitDX8();
	virtual ~CIFXDeviceTexUnitDX8();

	U32 IFXAPI GetTexUnitFunc(IFXenum eFunc, IFXenum eInterpSrc = (IFX_CONSTANT|IFX_ALPHA)) const;
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
	virtual IFXRESULT IFXAPI SetHWTexRepeatU(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatV(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatW(BOOL bEnabled);

	virtual IFXRESULT IFXAPI SetHWTexUnit();

	virtual IFXRESULT IFXAPI SetHWTexCoordSet(U32 uTCSet);

	IFXRESULT IFXAPI SetHWMinFilter(IFXenum eMinFilter);
	IFXRESULT IFXAPI SetHWMagFilter(IFXenum eMagFilter);
	IFXRESULT IFXAPI SetHWMipMode(IFXenum eMipMode);

	DWORD IFXAPI GetD3DFilterMode(IFXenum eFilter);
	
	CIFXDeviceTextureDX8* IFXAPI GetTexture(U32 uTexId);

	IFXRESULT IFXAPI Construct();

	CIFXRenderDeviceDX8* m_pDevice;
	U32 m_uTexUnitNum;

	U32 m_uTexGenMode;
	IFXMatrix4x4 m_mTexGenMatrix;

	IFXenum m_eTexMinFilter;
	IFXenum m_eTexMagFilter;
	IFXenum m_eTexMipMode;
	
	BOOL m_bNeedValidation;
	BOOL m_bCubeMap;

	D3DCAPS8 m_Caps;
};
typedef IFXSmartPtr<CIFXDeviceTexUnitDX8> CIFXDeviceTexUnitDX8Ptr;

#endif // CIFX_DEVICE_TEXUNITDX8_H
