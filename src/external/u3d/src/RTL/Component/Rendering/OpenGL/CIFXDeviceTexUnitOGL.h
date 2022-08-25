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
// CIFXDeviceTexUnitOGL.h
#ifndef CIFX_Device_TEXUNITOGL_H
#define CIFX_Device_TEXUNITOGL_H

#include "CIFXDeviceTexUnit.h"

//==============================
// CIFXDeviceTexUnitOGL
//==============================
class CIFXDeviceTexUnitOGL : public CIFXDeviceTexUnit
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitOGLFactory(IFXREFIID intId, void** ppUnk);

	//====================================
	// CIFXDeviceTexUnitOGL methods
	//====================================
	IFXRESULT IFXAPI Initialize(U32 uTexUnitNum, const CIFXRenderDeviceOGLPtr& spDevice, IFXOpenGL* pOGL);
	virtual IFXRESULT IFXAPI SetHWTexCoordGen (IFXenum eGenMode);
	
protected:
	CIFXDeviceTexUnitOGL();
	virtual ~CIFXDeviceTexUnitOGL();

	CIFXRenderDeviceOGL* m_pDevice;
	U32 m_uTexUnitNum;

	BOOL m_bCubeMap;
	GLenum m_eglTarget;

	IFXMatrix4x4 m_mTexGenMatrix;
	
	virtual IFXRESULT IFXAPI SetHWEnabled (BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWRGBInput0 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWRGBInput1 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWRGBInterpolatorSource(IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWRGBBlendFunc (IFXenum eFunc);
	virtual IFXRESULT IFXAPI SetHWAlphaInput0 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWAlphaInput1 (IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWAlphaInterpolatorSource(IFXenum eInput);
	virtual IFXRESULT IFXAPI SetHWAlphaFunc (IFXenum eFunc);
	virtual IFXRESULT IFXAPI SetHWTextureId (U32 uTexId);
	virtual IFXRESULT IFXAPI SetHWTexRepeatU(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatV(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTexRepeatW(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWTextureTransform (const IFXMatrix4x4 &mTexture);
	virtual IFXRESULT IFXAPI SetHWTexUnit();
	
	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	GLenum IFXAPI GetGLFunc(IFXenum eFunc, F32& fScale);
	GLenum IFXAPI GetGLInput(IFXenum eInput, GLenum& eOperand);

	IFXOpenGL* m_pOGL;
};
typedef IFXSmartPtr<CIFXDeviceTexUnitOGL> CIFXDeviceTexUnitOGLPtr;

#endif // CIFX_Device_TEXUNITOGL_H

// END OF FILE


