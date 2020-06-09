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
// CIFXDeviceLightOGL.cpp
#include "IFXRenderPCHOGL.h"
#include "CIFXDeviceLightOGL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightOGLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceLightOGL* pPtr = new CIFXDeviceLightOGL;
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

U32 CIFXDeviceLightOGL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceLightOGL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceLightOGL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceLight)
		{
			*(CIFXDeviceLight**)ppInterface = (CIFXDeviceLight*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceLightOGL)
		{
			*(CIFXDeviceLightOGL**)ppInterface = (CIFXDeviceLightOGL*) this;
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
IFXRESULT CIFXDeviceLightOGL::Initialize(U32 uIndex, IFXOpenGL* pOGL)
{
	IFXRESULT rc = IFX_OK;

	IFXRELEASE(m_pOGL);

	m_pOGL = pOGL;

	if(m_pOGL)
	{
		m_pOGL->AddRef();
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(uIndex < 8)
	{
		m_uIndex = GL_LIGHT0 + uIndex;
	}
	else
	{
		rc = IFX_E_INVALID_RANGE;
	}

	IFXRUN(rc, SetDefaults());

	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWLight()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWType(IFXenum eType)
{
	IFXRESULT rc = IFX_OK;
	
	m_eType = eType;

	switch(m_eType)
	{
	case IFX_POINT:
	case IFX_DIRECTIONAL:
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glLightf((GLenum) m_uIndex, GL_SPOT_CUTOFF, 180);
		break;
	case IFX_SPOT:
		rc = SetHWSpotOuterAngle(m_fSpotOuterAngle);
		break;
	}

	IFXRUN(rc, SetHWWorldMatrix(m_mWorldMatrix));
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWAmbient(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vAmbient = vColor;
	IFXVector4 vScaledColor = vColor;
	vScaledColor.Scale3(m_fIntensity);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glLightfv((GLenum) m_uIndex, GL_AMBIENT, vScaledColor.RawConst());
		
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWDiffuse(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	m_vDiffuse = vColor;
	IFXVector4 vScaledColor = vColor;
	vScaledColor.Scale3(m_fIntensity);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glLightfv((GLenum) m_uIndex, GL_DIFFUSE, vScaledColor.RawConst());
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWSpecular(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	m_vSpecular = vColor;
	IFXVector4 vScaledColor = vColor;
	vScaledColor.Scale3(m_fIntensity);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glLightfv((GLenum) m_uIndex, GL_SPECULAR, vScaledColor.RawConst());
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWIntensity(F32 fIntensity)
{
	IFXRESULT rc = IFX_OK;

	m_fIntensity = fIntensity;

	SetHWAmbient(m_vAmbient);
	SetHWDiffuse(m_vDiffuse);
	SetHWSpecular(m_vSpecular);

	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWAttenuation(const IFXVector3& vAttenuation)
{
	IFXRESULT rc = IFX_OK;

	m_vAttenuation = vAttenuation;
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glLightf((GLenum) m_uIndex, GL_CONSTANT_ATTENUATION, m_vAttenuation[0]);
	m_pOGL->glLightf((GLenum) m_uIndex, GL_LINEAR_ATTENUATION, m_vAttenuation[1]);
	m_pOGL->glLightf((GLenum) m_uIndex, GL_QUADRATIC_ATTENUATION, m_vAttenuation[2]);
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWSpotDecay(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bSpotDecay = bEnabled;

	if(bEnabled && m_eType == IFX_SPOT)
	{
		F32 fCosAngle = IFXCOS(m_fSpotOuterAngle * IFXTO_RAD);
		F32 fSpotExponent = 1.0f;
		if(fCosAngle > 1e-2f)
			fSpotExponent = log10f(.04f) / log10f(fCosAngle);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glLightf((GLenum) m_uIndex, GL_SPOT_EXPONENT, fSpotExponent); 
	}
	else
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glLightf((GLenum) m_uIndex, GL_SPOT_EXPONENT, 0.0f);
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWSpotInnerAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotInnerAngle = fAngle;
		
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWSpotOuterAngle(F32 fAngle)
{
	m_fSpotOuterAngle = fAngle;
	if(m_eType == IFX_SPOT)
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glLightf((GLenum) m_uIndex, GL_SPOT_CUTOFF, fAngle);
	}
	
	return SetHWSpotDecay(m_bSpotDecay);
}

IFXRESULT CIFXDeviceLightOGL::SetHWRange(F32 fRange)
{
	IFXRESULT rc = IFX_OK;
	
	m_fLightRange = fRange;
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;
	
	m_bEnabled = bEnabled;

	if(m_bEnabled)
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glEnable((GLenum) m_uIndex);
	}
	else
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glDisable((GLenum) m_uIndex);
	}
	
	return rc;
}

IFXRESULT CIFXDeviceLightOGL::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	if ( &m_mWorldMatrix != &mWorldMatrix )
		m_mWorldMatrix = mWorldMatrix;

	m_mModelViewMatrix.Multiply3x4(m_mLocalViewMatrix, m_mWorldMatrix);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMatrixMode(GL_MODELVIEW);
	m_pOGL->glPushMatrix();
	m_pOGL->glLoadMatrixf(m_mModelViewMatrix.Raw());

	switch(m_eType)
	{
	case IFX_SPOT:
		m_pOGL->glLightfv((GLenum) m_uIndex, GL_SPOT_DIRECTION, ms_vNegativeZ.RawConst());
	case IFX_POINT:
		m_vPosition.Set(0,0,0,1);
		m_pOGL->glLightfv((GLenum) m_uIndex, GL_POSITION, m_vPosition.RawConst());
		break;
	case IFX_DIRECTIONAL:
		m_vPosition.Set(0,0,1,0);
		m_pOGL->glLightfv((GLenum) m_uIndex, GL_POSITION, m_vPosition.RawConst());
		break;
	}

	m_pOGL->glPopMatrix();

	return IFX_OK;
}

CIFXDeviceLightOGL::CIFXDeviceLightOGL()
{
	m_refCount = 0;
}

CIFXDeviceLightOGL::~CIFXDeviceLightOGL()
{
	IFXRELEASE(m_pOGL);
}

IFXRESULT CIFXDeviceLightOGL::Construct()
{
	IFXRESULT rc = CIFXDeviceLight::Construct();
	
	InitData();
	
	return rc;
}

void CIFXDeviceLightOGL::InitData()
{
	m_vPosition.Set(0,0,0,1);
	m_vDirection.Set(0,0,1);
	
	m_pOGL = 0;
	m_uIndex = 0;
	m_mModelViewMatrix.Reset();
}

