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
// CIFXRenderDeviceDX8.cpp

#include "IFXRenderPCHDX8.h"
#include "CIFXRenderDeviceDX8.h"
#include "CIFXRenderDX8.h"
#include "D3DX8.h"


//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceDX8Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderDeviceDX8* pPtr = new CIFXRenderDeviceDX8;
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

U32 CIFXRenderDeviceDX8::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDeviceDX8::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDeviceDX8::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRenderDevice)
		{
			*(IFXRenderDevice**)ppInterface = (IFXRenderDevice*) this;
		}
		else
			if (interfaceId == CID_IFXRenderDevice)
			{
				*(CIFXRenderDevice**)ppInterface = (CIFXRenderDevice*) this;
			}
			else
				if (interfaceId == CID_IFXRenderDeviceDX8)
				{
					*(CIFXRenderDeviceDX8**)ppInterface = (CIFXRenderDeviceDX8*) this;
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

//===========================
// Public Methods
//===========================
CIFXRenderDeviceDX8::CIFXRenderDeviceDX8()
{
	m_refCount = 0;
}

CIFXRenderDeviceDX8::~CIFXRenderDeviceDX8()
{
	ReleaseVidMem();

	IFXDELETE(m_pWindows);
	IFXRELEASE(m_pDevice);
	IFXRELEASE(m_pD3D);
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}

	IFXUnuseRenderWindowClassWin();
}

void CIFXRenderDeviceDX8::InitData()
{
	// Blend Data
	m_uTestFunc = 0;
	m_uSrcBlend = 0;
	m_uDestBlend = 0;

	// Material Data
	memset(&m_D3DUnlitMaterial, 0, sizeof(m_D3DMaterial));
	memset(&m_D3DMaterial, 0, sizeof(m_D3DMaterial));
	m_bBlackSpecular = FALSE;

	memset(&m_Viewport, 0, sizeof(D3DVIEWPORT8));
	m_Viewport.MinZ = 0;
	m_Viewport.MaxZ = 1;

	m_uSmallIndicesUsed = 0;
	m_uDeviceNum = 0;
	m_pD3D = 0;
	m_pDevice = 0;
	m_hWnd = 0;
	m_bInitialized = FALSE;
	m_bInScene = FALSE;
	m_pWindows = 0;
	m_pBigIndexBuffer = 0;
	m_pSmallIndexBuffer = 0;
	m_pCurrentIndexBuffer = 0;
	m_uCurrentFVF = 0;
	m_pStreamSource = 0;
	m_uCurrentIndexOffset = 0;
	memset(&m_Caps, 0, sizeof(D3DCAPS8));
}

IFXRESULT CIFXRenderDeviceDX8::Construct()
{
	InitData();

	IFXUseRenderWindowClassWin();

	IFXRESULT rc = CIFXRenderDevice::Construct();

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < 8; i++)
		{
			rc = m_pspLightsDX8[i].Create(CID_IFXDeviceLightDX8, CID_IFXDeviceLightDX8);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspLightsDX8[i].QI(m_pspLights[i], CID_IFXDeviceLight);
			}
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsDX8[i].Create(CID_IFXDeviceTexUnitDX8, CID_IFXDeviceTexUnitDX8);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspTexUnitsDX8[i].QI(m_pspTexUnits[i], CID_IFXDeviceTexUnit);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::GetSupported(U32 uDeviceNum, BOOL& bSupported)
{
	IFXRESULT rc = IFX_OK;

	bSupported = TRUE;

	if(!m_bInitialized)
	{
		if(m_spDirectX8 == 0)
		{
			rc = m_spDirectX8.Create(CID_IFXDirectX8, IID_IFXDirectX8);
		}
		IFXRUN(rc, m_spDirectX8->LoadDX8());

		if(IFXSUCCESS(rc))
		{
			HRESULT hr = D3D_OK;

			LPDIRECT3D8 pD3D = m_spDirectX8->Direct3DCreate8(D3D_SDK_VERSION);

			if(!pD3D)
			{
				hr = IFX_E_UNSUPPORTED;
			}

			D3DCAPS8 caps;
			if(SUCCEEDED(hr))
			{
				hr = pD3D->GetDeviceCaps(uDeviceNum, D3DDEVTYPE_HAL, &caps);
			}

			if(SUCCEEDED(hr))
			{
				// If MaxStreams == 0 then the driver for this device is not written
				// for DX8+.  We don't want to take a chance on this one, so bail.
				if(caps.MaxStreams == 0)
				{
					rc = IFX_E_UNSUPPORTED;
				}
			}

			/// @todo Determine any other qualities we need in a device to be supported.

			IFXRELEASE(pD3D);

			if(FAILED(hr))
			{
				rc = IFX_E_UNSUPPORTED;
			}
		}
	}

	if(IFXFAILURE(rc))
	{
		bSupported = FALSE;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::Initialize(U32 uDeviceNum)
{
	IFXRESULT rc = CIFXRenderDevice::Initialize(uDeviceNum);

	HRESULT hr = D3D_OK;

	if(FALSE == m_bInitialized)
	{
		if(!m_spDirectX8.IsValid())
		{
			rc = m_spDirectX8.Create(CID_IFXDirectX8, IID_IFXDirectX8);
		}

		if(IFXSUCCESS(rc))
		{
			rc = m_spDirectX8->LoadDX8();
		}

		if(IFXSUCCESS(rc))
		{
			m_uDeviceNum = uDeviceNum;
			m_hWnd = CreateWindow(IFX_RENDER_WINDOW_CLASS, IFX_RENDER_WINDOW_CLASS, WS_CHILD, 0, 0,1, 1,
				GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL);

			m_pD3D = m_spDirectX8->Direct3DCreate8(D3D_SDK_VERSION);

			if(!m_pD3D)
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}

			if(IFXSUCCESS(rc))
			{
				D3DDISPLAYMODE d3ddm;
				hr = m_pD3D->GetAdapterDisplayMode( uDeviceNum, &d3ddm );

				if(SUCCEEDED(hr))
				{
					hr = m_pD3D->GetDeviceCaps(uDeviceNum, D3DDEVTYPE_HAL, &m_Caps);
				}

				DWORD uDeviceFlags = D3DCREATE_FPU_PRESERVE;

				if(SUCCEEDED(hr))
				{
					FillIFXRenderCaps();
					if(m_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
					{
						uDeviceFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
						if(m_Caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
						{
							uDeviceFlags |= D3DCREATE_PUREDEVICE;
						}
					}
					else
					{
						uDeviceFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					}
				}

				D3DPRESENT_PARAMETERS d3dpp;
				memset( &d3dpp, 0, sizeof(d3dpp) );
				d3dpp.Windowed   = TRUE;
				d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
				d3dpp.BackBufferFormat = d3ddm.Format;
				d3dpp.EnableAutoDepthStencil = FALSE;

				hr = m_pD3D->CreateDevice(  uDeviceNum, D3DDEVTYPE_HAL, m_hWnd,
					uDeviceFlags, &d3dpp, &m_pDevice);

				if(FAILED(hr))
				{
					rc = IFX_E_SUBSYSTEM_FAILURE;
				}

			}
		}

		CIFXRenderDeviceDX8Ptr spDevice;
		spDevice = this;

		if(IFXSUCCESS(rc))
		{
			U32 i;
			for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
			{
				rc = m_pspLightsDX8[i]->Initialize(i, spDevice);
			}
		}

		if(IFXSUCCESS(rc))
		{
			U32 i;
			for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
			{
				rc = m_pspTexUnitsDX8[i]->Initialize(i, spDevice,
					(i < m_Caps.MaxSimultaneousTextures) ? TRUE : FALSE );
			}
		}

		IFXRUN(rc, InitVidMem());

		if(IFXSUCCESS(rc))
			m_bInitialized = TRUE;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::GetCaps(IFXRenderCaps& rCaps) const
{
	rCaps = m_RCaps;

	return IFX_OK;
}

//=========================================
// Blend Settings
//=========================================
IFXRESULT CIFXRenderDeviceDX8::SetHWTestRef(F32 fRef)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_ALPHAREF, (DWORD)(fRef * 255));
	if(IFXSUCCESS(rc))
	{
		rc = m_Blend.SetReference(fRef);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	switch(eFunc)
	{
	case IFX_FB_ADD:
		m_uSrcBlend = D3DBLEND_ONE;
		m_uDestBlend = D3DBLEND_ONE;
		break;
	case IFX_FB_MULT:
		m_uSrcBlend = D3DBLEND_ZERO;
		m_uDestBlend = D3DBLEND_SRCCOLOR;
		break;
	case IFX_FB_ALPHA_BLEND:
		m_uSrcBlend = D3DBLEND_SRCALPHA;
		m_uDestBlend = D3DBLEND_INVSRCALPHA;
		break;
	case IFX_FB_INV_ALPHA_BLEND:
		m_uSrcBlend = D3DBLEND_INVSRCALPHA;
		m_uDestBlend = D3DBLEND_SRCALPHA;
		break;
	case IFX_FB_ADD_BLEND:
		m_uSrcBlend = D3DBLEND_SRCALPHA;
		m_uDestBlend = D3DBLEND_ONE;
		break;
	case IFX_FB_INVISIBLE:
		m_uSrcBlend = D3DBLEND_ZERO;
		m_uDestBlend = D3DBLEND_ONE;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		rc = SetRS(D3DRS_SRCBLEND, m_uSrcBlend);
		rc |= SetRS(D3DRS_DESTBLEND, m_uDestBlend);
		if(IFXSUCCESS(rc))
		{
			rc = m_Blend.SetBlendFunc(eFunc);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWTestFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	switch(eFunc)
	{
	case IFX_NEVER:
		m_uTestFunc = D3DCMP_NEVER;
		break;
	case IFX_LESS:
		m_uTestFunc = D3DCMP_LESS;
		break;
	case IFX_GREATER:
		m_uTestFunc = D3DCMP_GREATER;
		break;
	case IFX_EQUAL:
		m_uTestFunc = D3DCMP_EQUAL;
		break;
	case IFX_NOT_EQUAL:
		m_uTestFunc = D3DCMP_NOTEQUAL;
		break;
	case IFX_LEQUAL:
		m_uTestFunc = D3DCMP_LESSEQUAL;
		break;
	case IFX_GEQUAL:
		m_uTestFunc = D3DCMP_GREATEREQUAL;
		break;
	case IFX_ALWAYS:
		m_uTestFunc = D3DCMP_ALWAYS;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		rc = SetRS(D3DRS_ALPHAFUNC, m_uTestFunc);
		if(IFXSUCCESS(rc))
		{
			rc = m_Blend.SetTestFunc(eFunc);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWAlphaTestEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_ALPHATESTENABLE, bEnabled);
	if(IFXSUCCESS(rc))
	{
		m_bAlphaTestEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWBlendEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_ALPHABLENDENABLE, bEnabled);
	if(IFXSUCCESS(rc))
	{
		m_bBlendEnabled = bEnabled;
	}

	return rc;
}

//===============================
// Fog Settings
//===============================
IFXRESULT CIFXRenderDeviceDX8::SetHWFogEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_FOGENABLE, bEnabled);
	if(IFXSUCCESS(rc))
	{
		m_bFogEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWMode(IFXenum eMode)
{
	IFXRESULT rc = IFX_OK;

	U32 uFogMode = D3DFOG_NONE;
	switch(eMode)
	{
	case IFX_FOG_LINEAR:
		uFogMode = D3DFOG_LINEAR;
		break;
	case IFX_FOG_EXP:
		uFogMode = D3DFOG_EXP;
		break;
	case IFX_FOG_EXP2:
		uFogMode = D3DFOG_EXP2;
		break;
	}

	rc = SetRS(D3DRS_FOGVERTEXMODE, uFogMode);
	if(IFXSUCCESS(rc))
	{
		m_Fog.SetMode(eMode);
		rc = SetHWFar(m_Fog.GetFar());
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_Fog.SetColor(vColor);
	rc = SetRS(D3DRS_FOGCOLOR, vColor.GetBGRA());

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWLinearNear(F32 fNear)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_FOGSTART, *(U32*)&fNear);
	if(IFXSUCCESS(rc))
	{
		m_Fog.SetLinearNear(fNear);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWFar(F32 fFar)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_FOGEND, *(U32*)&fFar);
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
		F32 fDensity = CalcFogDensity(m_Fog.GetFar(), m_Fog.GetMode());
		rc = SetRS(D3DRS_FOGDENSITY, *(U32*)&fDensity);
	}

	return rc;
}

//===================================
// Material Settings
//===================================
IFXRESULT CIFXRenderDeviceDX8::SetHWEmissive(const IFXVector4& vColor)
{
	m_Material.SetEmissive(vColor);
	*(IFXVector4*)&m_D3DMaterial.Emissive = vColor;
	*(IFXVector4*)&m_D3DUnlitMaterial.Emissive = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWAmbient(const IFXVector4& vColor)
{
	m_Material.SetAmbient(vColor);
	*(IFXVector4*)&m_D3DMaterial.Ambient = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWDiffuse(const IFXVector4& vColor)
{
	m_Material.SetDiffuse(vColor);
	*(IFXVector4*)&m_D3DMaterial.Diffuse = vColor;
	*(IFXVector4*)&m_D3DUnlitMaterial.Diffuse = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWSpecular(const IFXVector4& vColor)
{
	m_Material.SetSpecular(vColor);
	*(IFXVector4*)&m_D3DMaterial.Specular = vColor;

	m_bBlackSpecular = FALSE;
	const F32* pf = vColor.RawConst();
	if(pf[0] == 0 && pf[1] == 0 && pf[2] == 0)
	{
		m_bBlackSpecular = TRUE;
	}

	if(m_bBlackSpecular)
	{
		SetRS(D3DRS_SPECULARENABLE, FALSE);
	}
	else if(m_Material.GetShininess())
	{
		SetRS(D3DRS_SPECULARENABLE, TRUE);
	}

	SetHWFastSpecular(m_Material.GetFastSpecular());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWShininess(F32 fShininess)
{
	if(m_Material.GetShininess() && !fShininess)
	{
		SetRS(D3DRS_SPECULARENABLE, FALSE);
	}
	if(!m_Material.GetShininess() && fShininess && !m_bBlackSpecular)
	{
		SetRS(D3DRS_SPECULARENABLE, TRUE);
	}
	m_Material.SetShininess(fShininess);
	m_D3DMaterial.Power = fShininess*128.0f;

	SetHWFastSpecular(m_Material.GetFastSpecular());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWFastSpecular(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if((m_Material.GetShininess() && !m_bBlackSpecular) && !bEnabled)
		rc = SetRS(D3DRS_LOCALVIEWER, TRUE);
	else
		rc = SetRS(D3DRS_LOCALVIEWER, FALSE);

	if(IFXSUCCESS(rc))
	{
		m_Material.SetFastSpecular(bEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWRenderStyle(IFXenum eRenderStyle)
{
	IFXRESULT rc = IFX_OK;

	DWORD d3dStyle = D3DFILL_SOLID;
	switch(eRenderStyle)
	{
	case IFX_FILLED:
		d3dStyle = D3DFILL_SOLID;
		break;
	case IFX_WIREFRAME:
		d3dStyle = D3DFILL_WIREFRAME;
		break;
	case IFX_POINTS:
		d3dStyle = D3DFILL_POINT;
		break;
	}

	/// @todo: for nearest modification : we will support customer adjustement point size,
	//D3DRS_POINTSCALEENABLE = FALSE
	// as default the point size == 1.0f

	m_pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(2.0f));

	m_Material.SetRenderStyle(eRenderStyle);
	rc = SetRS(D3DRS_FILLMODE, d3dStyle);

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWShadeModel(IFXenum eShadeModel)
{
	IFXRESULT rc = IFX_OK;

	DWORD d3dStyle = D3DSHADE_GOURAUD;
	switch(eShadeModel)
	{
	case IFX_SMOOTH:
		d3dStyle = D3DSHADE_GOURAUD;
		break;
	case IFX_FLAT:
		d3dStyle = D3DSHADE_FLAT;
		break;
	}

	m_Material.SetShadeModel(eShadeModel);
	rc = SetRS(D3DRS_SHADEMODE, d3dStyle);

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWMaterial()
{
	IFXRESULT rc = IFX_OK;

	HRESULT hr = D3D_OK;

	if(m_bUsingUnlitMaterial)
		hr = m_pDevice->SetMaterial(&m_D3DUnlitMaterial);
	else
		hr = m_pDevice->SetMaterial(&m_D3DMaterial);

	if(FAILED(hr))
	{
		rc = IFX_E_SUBSYSTEM_FAILURE;
	}

	return rc;
}


//===================================
// Miscellaneous Settings
//===================================
IFXRESULT CIFXRenderDeviceDX8::SetHWLighting(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bLighting = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWDepthTest(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if(bEnabled)
		{
			rc = SetRS(D3DRS_ZENABLE, D3DZB_TRUE);
		}
		else
		{
			rc = SetRS(D3DRS_ZENABLE, D3DZB_FALSE);
		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		m_bDepthTest = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWDepthWrite(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		rc = SetRS(D3DRS_ZWRITEENABLE, bEnabled);
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		m_bDepthWrite = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWDepthCompare(IFXenum eCompare)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		U32 uCompare = GetD3DCompare(eCompare);

		rc = SetRS(D3DRS_ZFUNC, uCompare);
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		m_eDepthCompare = eCompare;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWCull(BOOL bEnabled)
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

IFXRESULT CIFXRenderDeviceDX8::SetHWCullMode(IFXenum eCullMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		U32 uCullMode = D3DCULL_NONE;
		switch(eCullMode)
		{
		case IFX_CULL_NONE:
			uCullMode = D3DCULL_NONE;
			break;
		case IFX_CULL_CW:
			uCullMode = D3DCULL_CW;
			break;
		case IFX_CULL_CCW:
			uCullMode = D3DCULL_CCW;
			break;
		}

		rc = SetRS(D3DRS_CULLMODE, uCullMode);
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		m_eCullMode = eCullMode;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if ( &m_mViewMatrix != &mViewMatrix )
			m_mViewMatrix = mViewMatrix;
		NegateMatrixZ(m_mViewMatrix);
		HRESULT hr = m_pDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)m_mViewMatrix.RawConst());
		NegateMatrixZ(m_mViewMatrix);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; i < m_RCaps.m_uNumHWTexUnits; i++)
		{
			IFXenum eTexGen = m_pspTexUnits[i]->GetTexCoordGen();
			if(eTexGen != IFX_NONE)
			{
				m_pspTexUnitsDX8[i]->SetHWTexCoordGen(eTexGen);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if ( &m_mWorldMatrix != &mWorldMatrix )
			m_mWorldMatrix = mWorldMatrix;
		HRESULT hr = m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)m_mWorldMatrix.RawConst());
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; i < m_RCaps.m_uNumHWTexUnits; i++)
		{
			IFXenum eTexGen = m_pspTexUnits[i]->GetTexCoordGen();
			if(eTexGen != IFX_NONE)
			{
				m_pspTexUnitsDX8[i]->SetHWTexCoordGen(eTexGen);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_AMBIENT, vGlobalAmbient.GetBGRA());

	if(IFXSUCCESS(rc))
	{
		m_vGlobalAmbient = vGlobalAmbient;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWConstantColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	U32 uColor = vColor.GetBGRA();

	rc = SetRS(D3DRS_TEXTUREFACTOR, uColor);

	if(IFXSUCCESS(rc))
	{
		m_vConstantColor = vColor;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWNormalization(BOOL bNormalization)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_NORMALIZENORMALS, bNormalization);

	if(IFXSUCCESS(rc))
	{
		m_bNormalization = bNormalization;
	}

	return rc;
}

//===================================
// Stencil Settings
//===================================
IFXRESULT CIFXRenderDeviceDX8::SetHWStencilEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILENABLE, bEnabled);

	if(IFXSUCCESS(rc))
	{
		m_bStencilEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILFAIL, GetStencilOp(eOp));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetFailureOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWZFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILZFAIL, GetStencilOp(eOp));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetZFailureOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWPassOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILPASS, GetStencilOp(eOp));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetPassOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWStencilFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILFUNC, GetD3DCompare(eFunc));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetStencilFunc(eFunc);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWTestMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILMASK, uMask);

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetTestMask(uMask);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWWriteMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILWRITEMASK, uMask);

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetWriteMask(uMask);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetHWStencilRef(U32 uRef)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILREF, uRef);

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetReference(uRef);
	}

	return rc;
}


U32 CIFXRenderDeviceDX8::GetStencilOp(IFXenum eOp)
{
	U32 uOp = D3DSTENCILOP_KEEP;
	switch(eOp)
	{
	case IFX_ZERO:
		uOp = D3DSTENCILOP_ZERO;
		break;
	case IFX_REPLACE:
		uOp = D3DSTENCILOP_REPLACE;
		break;
	case IFX_INCR:
		uOp = D3DSTENCILOP_INCRSAT;
		break;
	case IFX_DECR:
		uOp = D3DSTENCILOP_DECRSAT;
		break;
	case IFX_INVERT:
		uOp = D3DSTENCILOP_INVERT;
		break;
	}

	return uOp;
}

//=====================================
// View Methods
//=====================================
IFXRESULT CIFXRenderDeviceDX8::SetHWView()
{
	IFXRESULT rc = IFX_OK;

	CalcVisibleViewport();

	if(m_rcVisibleViewport.m_Width <= 0 || m_rcVisibleViewport.m_Height <= 0)
	{
		rc = IFX_E_INVALID_VIEWPORT;
	}

	if(IFXSUCCESS(rc))
	{
		m_Viewport.X = m_rcVisibleViewport.m_X;
		m_Viewport.Y = m_rcVisibleViewport.m_Y;
		m_Viewport.Width = m_rcVisibleViewport.m_Width;
		m_Viewport.Height = m_rcVisibleViewport.m_Height;

		CalcProjectionMatrix();

		HRESULT hr = D3D_OK;

		hr = m_pDevice->SetViewport(&m_Viewport);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
		else
		{
			hr = m_pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_mProjection);
			if(FAILED(hr))
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}
		}
	}

	return rc;
}

void CIFXRenderDeviceDX8::CalcProjectionMatrix()
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
IFXRESULT CIFXRenderDeviceDX8::DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshType)
{
	IFXRESULT rc = IFX_OK;
	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	if(!vaAttribs.m_uData.m_bDiffuseIsBGR && vaAttribs.m_uData.m_bHasDiffuseColors)
		IFXRUN(rc, rMesh.SwizzleDiffuseColors());

	if(vaAttribs.m_uData.m_bHasDiffuseColors && m_Material.GetUseVertexColors())
	{
		if(!m_bUsingVertColors)
		{
			IFXRUN(rc, SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1));
			IFXRUN(rc, SetRS(D3DRS_COLORVERTEX, TRUE));
		}

		m_bUsingVertColors = TRUE;
	}
	else
	{
		if(m_bUsingVertColors)
		{
			IFXRUN(rc, SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL));
			IFXRUN(rc, SetRS(D3DRS_COLORVERTEX, FALSE));
		}

		m_bUsingVertColors = FALSE;
	}

	if(m_bLighting || !m_Material.GetUseVertexColors() || !vaAttribs.m_uData.m_bHasDiffuseColors)
	{
		if(!m_bUsingLighting)
			IFXRUN(rc, SetRS(D3DRS_LIGHTING, TRUE));

		m_bUsingLighting = TRUE;

		if(m_bLighting)
		{
			if(m_bUsingUnlitMaterial)
				m_pDevice->SetMaterial(&m_D3DMaterial);

			m_bUsingUnlitMaterial = FALSE;
		}
		else
		{
			if(!m_bUsingUnlitMaterial)
				m_pDevice->SetMaterial(&m_D3DUnlitMaterial);

			m_bUsingUnlitMaterial = TRUE;
		}
	}
	else
	{
		if(m_bUsingLighting)
			IFXRUN(rc, SetRS(D3DRS_LIGHTING, FALSE));

		m_bUsingLighting = FALSE;
	}

	if(vaAttribs.m_uData.m_b32BitIndices || (rMesh.GetNumVertices() > m_Caps.MaxVertexIndex))
	{
		rc = DrawBigMesh(rMesh, uNumRenderCalls, uMeshType);
	}
	else
	{
		IFXInterleavedData* pData = 0;
		rMesh.GetInterleavedVertexData(pData);
		//U32 uTimeStamp = pData->GetTimeStamp();
		U32 uMaxNumVertices = rMesh.GetMaxNumVertices();

		/*if(rMesh.GetNumFaces() > 12 && uTimeStamp && IFXRenderServices::GetTimeStamp() &&
		(uTimeStamp != IFXRenderServices::GetTimeStamp()) &&
		uMaxNumVertices < m_Caps.MaxVertexIndex)
		{
		rc = DrawStaticSmallMesh(rMesh, *pData, uNumRenderCalls, bLines);
		}
		else*/
		{
			//pData->SetTimeStamp(IFXRenderServices::GetTimeStamp());
			DeleteStaticVB(pData->GetId());
			rc = DrawSmallMesh(rMesh, *pData, uNumRenderCalls, uMeshType);
		}

		IFXRELEASE(pData);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::DrawStaticSmallMesh( IFXMesh& rMesh,
												   IFXInterleavedData& rVerts,
												   U32& uNumRenderCalls,
												   U8 uMeshDrawType)
{
	IFXRESULT rc = IFX_OK;

	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	DWORD fvf = BuildFVF(vaAttribs);

	IFXDX8StaticVB* pVerts = 0;
	rc = GetStaticVertexBuffer( fvf, rVerts.GetId(), rVerts.GetStride(),
		rMesh.GetMaxNumVertices(), pVerts);

	if(IFXSUCCESS(rc))
	{
		if(!pVerts->m_uSrcDataVersion)
		{
			rc = CopyVerticesToStaticBuffer(rVerts, rMesh.GetMaxNumVertices(), *pVerts);
		}

		IFXASSERTBOX(pVerts->m_uSrcDataVersion == rVerts.GetVersionWord(0),
			"Drawing Dirty Static Mesh Data!!!");
	}

	IFXRUN(rc, SetStreamSource(pVerts->m_pVB, rVerts.GetStride()));
	IFXRUN(rc, SetFVF(fvf));

	IFXRUN(rc, RenderSmallMesh(rMesh, 0, uNumRenderCalls, uMeshDrawType));

	if(pVerts)
	{
		if(pVerts->m_pVB)
		{
			pVerts->m_pVB->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::DrawSmallMesh( IFXMesh& rMesh,
											 IFXInterleavedData& rVerts,
											 U32& uNumRenderCalls,
											 U8 uMeshDrawType)
{
	IFXRESULT rc = IFX_OK;

	U32 uVertexBufferStart = 0;

	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	DWORD fvf = BuildFVF(vaAttribs);

	IFXDX8VB* pVerts = 0;
	rc = GetVertexBuffer(fvf, rVerts.GetStride(), pVerts);

	IFXRUN(rc, CopyVerticesToBuffer(rVerts, *pVerts, uVertexBufferStart, 0, rMesh.GetNumVertices()));
	IFXRUN(rc, SetStreamSource(pVerts->m_pVB, rVerts.GetStride()));
	IFXRUN(rc, SetFVF(fvf));

	IFXRUN(rc, RenderSmallMesh(rMesh, uVertexBufferStart, uNumRenderCalls, uMeshDrawType));

	if(pVerts)
	{
		if(pVerts->m_pVB)
		{
			pVerts->m_pVB->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::RenderSmallMesh( IFXMesh& rMesh,
											   U32 uVBStart,
											   U32& uNumRenderCalls,
											   U8 uMeshDrawType)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	U32 uNumFacesRendered = 0;
	U32 uTotalFaces = rMesh.GetNumFaces();

	if(uMeshDrawType==1) // draw lines
		uTotalFaces = rMesh.GetNumLines();

	if(uMeshDrawType==2) // draw points
		uTotalFaces = rMesh.GetNumVertices();

	while((uNumFacesRendered < uTotalFaces) && IFXSUCCESS(rc))
	{
		U32 uNumFacesToRender = uTotalFaces - uNumFacesRendered;
		if(uNumFacesToRender > m_Caps.MaxPrimitiveCount)
		{
			uNumFacesToRender = m_Caps.MaxPrimitiveCount;
		}

		U32 uIndexBufferStart = 0;
		IFXInterleavedData* pIndices = 0;
		if(uMeshDrawType==1) // draw lines
		{
			IFXRUN(rc, rMesh.GetMeshData(IFX_MESH_LINE, pIndices));
		}
		else if (uMeshDrawType==0) // draw faces
		{
			IFXRUN(rc, rMesh.GetMeshData(IFX_MESH_FACE, pIndices));
		} else // draw points
		{
			IFXRUN(rc, rMesh.GetMeshData(IFX_MESH_POINT, pIndices));
		}

		IFXRUN(rc, CopyIndicesToBuffer(*pIndices, uIndexBufferStart, uNumFacesRendered, uNumFacesToRender, uMeshDrawType));
		IFXRUN(rc, SetIndices(m_pSmallIndexBuffer, uVBStart));

		IFXRELEASE(pIndices);

		if(SUCCEEDED(hr))
		{
			if(uMeshDrawType==0)
			{
				hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
					0, rMesh.GetNumVertices(),
					uIndexBufferStart, uNumFacesToRender );
			}
			else if (uMeshDrawType==1)
			{
				hr = m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST,
					0, rMesh.GetNumVertices(),
					uIndexBufferStart, uNumFacesToRender );
			} else // (uMeshDrawType==2)
			{
				hr = m_pDevice->DrawIndexedPrimitive(D3DPT_POINTLIST,
					0, rMesh.GetNumVertices(),
					uIndexBufferStart, uNumFacesToRender );
			}

			uNumRenderCalls++;
		}

		uNumFacesRendered += uNumFacesToRender;

		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::DrawBigMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	DWORD fvf = BuildFVF(vaAttribs);

	IFXVertexIter vIter;
	rMesh.GetVertexIter(vIter);

	IFXU16FaceIter f16Iter;
	IFXU32FaceIter f32Iter;

	IFXU16LineIter l16Iter;
	IFXU32LineIter l32Iter;

	IFXDX8VB* pVerts = 0;
	rc = GetVertexBuffer(fvf, vIter.GetStride(), pVerts);

	IFXRUN(rc, SetStreamSource(pVerts->m_pVB, vIter.GetStride()));
	IFXRUN(rc, SetFVF(fvf));

	// Now we need to copy the mesh data into the vertex buffer in the order
	// that they are indexed by the face set.

	U32 uNumFacesRendered = 0;
	U32 uNumTotalFaces = rMesh.GetNumFaces();

	if(uMeshDrawType==1)
		uNumTotalFaces = rMesh.GetNumLines();

	if(uMeshDrawType==2)
		uNumTotalFaces = rMesh.GetNumVertices();

	while((uNumFacesRendered < uNumTotalFaces) && IFXSUCCESS(rc))
	{
		U8* pDst;
		U32 uNumFacesToRender = uNumTotalFaces - uNumFacesRendered;

		if(uMeshDrawType==1)
		{
			if(uNumFacesToRender > (IFX_MAX_NUM_VERTICES / 2))
			{
				uNumFacesToRender = (IFX_MAX_NUM_VERTICES / 2);
			}
		}
		else if(uMeshDrawType==0)
		{
			if(uNumFacesToRender > (IFX_MAX_NUM_VERTICES / 3))
			{
				uNumFacesToRender = (IFX_MAX_NUM_VERTICES / 3); // This is divisible by 3, so it works.
			}
		} else // if(uMeshDrawType==2)
		{
			if(uNumFacesToRender > IFX_MAX_NUM_VERTICES )
			{
				uNumFacesToRender = IFX_MAX_NUM_VERTICES;
			}
		}

		hr = pVerts->m_pVB->Lock(0, 0, (BYTE**)&pDst, D3DLOCK_DISCARD);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		IFXInterleavedData* pData = 0;
		if(IFXSUCCESS(rc))
		{
			rc = rMesh.GetInterleavedVertexData(pData);
		}

		if(IFXSUCCESS(rc))
		{
			IFXVector3Iter vPosIter;
			pData->GetVectorIter(0, vPosIter);
			if(vaAttribs.m_uData.m_b32BitIndices)
			{
				if(uMeshDrawType==0)
				{
					rMesh.GetU32FaceIter(f32Iter);
					f32Iter.PointAt(uNumFacesRendered);
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					IFXU32Face* pFace = 0;
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++)
					{
						pFace = f32Iter.Next();

						// Vertex A
						pSrc = (U8*)vPosIter.Index(pFace->VertexA());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;

						// Vertex B
						pSrc = (U8*)vPosIter.Index(pFace->VertexB());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;

						// Vertex C
						pSrc = (U8*)vPosIter.Index(pFace->VertexC());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}
				}
				else if(uMeshDrawType==1)
				{
					rMesh.GetU32LineIter(l32Iter);
					l32Iter.PointAt(uNumFacesRendered);
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					IFXU32Line* pLine = 0;
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++)
					{
						pLine = l32Iter.Next();

						// Vertex A
						pSrc = (U8*)vPosIter.Index(pLine->VertexA());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;

						// Vertex B
						pSrc = (U8*)vPosIter.Index(pLine->VertexB());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}
				} else {
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					vPosIter.PointAt(uNumFacesRendered);
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++)
					{

						// Vertex
						pSrc = (U8*)vPosIter.Next();
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}

				}

			}
			else
			{
				if(uMeshDrawType==0)
				{
					rMesh.GetU16FaceIter(f16Iter);
					f16Iter.PointAt(uNumFacesRendered);
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					IFXU16Face* pFace = 0;
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++)
					{
						pFace = f16Iter.Next();

						// Vertex A
						pSrc = (U8*)vPosIter.Index(pFace->VertexA());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;

						// Vertex B
						pSrc = (U8*)vPosIter.Index(pFace->VertexB());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;

						// Vertex C
						pSrc = (U8*)vPosIter.Index(pFace->VertexC());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}
				}
				else if(uMeshDrawType==1)
				{
					rMesh.GetU16LineIter(l16Iter);
					l16Iter.PointAt(uNumFacesRendered);
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					IFXU16Line* pLine = 0;
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++)
					{
						pLine = l16Iter.Next();

						// Vertex A
						pSrc = (U8*)vPosIter.Index(pLine->VertexA());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;

						// Vertex B
						pSrc = (U8*)vPosIter.Index(pLine->VertexB());
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}
				} else {
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					vPosIter.PointAt(uNumFacesRendered);
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++)
					{
						// Vertex
						pSrc = (U8*)vPosIter.Next();
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}

				}

			}

			hr = pVerts->m_pVB->Unlock();
			if(FAILED(hr))
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}
		}

		IFXRELEASE(pData);

		if(IFXSUCCESS(rc))
		{
			uNumRenderCalls++;
			if(uMeshDrawType==0)
			{
				hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, uNumFacesToRender);
			}
			else if (uMeshDrawType==1)
			{
				hr = m_pDevice->DrawPrimitive(D3DPT_LINELIST, 0, uNumFacesToRender);
			} else // uMeshDrawType==2
			{
				hr = m_pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, uNumFacesToRender);
			}

			if(FAILED(hr))
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}

			uNumFacesRendered += uNumFacesToRender;
		}
	}

	if(pVerts)
	{
		pVerts->m_uVertsUsed = IFX_MAX_NUM_VERTICES;
		if(pVerts->m_pVB)
		{
			pVerts->m_pVB->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::CopyIndicesToBuffer( IFXInterleavedData& rIndices,
												   U32& uIBStart,
												   U32 uPrimitiveStart,
												   U32 uNumPrimitives,
												   U8 uMeshDrawType)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	U8* pSrcData = 0;
	U8* pDstData = 0;

	IFXU16FaceIter fIter;
	IFXU16LineIter lIter;
	IFXU32Iter     pIter; //

	U32 uNumIndices = uNumPrimitives * 3;
	U32 uPrimSize = sizeof(IFXU16Face);

	if(uMeshDrawType == 0) // draw faces
	{
		rIndices.GetVectorIter(0, fIter);
		pSrcData = (U8*)fIter.Get();
	}
	else if (uMeshDrawType == 1) // draw lines
	{
		rIndices.GetVectorIter(0, lIter);
		pSrcData = (U8*)lIter.Get();
		uPrimSize = sizeof(IFXU16Line);
		uNumIndices = uNumPrimitives * 2;
	}
	else if(uMeshDrawType == 2) // draw points
	{
		uNumIndices = uNumPrimitives;
		uPrimSize = sizeof(U16);
		pSrcData = (U8*)IFXAllocate(uPrimSize*uNumIndices);
		U32 ind;
		for ( ind = 0; ind < uNumIndices; ind++)  {
			((U16*)pSrcData)[ind] = ind;
		}

	} else  {
		IFXASSERT(0);
	}

	pSrcData +=  uPrimitiveStart*uPrimSize;

	if(uNumIndices <= (IFX_MAX_NUM_INDICES - m_uSmallIndicesUsed))
	{
		hr = m_pSmallIndexBuffer->Lock( m_uSmallIndicesUsed * 2,
			uNumPrimitives * uPrimSize,
			(BYTE**)&pDstData,
			D3DLOCK_NOOVERWRITE );
		uIBStart = m_uSmallIndicesUsed;
		m_uSmallIndicesUsed += uNumIndices;
	}
	else
	{
		// Lock entire buffer
		hr = m_pSmallIndexBuffer->Lock( 0, 0, (BYTE**)&pDstData,
			D3DLOCK_DISCARD );
		uIBStart = 0;
		m_uSmallIndicesUsed = uNumIndices;
	}

	if(FAILED(hr))
	{
		rc = IFX_E_SUBSYSTEM_FAILURE;
	}

	if(IFXSUCCESS(rc))
	{
		memcpy(pDstData, pSrcData, 2 * uNumIndices);
	}

	if (uMeshDrawType == 2 && pSrcData) {
		IFXDeallocate(pSrcData);
	}
	if(IFXSUCCESS(rc))
	{
		hr = m_pSmallIndexBuffer->Unlock();
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::CopyVerticesToBuffer(  IFXInterleavedData& rVertices,
													IFXDX8VB& rVertBuf,
													U32& uVBStart,
													U32 uStart,
													U32 uNumVertices )
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	U8* pSrcData = 0;
	U8* pDstData = 0;

	if(0 == rVertBuf.m_pVB)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc))
	{
		if(uNumVertices <= (IFX_MAX_NUM_VERTICES - rVertBuf.m_uVertsUsed))
		{
			hr = rVertBuf.m_pVB->Lock(rVertBuf.m_uVertsUsed * rVertices.GetStride(),
				uNumVertices * rVertices.GetStride(),
				(BYTE**)&pDstData,
				D3DLOCK_NOOVERWRITE );
				
			uVBStart = rVertBuf.m_uVertsUsed;
			rVertBuf.m_uVertsUsed += uNumVertices;
		}
		else
		{
			// Lock entire buffer
			hr = rVertBuf.m_pVB->Lock(0, 0, (BYTE**)&pDstData,
				D3DLOCK_DISCARD );
				
			uVBStart = 0;
			rVertBuf.m_uVertsUsed = uNumVertices;
		}

		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
		if (NULL == pDstData)
			rc = IFX_E_DX7SURFACELOST;
	}

	if(IFXSUCCESS(rc))
	{
		IFXVector3Iter vPosIter;
		rVertices.GetVectorIter(0, vPosIter);
		pSrcData = (U8*)vPosIter.Index(uStart);

		memcpy(pDstData, pSrcData, uNumVertices * rVertices.GetStride());
	}

	if(IFXSUCCESS(rc))
	{
		hr = rVertBuf.m_pVB->Unlock();
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::CopyVerticesToStaticBuffer(  IFXInterleavedData& rVertices,
														  U32 uNumVertices,
														  IFXDX8StaticVB& rVertBuf )
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	U8* pSrcData = 0;
	U8* pDstData = 0;

	if(0 == rVertBuf.m_pVB)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc))
	{
		// Lock entire buffer
		hr = rVertBuf.m_pVB->Lock(0, 0, (BYTE**)&pDstData, 0 );

		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		IFXVector3Iter vPosIter;
		rVertices.GetVectorIter(0, vPosIter);
		pSrcData = (U8*)vPosIter.Get();

		memcpy(pDstData, pSrcData, uNumVertices * rVertices.GetStride());
	}

	if(IFXSUCCESS(rc))
	{
		hr = rVertBuf.m_pVB->Unlock();
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		rVertBuf.m_uSrcDataVersion = rVertices.GetVersionWord(0);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::GetVertexBuffer( DWORD fvf, U32 uStride,
											   IFXDX8VB*& pVerts)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	IFXUnknown* pUnk = 0;
	IFXVoidWrapper* pVoidWrap = 0;

	// First check to see if it is in the buffer hash
	if(IFXSUCCESS(m_spVertexBuffers->GetData(fvf, pUnk)))
	{
		rc = pUnk->QueryInterface(IID_IFXVoidWrapper, (void**)&pVoidWrap);

		if(IFXSUCCESS(rc))
		{
			pVerts = (IFXDX8VB*)pVoidWrap->GetData();
		}
	}
	else
	{
		// We need to create the buffer and add it to the buffer hash
		pVerts = new IFXDX8VB;

		hr = m_pDevice->CreateVertexBuffer( uStride * IFX_MAX_NUM_VERTICES,
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			fvf, D3DPOOL_DEFAULT, &(pVerts->m_pVB)  );
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		if(IFXSUCCESS(rc))
		{
			// Now we need to add it to the buffer hash
			rc = IFXCreateComponent(CID_IFXVoidWrapper,
				IID_IFXVoidWrapper,
				(void**)&pVoidWrap);
		}

		if(IFXSUCCESS(rc))
		{
			pVoidWrap->SetData(pVerts);
			rc = pVoidWrap->QueryInterface(IID_IFXUnknown, (void**)&pUnk);
		}

		if(IFXSUCCESS(rc))
		{
			rc = m_spVertexBuffers->AddData(fvf, pUnk);
		}
	}

	if(IFXSUCCESS(rc) && pVerts)
	{
		// Increment the ref count
		pVerts->m_pVB->AddRef();
	}

	IFXRELEASE(pUnk);
	IFXRELEASE(pVoidWrap);

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::GetStaticVertexBuffer( DWORD fvf,
													 U32 uId,
													 U32 uStride,
													 U32 uNumVerts,
													 IFXDX8StaticVB*& pVerts)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	IFXUnknown* pUnk = 0;
	IFXVoidWrapper* pVoidWrap = 0;

	// First check to see if it is in the buffer hash
	if(IFXSUCCESS(m_spStaticBuffers->GetData(uId, pUnk)))
	{
		rc = pUnk->QueryInterface(IID_IFXVoidWrapper, (void**)&pVoidWrap);

		if(IFXSUCCESS(rc))
		{
			pVerts = (IFXDX8StaticVB*)pVoidWrap->GetData();
		}
	}
	else
	{
		// We need to create the buffer and add it to the buffer hash
		pVerts = new IFXDX8StaticVB;

		hr = m_pDevice->CreateVertexBuffer( uStride * uNumVerts,
			D3DUSAGE_WRITEONLY,
			fvf, D3DPOOL_DEFAULT, &(pVerts->m_pVB)  );
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		if(IFXSUCCESS(rc))
		{
			// Now we need to add it to the buffer hash
			rc = IFXCreateComponent(CID_IFXVoidWrapper,
				IID_IFXVoidWrapper,
				(void**)&pVoidWrap);
		}

		if(IFXSUCCESS(rc))
		{
			pVoidWrap->SetData(pVerts);
			rc = pVoidWrap->QueryInterface(IID_IFXUnknown, (void**)&pUnk);
		}

		if(IFXSUCCESS(rc))
		{
			rc = m_spStaticBuffers->AddData(uId, pUnk);
		}
	}

	if(IFXSUCCESS(rc) && pVerts)
	{
		// Increment the ref count
		pVerts->m_pVB->AddRef();
	}

	IFXRELEASE(pUnk);
	IFXRELEASE(pVoidWrap);

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetIndices(LPDIRECT3DINDEXBUFFER8 pIndices, U32 uOffset)
{
	IFXRESULT rc = IFX_OK;

	HRESULT hr = D3D_OK;

	if(pIndices != m_pCurrentIndexBuffer || uOffset != m_uCurrentIndexOffset)
	{
		m_pCurrentIndexBuffer = pIndices;
		m_uCurrentIndexOffset = uOffset;

		hr = m_pDevice->SetIndices(m_pCurrentIndexBuffer, m_uCurrentIndexOffset);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetFVF(U32 uFVF)
{
	IFXRESULT rc = IFX_OK;

	if(m_uCurrentFVF != uFVF)
	{
		m_uCurrentFVF = uFVF;
		HRESULT hr = m_pDevice->SetVertexShader(m_uCurrentFVF);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::SetStreamSource(LPDIRECT3DVERTEXBUFFER8 pVerts, U32 uStride)
{
	IFXRESULT rc = IFX_OK;

	if(m_pStreamSource != pVerts)
	{
		m_pStreamSource = pVerts;
		HRESULT hr = m_pDevice->SetStreamSource(0, pVerts, uStride);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

DWORD CIFXRenderDeviceDX8::BuildFVF(IFXVertexAttributes vaAttribs)
{
	DWORD fvf = 0;

	if(vaAttribs.m_uData.m_bHasPositions)
	{
		fvf |= D3DFVF_XYZ;
	}

	if(vaAttribs.m_uData.m_bHasNormals)
	{
		fvf |= D3DFVF_NORMAL;
	}

	if(vaAttribs.m_uData.m_bHasDiffuseColors)
	{
		fvf |= D3DFVF_DIFFUSE;
	}

	if(vaAttribs.m_uData.m_bHasSpecularColors)
	{
		fvf |= D3DFVF_SPECULAR;
	}

	switch(vaAttribs.m_uData.m_uNumTexCoordLayers)
	{
	case 0:
		fvf |= D3DFVF_TEX0;
		break;
	case 1:
		fvf |= D3DFVF_TEX1;
		break;
	case 2:
		fvf |= D3DFVF_TEX2;
		break;
	case 3:
		fvf |= D3DFVF_TEX3;
		break;
	case 4:
		fvf |= D3DFVF_TEX4;
		break;
	case 5:
		fvf |= D3DFVF_TEX5;
		break;
	case 6:
		fvf |= D3DFVF_TEX6;
		break;
	case 7:
		fvf |= D3DFVF_TEX7;
		break;
	case 8:
		fvf |= D3DFVF_TEX8;
		break;
	}

	U32 i;
	for( i = 0; i < vaAttribs.m_uData.m_uNumTexCoordLayers; i++)
	{
		switch(vaAttribs.GetTexCoordSize(i))
		{
		case 1:
			fvf |= D3DFVF_TEXCOORDSIZE1(i);
			break;
		case 2:
			fvf |= D3DFVF_TEXCOORDSIZE2(i);
			break;
		case 3:
			fvf |= D3DFVF_TEXCOORDSIZE3(i);
			break;
		case 4:
			fvf |= D3DFVF_TEXCOORDSIZE4(i);
			break;
		}
	}

	return fvf;
}

U32 CIFXRenderDeviceDX8::GetD3DCompare(IFXenum eCompare)
{
	U32 uVal = D3DCMP_LESSEQUAL;

	switch(eCompare)
	{
	case IFX_NEVER:
		uVal = D3DCMP_NEVER;
		break;
	case IFX_LESS:
		uVal = D3DCMP_LESS;
		break;
	case IFX_GREATER:
		uVal = D3DCMP_GREATER;
		break;
	case IFX_EQUAL:
		uVal = D3DCMP_EQUAL;
		break;
	case IFX_NOT_EQUAL:
		uVal = D3DCMP_NOTEQUAL;
		break;
	case IFX_LEQUAL:
		uVal = D3DCMP_LESSEQUAL;
		break;
	case IFX_GEQUAL:
		uVal = D3DCMP_GREATEREQUAL;
		break;
	case IFX_ALWAYS:
		uVal = D3DCMP_ALWAYS;
		break;
	}

	return uVal;
}

void CIFXRenderDeviceDX8::FillIFXRenderCaps()
{
	// Clear any existing data
	memset(&m_RCaps, 0, sizeof(IFXRenderCaps));

	m_RCaps.m_uNumHWTexUnits = m_Caps.MaxSimultaneousTextures;
	m_RCaps.m_uMaxTextureWidth = m_Caps.MaxTextureWidth;
	m_RCaps.m_uMaxTextureHeight = m_Caps.MaxTextureHeight;

	m_RCaps.m_bCubeTextures = (m_Caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) ? 1 : 0;

	m_RCaps.m_b16BitColor = TRUE;
	m_RCaps.m_b32BitColor = TRUE;

	D3DDISPLAYMODE d3ddm;
	HRESULT hr = m_pD3D->GetAdapterDisplayMode( m_uDeviceNum, &d3ddm );

	if(SUCCEEDED(hr))
	{
		if(SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			D3DFMT_D16) ) )
		{
			m_RCaps.m_b16BitDepth = SUCCEEDED(m_pD3D->CheckDepthStencilMatch( m_uDeviceNum,
				D3DDEVTYPE_HAL,
				d3ddm.Format,
				d3ddm.Format,
				D3DFMT_D16    ) );
		}

		if(SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			D3DFMT_D24X8) ) )
		{
			m_RCaps.m_b24BitDepth = SUCCEEDED(m_pD3D->CheckDepthStencilMatch( m_uDeviceNum,
				D3DDEVTYPE_HAL,
				d3ddm.Format,
				d3ddm.Format,
				D3DFMT_D24X8  ) );
		}

		if(SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			D3DFMT_D24S8) ) )
		{
			m_RCaps.m_b8BitStencil = SUCCEEDED(m_pD3D->CheckDepthStencilMatch(  m_uDeviceNum,
				D3DDEVTYPE_HAL,
				d3ddm.Format,
				d3ddm.Format,
				D3DFMT_D24S8  ) );
		}

		m_RCaps.m_bAA4XSW = FALSE;

		m_RCaps.m_bAA2X = SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			TRUE,
			D3DMULTISAMPLE_2_SAMPLES ) );

		m_RCaps.m_bAA3X = SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			TRUE,
			D3DMULTISAMPLE_3_SAMPLES ) );

		m_RCaps.m_bAA4X = SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			TRUE,
			D3DMULTISAMPLE_4_SAMPLES ) );

		m_RCaps.m_bTex4444 = SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_A4R4G4B4) );
		m_RCaps.m_bTex5551 = SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_A1R5G5B5) );
		m_RCaps.m_bTex5550 = SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_X1R5G5B5) );
		m_RCaps.m_bTex5650 = SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_R5G6B5) );
		m_RCaps.m_bTex8880 = SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_X8R8G8B8) );
		m_RCaps.m_bTex8888 = SUCCEEDED(m_pD3D->CheckDeviceFormat( m_uDeviceNum,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_A8R8G8B8) );
	}
}

IFXRESULT CIFXRenderDeviceDX8::SetRenderStateDebug(char* szFileName, U32 uLineNum, D3DRENDERSTATETYPE rsType, DWORD uRs)
{
	//OutputDebugString("Setting RenderState\n");
	HRESULT hr = m_pDevice->SetRenderState(rsType, uRs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}

	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXRESULT CIFXRenderDeviceDX8::SetTexStageStateDebug(char* szFileName, U32 uLineNum, U32 uStageNum, D3DTEXTURESTAGESTATETYPE tsType, DWORD uTs)
{
	//OutputDebugString("\t\tSetting Texture Stage State\n");
	HRESULT hr = m_pDevice->SetTextureStageState(uStageNum, tsType, uTs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}

	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXRESULT CIFXRenderDeviceDX8::ReleaseVidMem()
{
	IFXRESULT rc = IFX_OK;

	if(m_spVertexBuffers.IsValid())
	{
		// The vertex buffer hash should hold nothing but vertex buffers.
		// Since they are wrapped in IFXVoidWrapper objects, they need to
		// be explicitly released.
		IFXUnknown* pUnk = 0;
		U32 uId = 0;
		while(IFXSUCCESS(m_spVertexBuffers->ExtractLowestId(uId, pUnk)))
		{
			IFXVoidWrapper* pvData = 0;
			if(IFXSUCCESS(pUnk->QueryInterface(IID_IFXVoidWrapper, (void**)&pvData)))
			{
				IFXDX8VB* pVertBuffer =
					(IFXDX8VB*)pvData->GetData();
				IFXDELETE(pVertBuffer);
				IFXRELEASE(pvData);
			}
			IFXRELEASE(pUnk);
		}

		m_spVertexBuffers = 0;
	}

	if(m_spStaticBuffers.IsValid())
	{
		// The vertex buffer hash should hold nothing but vertex buffers.
		// Since they are wrapped in IFXVoidWrapper objects, they need to
		// be explicitly released.
		IFXUnknown* pUnk = 0;
		U32 uId = 0;
		while(IFXSUCCESS(m_spStaticBuffers->ExtractLowestId(uId, pUnk)))
		{
			IFXVoidWrapper* pvData = 0;
			if(IFXSUCCESS(pUnk->QueryInterface(IID_IFXVoidWrapper, (void**)&pvData)))
			{
				IFXDX8StaticVB* pVertBuffer =
					(IFXDX8StaticVB*)pvData->GetData();
				IFXDELETE(pVertBuffer);
				IFXRELEASE(pvData);
			}
			IFXRELEASE(pUnk);
		}

		m_spStaticBuffers = 0;
	}

	m_pStreamSource = 0;
	m_uCurrentFVF = 0;
	m_pCurrentIndexBuffer = 0;

	// We must delete the textures before the D3D object goes away.
	if(m_spTextures.IsValid())
		m_spTextures->Clear();

	IFXRELEASE(m_pBigIndexBuffer);
	IFXRELEASE(m_pSmallIndexBuffer);

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::InitVidMem()
{
	IFXRESULT rc = IFX_OK;

	IFXRUN(rc, m_spVertexBuffers.Create(CID_IFXSimpleHash, IID_IFXSimpleHash));
	IFXRUN(rc, m_spVertexBuffers->Initialize(16));

	IFXRUN(rc, m_spStaticBuffers.Create(CID_IFXSimpleHash, IID_IFXSimpleHash));
	IFXRUN(rc, m_spStaticBuffers->Initialize(32));

	if(IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pSmallIndexBuffer);
		HRESULT hr = m_pDevice->CreateIndexBuffer(  IFX_MAX_NUM_INDICES * 2 /* 2 == bytes per index */,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			D3DFMT_INDEX16, D3DPOOL_DEFAULT,
			&m_pSmallIndexBuffer );
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; i < IFX_MAX_LIGHTS; i++)
		{
			m_pspLights[i]->Reset();
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; i < IFX_MAX_TEXUNITS; i++)
		{
			m_pspTexUnitsDX8[i]->InitData();
			m_pspTexUnits[i]->Reset();
		}
	}

	m_rcWindow.Set(0,0,1,1);
	m_rcVisibleWindow.Set(0,0,1,1);
	m_View.SetViewport(IFXRect(0,0,1,1));
	IFXRUN(rc, SetDefaults());

	m_bUsingUnlitMaterial = FALSE;
	m_bUsingLighting = TRUE;

	IFXRUN(rc, SetRS(D3DRS_COLORVERTEX, TRUE));
	m_bUsingVertColors = FALSE;
	IFXRUN(rc, SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL));
	IFXRUN(rc, SetRS(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL));
	IFXRUN(rc, SetRS(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL));
	IFXRUN(rc, SetRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));


	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::ResetDevice()
{
	IFXRESULT rc = IFX_OK;

	if(0 == m_pDevice)
	{
		rc = IFX_E_NOT_INITIALIZED;
	}
	else
	{
		HRESULT hr = D3D_OK;

		while(D3DERR_DEVICELOST == (hr = m_pDevice->TestCooperativeLevel()))
		{
			IFXASSERTBOXEX(D3DERR_DEVICELOST == hr, "Device Still Lost", FALSE, TRUE);
		}

		if(D3DERR_DEVICENOTRESET == hr)
		{
			IFXASSERTBOXEX(FALSE, "Resetting device...", FALSE, TRUE);
			// Need to Destroy all allocated objects in video memory first.
			// This will be accomplished by releasing all textures, vertex buffers,
			// and swap chains.
			IFXRenderDX8StackEntry* pEntry = m_pWindows;
			while(pEntry)
			{
				pEntry->m_pRender->ReleaseVidMem();
				pEntry = pEntry->m_pNext;
			}

			ReleaseVidMem();

			D3DDISPLAYMODE d3ddm;
			m_pD3D->GetAdapterDisplayMode( m_uDeviceNum, &d3ddm );

			D3DPRESENT_PARAMETERS d3dpp;
			memset( &d3dpp, 0, sizeof(d3dpp) );
			d3dpp.Windowed   = TRUE;
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
			d3dpp.BackBufferFormat = d3ddm.Format;

			hr = m_pDevice->Reset(&d3dpp);

			IFXASSERTBOX(SUCCEEDED(hr), "Failed to Reset Device!");

			if(FAILED(hr))
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}
			else
			{
				rc = InitVidMem();
			}
		}
		else if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX8::RegisterWindow(CIFXRenderDX8* pRender)
{
	IFXRESULT rc = IFX_OK;

	if(pRender)
	{
		IFXRenderDX8StackEntry* pEntry = FindStackEntry(pRender);
		if(0 == pEntry)
		{
			pEntry = new IFXRenderDX8StackEntry;
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

IFXRESULT CIFXRenderDeviceDX8::UnregisterWindow(CIFXRenderDX8* pRender)
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

IFXRenderDX8StackEntry* CIFXRenderDeviceDX8::FindStackEntry(CIFXRenderDX8* pRender)
{
	IFXRenderDX8StackEntry* pReturn = m_pWindows;

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

void CIFXRenderDeviceDX8::RemoveStackEntry(CIFXRenderDX8* pRender)
{
	IFXRenderDX8StackEntry* pEntry = FindStackEntry(pRender);

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

IFXRESULT CIFXRenderDeviceDX8::GetDXCaps(D3DCAPS8& rCaps) const
{
	rCaps = m_Caps;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX8::DeleteStaticVB(U32 uMeshId)
{
	IFXRESULT rc = IFX_OK;

	if(m_spStaticBuffers.IsValid())
	{
		// The vertex buffer hash should hold nothing but vertex buffers.
		// Since they are wrapped in IFXVoidWrapper objects, they need to
		// be explicitly released.
		IFXUnknown* pUnk = 0;
		U32 uId = 0;
		if(IFXSUCCESS(m_spStaticBuffers->ExtractData(uMeshId, pUnk)))
		{
			IFXVoidWrapper* pvData = 0;
			if(IFXSUCCESS(pUnk->QueryInterface(IID_IFXVoidWrapper, (void**)&pvData)))
			{
				IFXDX8StaticVB* pVertBuffer =
					(IFXDX8StaticVB*)pvData->GetData();
				IFXDELETE(pVertBuffer);
				IFXRELEASE(pvData);
			}
			IFXRELEASE(pUnk);
		}
	}

	return rc;
}
