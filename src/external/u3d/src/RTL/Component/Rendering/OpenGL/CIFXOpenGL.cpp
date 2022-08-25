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
// CIFXOpenGL.cpp

#include "IFXRenderPCHOGL.h"
#include "CIFXOpenGL.h"
#include "IFXOSLoader.h"

IFXRESULT CIFXOpenGL::Construct()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXOpenGL::LoadExtensions()
{
	IFXRESULT rc = IFX_OK;

	const char* szExtensions = (const char*)glGetString(GL_EXTENSIONS);
	char* pExts = new char[strlen(szExtensions)+1];
	if(pExts)
	{
#ifdef IFXDEBUG_OGLINFO
		IFXTRACE_GENERIC(L"\nOpenGL Information:\n");
		IFXTRACE_GENERIC(L"\tVendor:          %hs\n", glGetString(GL_VENDOR));
		IFXTRACE_GENERIC(L"\tRenderer:        %hs\n", glGetString(GL_RENDERER));
		IFXTRACE_GENERIC(L"\tVersion:         %hs\n", glGetString(GL_VERSION));
		IFXTRACE_GENERIC(L"\tExtensions:\n");
#endif
		strcpy(pExts, szExtensions);
		char* pExt = pExts;
		for(pExt = strtok(pExts, " "); pExt; pExt = strtok(NULL, " "))
		{
#ifdef IFXDEBUG_OGLINFO
			IFXTRACE_GENERIC(L"\t\t%hs\n", pExt);
#endif
			if(!strcmp(pExt, "GL_ARB_multitexture"))
			{
				m_bMultiTexture = TRUE;
			}
			else if(!strcmp(pExt, "GL_ARB_texture_compression"))
			{
				m_bTexCompress = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_packed_pixels"))
			{
				m_bPackedPixels = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_bgra"))
			{
				m_bBGRA = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_separate_specular_color"))
			{
				m_bSeparateSpecular = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_texture_env_combine"))
			{
				m_bTexCombine = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_texture_env_add"))
			{
				m_bTexAdd = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_texture_env_dot3"))
			{
				m_bTexDot3 = TRUE;
			}
			else if(!strcmp(pExt, "GL_EXT_texture_cube_map"))
			{
				m_bCubeTexture = TRUE;
			}
			else if(!strcmp(pExt, "GL_ARB_texture_cube_map"))
			{
				m_bCubeTexture = TRUE;
			}
			else if(!strcmp(pExt, "GL_SGIS_generate_mipmap"))
			{
				m_bAutoMipMap = TRUE;
			}
		}
		IFXDELETE_ARRAY(pExts);

		const char* szVersion = (const char*)glGetString(GL_VERSION);
		I32 iMajor;
		I32 iMinor;
		sscanf(szVersion, "%d.%d", &iMajor, &iMinor);

		if(iMinor >= 2 && strcmp((const char*)glGetString(GL_VENDOR), "ATI Technologies Inc."))
		{
			m_bOpenGL_1_2 = TRUE; ///@todo: it is not right way to check 1.2 OpenGL version
			m_bPackedPixels = TRUE;
		}
	}
	else
	{
		rc = IFX_E_OUT_OF_MEMORY;
	}
				
	if(m_bOpenGL_1_2)
	{
		glDrawRangeElements =	(GLDrawRangeElements)	GetOGLExtensionFunc("glDrawRangeElements");
	}

	if(m_bMultiTexture)
	{
		glActiveTextureARB =		(GLActiveTextureARB)		GetOGLExtensionFunc("glActiveTextureARB");
		glClientActiveTextureARB =	(GLClientActiveTextureARB)	GetOGLExtensionFunc("glClientActiveTextureARB");
	}

/*	if(m_bTexCompress)
	{
		glCompressedTexImage3DARB =		(GLCompressedTexImage3DARB)		GetOGLExtensionFunc("glCompressedTexImage3DARB");
		glCompressedTexImage2DARB =		(GLCompressedTexImage2DARB)		GetOGLExtensionFunc("glCompressedTexImage2DARB");
		glCompressedTexImage1DARB =		(GLCompressedTexImage1DARB)		GetOGLExtensionFunc("glCompressedTexImage1DARB");
		glCompressedTexSubImage3DARB =	(GLCompressedTexSubImage3DARB)	GetOGLExtensionFunc("glCompressedTexSubImage3DARB");
		glCompressedTexSubImage2DARB =	(GLCompressedTexSubImage2DARB)	GetOGLExtensionFunc("glCompressedTexSubImage2DARB");
		glCompressedTexSubImage1DARB =	(GLCompressedTexSubImage1DARB)	GetOGLExtensionFunc("glCompressedTexSubImage1DARB");
		glGetCompressedTexImageARB =	(GLGetCompressedTexImageARB)	GetOGLExtensionFunc("glGetCompressedTexImageARB");
	}
*/
	
	return rc;
}

IFXOGLPROC IFXAPI CIFXOpenGL::GetOGLExtensionFunc(const char* szFuncName)
{
	IFXHANDLE m_hLibrary = NULL;
	IFXGetAddress(m_hLibrary, szFuncName);
	return (IFXOGLPROC)m_hLibrary;
}



