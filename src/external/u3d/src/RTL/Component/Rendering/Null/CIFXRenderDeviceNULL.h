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
// CIFXRenderDeviceNULL.h
#ifndef CIFX_RENDER_DEVICE_NULL_H
#define CIFX_RENDER_DEVICE_NULL_H

#include "CIFXRenderDevice.h"
#include "CIFXDeviceLightNULL.h"
#include "CIFXDeviceTextureNULL.h"
#include "CIFXDeviceTexUnitNULL.h"

class CIFXRenderNULL;

class IFXRenderNULLStackEntry
{
public:
	IFXRenderNULLStackEntry() : m_pRender(NULL), m_pNext(NULL), m_pPrev(NULL) {}
	~IFXRenderNULLStackEntry() { IFXDELETE(m_pNext); }
	CIFXRenderNULL* m_pRender;
	IFXRenderNULLStackEntry* m_pNext;
	IFXRenderNULLStackEntry* m_pPrev;
};

class CIFXRenderDeviceNULL : public CIFXRenderDevice
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceNULLFactory(IFXREFIID intId, void** ppUnk);

	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	IFXRESULT IFXAPI GetSupported(U32 uDeviceNum, BOOL& bSupported);

	IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps) const;

	IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType);
	
	IFXRESULT IFXAPI ResetDevice();
	IFXRESULT IFXAPI RegisterWindow(CIFXRenderNULL* pRender);
	IFXRESULT IFXAPI UnregisterWindow(CIFXRenderNULL* pRender);
	
	U32 IFXAPI GetDeviceNum() { return m_uDeviceNum; }

protected:
	CIFXRenderDeviceNULL();
	virtual ~CIFXRenderDeviceNULL();
	
	void IFXAPI InitData();
	IFXRESULT IFXAPI Create();

	IFXRESULT IFXAPI Construct();

	void IFXAPI FillIFXRenderCaps();

	IFXRenderNULLStackEntry* IFXAPI FindStackEntry(CIFXRenderNULL* pRender);
	void IFXAPI RemoveStackEntry(CIFXRenderNULL* pRender);

	// Blend State Settings
	IFXRESULT IFXAPI SetHWTestRef(F32 fRef);
	IFXRESULT IFXAPI SetHWBlendFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWTestFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWAlphaTestEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWBlendEnabled(BOOL bEnabled);

	// Fog Settings
	IFXRESULT IFXAPI SetHWFogEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWMode(IFXenum eMode);
	IFXRESULT IFXAPI SetHWColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWLinearNear(F32 fNear);
	IFXRESULT IFXAPI SetHWFar(F32 fFar);

	// Material Settings
	IFXRESULT IFXAPI SetHWEmissive(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWAmbient(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWDiffuse(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWSpecular(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWShininess(F32 fShininess);
	IFXRESULT IFXAPI SetHWFastSpecular(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWRenderStyle(IFXenum eRenderStyle);
	IFXRESULT IFXAPI SetHWShadeModel(IFXenum eShadeModel);
	IFXRESULT IFXAPI SetHWMaterial();

	// Miscellaneous State Settings
	IFXRESULT IFXAPI SetHWLighting(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthTest(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthWrite(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthCompare(IFXenum eCompare);
	IFXRESULT IFXAPI SetHWCull(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWCullMode(IFXenum eCullMode);
	IFXRESULT IFXAPI SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix);
	IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mViewMatrix);
	IFXRESULT IFXAPI SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient);
	IFXRESULT IFXAPI SetHWConstantColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWNormalization(BOOL bNormalization);

	// Stencil Settings
	IFXRESULT IFXAPI SetHWStencilEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWFailureOp(IFXenum eOp);
	IFXRESULT IFXAPI SetHWZFailureOp(IFXenum eOp);
	IFXRESULT IFXAPI SetHWPassOp(IFXenum eOp);
	IFXRESULT IFXAPI SetHWStencilFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWTestMask(U32 uMask);
	IFXRESULT IFXAPI SetHWWriteMask(U32 uMask);
	IFXRESULT IFXAPI SetHWStencilRef(U32 uRef);

	// View Settings
	IFXRESULT IFXAPI SetHWView();
	void IFXAPI CalcProjectionMatrix();

	BOOL						m_bInitialized;
	U32							m_uDeviceNum;

	IFXRenderCaps				m_RCaps;

	IFXRenderNULLStackEntry*	m_pWindows;

	CIFXDeviceLightNULLPtr		m_pspLightsNULL[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitNULLPtr	m_pspTexUnitsNULL[IFX_MAX_TEXUNITS];
};
typedef IFXSmartPtr<CIFXRenderDeviceNULL> CIFXRenderDeviceNULLPtr;

#endif // CIFX_RENDER_DEVICE_NULL_H

// END OF FILE


