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
// CIFXRenderDeviceNULL.cpp

#include "IFXRenderPCHNULL.h"
#include "CIFXRenderDeviceNULL.h"
#include "CIFXRenderNULL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceNULLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderDeviceNULL* pPtr = new CIFXRenderDeviceNULL;
		if (pPtr)
		{
			rc = pPtr->Construct();
			if (IFXFAILURE(rc))
				IFXDELETE(pPtr);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if (IFXSUCCESS(rc))
		{
			pPtr->AddRef();
			rc = pPtr->QueryInterface(intId, ppUnk);
			pPtr->Release();
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}
	return rc;
}

U32 CIFXRenderDeviceNULL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDeviceNULL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDeviceNULL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRenderDevice)
		{
			*(IFXRenderDevice**)ppInterface = (IFXRenderDevice*) this;
		}
		else if (interfaceId == CID_IFXRenderDevice)
		{
			*(CIFXRenderDevice**)ppInterface = (CIFXRenderDevice*) this;
		}
		else if (interfaceId == CID_IFXRenderDeviceNULL)
		{
			*(CIFXRenderDeviceNULL**)ppInterface = (CIFXRenderDeviceNULL*) this;
		}
		else if (interfaceId == IID_IFXUnknown)
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

//===========================
// Public Methods
//===========================
CIFXRenderDeviceNULL::CIFXRenderDeviceNULL()
{
	m_refCount = 0;
}

CIFXRenderDeviceNULL::~CIFXRenderDeviceNULL()
{
	IFXDELETE(m_pWindows);
}

void CIFXRenderDeviceNULL::InitData()
{
	m_uDeviceNum = 0;
	m_bInitialized = FALSE;
	m_pWindows = 0;
}

IFXRESULT CIFXRenderDeviceNULL::Construct()
{
	InitData();

	IFXRESULT rc = CIFXRenderDevice::Construct();

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < 8; i++)
		{
			rc = m_pspLightsNULL[i].Create(CID_IFXDeviceLightNULL, CID_IFXDeviceLightNULL);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspLightsNULL[i].QI(m_pspLights[i], CID_IFXDeviceLight);
			}
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsNULL[i].Create(CID_IFXDeviceTexUnitNULL, CID_IFXDeviceTexUnitNULL);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspTexUnitsNULL[i].QI(m_pspTexUnits[i], CID_IFXDeviceTexUnit);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::GetSupported(U32 uDeviceNum, BOOL& bSupported)
{
	IFXRESULT rc = IFX_OK;

	bSupported = TRUE;

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::Initialize(U32 uDeviceNum)
{
	IFXRESULT rc = CIFXRenderDevice::Initialize(uDeviceNum);

	if(FALSE == m_bInitialized)
	{
		m_bInitialized = TRUE;
	}

	CIFXRenderDeviceNULLPtr spDevice;
	spDevice = this;

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLightsNULL[i]->Initialize(i, spDevice);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsNULL[i]->Initialize(i, spDevice,
				(i < 1) ? TRUE : FALSE );
		}
	}

	FillIFXRenderCaps();

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::GetCaps(IFXRenderCaps& rCaps) const
{
	rCaps = m_RCaps;

	return IFX_OK;
}

//=========================================
// Blend Settings
//=========================================
IFXRESULT CIFXRenderDeviceNULL::SetHWTestRef(F32 fRef)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Blend.SetReference(fRef);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		if(IFXSUCCESS(rc))
		{
			rc = m_Blend.SetBlendFunc(eFunc);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWTestFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		if(IFXSUCCESS(rc))
		{
			rc = m_Blend.SetTestFunc(eFunc);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWAlphaTestEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bAlphaTestEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWBlendEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bBlendEnabled = bEnabled;
	}

	return rc;
}

//===============================
// Fog Settings
//===============================
IFXRESULT CIFXRenderDeviceNULL::SetHWFogEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bFogEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWMode(IFXenum eMode)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_Fog.SetMode(eMode);
		rc = SetHWFar(m_Fog.GetFar());
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_Fog.SetColor(vColor);

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWLinearNear(F32 fNear)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_Fog.SetLinearNear(fNear);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWFar(F32 fFar)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_Fog.SetFar(fFar);
	}

	if(m_Fog.GetMode() == IFX_FOG_LINEAR)
	{
		return rc;
	}

	if(IFXSUCCESS(rc))
	{
		//F32 fDensity = CalcFogDensity(m_Fog.GetFar(), m_Fog.GetMode());
	}

	return rc;
}

//===================================
// Material Settings
//===================================
IFXRESULT CIFXRenderDeviceNULL::SetHWEmissive(const IFXVector4& vColor)
{
	m_Material.SetEmissive(vColor);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWAmbient(const IFXVector4& vColor)
{
	m_Material.SetAmbient(vColor);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWDiffuse(const IFXVector4& vColor)
{
	m_Material.SetDiffuse(vColor);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWSpecular(const IFXVector4& vColor)
{
	m_Material.SetSpecular(vColor);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWShininess(F32 fShininess)
{
	m_Material.SetShininess(fShininess);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWFastSpecular(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_Material.SetFastSpecular(bEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWRenderStyle(IFXenum eRenderStyle)
{
	IFXRESULT rc = IFX_OK;

	m_Material.SetRenderStyle(eRenderStyle);

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWShadeModel(IFXenum eShadeModel)
{
	IFXRESULT rc = IFX_OK;

	m_Material.SetShadeModel(eShadeModel);

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWMaterial()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}


//===================================
// Miscellaneous Settings
//===================================
IFXRESULT CIFXRenderDeviceNULL::SetHWLighting(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bLighting = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWDepthTest(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bDepthTest = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWDepthWrite(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bDepthWrite = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWDepthCompare(IFXenum eCompare)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_eDepthCompare = eCompare;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWCull(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(bEnabled)
	{
		rc = SetHWCullMode(m_eCullMode);
	}
	else
	{
		IFXenum eCullMode = m_eCullMode;
		rc = SetHWCullMode(IFX_CULL_NONE);
		m_eCullMode = eCullMode;
	}

	if(IFXSUCCESS(rc))
	{
		m_bCull = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWCullMode(IFXenum eCullMode)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_eCullMode = eCullMode;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	IFXRESULT rc = IFX_OK;

	if ( &m_mViewMatrix != &mViewMatrix )
		m_mViewMatrix = mViewMatrix;

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	IFXRESULT rc = IFX_OK;

	if ( &m_mWorldMatrix != &mWorldMatrix )
		m_mWorldMatrix = mWorldMatrix;

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_vGlobalAmbient = vGlobalAmbient;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWConstantColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_vConstantColor = vColor;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWNormalization(BOOL bNormalization)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bNormalization = bNormalization;
	}

	return rc;
}

//===================================
// Stencil Settings
//===================================
IFXRESULT CIFXRenderDeviceNULL::SetHWStencilEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bStencilEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetFailureOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWZFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetZFailureOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWPassOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetPassOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWStencilFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetStencilFunc(eFunc);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWTestMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetTestMask(uMask);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWWriteMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetWriteMask(uMask);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::SetHWStencilRef(U32 uRef)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetReference(uRef);
	}

	return rc;
}


//=====================================
// View Methods
//=====================================
IFXRESULT CIFXRenderDeviceNULL::SetHWView()
{
	IFXRESULT rc = IFX_OK;

	CalcVisibleViewport();

	return rc;
}

void CIFXRenderDeviceNULL::CalcProjectionMatrix()
{
	F32* pfMat = m_mProjection.Raw();
	IFXenum projectionMode = m_View.GetProjectionMode();

	/// @todo: Hook up the custom width/projectionRect data to the projection matrix calculation.

	if(projectionMode == IFX_PERSPECTIVE3)
	{
		F32 fHalfHeight = 1.0f / tanf(m_View.GetFOVy() * IFXTO_RAD * 0.5f);

		F32 fHalfWidth = fHalfHeight / m_fAspect;

		F32 fTop = m_View.GetNearClip()/(fHalfHeight);
		F32 fBottom = -fTop;

		F32 fRight = m_View.GetNearClip()/(fHalfWidth);
		F32 fLeft = -fRight;

		F32 fWidthScale = (fRight - fLeft) / F32(m_rcBBViewport.m_Width);
		F32 fHeightScale = (fTop - fBottom) / F32(m_rcBBViewport.m_Height);

		F32 fSrcWidth = fRight - fLeft;
		F32 fSrcHeight = fTop - fBottom;

		F32 fClipWidth = F32(m_rcVisibleViewport.m_Width) * fWidthScale;
		F32 fClipX = F32(-m_rcBBViewport.m_X + m_rcVisibleViewport.m_X)*fWidthScale - fSrcWidth*0.5f;
		F32 fClipHeight = F32(m_rcVisibleViewport.m_Height) * fHeightScale;
		F32 fClipY = F32(-m_rcBBViewport.m_Y + m_rcVisibleViewport.m_Y)*fHeightScale - fSrcHeight*0.5f;

		fWidthScale = F32(m_rcVisibleViewport.m_Width) / F32(m_rcBBViewport.m_Width);
		fHeightScale = F32(m_rcVisibleViewport.m_Height) / F32(m_rcBBViewport.m_Height);

		F32 fWidthTrans = fClipX + fClipWidth*0.5f;
		F32 fHeightTrans = fClipY + fClipHeight*0.5f;

		fRight = fRight*fWidthScale - fWidthTrans;
		fLeft = fLeft*fWidthScale - fWidthTrans;
		fTop = fTop*fHeightScale + fHeightTrans;
		fBottom = fBottom*fHeightScale + fHeightTrans;

		pfMat[ 0] = 2*m_View.GetNearClip()/(fRight-fLeft);
		pfMat[ 1] = 0;
		pfMat[ 2] = 0;
		pfMat[ 3] = 0;

		pfMat[ 4] = 0;
		pfMat[ 5] = 2*m_View.GetNearClip()/(fTop-fBottom);
		pfMat[ 6] = 0;
		pfMat[ 7] = 0;

		pfMat[ 8] = (fRight+fLeft)/(fRight-fLeft);
		pfMat[ 9] = (fTop+fBottom)/(fTop-fBottom);
		pfMat[10] = (m_View.GetFarClip())/(m_View.GetFarClip() - m_View.GetNearClip());
		pfMat[11] = 1;

		pfMat[12] = 0;
		pfMat[13] = 0;
		pfMat[14] = -m_View.GetFarClip()*m_View.GetNearClip() / (m_View.GetFarClip() - m_View.GetNearClip());
		pfMat[15] = 0;
	}
	else if(projectionMode == IFX_ORTHOGRAPHIC)
	{
		F32 fOrthoWidth = m_View.GetOrthoHeight() * m_fAspect;

		F32 fRight = fOrthoWidth * 0.5f;
		F32 fLeft = -fRight;

		F32 fTop = m_View.GetOrthoHeight() * 0.5f;
		F32 fBottom = -fTop;

		F32 fWidthScale = (fRight - fLeft) / F32(m_rcBBViewport.m_Width);
		F32 fHeightScale = (fTop - fBottom) / F32(m_rcBBViewport.m_Height);

		F32 fSrcWidth = fRight - fLeft;
		F32 fSrcHeight = fTop - fBottom;

		F32 fClipWidth = F32(m_rcVisibleViewport.m_Width) * fWidthScale;
		F32 fClipX = F32(-m_rcBBViewport.m_X + m_rcVisibleViewport.m_X)*fWidthScale - fSrcWidth*0.5f;
		F32 fClipHeight = F32(m_rcVisibleViewport.m_Height) * fHeightScale;
		F32 fClipY = F32(-m_rcBBViewport.m_Y + m_rcVisibleViewport.m_Y)*fHeightScale - fSrcHeight*0.5f;

		fWidthScale = F32(m_rcVisibleViewport.m_Width) / F32(m_rcBBViewport.m_Width);
		fHeightScale = F32(m_rcVisibleViewport.m_Height) / F32(m_rcBBViewport.m_Height);

		F32 fWidthTrans = fClipX + fClipWidth*0.5f;
		F32 fHeightTrans = fClipY + fClipHeight*0.5f;

		fRight = fRight*fWidthScale + fWidthTrans;
		fLeft = fLeft*fWidthScale + fWidthTrans;
		fTop = fTop*fHeightScale - fHeightTrans;
		fBottom = fBottom*fHeightScale - fHeightTrans;

		pfMat[ 0] = 2 / (fOrthoWidth*fWidthScale);
		pfMat[ 1] = 0;
		pfMat[ 2] = 0;
		pfMat[ 3] = 0;

		pfMat[ 4] = 0;
		pfMat[ 5] = 2 / (m_View.GetOrthoHeight()*fHeightScale);
		pfMat[ 6] = 0;
		pfMat[ 7] = 0;

		pfMat[ 8] = 0;
		pfMat[ 9] = 0;
		pfMat[10] = 1 / (m_View.GetFarClip() - m_View.GetNearClip());
		pfMat[11] = 0;

		pfMat[12] = -(fRight+fLeft)/(fRight-fLeft);
		pfMat[13] = -(fTop+fBottom)/(fTop-fBottom);
		pfMat[14] = -(m_View.GetNearClip())/(m_View.GetFarClip()-m_View.GetNearClip());
		pfMat[15] = 1;
	}
	else
	{
		IFXASSERT( FALSE );
	}
}


//=====================================
// RenderDevice Operational Methods
//=====================================
IFXRESULT CIFXRenderDeviceNULL::DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshType)
{
	IFXRESULT rc = IFX_OK;
	return rc;
}

void CIFXRenderDeviceNULL::FillIFXRenderCaps()
{
	// Clear any existing data
	memset(&m_RCaps, 0, sizeof(IFXRenderCaps));

	m_RCaps.m_uNumHWTexUnits = 1;
	m_RCaps.m_uMaxTextureWidth = 2048;
	m_RCaps.m_uMaxTextureHeight = 2048;

	m_RCaps.m_bCubeTextures = FALSE;

	m_RCaps.m_b16BitColor = TRUE;
	m_RCaps.m_b32BitColor = TRUE;

	m_RCaps.m_b16BitDepth = TRUE;
	m_RCaps.m_b24BitDepth = TRUE;

	m_RCaps.m_b8BitStencil = FALSE;

	m_RCaps.m_bAA4XSW = FALSE;
	m_RCaps.m_bAA2X = FALSE;
	m_RCaps.m_bAA3X = FALSE;
	m_RCaps.m_bAA4X = FALSE;
	m_RCaps.m_bTex4444 = TRUE;
	m_RCaps.m_bTex5551 = TRUE;
	m_RCaps.m_bTex5550 = TRUE;
	m_RCaps.m_bTex5650 = TRUE;
	m_RCaps.m_bTex8880 = TRUE;
	m_RCaps.m_bTex8888 = TRUE;

}

IFXRESULT CIFXRenderDeviceNULL::ResetDevice()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::RegisterWindow(CIFXRenderNULL* pRender)
{
	IFXRESULT rc = IFX_OK;

	if(pRender)
	{
		IFXRenderNULLStackEntry* pEntry = FindStackEntry(pRender);
		if(0 == pEntry)
		{
			pEntry = new IFXRenderNULLStackEntry;
			pEntry->m_pNext = m_pWindows;
			if(m_pWindows)
			{
				m_pWindows->m_pPrev = pEntry;
			}
			m_pWindows = pEntry;

			m_pWindows->m_pRender = pRender;
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceNULL::UnregisterWindow(CIFXRenderNULL* pRender)
{
	IFXRESULT rc = IFX_OK;

	if(pRender)
	{
		RemoveStackEntry(pRender);
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

IFXRenderNULLStackEntry* CIFXRenderDeviceNULL::FindStackEntry(CIFXRenderNULL* pRender)
{
	IFXRenderNULLStackEntry* pReturn = m_pWindows;

	while(pReturn)
	{
		if(pReturn->m_pRender == pRender)
		{
			break;
		}

		pReturn = pReturn->m_pNext;
	}

	return pReturn;
}

void CIFXRenderDeviceNULL::RemoveStackEntry(CIFXRenderNULL* pRender)
{
	IFXRenderNULLStackEntry* pEntry = FindStackEntry(pRender);

	if(pEntry)
	{
		if(m_pWindows == pEntry)
		{
			m_pWindows = pEntry->m_pNext;
		}
		if(pEntry->m_pNext)
		{
			pEntry->m_pNext->m_pPrev = pEntry->m_pPrev;
		}
		if(pEntry->m_pPrev)
		{
			pEntry->m_pPrev->m_pNext = pEntry->m_pNext;
		}

		pEntry->m_pNext = 0;
		pEntry->m_pPrev = 0;

		IFXDELETE(pEntry);
	}
}
