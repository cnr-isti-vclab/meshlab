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
// CIFXDeviceTexUnitOGL.h
#include "IFXRenderPCHOGL.h"
#include "CIFXDeviceTexUnitOGL.h"

#ifndef GL_EXT_texture_env_combine
#ifdef GL_ARB_texture_env_combine
#define GL_COMBINE_EXT GL_COMBINE_ARB
#define GL_COMBINE_ALPHA_EXT GL_COMBINE_ALPHA_ARB
#define GL_SOURCE0_ALPHA_EXT GL_SOURCE0_ALPHA_ARB
#define GL_OPERAND0_ALPHA_EXT GL_OPERAND0_ALPHA_ARB
#define GL_SOURCE1_ALPHA_EXT GL_SOURCE1_ALPHA_ARB
#define GL_OPERAND1_ALPHA_EXT GL_OPERAND1_ALPHA_ARB
#define GL_SOURCE2_ALPHA_EXT GL_SOURCE2_ALPHA_ARB
#define GL_OPERAND2_ALPHA_EXT GL_OPERAND2_ALPHA_ARB
#define GL_RGB_SCALE_EXT GL_RGB_SCALE_ARB
#define GL_COMBINE_RGB_EXT GL_COMBINE_RGB_ARB
#define GL_SOURCE0_RGB_EXT GL_SOURCE0_RGB_ARB
#define GL_OPERAND0_RGB_EXT GL_OPERAND0_RGB_ARB
#define GL_SOURCE1_RGB_EXT GL_SOURCE1_RGB_ARB
#define GL_OPERAND1_RGB_EXT GL_OPERAND1_RGB_ARB
#define GL_SOURCE2_RGB_EXT GL_SOURCE2_RGB_ARB
#define GL_OPERAND2_RGB_EXT GL_OPERAND2_RGB_ARB
#define GL_ADD_SIGNED_EXT GL_ADD_SIGNED_ARB
#define GL_INTERPOLATE_EXT GL_INTERPOLATE_ARB
#define GL_PREVIOUS_EXT GL_PREVIOUS_ARB
#define GL_PRIMARY_COLOR_EXT GL_PRIMARY_COLOR_ARB
#define GL_CONSTANT_EXT GL_CONSTANT_ARB
#endif
#endif
//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitOGLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTexUnitOGL* pPtr = new CIFXDeviceTexUnitOGL;
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

U32 CIFXDeviceTexUnitOGL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTexUnitOGL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTexUnitOGL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexUnit)
		{
			*(CIFXDeviceTexUnit**)ppInterface = (CIFXDeviceTexUnit*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTexUnitOGL)
		{
			*(CIFXDeviceTexUnitOGL**)ppInterface = (CIFXDeviceTexUnitOGL*) this;
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
IFXRESULT CIFXDeviceTexUnitOGL::SetHWTexUnit()
{
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitOGL::Initialize(U32 uTexUnitNum, 
										   const CIFXRenderDeviceOGLPtr& spDevice,
										   IFXOpenGL* pOGL)
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

	m_pDevice = spDevice.GetPointerNR();
	m_uTexUnitNum = uTexUnitNum;

	if(uTexUnitNum < m_pDevice->GetNumHWTexUnits())
	{
		m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

		// The first texture should be created at this point
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glBindTexture(GL_TEXTURE_2D, 1);

		// If GL_EXT_texture_env_combine or GL_ARB_texture_env_combine
		// is not supported we should never get here!
		m_pOGL->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

		IFXRUN(rc, SetDefaults());
	}

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWEnabled(BOOL bEnabled)
{
	m_bEnabled = bEnabled;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	if(m_bEnabled)
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		if(m_bCubeMap)
		{
			m_pOGL->glEnable((GLenum) GL_TEXTURE_CUBE_MAP_ARB);
		}
		else
		{
			m_pOGL->glEnable(GL_TEXTURE_2D);
		}
		m_pOGL->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		if(m_bCubeMap)
		{
			m_pOGL->glDisable((GLenum) GL_TEXTURE_CUBE_MAP_ARB);
		}
		else
		{
			m_pOGL->glDisable(GL_TEXTURE_2D);
		}
		m_pOGL->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWAlphaFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);
	F32 fScale = 1.0f;

	GLenum eGLFunc = GetGLFunc(eFunc, fScale);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, fScale);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_COMBINE_ALPHA_EXT, eGLFunc);

	m_eABlendFunc = eFunc;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWAlphaInput0(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	GLenum eOperand = GL_SRC_ALPHA;
	GLenum eSrc = GetGLInput(eInput | IFX_ALPHA, eOperand);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE0_ALPHA_EXT, eSrc);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND0_ALPHA_EXT, eOperand);

	m_eAInput0 = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWAlphaInput1(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	GLenum eOperand = GL_SRC_ALPHA;
	GLenum eSrc = GetGLInput(eInput | IFX_ALPHA, eOperand);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE1_ALPHA_EXT, eSrc);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND1_ALPHA_EXT, eOperand);

	m_eAInput1 = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWAlphaInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	GLenum eOperand = GL_SRC_ALPHA;
	GLenum eSrc = GetGLInput(eInput | IFX_ALPHA, eOperand);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE2_ALPHA_EXT, eSrc);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND2_ALPHA_EXT, eOperand);

	m_eAInterpSrc = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWRGBBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);
	F32 fScale = 1.0f;

	GLenum eGLFunc = GetGLFunc(eFunc, fScale);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvf(GL_TEXTURE_ENV, (GLenum) GL_RGB_SCALE_EXT, fScale);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_COMBINE_RGB_EXT, eGLFunc);

	m_eRGBBlendFunc = eFunc;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWRGBInput0(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	GLenum eOperand = GL_SRC_ALPHA;
	GLenum eSrc = GetGLInput(eInput, eOperand);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE0_RGB_EXT, eSrc);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND0_RGB_EXT, eOperand);

	m_eRGBInput0 = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWRGBInput1(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	GLenum eOperand = GL_SRC_ALPHA;
	GLenum eSrc = GetGLInput(eInput, eOperand);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE1_RGB_EXT, eSrc);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND1_RGB_EXT, eOperand);

	m_eRGBInput1 = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWRGBInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	GLenum eOperand = GL_SRC_ALPHA;
	GLenum eSrc = GetGLInput(eInput | IFX_ALPHA, eOperand);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE2_RGB_EXT, eSrc);
	m_pOGL->glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND2_RGB_EXT, eOperand);

	m_eRGBInterpSrc = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWTexCoordGen(IFXenum eGenMode)
{
	IFXRESULT rc = IFX_OK;

	m_eTexCoordGen = eGenMode;
	
	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);
	m_mTexGenMatrix.Reset();

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");

	switch(eGenMode)
	{
	case IFX_NONE:
		m_pOGL->glDisable(GL_TEXTURE_GEN_S);
		m_pOGL->glDisable(GL_TEXTURE_GEN_T);
		m_pOGL->glDisable(GL_TEXTURE_GEN_R);
		break;
	case IFX_TEXGEN_REFLECTION_SPHERE:
		m_pOGL->glEnable(GL_TEXTURE_GEN_S);
		m_pOGL->glEnable(GL_TEXTURE_GEN_T);
		m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		break;
	case IFX_TEXGEN_VIEWPOSITION:
		{
			// Texgen matrix is the local->canera matrix.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();
			m_mTexGenMatrix.Multiply3x4(mView, m_pDevice->GetWorldMatrix());

			m_pOGL->glEnable(GL_TEXTURE_GEN_S);
			m_pOGL->glEnable(GL_TEXTURE_GEN_T);
			m_pOGL->glEnable(GL_TEXTURE_GEN_R);
			m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			IFXVector4 vPlane(1,0,0,0);
			m_pOGL->glTexGenfv(GL_S, GL_OBJECT_PLANE, vPlane.Raw());
			vPlane.Set(0,1,0,0);
			m_pOGL->glTexGenfv(GL_T, GL_OBJECT_PLANE, vPlane.Raw());
			vPlane.Set(0,0,1,0);
			m_pOGL->glTexGenfv(GL_R, GL_OBJECT_PLANE, vPlane.Raw());
		}
		break;
	case IFX_TEXGEN_VIEWNORMAL:
		if(m_pOGL->CubeTextureSupport())
		{
			m_pOGL->glEnable(GL_TEXTURE_GEN_S);
			m_pOGL->glEnable(GL_TEXTURE_GEN_T);
			m_pOGL->glEnable(GL_TEXTURE_GEN_R);
			m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
			m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
			m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
		}
		break;
	case IFX_TEXGEN_VIEWREFLECTION:
		if(m_pOGL->CubeTextureSupport())
		{
			m_pOGL->glEnable(GL_TEXTURE_GEN_S);
			m_pOGL->glEnable(GL_TEXTURE_GEN_T);
			m_pOGL->glEnable(GL_TEXTURE_GEN_R);
			m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
			m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
			m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
		}
		break;
	case IFX_TEXGEN_WORLDPOSITION:
		{
			// Texgen matrix is the local->world matrix.
			m_mTexGenMatrix = m_pDevice->GetWorldMatrix();

			m_pOGL->glEnable(GL_TEXTURE_GEN_S);
			m_pOGL->glEnable(GL_TEXTURE_GEN_T);
			m_pOGL->glEnable(GL_TEXTURE_GEN_R);
			m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			IFXVector4 vPlane(1,0,0,0);
			m_pOGL->glTexGenfv(GL_S, GL_OBJECT_PLANE, vPlane.Raw());
			vPlane.Set(0,1,0,0);
			m_pOGL->glTexGenfv(GL_T, GL_OBJECT_PLANE, vPlane.Raw());
			vPlane.Set(0,0,1,0);
			m_pOGL->glTexGenfv(GL_R, GL_OBJECT_PLANE, vPlane.Raw());
		}
		break;
	case IFX_TEXGEN_WORLDNORMAL:
		{
			// TexGenMatrix is inverse transpose of the camera->world matrix
			// The device View matrix is the Inverse of camera->world matrix
			m_mTexGenMatrix = m_pDevice->GetViewMatrix();

			// Don't want to translate direction vectors
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Now for the transpose part of inverse transpose.
			m_mTexGenMatrix.Transpose();

			if(m_pOGL->CubeTextureSupport())
			{
				m_pOGL->glEnable(GL_TEXTURE_GEN_S);
				m_pOGL->glEnable(GL_TEXTURE_GEN_T);
				m_pOGL->glEnable(GL_TEXTURE_GEN_R);
				m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
				m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
				m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
			}
		}
		break;
	case IFX_TEXGEN_WORLDREFLECTION:
		{
			// TexGenMatrix is inverse transpose of the camera->world matrix
			// The device View matrix is the Inverse of camera->world matrix
			m_mTexGenMatrix = m_pDevice->GetViewMatrix();

			// Don't want to translate direction vectors
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Now for the transpose part of inverse transpose.
			m_mTexGenMatrix.Transpose();

			if(m_pOGL->CubeTextureSupport())
			{
				m_pOGL->glEnable(GL_TEXTURE_GEN_S);
				m_pOGL->glEnable(GL_TEXTURE_GEN_T);
				m_pOGL->glEnable(GL_TEXTURE_GEN_R);
				m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
				m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
				m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
			}
		}
		break;
	case IFX_TEXGEN_LOCALPOSITION:
		{
			m_pOGL->glEnable(GL_TEXTURE_GEN_S);
			m_pOGL->glEnable(GL_TEXTURE_GEN_T);
			m_pOGL->glEnable(GL_TEXTURE_GEN_R);
			m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			IFXVector4 vPlane(1,0,0,0);
			m_pOGL->glTexGenfv(GL_S, GL_OBJECT_PLANE, vPlane.Raw());
			vPlane.Set(0,1,0,0);
			m_pOGL->glTexGenfv(GL_T, GL_OBJECT_PLANE, vPlane.Raw());
			vPlane.Set(0,0,1,0);
			m_pOGL->glTexGenfv(GL_R, GL_OBJECT_PLANE, vPlane.Raw());
		}
		break;
	case IFX_TEXGEN_LOCALNORMAL:
		{
			// The texgen matrix is the inverse transpose of the camera->local
			// matrix.  This is the same as the transpose of the concatenated
			// world and view matrices sent to the device.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();

			// Concatenate the world and view matrices.
			m_mTexGenMatrix.Multiply3x4(mView, m_pDevice->GetWorldMatrix());

			// Don't want to translate direction vectors.
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Finally, take the transpose.
			m_mTexGenMatrix.Transpose();

			if(m_pOGL->CubeTextureSupport())
			{
				m_pOGL->glEnable(GL_TEXTURE_GEN_S);
				m_pOGL->glEnable(GL_TEXTURE_GEN_T);
				m_pOGL->glEnable(GL_TEXTURE_GEN_R);
				m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
				m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
				m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
			}
		}
		break;
	case IFX_TEXGEN_LOCALREFLECTION:
		{
			// The texgen matrix is the inverse transpose of the camera->local
			// matrix.  This is the same as the transpose of the concatenated
			// world and view matrices sent to the device.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();

			// Concatenate the world and view matrices.
			m_mTexGenMatrix.Multiply3x4(mView, m_pDevice->GetWorldMatrix());

			// Don't want to translate direction vectors.
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Finally, take the transpose.
			m_mTexGenMatrix.Transpose();

			if(m_pOGL->CubeTextureSupport())
			{
				m_pOGL->glEnable(GL_TEXTURE_GEN_S);
				m_pOGL->glEnable(GL_TEXTURE_GEN_T);
				m_pOGL->glEnable(GL_TEXTURE_GEN_R);
				m_pOGL->glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
				m_pOGL->glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
				m_pOGL->glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
			}
		}
		break;
	}

	IFXRUN(rc, SetHWTextureTransform(m_mTexMatrix));


	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWTextureTransform(const IFXMatrix4x4 &mTexture)
{
	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	if ( &m_mTexMatrix != &mTexture )
		m_mTexMatrix = mTexture;

	IFXMatrix4x4 mTrans;
	mTrans.Reset();

	mTrans.Multiply3x4(m_mTexMatrix, m_mTexGenMatrix);

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glMatrixMode(GL_TEXTURE);
	m_pOGL->glLoadMatrixf(mTrans.RawConst());

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWTextureId(U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	m_pDevice->SetActiveTexUnit(m_uTexUnitNum);

	m_uTexId = uTexId;

	if(m_uTexId != 1)
	{
		IFXUnknown* pUnk = 0;
		rc = m_pDevice->GetTextures()->GetData(uTexId, pUnk);

		if(IFXSUCCESS(rc))
		{
			CIFXDeviceTextureOGL* pTex = 0;
			rc = pUnk->QueryInterface(CID_IFXDeviceTextureOGL, (void**)&pTex);

			if(IFXSUCCESS(rc))
			{
				uTexId = pTex->Id();
				BOOL bCubeMap = (pTex->GetType() == IFX_TEXTURE_CUBE);
				if(m_bCubeMap != bCubeMap)
				{
					m_bCubeMap = bCubeMap;
					if(m_bCubeMap)
						m_eglTarget = (GLenum) GL_TEXTURE_CUBE_MAP_ARB;
					else
						m_eglTarget = GL_TEXTURE_2D;

					if(m_bEnabled)
					{
						if(m_bCubeMap)
						{
							m_pOGL->glDisable(GL_TEXTURE_2D);
							m_pOGL->glEnable((GLenum) GL_TEXTURE_CUBE_MAP_ARB);
						}
						else
						{
							m_pOGL->glEnable(GL_TEXTURE_2D);
							m_pOGL->glDisable((GLenum) GL_TEXTURE_CUBE_MAP_ARB);
						}
					}

					if(m_eTexCoordGen != IFX_NONE)
						SetHWTexCoordGen(m_eTexCoordGen);
				}
			}
			else
			{
				uTexId = 1;
				m_bCubeMap = FALSE;
				m_eglTarget = GL_TEXTURE_2D;
			}

			IFXRELEASE(pTex);
		}

		IFXRELEASE(pUnk);
	}

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glBindTexture(m_eglTarget, uTexId);

	IFXRUN(rc, SetHWTexRepeatU(m_bTexRepeatU));
	IFXRUN(rc, SetHWTexRepeatV(m_bTexRepeatV));
	IFXRUN(rc, SetHWTexRepeatW(m_bTexRepeatW));

	return rc;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWTexRepeatU(BOOL bEnabled)
{
	m_bTexRepeatU = bEnabled;
	
	if(bEnabled && !m_bCubeMap)
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexParameteri(m_eglTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	}
	else
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexParameteri(m_eglTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWTexRepeatV(BOOL bEnabled)
{
	m_bTexRepeatV = bEnabled;
	
	if(bEnabled && !m_bCubeMap)
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexParameteri(m_eglTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexParameteri(m_eglTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitOGL::SetHWTexRepeatW(BOOL bEnabled)
{
	m_bTexRepeatW = bEnabled;
	
	if(bEnabled && !m_bCubeMap)
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexParameteri(m_eglTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glTexParameteri(m_eglTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	
	return IFX_OK;
}

GLenum CIFXDeviceTexUnitOGL::GetGLFunc(IFXenum eFunc, F32& fScale)
{
	GLenum eOut = GL_REPLACE;
	fScale = 1.0f;

	switch(eFunc)
	{
	case IFX_SELECT_ARG0:
		eOut = GL_REPLACE;
		break;
	case IFX_MODULATE:
		eOut = GL_MODULATE;
		break;
	case IFX_MODULATE2X:
		fScale = 2;
		eOut = GL_MODULATE;
		break;
	case IFX_MODULATE4X:
		fScale = 4;
		eOut = GL_MODULATE;
		break;
	case IFX_ADD:
		eOut = GL_ADD;
		break;
	case IFX_ADDSIGNED:
		eOut = (GLenum) GL_ADD_SIGNED_EXT;
		break;
	case IFX_ADDSIGNED2X:
		fScale = 2;
		eOut = (GLenum) GL_ADD_SIGNED_EXT;
		break;
	case IFX_INTERPOLATE:
		eOut = (GLenum) GL_INTERPOLATE_EXT;
		break;
	case IFX_DOTPRODUCT3:
		eOut = GL_REPLACE;
		break;
	}

	return eOut;
}

GLenum CIFXDeviceTexUnitOGL::GetGLInput(IFXenum eInput, GLenum& eOperand)
{
	GLenum eOut = GL_TEXTURE;

	IFXenum eMods = eInput & (IFX_INVERSE | IFX_ALPHA);
	eInput &= ~(IFX_INVERSE | IFX_ALPHA);

	switch(eInput)
	{
	case IFX_TEXTURE:
		eOut = GL_TEXTURE;
		break;
	case IFX_INCOMING:
		eOut = (GLenum) GL_PREVIOUS_EXT;
		break;
	case IFX_DIFFUSE:
		eOut = (GLenum) GL_PRIMARY_COLOR_EXT;
		break;
	case IFX_CONSTANT:
		eOut = (GLenum) GL_CONSTANT_EXT;
		break;
	}

	if(eMods & IFX_ALPHA)
	{
		if(eMods & IFX_INVERSE)
		{
			eOperand = GL_ONE_MINUS_SRC_ALPHA;
		}
		else
		{
			eOperand = GL_SRC_ALPHA;
		}
	}
	else
	{
		if(eMods & IFX_INVERSE)
		{
			eOperand = GL_ONE_MINUS_SRC_COLOR;
		}
		else
		{
			eOperand = GL_SRC_COLOR;
		}
	}

	return eOut;
}

CIFXDeviceTexUnitOGL::CIFXDeviceTexUnitOGL()
{
	m_refCount = 0;
}

CIFXDeviceTexUnitOGL::~CIFXDeviceTexUnitOGL()
{
	m_pDevice = 0;
	IFXRELEASE(m_pOGL);
}

IFXRESULT CIFXDeviceTexUnitOGL::Construct()
{
	IFXRESULT rc = CIFXDeviceTexUnit::Construct();
	
	InitData();
	
	return rc;
}

void CIFXDeviceTexUnitOGL::InitData()
{
	m_eglTarget = GL_TEXTURE_2D;
	m_bCubeMap = FALSE;
	m_pDevice = 0;
	m_pOGL = 0;
	m_uTexUnitNum = 0;
}



// END OF FILE


