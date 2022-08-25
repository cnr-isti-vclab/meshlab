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
// CIFXRenderDeviceOGL.cpp

#include "IFXRenderPCHOGL.h"
#include "CIFXRenderDeviceOGL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceOGLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderDeviceOGL* pPtr = new CIFXRenderDeviceOGL;
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

U32 CIFXRenderDeviceOGL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDeviceOGL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDeviceOGL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
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
				if (interfaceId == CID_IFXRenderDeviceOGL)
				{
					*(CIFXRenderDeviceOGL**)ppInterface = (CIFXRenderDeviceOGL*) this;
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
IFXRESULT CIFXRenderDeviceOGL::Construct()
{
	InitData();

	IFXUseRenderWindowClassWin();

	IFXRESULT rc = CIFXRenderDevice::Construct();

	if(IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXOpenGL, IID_IFXOpenGL, (void**)&m_pOGL);
		IFXRUN(rc, m_pOGL->QueryInterface(IID_IFXOpenGLOS, (void**)&m_pOSOGL));

		IFXRUN(rc, m_pOGL->LoadOSGL());
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < 8; i++)
		{
			rc = m_pspLightsOGL[i].Create(CID_IFXDeviceLightOGL, CID_IFXDeviceLightOGL);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspLightsOGL[i].QI(m_pspLights[i], CID_IFXDeviceLight);
			}
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsOGL[i].Create(CID_IFXDeviceTexUnitOGL, CID_IFXDeviceTexUnitOGL);
			if(IFXSUCCESS(rc))
			{
				rc = m_pspTexUnitsOGL[i].QI(m_pspTexUnits[i], CID_IFXDeviceTexUnit);
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::InitializeOS()
{
	IFXRESULT rc = IFX_OK;

	// Windows CANNOT deal with multiple devices!
	if(m_uDeviceNum != 0)
	{
		rc = IFX_E_UNSUPPORTED;
	}

	if(IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pOSOGL);
		rc = m_pOGL->QueryInterface(IID_IFXOpenGLOS, (void**)&m_pOSOGL);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::Initialize(U32 uDeviceNum)
{
	IFXRESULT rc = CIFXRenderDevice::Initialize(uDeviceNum);

	m_uDeviceNum = uDeviceNum;

	IFXRUN(rc, InitializeOS());
	IFXRUN(rc, InitContext());
	IFXRUN(rc, m_pOGL->LoadExtensions());
	IFXRUN(rc, SetDefaults());

	if( IFXSUCCESS( rc ) )
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glLightModeli((GLenum) GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);

		if(m_pOGL->MultiTexSupport())
		{
			GLint glNumTexUnits;
			
			m_pOGL->glGetIntegerv((GLenum) GL_MAX_TEXTURE_UNITS_ARB, &glNumTexUnits);
			m_uNumTexUnits = glNumTexUnits;
		}
		else
		{
			m_uNumTexUnits = 1;
		}

		FillIFXRenderCaps();

		m_uActiveTexUnit = 0;

		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLightsOGL[i]->Initialize(i, m_pOGL);
		}
	}

	if( IFXSUCCESS( rc ) )
	{
		// Before we initialize the texture units, we should
		// create our "default" texture
		m_pOGL->glBindTexture(GL_TEXTURE_2D, 1);

		U32 pTex[] =
		{
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF
		};

		m_pOGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTex);

		CIFXRenderDeviceOGLPtr spDevice;
		spDevice = this;

		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc = m_pspTexUnitsOGL[i]->Initialize(i, spDevice, m_pOGL);
		}
	}

	if( IFXSUCCESS( rc ) )
	{
		m_pOGL->glEnable(GL_SCISSOR_TEST);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::GetCaps(IFXRenderCaps& rCaps) const
{
	rCaps = m_RCaps;

	return IFX_OK;
}

//===================================================
// Blend Settings
//===================================================
IFXRESULT CIFXRenderDeviceOGL::SetHWTestRef(F32 fRef)
{
	IFXRESULT rc = IFX_OK;

	m_Blend.SetReference(fRef);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glAlphaFunc(m_uTestFunc, fRef);

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	switch(eFunc)
	{
	case IFX_FB_ADD:
		m_uSrcBlend = GL_ONE;
		m_uDestBlend = GL_ONE;
		IFXTRACE_GENERIC(L"SrcBlend = GL_ONE; DestBlend = GL_ONE\n");
		break;
	case IFX_FB_MULT:
		m_uSrcBlend = GL_ZERO;
		m_uDestBlend = GL_SRC_COLOR;
		IFXTRACE_GENERIC(L"SrcBlend = GL_ZERO; DestBlend = GL_SRC_COLOR\n");
		break;
	case IFX_FB_ALPHA_BLEND:
		m_uSrcBlend = GL_SRC_ALPHA;
		m_uDestBlend = GL_ONE_MINUS_SRC_ALPHA;
		IFXTRACE_GENERIC(L"SrcBlend = GL_SRC_ALPHA; DestBlend = GL_ONE_MINUS_SRC_ALPHA\n");
		break;
	case IFX_FB_INV_ALPHA_BLEND:
		m_uSrcBlend = GL_ONE_MINUS_SRC_ALPHA;
		m_uDestBlend = GL_SRC_ALPHA;
		IFXTRACE_GENERIC(L"SrcBlend = GL_ONE_MINUS_SRC_ALPHA; DestBlend = GL_SRC_ALPHA\n");
		break;
	case IFX_FB_ADD_BLEND:
		m_uSrcBlend = GL_SRC_ALPHA;
		m_uDestBlend = GL_ONE;
		IFXTRACE_GENERIC(L"SrcBlend = GL_SRC_ALPHA; DestBlend = GL_ONE\n");
		break;
	case IFX_FB_INVISIBLE:
		m_uSrcBlend = GL_ZERO;
		m_uDestBlend = GL_ONE;
		IFXTRACE_GENERIC(L"SrcBlend = GL_ZERO; DestBlend = GL_ONE\n");
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glBlendFunc(m_uSrcBlend, m_uDestBlend);
		m_Blend.SetBlendFunc(eFunc);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWTestFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	switch(eFunc)
	{
	case IFX_NEVER:
		m_uTestFunc = GL_NEVER;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_NEVER\n");
		break;
	case IFX_LESS:
		m_uTestFunc = GL_LESS;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_LESS\n");
		break;
	case IFX_GREATER:
		m_uTestFunc = GL_GREATER;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_GREATER\n");
		break;
	case IFX_EQUAL:
		m_uTestFunc = GL_EQUAL;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_EQUAL\n");
		break;
	case IFX_NOT_EQUAL:
		m_uTestFunc = GL_NOTEQUAL;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_NOTEQUAL\n");
		break;
	case IFX_LEQUAL:
		m_uTestFunc = GL_LEQUAL;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_LEQUAL\n");
		break;
	case IFX_GEQUAL:
		m_uTestFunc = GL_GEQUAL;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_GEQUAL\n");
		break;
	case IFX_ALWAYS:
		m_uTestFunc = GL_ALWAYS;
		IFXTRACE_GENERIC(L"m_uTestFunc = GL_ALWAYS\n");
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
		IFXTRACE_GENERIC(L"m_uTestFunc = IFX_E_INVALID_RANGE\n");
	}

	if(IFXSUCCESS(rc))
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glAlphaFunc(m_uTestFunc, m_Blend.GetReference());
		m_Blend.SetTestFunc(eFunc);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWAlphaTestEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(bEnabled)
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glEnable(GL_ALPHA_TEST);
	}
	else
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glDisable(GL_ALPHA_TEST);
	}

	m_bAlphaTestEnabled = bEnabled;

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWBlendEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(bEnabled)
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glEnable(GL_BLEND);
	}
	else
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glDisable(GL_BLEND);
	}

	m_bBlendEnabled = bEnabled;

	return rc;
}

//===================================================
// Fog Settings
//===================================================
IFXRESULT CIFXRenderDeviceOGL::SetHWFogEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bFogEnabled = bEnabled;
	if(m_bFogEnabled)
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glEnable(GL_FOG);
	}
	else
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glDisable(GL_FOG);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWMode(IFXenum eMode)
{
	IFXRESULT rc = IFX_OK;


	GLint iFogMode = GL_LINEAR;
	switch(eMode)
	{
	case IFX_FOG_LINEAR:
		iFogMode = GL_LINEAR;
		break;
	case IFX_FOG_EXP:
		iFogMode = GL_EXP;
		break;
	case IFX_FOG_EXP2:
		iFogMode = GL_EXP2;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
		break;
	}

	if(IFXSUCCESS(rc))
	{
		m_Fog.SetMode(eMode);
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glFogi(GL_FOG_MODE, iFogMode);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_Fog.SetColor(vColor);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glFogfv(GL_FOG_COLOR, vColor.RawConst());

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWLinearNear(F32 fNear)
{
	IFXRESULT rc = IFX_OK;

	m_Fog.SetLinearNear(fNear);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glFogf(GL_FOG_START, fNear);

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWFar(F32 fFar)
{
	IFXRESULT rc = IFX_OK;

	m_Fog.SetFar(fFar);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glFogf(GL_FOG_END, fFar);

	if(m_Fog.GetMode() == IFX_FOG_LINEAR)
	{
		return rc;
	}

	if(IFXSUCCESS(rc))
	{
		F32 fDensity = CalcFogDensity(m_Fog.GetFar(), m_Fog.GetMode());
		m_pOGL->glFogf(GL_FOG_DENSITY, fDensity);
	}

	return rc;
}

//===================================================
// Material Settings
//===================================================
IFXRESULT CIFXRenderDeviceOGL::SetHWEmissive(const IFXVector4& vColor)
{
	m_Material.SetEmissive(vColor);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMaterialfv(GL_FRONT, GL_EMISSION, vColor.RawConst());
	m_pOGL->glColor3fv(vColor.RawConst());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWAmbient(const IFXVector4& vColor)
{
	m_Material.SetAmbient(vColor);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMaterialfv(GL_FRONT, GL_AMBIENT, vColor.RawConst());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWDiffuse(const IFXVector4& vColor)
{
	m_Material.SetDiffuse(vColor);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMaterialfv(GL_FRONT, GL_DIFFUSE, vColor.RawConst());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWSpecular(const IFXVector4& vColor)
{
	m_Material.SetSpecular(vColor);
	if(m_Material.GetShininess())
	{
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glMaterialfv(GL_FRONT, GL_SPECULAR, vColor.RawConst());
	}

	m_bBlackSpecular = FALSE;
	const F32* pf = vColor.RawConst();
	if(pf[0] == 0 && pf[1] == 0 && pf[2] == 0)
	{
		m_bBlackSpecular = TRUE;
	}

	SetHWFastSpecular(m_Material.GetFastSpecular());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWShininess(F32 fShininess)
{
	if(m_Material.GetShininess() && !fShininess)
	{
		IFXVector4 black(0,0,0,0);
		m_pOGL->glMaterialfv(GL_FRONT, GL_SPECULAR, &black.R());
	}
	if(!m_Material.GetShininess() && fShininess)
	{
		m_pOGL->glMaterialfv(GL_FRONT, GL_SPECULAR, m_Material.GetSpecular().RawConst());
	}
	m_Material.SetShininess(fShininess);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMaterialf(GL_FRONT, GL_SHININESS, fShininess*128.0f);

	SetHWFastSpecular(m_Material.GetFastSpecular());

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWFastSpecular(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_Material.SetFastSpecular(bEnabled);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(m_Material.GetShininess() && !m_bBlackSpecular && !bEnabled)
		m_pOGL->glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, TRUE);
	else
		m_pOGL->glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, FALSE);

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWRenderStyle(IFXenum eRenderStyle)
{
	IFXRESULT rc = IFX_OK;
	m_Material.SetRenderStyle(eRenderStyle);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	switch(eRenderStyle)
	{
	case IFX_WIREFRAME:
		m_pOGL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case IFX_POINTS:
		m_pOGL->glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

		break;
	default:
		m_pOGL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
	m_pOGL->glPointSize(2.0f);
	m_pOGL->glLineWidth(1.0f);
	m_pOGL->glEnable(GL_LINE_SMOOTH);
	m_pOGL->glEnable(GL_POINT_SMOOTH);
	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWShadeModel(IFXenum eShadeModel)
{
	IFXRESULT rc = IFX_OK;
	m_Material.SetShadeModel(eShadeModel);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(eShadeModel == IFX_SMOOTH)
	{
		m_pOGL->glShadeModel(GL_SMOOTH);
	}
	else
	{
		m_pOGL->glShadeModel(GL_FLAT);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWMaterial()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

//===================================================
// Miscellaneous Settings
//===================================================
IFXRESULT CIFXRenderDeviceOGL::SetHWLighting(BOOL bEnabled)
{
	m_bLighting = bEnabled;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(bEnabled)
	{
		m_pOGL->glEnable(GL_LIGHTING);
		IFXTRACE_GENERIC(L"gl LIGHTING is ON\n");
	}
	else
	{
		m_pOGL->glDisable(GL_LIGHTING);
		IFXTRACE_GENERIC(L"gl LIGHTING is OFF\n");
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWDepthTest(BOOL bEnabled)
{
	m_bDepthTest = bEnabled;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(bEnabled)
	{
		m_pOGL->glEnable(GL_DEPTH_TEST);
		IFXTRACE_GENERIC(L"glEnable GL_DEPTH_TEST\n");
	}
	else
	{
		m_pOGL->glDisable(GL_DEPTH_TEST);
		IFXTRACE_GENERIC(L"glDisable GL_DEPTH_TEST\n");
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWDepthWrite(BOOL bEnabled)
{
	m_bDepthWrite = bEnabled;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(bEnabled)
	{
		m_pOGL->glDepthMask(0xFF);
	}
	else
	{
		m_pOGL->glDepthMask(FALSE);
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWDepthCompare(IFXenum eCompare)
{
	m_eDepthCompare = eCompare;

	GLenum glCmp = GetDepthFunc(eCompare);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glDepthFunc(glCmp);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWCull(BOOL bEnabled)
{
	m_bCull = bEnabled;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(bEnabled)
	{
		m_pOGL->glEnable(GL_CULL_FACE);
	}
	else
	{
		m_pOGL->glDisable(GL_CULL_FACE);
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWCullMode(IFXenum eCullMode)
{
	m_eCullMode = eCullMode;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(eCullMode == IFX_CULL_CW)
	{
		m_pOGL->glCullFace(GL_BACK);
	}
	else
	{
		m_pOGL->glCullFace(GL_FRONT);
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWLineOffset(U32 uOffset)
{
	m_uLineOffset = uOffset;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glPolygonOffset((F32)uOffset, (F32)uOffset);

	if(uOffset)
	{
		m_pOGL->glEnable(GL_POLYGON_OFFSET_LINE);
		m_pOGL->glEnable(GL_POLYGON_OFFSET_FILL);
	}
	else
	{
		m_pOGL->glDisable(GL_POLYGON_OFFSET_LINE);
		m_pOGL->glEnable(GL_POLYGON_OFFSET_FILL);
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix)
{
	if ( &m_mViewMatrix != &mViewMatrix )
		m_mViewMatrix = mViewMatrix;
	CIFXDeviceLight::SetViewMatrix(mViewMatrix);

	m_mWorldViewMatrix.Multiply(m_mViewMatrix, m_mWorldMatrix);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMatrixMode(GL_MODELVIEW);
	m_pOGL->glLoadMatrixf((GLfloat*)m_mWorldViewMatrix.RawConst());

	U32 i;
	for( i = 0; i < m_RCaps.m_uNumHWTexUnits; i++)
	{
		IFXenum eTexGen = m_pspTexUnits[i]->GetTexCoordGen();
		if(eTexGen != IFX_NONE)
		{
			m_pspTexUnitsOGL[i]->SetHWTexCoordGen(eTexGen);
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix)
{
	if ( &m_mWorldMatrix != &mWorldMatrix )
		m_mWorldMatrix = mWorldMatrix;

	m_mWorldViewMatrix.Multiply(m_mViewMatrix, m_mWorldMatrix);
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMatrixMode(GL_MODELVIEW);
	m_pOGL->glLoadMatrixf((GLfloat*)m_mWorldViewMatrix.RawConst());

	U32 i;
	for( i = 0; i < m_RCaps.m_uNumHWTexUnits; i++)
	{
		IFXenum eTexGen = m_pspTexUnits[i]->GetTexCoordGen();
		if(eTexGen != IFX_NONE)
		{
			m_pspTexUnitsOGL[i]->SetHWTexCoordGen(eTexGen);
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient)
{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vGlobalAmbient.RawConst());
	m_vGlobalAmbient = vGlobalAmbient;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWConstantColor(const IFXVector4& vColor)
{
	U32 i;
	for( i = 0; i < m_uNumTexUnits; i++)
	{
		SetActiveTexUnit(i);
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, vColor.RawConst());
	}

	m_vConstantColor = vColor;

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWNormalization(BOOL bNormalization)
{
	m_bNormalization = bNormalization;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(bNormalization)
	{
		m_pOGL->glEnable(GL_NORMALIZE);
	}
	else
	{
		m_pOGL->glDisable(GL_NORMALIZE);
	}

	return IFX_OK;
}

//===================================================
// Stencil Settings
//===================================================
IFXRESULT CIFXRenderDeviceOGL::SetHWStencilEnabled(BOOL bEnabled)
{
	if(m_bStencilEnabled != bEnabled)
	{
		m_bStencilEnabled = bEnabled;
		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		if(bEnabled)
		{
			m_pOGL->glEnable(GL_STENCIL_TEST);
		}
		else
		{
			m_pOGL->glDisable(GL_STENCIL_TEST);
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::SetHWStencil()
{
	IFXRESULT rc = IFX_OK;

	GLenum eFunc = GetGLStencilFunc(m_Stencil.GetStencilFunc());

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glStencilFunc(eFunc, m_Stencil.GetReference(), m_Stencil.GetTestMask());

	GLenum ePass = GetGLStencilOp(m_Stencil.GetPassOp());
	GLenum eZFail = GetGLStencilOp(m_Stencil.GetZFailureOp());
	GLenum eFail = GetGLStencilOp(m_Stencil.GetFailureOp());

	m_pOGL->glStencilOp(eFail, eZFail, ePass);

	m_pOGL->glStencilMask(m_Stencil.GetWriteMask());

	return rc;
}

GLenum CIFXRenderDeviceOGL::GetGLStencilFunc(IFXenum eFunc)
{
	GLenum eVal = GL_NEVER;

	switch(eFunc)
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

	return eVal;
}

GLenum CIFXRenderDeviceOGL::GetGLStencilOp(IFXenum eOp)
{
	GLenum eVal = GL_KEEP;

	switch(eOp)
	{
	case IFX_ZERO:
		eVal = GL_ZERO;
		break;
	case IFX_REPLACE:
		eVal = GL_REPLACE;
		break;
	case IFX_INCR:
		eVal = GL_INCR;
		break;
	case IFX_DECR:
		eVal = GL_DECR;
		break;
	case IFX_INVERT:
		eVal = GL_INVERT;
		break;
	}

	return eVal;
}


//===================================================
// View Settings
//===================================================
IFXRESULT CIFXRenderDeviceOGL::SetHWView()
{
	IFXRESULT rc = IFX_OK;

	CalcVisibleViewport();

	if(m_rcVisibleViewport.m_Width <= 0 || m_rcVisibleViewport.m_Height <= 0)
	{
		rc = IFX_E_INVALID_VIEWPORT;
	}

	if(IFXSUCCESS(rc))
	{
		CalcProjectionMatrix();

		IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glMatrixMode(GL_PROJECTION);

		// IFX matches MS Windows, with 2D y as positive going down
		// GL uses y as positive going up for 2D.
		// Need to convert our positive y offset from the top of the backbuffer to
		// be an offset to from the bottom of the backbuffer
		I32 iY = m_rcVisibleWindow.m_Height - m_rcVisibleViewport.m_Height - m_rcVisibleViewport.m_Y;

		m_pOGL->glViewport( m_rcVisibleViewport.m_X,
			iY,
			m_rcVisibleViewport.m_Width,
			m_rcVisibleViewport.m_Height );

		m_pOGL->glScissor(  m_rcVisibleViewport.m_X,
			iY,
			m_rcVisibleViewport.m_Width,
			m_rcVisibleViewport.m_Height );


		m_pOGL->glLoadMatrixf(m_mProjection.RawConst());
		m_pOGL->glMatrixMode(GL_MODELVIEW);
	}

	return rc;
}

void CIFXRenderDeviceOGL::CalcProjectionMatrix()
{
	/// @todo: Hook up the custom m_Width/projectionRect data to the projection matrix calculation.

	F32* pfMat = m_mProjection.Raw();
	IFXenum projectionMode = m_View.GetProjectionMode();

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

		fRight = fRight*fWidthScale + fWidthTrans;
		fLeft = fLeft*fWidthScale + fWidthTrans;
		fTop = fTop*fHeightScale - fHeightTrans;
		fBottom = fBottom*fHeightScale - fHeightTrans;

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
		pfMat[10] = -(m_View.GetFarClip() + m_View.GetNearClip())/(m_View.GetFarClip() - m_View.GetNearClip());
		pfMat[11] = -1;

		pfMat[12] = 0;
		pfMat[13] = 0;
		pfMat[14] = -2*m_View.GetFarClip()*m_View.GetNearClip() / (m_View.GetFarClip() - m_View.GetNearClip());
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
		pfMat[10] = -2 / (m_View.GetFarClip() - m_View.GetNearClip());
		pfMat[11] = 0;

		pfMat[12] = -(fRight+fLeft)/(fRight-fLeft);
		pfMat[13] = -(fTop+fBottom)/(fTop-fBottom);
		pfMat[14] = -(m_View.GetFarClip()+m_View.GetNearClip())/(m_View.GetFarClip()-m_View.GetNearClip());
		pfMat[15] = 1;
	}
	else
	{
		IFXASSERT( FALSE );
	}
}

//===================================================
// RenderDevice Operations
//===================================================

IFXRESULT CIFXRenderDeviceOGL::DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshType)
{
	IFXRESULT rc = IFX_OK;

	IFXVector3Iter vPosIter;
	IFXVector3Iter vNormIter;
	IFXU32Iter vColorIter;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	IFXVertexAttributes vaAttribs = rMesh.GetAttributes();

	if(vaAttribs.m_uData.m_bDiffuseIsBGR && vaAttribs.m_uData.m_bHasDiffuseColors)
		IFXRUN(rc, rMesh.SwizzleDiffuseColors());

	IFXInterleavedData* pData = 0;
	rc = rMesh.GetInterleavedVertexData(pData);

    if( IFXSUCCESS( rc ) )
    {
	    if(m_bLighting && vaAttribs.m_uData.m_bHasDiffuseColors && m_Material.GetUseVertexColors())
	    {
		    if(!m_bUsingColorMaterial)
		    {
			    m_pOGL->glEnable(GL_COLOR_MATERIAL);
			    m_pOGL->glColorMaterial(GL_FRONT, GL_DIFFUSE);
		    }

		    m_bUsingColorMaterial = TRUE;
	    }
	    else
	    {
		    if(m_bUsingColorMaterial)
		    {
			    m_pOGL->glDisable(GL_COLOR_MATERIAL);
			    m_bUsingColorMaterial = FALSE;

			    SetHWDiffuse(m_Material.GetDiffuse());
			    SetHWEmissive(m_Material.GetEmissive());
			    SetHWAmbient(m_Material.GetAmbient());
			    SetHWSpecular(m_Material.GetSpecular());
			    SetHWShininess(m_Material.GetShininess());
		    }

		    m_pOGL->glColor3fv(m_Material.GetEmissive().RawConst());
	    }
    }

	IFXU16FaceIter f16FaceIter;
	IFXU32FaceIter f32FaceIter;

	IFXU16LineIter l16LineIter;
	IFXU32LineIter l32LineIter;

	U32 uVectorNum = 0;

    if( IFXSUCCESS( rc ) )
    {
	    if(vaAttribs.m_uData.m_bHasPositions)
	    {
		    pData->GetVectorIter(uVectorNum++, vPosIter);
		    m_pOGL->glEnableClientState(GL_VERTEX_ARRAY);
		    m_pOGL->glVertexPointer(3, GL_FLOAT, vPosIter.GetStride(), vPosIter.Get());
	    }
	    else
	    {
		    m_pOGL->glDisableClientState(GL_VERTEX_ARRAY);
	    }
    }

    if( IFXSUCCESS( rc ) )
    {
	    if(vaAttribs.m_uData.m_bHasNormals)
	    {
		    pData->GetVectorIter(uVectorNum++, vNormIter);
		    m_pOGL->glEnableClientState(GL_NORMAL_ARRAY);
		    m_pOGL->glNormalPointer(GL_FLOAT, vNormIter.GetStride(), vNormIter.Get());
	    }
	    else
	    {
		    m_pOGL->glDisableClientState(GL_NORMAL_ARRAY);
	    }
    }

    if( IFXSUCCESS( rc ) )
    {
	    if(vaAttribs.m_uData.m_bHasDiffuseColors)
	    {
		    if(m_Material.GetUseVertexColors())
		    {
			    pData->GetVectorIter(uVectorNum, vColorIter);
			    m_pOGL->glEnableClientState(GL_COLOR_ARRAY);
			    m_pOGL->glColorPointer(4, GL_UNSIGNED_BYTE, vColorIter.GetStride(), vColorIter.Get());
		    }
		    else
		    {
			    m_pOGL->glDisableClientState(GL_COLOR_ARRAY);
		    }

		    uVectorNum++;
	    }
	    else
	    {
		    m_pOGL->glDisableClientState(GL_COLOR_ARRAY);
	    }
    }

    if( IFXSUCCESS( rc ) )
    {
	    if(vaAttribs.m_uData.m_bHasSpecularColors)
		    uVectorNum++;

	    IFXVector2Iter tcIter;

	    U32 i;
	    for( i = 0; i < m_uNumTexUnits; i++)
	    {
		    SetActiveTexUnit(i);

		    if(m_pspTexUnits[i]->GetEnabled())
		    {
			    if(m_pspTexUnitsOGL[i]->GetTexCoordGen() == IFX_NONE)
			    {
				    pData->GetVectorIter(uVectorNum + m_pspTexUnits[i]->GetTexCoordSet(), tcIter);
				    m_pOGL->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				    m_pOGL->glTexCoordPointer(2, GL_FLOAT, tcIter.GetStride(), tcIter.Get());
			    }
			    else
			    {
				    m_pOGL->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			    }
		    }
		    else
		    {
			    m_pOGL->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		    }
	    }

	    GLenum eType = (GLenum) 0;
	    void* pvData = 0;

	    GLenum ePrimType = GL_TRIANGLES;
	    U32 uNumIndices = rMesh.GetNumFaces()*3;
	    if (uMeshType==1) // draw lines
	    {
		    ePrimType = GL_LINES;
		    uNumIndices = rMesh.GetNumLines()*2;
	    } else if (uMeshType==2) { // draw points
		    ePrimType = GL_POINTS;
		    uNumIndices = rMesh.GetNumVertices();
	    }

	    if(vaAttribs.m_uData.m_b32BitIndices)
	    {
		    if(uMeshType==0) {
			    rMesh.GetU32FaceIter(f32FaceIter);
			    pvData = (void*)f32FaceIter.Get();
		    }
		    else if (uMeshType==1) {
			    rMesh.GetU32LineIter(l32LineIter);
			    pvData = (void*)l32LineIter.Get();
		    } else { // uMeshType==2 draw points
			    pvData = IFXAllocate(sizeof(U32)*uNumIndices);
			    for (U32 ind = 0; ind< uNumIndices; ind++) {
				    ((U32*)pvData)[ind] = ind;
		    }
		    }

		    eType = GL_UNSIGNED_INT;
	    }
	    else
	    {
		    if(uMeshType==0) {
			    rMesh.GetU16FaceIter(f16FaceIter);
			    pvData = (void*)f16FaceIter.Get();
		    }
		    else if (uMeshType==1) {
			    rMesh.GetU16LineIter(l16LineIter);
			    pvData = (void*)l16LineIter.Get();
		    } else { // uMeshType==2 draw points
			    pvData = IFXAllocate(sizeof(U16)*uNumIndices);
			    for (U16 ind = 0; ind< uNumIndices; ind++) {
				    ((U16*)pvData)[ind] = ind;
			    }
		    }

		    eType = GL_UNSIGNED_SHORT;
	    }

	    if(m_pOGL->glDrawRangeElements)
	    {
		    m_pOGL->glDrawRangeElements(ePrimType, 0, rMesh.GetNumVertices()-1,
			    uNumIndices, eType, pvData);
		    uNumRenderCalls++;
	    }
	    else
	    {
		    m_pOGL->glDrawElements( ePrimType, uNumIndices,
			    eType, pvData);

		    uNumRenderCalls++;
	    }

	    if (uMeshType==2 && pvData) {
		    IFXDeallocate(pvData);
	    }

	    IFXRELEASE(pData);

    }

	return rc;
}


void* CIFXRenderDeviceOGL::GetCurrent() const
{
	// This returns 0 so that we force MakeCurrent
	// to reach down to the OpenGL layer on every
	// call (who knows who grabbed the current
	// context since last time).
	return 0;
}

void CIFXRenderDeviceOGL::FillIFXRenderCaps()
{
	m_RCaps.m_b16BitColor = TRUE;
	m_RCaps.m_b32BitColor = TRUE;
	m_RCaps.m_b16BitDepth = TRUE;
	m_RCaps.m_b24BitDepth = TRUE;

	/// @todo: Check wgl/agl pixel formats for stencil buffer support
	m_RCaps.m_b8BitStencil = TRUE;

	m_RCaps.m_bTex4444 = TRUE;
	m_RCaps.m_bTex5550 = TRUE;
	m_RCaps.m_bTex5551 = TRUE;
	m_RCaps.m_bTex5650 = TRUE;
	m_RCaps.m_bTex8880 = TRUE;
	m_RCaps.m_bTex8888 = TRUE;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&m_RCaps.m_uMaxTextureWidth);
	m_RCaps.m_uMaxTextureHeight = m_RCaps.m_uMaxTextureWidth;

	m_RCaps.m_bCubeTextures = m_pOGL->CubeTextureSupport();

	m_RCaps.m_uNumHWTexUnits = m_uNumTexUnits;
}

void CIFXRenderDeviceOGL::SetActiveTexUnit(U32 uTexUnit)
{
	if(m_uActiveTexUnit != uTexUnit)
	{
		if(uTexUnit < m_uNumTexUnits)
		{
			IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
			m_pOGL->glActiveTextureARB((GLenum) (GL_TEXTURE0_ARB + uTexUnit));
			m_pOGL->glClientActiveTextureARB((GLenum) (GL_TEXTURE0_ARB + uTexUnit));

			m_uActiveTexUnit = uTexUnit;
		}
	}
}


GLenum CIFXRenderDeviceOGL::GetDepthFunc(IFXenum eCmp)
{
	GLenum eVal = GL_LEQUAL;
	switch(eCmp)
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

	return eVal;
}

IFXRESULT CIFXRenderDeviceOGL::InitContext()
{
	IFXRESULT rc = InitWindowOS();

	if(IFXSUCCESS(rc))
	{
		rc = InitContextOS();
	}

	if(IFXSUCCESS(rc))
	{
		m_pOGL->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	}

	return rc;
}

CIFXRenderDeviceOGL::CIFXRenderDeviceOGL()
{
	m_refCount = 0;
}

CIFXRenderDeviceOGL::~CIFXRenderDeviceOGL()
{
	// Need to do a make current so that the textures have the correct context
	// for deleting GL texture objects.
	MakeDeviceCurrent();

	// We must delete the textures before the OGL object goes away
	// The smart pointer assignment to 0 will release the current
	// texture set.
	m_spTextures = 0;
	CIFXDeviceTextureOGL::DeleteSurfaceCache();

	DestroyOS();

	IFXRELEASE(m_pOGL);
}

void CIFXRenderDeviceOGL::InitData()
{
	// Blend Data
	m_uSrcBlend = GL_SRC_ALPHA;
	m_uDestBlend = GL_ONE_MINUS_SRC_ALPHA;
	m_uTestFunc = GL_ALWAYS;

	// Material Data
	m_bBlackSpecular = FALSE;
	m_bUsingColorMaterial = FALSE;

	m_uDeviceNum = 0;
	m_mWorldViewMatrix.Reset();

	m_uNumTexUnits = 1;
	m_uActiveTexUnit = 0;

	m_pOGL = 0;
	m_pOSOGL = 0;

	m_pCurrentRender = 0;
}
