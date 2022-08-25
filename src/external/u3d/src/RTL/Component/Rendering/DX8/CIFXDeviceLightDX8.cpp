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
// CIFXDeviceLightDX8.cpp
#include "IFXRenderPCHDX8.h"
#include "CIFXDeviceLightDX8.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightDX8Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceLightDX8* pPtr = new CIFXDeviceLightDX8;
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

U32 CIFXDeviceLightDX8::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceLightDX8::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceLightDX8::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceLight)
		{
			*(CIFXDeviceLight**)ppInterface = (CIFXDeviceLight*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceLightDX8)
		{
			*(CIFXDeviceLightDX8**)ppInterface = (CIFXDeviceLightDX8*) this;
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
CIFXDeviceLightDX8::CIFXDeviceLightDX8()
{
	m_refCount = 0;
}

CIFXDeviceLightDX8::~CIFXDeviceLightDX8()
{
	m_pDevice = 0;
}

IFXRESULT CIFXDeviceLightDX8::Construct()
{
	IFXRESULT rc = CIFXDeviceLight::Construct();

	InitData();

	return rc;
}

void CIFXDeviceLightDX8::InitData()
{
	m_pDevice = 0;
	
	m_uIndex = 0;
	memset(&m_Light, 0, sizeof(m_Light));
	m_Light.Falloff = 1.0f;
	m_Light.Range = sqrtf(FLT_MAX);
	
	m_fPhi = 1.27f;
	m_fTheta = 1.27f;
}

IFXRESULT CIFXDeviceLightDX8::Initialize(U32 uIndex, CIFXRenderDeviceDX8Ptr& spDevice)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice = spDevice.GetPointerNR();

	if(m_pDevice)
	{
		m_uIndex = uIndex;
		
		rc = SetDefaults();
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWLight()
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		HRESULT hr = m_pDevice->GetD3DDevice()->SetLight(m_uIndex, &m_Light);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		HRESULT hr = m_pDevice->GetD3DDevice()->LightEnable(m_uIndex, bEnabled);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		m_bEnabled = bEnabled;
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWType(IFXenum eType)
{
	IFXRESULT rc = IFX_OK;
	
	switch(eType)
	{
	case IFX_POINT:
		m_Light.Type = D3DLIGHT_POINT;
		m_eType = eType;
		break;
	case IFX_SPOT:
		m_Light.Type = D3DLIGHT_SPOT;
		m_eType = eType;
		break;
	case IFX_DIRECTIONAL:
		m_Light.Type = D3DLIGHT_DIRECTIONAL;
		m_eType = eType;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWAmbient(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	*(IFXVector4*)&(m_Light.Ambient) = vColor;
	((IFXVector4*)&(m_Light.Ambient))->Scale3(m_fIntensity);

	m_vAmbient = vColor;
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWDiffuse(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	*(IFXVector4*)&(m_Light.Diffuse) = vColor;
	((IFXVector4*)&(m_Light.Diffuse))->Scale3(m_fIntensity);
	
	m_vDiffuse = vColor;
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWSpecular(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	*(IFXVector4*)&(m_Light.Specular) = vColor;
	((IFXVector4*)&(m_Light.Specular))->Scale3(m_fIntensity);

	m_vSpecular = vColor;
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWIntensity(F32 fIntensity)
{
	IFXRESULT rc = IFX_OK;

	m_fIntensity = fIntensity;

	SetHWAmbient(m_vAmbient);
	SetHWDiffuse(m_vDiffuse);
	SetHWSpecular(m_vSpecular);

	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWAttenuation(const IFXVector3& vAttenuation)
{
	IFXRESULT rc = IFX_OK;
	
	m_vAttenuation = vAttenuation;
	m_Light.Attenuation0 = m_vAttenuation[0];
	m_Light.Attenuation1 = m_vAttenuation[1];
	m_Light.Attenuation2 = m_vAttenuation[2];
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWSpotDecay(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(bEnabled)
	{
		m_Light.Theta = m_fTheta;
	}
	else
	{
		m_Light.Theta = m_fPhi;
	}

	m_bSpotDecay = bEnabled;
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWSpotInnerAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotInnerAngle = fAngle;

	fAngle = IFXTO_RAD* fAngle * 1.99999f;
	
	m_fTheta = fAngle;
	if(m_bSpotDecay)
	{
		m_Light.Theta = fAngle;
	}
	else
	{
		m_Light.Theta = fAngle;
	}
		
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWSpotOuterAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotOuterAngle = fAngle;
	
	fAngle = IFXTO_RAD * fAngle * 1.99999f;

	m_fPhi = fAngle;
	m_Light.Phi = fAngle;

	if(FALSE == m_bSpotDecay)
	{
		m_Light.Theta = fAngle;
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWRange(F32 fRange)
{
	IFXRESULT rc = IFX_OK;
	
	m_Light.Range = fRange;
	m_fLightRange = fRange;

	return rc;
}

IFXRESULT CIFXDeviceLightDX8::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	if ( &m_mWorldMatrix != &mWorldMatrix )
		m_mWorldMatrix = mWorldMatrix;

	m_mWorldMatrix.TransformVector(ms_vZero, *(IFXVector3*)&(m_Light.Position));
	m_mWorldMatrix.RotateVector(ms_vNegativeZ, *(IFXVector3*)&(m_Light.Direction));
	
	return IFX_OK;
}

