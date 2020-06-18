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

#include <GL/gl.h>

// OpenGL1.2 and common extensions
#include <GL/glext.h>

#include "IFXBaseOpenGL.h"
#include "IFXRenderWindow.h"

typedef void* (*GLXGetProcAddress) (GLubyte*);
typedef void (*GLXSwapBuffers) (Display*, GLXDrawable);
typedef XVisualInfo* (*GLXChooseVisual) (Display*, int, int*);
typedef GLXContext (*GLXCreateContext) (Display*, XVisualInfo*, GLXContext, Bool);
typedef Bool (*GLXMakeCurrent) (Display*, GLXDrawable, GLXContext);
typedef GLXContext (*GLXGetCurrentContext) (void);
typedef void (*GLXDestroyContext) (Display*, GLXContext);

/**
 *	Interface ID for the IFXOpenGLOS interface.
 */
// {8A800407-6EF7-4612-9652-0E66992CB388}
IFXDEFINE_GUID(IID_IFXOpenGLOS,
0x8a800407, 0x6ef7, 0x4612, 0x96, 0x52, 0xe, 0x66, 0x99, 0x2c, 0xb3, 0x88);

struct IFXOGLData
{
	I32		m_iPixelFormat;
	IFXRenderWindow win;
	GLXContext m_Ctx;
	GLXDrawable m_Draw;
	GLXContext m_OldCtx;

	IFXOGLData()
	{
		m_iPixelFormat = 0;
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
	GLXGetProcAddress glxGetProcAddress;
	GLXSwapBuffers glxSwapBuffers;
	GLXChooseVisual glxChooseVisual;
	GLXCreateContext glxCreateContext;
	GLXMakeCurrent glxMakeCurrent;
	GLXGetCurrentContext glxGetCurrentContext;
	GLXDestroyContext glxDestroyContext;

	Display* pTestDpy;
	Window	testWindow;
	XVisualInfo* pTestVisInfo;
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

