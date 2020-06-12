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
// CIFXRenderDevice.h
#ifndef CIFX_RENDER_DEVICE_H
#define CIFX_RENDER_DEVICE_H

#include "IFXRenderDevice.h"
#include "CIFXDeviceLight.h"
#include "CIFXDeviceTexture.h"
#include "CIFXDeviceTexUnit.h"

class CIFXRenderDevice : virtual public IFXRenderDevice, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);
	IFXRESULT IFXAPI DeleteMesh(U32 uInMeshId);
	IFXRESULT IFXAPI DeleteTexture(U32 uInId);	

	IFXRESULT IFXAPI DeleteAllTextures();

	IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps) const { return IFX_OK; }
	
	const IFXSimpleHashPtr& IFXAPI GetTextures() const;
	const CIFXDeviceLightPtr& IFXAPI GetLight(U32 uLight) const;
	const CIFXDeviceTexUnitPtr& IFXAPI GetTexUnit(U32 uTexUnit) const;

	// U8 uMeshType -> 0 - draw faces, 1 -draw lines, 2 draw points
	virtual IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshType)=0;

	// Blend Settings
	IFXRESULT IFXAPI SetBlendEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetAlphaTestEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetBlend(IFXRenderBlend& rBlend);

	// Fog Settings
	IFXRESULT IFXAPI SetFogEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetFog(IFXRenderFog& rFog);

	// Material Settings
	IFXRESULT IFXAPI SetMaterial(IFXRenderMaterial& rMaterial);

	// Miscellaneous Settings
	IFXRESULT IFXAPI SetLighting(BOOL bEnabled);
	IFXRESULT IFXAPI SetDepthTest(BOOL bEnabled);
	IFXRESULT IFXAPI SetDepthWrite(BOOL bEnabled);
	IFXRESULT IFXAPI SetDepthCompare(IFXenum eCompare);
	IFXRESULT IFXAPI SetCull(BOOL bEnabled);
	IFXRESULT IFXAPI SetCullMode(IFXenum eCullMode);
	IFXRESULT IFXAPI SetLineOffset(U32 uOffset);
	IFXRESULT IFXAPI SetViewMatrix(const IFXMatrix4x4& mViewMatrix);
	const IFXMatrix4x4& GetViewMatrix();
	IFXRESULT IFXAPI SetWorldMatrix(const IFXMatrix4x4& mWorldMatrix);
	IFXRESULT IFXAPI SetGlobalAmbient(const IFXVector4& vGlobalAmbient);
	IFXRESULT IFXAPI SetConstantColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetNormalization(BOOL bNormalization);

	// Stencil Settings
	IFXRESULT IFXAPI SetStencilEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetStencil(IFXRenderStencil& rStencil);

	// View Settings
	IFXRESULT IFXAPI SetWindowSize(const IFXRect& rcWindow);
	IFXRESULT IFXAPI SetVisibleWindow(const IFXRect& rcVisibleWindow);
	IFXRESULT IFXAPI SetView(IFXRenderView& rView);

	// Needed for NPR
	IFXRESULT IFXAPI GetTransformMatrix(IFXMatrix4x4& rMatrix);

	BOOL	IFXAPI GetLighting() const;
	BOOL	IFXAPI GetDepthTest() const;
	BOOL	IFXAPI GetDepthWrite() const;
	IFXenum	IFXAPI GetDepthCompare() const;
	BOOL	IFXAPI GetCull() const;
	IFXenum	IFXAPI GetCullMode() const;
	const IFXMatrix4x4& IFXAPI GetViewMatrix() const;
	const IFXVector4& IFXAPI GetGlobalAmbient() const;

	const IFXRect& IFXAPI GetVisibleViewport() const { return m_rcVisibleViewport; }

	virtual void IFXAPI SetCurrent(void* pCurrent);
	virtual void* IFXAPI GetCurrent() const;

	virtual U32	IFXAPI GetNumHWTexUnits() { return 1; }

	static void IFXAPI NegateMatrixZ(IFXMatrix4x4& mMatrix);

	static IFXRESULT IFXAPI Shutdown();
	
protected:
	CIFXRenderDevice();
	virtual ~CIFXRenderDevice();

	void		IFXAPI		InitData();
	IFXRESULT	IFXAPI		Construct();
	IFXRESULT	IFXAPI		SetDefaults();

	// Blend Methods
	virtual IFXRESULT IFXAPI SetHWBlendEnabled(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWBlendFunc(IFXenum eFunc)=0;
	virtual IFXRESULT IFXAPI SetHWTestRef(F32 fRef)=0;
	virtual IFXRESULT IFXAPI SetHWTestFunc(IFXenum eFunc)=0;
	virtual IFXRESULT IFXAPI SetHWAlphaTestEnabled(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWBlend() { return IFX_OK; }

	// Blend Data
	IFXRenderBlend			m_Blend;
	BOOL					m_bBlendEnabled;
	BOOL					m_bAlphaTestEnabled;
	
	// Fog Methods
	virtual IFXRESULT IFXAPI SetHWFogEnabled(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWMode(IFXenum eMode)=0;
	virtual IFXRESULT IFXAPI SetHWColor(const IFXVector4& vColor)=0;
	virtual IFXRESULT IFXAPI SetHWLinearNear(F32 fNear)=0;
	virtual IFXRESULT IFXAPI SetHWFar(F32 fFar)=0;
	virtual IFXRESULT IFXAPI SetHWFog() { return IFX_OK; }
	F32 IFXAPI CalcFogDensity(F32 fFar, IFXenum eFogMode);

	// Fog Data
	IFXRenderFog			m_Fog;
	BOOL					m_bFogEnabled;

	// Material Methods
	virtual IFXRESULT IFXAPI SetHWUseVertexColors(BOOL bValue) { return m_Material.SetUseVertexColors(bValue); }
	virtual IFXRESULT IFXAPI SetHWEmissive(const IFXVector4& vColor)=0;
	virtual IFXRESULT IFXAPI SetHWAmbient(const IFXVector4& vColor)=0;
	virtual IFXRESULT IFXAPI SetHWDiffuse(const IFXVector4& vColor)=0;
	virtual IFXRESULT IFXAPI SetHWSpecular(const IFXVector4& vColor)=0;
	virtual IFXRESULT IFXAPI SetHWShininess(F32 fShininess)=0;
	virtual IFXRESULT IFXAPI SetHWFastSpecular(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWRenderStyle(IFXenum eRenderStyle)=0;
	virtual IFXRESULT IFXAPI SetHWShadeModel(IFXenum eShadeModel)=0;
	virtual IFXRESULT IFXAPI SetHWMaterial() { return IFX_OK; }

	// Material Data
	IFXRenderMaterial		m_Material;

	// Miscellaneous State methods
	virtual IFXRESULT IFXAPI SetHWLighting(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWDepthTest(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWDepthWrite(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWDepthCompare(IFXenum eCompare)=0;
	virtual IFXRESULT IFXAPI SetHWCull(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWCullMode(IFXenum eCullMode)=0;	
	virtual IFXRESULT IFXAPI SetHWLineOffset(U32 uLineOffset) { m_uLineOffset = uLineOffset; return IFX_OK; }
	virtual IFXRESULT IFXAPI SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix)=0;
	virtual IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)=0;
	virtual IFXRESULT IFXAPI SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient)=0;
	virtual IFXRESULT IFXAPI SetHWConstantColor(const IFXVector4& vColor)=0;
	virtual IFXRESULT IFXAPI SetHWNormalization(BOOL bNormalization)=0;

	// Miscellaneous State Data
	BOOL					m_bLighting;
	BOOL					m_bDepthTest;
	BOOL					m_bDepthWrite;
	IFXenum					m_eDepthCompare;
	BOOL					m_bCull;
	IFXenum					m_eCullMode;
	IFXMatrix4x4			m_mViewMatrix;
	BOOL					m_bViewMatrixScaled;
	IFXMatrix4x4			m_mWorldMatrix;
	BOOL					m_bWorldMatrixScaled;
	IFXVector4				m_vGlobalAmbient;
	IFXVector4				m_vConstantColor;
	BOOL					m_bNormalization;
	U32						m_uLineOffset;
	
	// Stencil Methods
	virtual IFXRESULT IFXAPI SetHWStencilEnabled(BOOL bEnabled)=0;
	virtual IFXRESULT IFXAPI SetHWFailureOp(IFXenum eOp)=0;
	virtual IFXRESULT IFXAPI SetHWZFailureOp(IFXenum eOp)=0;
	virtual IFXRESULT IFXAPI SetHWPassOp(IFXenum eOp)=0;
	virtual IFXRESULT IFXAPI SetHWStencilFunc(IFXenum eFunc)=0;
	virtual IFXRESULT IFXAPI SetHWTestMask(U32 uMask)=0;
	virtual IFXRESULT IFXAPI SetHWWriteMask(U32 uMask)=0;
	virtual IFXRESULT IFXAPI SetHWStencilRef(U32 uRef)=0;
	virtual IFXRESULT IFXAPI SetHWStencil() { return IFX_OK; }

	// Stencil Data
	IFXRenderStencil		m_Stencil;
	BOOL					m_bStencilEnabled;

	// View Settings
	virtual IFXRESULT IFXAPI SetHWProjectionMode(IFXenum eMode) { return m_View.SetProjectionMode(eMode); }
	virtual IFXRESULT IFXAPI SetHWAutoHorizontal(BOOL bAutoHorizontal) { return m_View.SetAutoHorizontalView(bAutoHorizontal); }
	virtual IFXRESULT IFXAPI SetHWUseProjectionRect(BOOL bUseProjRect) { return m_View.SetUseProjectionRect(bUseProjRect); }
	virtual IFXRESULT IFXAPI SetHWFOVx(F32 fFOV) { return m_View.SetFOVx(fFOV); }
	virtual IFXRESULT IFXAPI SetHWFOVy(F32 fFOV) { return m_View.SetFOVy(fFOV); }
	virtual IFXRESULT IFXAPI SetHWOrthoHeight(F32 fHeight) { return m_View.SetOrthoHeight(fHeight); }
	virtual IFXRESULT IFXAPI SetHWOrthoWidth(F32 fWidth) { return m_View.SetOrthoWidth(fWidth); }
	virtual IFXRESULT IFXAPI SetHWNearClip(F32 fNearClip) { return m_View.SetNearClip(fNearClip); }
	virtual IFXRESULT IFXAPI SetHWFarClip(F32 fFarClip) { return m_View.SetFarClip(fFarClip); }
	virtual IFXRESULT IFXAPI SetHWPixelAspect(F32 fPixelAspect) { return m_View.SetPixelAspect(fPixelAspect); }
	virtual IFXRESULT IFXAPI SetHWProjectionRect(const IFXF32Rect& rcProjection) { return m_View.SetProjectionRect(rcProjection); }
	virtual IFXRESULT IFXAPI SetHWViewport(const IFXRect& rcViewport) { return m_View.SetViewport(rcViewport); }
	virtual IFXRESULT IFXAPI SetHWView() { return IFX_OK; }
	void IFXAPI CalcVisibleViewport();

	// View Data
	IFXMatrix4x4			m_mProjection;
	IFXRenderView			m_View;
	IFXRect					m_rcBBViewport;
	IFXRect					m_rcVisibleViewport;
	IFXRect					m_rcWindow;
	IFXRect					m_rcVisibleWindow;
	F32						m_fAspect;
	BOOL 					m_bWindowChanged;

	// Info about current render target
	void*					m_pvCurrent;

	IFXSimpleHashPtr		m_spTextures;
	CIFXDeviceLightPtr		m_pspLights[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitPtr	m_pspTexUnits[IFX_MAX_TEXUNITS];
};
typedef IFXSmartPtr<CIFXRenderDevice> CIFXRenderDevicePtr;

IFXINLINE const IFXSimpleHashPtr& CIFXRenderDevice::GetTextures() const
{
	return m_spTextures;
}

IFXINLINE const CIFXDeviceLightPtr& CIFXRenderDevice::GetLight(U32 uLight) const
{
	return m_pspLights[uLight];
}

IFXINLINE const CIFXDeviceTexUnitPtr& CIFXRenderDevice::GetTexUnit(U32 uTexUnit) const
{
	return m_pspTexUnits[uTexUnit];
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetCull(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;
	
	if(bEnabled != m_bCull)
	{
		rc = SetHWCull(bEnabled);
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetCullMode(IFXenum eCullMode)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_eCullMode != eCullMode)
	{
		switch(eCullMode)
		{
		case IFX_CULL_CW:
		case IFX_CULL_CCW:
			rc = SetHWCullMode(eCullMode);
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetLineOffset(U32 uLineOffset)
{
	IFXRESULT rc = IFX_OK;

	if(m_uLineOffset != uLineOffset)
	{
		rc = SetHWLineOffset(uLineOffset);
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetLighting(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;
	
	if(bEnabled != m_bLighting)
	{
		rc = SetHWLighting(bEnabled);
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetDepthTest(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;
	
	if(bEnabled != m_bDepthTest)
	{
		rc = SetHWDepthTest(bEnabled);
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetDepthWrite(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;
	
	if(bEnabled != m_bDepthWrite)
	{
		rc = SetHWDepthWrite(bEnabled);
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetDepthCompare(IFXenum eCompare)
{
	IFXRESULT rc = IFX_OK;
	
	if(eCompare != m_eDepthCompare)
	{
		rc = SetHWDepthCompare(eCompare);
	}
	
	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(! (m_mViewMatrix == mViewMatrix))
	{
		rc = SetHWViewMatrix(mViewMatrix);

		if(IFXSUCCESS(rc))
		{
			F32 fDet = m_mViewMatrix.CalcDeterminant();
			if(fDet < 0.999f || fDet > 1.001f)
			{
				m_bViewMatrixScaled = TRUE;
			}
			else
			{
				m_bViewMatrixScaled = FALSE;
			}

			rc = SetNormalization(m_bWorldMatrixScaled || m_bViewMatrixScaled);
		}
	}

	return rc;
}

IFXINLINE const IFXMatrix4x4& CIFXRenderDevice::GetViewMatrix()
{
	return m_mViewMatrix;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(! (m_mWorldMatrix == mWorldMatrix))
	{
		rc = SetHWWorldMatrix(mWorldMatrix);

		if(IFXSUCCESS(rc))
		{
			F32 fDet = m_mWorldMatrix.CalcDeterminant();
			if(fDet < 0.999f || fDet > 1.001f)
			{
				m_bWorldMatrixScaled = TRUE;
			}
			else
			{
				m_bWorldMatrixScaled = FALSE;
			}

			rc = SetNormalization(m_bWorldMatrixScaled || m_bViewMatrixScaled);
		}
	}

	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetGlobalAmbient(const IFXVector4& vGlobalAmbient)
{
	IFXRESULT rc = IFX_OK;

	if(! (m_vGlobalAmbient == vGlobalAmbient))
	{
		rc = SetHWGlobalAmbient(vGlobalAmbient);
	}

	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetConstantColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	if(! (m_vConstantColor == vColor))
	{
		rc = SetHWConstantColor(vColor);
	}

	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetNormalization(BOOL bNormalization)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_bNormalization != bNormalization)
	{
		rc = SetHWNormalization(bNormalization);
	}

	return rc;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetWindowSize(const IFXRect& rcWindow)
{
	if(! (m_rcWindow == rcWindow))
	{
		m_bWindowChanged = TRUE;
		m_rcWindow = rcWindow;
	}

	return IFX_OK;
}

IFXINLINE IFXRESULT CIFXRenderDevice::SetVisibleWindow(const IFXRect& rcVisibleWindow)
{
	if(! (m_rcVisibleWindow == rcVisibleWindow))
	{
		m_bWindowChanged = TRUE;
		m_rcVisibleWindow = rcVisibleWindow;
	}

	return IFX_OK;
}

IFXINLINE BOOL CIFXRenderDevice::GetLighting() const
{
	return m_bLighting;
}

IFXINLINE BOOL CIFXRenderDevice::GetDepthTest() const
{
	return m_bDepthTest;
}

IFXINLINE BOOL CIFXRenderDevice::GetDepthWrite() const
{
	return m_bDepthWrite;
}

IFXINLINE IFXenum CIFXRenderDevice::GetDepthCompare() const
{
	return m_eDepthCompare;
}

IFXINLINE BOOL CIFXRenderDevice::GetCull() const
{
	return m_bCull;
}

IFXINLINE IFXenum CIFXRenderDevice::GetCullMode() const
{
	return m_eCullMode;
}

IFXINLINE const IFXMatrix4x4& CIFXRenderDevice::GetViewMatrix() const
{
	return m_mViewMatrix;
}

IFXINLINE const IFXVector4& CIFXRenderDevice::GetGlobalAmbient() const
{
	return m_vGlobalAmbient;
}

IFXINLINE void CIFXRenderDevice::NegateMatrixZ(IFXMatrix4x4& mMatrix)
{
	F32* fMat = mMatrix.Raw();

	fMat[ 2] = -fMat[ 2];
	fMat[ 6] = -fMat[ 6];
	fMat[10] = -fMat[10];
	fMat[14] = -fMat[14];
}

IFXINLINE void CIFXRenderDevice::SetCurrent(void* pCurrent)
{
	m_pvCurrent = pCurrent;
}

IFXINLINE void* CIFXRenderDevice::GetCurrent() const
{
	return m_pvCurrent;
}


#endif // CIFX_RENDER_DEVICE_H

// END OF FILE



