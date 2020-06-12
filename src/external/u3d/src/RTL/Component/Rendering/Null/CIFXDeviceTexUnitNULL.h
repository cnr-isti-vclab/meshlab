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
// CIFXDeviceTexUnitNULL.h
#ifndef CIFX_DEVICE_TEXUNITNULL_H
#define CIFX_DEVICE_TEXUNITNULL_H

#include "CIFXDeviceTexUnit.h"

//==============================
// CIFXDeviceTexUnitNULL
//==============================
class CIFXDeviceTexUnitNULL : public CIFXDeviceTexUnit
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitNULLFactory(IFXREFIID intId, void** ppUnk);

	//====================================
	// CIFXDeviceTexUnitNULL methods
	//====================================
	IFXRESULT IFXAPI Initialize(U32 uTexUnitNum, CIFXRenderDeviceNULLPtr& spDevice, BOOL bHW = FALSE);
	
protected:
	CIFXDeviceTexUnitNULL();
	virtual ~CIFXDeviceTexUnitNULL();

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
	virtual IFXRESULT IFXAPI SetHWTexCoordGen(IFXenum eGenMode);
	virtual IFXRESULT IFXAPI SetHWTextureTransform(const IFXMatrix4x4& mTrans);
	virtual IFXRESULT IFXAPI SetHWTexRepeatU(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatV(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatW(BOOL bEnabled);

	virtual IFXRESULT IFXAPI SetHWTexUnit();

	virtual IFXRESULT IFXAPI SetHWTexCoordSet(U32 uTCSet);

	IFXRESULT IFXAPI SetHWMinFilter(IFXenum eMinFilter);
	IFXRESULT IFXAPI SetHWMagFilter(IFXenum eMagFilter);
	IFXRESULT IFXAPI SetHWMipMode(IFXenum eMipMode);

	CIFXDeviceTextureNULL* IFXAPI GetTexture(U32 uTexId);

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	CIFXRenderDeviceNULL* m_pDevice;
	U32 m_uTexUnitNum;

	IFXenum m_eTexMinFilter;
	IFXenum m_eTexMagFilter;
	IFXenum m_eTexMipMode;
};
typedef IFXSmartPtr<CIFXDeviceTexUnitNULL> CIFXDeviceTexUnitNULLPtr;

#endif // CIFX_DEVICE_TEXUNITNULL_H
