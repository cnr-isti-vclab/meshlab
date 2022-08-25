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
// CIFXDeviceLight.cpp
#include "IFXRenderPCH.h"
#include "CIFXDeviceLight.h"

//===========================
// Static (class) data
//===========================

IFXMatrix4x4 CIFXDeviceLight::ms_mViewMatrix;
const IFXVector3 CIFXDeviceLight::ms_vZero(0,0,0);
const IFXVector3 CIFXDeviceLight::ms_vNegativeZ(0,0,-1);
const IFXVector3 CIFXDeviceLight::ms_vPositiveZ(0,0,1);

U32 CIFXDeviceLight::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceLight::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceLight::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceLight)
		{
			*(CIFXDeviceLight**)ppInterface = (CIFXDeviceLight*) this;
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
IFXRESULT CIFXDeviceLight::SetHWType(IFXenum eType)
{
	m_eType = eType;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWAmbient(const IFXVector4& vColor)
{
	m_vAmbient = vColor;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWDiffuse(const IFXVector4& vColor)
{
	m_vDiffuse = vColor;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWSpecular(const IFXVector4& vColor)
{
	m_vSpecular = vColor;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWIntensity(F32 fIntensity)
{
	m_fIntensity = fIntensity;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWAttenuation(const IFXVector3& vAttenuation)
{
	m_vAttenuation = vAttenuation;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWSpotDecay(BOOL bEnabled)
{
	m_bSpotDecay = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWSpotInnerAngle(F32 fAngle)
{
	m_fSpotInnerAngle = fAngle;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWSpotOuterAngle(F32 fAngle)
{
	m_fSpotOuterAngle = fAngle;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWRange(F32 fRange)
{
	m_fLightRange = fRange;

	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	m_mWorldMatrix = mWorldMatrix;

	return IFX_OK;
}


IFXRESULT CIFXDeviceLight::SetHWLight()
{
	return IFX_OK;
}

IFXRESULT CIFXDeviceLight::SetLight(IFXRenderLight& rLight)
{
	IFXRESULT rc = IFX_OK;

	CompareDevice(rc, m_bSpotDecay, rLight.GetSpotDecay(), SetHWSpotDecay);
	CompareDevice(rc, m_fLightRange, rLight.GetRange(), SetHWRange);
	CompareDevice(rc, m_vAmbient, rLight.GetAmbient(), SetHWAmbient);
	CompareDevice(rc, m_vDiffuse, rLight.GetDiffuse(), SetHWDiffuse);
	CompareDevice(rc, m_vSpecular, rLight.GetSpecular(), SetHWSpecular);
	CompareDevice(rc, m_fIntensity, rLight.GetIntensity(), SetHWIntensity);
	CompareDevice(rc, m_vAttenuation, rLight.GetAttenuation(), SetHWAttenuation);
	CompareDevice(rc, m_fSpotInnerAngle, rLight.GetSpotInnerAngle(), SetHWSpotInnerAngle);
	CompareDevice(rc, m_fSpotOuterAngle, rLight.GetSpotOuterAngle(), SetHWSpotOuterAngle);
	CompareDevice(rc, m_eType, rLight.GetType(), SetHWType);

	// This is pulled out to deal with some limitations of OpenGL.
	// OpenGL depends on the ViewMatrix for setting lights (ugh).
	if( (!(m_mWorldMatrix == rLight.GetWorldMatrix())) || (!(m_mLocalViewMatrix == ms_mViewMatrix)) )
	{
		m_mLocalViewMatrix = ms_mViewMatrix;
		MakeDeviceDirty();
		rc |= SetHWWorldMatrix(rLight.GetWorldMatrix());
	}

	if(m_bDirty)
	{
		m_bDirty = FALSE;
		rc |= SetHWLight();
	}	
	
	return rc;
}

IFXRESULT CIFXDeviceLight::SetDefaults()
{
	IFXRESULT rc = SetHWEnabled(m_bEnabled);

	IFXRUN(rc, SetHWSpotDecay(m_bSpotDecay));
	IFXRUN(rc, SetHWRange(m_fLightRange));
	IFXRUN(rc, SetHWAmbient(m_vAmbient));
	IFXRUN(rc, SetHWDiffuse(m_vDiffuse));
	IFXRUN(rc, SetHWSpecular(m_vSpecular));
	IFXRUN(rc, SetHWAttenuation(m_vAttenuation));
	IFXRUN(rc, SetHWSpotInnerAngle(m_fSpotInnerAngle));
	IFXRUN(rc, SetHWSpotOuterAngle(m_fSpotOuterAngle));
	IFXRUN(rc, SetHWType(m_eType));
	IFXRUN(rc, SetHWWorldMatrix(m_mWorldMatrix));
	IFXRUN(rc, SetHWLight());

	return rc;
}


CIFXDeviceLight::CIFXDeviceLight()
{
	// EMPTY
}

CIFXDeviceLight::~CIFXDeviceLight()
{
	// EMPTY
}

IFXRESULT CIFXDeviceLight::Construct()
{
	IFXRESULT rc = IFX_OK;

	IFXRenderLight::InitData();
	CIFXDeviceBase::InitData();
	InitData();

	return rc;
}

void CIFXDeviceLight::InitData()
{
	m_mLocalViewMatrix.Reset();
}

void CIFXDeviceLight::SetViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	if(! (ms_mViewMatrix == mViewMatrix))
	{
		ms_mViewMatrix = mViewMatrix;
	}
}
