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
// CIFXDeviceLightNULL.cpp
#include "IFXRenderPCHNULL.h"
#include "CIFXDeviceLightNULL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightNULLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceLightNULL* pPtr = new CIFXDeviceLightNULL;
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

U32 CIFXDeviceLightNULL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceLightNULL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceLightNULL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceLight)
		{
			*(CIFXDeviceLight**)ppInterface = (CIFXDeviceLight*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceLightNULL)
		{
			*(CIFXDeviceLightNULL**)ppInterface = (CIFXDeviceLightNULL*) this;
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
CIFXDeviceLightNULL::CIFXDeviceLightNULL()
{
	m_refCount = 0;
}

CIFXDeviceLightNULL::~CIFXDeviceLightNULL()
{
	m_pDevice = 0;
}

IFXRESULT CIFXDeviceLightNULL::Construct()
{
	IFXRESULT rc = CIFXDeviceLight::Construct();

	InitData();

	return rc;
}

void CIFXDeviceLightNULL::InitData()
{
	m_pDevice = 0;
	
	m_uIndex = 0;
}

IFXRESULT CIFXDeviceLightNULL::Initialize(U32 uIndex, CIFXRenderDeviceNULLPtr& spDevice)
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

IFXRESULT CIFXDeviceLightNULL::SetHWLight()
{
	IFXRESULT rc = IFX_OK;

	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_bEnabled = bEnabled;
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWType(IFXenum eType)
{
	IFXRESULT rc = IFX_OK;
	
	m_eType = eType;

	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWAmbient(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vAmbient = vColor;
	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWDiffuse(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	m_vDiffuse = vColor;
	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWSpecular(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	m_vSpecular = vColor;
	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWIntensity(F32 fIntensity)
{
	IFXRESULT rc = IFX_OK;

	m_fIntensity = fIntensity;

	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWAttenuation(const IFXVector3& vAttenuation)
{
	IFXRESULT rc = IFX_OK;
	
	m_vAttenuation = vAttenuation;

	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWSpotDecay(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bSpotDecay = bEnabled;
	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWSpotInnerAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotInnerAngle = fAngle;

	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWSpotOuterAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotOuterAngle = fAngle;
	
	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWRange(F32 fRange)
{
	IFXRESULT rc = IFX_OK;
	
	m_fLightRange = fRange;

	return rc;
}

IFXRESULT CIFXDeviceLightNULL::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	if ( &m_mWorldMatrix != &mWorldMatrix )
		m_mWorldMatrix = mWorldMatrix;

	return IFX_OK;
}

