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
// CIFXRenderDeviceDX7.cpp

#include "IFXRenderPCHDX7.h"
#include "CIFXRenderDeviceDX7.h"
#include "CIFXRenderDX7.h"

//============================================
// Static (local file) data types and methods
//============================================
struct DDEnumData
{
	GUID* pgDevice;
	U32 uDevNum;
	U32 uCurrentDevNum;
};

static BOOL WINAPI DDEnumCallback(  GUID FAR * pGuid,
								  LPTSTR szDriverDesc,
								  LPTSTR szDriverName,
								  LPVOID pvData,
								  HMONITOR hMon
								  )
{
	DDEnumData* pddeData = (DDEnumData*)pvData;
	if(0 == pddeData)
	{
		return FALSE;
	}

	if(pGuid)
	{
		if(pddeData->uCurrentDevNum == pddeData->uDevNum)
		{
			pddeData->pgDevice = new GUID;
			memcpy(pddeData->pgDevice, pGuid, sizeof(GUID));
			return FALSE;
		}
		pddeData->uCurrentDevNum++;
	}

	return TRUE;
}

static HRESULT CALLBACK D3DEnumCallback(LPSTR szDeviceDescription,
										LPSTR szDeviceName,
										LPD3DDEVICEDESC7 pDesc,
										LPVOID pvContext)
{
	BOOL* pbSupported = (BOOL*)pvContext;
	if(pbSupported && pDesc)
	{
		/// @todo: Determine if this is adequate to use!
		*pbSupported = TRUE;
		return D3DENUMRET_CANCEL;
	}

	return D3DENUMRET_OK;
}


//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceDX7Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderDeviceDX7* pPtr = new CIFXRenderDeviceDX7;
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

U32 CIFXRenderDeviceDX7::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDeviceDX7::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDeviceDX7::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
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
		else if (interfaceId == CID_IFXRenderDeviceDX7)
		{
			*(CIFXRenderDeviceDX7**)ppInterface = (CIFXRenderDeviceDX7*) this;
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
CIFXRenderDeviceDX7::CIFXRenderDeviceDX7()
{
	m_refCount = 0;
}

CIFXRenderDeviceDX7::~CIFXRenderDeviceDX7()
{
	ReleaseD3D();
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}

	IFXUnuseRenderWindowClassWin();
}

IFXRESULT CIFXRenderDeviceDX7::ReleaseD3D()
{
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
				IFXDX7VB* pVertBuffer =
					(IFXDX7VB*)pvData->GetData();
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
				IFXDX7StaticVB* pVertBuffer =
					(IFXDX7StaticVB*)pvData->GetData();
				IFXDELETE(pVertBuffer);
				IFXRELEASE(pvData);
			}
			IFXRELEASE(pUnk);
		}

		m_spStaticBuffers = 0;
	}

	// We must delete the textures before the D3D object goes away
	// The smart pointer assignment to 0 will release the current
	// texture set.
	m_spTextures = 0;

	IFXDELETE(m_pWindows);
	IFXRELEASE(m_pDepthBuffer);
	IFXRELEASE(m_pBackBuffer);
	IFXRELEASE(m_pScreen);
	IFXRELEASE(m_pDevice);
	IFXRELEASE(m_pD3D);
	IFXRELEASE(m_pDD);

	return IFX_OK;
}

void CIFXRenderDeviceDX7::InitData()
{
	m_uSrcBlend = 0;
	m_uDestBlend = 0;
	m_uTestFunc = 0;

	memset(&m_D3DUnlitMaterial, 0, sizeof(D3DMATERIAL7));
	memset(&m_D3DMaterial, 0, sizeof(D3DMATERIAL7));
	m_bBlackSpecular = FALSE;

	memset(&m_Caps, 0, sizeof(D3DDEVICEDESC7));

	memset(&m_Viewport, 0, sizeof(D3DVIEWPORT7));
	m_Viewport.dvMinZ = 0;
	m_Viewport.dvMaxZ = 1;

	m_bUsingLighting = TRUE;
	m_bUsingVertColors = FALSE;
	m_bUsingUnlitMaterial = FALSE;
	m_uDeviceNum = 0;
	m_pDD = 0;
	m_pD3D = 0;
	m_pDevice = 0;
	m_pScreen = 0;
	m_pBackBuffer = 0;
	m_pDepthBuffer = 0;
	m_hWnd = 0;
	m_bInitialized = FALSE;
	m_bInScene = FALSE;
	m_pWindows = 0;
	m_bSoftwareAA = FALSE;
}

IFXRESULT CIFXRenderDeviceDX7::Construct()
{
	InitData();

	IFXUseRenderWindowClassWin();

	IFXRESULT rc = CIFXRenderDevice::Construct();

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < 8; i++)
		{
			rc = m_pspLightsDX7[i].Create(CID_IFXDeviceLightDX7, CID_IFXDeviceLightDX7);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspLightsDX7[i].QI(m_pspLights[i], CID_IFXDeviceLight);
			}
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsDX7[i].Create(CID_IFXDeviceTexUnitDX7, CID_IFXDeviceTexUnitDX7);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspTexUnitsDX7[i].QI(m_pspTexUnits[i], CID_IFXDeviceTexUnit);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::GetSupported(U32 uDeviceNum, BOOL& bSupported)
{
	IFXRESULT rc = IFX_OK;

	bSupported = TRUE;

	if(!m_bInitialized)
	{
		if(m_spDirectX7 == 0)
		{
			rc = m_spDirectX7.Create(CID_IFXDirectX7, IID_IFXDirectX7);
		}

		IFXRUN(rc, m_spDirectX7->LoadDX7());

		if(IFXSUCCESS(rc))
		{
			HRESULT hr = D3D_OK;

			DDEnumData ddeData;
			ddeData.pgDevice = 0;
			ddeData.uDevNum = uDeviceNum;
			ddeData.uCurrentDevNum = 0;

			if(m_spDirectX7->m_DirectDrawCreateEx)
			{
				hr = m_spDirectX7->m_DirectDrawEnumerateEx(DDEnumCallback, (LPVOID)&ddeData, DDENUM_ATTACHEDSECONDARYDEVICES);

				if ( FAILED( hr ) )
				{
					// Ignored this failure because we can still use the
					// NULL device in the subsequent call to
					// DirectDrawCreateEx.
					hr = D3D_OK;

					// Ensure that the device GUID pointer is NULL.
					IFXDELETE( ddeData.pgDevice );
				}
			}

			LPDIRECTDRAW7 pDD = 0;
			if(SUCCEEDED(hr))
			{
				hr = m_spDirectX7->m_DirectDrawCreateEx(ddeData.pgDevice, (void**)&pDD, IID_IDirectDraw7, 0);
			}

			IFXDELETE( ddeData.pgDevice );

			LPDIRECT3D7 pD3D = 0;
			if(SUCCEEDED(hr))
			{
				hr = pDD->QueryInterface(IID_IDirect3D7, (LPVOID*)&pD3D);
			}

			if(SUCCEEDED(hr))
			{
				hr = pD3D->EnumDevices(D3DEnumCallback, (LPVOID)&bSupported);
			}

			IFXRELEASE(pD3D);
			IFXRELEASE(pDD);

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

IFXRESULT CIFXRenderDeviceDX7::Initialize(U32 uDeviceNum)
{
	IFXRESULT rc = CIFXRenderDevice::Initialize(uDeviceNum);

	if(FALSE == m_bInitialized)
	{
		if(IFXSUCCESS(rc))
		{
			m_uDeviceNum = uDeviceNum;

			m_hWnd = CreateWindow(LPCWSTR(IFX_RENDER_WINDOW_CLASS), LPCWSTR(IFX_RENDER_WINDOW_CLASS), WS_CHILD, 0, 0,1, 1,
				GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL);

			rc = CreateD3D();
		}

		if(IFXSUCCESS(rc))
		{
			m_bInitialized = TRUE;
		}

		IFXRUN(rc, SetDefaults());
	}

	CIFXRenderDeviceDX7Ptr spDevice;
	spDevice = this;

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLightsDX7[i]->Initialize(i, spDevice);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsDX7[i]->Initialize(i, spDevice, i < m_Caps.wMaxSimultaneousTextures);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::GetCaps(IFXRenderCaps& rCaps) const
{
	rCaps = m_RCaps;

	return IFX_OK;
}

//=========================================
// Blend Settings
//=========================================
IFXRESULT CIFXRenderDeviceDX7::SetHWTestRef(F32 fRef)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_ALPHAREF, (DWORD)(fRef * 255));
	if(IFXSUCCESS(rc))
	{
		rc = m_Blend.SetReference(fRef);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWBlendFunc(IFXenum eFunc)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWTestFunc(IFXenum eFunc)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWAlphaTestEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_ALPHATESTENABLE, bEnabled);
	if(IFXSUCCESS(rc))
	{
		m_bAlphaTestEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWBlendEnabled(BOOL bEnabled)
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
IFXRESULT CIFXRenderDeviceDX7::SetHWFogEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_FOGENABLE, bEnabled);
	if(IFXSUCCESS(rc))
	{
		m_bFogEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWMode(IFXenum eMode)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_Fog.SetColor(vColor);
	rc = SetRS(D3DRS_FOGCOLOR, vColor.GetBGRA());

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWLinearNear(F32 fNear)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS(D3DRS_FOGSTART, *(U32*)&fNear);
	if(IFXSUCCESS(rc))
	{
		m_Fog.SetLinearNear(fNear);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWFar(F32 fFar)
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
IFXRESULT CIFXRenderDeviceDX7::SetHWEmissive(const IFXVector4& vColor)
{
	m_Material.SetEmissive(vColor);
	*(IFXVector4*)&m_D3DMaterial.dcvEmissive = vColor;
	*(IFXVector4*)&m_D3DUnlitMaterial.dcvEmissive = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWAmbient(const IFXVector4& vColor)
{
	m_Material.SetAmbient(vColor);
	*(IFXVector4*)&m_D3DMaterial.dcvAmbient = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWDiffuse(const IFXVector4& vColor)
{
	m_Material.SetDiffuse(vColor);
	*(IFXVector4*)&m_D3DMaterial.dcvDiffuse = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWSpecular(const IFXVector4& vColor)
{
	m_Material.SetSpecular(vColor);
	*(IFXVector4*)&m_D3DMaterial.dcvSpecular = vColor;

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

IFXRESULT CIFXRenderDeviceDX7::SetHWShininess(F32 fShininess)
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
	m_D3DMaterial.dvPower = fShininess*128.0f;

	SetHWFastSpecular(m_Material.GetFastSpecular());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWFastSpecular(BOOL bEnabled)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWRenderStyle(IFXenum eRenderStyle)
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

	/// @todo: we need to do additional investigate for points scaling
	//D3DRENDERSTATETYPE D3DFILL_POINT;
	//m_pDevice->SetRenderState(D3DFILL_POINT, F2DW(2.0f));

	m_Material.SetRenderStyle(eRenderStyle);
	rc = SetRS(D3DRS_FILLMODE, d3dStyle);

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWShadeModel(IFXenum eShadeModel)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWMaterial()
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
IFXRESULT CIFXRenderDeviceDX7::SetHWLighting(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bLighting = bEnabled;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWDepthTest(BOOL bEnabled)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWDepthWrite(BOOL bEnabled)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWDepthCompare(IFXenum eCompare)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWCull(BOOL bEnabled)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWCullMode(IFXenum eCullMode)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if ( &m_mViewMatrix != &mViewMatrix )
			m_mViewMatrix = mViewMatrix;
		NegateMatrixZ(m_mViewMatrix);
		HRESULT hr = m_pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)m_mViewMatrix.Raw());
		NegateMatrixZ(m_mViewMatrix);
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "SetTransform Failed", FALSE, TRUE);
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
				m_pspTexUnitsDX7[i]->SetHWTexCoordGen(eTexGen);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if ( &m_mWorldMatrix != &mWorldMatrix )
			m_mWorldMatrix = mWorldMatrix;
		HRESULT hr = m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)m_mWorldMatrix.Raw());
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "SetTransform Failed", FALSE, TRUE);
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
				m_pspTexUnitsDX7[i]->SetHWTexCoordGen(eTexGen);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient)
{
	IFXRESULT rc = IFX_OK;

	U32 uColor = vGlobalAmbient.GetBGRA();

	rc = SetRS(D3DRS_AMBIENT, uColor);

	if(IFXSUCCESS(rc))
	{
		m_vGlobalAmbient = vGlobalAmbient;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWConstantColor(const IFXVector4& vColor)
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

IFXRESULT CIFXRenderDeviceDX7::SetHWNormalization(BOOL bNormalization)
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
IFXRESULT CIFXRenderDeviceDX7::SetHWStencilEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILENABLE, bEnabled);

	if(IFXSUCCESS(rc))
	{
		m_bStencilEnabled = bEnabled;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILFAIL, GetStencilOp(eOp));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetFailureOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWZFailureOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILZFAIL, GetStencilOp(eOp));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetZFailureOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWPassOp(IFXenum eOp)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILPASS, GetStencilOp(eOp));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetPassOp(eOp);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWStencilFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILFUNC, GetD3DCompare(eFunc));

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetStencilFunc(eFunc);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWTestMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILMASK, uMask);

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetTestMask(uMask);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWWriteMask(U32 uMask)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILWRITEMASK, uMask);

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetWriteMask(uMask);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::SetHWStencilRef(U32 uRef)
{
	IFXRESULT rc = IFX_OK;

	rc = SetRS( D3DRS_STENCILREF, uRef);

	if(IFXSUCCESS(rc))
	{
		rc = m_Stencil.SetReference(uRef);
	}

	return rc;
}


U32 CIFXRenderDeviceDX7::GetStencilOp(IFXenum eOp)
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
IFXRESULT CIFXRenderDeviceDX7::SetHWView()
{
	IFXRESULT rc = IFX_OK;

	CalcVisibleViewport();

	if(m_rcVisibleViewport.m_Width <= 0 || m_rcVisibleViewport.m_Height <= 0)
	{
		rc = IFX_E_INVALID_VIEWPORT;
	}

	if(IFXSUCCESS(rc))
	{
		m_Viewport.dwX = m_rcVisibleViewport.m_X;
		m_Viewport.dwY = m_rcVisibleViewport.m_Y;
		m_Viewport.dwWidth = m_rcVisibleViewport.m_Width;
		m_Viewport.dwHeight = m_rcVisibleViewport.m_Height;

		if(m_bSoftwareAA)
		{
			m_Viewport.dwX *= IFX_AA_SS_FACTOR;
			m_Viewport.dwY *= IFX_AA_SS_FACTOR;
			m_Viewport.dwWidth *= IFX_AA_SS_FACTOR;
			m_Viewport.dwHeight *= IFX_AA_SS_FACTOR;
		}

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

void CIFXRenderDeviceDX7::CalcProjectionMatrix()
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
IFXRESULT CIFXRenderDeviceDX7::DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType)
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

	if(IFXSUCCESS(rc))
	{
		// Even though DX7 claims to support up to 0xFFFF vertices, some video cards
		// with older drivers (DX5 drivers) only support 0x7FFF vertices.

		if(vaAttribs.m_uData.m_b32BitIndices || (rMesh.GetNumVertices() > 0x7FFF))
		{
			rc = DrawBigMesh(rMesh, uNumRenderCalls, uMeshDrawType);
		}
		else
		{
			IFXInterleavedData* pData = 0;
			rMesh.GetInterleavedVertexData(pData);
			//U32 uTimeStamp = pData->GetTimeStamp();
			U32 uMaxNumVertices = rMesh.GetMaxNumVertices();

			/*if(rMesh.GetNumFaces() > 12 && uTimeStamp && IFXRenderServices::GetTimeStamp() &&
			(uTimeStamp != IFXRenderServices::GetTimeStamp()) &&
			uMaxNumVertices < 0x7FFF)
			{
			rc = DrawStaticSmallMesh(rMesh, *pData, uNumRenderCalls, bLines);
			}
			else*/
			{
				//pData->SetTimeStamp(IFXRenderServices::GetTimeStamp());
				DeleteStaticVB(pData->GetId());
				rc = DrawSmallMesh(rMesh, *pData, uNumRenderCalls, uMeshDrawType);
			}

			IFXRELEASE(pData);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::DrawBigMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType )
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

	IFXDX7VB* pVerts = 0;
	rc = GetVertexBuffer(fvf, vIter.GetStride(), pVerts);

	// Now we need to copy the mesh data into the vertex buffer in the order
	// that they are indexed by the face set.

	U32 uNumTotalFaces;
	U32 uNumFacesRendered = 0;
	switch(uMeshDrawType){
case 0:
	uNumTotalFaces = rMesh.GetNumFaces();
	break;
case 1:
	uNumTotalFaces = rMesh.GetNumLines();
	break;
case 2:
	uNumTotalFaces = rMesh.GetNumVertices();
	break;
default:
	IFXASSERT(0);
	rc = IFX_FALSE;
	break;
	}

	while((uNumFacesRendered < uNumTotalFaces) && IFXSUCCESS(rc))
	{
		U8* pDst;
		U32 uNumFacesToRender = uNumTotalFaces - uNumFacesRendered;
		if(uMeshDrawType == 0)
		{
			if(uNumFacesToRender > (IFX_MAX_NUM_VERTICES / 3))
			{
				uNumFacesToRender = (IFX_MAX_NUM_VERTICES / 3);
			}
		}
		else if (uMeshDrawType == 1 )
		{
			if(uNumFacesToRender > (IFX_MAX_NUM_VERTICES / 2))
			{
				uNumFacesToRender = (IFX_MAX_NUM_VERTICES / 2);
			}
		}
		else if (uMeshDrawType == 2 )
		{
			if(uNumFacesToRender > (IFX_MAX_NUM_VERTICES ))
			{
				uNumFacesToRender = IFX_MAX_NUM_VERTICES;
			}
		}
		else
		{
			IFXASSERT(0);
		}


		if(IFXSUCCESS(rc))
		{
			hr = pVerts->m_pVB[pVerts->m_uCurrentVB]->Lock( DDLOCK_DISCARDCONTENTS | DDLOCK_WRITEONLY | DDLOCK_WAIT,
				(void**)&pDst, 0 );
		}

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
				}
				else if(uMeshDrawType == 2)
				{
					rMesh.GetVectorIter(0,vIter);
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					IFXVector3* pVector = 0;

					uNumFacesToRender = rMesh.GetNumVertices();
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++){
						pSrc = (U8*)vIter.Index(i);
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}

				} else {
					IFXASSERT(0);
					rc = IFX_FALSE;
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
				}
				else if(uMeshDrawType == 2) {

					rMesh.GetVectorIter(0,vIter);
					U32 uStride = vIter.GetStride();
					U8* pSrc = 0;
					IFXVector3* pVector = 0;

					uNumFacesToRender = rMesh.GetNumVertices();
					U32 i;
					for( i = 0; i < uNumFacesToRender; i++){
						pSrc = (U8*)vIter.Index(i);
						memcpy(pDst, pSrc, uStride);
						pDst += uStride;
					}

				} else {
					IFXASSERT(0);
					rc = IFX_FALSE;
				}
			}


			if(IFXSUCCESS(rc)) {

				hr = pVerts->m_pVB[pVerts->m_uCurrentVB]->Unlock();
			}
			if(FAILED(hr))
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}
		}

		IFXRELEASE(pData);

		if(IFXSUCCESS(rc))
		{
			uNumRenderCalls++;
			if(uMeshDrawType == 0) { //mesh

				hr = m_pDevice->DrawPrimitiveVB(D3DPT_TRIANGLELIST,
					pVerts->m_pVB[pVerts->m_uCurrentVB],
					0, uNumFacesToRender*3, 0);

			} else if(uMeshDrawType == 1 ) {// lines

				hr = m_pDevice->DrawPrimitiveVB(D3DPT_LINELIST,
					pVerts->m_pVB[pVerts->m_uCurrentVB],
					0, uNumFacesToRender*2, 0);

			} else if( uMeshDrawType == 2 ) {// points

				hr = m_pDevice->DrawPrimitiveVB(D3DPT_POINTLIST,
					pVerts->m_pVB[pVerts->m_uCurrentVB],
					0, uNumFacesToRender, 0);
			}
			else
			{
				IFXASSERT(0);
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
		pVerts->m_uVertsUsed[pVerts->m_uCurrentVB] = IFX_MAX_NUM_VERTICES;
		if(pVerts->m_pVB[pVerts->m_uCurrentVB])
		{
			pVerts->m_pVB[pVerts->m_uCurrentVB]->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::DrawStaticSmallMesh( IFXMesh& rMesh,
												   IFXInterleavedData& rVerts,
												   U32& uNumRenderCalls,
												   U8 uMeshDrawType )
{
	IFXRESULT rc = IFX_OK;

	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	DWORD fvf = BuildFVF(vaAttribs);

	IFXDX7StaticVB* pVerts = 0;
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

	IFXRUN(rc, RenderSmallMesh(rMesh, pVerts->m_pVB, 0, uNumRenderCalls, uMeshDrawType));

	if(pVerts)
	{
		if(pVerts->m_pVB)
		{
			pVerts->m_pVB->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::DrawSmallMesh(IFXMesh& rMesh,
											 IFXInterleavedData& rVerts,
											 U32& uNumRenderCalls,
											 U8 uMeshDrawType )
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();
	DWORD fvf = BuildFVF(vaAttribs);

	IFXU16FaceIter fIter;
	rMesh.GetU16FaceIter(fIter);

	IFXU16LineIter lIter;
	if(uMeshDrawType==1)
		rMesh.GetU16LineIter(lIter);

	U32 uVBStart = 0;
	IFXDX7VB* pVerts = 0;
	rc = GetVertexBuffer(fvf, rVerts.GetStride(), pVerts);

	if(IFXSUCCESS(rc))
	{
		rc = CopyVerticesToBuffer(rVerts, *pVerts, uVBStart, 0, rMesh.GetNumVertices(), uMeshDrawType);
	}

	IFXRUN(rc, RenderSmallMesh(rMesh, pVerts->m_pVB[pVerts->m_uCurrentVB], uVBStart, uNumRenderCalls, uMeshDrawType));

	if(pVerts)
	{
		if(pVerts->m_pVB[pVerts->m_uCurrentVB])
		{
			pVerts->m_pVB[pVerts->m_uCurrentVB]->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::RenderSmallMesh( IFXMesh& rMesh,
											   LPDIRECT3DVERTEXBUFFER7 pVerts,
											   U32 uVBStart,
											   U32& uNumRenderCalls,
											   U8 uMeshDrawType  )
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	U32 uTotalIndices = rMesh.GetNumFaces()*3;
	U32 uIndicesToRender = 0;
	U32 uIndicesRendered = 0;

	IFXU16FaceIter fIter;
	IFXU16LineIter lIter;
	IFXVertexIter  vIter;

	U16* pIndices = NULL;
	U32 uPrimSize = 0;

	switch(uMeshDrawType)
	{
	case 0:
		rMesh.GetU16FaceIter(fIter);
		pIndices = (U16*)fIter.Get();
		uPrimSize = 3;
		uTotalIndices = rMesh.GetNumFaces()*3;
		break;

	case 1:
		rMesh.GetU16LineIter(lIter);
		uPrimSize = 2;
		pIndices = (U16*)lIter.Get();
		uTotalIndices = rMesh.GetNumLines()*2;
		break;

	case 2:

		uTotalIndices = rMesh.GetNumVertices();
		break;

	default:

		rc = IFX_FALSE;
		IFXASSERT(0);
		break;
	}

	while(IFXSUCCESS(rc) && (uIndicesRendered < uTotalIndices))
	{
		uIndicesToRender = uTotalIndices - uIndicesRendered;

		if(uIndicesToRender > D3DMAXNUMVERTICES)
		{
			uIndicesToRender = D3DMAXNUMVERTICES / uPrimSize;
			uIndicesToRender *= uPrimSize;
		}

		if(uMeshDrawType == 0 ) // mesh
		{
			hr = m_pDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLELIST,   // Primitive type
				pVerts,         // Vertex buffer
				uVBStart,       // Start vertex
				rMesh.GetNumVertices(), // Num vertices
				pIndices + uIndicesRendered,// Index array ptr
				uIndicesToRender,   // Num indices
				0 );          // Flags
		} else if( uMeshDrawType == 1 ){ // lines

			hr = m_pDevice->DrawIndexedPrimitiveVB( D3DPT_LINELIST,     // Primitive type
				pVerts,         // Vertex buffer
				uVBStart,       // Start vertex
				rMesh.GetNumVertices(), // Num vertices
				pIndices + uIndicesRendered,// Index array ptr
				uIndicesToRender,   // Num indices
				0 );          // Flags

		} else if( uMeshDrawType == 2 ) { // points

			U32 numPnt = rMesh.GetNumVertices();

			hr = m_pDevice->DrawPrimitiveVB(D3DPT_POINTLIST, pVerts, uVBStart, numPnt, 0);

		} else {
			rc = IFX_FALSE;
			IFXASSERT(0);
		}

		uNumRenderCalls++;

		uIndicesRendered += uIndicesToRender;

		if(FAILED(hr))
		{
			IFXASSERT(0);
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::CopyVerticesToStaticBuffer(  IFXInterleavedData& rVertices,
														  U32 uNumVertices,
														  IFXDX7StaticVB& rVertBuf )
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
		hr = rVertBuf.m_pVB->Lock(DDLOCK_WRITEONLY | DDLOCK_WAIT, (void**)&pDstData, 0 );

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

IFXRESULT CIFXRenderDeviceDX7::CopyVerticesToBuffer(IFXInterleavedData& rData,
													IFXDX7VB& rVerts,
													U32& uVBStart,
													U32 uStart,
													U32 uNumVertices,
													U8 uMeshDrawType )
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	U8* pSrcData = 0;
	U8* pDstData = 0;

	if(0 == rVerts.m_pVB)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc))
	{
		if(uNumVertices < (IFX_MAX_NUM_VERTICES - rVerts.m_uVertsUsed[rVerts.m_uCurrentVB]))
		{
			hr = rVerts.m_pVB[rVerts.m_uCurrentVB]->Lock( DDLOCK_NOOVERWRITE | DDLOCK_WRITEONLY | DDLOCK_WAIT,
				(void**)&pDstData, 0 );
			if (SUCCEEDED(hr) && pDstData)
			{
				uVBStart = rVerts.m_uVertsUsed[rVerts.m_uCurrentVB];
				rVerts.m_uVertsUsed[rVerts.m_uCurrentVB] += uNumVertices;
				pDstData += uVBStart * rData.GetStride();
			}
		}
		else
		{
			hr = rVerts.m_pVB[rVerts.m_uCurrentVB]->Lock( DDLOCK_DISCARDCONTENTS | DDLOCK_WRITEONLY | DDLOCK_WAIT,
				(void**)&pDstData, 0 );
			if (SUCCEEDED(hr) && pDstData)
			{
				uVBStart = 0;
				rVerts.m_uVertsUsed[rVerts.m_uCurrentVB] = uNumVertices;
			}
		}

		if(FAILED(hr))
		{
			IFXTRACE_GENERIC(L"Lock failed\n");
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		if (NULL == pDstData)
			rc = IFX_E_DX7SURFACELOST;
	}

	if(IFXSUCCESS(rc))
	{
		IFXVector3Iter vPosIter;
		IFXU16LineIter lIter;
		IFXU32Iter     pIter;
		U32 stride = 0;
		switch(uMeshDrawType)
		{
		case 0:
		case 1:
			rData.GetVectorIter(0, vPosIter);
			stride = rData.GetStride();
			pSrcData = (U8*)vPosIter.Index(uStart);
			memcpy(pDstData, pSrcData, uNumVertices * rData.GetStride());
			break;

		case 2:
			rData.GetVectorIter(0, vPosIter);
			stride = rData.GetStride();
			pSrcData = (U8*)vPosIter.Index(uStart);
			memcpy(pDstData, pSrcData, uNumVertices * rData.GetStride());
			break;

		default:
			IFXASSERT(0);
		}
	}

	if(IFXSUCCESS(rc))
	{
		hr = rVerts.m_pVB[rVerts.m_uCurrentVB]->Unlock();
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::GetStaticVertexBuffer( DWORD fvf,
													 U32 uId,
													 U32 uStride,
													 U32 uNumVerts,
													 IFXDX7StaticVB*& pVerts)
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
			pVerts = (IFXDX7StaticVB*)pVoidWrap->GetData();
		}
	}
	else
	{
		// We need to create the buffer and add it to the buffer hash
		pVerts = new IFXDX7StaticVB;
		D3DVERTEXBUFFERDESC d3dBufDesc;
		memset(&d3dBufDesc, 0, sizeof(D3DVERTEXBUFFERDESC));

		d3dBufDesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
		d3dBufDesc.dwCaps = D3DVBCAPS_WRITEONLY;
		d3dBufDesc.dwFVF = fvf;
		d3dBufDesc.dwNumVertices = uNumVerts;

		hr = m_pD3D->CreateVertexBuffer( &d3dBufDesc, &pVerts->m_pVB, 0 );
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

IFXRESULT CIFXRenderDeviceDX7::GetVertexBuffer( DWORD fvf, U32 uStride,
											   IFXDX7VB*& pVerts)
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
			pVerts = (IFXDX7VB*)pVoidWrap->GetData();
		}
	}
	else
	{
		pVerts = new IFXDX7VB;

		D3DVERTEXBUFFERDESC d3dBufDesc;
		memset(&d3dBufDesc, 0, sizeof(D3DVERTEXBUFFERDESC));

		U32 i;
		for( i = 0; i < IFX_DX7_NUM_VBS; i++)
		{
			d3dBufDesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
			d3dBufDesc.dwCaps = D3DVBCAPS_WRITEONLY;
			d3dBufDesc.dwFVF = fvf;
			d3dBufDesc.dwNumVertices = 0xFFFF;
			// We need to create the buffer and add it to the buffer hash
			hr = m_pD3D->CreateVertexBuffer( &d3dBufDesc, &(pVerts->m_pVB[i]), 0 );
			if(FAILED(hr))
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}
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
		pVerts->NewVB();
		// Increment the ref count
		pVerts->m_pVB[pVerts->m_uCurrentVB]->AddRef();
	}

	IFXRELEASE(pUnk);
	IFXRELEASE(pVoidWrap);

	return rc;
}

DWORD CIFXRenderDeviceDX7::BuildFVF(IFXVertexAttributes vaAttribs)
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

U32 CIFXRenderDeviceDX7::GetD3DCompare(IFXenum eCompare)
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

IFXRESULT CIFXRenderDeviceDX7::SetRenderStateDebug(char* szFileName, U32 uLineNum, D3DRENDERSTATETYPE rsType, DWORD uRs)
{
	//OutputDebugString("Setting RenderState\n");
	HRESULT hr = m_pDevice->SetRenderState(rsType, uRs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}

	IFXTRACE_GENERIC(L"%s(%d): Error Setting DX7 Render State: %d, %d\n", szFileName, uLineNum, rsType, uRs);

	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXRESULT CIFXRenderDeviceDX7::SetTexStageStateDebug(char* szFileName, U32 uLineNum, U32 uStageNum, D3DTEXTURESTAGESTATETYPE tsType, DWORD uTs)
{
	//OutputDebugString("\t\tSetting Texture Stage State\n");
	HRESULT hr = m_pDevice->SetTextureStageState(uStageNum, tsType, uTs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}

	IFXTRACE_GENERIC(L"%s(%d): Error Setting DX7 Texture Stage State: %d, %d, %d\n", szFileName, uLineNum, uStageNum, tsType, uTs);

	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXRESULT CIFXRenderDeviceDX7::ResetDevice()
{
	IFXRESULT rc = IFX_OK;

	if(0 == m_pDevice)
	{
		rc = IFX_E_NOT_INITIALIZED;
	}
	else
	{
		IFXASSERTBOXEX(FALSE, "Device Lost!", FALSE, TRUE);

		// Need to Destroy all allocated objects in video memory first.
		// This will be accomplished by releasing all textures, vertex buffers,
		// and swap chains.
		IFXRenderDX7StackEntry* pEntry = m_pWindows;
		while(pEntry)
		{
			pEntry->m_pRender->ReleaseVidMem();
			pEntry = pEntry->m_pNext;
		}

		rc = ReleaseD3D();

		rc = IFX_E_NEED_RESET;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::CreateD3D()
{
	IFXRESULT rc = IFX_OK;

	DDSURFACEDESC2 ddSurfDesc;

	if(!m_spDirectX7.IsValid())
	{
		rc = m_spDirectX7.Create(CID_IFXDirectX7, IID_IFXDirectX7);
	}

	IFXRUN(rc, m_spVertexBuffers.Create(CID_IFXSimpleHash, IID_IFXSimpleHash));
	IFXRUN(rc, m_spVertexBuffers->Initialize(16));

	IFXRUN(rc, m_spStaticBuffers.Create(CID_IFXSimpleHash, IID_IFXSimpleHash));
	IFXRUN(rc, m_spStaticBuffers->Initialize(32));

	if(IFXSUCCESS(rc))
	{
		rc = m_spDirectX7->LoadDX7();
	}

	if(IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pBackBuffer);
		IFXRELEASE(m_pDepthBuffer);
		IFXRELEASE(m_pScreen);
		IFXRELEASE(m_pDevice);
		IFXRELEASE(m_pD3D);
		IFXRELEASE(m_pDD);

		HRESULT hr = D3D_OK;

		DDEnumData ddeData;
		ddeData.pgDevice = 0;
		ddeData.uDevNum = m_uDeviceNum;
		ddeData.uCurrentDevNum = 0;

		if(m_uDeviceNum && m_spDirectX7->m_DirectDrawEnumerateEx)
		{
			hr = m_spDirectX7->m_DirectDrawEnumerateEx( DDEnumCallback, ( LPVOID ) &ddeData, DDENUM_ATTACHEDSECONDARYDEVICES );

			if ( FAILED( hr ) )
			{
				// Ignored this failure because we can still use the
				// NULL device in the subsequent call to
				// DirectDrawCreateEx.
				hr = D3D_OK;

				// Ensure that the device GUID pointer is NULL.
				IFXDELETE( ddeData.pgDevice );
			}
		}

		if( SUCCEEDED( hr ) )
		{
			hr = m_spDirectX7->m_DirectDrawCreateEx( ddeData.pgDevice, ( void** ) &m_pDD, IID_IDirectDraw7, 0 );
		}

		IFXDELETE(ddeData.pgDevice);

		if(SUCCEEDED(hr))
		{
			hr = m_pDD->QueryInterface(IID_IDirect3D7, (LPVOID*)&m_pD3D);
		}

		if(SUCCEEDED(hr))
		{
			hr = m_pDD->SetCooperativeLevel(NULL, DDSCL_NORMAL | DDSCL_FPUPRESERVE);
		}

		if(SUCCEEDED(hr))
		{
			memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
			ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
			ddSurfDesc.dwFlags = DDSD_CAPS;
			ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			hr = m_pDD->CreateSurface(&ddSurfDesc, &m_pScreen, 0);
		}

		if(SUCCEEDED(hr))
		{
			memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
			ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);

			hr = m_pScreen->GetSurfaceDesc(&ddSurfDesc);

			if(SUCCEEDED(hr))
			{
				ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
				ddSurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
				ddSurfDesc.dwHeight = 1;
				ddSurfDesc.dwWidth = 1;
				ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;

				hr = m_pDD->CreateSurface(&ddSurfDesc, &m_pBackBuffer, 0);
			}

			if(SUCCEEDED(hr))
			{
				hr = m_pD3D->CreateDevice(IID_IDirect3DTnLHalDevice, m_pBackBuffer, &m_pDevice);
				if(FAILED(hr))
				{
					hr = m_pD3D->CreateDevice(IID_IDirect3DHALDevice, m_pBackBuffer, &m_pDevice);
				}
			}

			if(SUCCEEDED(hr))
			{
				memset(&m_Caps, 0, sizeof(D3DDEVICEDESC7));
				hr = m_pDevice->GetCaps(&m_Caps);
			}

			if(SUCCEEDED(hr))
			{
				FillIFXRenderCaps();
			}
		}

		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(m_pDevice && IFXSUCCESS(rc))
	{
		rc = SetDefaults();
	}

	if(IFXSUCCESS(rc))
	{
		m_bUsingLighting = TRUE;
		m_bUsingVertColors = FALSE;
		SetRS(D3DRS_COLORVERTEX, TRUE);
		SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRS(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRS(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		SetRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	}

	return rc;
}

static HRESULT CALLBACK EnumTexFormats(LPDDPIXELFORMAT pFmt, LPVOID pContext)
{
	IFXRenderCaps* pCaps = (IFXRenderCaps*)pContext;
	if(pFmt && pCaps)
	{
		if(pFmt->dwFlags & DDPF_RGB)
		{
			if(pFmt->dwRGBBitCount == 32)
			{
				if(pFmt->dwRGBAlphaBitMask)
				{
					pCaps->m_bTex8888 = TRUE;
				}
				else
				{
					pCaps->m_bTex8880 = TRUE;
				}
			}
			else if(pFmt->dwRGBBitCount == 16)
			{
				switch(pFmt->dwRGBAlphaBitMask)
				{
				case 0x8000:
					pCaps->m_bTex5551 = TRUE;
					break;
				case 0xF000:
					pCaps->m_bTex4444 = TRUE;
					break;
				case 0:
					if(pFmt->dwGBitMask == 0x07e0)
					{
						pCaps->m_bTex5650 = TRUE;
					}
					else if (pFmt->dwGBitMask == 0x03e0)
					{
						pCaps->m_bTex5550 = TRUE;
					}
				}
			}
		}
	}

	return D3DENUMRET_OK;
}

static HRESULT CALLBACK EnumZFormats(LPDDPIXELFORMAT pFmt, LPVOID pContext)
{
	IFXRenderCaps* pCaps = (IFXRenderCaps*)pContext;
	if(pFmt && pCaps)
	{
		if(pFmt->dwFlags & DDPF_ZBUFFER)
		{
			if(pFmt->dwZBufferBitDepth == 16)
			{
				pCaps->m_b16BitDepth = TRUE;
			}
			else if(pFmt->dwZBufferBitDepth == 24)
			{
				pCaps->m_b24BitDepth = TRUE;
			}
			else if(pFmt->dwZBufferBitDepth == 32)
			{
				pCaps->m_b24BitDepth = TRUE;
				if(pFmt->dwStencilBitDepth == 8)
				{
					pCaps->m_b8BitStencil = TRUE;
				}
			}
		}
	}

	return D3DENUMRET_OK;
}

void CIFXRenderDeviceDX7::FillIFXRenderCaps()
{
	// Clear any existing caps data
	memset(&m_RCaps, 0, sizeof(IFXRenderCaps));

	m_RCaps.m_uNumHWTexUnits = m_Caps.wMaxSimultaneousTextures;
	m_RCaps.m_uMaxTextureWidth = m_Caps.dwMaxTextureWidth;
	m_RCaps.m_uMaxTextureHeight = m_Caps.dwMaxTextureHeight;

	m_RCaps.m_bCubeTextures = (m_Caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP) ?
		1 : 0;

	if(m_Caps.dwDeviceRenderBitDepth & DDBD_16)
	{
		m_RCaps.m_b16BitColor = TRUE;
	}

	if(m_Caps.dwDeviceRenderBitDepth & DDBD_32)
	{
		m_RCaps.m_b32BitColor = TRUE;
	}

	m_pD3D->EnumZBufferFormats(m_Caps.deviceGUID, EnumZFormats, &m_RCaps);

	if(m_RCaps.m_b24BitDepth)
	{
		if(IFXFAILURE(TryDepthFormat(IFX_DEPTH_D32S0)))
		{
			m_RCaps.m_b24BitDepth = FALSE;
			m_RCaps.m_b8BitStencil = FALSE;
		}
	}

	if(m_RCaps.m_b8BitStencil)
	{
		if(IFXFAILURE(TryDepthFormat(IFX_DEPTH_D24S8)))
		{
			m_RCaps.m_b8BitStencil = FALSE;
		}
	}

	m_pDevice->EnumTextureFormats(EnumTexFormats, &m_RCaps);

	m_RCaps.m_bAA4XSW = TRUE;

	if( (m_Caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT) &&
		(m_Caps.dpcLineCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT) )
	{
		m_RCaps.m_bAA2X = TRUE;
	}
}

IFXRESULT CIFXRenderDeviceDX7::TryDepthFormat(IFXenum eDepthFormat)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	LPDIRECTDRAWSURFACE7 pDepthBuffer = 0;
	DDSURFACEDESC2 ddSurfDesc;
	memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
	ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);

	if(SUCCEEDED(hr))
	{
		ddSurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
		ddSurfDesc.dwHeight = 1;
		ddSurfDesc.dwWidth = 1;
		ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		GetDepthFormat(eDepthFormat, ddSurfDesc.ddpfPixelFormat);
		hr = m_pDD->CreateSurface(&ddSurfDesc, &pDepthBuffer, 0);
	}

	if(SUCCEEDED(hr))
	{
		hr = m_pBackBuffer->AddAttachedSurface(pDepthBuffer);
	}

	if(SUCCEEDED(hr))
	{
		hr = m_pDevice->SetRenderTarget(m_pBackBuffer, 0);

		if(pDepthBuffer->Release())
			m_pBackBuffer->DeleteAttachedSurface(0, pDepthBuffer);
		pDepthBuffer = 0;
		m_pDevice->SetRenderTarget(m_pBackBuffer, 0);
	}

	IFXRELEASE(pDepthBuffer);



	if(FAILED(hr))
	{
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceDX7::RegisterWindow(CIFXRenderDX7* pRender)
{
	IFXRESULT rc = IFX_OK;

	if(pRender)
	{
		IFXRenderDX7StackEntry* pEntry = FindStackEntry(pRender);
		if(0 == pEntry)
		{
			pEntry = new IFXRenderDX7StackEntry;
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

IFXRESULT CIFXRenderDeviceDX7::UnregisterWindow(CIFXRenderDX7* pRender)
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

IFXRenderDX7StackEntry* CIFXRenderDeviceDX7::FindStackEntry(CIFXRenderDX7* pRender)
{
	IFXRenderDX7StackEntry* pReturn = m_pWindows;

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

void CIFXRenderDeviceDX7::RemoveStackEntry(CIFXRenderDX7* pRender)
{
	IFXRenderDX7StackEntry* pEntry = FindStackEntry(pRender);

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

void CIFXRenderDeviceDX7::GetDepthFormat(IFXenum eDepthFormat, DDPIXELFORMAT& ddpf)
{
	memset(&ddpf, 0 , sizeof(DDPIXELFORMAT));
	ddpf.dwSize = sizeof(DDPIXELFORMAT);
	ddpf.dwFlags |= DDPF_ZBUFFER;
	ddpf.dwZBufferBitDepth = 16;
	ddpf.dwZBitMask = 0x0000ffff;

	switch(eDepthFormat)
	{
	case IFX_DEPTH_D16S0:
		ddpf.dwZBufferBitDepth = 16;
		ddpf.dwZBitMask = 0x0000ffff;
		break;
	case IFX_DEPTH_D32S0:
		ddpf.dwZBufferBitDepth = 24;
		ddpf.dwZBitMask = 0x00ffffff;
		break;
	case IFX_DEPTH_D24S8:
		ddpf.dwZBufferBitDepth = 32;
		ddpf.dwStencilBitDepth = 8;
		ddpf.dwZBitMask = 0x00ffffff;
		ddpf.dwStencilBitMask = 0xff000000;
		ddpf.dwFlags |= DDPF_STENCILBUFFER;
		break;
	}
}

IFXRESULT CIFXRenderDeviceDX7::GetDXCaps(D3DDEVICEDESC7& rCaps) const
{
	rCaps = m_Caps;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceDX7::DeleteStaticVB(U32 uMeshId)
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
				IFXDX7StaticVB* pVertBuffer =
					(IFXDX7StaticVB*)pvData->GetData();
				IFXDELETE(pVertBuffer);
				IFXRELEASE(pvData);
			}
			IFXRELEASE(pUnk);
		}
	}

	return rc;
}
