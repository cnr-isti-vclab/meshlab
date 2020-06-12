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
// CIFXRenderNULL.cpp

#include "IFXRenderPCHNULL.h"
#include "CIFXRenderNULL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderNULLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderNULL* pPtr = new CIFXRenderNULL;
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

U32 CIFXRenderNULL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderNULL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderNULL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRender)
		{
			*(IFXRender**)ppInterface = (IFXRender*) this;
		}
		else
		if (interfaceId == CID_IFXRenderNULL)
		{
			*(CIFXRenderNULL**)ppInterface = (CIFXRenderNULL*) this;
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
CIFXRenderNULL::CIFXRenderNULL()
{
	m_refCount = 0;
}

CIFXRenderNULL::~CIFXRenderNULL()
{
	if(m_spDeviceNULL.IsValid())
	{
		m_spDeviceNULL->UnregisterWindow(this);
	}
}

IFXRESULT CIFXRenderNULL::Construct()
{
	IFXRESULT rc = CIFXRender::Construct();

	InitData();

	if(IFXSUCCESS(rc))
	{
		m_idRenderDevice = CID_IFXRenderDeviceNULL;
	}

	return rc;
}

void CIFXRenderNULL::InitData()
{
	m_bNeedClear = TRUE;
}

IFXRESULT CIFXRenderNULL::Initialize(U32 uMonitorNum)
{
	IFXRESULT rc = CIFXRender::Initialize(uMonitorNum);

	IFXRUN(rc, m_spDevice.QI(m_spDeviceNULL, CID_IFXRenderDeviceNULL));

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLights[i].QI(m_pspLightsNULL[i], CID_IFXDeviceLightNULL);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc |= m_pspTexUnits[i].QI(m_pspTexUnitsNULL[i], CID_IFXDeviceTexUnitNULL);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 uNumMonitors = 0;
		const IFXMonitor* pMons = m_pServices->GetMonitors(uNumMonitors);
		m_rcDevice = pMons[uMonitorNum].m_rcScreen;
	}

	if(IFXSUCCESS(rc))
	{
		m_spDeviceNULL->RegisterWindow(this);
	}

	if(IFXSUCCESS(rc))
	{
		m_iInitialized = IFX_OK;
	}

	SetHWAntiAliasingMode(m_eAAMode);

	return rc;
}

IFXRESULT CIFXRenderNULL::SetHWAntiAliasingMode(IFXenum eAAMode)
{
	m_eAAMode = eAAMode;
	return IFX_OK;
}

IFXRESULT CIFXRenderNULL::SetHWDepthStencilFormat(IFXenum eDepthStencil)
{
	IFXRESULT rc = IFX_OK;

	m_eDepthStencil = eDepthStencil;

	return rc;
}

IFXRESULT CIFXRenderNULL::SetHWWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXRenderNULL::ClearHW(const IFXRenderClear& rClear)
{
	IFXRESULT rc = IFX_OK;

	m_bNeedClear = FALSE;

	return rc;
}

IFXRESULT CIFXRenderNULL::MakeHWCurrent()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXRenderNULL::Swap()
{
	IFXRESULT rc = IFX_OK;


	return rc;
}

IFXRESULT CIFXRenderNULL::SetVSyncEnabled(BOOL bVSyncEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_bVSyncEnabled != bVSyncEnabled)
	{
		m_bVSyncEnabled = bVSyncEnabled;
		rc = SetHWWindow(TRUE);
	}

	return rc;
}

IFXRESULT CIFXRenderNULL::CreateTexture(CIFXDeviceTexturePtr& rTexture)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDeviceNULL.IsValid())
	{
		CIFXDeviceTextureNULLPtr rTexNULL;
		rc = rTexNULL.Create(CID_IFXDeviceTextureNULL, CID_IFXDeviceTextureNULL);
		if(IFXSUCCESS(rc))
		{
			rc = rTexNULL->Initialize(m_spDeviceNULL);
		}
		if(IFXSUCCESS(rc))
		{
			rc = rTexNULL.QI(rTexture, CID_IFXDeviceTexture);
		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

// END OF FILE


