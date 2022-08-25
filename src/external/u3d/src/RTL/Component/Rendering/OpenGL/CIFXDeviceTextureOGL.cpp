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
// CIFXDeviceTextureOGL.h
#include "IFXRenderPCHOGL.h"
#include "CIFXDeviceTextureOGL.h"

U8*	CIFXDeviceTextureOGL::ms_pSurface = 0;
U32 CIFXDeviceTextureOGL::ms_uSurfaceSize = 0;

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureOGLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTextureOGL* pPtr = new CIFXDeviceTextureOGL;
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

U32 CIFXDeviceTextureOGL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTextureOGL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTextureOGL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexture)
		{
			*(CIFXDeviceTexture**)ppInterface = (CIFXDeviceTexture*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTextureOGL)
		{
			*(CIFXDeviceTextureOGL**)ppInterface = (CIFXDeviceTextureOGL*) this;
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
// Static Methods
//===========================
void CIFXDeviceTextureOGL::DeleteSurfaceCache()
{
	IFXDELETE_ARRAY(ms_pSurface);
	ms_uSurfaceSize = 0;
}

//===========================
// Public Methods
//===========================
IFXRESULT CIFXDeviceTextureOGL::Initialize(IFXOpenGL* pOGL, CIFXRenderDeviceOGL& rDevice)
{
	IFXRESULT rc = IFX_OK;

	IFXRELEASE(m_pOGL);

	m_pOGL = pOGL;
	m_spDevice = &rDevice;

	if(m_pOGL)
	{
		m_pOGL->AddRef();
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	m_spDevice->GetCaps(m_RCaps);

	if(IFXSUCCESS(rc))
	{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
		m_pOGL->glGenTextures(1, &m_uTexId);
	}
	
	if(IFXSUCCESS(rc))
	{
		rc = SetDefaults();
	}

	return rc;
}


//===========================
// Protected Methods
//===========================
IFXRESULT CIFXDeviceTextureOGL::SetHWType(IFXenum eType)
{
	m_eType = eType;
	m_eGLType = GetGLTextureType(m_eType, &m_eGLBindingType);
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWRenderFormat(IFXenum eFormat)
{
	m_eRenderFormat = eFormat;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWWidth(U32 uWidth)
{
	m_uWidth = uWidth;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWHeight(U32 uHeight)
{
	m_uHeight = uHeight;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWDepth(U32 uDepth)
{
	m_uDepth = uDepth;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWMaxMipLevel(U32 uMipLevel)
{
	m_uMaxMipLevel = uMipLevel;
	
	GLint iBoundTexture = 0;
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glGetIntegerv(m_eGLBindingType, &iBoundTexture);
	m_pOGL->glBindTexture(m_eGLType, m_uTexId);
	
	m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MAX_LEVEL, uMipLevel);
	
	m_pOGL->glBindTexture(m_eGLType, iBoundTexture);
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWMinFilter(IFXenum eFilter)
{
	m_eMinFilter = eFilter;

	GLint iBoundTexture = 0;
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glGetIntegerv(m_eGLBindingType, &iBoundTexture);
	m_pOGL->glBindTexture(m_eGLType, m_uTexId);
	
	switch(m_eMipMode)
	{
	case IFX_NONE:
		if(eFilter == IFX_LINEAR)
		{
			m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		break;
	case IFX_NEAREST:
		if(m_pOGL->AutoMipMapSupport() && m_eGLType == GL_TEXTURE_2D)
		{
		  m_pOGL->glTexParameteri(GL_TEXTURE_2D, (GLenum) GL_GENERATE_MIPMAP_SGIS, TRUE);
		}
		if(eFilter == IFX_LINEAR)
		{
			m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		break;
	case IFX_LINEAR:
		if(m_pOGL->AutoMipMapSupport() && m_eGLType == GL_TEXTURE_2D)
		{
			m_pOGL->glTexParameteri(GL_TEXTURE_2D, (GLenum) GL_GENERATE_MIPMAP_SGIS, TRUE);
		}
		if(eFilter == IFX_LINEAR)
		{
			m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		break;
	}
	
	m_pOGL->glBindTexture(m_eGLType, iBoundTexture);
	
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWMagFilter(IFXenum eFilter)
{
	m_eMagFilter = eFilter;
	
	GLint iBoundTexture = 0;
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glGetIntegerv(m_eGLBindingType, &iBoundTexture);
	m_pOGL->glBindTexture(m_eGLType, m_uTexId);
	
	if(eFilter == IFX_LINEAR)
	{
		m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		m_pOGL->glTexParameteri(m_eGLType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	m_pOGL->glBindTexture(m_eGLType, iBoundTexture);
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWMipMode(IFXenum eMode)
{
	m_eMipMode = eMode;
	
	return SetHWMinFilter(m_eMinFilter);
}

IFXRESULT CIFXDeviceTextureOGL::SetHWDynamic(BOOL bDyanmic)
{
	m_bDynamic = bDyanmic;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureOGL::SetHWTexture()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXDeviceTextureOGL::Lock(	IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;

	if(m_eType != eType && m_eType == IFX_TEXTURE_2D)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(m_eType == IFX_TEXTURE_CUBE && eType == IFX_TEXTURE_2D)
	{
		eType = IFX_TEXTURE_CUBE_PZ;
	}

	if(IFXSUCCESS(rc))
	{
		U32 uSize = m_uWidth * m_uHeight * 4;
		if(uSize > ms_uSurfaceSize)
		{
			ms_uSurfaceSize = 0;
			IFXDELETE_ARRAY(ms_pSurface);
			ms_pSurface = new U8[uSize];

			if(ms_pSurface == 0)
			{
				rc = IFX_E_OUT_OF_MEMORY;
			}
			else
			{
				ms_uSurfaceSize = uSize;
			}
		}
	}

	if(IFXSUCCESS(rc))
	{
		rTex.m_width = m_uWidth >> uMipLevel;
		if(rTex.m_width == 0)
		{
			rTex.m_width = 1;
		}

		rTex.m_height = m_uHeight >> uMipLevel;
		if(rTex.m_height == 0)
		{
			rTex.m_height = 1;
		}

		rTex.eChannelOrder = IFX_BGRA;
		rTex.eRenderFormat = IFX_RGBA_8888;
		rTex.m_pData = (U8*)ms_pSurface;
		rTex.m_pitch = rTex.m_width * 4;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureOGL::Unlock(	IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;

	GLint iBoundTexture = 0;
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glGetIntegerv(m_eGLBindingType, &iBoundTexture);
	m_pOGL->glBindTexture(m_eGLType, m_uTexId);

	GLenum eFormat = GL_RGBA;

	switch(m_eRenderFormat)
	{
	case IFX_RGBA_8888:
		eFormat = GL_RGBA8;
		break;
	case IFX_RGBA_8880:
		eFormat = GL_RGB8;
		break;
	case IFX_RGBA_5550:
		eFormat = GL_RGB5;
		break;
	case IFX_RGBA_5551:
		eFormat = GL_RGB5_A1;
		break;
	case IFX_RGBA_5650:
		eFormat = GL_RGB5;
		break;
	case IFX_RGBA_4444:
		eFormat = GL_RGBA4;
		break;

	}
	
	if(ms_pSurface)
	{
		if(m_bDirty)
		{
///@todo: investigate why it was #ifdef WIN32
			if(m_pOGL->PackedPixelSupport())
			{
				m_pOGL->glTexImage2D(	GetGLTextureType(eType), uMipLevel, eFormat,	
							rTex.m_width, rTex.m_height, 0, (GLenum) GL_BGRA_EXT,
										GL_UNSIGNED_INT_8_8_8_8_REV, ms_pSurface	);
			}
			else
			{
				m_pOGL->glTexImage2D(	GetGLTextureType(eType), uMipLevel, eFormat,	
							rTex.m_width, rTex.m_height, 0, (GLenum) GL_BGRA_EXT,
										GL_UNSIGNED_BYTE, ms_pSurface	);
			}
//#endif
		}
		else
		{
///@todo: investigate why it was #ifdef WIN32
			if(m_pOGL->PackedPixelSupport())
			{
				m_pOGL->glTexSubImage2D(GetGLTextureType(eType), uMipLevel, 0, 0,	
										rTex.m_width, rTex.m_height, (GLenum) GL_BGRA_EXT,
										GL_UNSIGNED_INT_8_8_8_8_REV, ms_pSurface	);
			}
			else
			{
				m_pOGL->glTexSubImage2D(GetGLTextureType(eType), uMipLevel, 0, 0,	
										rTex.m_width, rTex.m_height, (GLenum) GL_BGRA_EXT,
										GL_UNSIGNED_BYTE, ms_pSurface	);
			}
//#endif

		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	m_pOGL->glBindTexture(m_eGLType, iBoundTexture);

	if(m_eMipMode != IFX_NONE && m_pOGL->AutoMipMapSupport() && m_eGLType == GL_TEXTURE_2D)
	{
		m_bNeedData = FALSE;
	}
	
	return rc;
}

GLenum CIFXDeviceTextureOGL::GetGLTextureType(IFXenum eType, GLenum* glBinding)
{
	GLenum eVal = GL_TEXTURE_2D;

	switch(eType)
	{
	case IFX_TEXTURE_1D:
		eVal = GL_TEXTURE_1D;
		if(glBinding)
			*glBinding = GL_TEXTURE_BINDING_1D;
		break;
	case IFX_TEXTURE_2D:
		eVal = GL_TEXTURE_2D;
		if(glBinding)
			*glBinding = GL_TEXTURE_BINDING_2D;
		break;
	case IFX_TEXTURE_3D:
		eVal = GL_TEXTURE_3D;
		break;
	case IFX_TEXTURE_CUBE:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_ARB;
		if(glBinding)
			*glBinding = (GLenum) GL_TEXTURE_BINDING_CUBE_MAP_ARB;
		break;
	case IFX_TEXTURE_CUBE_PX:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
		break;
	case IFX_TEXTURE_CUBE_PY:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
		break;
	case IFX_TEXTURE_CUBE_PZ:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
		break;
	case IFX_TEXTURE_CUBE_NX:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
		break;
	case IFX_TEXTURE_CUBE_NY:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
		break;
	case IFX_TEXTURE_CUBE_NZ:
		eVal = (GLenum) GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
		break;
	}

	return eVal;
}

CIFXDeviceTextureOGL::CIFXDeviceTextureOGL()
{
	m_refCount = 0;
}

CIFXDeviceTextureOGL::~CIFXDeviceTextureOGL()
{
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	m_pOGL->glDeleteTextures(1, &m_uTexId);

	IFXRELEASE(m_pOGL);
}

IFXRESULT CIFXDeviceTextureOGL::Construct()
{
	IFXRESULT rc = CIFXDeviceTexture::Construct();
	
	InitData();
		
	return rc;
}

void CIFXDeviceTextureOGL::InitData()
{
	m_uTexId = 0;
	m_pOGL = 0;
	m_eGLType = GL_TEXTURE_2D;
	m_eGLBindingType = GL_TEXTURE_BINDING_2D;
}


