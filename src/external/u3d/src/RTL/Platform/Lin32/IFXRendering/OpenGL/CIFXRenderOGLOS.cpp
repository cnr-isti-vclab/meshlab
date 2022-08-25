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
// CIFXRenderOGLOS.cpp

#include "IFXRenderPCHOGL.h"
#include "CIFXRenderOGL.h"

//***************************************************************************
//	Local function prototypes
//***************************************************************************

void Screen2Client(Display* pDisplay, Window thisWin, int *x, int *y);

//***************************************************************************
//	Public methods
//***************************************************************************

IFXRESULT CIFXRenderDeviceOGL::GetSupported(U32 uDeviceNum, BOOL& bSupported)
{
	IFXRESULT rc = IFX_OK;

	if(!m_pOGL)
	{
		rc = IFXCreateComponent(CID_IFXOpenGL, IID_IFXOpenGL, (void**)&m_pOGL);
	}

	if(!m_pOSOGL && IFXSUCCESS(rc))
	{
		rc = m_pOGL->QueryInterface(IID_IFXOpenGLOS, (void**)&m_pOSOGL);
	}


	if(IFXSUCCESS(rc))
		rc = m_pOGL->LoadOSGL();

	if(IFXSUCCESS(rc))
	{
		m_glData.m_OldCtx = m_pOSOGL->glxGetCurrentContext();
	}

	m_pOSOGL->m_bValidOpenGL = FALSE;

	if(IFXSUCCESS(rc))
	{
		bSupported = TRUE;
	}
	else
	{
		bSupported = FALSE;
		rc = IFX_OK;
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::SetVSyncEnabledOS(BOOL bEnabled)
{
	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::DestroyOS()
{
	if(m_pOSOGL)
	{
		m_pOSOGL->m_bValidOpenGL = FALSE;
	}
	if (m_glData.m_Ctx)
	{
		glXDestroyContext((Display*)m_glData.win.GetWindowPtr(), m_glData.m_Ctx);
		m_glData.m_Ctx = NULL;
	}
	if (m_glData.win.GetWindowPtr())
	{
		XFree(m_pOSOGL->pTestVisInfo);
		XDestroyWindow(m_pOSOGL->pTestDpy, m_pOSOGL->testWindow);
		XCloseDisplay(m_pOSOGL->pTestDpy);
	}
	if(m_pOSOGL)
	{
		m_pOSOGL->m_bValidOpenGL = FALSE;
		m_glData.win.SetWindowPtr(NULL);
		IFXRELEASE(m_pOSOGL);
	}

	IFXUnuseRenderWindowClassWin();

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::InitContextOS()
{
	IFXRESULT rc = IFX_OK;
	Display *pDisplay = (Display*)m_glData.win.GetWindowPtr();
	XVisualInfo *visInfo = m_glData.win.GetVisual();

	if (NULL == pDisplay || NULL == visInfo || NULL == m_pOSOGL) 
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) 
	{
		m_glData.m_Ctx = m_pOSOGL->glxCreateContext(pDisplay, 
										visInfo, 
										NULL, 
										TRUE);
		if (NULL == m_glData.m_Ctx)
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
			IFXTRACE_GENERIC(L"IFX_E_SUBSYSTEM_FAILURE ---InitContextOS() --- Can't create Context\n");
		}
	}

	if (IFXSUCCESS(rc))
		rc = MakeDeviceCurrent();

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::InitWindowOS()
{
	IFXRESULT rc = IFX_OK;

	if (NULL == m_pOSOGL)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc) && NULL == m_glData.win.GetWindowPtr())
	{
		Display *pDisplay;
		Window win, rootwin;
		XVisualInfo *VisInfo;
		int attrib[] = {
			GLX_RGBA,
				GLX_RED_SIZE, 1,
				GLX_GREEN_SIZE, 1,
				GLX_BLUE_SIZE, 1,
				GLX_DEPTH_SIZE, 1,
				GLX_DOUBLEBUFFER,
				None 
		};
		XSetWindowAttributes WinAttr;
		int scrnum;
		IFXRenderWindow RenderWin;
		U32 mask;

		pDisplay = XOpenDisplay(NULL);
		if (!pDisplay)
			rc = IFX_E_UNSUPPORTED;
		if (IFXSUCCESS(rc))
		{
			scrnum = DefaultScreen(pDisplay);
			VisInfo = m_pOSOGL->glxChooseVisual(pDisplay, scrnum, attrib);
			if (!VisInfo)
				rc = IFX_E_UNSUPPORTED;
		}
		if (IFXSUCCESS(rc))
		{
			rootwin = RootWindow(pDisplay, scrnum);
			if (!rootwin)
				rc = IFX_E_UNSUPPORTED;
		}
		if (IFXSUCCESS(rc))
		{
			WinAttr.colormap = XCreateColormap(pDisplay, rootwin, VisInfo->visual, AllocNone);
			WinAttr.event_mask = StructureNotifyMask | ExposureMask;
			mask = CWBorderPixel | CWColormap;
			win = XCreateWindow( pDisplay, rootwin, 
				0, 0, 1, 1, 
				0, VisInfo->depth, InputOutput, VisInfo->visual, mask, &WinAttr);
		}

		if (IFXSUCCESS(rc))
		{
			m_pOSOGL->pTestDpy = pDisplay;
			m_pOSOGL->pTestVisInfo = VisInfo;
			m_pOSOGL->testWindow = win;

			m_glData.win.SetWindowPtr(pDisplay);
			m_glData.win.SetDrawable(win);
			m_glData.win.SetVisual(VisInfo);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::MakeDeviceCurrent()
{
	IFXRESULT rc = IFX_OK;

	if(m_pOSOGL)
	{
		Display *pDisplay = (Display*)(Display*)m_glData.win.GetWindowPtr();
		Window win = m_glData.win.GetDrawable();
		if( glXMakeCurrent(pDisplay, win, m_glData.m_Ctx) )
		{
			m_pOSOGL->m_bValidOpenGL = TRUE;
			rc = IFX_OK;
		}
		else
			rc = IFX_E_SUBSYSTEM_FAILURE;
	}
	else
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXFAILURE(rc))
	{
		IFXTRACE_GENERIC(L"MakeDeviceCurrent\n(Warning): Have INVALID Render Context\n\n");
		m_pOSOGL->m_bValidOpenGL = FALSE;
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::DestroyOS()
{
	// We need to make current before we call any openg methods
	// Unfortunately, the window has been destroyed already, so
	// doing a regular make current on our own window won't work.
	// So, we do a make current on the window that is owned by the
	// device.  This will allow the textures to be deleted using
	// a valid rendering context.
	if(m_spDeviceOGL.IsValid())
	{
		m_spDeviceOGL->MakeDeviceCurrent();
	}

	IFXRELEASE(m_pOSOGL);

	return IFX_OK;
}

IFXRESULT CIFXRenderOGL::InitializeOS()
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pOSOGL);
		rc = m_pOGL->QueryInterface(IID_IFXOpenGLOS, (void**)&m_pOSOGL);
	}
	m_glData.m_Ctx = 0;

	return rc;
}

IFXRESULT CIFXRenderOGL::MakeCurrentOS()
{
	IFXRESULT rc = IFX_OK;
	IFXOGLData glData;

	if (NULL == m_pOSOGL) 
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) 
	{
		m_spDeviceOGL->GetOpenGLData(glData);
		glData.m_OldCtx = m_pOSOGL->glxGetCurrentContext();
	
		if( glData.m_OldCtx != glData.m_Ctx )
		{
			if( glData.m_Ctx )
			{
				rc = m_spDeviceOGL->MakeDeviceCurrent();
			}
			else
				rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::SetHWWindowOS(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	if (NULL == m_pOSOGL)
		rc = IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(rc) && m_spDevice.IsValid())
	{
		IFXOGLData glData;
		Display* pDisplay = (Display*)m_glData.win.GetWindowPtr();
		Window win = m_glData.win.GetDrawable();
		XVisualInfo *VisInfo;

		if (NULL == m_glData.m_Ctx) 
		{
			m_spDeviceOGL->GetOpenGLData(glData);
	
			pDisplay = (Display*)glData.win.GetWindowPtr();
			if (NULL != pDisplay && glData.m_Ctx) 
			{
				if (glData.m_Ctx == m_pOSOGL->glxGetCurrentContext()) 
				{
					m_pOSOGL->glxMakeCurrent(pDisplay, None, NULL);
				}
				m_pOSOGL->glxDestroyContext(pDisplay, glData.m_Ctx);
				glData.m_Ctx = NULL;
			}
	
			if (m_Window.GetWindowPtr())
			{
				m_glData.win.SetWindowPtr(m_Window.GetWindowPtr());
				m_glData.win.SetDrawable(m_Window.GetDrawable());
				m_glData.win.SetVisual(m_Window.GetVisual());
				m_glData.win.SetWindowSize(m_Window.GetWindowSize());

				pDisplay = (Display*)m_glData.win.GetWindowPtr();
				VisInfo = m_glData.win.GetVisual();
#ifdef IFXDEBUG_OGLINFO
				IFXTRACE_GENERIC(L"---SetHWWindowOS(BOOL bDirty)---\n\n");
				 IFXTRACE_GENERIC(L"VisInfo->bits_per_rgb: %i\n", VisInfo->bits_per_rgb);
				 IFXTRACE_GENERIC(L"VisInfo->blue_mask: %i\n", VisInfo->blue_mask);
				 IFXTRACE_GENERIC(L"VisInfo->red_mask: %i\n", VisInfo->red_mask);
				 IFXTRACE_GENERIC(L"VisInfo->green_mask: %i\n", VisInfo->green_mask);
				 IFXTRACE_GENERIC(L"VisInfo->c_class: %i\n", VisInfo->c_class);
				 IFXTRACE_GENERIC(L"VisInfo->colormap_size: %i\n", VisInfo->colormap_size);
				 IFXTRACE_GENERIC(L"VisInfo->depth: %i\n", VisInfo->depth);
				 IFXTRACE_GENERIC(L"VisInfo->screen: %i\n", VisInfo->screen);
#endif
				 if (!VisInfo) {
					IFXTRACE_GENERIC(L"Error: couldn't get an RGB, Double-buffered visual\n");
				 }

				 win = m_glData.win.GetDrawable();
				m_glData.m_Ctx = m_pOSOGL->glxCreateContext(pDisplay, 
												VisInfo, 
												None, 
												GL_TRUE);
	
				if (NULL == m_glData.m_Ctx)
					rc = IFX_E_SUBSYSTEM_FAILURE;
	
				if( m_pOSOGL->glxMakeCurrent(pDisplay, win, m_glData.m_Ctx) )
				{
					m_pOSOGL->m_bValidOpenGL = TRUE;
					rc = IFX_OK;
				}
				else
				{
					IFXTRACE_GENERIC(L"SetHWWindowOS -- INVALID Render Context\n\n");
					rc = IFX_E_SUBSYSTEM_FAILURE;
				}
	
				m_spDeviceOGL->SetOpenGLData(m_glData);

				XMapWindow(pDisplay, win);
			}
			m_bNeedClear = TRUE;
		}

		if (IFXSUCCESS(rc) && NULL != pDisplay) 
		{
			XWindowAttributes attributes;
			XGetWindowAttributes(pDisplay, win, &attributes);
			IFXRect rcMon = m_rcDevice;
			Screen2Client(pDisplay, win, &rcMon.m_X, &rcMon.m_Y);
	
				m_rcBackBuffer.Set( attributes.x, attributes.y, attributes.width, attributes.height );
				rc = SizeBackBuffer(rcMon);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::ResizeBufferOS()
{
	return IFX_OK;
}

IFXRESULT CIFXRenderOGL::ClearOS(const IFXRenderClear& rClear)
{
	IFXRESULT rc = IFX_OK;

	if (NULL == m_pOGL)
		rc = IFX_E_NOT_INITIALIZED;
	
	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(IFXSUCCESS(rc))
	{
		U32 uBuffers = 0;
		if(m_bNeedClear || (rClear.GetColorCleared() && !(!m_Window.GetDTS() && m_Window.GetTransparent())))
		{
			IFXVector3 vColor = rClear.GetColorValue();
			m_pOGL->glClearColor(vColor.X(), vColor.Y(), vColor.Z(), 1.0f);
			uBuffers |= GL_COLOR_BUFFER_BIT;
		}

		if((m_bNeedClear || rClear.GetStencilCleared()) && m_eDepthStencil == IFX_DEPTH_D24S8)
		{
			m_pOGL->glClearStencil(rClear.GetStencilValue());
			uBuffers |= GL_STENCIL_BUFFER_BIT;
		}

		if(m_bNeedClear || rClear.GetDepthCleared())
		{
			m_pOGL->glClearDepth(rClear.GetDepthValue());
			uBuffers |= GL_DEPTH_BUFFER_BIT;
		}

		if ( uBuffers )
		{
			m_pOGL->glClear(uBuffers);
		}
	}

	m_bNeedClear = FALSE;

	return rc;
}

IFXRESULT CIFXRenderOGL::SwapOS()
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	Display* pDisplay = (Display*)m_glData.win.GetWindowPtr();
	GLXDrawable win = m_glData.win.GetDrawable();

	if (NULL != pDisplay && NULL != m_pOSOGL) 
	{
		m_pOSOGL->glxSwapBuffers(pDisplay, win);
	}
	else
		rc = IFX_E_NOT_INITIALIZED;

	return rc;
}

IFXRESULT CIFXRenderOGL::SetHWDTSOS(BOOL bDTS)
{
	m_Window.SetDTS(bDTS);

	return IFX_OK;
}

//***************************************************************************
//	Local functions
//***************************************************************************

void Screen2Client(Display* pDisplay, Window thisWin, int *x, int *y)
{
	int xx = *x;
	int yy = *y;
	Window childWin;
    Window rootWin = RootWindowOfScreen(DefaultScreenOfDisplay(pDisplay));
    XTranslateCoordinates(pDisplay, rootWin, thisWin, xx, yy, x, y, &childWin);
}

