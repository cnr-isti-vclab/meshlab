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
// CIFXRenderDeviceOGL.h
#ifndef CIFX_RENDER_DEVICE_OGL_H
#define CIFX_RENDER_DEVICE_OGL_H

#include "CIFXRenderDevice.h"
#include "CIFXDeviceLightOGL.h"
#include "CIFXDeviceTextureOGL.h"
#include "CIFXDeviceTexUnitOGL.h"

#include "IFXSimpleHash.h"

class IFXOpenGL;
class IFXOpenGLOS;
class CIFXRenderOGL;

class CIFXRenderDeviceOGL : public CIFXRenderDevice
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceOGLFactory(IFXREFIID intId, void** ppUnk);
	
	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	IFXRESULT IFXAPI GetSupported(U32 uDeviceNum, BOOL& bSupported);

	IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps) const;

	IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshType);
	
	U32 IFXAPI GetDeviceNum() { return m_uDeviceNum; }

	void* IFXAPI GetCurrent() const;
	
	IFXOpenGL* IFXAPI GetOpenGL() { return m_pOGL; }

	void IFXAPI GetOpenGLData(IFXOGLData& oglData) 
	{
		memcpy(&oglData, &m_glData, sizeof(IFXOGLData));
	}

	void IFXAPI SetOpenGLData(IFXOGLData& oglData)
	{
		memcpy(&m_glData, &oglData, sizeof(IFXOGLData));
		SetDefaults();
	}

	void IFXAPI SetActiveTexUnit(U32 uTexUnit);

	U32	IFXAPI GetNumHWTexUnits() { return m_uNumTexUnits; }

	IFXRESULT IFXAPI MakeDeviceCurrent();
	
	BOOL IFXAPI SetCurrentOGLRender(CIFXRenderOGL* pRender)
	{
		if(m_pCurrentRender != pRender)
		{
			m_pCurrentRender = pRender;
			return TRUE;
		}
		
		return FALSE;
	}

	const IFXMatrix4x4& IFXAPI GetViewMatrix() const { return m_mViewMatrix; }
	const IFXMatrix4x4& IFXAPI GetWorldMatrix() const { return m_mWorldMatrix; }
	
protected:
	CIFXRenderDeviceOGL();
	virtual ~CIFXRenderDeviceOGL();
	
	void IFXAPI InitData();
	IFXRESULT IFXAPI Construct();

	void IFXAPI FillIFXRenderCaps();
	
	GLenum IFXAPI GetDepthFunc(IFXenum eCmp);

	IFXRESULT IFXAPI InitContext();

	// Blend Settings
	IFXRESULT IFXAPI SetHWBlendEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWBlendFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWTestRef(F32 fRef);
	IFXRESULT IFXAPI SetHWTestFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWAlphaTestEnabled(BOOL bEnabled);

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
	
	// Miscellaneous Settings
	IFXRESULT IFXAPI SetHWLighting(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthTest(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthWrite(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthCompare(IFXenum eCompare);
	IFXRESULT IFXAPI SetHWCull(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWCullMode(IFXenum eCullMode);
	IFXRESULT IFXAPI SetHWLineOffset(U32 uOffset);
	IFXRESULT IFXAPI SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix);
	IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix);
	IFXRESULT IFXAPI SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient);
	IFXRESULT IFXAPI SetHWConstantColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWNormalization(BOOL bNormalization);
	
	// Stencil Settings
	IFXRESULT IFXAPI SetHWStencilEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWFailureOp(IFXenum eOp) { return m_Stencil.SetFailureOp(eOp); }
	IFXRESULT IFXAPI SetHWZFailureOp(IFXenum eOp) { return m_Stencil.SetZFailureOp(eOp); }
	IFXRESULT IFXAPI SetHWPassOp(IFXenum eOp) { return m_Stencil.SetPassOp(eOp); }
	IFXRESULT IFXAPI SetHWStencilFunc(IFXenum eFunc) { return m_Stencil.SetStencilFunc(eFunc); }
	IFXRESULT IFXAPI SetHWTestMask(U32 uMask) { return m_Stencil.SetTestMask(uMask); }
	IFXRESULT IFXAPI SetHWWriteMask(U32 uMask) { return m_Stencil.SetWriteMask(uMask); }
	IFXRESULT IFXAPI SetHWStencilRef(U32 uRef) { return m_Stencil.SetReference(uRef); }
	IFXRESULT IFXAPI SetHWStencil();
	GLenum IFXAPI GetGLStencilOp(IFXenum eOp);
	GLenum IFXAPI GetGLStencilFunc(IFXenum eFunc);	

	// View Data
	IFXRESULT IFXAPI SetHWView();
	void IFXAPI CalcProjectionMatrix();

	// Blend Data
	GLenum m_uSrcBlend;
	GLenum m_uDestBlend;
	GLenum m_uTestFunc;

	// Material Data
	BOOL	m_bBlackSpecular;
	BOOL	m_bUsingColorMaterial;

	// OS Specific Implementations
	IFXRESULT IFXAPI DestroyOS();
	IFXRESULT IFXAPI InitializeOS();
	IFXRESULT IFXAPI InitContextOS();
	IFXRESULT IFXAPI InitWindowOS();
	
	IFXOGLData m_glData;

	IFXOpenGL* m_pOGL;
	IFXOpenGLOS* m_pOSOGL;
	
	CIFXRenderOGL* m_pCurrentRender;
	
	U32 m_uDeviceNum;

	IFXMatrix4x4 m_mWorldViewMatrix;

	CIFXDeviceLightOGLPtr		m_pspLightsOGL[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitOGLPtr		m_pspTexUnitsOGL[IFX_MAX_TEXUNITS];

	U32	m_uNumTexUnits;
	U32	m_uActiveTexUnit;

	IFXRenderCaps	m_RCaps;

};
typedef IFXSmartPtr<CIFXRenderDeviceOGL> CIFXRenderDeviceOGLPtr;

#endif // CIFX_RENDER_DEVICE_OGL_H

// END OF FILE


