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
// CIFXRenderOGL.cpp

#include "IFXRenderPCHOGL.h"
#include "CIFXRenderOGL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderOGLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderOGL* pPtr = new CIFXRenderOGL;
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

U32 CIFXRenderOGL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderOGL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderOGL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRender)
		{
			*(IFXRender**)ppInterface = (IFXRender*) this;
		}
		else
		if (interfaceId == CID_IFXRenderOGL)
		{
			*(CIFXRenderOGL**)ppInterface = (CIFXRenderOGL*) this;
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


//============================
// Private Methods
//============================
CIFXRenderOGL::CIFXRenderOGL()
{
	m_refCount = 0;
}

CIFXRenderOGL::~CIFXRenderOGL()
{
	DestroyOS();

	IFXRELEASE(m_pOGL);
}

IFXRESULT CIFXRenderOGL::Initialize(U32 uMonitorNum)
{
	IFXRESULT rc = CIFXRender::Initialize(uMonitorNum);

	IFXRUN(rc, m_spDevice.QI(m_spDeviceOGL, CID_IFXRenderDeviceOGL));

	if(IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pOGL);
		m_pOGL = m_spDeviceOGL->GetOpenGL();
		if(m_pOGL)
		{
			m_pOGL->AddRef();
		}
		else
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLights[i].QI(m_pspLightsOGL[i], CID_IFXDeviceLightOGL);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnits[i].QI(m_pspTexUnitsOGL[i], CID_IFXDeviceTexUnitOGL);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 uNumMonitors = 0;
		const IFXMonitor* pMons = m_pServices->GetMonitors(uNumMonitors);
		m_rcDevice = pMons[uMonitorNum].m_rcScreen;
		rc = InitializeOS();
	}

	if(IFXSUCCESS(rc))
	{
		m_iInitialized = IFX_OK;
	}

	IFXRUN(rc, SetVSyncEnabledOS(m_bVSyncEnabled));

	return rc;
}

IFXRESULT CIFXRenderOGL::SetHWWindow(BOOL bDirty)
{
	IFXRESULT rc = SetHWWindowOS(bDirty);

	if(IFXSUCCESS(rc))
	{
		CalcVisibleWindow();
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::Enable(IFXenum eParam)
{
	IFXRESULT rc = CIFXRender::Enable(eParam);

	if(eParam == IFX_VSYNC)
	{
		rc = SetVSyncEnabledOS(m_bVSyncEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::Disable(IFXenum eParam)
{
	IFXRESULT rc = CIFXRender::Disable(eParam);

	if(eParam == IFX_VSYNC)
	{
		rc = SetVSyncEnabledOS(m_bVSyncEnabled);
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::Swap()
{
	IFXRESULT rc = IFX_OK;

	rc = SwapOS();

	return rc;
}

IFXRESULT CIFXRenderOGL::DeleteTexture(U32 uTexId)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{

	}

	return rc;
}

IFXRESULT CIFXRenderOGL::GetTextureState(U32 uTexId, U32& uState)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{

	}

	return rc;
}

IFXRESULT CIFXRenderOGL::SetDepthMode(IFXenum eDepthMode)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		GLenum eVal = GL_LEQUAL;

		switch(eDepthMode)
		{
		case IFX_NEVER:
			eVal = GL_NEVER;
			break;
		case IFX_LESS:
			eVal = GL_LESS;
			break;
		case IFX_GREATER:
			eVal = GL_GREATER;
			break;
		case IFX_EQUAL:
			eVal = GL_EQUAL;
			break;
		case IFX_NOT_EQUAL:
			eVal = GL_NOTEQUAL;
			break;
		case IFX_LEQUAL:
			eVal = GL_LEQUAL;
			break;
		case IFX_GEQUAL:
			eVal = GL_GEQUAL;
			break;
		case IFX_ALWAYS:
			eVal = GL_ALWAYS;
			break;
		}

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glDepthFunc(eVal);
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::CreateTexture(CIFXDeviceTexturePtr& pTexture)
{
	CIFXDeviceTextureOGLPtr spTex;

	IFXRESULT rc = spTex.Create(CID_IFXDeviceTextureOGL, CID_IFXDeviceTextureOGL);

	if(IFXSUCCESS(rc))
	{
		rc = spTex->Initialize(m_pOGL, *m_spDeviceOGL.GetPointerNR());
	}

	if(IFXSUCCESS(rc))
	{
		rc = spTex.QI(pTexture, CID_IFXDeviceTexture);
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::Construct()
{
	IFXRESULT rc = CIFXRender::Construct();

	InitData();

	if(IFXSUCCESS(rc))
	{
		m_idRenderDevice = CID_IFXRenderDeviceOGL;
	}

	return rc;

}

void CIFXRenderOGL::InitData()
{
	m_eColorFormat = IFX_RGBA;

	m_pOGL = 0;
	m_pOSOGL = 0;

	m_bNeedClear = TRUE;
	m_pTempBuffer = 0;
	m_uTempBufferSize = 0;
}
