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
// CIFXRender.cpp

#include "IFXRenderPCH.h"
#include "CIFXRenderServices.h"
#include "CIFXRender.h"

#define IFXCompareDevice(hr,a,b,func) {if(! (a == b)) { bDirty = TRUE; hr |= func(b); } }

//===============================================
// Static object definitions for any of the
// IFXRender* helper classes.
//===============================================

U32 CIFXRender::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRender::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRender::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRender)
		{
			*(IFXRender**)ppInterface = (IFXRender*) this;
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

//=========================
// CIFXRender Methods
//=========================
CIFXRender::CIFXRender()
{

}

CIFXRender::~CIFXRender()
{
	IFXRELEASE(m_pServices);

	// Release all textures on the device.  Textures that are needed
	// for other sprites will be reloaded.
	if(m_spDevice.IsValid())
	{
		m_spDevice->DeleteAllTextures();
	}
}

IFXRESULT CIFXRender::Construct()
{
	IFXRESULT rc = IFX_OK;

	InitData();

	return rc;
}

void CIFXRender::InitData()
{
	m_iInitialized = IFX_E_NOT_INITIALIZED;
	m_eAAMode = IFX_NONE;
	m_eDepthStencil = IFX_DEPTH_D16S0;
	m_uNumHWTexUnits = 1;
	CIFXRenderServicesPtr rServices;
	rServices.Create(CID_IFXRenderServices, CID_IFXRenderServices);
	m_pServices = rServices.GetPointerAR();
	m_uDeviceNum = 0;
	m_rcVisibleWindow.Set(0,0,0,0);
	m_bVSyncEnabled = FALSE;
	m_eColorFormat = IFX_BGRA;
	ClearPerformanceData();
}

IFXRESULT CIFXRender::GetCaps(IFXRenderCaps& rCaps)
{
	IFXRESULT rc = IFX_E_NOT_INITIALIZED;

	if(m_spDevice.IsValid())
	{
		rc = IFX_OK;
		rCaps = m_RCaps;
	}

	return rc;
}

IFXRESULT CIFXRender::Initialize(U32 uMonitorNum)
{
	IFXRESULT rc = IFX_OK;

	U32 uNumMonitors = 0;
	const IFXMonitor* pMons = m_pServices->GetMonitors(uNumMonitors);

	if(uMonitorNum >= uNumMonitors)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	IFXRenderDevicePtr rDevice;

	if(IFXSUCCESS(rc))
	{
		m_uDeviceNum = pMons[uMonitorNum].m_uDeviceNum;
		rc = m_pServices->GetRenderDevice(m_idRenderDevice, m_uDeviceNum, rDevice);
	}

	if(IFXSUCCESS(rc))
	{
		rc = rDevice.QI(m_spDevice, CID_IFXRenderDevice);
		rDevice = 0;
	}

	if(IFXSUCCESS(rc))
	{
		m_uNumHWTexUnits = m_spDevice->GetNumHWTexUnits();

		m_spTextures = m_spDevice->GetTextures();

		U32 i = 0;
		for(i = 0; i < IFX_MAX_LIGHTS; i++)
		{
			m_pspLights[i] = m_spDevice->GetLight(i);
		}

		for(i = 0; i < IFX_MAX_TEXUNITS; i++)
		{
			m_pspTexUnits[i] = m_spDevice->GetTexUnit(i);
		}
	}

	if(IFXSUCCESS(rc))
	{
		rc = m_spDevice->GetCaps(m_RCaps);
	}

	return rc;
}

IFXRESULT CIFXRender::GetTransformMatrix(IFXMatrix4x4& rMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid())
	{
		rc = m_spDevice->GetTransformMatrix(rMatrix);
	}
	else
		rc = IFX_E_NOT_INITIALIZED;

	return rc;
}

IFXRESULT CIFXRender::GetPerformanceData(IFXenum eData, U32& ruData)
{
	IFXRESULT rc = IFX_OK;

	switch(eData)
	{
	case IFX_NUM_FACES:
		ruData = m_uNumFacesRendered;
		break;
	case IFX_NUM_VERTICES:
		ruData = m_uNumVerticesRendered;
		break;
	case IFX_NUM_MESHES:
		ruData = m_uNumMeshesRendered;
		break;
	case IFX_NUM_RENDER_CALLS:
		ruData = m_uNumRenderCalls;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXRender::ClearPerformanceData()
{
	m_uNumFacesRendered = 0;
	m_uNumVerticesRendered = 0;
	m_uNumMeshesRendered = 0;
	m_uNumRenderCalls = 0;

	return IFX_OK;
}

IFXenum CIFXRender::GetColorFormat()
{
	return m_eColorFormat;
}

IFXRESULT CIFXRender::Enable(IFXenum eParam)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		if ((eParam >= IFX_LIGHT0) && (eParam < (IFX_LIGHT0+IFX_MAX_LIGHTS))) {
			rc = m_pspLights[eParam - IFX_LIGHT0]->SetEnabled(TRUE);
		} else

		if ((eParam >= IFX_TEXUNIT0) && (eParam < (IFX_TEXUNIT0+IFX_MAX_TEXUNITS))) {
			rc = m_pspTexUnits[eParam - IFX_TEXUNIT0]->SetEnabled(TRUE);
		} else

		switch(eParam)
		{
		case IFX_LIGHTING:
			rc = m_spDevice->SetLighting(TRUE);
			break;
		case IFX_DEPTH_TEST:
			rc = m_spDevice->SetDepthTest(TRUE);
			break;
		case IFX_DEPTH_WRITE:
			rc = m_spDevice->SetDepthWrite(TRUE);
			break;
		case IFX_CULL:
			rc = m_spDevice->SetCull(TRUE);
			break;
		case IFX_FOG:
			rc = m_spDevice->SetFogEnabled(TRUE);
			break;
		case IFX_FB_BLEND:
			rc = m_spDevice->SetBlendEnabled(TRUE);
			break;
		case IFX_FB_ALPHA_TEST:
			rc = m_spDevice->SetAlphaTestEnabled(TRUE);
			break;
		case IFX_STENCIL:
			if(m_eDepthStencil == IFX_DEPTH_D24S8)
				rc = m_spDevice->SetStencilEnabled(TRUE);
			else
				rc = m_spDevice->SetStencilEnabled(FALSE);
			break;
		case IFX_VSYNC:
			rc = SetVSyncEnabled(TRUE);
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXRESULT CIFXRender::Disable(IFXenum eParam)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		if ((eParam >= IFX_LIGHT0) && (eParam < (IFX_LIGHT0+IFX_MAX_LIGHTS))) {
			rc = m_pspLights[eParam - IFX_LIGHT0]->SetEnabled(FALSE);
		} else

		if ((eParam >= IFX_TEXUNIT0) && (eParam < (IFX_TEXUNIT0+IFX_MAX_TEXUNITS))) {
			rc = m_pspTexUnits[eParam - IFX_TEXUNIT0]->SetEnabled(FALSE);
		} else

		switch(eParam)
		{
		case IFX_LIGHTING:
			rc = m_spDevice->SetLighting(FALSE);
			break;
		case IFX_DEPTH_TEST:
			rc = m_spDevice->SetDepthTest(FALSE);
			break;
		case IFX_DEPTH_WRITE:
			rc = m_spDevice->SetDepthWrite(FALSE);
			break;
		case IFX_CULL:
			rc = m_spDevice->SetCull(FALSE);
			break;
		case IFX_FOG:
			rc = m_spDevice->SetFogEnabled(FALSE);
			break;
		case IFX_FB_BLEND:
			rc = m_spDevice->SetBlendEnabled(FALSE);
			break;
		case IFX_FB_ALPHA_TEST:
			rc = m_spDevice->SetAlphaTestEnabled(FALSE);
			break;
		case IFX_STENCIL:
			rc = m_spDevice->SetStencilEnabled(FALSE);
			break;
		case IFX_VSYNC:
			rc = SetVSyncEnabled(FALSE);
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXRESULT CIFXRender::GetEnabled(IFXenum eParam, BOOL& bEnabled)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		if ((eParam >= IFX_LIGHT0) && (eParam < (IFX_LIGHT0+IFX_MAX_LIGHTS))) {
			bEnabled = m_pspLights[eParam - IFX_LIGHT0]->GetEnabled();
		} else

		if ((eParam >= IFX_TEXUNIT0) && (eParam < (IFX_TEXUNIT0+IFX_MAX_TEXUNITS))) {
			bEnabled = m_pspTexUnits[eParam - IFX_TEXUNIT0]->GetEnabled();
		} else

		switch(eParam)
		{
		case IFX_LIGHTING:
			bEnabled = m_spDevice->GetLighting();
			break;
		case IFX_DEPTH_TEST:
			bEnabled = m_spDevice->GetDepthTest();
			break;
		case IFX_DEPTH_WRITE:
			bEnabled = m_spDevice->GetDepthWrite();
			break;
		case IFX_CULL:
			bEnabled = m_spDevice->GetCull();
			break;
		case IFX_FOG:
			bEnabled = FALSE;
			break;
		case IFX_FB_BLEND:
			bEnabled = FALSE;
			break;
		case IFX_FB_ALPHA_TEST:
			bEnabled = FALSE;
			break;
		case IFX_STENCIL:
			bEnabled = FALSE;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXRESULT CIFXRender::Clear(const IFXRenderClear& rClear)
{
	IFXRESULT rc = IFX_OK;

	BOOL bDepthWrite = m_spDevice->GetDepthWrite();

	m_spDevice->SetDepthWrite(TRUE);

	rc = ClearHW(rClear);

	m_spDevice->SetDepthWrite(bDepthWrite);

	return rc;
}

IFXRESULT CIFXRender::MakeCurrent()
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid())
	{
		void* pCurr = m_spDevice->GetCurrent();
		if(pCurr != this)
		{
			m_spDevice->SetCurrent(this);

			rc = MakeHWCurrent();

			m_spDevice->SetWindowSize(m_Window.GetWindowSize());
			m_spDevice->SetVisibleWindow(m_rcVisibleWindow);
		}
	}

	return rc;
}

IFXRESULT CIFXRender::DrawMeshLines(IFXMesh& rMesh)
{
	IFXRESULT rc = IFX_OK;


	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	// First we must make sure that all texture units have texture coordinates flowing to
	// them (if needed).  If the texture unit is not doing auto-texcoord-gen, then we
	// must make sure that either:
	//
	//	a) The mesh has texture coordinates for every unit that is enabled, or if not:
	//
	//	b) Redirect any existing texture coordinates into those units that need them, or:
	//
	//	c) Disable all texture units if there are no tex coords to work with.
	U32 i;
	for( i = 0; i < m_uNumHWTexUnits; i++)
	{
		if(m_pspTexUnits[i]->GetEnabled())
		{
			U32 uTexCoordSet = 0;
			if(m_pspTexUnits[i]->GetTextureCoordinateSet() < 0)
			{
				uTexCoordSet = i;
			}
			else
			{
				uTexCoordSet = (U32) m_pspTexUnits[i]->GetTextureCoordinateSet();
			}

			if(vaAttribs.m_uData.m_uNumTexCoordLayers > uTexCoordSet)
			{
				m_pspTexUnits[i]->SetTexCoordSet(uTexCoordSet);
			}
			else if(vaAttribs.m_uData.m_uNumTexCoordLayers)
			{
				m_pspTexUnits[i]->SetTexCoordSet(0);
			}
			else
			{
				m_pspTexUnits[i]->SetEnabled(FALSE);
			}
		}
	}

	if (IFXSUCCESS(rc))
	{
		if(m_spDevice.IsValid())
		{
			rc = m_spDevice->DrawMesh(rMesh, m_uNumRenderCalls, /*draw lines*/1);
		}
	}

	return rc;
}
  
IFXRESULT CIFXRender::DrawMeshPoints(IFXMesh& rMesh)
{
	IFXRESULT rc = IFX_OK;


	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	// First we must make sure that all texture units have texture coordinates flowing to
	// them (if needed).  If the texture unit is not doing auto-texcoord-gen, then we
	// must make sure that either:
	//
	//	a) The mesh has texture coordinates for every unit that is enabled, or if not:
	//
	//	b) Redirect any existing texture coordinates into those units that need them, or:
	//
	//	c) Disable all texture units if there are no tex coords to work with.
	U32 i;
	for( i = 0; i < m_uNumHWTexUnits; i++)
	{
		if(m_pspTexUnits[i]->GetEnabled())
		{
			U32 uTexCoordSet = 0;
			if(m_pspTexUnits[i]->GetTextureCoordinateSet() < 0)
			{
				uTexCoordSet = i;
			}
			else
			{
				uTexCoordSet = (U32) m_pspTexUnits[i]->GetTextureCoordinateSet();
			}

			if(vaAttribs.m_uData.m_uNumTexCoordLayers > uTexCoordSet)
			{
				m_pspTexUnits[i]->SetTexCoordSet(uTexCoordSet);
			}
			else if(vaAttribs.m_uData.m_uNumTexCoordLayers)
			{
				m_pspTexUnits[i]->SetTexCoordSet(0);
			}
			else
			{
				m_pspTexUnits[i]->SetEnabled(FALSE);
			}
		}
	}

	if (IFXSUCCESS(rc))
	{
		if(m_spDevice.IsValid())
		{
			rc = m_spDevice->DrawMesh(rMesh, m_uNumRenderCalls, /*draw points*/2);
		}
	}

	return rc;
}

IFXRESULT CIFXRender::DrawMesh(IFXMesh& rMesh)
{
	IFXRESULT rc = IFX_OK;

	m_uNumMeshesRendered++;
	m_uNumFacesRendered += rMesh.GetNumFaces();
	m_uNumVerticesRendered += rMesh.GetNumVertices();

	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	// First we must make sure that all texture units have texture coordinates flowing to
	// them (if needed).  If the texture unit is not doing auto-texcoord-gen, then we
	// must make sure that either:
	//
	//	a) The mesh has texture coordinates for every unit that is enabled, or if not:
	//
	//	b) Redirect any existing texture coordinates into those units that need them, or:
	//
	//	c) Disable all texture units if there are no tex coords to work with.
	U32 i;
	for( i = 0; i < m_uNumHWTexUnits; i++)
	{
		if(m_pspTexUnits[i]->GetEnabled())
		{
			U32 uTexCoordSet = 0;
			if(m_pspTexUnits[i]->GetTextureCoordinateSet() < 0)
			{
				uTexCoordSet = i;
			}
			else
			{
				uTexCoordSet = (U32) m_pspTexUnits[i]->GetTextureCoordinateSet();
			}

			if(vaAttribs.m_uData.m_uNumTexCoordLayers > uTexCoordSet)
			{
				m_pspTexUnits[i]->SetTexCoordSet(uTexCoordSet);
			}
			else if(vaAttribs.m_uData.m_uNumTexCoordLayers)
			{
				m_pspTexUnits[i]->SetTexCoordSet(0);
			}
			else
			{
				m_pspTexUnits[i]->SetEnabled(FALSE);
			}
		}
	}

	if (IFXSUCCESS(rc))
	{
		if(m_spDevice.IsValid())
		{
			rc = m_spDevice->DrawMesh(rMesh, m_uNumRenderCalls, 0 /*draw faces*/);
		}
	}

	return rc;
}

IFXRESULT CIFXRender::SetBlend(IFXRenderBlend& rBlend)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetBlend(rBlend);
	}

	return rc;
}

IFXRESULT CIFXRender::SetFog(IFXRenderFog& rFog)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetFog(rFog);
	}

	return rc;
}

IFXRESULT CIFXRender::SetLight(IFXenum eLight, IFXRenderLight& rLight)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		if ((eLight >= IFX_LIGHT0) && (eLight < (IFX_LIGHT0+IFX_MAX_LIGHTS)))
		{
			if(m_pspLights && m_pspLights[eLight - IFX_LIGHT0].IsValid())
			{
				rc = m_pspLights[eLight - IFX_LIGHT0]->SetLight(rLight);
			}
		}
		else
		{
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXRESULT CIFXRender::SetMaterial(IFXRenderMaterial& rMat)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetMaterial(rMat);
	}

	return rc;
}

IFXRESULT CIFXRender::SetStencil(IFXRenderStencil& rStencil)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetStencil(rStencil);
	}

	return rc;
}

IFXRESULT CIFXRender::SetTexture(IFXTextureObject& rTexture)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		U32 uTexId = rTexture.GetId();

		IFXUnknownPtr rUnk;
		CIFXDeviceTexturePtr rTex;

		if(IFXSUCCESS(m_spTextures->GetData(uTexId, rUnk)))
		{
			rc = rUnk.QI(rTex, CID_IFXDeviceTexture);
		}
		else
		{
			rc = CreateTexture(rTex);
			if(IFXSUCCESS(rc))
			{
				rc = rTex.QI(rUnk, IID_IFXUnknown);
			}

			if(IFXSUCCESS(rc))
			{
				rc = m_spTextures->AddData(uTexId, rUnk);
			}
		}

		if(IFXSUCCESS(rc))
		{
			rc = rTex->SetTexture(rTexture);
		}
	}

	return rc;
}

IFXRESULT CIFXRender::SetTextureUnit(IFXenum eTexUnit, IFXRenderTexUnit& rTexUnit)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		if ((eTexUnit >= IFX_TEXUNIT0) && (eTexUnit < (IFX_TEXUNIT0+IFX_MAX_TEXUNITS)))
		{
			if(m_pspTexUnits && m_pspTexUnits[eTexUnit - IFX_TEXUNIT0].IsValid())
			{
				rc = m_pspTexUnits[eTexUnit - IFX_TEXUNIT0]->SetTexUnit(rTexUnit);
			}
		}
		else
		{
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXRESULT CIFXRender::SetConstantColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid())
	{
		rc = m_spDevice->SetConstantColor(vColor);
	}

	return rc;
}

IFXRESULT CIFXRender::SetView(IFXRenderView& rView)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetView(rView);
	}

	return rc;
}

IFXRESULT CIFXRender::SetWindow(IFXRenderWindow& rWindow)
{
	IFXRESULT rc = m_iInitialized;

	BOOL bDirty = FALSE;

	if( IFXSUCCESS(rc) )
	{
		IFXenum eMode = GetCompatibleAAMode(rWindow.GetAntiAliasingMode(), rWindow.GetAntiAliasingEnabled());
		IFXCompareDevice(rc, m_eAAMode, eMode, SetHWAntiAliasingMode);
		eMode = GetCompatibleDepthMode(m_pServices->GetDepthBufferFormat());
		IFXCompareDevice(rc, m_eDepthStencil, eMode, SetHWDepthStencilFormat);
/*		IFXCompareDevice(rc, m_Window.GetDTS(), rWindow.GetDTS(), SetHWDTS);
		IFXCompareDevice(rc, m_Window.GetTransparent(), rWindow.GetTransparent(), SetHWTransparent);
		IFXCompareDevice(rc, m_Window.GetWindowSize(), rWindow.GetWindowSize(), SetHWWindowSize);
		IFXCompareDevice(rc, m_Window.GetWindowPtr(), rWindow.GetWindowPtr(), SetHWWindowPtr);*/

		bDirty = m_Window.SetDirtyWindow(rWindow);
	}

	if(IFXSUCCESS(rc))
	{
		rc = SetHWWindow(bDirty);

		if(IFXSUCCESS(rc))
		{
			rc |= m_spDevice->SetWindowSize(rWindow.GetWindowSize());
			rc |= m_spDevice->SetVisibleWindow(m_rcVisibleWindow);
		}
	}

	return rc;
}

IFXRESULT CIFXRender::GetWindow(IFXRenderWindow& rWindow)
{
	rWindow = m_Window;
	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWAntiAliasingMode(IFXenum eAAMode)
{
	m_eAAMode = eAAMode;

	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWDepthStencilFormat(IFXenum eDepthStencil)
{
	m_eDepthStencil = eDepthStencil;

	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWDTS(BOOL bDTS)
{
	m_Window.SetDTS(bDTS);

	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWTransparent(BOOL bTransparent)
{
	m_Window.SetTransparent(bTransparent);

	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWWindowSize(const IFXRect& rcWindow)
{
	m_Window.SetWindowSize(rcWindow);

	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWWindowPtr(void* pvWindow)
{
	m_Window.SetWindowPtr(pvWindow);

	return IFX_OK;
}

IFXRESULT CIFXRender::SetHWDeviceNum(U32 uDeviceNum)
{
	m_uDeviceNum = uDeviceNum;

	return IFX_OK;
}

void CIFXRender::CalcVisibleWindow()
{
	// Calculate the visible portion of the window
	m_rcVisibleWindow = m_Window.GetWindowSize();
	if(m_rcVisibleWindow.m_X < m_rcBackBuffer.m_X)
	{
		I32 iRight = m_rcVisibleWindow.Right();
		m_rcVisibleWindow.m_X = m_rcBackBuffer.m_X;
		m_rcVisibleWindow.SetRight(iRight);
	}
	if(m_rcVisibleWindow.m_Y < m_rcBackBuffer.m_Y)
	{
		I32 iBottom = m_rcVisibleWindow.Bottom();
		m_rcVisibleWindow.m_Y = m_rcBackBuffer.m_Y;
		m_rcVisibleWindow.SetBottom(iBottom);
	}

	if(m_rcVisibleWindow.Right() > m_rcBackBuffer.Right())
	{
		m_rcVisibleWindow.SetRight(m_rcBackBuffer.Right());
	}
	if(m_rcVisibleWindow.Bottom() > m_rcBackBuffer.Bottom())
	{
		m_rcVisibleWindow.SetBottom(m_rcBackBuffer.Bottom());
	}
}

IFXRESULT CIFXRender::SizeBackBuffer(const IFXRect& rcMonitor)
{
	IFXRESULT rc = IFX_OK;
	const IFXRect& rcWindow = m_Window.GetWindowSize();

	if(m_rcBackBuffer.m_X < rcWindow.m_X)
	{
		U32 uRight = m_rcBackBuffer.Right();
		m_rcBackBuffer.m_X = rcWindow.m_X;
		m_rcBackBuffer.SetRight(uRight);
	}

	if(m_rcBackBuffer.Right() > rcWindow.Right())
	{
		m_rcBackBuffer.SetRight(rcWindow.Right());
	}

	if(m_rcBackBuffer.m_Y < rcWindow.m_Y)
	{
		U32 uBottom = m_rcBackBuffer.Bottom();
		m_rcBackBuffer.m_Y = rcWindow.m_Y;
		m_rcBackBuffer.SetBottom(uBottom);
	}

	if(m_rcBackBuffer.Bottom() > rcWindow.Bottom())
	{
		m_rcBackBuffer.SetBottom(rcWindow.Bottom());
	}

	if(m_rcBackBuffer.m_X < rcMonitor.m_X)
	{
		U32 uRight = m_rcBackBuffer.Right();
		m_rcBackBuffer.m_X = rcMonitor.m_X;
		m_rcBackBuffer.SetRight(uRight);
	}

	if(m_rcBackBuffer.Right() > rcMonitor.Right())
	{
		m_rcBackBuffer.SetRight(rcMonitor.Right());
	}

	if(m_rcBackBuffer.m_Y < rcMonitor.m_Y)
	{
		U32 uBottom = m_rcBackBuffer.Bottom();
		m_rcBackBuffer.m_Y = rcMonitor.m_Y;
		m_rcBackBuffer.SetBottom(uBottom);
	}

	if(m_rcBackBuffer.Bottom() > rcMonitor.Bottom())
	{
		m_rcBackBuffer.SetBottom(rcMonitor.Bottom());
	}

	if(m_rcBackBuffer.m_Width <= 0 || m_rcBackBuffer.m_Height <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}


	return rc;
}

IFXRESULT CIFXRender::SetViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetViewMatrix(mViewMatrix);
	}

	return rc;
}

const IFXMatrix4x4& CIFXRender::GetViewMatrix()
{
	return  m_spDevice->GetViewMatrix();
}



IFXRESULT CIFXRender::SetWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		rc = m_spDevice->SetWorldMatrix(mWorldMatrix);
	}

	return rc;
}

IFXRESULT CIFXRender::SetCullMode(IFXenum eCullMode)
{
	IFXRESULT rc = IFX_OK;

	rc = m_spDevice->SetCullMode(eCullMode);

	return rc;
}

IFXRESULT CIFXRender::SetLineOffset(U32 uOffset)
{
	IFXRESULT rc = IFX_OK;

	rc = m_spDevice->SetLineOffset(uOffset);

	return rc;
}

IFXRESULT CIFXRender::GetGlobalAmbient(IFXVector4& vColor)
{
	vColor = m_spDevice->GetGlobalAmbient();

	return IFX_OK;
}

IFXRESULT CIFXRender::SetGlobalAmbient(const IFXVector4& vColor)
{
	return m_spDevice->SetGlobalAmbient(vColor);
}

IFXRESULT CIFXRender::SetVSyncEnabled(BOOL bVSyncEnabled)
{
	m_bVSyncEnabled = bVSyncEnabled;

	return IFX_OK;
}

IFXRESULT CIFXRender::SetDepthMode(IFXenum eDepthMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid())
	{
		rc = m_spDevice->SetDepthCompare(eDepthMode);
	}

	return rc;
}

IFXRESULT CIFXRender::DeleteTexture(U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid())
	{
		rc = m_spDevice->DeleteTexture(uTexId);
	}

	return rc;
}

IFXenum CIFXRender::GetCompatibleAAMode(IFXenum eAAMode, IFXenum eAAEnabled)
{
	switch(eAAEnabled)
	{
	case IFX_AA_DEFAULT:
		if(m_pServices->GetDefaultAAEnalbed())
			return GetCompatibleAAMode(eAAMode, IFX_AA_ENABLED);
		else
			return IFX_NONE;
		break;
	case IFX_AA_DISABLED:
		return IFX_NONE;
		break;
	}

	switch(eAAMode)
	{
	case IFX_AA_DEFAULT:
		eAAMode = GetCompatibleAAMode(m_pServices->GetDefaultAAMode(), eAAEnabled);
		break;
	case IFX_AA_4X_SW:
		if(!m_RCaps.m_bAA4XSW)
			eAAMode = IFX_NONE;
		break;
	case IFX_AA_4X:
		if(!m_RCaps.m_bAA4X)
			eAAMode = GetCompatibleAAMode(IFX_AA_3X, eAAEnabled);
		break;
	case IFX_AA_3X:
		if(!m_RCaps.m_bAA3X)
			eAAMode = GetCompatibleAAMode(IFX_AA_2X, eAAEnabled);
		break;
	case IFX_AA_2X:
		if(!m_RCaps.m_bAA2X)
			eAAMode = IFX_NONE;
		break;
	}

	return eAAMode;
}

IFXenum CIFXRender::GetCompatibleDepthMode(IFXenum eDepthMode)
{
	switch(eDepthMode)
	{
	case IFX_DEPTH_D16S0:
		if(!m_RCaps.m_b16BitDepth)
			eDepthMode = IFX_NONE;
		break;
	case IFX_DEPTH_D32S0:
		if(!m_RCaps.m_b24BitDepth)
			eDepthMode = GetCompatibleDepthMode(IFX_DEPTH_D16S0);
		break;
	case IFX_DEPTH_D24S8:
		if(!m_RCaps.m_b8BitStencil)
			eDepthMode = GetCompatibleDepthMode(IFX_DEPTH_D32S0);
		break;
	default:
		eDepthMode = IFX_DEPTH_D16S0;
		break;
	}

	return eDepthMode;
}

IFXRESULT CIFXRender::CopyImageData(U8* pSrc, U8* pDst, U32 uSrcPitch, U32 uDstPitch,
									U32 uWidth, U32 uHeight, U32 uBpp, BOOL bFlip)
{
	//IFXRESULT rc = IFX_OK;

	if(bFlip)
	{
		U32 y;
		for( y = 0; y < uHeight; y++)
		{
			memcpy(pDst+((uHeight-1-y)*uDstPitch), pSrc+(y*uSrcPitch), uBpp*uWidth);
		}
	}
	else
	{
		U32 y;
		for( y = 0; y < uHeight; y++)
		{
			memcpy(pDst+(y*uDstPitch), pSrc+(y*uSrcPitch), uBpp*uWidth);
		}
	}

	return IFX_OK;
}
