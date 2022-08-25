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
// CIFXRenderDevice.cpp

#include "IFXRenderPCH.h"
#include "CIFXRenderDevice.h"

#define IFXCompareDevice(hr,a,b,func) {if(! (a == b)) { bDirty = TRUE; hr |= func(b); } }
// Uses a saturation value of 1/100 (0.01).
// constant = ln(1/saturation_value)
#define EXPONENTIAL_FOG_CONSTANT			((F32)(4.6051701859880913680359829093687))

// constant = sqrt(ln(1/saturation_value))
#define EXPONENTIAL_SQUARED_FOG_CONSTANT	((F32)(2.14596602628934723963618357029))

U32 CIFXRenderDevice::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDevice::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDevice::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRenderDevice)
		{
			*(IFXRenderDevice**)ppInterface = (IFXRenderDevice*) this;
		}
		else
		if (interfaceId == IID_IFXUnknown)
		{
			*(IFXUnknown**)ppInterface = (IFXUnknown*) this;
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
		if (IFXSUCCESS(result))
			AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}
	return result;
}


IFXRESULT IFXAPI CIFXRenderDevice::Shutdown()
{
	return IFX_OK;
}

IFXRESULT CIFXRenderDevice::Construct()
{
	IFXRESULT rc = IFX_OK;

	InitData();

	if(IFXSUCCESS(rc))
	{
		rc = m_spTextures.Create(CID_IFXSimpleHash, IID_IFXSimpleHash);
	}

	if(IFXSUCCESS(rc))
	{
		rc = m_spTextures->Initialize(64);
	}

	return rc;
}

void CIFXRenderDevice::InitData()
{
	m_bBlendEnabled = FALSE;
	m_bAlphaTestEnabled = FALSE;

	m_bFogEnabled = FALSE;

	m_bLighting = TRUE;
	m_bDepthTest = TRUE;
	m_bDepthWrite = TRUE;
	m_eDepthCompare = IFX_LEQUAL;
	m_bCull = TRUE;
	m_eCullMode = IFX_CULL_CW;
	m_uLineOffset = 0;
	m_pvCurrent = 0;

	m_mViewMatrix.Reset();
	m_mWorldMatrix.Reset();

	m_bViewMatrixScaled = FALSE;
	m_bWorldMatrixScaled = FALSE;
	m_bNormalization = FALSE;

	m_vGlobalAmbient.Set(0,0,0,1);
	m_vConstantColor.Set(0.5f, 0.5f, 0.5f, 0.5f);

	m_rcWindow.Set(0,0,1,1);
	m_rcBBViewport = m_rcWindow;
	m_rcVisibleWindow = m_rcWindow;
	m_rcVisibleViewport = m_rcWindow;
	m_fAspect = 1;
	m_bWindowChanged = FALSE;

	m_bStencilEnabled = FALSE;
}

IFXRESULT CIFXRenderDevice::Initialize(U32 uDeviceNum)
{
	return IFX_OK;
}

IFXRESULT CIFXRenderDevice::GetTransformMatrix(IFXMatrix4x4& mMatrix)
{
	IFXRESULT rc = IFX_OK;

	IFXMatrix4x4 mProj;
	F32* fProj = mProj.Raw();
	IFXenum projectionMode = m_View.GetProjectionMode();

	if(projectionMode == IFX_PERSPECTIVE3)
	{
		F32 fHalfHeight = 1.0f / tanf(m_View.GetFOVy() * IFXTO_RAD * 0.5f);

		F32 fHalfWidth = fHalfHeight / m_fAspect;

		fProj[ 0] = fHalfWidth;
		fProj[ 1] = 0;
		fProj[ 2] = 0;
		fProj[ 3] = 0;

		fProj[ 4] = 0;
		fProj[ 5] = fHalfHeight;
		fProj[ 6] = 0;
		fProj[ 7] = 0;

		fProj[ 8] = 0;
		fProj[ 9] = 0;
		fProj[10] = -(m_View.GetFarClip()+m_View.GetNearClip())/(m_View.GetFarClip() - m_View.GetNearClip());
		fProj[11] = -1;

		fProj[12] = 0;
		fProj[13] = 0;
		fProj[14] = -2*m_View.GetFarClip()*m_View.GetNearClip() / (m_View.GetFarClip() - m_View.GetNearClip());
		fProj[15] = 0;
	}
	else if(projectionMode == IFX_ORTHOGRAPHIC)
	{
		F32 fOrthoWidth = m_View.GetOrthoHeight() * m_fAspect;

		fProj[ 0] = 2 / fOrthoWidth;
		fProj[ 1] = 0;
		fProj[ 2] = 0;
		fProj[ 3] = 0;

		fProj[ 4] = 0;
		fProj[ 5] = 2 / m_View.GetOrthoHeight();
		fProj[ 6] = 0;
		fProj[ 7] = 0;

		fProj[ 8] = 0;
		fProj[ 9] = 0;
		fProj[10] = 1 / (m_View.GetFarClip() - m_View.GetNearClip());
		fProj[11] = 0;

		fProj[12] = 0;
		fProj[13] = 0;
		fProj[14] = -(m_View.GetNearClip())/(m_View.GetFarClip()-m_View.GetNearClip());
		fProj[15] = 1;
	}
	else
	{
		rc = IFX_E_UNSUPPORTED;
		IFXASSERT( FALSE );
	}

	if( IFXSUCCESS(rc) )
	{
		IFXMatrix4x4 mModelView;
		mModelView.Multiply(m_mViewMatrix, m_mWorldMatrix);
		mMatrix.Multiply(mProj, mModelView);
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetDefaults()
{
	IFXRESULT rc = IFX_OK;
	
	// Blend State
	IFXRUN(rc, SetHWBlendEnabled(m_bBlendEnabled));
	IFXRUN(rc, SetHWAlphaTestEnabled(m_bAlphaTestEnabled));
	IFXRUN(rc, SetHWBlendFunc(m_Blend.GetBlendFunc()));
	IFXRUN(rc, SetHWTestFunc(m_Blend.GetTestFunc()));
	IFXRUN(rc, SetHWTestRef(m_Blend.GetReference()));
	IFXRUN(rc, SetHWBlend());

	// Fog State
	IFXRUN(rc, SetHWFogEnabled(m_bFogEnabled));
	IFXRUN(rc, SetHWMode(m_Fog.GetMode()));
	IFXRUN(rc, SetHWColor(m_Fog.GetColor()));
	IFXRUN(rc, SetHWLinearNear(m_Fog.GetLinearNear()));
	IFXRUN(rc, SetHWFar(m_Fog.GetFar()));
	IFXRUN(rc, SetHWFog());

	// Material State
	IFXRUN(rc, SetHWShininess(m_Material.GetShininess()));
	IFXRUN(rc, SetHWFastSpecular(m_Material.GetFastSpecular()));
	IFXRUN(rc, SetHWAmbient(m_Material.GetAmbient()));
	IFXRUN(rc, SetHWDiffuse(m_Material.GetDiffuse()));
	IFXRUN(rc, SetHWEmissive(m_Material.GetEmissive()));
	IFXRUN(rc, SetHWSpecular(m_Material.GetSpecular()));
	IFXRUN(rc, SetHWRenderStyle(m_Material.GetRenderStyle()));
	IFXRUN(rc, SetHWShadeModel(m_Material.GetShadeModel()));
	IFXRUN(rc, SetHWMaterial());

	// Miscellaneous State
	IFXRUN(rc, SetHWLighting(m_bLighting));
	IFXRUN(rc, SetHWDepthTest(m_bDepthTest));
	IFXRUN(rc, SetHWDepthWrite(m_bDepthWrite));
	IFXRUN(rc, SetHWDepthCompare(m_eDepthCompare));
	IFXRUN(rc, SetHWLineOffset(m_uLineOffset));
	IFXRUN(rc, SetHWCull(m_bCull));
	IFXRUN(rc, SetHWCullMode(m_eCullMode));
	IFXRUN(rc, SetHWViewMatrix(m_mViewMatrix));
	IFXRUN(rc, SetHWWorldMatrix(m_mWorldMatrix));
	IFXRUN(rc, SetHWGlobalAmbient(m_vGlobalAmbient));
	IFXRUN(rc, SetHWConstantColor(m_vConstantColor));

	// Stencil State
	IFXRUN(rc, SetHWStencilEnabled(m_bStencilEnabled));
	IFXRUN(rc, SetHWFailureOp(m_Stencil.GetFailureOp()));
	IFXRUN(rc, SetHWPassOp(m_Stencil.GetPassOp()));
	IFXRUN(rc, SetHWZFailureOp(m_Stencil.GetZFailureOp()));
	IFXRUN(rc, SetHWStencilRef(m_Stencil.GetReference()));
	IFXRUN(rc, SetHWTestMask(m_Stencil.GetTestMask()));
	IFXRUN(rc, SetHWWriteMask(m_Stencil.GetWriteMask()));
	IFXRUN(rc, SetHWStencil());

	// View State
	IFXRUN(rc, SetHWProjectionMode(m_View.GetProjectionMode()));
	IFXRUN(rc, SetHWAutoHorizontal(m_View.GetAutoHorizontal()));
	IFXRUN(rc, SetHWUseProjectionRect(m_View.GetUseProjectionRect()));
	IFXRUN(rc, SetHWFOVx(m_View.GetFOVx()));
	IFXRUN(rc, SetHWFOVy(m_View.GetFOVy()));
	IFXRUN(rc, SetHWOrthoHeight(m_View.GetOrthoHeight()));
	IFXRUN(rc, SetHWOrthoWidth(m_View.GetOrthoWidth()));
	IFXRUN(rc, SetHWNearClip(m_View.GetNearClip()));
	IFXRUN(rc, SetHWFarClip(m_View.GetFarClip()));
	IFXRUN(rc, SetHWPixelAspect(m_View.GetPixelAspect()));
	IFXRUN(rc, SetHWProjectionRect(m_View.GetProjectionRect()));
	IFXRUN(rc, SetHWViewport(m_View.GetViewport()));
	IFXRUN(rc, SetHWView());

	return rc;
}

IFXRESULT CIFXRenderDevice::DeleteMesh(U32 uInMeshId)
{
	return IFX_OK;
}

IFXRESULT CIFXRenderDevice::DeleteTexture(U32 uInId)
{
	IFXRESULT rc = IFX_OK;

	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		if(m_pspTexUnits[i]->GetTextureId() == uInId)
		{
			m_pspTexUnits[i]->SetTextureId(1);
		}
	}

	if(m_spTextures.IsValid())
	{
		m_spTextures->RemoveData(uInId);
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::DeleteAllTextures()
{
	IFXRESULT rc = IFX_OK;

	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		if(m_pspTexUnits[i].IsValid())
		{
			m_pspTexUnits[i]->SetTextureId(1);
		}
	}

	if(m_spTextures.IsValid())
		m_spTextures->Clear();

	return rc;
}

IFXRESULT CIFXRenderDevice::SetBlendEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_bBlendEnabled != bEnabled)
	{
		rc = SetHWBlendEnabled(bEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetAlphaTestEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_bAlphaTestEnabled != bEnabled)
	{
		rc = SetHWAlphaTestEnabled(bEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetBlend(IFXRenderBlend& rBlend)
{
	IFXRESULT rc = IFX_OK;

	BOOL bDirty = FALSE;

	IFXCompareDevice(rc, m_Blend.GetBlendFunc(), rBlend.GetBlendFunc(), SetHWBlendFunc);
	IFXCompareDevice(rc, m_Blend.GetTestFunc(), rBlend.GetTestFunc(), SetHWTestFunc);
	IFXCompareDevice(rc, m_Blend.GetReference(), rBlend.GetReference(), SetHWTestRef);

	if(bDirty)
	{
		rc |= SetHWBlend();
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetFogEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_bFogEnabled != bEnabled)
	{
		rc = SetHWFogEnabled(bEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetFog(IFXRenderFog& rFog)
{
	IFXRESULT rc = IFX_OK;

	BOOL bDirty = FALSE;

	IFXCompareDevice(rc, m_Fog.GetMode(), rFog.GetMode(), SetHWMode);
	IFXCompareDevice(rc, m_Fog.GetColor(), rFog.GetColor(), SetHWColor);		
	IFXCompareDevice(rc, m_Fog.GetLinearNear(), rFog.GetLinearNear(), SetHWLinearNear);		
	IFXCompareDevice(rc, m_Fog.GetFar(), rFog.GetFar(), SetHWFar);
		
	if(bDirty)
	{
		rc |= SetHWFog();
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetMaterial(IFXRenderMaterial& rMaterial)
{
	IFXRESULT rc = IFX_OK;

	BOOL bDirty = FALSE;

	IFXCompareDevice(rc, m_Material.GetShininess(), rMaterial.GetShininess(), SetHWShininess);
	IFXCompareDevice(rc, m_Material.GetFastSpecular(), rMaterial.GetFastSpecular(), SetHWFastSpecular);
	IFXCompareDevice(rc, m_Material.GetAmbient(), rMaterial.GetAmbient(), SetHWAmbient);
	IFXCompareDevice(rc, m_Material.GetDiffuse(), rMaterial.GetDiffuse(), SetHWDiffuse);
	IFXCompareDevice(rc, m_Material.GetEmissive(), rMaterial.GetEmissive(), SetHWEmissive);
	IFXCompareDevice(rc, m_Material.GetSpecular(), rMaterial.GetSpecular(), SetHWSpecular);
	IFXCompareDevice(rc, m_Material.GetRenderStyle(), rMaterial.GetRenderStyle(), SetHWRenderStyle);
	IFXCompareDevice(rc, m_Material.GetShadeModel(), rMaterial.GetShadeModel(), SetHWShadeModel);
	IFXCompareDevice(rc, m_Material.GetUseVertexColors(), rMaterial.GetUseVertexColors(), SetHWUseVertexColors);

	if(bDirty)
	{
		rc = SetHWMaterial();
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetStencilEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	
	if(m_bStencilEnabled != bEnabled)
	{
		rc = SetHWStencilEnabled(bEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetStencil(IFXRenderStencil& rStencil)
{
	IFXRESULT rc = IFX_OK;

	BOOL bDirty = FALSE;

	IFXCompareDevice(rc, m_Stencil.GetFailureOp(), rStencil.GetFailureOp(), SetHWFailureOp);
	IFXCompareDevice(rc, m_Stencil.GetPassOp(), rStencil.GetPassOp(), SetHWPassOp);
	IFXCompareDevice(rc, m_Stencil.GetZFailureOp(), rStencil.GetZFailureOp(), SetHWZFailureOp);
	IFXCompareDevice(rc, m_Stencil.GetReference(), rStencil.GetReference(), SetHWStencilRef);
	IFXCompareDevice(rc, m_Stencil.GetTestMask(), rStencil.GetTestMask(), SetHWTestMask);
	IFXCompareDevice(rc, m_Stencil.GetWriteMask(), rStencil.GetWriteMask(), SetHWWriteMask);
	IFXCompareDevice(rc, m_Stencil.GetStencilFunc(), rStencil.GetStencilFunc(), SetHWStencilFunc);
		
	if(bDirty)
	{
		rc = SetHWStencil();
	}

	return rc;
}

IFXRESULT CIFXRenderDevice::SetView(IFXRenderView& rView)
{
	IFXRESULT rc = IFX_OK;

	BOOL bDirty = FALSE;

	IFXCompareDevice(rc, m_View.GetProjectionMode(), rView.GetProjectionMode(), SetHWProjectionMode);
	IFXCompareDevice(rc, m_View.GetAutoHorizontal(), rView.GetAutoHorizontal(), SetHWAutoHorizontal);
	IFXCompareDevice(rc, m_View.GetUseProjectionRect(), rView.GetUseProjectionRect(), SetHWUseProjectionRect);
	IFXCompareDevice(rc, m_View.GetFOVx(), rView.GetFOVx(), SetHWFOVx);
	IFXCompareDevice(rc, m_View.GetFOVy(), rView.GetFOVy(), SetHWFOVy);
	IFXCompareDevice(rc, m_View.GetOrthoHeight(), rView.GetOrthoHeight(), SetHWOrthoHeight);
	IFXCompareDevice(rc, m_View.GetOrthoWidth(), rView.GetOrthoWidth(), SetHWOrthoWidth);
	IFXCompareDevice(rc, m_View.GetNearClip(), rView.GetNearClip(), SetHWNearClip);
	IFXCompareDevice(rc, m_View.GetFarClip(), rView.GetFarClip(), SetHWFarClip);
	IFXCompareDevice(rc, m_View.GetPixelAspect(), rView.GetPixelAspect(), SetHWPixelAspect);
	IFXCompareDevice(rc, m_View.GetProjectionRect(), rView.GetProjectionRect(), SetHWProjectionRect);
	IFXCompareDevice(rc, m_View.GetViewport(), rView.GetViewport(), SetHWViewport);

	if(bDirty || m_bWindowChanged)
	{
		m_bWindowChanged = FALSE;
		rc = SetHWView();
	}

	return rc;
}

//===========================================
// Protected Methods
//===========================================
F32 CIFXRenderDevice::CalcFogDensity(F32 fFar, IFXenum eFogMode)
{
	if(m_Fog.GetMode() == IFX_FOG_EXP)
	{
		return EXPONENTIAL_FOG_CONSTANT/fFar;
	}

	return EXPONENTIAL_SQUARED_FOG_CONSTANT/fFar;
}

void CIFXRenderDevice::CalcVisibleViewport()
{
	// Set viewport relative to the window
	m_rcBBViewport = m_View.GetViewport();

	// Make relative to the visible window
	m_rcBBViewport.m_X += m_rcWindow.m_X - m_rcVisibleWindow.m_X;
	m_rcBBViewport.m_Y += m_rcWindow.m_Y - m_rcVisibleWindow.m_Y;

	// Start the visible portion of the viewport here
	m_rcVisibleViewport = m_rcBBViewport;
	
	// Adjust to fit within the visible window
	if(m_rcVisibleViewport.Right() > m_rcVisibleWindow.m_Width)
	{
		m_rcVisibleViewport.SetRight(m_rcVisibleWindow.m_Width);
	}
	if(m_rcVisibleViewport.Bottom() > m_rcVisibleWindow.m_Height)
	{
		m_rcVisibleViewport.SetBottom(m_rcVisibleWindow.m_Height);
	}
	if(m_rcVisibleViewport.m_X < 0)
	{
		m_rcVisibleViewport.m_Width += m_rcVisibleViewport.m_X;
		m_rcVisibleViewport.m_X -= m_rcVisibleViewport.m_X;
	}
	if(m_rcVisibleViewport.m_Y < 0)
	{
		m_rcVisibleViewport.m_Height += m_rcVisibleViewport.m_Y;
		m_rcVisibleViewport.m_Y -= m_rcVisibleViewport.m_Y;
	}
	
	m_fAspect = m_View.GetPixelAspect() * ((F32)m_View.GetViewport().m_Width / (F32)m_View.GetViewport().m_Height);	
}

CIFXRenderDevice::CIFXRenderDevice()
{
	// EMPTY
}

CIFXRenderDevice::~CIFXRenderDevice()
{
    DeleteAllTextures();
}
