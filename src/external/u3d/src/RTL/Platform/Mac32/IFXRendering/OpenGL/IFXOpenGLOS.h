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
// IFXOpenGLOS.h
#ifndef IFX_OPENGL_OS_H
#define IFX_OPENGL_OS_H

#ifndef APIENTRY
#define APIENTRY
#endif

#include <OpenGL/gl.h>
#include <AGL/agl.h>

// OpenGL1.2 and common extensions
#include <OpenGL/glext.h>

#include "IFXBaseOpenGL.h"
#include "IFXRenderWindow.h"

typedef void* (*AGLGetProcAddress) (GLubyte*);
typedef AGLContext  (* AGLCreateContext)  (AGLPixelFormat pix, AGLContext share);
typedef GLboolean (* AGLUpdateContext)  (AGLContext ctx);
typedef GLboolean (* AGLSetDrawable)    (AGLContext, AGLDrawable);
typedef AGLDrawable (* AGLGetDrawable)    (AGLContext);
typedef AGLContext  (* AGLGetCurrentContext)(void);
typedef GLboolean   (* AGLCopyContext)    (AGLContext src, AGLContext dst, GLuint mask);
typedef GLboolean   (* AGLDestroyContext) (AGLContext);
typedef GLboolean   (* AGLSetCurrentContext)(AGLContext);
typedef void      (* AGLSwapBuffers)    (AGLContext);
typedef GLboolean   (* AGLSetInteger)   (AGLContext, GLenum pname, GLint *params);
typedef AGLPixelFormat (* AGLChoosePixelFormat)(AGLDevice *gdev, GLint ndev, const GLint *attribs);
typedef GLboolean   (* AGLEnable)     (AGLContext,GLenum);
typedef GLboolean   (* AGLDisable)      (AGLContext,GLenum);
typedef void    (* AGLDestroyPixelFormat)(AGLPixelFormat);
typedef GLboolean   (* AGLSetOffScreen)   (AGLContext,GLsizei,GLsizei,GLsizei,GLvoid *baseaddr);
typedef GLboolean   (*AGLDescribePixelFormat)(AGLPixelFormat pix, GLint attrib, GLint *value);
typedef AGLRendererInfo (* AGLQueryRendererInfo)(const AGLDevice *gdevs, GLint ndev);
typedef void      (* AGLDestroyRendererInfo)(AGLRendererInfo rend);
typedef AGLRendererInfo (* AGLNextRendererInfo)(AGLRendererInfo rend);
typedef GLboolean     (* AGLDescribeRenderer)(AGLRendererInfo rend, GLint prop, GLint *value);

/**
 *	Interface ID for the IFXOpenGLOS interface.
 */
// {8A800407-6EF7-4612-9652-0E66992CB388}
IFXDEFINE_GUID(IID_IFXOpenGLOS,
0x8a800407, 0x6ef7, 0x4612, 0x96, 0x52, 0xe, 0x66, 0x99, 0x2c, 0xb3, 0x88);

struct IFXOGLData
{
	WindowPtr				m_window;
	CGrafPtr				m_port;    // port that we render to
	AGLContext				m_hAGL;    // handle to open gl context
	AGLPixelFormat			m_pixFmt;
	AGLContext				m_hOldAGL;
	CGrafPtr				m_oldPort;
	long					m_lPixelsize;
	Rect					m_Rect;
	PixMapHandle			m_hPixelMap;
	GWorldPtr				m_pgWorld;
	GLint					m_pBufferRect[4];
	I32						m_bFirstPassAfterInit;
	I32						m_runningInClassic;

	IFXOGLData()
	{
		m_window = 0;
		m_port = 0;
		m_hAGL = 0;
		m_pixFmt = 0;
		m_hOldAGL = 0;
		m_oldPort = 0;
		m_lPixelsize = 0;
		m_hPixelMap = 0;
		m_pgWorld = 0;
		m_bFirstPassAfterInit = TRUE;
		m_runningInClassic = FALSE;
		m_pBufferRect[0] = 0;
		m_pBufferRect[1] = 0;
		m_pBufferRect[2] = 0;
		m_pBufferRect[3] = 0;
	}

	~IFXOGLData()
	{
	}
};

/**
 *	The IFXOpenGLOS interface contains function pointers for all
 *	OS specific OpenGL function calls.  
 */
class IFXOpenGLOS : virtual public IFXOpenGL
{
//	IFX_DECLARE_IDENTITY(IFXOpenGLOS);
public:
	AGLCreateContext       aglCreateContext;
	AGLUpdateContext       aglUpdateContext;
	AGLGetCurrentContext   aglGetCurrentContext;
	AGLGetDrawable         aglGetDrawable;
	AGLCopyContext         aglCopyContext;
	AGLDestroyContext      aglDestroyContext;
	AGLSetCurrentContext   aglSetCurrentContext;
	AGLSetDrawable         aglSetDrawable;
	AGLSetInteger          aglSetInteger;
	AGLChoosePixelFormat   aglChoosePixelFormat;
	AGLDestroyPixelFormat  aglDestroyPixelFormat;
	AGLSetOffScreen        aglSetOffScreen;
	AGLEnable              aglEnable;
	AGLDisable             aglDisable;
	AGLDescribePixelFormat aglDescribePixelFormat;
	AGLQueryRendererInfo   aglQueryRendererInfo;
	AGLDestroyRendererInfo aglDestroyRendererInfo;
	AGLNextRendererInfo    aglNextRendererInfo;
	AGLDescribeRenderer    aglDescribeRenderer;  
	AGLSwapBuffers         aglSwapBuffers;

protected:
	/**
	 *	Constructor - this is empty
	 */
	IFXOpenGLOS() {}
	/**
	 *	Destructor - this is empty
	 */
	virtual ~IFXOpenGLOS() {}
};

#endif // IFX_OPENGL_OS_H

