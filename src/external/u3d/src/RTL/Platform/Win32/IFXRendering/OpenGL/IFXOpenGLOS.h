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
#ifndef IFX_OPENGL_WIN_H
#define IFX_OPENGL_WIN_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GL/gl.h>
#include <glext.h>

#include "IFXBaseOpenGL.h"
#include "IFXRenderWindow.h"

// WGL functions
typedef BOOL  (WINAPI * WGLCopyContext)		(HGLRC, HGLRC, UINT);
typedef HGLRC (WINAPI * WGLCreateContext)	(HDC);
typedef HGLRC (WINAPI * WGLCreateLayerContext)(HDC, int);
typedef BOOL  (WINAPI * WGLDeleteContext)	(HGLRC);
typedef HGLRC (WINAPI * WGLGetCurrentContext)(void);
typedef HDC   (WINAPI * WGLGetCurrentDC)	(void);
typedef PROC  (WINAPI * WGLGetProcAddress)	(LPCSTR);
typedef BOOL  (WINAPI * WGLMakeCurrent)		(HDC, HGLRC);
typedef BOOL  (WINAPI * WGLShareLists)		(HGLRC, HGLRC);
typedef BOOL  (WINAPI * WGLSwapLayerBuffers)(HDC, unsigned int);
typedef BOOL  (WINAPI * WGLSwapIntervalEXT) (int);

// For Nvidia's VAR extension
typedef void* (WINAPI * WGLAllocateMemoryNV) (GLsizei, GLfloat, GLfloat, GLfloat);
typedef void  (WINAPI * WGLFreeMemoryNV)	(void*);

/**
 *	Interface ID for the IFXOpenGLOS interface.
 */
// {8A800407-6EF7-4612-9652-0E66992CB388}
IFXDEFINE_GUID(IID_IFXOpenGLOS,
0x8a800407, 0x6ef7, 0x4612, 0x96, 0x52, 0xe, 0x66, 0x99, 0x2c, 0xb3, 0x88);

struct IFXOGLData
{
	IFXRenderWindow win;
	HDC		m_hDC;
	HDC		m_hMemDC;
	HWND	m_hWnd;
	HWND	m_hParentWnd;
	HGLRC	m_hRC;
	
	I32		m_iPixelFormat;

	IFXOGLData()
	{
		m_hDC = 0;
		m_hMemDC = 0;
		m_hWnd = 0;
		m_hParentWnd = 0;
		m_hRC = 0;
		m_iPixelFormat = 0;
	}
};

/**
 *	The IFXOpenGLOS interface contains function pointers for all
 *	Microsoft* Windows* specific OpenGL function calls.  Please see
 *	the OpenGL reference at http://msdn.microsoft.com/library for
 *	further documentation of what each function does.
 */
class IFXOpenGLOS : virtual public IFXOpenGL
{
//	IFX_DECLARE_IDENTITY(IFXOpenGLOS);
public:
	WGLCopyContext 			wglCopyContext;
	WGLCreateContext 		wglCreateContext;
	WGLCreateLayerContext 	wglCreateLayerContext;
	WGLDeleteContext 		wglDeleteContext;
	WGLGetCurrentContext 	wglGetCurrentContext;
	WGLGetCurrentDC 		wglGetCurrentDC;
	WGLGetProcAddress 		wglGetProcAddress;
	WGLMakeCurrent 			wglMakeCurrent;
	WGLShareLists 			wglShareLists;
	WGLSwapLayerBuffers		wglSwapLayerBuffers;
	WGLSwapIntervalEXT		wglSwapIntervalEXT;

	WGLAllocateMemoryNV		wglAllocateMemoryNV;
	WGLFreeMemoryNV			wglFreeMemoryNV;

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

#endif // IFX_OPENGL_WIN_H

