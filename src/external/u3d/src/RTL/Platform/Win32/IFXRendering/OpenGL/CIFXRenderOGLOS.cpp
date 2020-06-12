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
// CIFXRenderOGLWin.cpp

#include "IFXRenderPCHOGL.h"
#include "CIFXRenderOGL.h"

HWND CreateTestWnd()
{
	HWND hWnd= 0;

	IFXUseRenderWindowClassWin();

	hWnd = CreateWindow(IFX_RENDER_WINDOW_CLASS, IFX_RENDER_WINDOW_CLASS,
						WS_OVERLAPPEDWINDOW, 0, 0, 120,120,
						GetDesktopWindow(), 0, GetModuleHandle(0), 0);

	return hWnd;
}

void DestroyTestWnd(HWND hWnd)
{
	if(hWnd)
	{
		DestroyWindow(hWnd);

		IFXUnuseRenderWindowClassWin();
	}
}

IFXRESULT InitOpenGL(HDC hDC, HGLRC& hRC, IFXOpenGLOS* pWinOGL)
{
	IFXRESULT rc = IFX_OK;

	if(hDC)
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
				PFD_SUPPORT_OPENGL |
				PFD_DOUBLEBUFFER |
				PFD_TYPE_RGBA,
			24,
			0, 0, // Red
			0, 0, // Green
			0, 0, // Blue
			0, 0, // Alpha
			0, 0, 0, 0, 0, // Accumulation
			24, // Depth
			0, // Stencil
			0, // Aux
			PFD_MAIN_PLANE, // Layer (ignored)
			0, // Overlay / underlay planes
			0, // Ignored layer mask
			0, // Chromakey info for layer planes
			0 // Damage Mask
		};

		int iPF = ChoosePixelFormat(hDC, &pfd);
		if(iPF)
		{
			if(SetPixelFormat(hDC, iPF, &pfd))
			{
				hRC = pWinOGL->wglCreateContext(hDC);
				if(0 == hRC)
				{
					rc = IFX_E_UNSUPPORTED;
				}
			}
			else
			{
				rc = IFX_E_UNSUPPORTED;
			}
		}
		else
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}
	else
	{
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::GetSupported(U32 uDeviceNum, BOOL& bSupported)
{
	IFXRESULT rc = IFX_OK;

	IFXRenderServicesPtr pServices;
	rc = pServices.Create(CID_IFXRenderServices, IID_IFXRenderServices);

	U32 uNumDevices = 1;

	if(IFXSUCCESS(rc))
	{
		uNumDevices = pServices->GetNumDevices();
	}

	if(1/* == uNumDevices*/)
	{
		if(!m_pOGL)
		{
			rc = IFXCreateComponent(CID_IFXOpenGL, IID_IFXOpenGL, (void**)&m_pOGL);
			if(!m_pOSOGL)
			{
				IFXRUN(rc, m_pOGL->QueryInterface(IID_IFXOpenGLOS, (void**)&m_pOSOGL));
			}
		}

		HDC hOldDC = 0;
		HGLRC hOldRC = 0;

		if(IFXSUCCESS(rc))
		{
			IFXRUN(rc, m_pOGL->LoadOSGL());

			if(IFXSUCCESS(rc))
			{
				hOldDC = m_pOSOGL->wglGetCurrentDC();
				hOldRC = m_pOSOGL->wglGetCurrentContext();
			}

			HWND hWnd = CreateTestWnd();
			HDC hDC = GetDC(hWnd);
			HGLRC hRC = 0;

			if(IFXSUCCESS(rc))
			{
				if(hDC)
				{
					rc = InitOpenGL(hDC, hRC, m_pOSOGL);
				}
				else
				{
					rc = IFX_E_UNSUPPORTED;
				}
			}

			if(IFXSUCCESS(rc))
			{
				if(m_pOSOGL->wglMakeCurrent(hDC, hRC))
				{
					m_pOSOGL->m_bValidOpenGL = TRUE;
					IFXLOGOGL("\nHave VALID Render Context\n\n");
					m_pOSOGL->LoadExtensions();

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
					const char* pString = (const char*)m_pOGL->glGetString(GL_VENDOR);

					if(pString)
					{
						if(strcmp(pString, "Microsoft Corporation"))
						{
							if(m_pOSOGL->TexEnvCombineSupport() && m_pOSOGL->SeparateSpecularSupport())
								bSupported = TRUE;
							else
								bSupported = FALSE;
						}
						else
						{
							bSupported = FALSE;
						}
					}
				}
				else
				{
					m_pOSOGL->m_bValidOpenGL = FALSE;
					IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");
					rc = IFX_E_UNSUPPORTED;
				}

				m_pOSOGL->wglMakeCurrent(hOldDC, hOldRC);
				m_pOSOGL->m_bValidOpenGL = FALSE;
				IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");
				m_pOSOGL->wglDeleteContext(hRC);
				ReleaseDC(hWnd, hDC);
				DestroyTestWnd(hWnd);
			}
		}
	}
	else
	{
		rc = IFX_E_UNSUPPORTED;
	}

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
	if(m_pOSOGL->wglSwapIntervalEXT)
	{
		if(bEnabled)
		{
			m_pOSOGL->wglSwapIntervalEXT(1);
		}
		else
		{
			m_pOSOGL->wglSwapIntervalEXT(0);
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::DestroyOS()
{
	if(m_pOSOGL)
	{
		m_pOSOGL->wglMakeCurrent(0, 0);
		m_pOSOGL->m_bValidOpenGL = FALSE;
		IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");
		m_pOSOGL->wglDeleteContext(m_glData.m_hRC);
	}

	IFXRELEASE(m_pOSOGL);

	if(m_glData.m_hDC)
	{
		ReleaseDC(m_glData.m_hWnd, m_glData.m_hDC);
	}

	if(m_glData.m_hWnd)
	{
		DestroyWindow(m_glData.m_hWnd);
		m_glData.m_hWnd = 0;
	}

	IFXUnuseRenderWindowClassWin();

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::InitContextOS()
{

	IFXRESULT rc = IFX_OK;

	// Set up pixel format descriptor
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), //struct size
			1,							//version number
			PFD_DRAW_TO_WINDOW | //flags, draw to a window
			PFD_SUPPORT_OPENGL | //use OpenGL
			PFD_DOUBLEBUFFER,	//be double buffered
			PFD_TYPE_RGBA,		//RGBA mode window
			24,					//24 bits for color (8,8,8)
			0,0,0,				//RGB bits and shift sizes
			0,0,0,				//Don't care about them
			0,0,				//No alpha buffer info
			0,0,0,0,0,			//No accumulation buffer
			24,					//32 bit depth buffer
			0,					//No stencil buffer
			0,					//No auxilary buffers
			PFD_MAIN_PLANE,		//Layer type
			0,					//Reserved (must be 0)
			0,					//No layer mask
			0,					//No visible mask
			0					//No damage mask
	};

	if(IFXSUCCESS(rc))
	{
		m_glData.m_iPixelFormat = ChoosePixelFormat(m_glData.m_hDC, &pfd);
		if(!SetPixelFormat(m_glData.m_hDC, m_glData.m_iPixelFormat, &pfd))
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	if(IFXSUCCESS(rc))
	{
		m_glData.m_hRC = m_pOSOGL->wglCreateContext(m_glData.m_hDC);
		if(0 == m_glData.m_hRC)
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	if(IFXSUCCESS(rc))
	{
		if(!m_pOSOGL->wglMakeCurrent(m_glData.m_hDC, m_glData.m_hRC))
		{
			m_pOSOGL->m_bValidOpenGL = FALSE;
			IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");
			rc = IFX_E_UNSUPPORTED;
		}
		else
		{
			m_pOSOGL->m_bValidOpenGL = TRUE;
			IFXLOGOGL("\nHave VALID Render Context\n\n");
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::InitWindowOS()
{
	IFXRESULT rc = IFX_OK;

	if(0 == m_glData.m_hWnd)
	{
		m_glData.m_hWnd = CreateWindow(	IFX_RENDER_WINDOW_CLASS, IFX_RENDER_WINDOW_CLASS,
										WS_CHILD, 0, 0,1, 1,
										GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL);
		if(m_glData.m_hWnd)
		{
			m_glData.m_hDC = GetDC(m_glData.m_hWnd);
		}

		if(! (m_glData.m_hDC && m_glData.m_hWnd))
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::MakeDeviceCurrent()
{
	IFXRESULT rc = IFX_OK;

	if(m_pOSOGL && m_pOSOGL->wglMakeCurrent)
	{
		if(m_pOSOGL->wglMakeCurrent(m_glData.m_hDC, m_glData.m_hRC))
		{
			if(m_glData.m_hDC && m_glData.m_hRC)
			{
				m_pOSOGL->m_bValidOpenGL = TRUE;
				IFXLOGOGL("\nHave VALID Render Context\n\n");
				return rc;
			}
		}
	}

	IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");
	m_pOSOGL->m_bValidOpenGL = FALSE;

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

	if(m_pOSOGL)
	{
		if(m_glData.m_hDC)
		{
			ReleaseDC(m_glData.m_hWnd, m_glData.m_hDC);
		}

		if(m_glData.m_hWnd)
		{
			DestroyWindow(m_glData.m_hWnd);
		}
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

	if(IFXSUCCESS(rc))
	{
		IFXOGLData glData;
		m_spDeviceOGL->GetOpenGLData(glData);

		m_glData.m_hRC = glData.m_hRC;
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::MakeCurrentOS()
{
	HDC		hOldDC;
	HGLRC	hOldRC;

	hOldRC = m_pOSOGL->wglGetCurrentContext();
	hOldDC = m_pOSOGL->wglGetCurrentDC();

	if((hOldRC != m_glData.m_hRC) || (hOldDC != m_glData.m_hDC))
	{
		if(m_glData.m_hRC && m_glData.m_hDC)
		{
			if(m_pOSOGL->wglMakeCurrent(m_glData.m_hDC, m_glData.m_hRC))
			{
				m_pOSOGL->m_bValidOpenGL = TRUE;
				IFXLOGOGL("\nHave VALID Render Context\n\n");
			}
			else
			{
				m_pOSOGL->m_bValidOpenGL = FALSE;
				IFXLOGOGL("\n(Warning): Have INVALID Render Context!!\n\n");
			}
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderOGL::SetHWWindowOS(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid())
	{
		IFXRect rcBackSave = m_rcBackBuffer;

		DWORD dwWindStyles = 0;

		if(m_Window.GetDTS())
		{
			m_glData.m_hParentWnd = (HWND)m_Window.GetWindowPtr();
			RECT rcBB;
			GetClientRect(m_glData.m_hParentWnd, &rcBB);
			IFXRect rcMon = m_rcDevice;
			ScreenToClient(m_glData.m_hParentWnd, (LPPOINT)&rcMon.m_X);

			if(IFXSUCCESS(rc))
			{
				m_rcBackBuffer.Set( rcBB.left, rcBB.top, rcBB.right - rcBB.left, rcBB.bottom - rcBB.top );
				rc = SizeBackBuffer(rcMon);
			}

			dwWindStyles = WS_CHILD | WS_VISIBLE;
		}
		else
		{
			m_glData.m_hMemDC = (HDC)m_Window.GetWindowPtr();
			m_glData.m_hParentWnd = 0;
			HBITMAP hBM = (HBITMAP)GetCurrentObject(m_glData.m_hMemDC, OBJ_BITMAP);
			BITMAP bm;
			GetObject(hBM, sizeof(BITMAP), (void*)&bm);

			m_rcBackBuffer.Set(0, 0, bm.bmWidth, bm.bmHeight);

			if(IFXSUCCESS(rc))
			{
				rc = SizeBackBuffer(m_Window.GetWindowSize());
			}

			dwWindStyles = WS_POPUP;
		}

		if(!(rcBackSave == m_rcBackBuffer) || !m_glData.m_hWnd)
		{
			if(IFXSUCCESS(rc))
			{
				if(!m_glData.m_hWnd)
				{
					m_glData.m_hWnd = CreateWindow(	IFX_RENDER_WINDOW_CLASS, IFX_RENDER_WINDOW_CLASS,
													dwWindStyles, m_rcBackBuffer.m_X, m_rcBackBuffer.m_Y,
													m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height,
													m_glData.m_hParentWnd, NULL, GetModuleHandle(NULL), NULL);

					if(m_glData.m_hWnd)
					{
						m_glData.m_hDC = GetDC(m_glData.m_hWnd);
					}
					else
					{
						rc = IFX_E_SUBSYSTEM_FAILURE;
					}

					// Set up pixel format descriptor
					PIXELFORMATDESCRIPTOR pfd = {
						sizeof(PIXELFORMATDESCRIPTOR), //struct size
							1,							//version number
							PFD_DRAW_TO_WINDOW | //flags, draw to a window
							PFD_SUPPORT_OPENGL | //use OpenGL
							PFD_DOUBLEBUFFER,	//be double buffered
							PFD_TYPE_RGBA,		//RGBA mode window
							24,					//24 bits for color (8,8,8)
							0,0,0,				//RGB bits and shift sizes
							0,0,0,				//Don't care about them
							0,0,				//No alpha buffer info
							0,0,0,0,0,			//No accumulation buffer
							m_eDepthStencil == IFX_DEPTH_D16S0 ? 16 : 24,//depth buffer
							m_eDepthStencil == IFX_DEPTH_D24S8 ? 8 : 0,	//stencil buffer
							0,					//No auxilary buffers
							PFD_MAIN_PLANE,		//Layer type
							0,					//Reserved (must be 0)
							0,					//No layer mask
							0,					//No visible mask
							0					//No damage mask
					};

					if(IFXSUCCESS(rc))
					{
						m_glData.m_iPixelFormat = ChoosePixelFormat(m_glData.m_hDC, &pfd);
						if(!SetPixelFormat(m_glData.m_hDC, m_glData.m_iPixelFormat, &pfd))
						{
							rc = IFX_E_UNSUPPORTED;
						}
					}

					if(IFXSUCCESS(rc))
					{
//						m_glData.m_hRC = m_pOSOGL->wglCreateContext(m_glData.m_hDC);
						if(!m_pOSOGL->wglMakeCurrent(m_glData.m_hDC, m_glData.m_hRC))
						{
							m_pOSOGL->m_bValidOpenGL = FALSE;
							IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");
							rc = IFX_E_UNSUPPORTED;
						}
						else
						{
							m_pOSOGL->m_bValidOpenGL = TRUE;
							IFXLOGOGL("\nHave VALID Render Context\n\n");
						}

						ReleaseDC(m_glData.m_hWnd, m_glData.m_hDC);
					}
				}
			}

			if(IFXSUCCESS(rc))
			{
				SetWindowPos(m_glData.m_hWnd, HWND_NOTOPMOST, m_rcBackBuffer.m_X, m_rcBackBuffer.m_Y,
					m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height, SWP_NOZORDER);
			}

			m_bNeedClear = TRUE;
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

			if((m_bNeedClear || rClear.GetColorCleared()) && (!m_Window.GetDTS() && m_Window.GetTransparent()))
			{
				// Copy image data
				HBITMAP hBM = (HBITMAP)GetCurrentObject(m_glData.m_hMemDC, OBJ_BITMAP);
				BITMAP bm;
				GetObject(hBM, sizeof(BITMAP), (void*)&bm);

				IFXRect rcIClear = m_spDevice->GetVisibleViewport();

				U32 uBpp = bm.bmBitsPixel >> 3;
				U32 uY = bm.bmHeight - rcIClear.m_Height - m_rcBackBuffer.m_Y - rcIClear.m_Y;
				U8* pSrc = ((U8*)bm.bmBits) + uY*bm.bmWidthBytes +
					m_rcBackBuffer.m_X*uBpp + rcIClear.m_X*uBpp;


				U32 uSize = rcIClear.m_Width * rcIClear.m_Height * uBpp;

				if(uSize > m_uTempBufferSize)
				{
					IFXDELETE_ARRAY(m_pTempBuffer);
					m_pTempBuffer = new U8[uSize];

					m_uTempBufferSize = uSize;
				}

				rc = CopyImageData(pSrc, m_pTempBuffer, bm.bmWidthBytes, rcIClear.m_Width * uBpp,
								   rcIClear.m_Width, rcIClear.m_Height, uBpp, FALSE);

				// This whole deal with the identity transform matrices and
				// glRasterPos is to make sure that the glDrawPixels command
				// will draw the image at the correct location in the back buffer.
				m_pOGL->glMatrixMode(GL_MODELVIEW);
				m_pOGL->glPushMatrix();
				m_pOGL->glLoadIdentity();
				m_pOGL->glMatrixMode(GL_PROJECTION);
				m_pOGL->glPushMatrix();
				m_pOGL->glLoadIdentity();

				m_pOGL->glRasterPos2i(-1, -1);

				BOOL bDepth = m_pOGL->glIsEnabled(GL_DEPTH_TEST);
				m_pOGL->glDisable(GL_DEPTH_TEST);

				BOOL bTexEnabled[8] = {0};

				if(m_pOGL->MultiTexSupport())
				{
					I32 iNumTexUnits = 1;
					m_pOGL->glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &iNumTexUnits);
					for(I32 i = 0; i < iNumTexUnits; i++)
					{
						m_spDeviceOGL->SetActiveTexUnit(i);
						bTexEnabled[i] = m_pOGL->glIsEnabled(GL_TEXTURE_2D);
						m_pOGL->glDisable(GL_TEXTURE_2D);
					}
				}

				BOOL bAlphaTest = m_pOGL->glIsEnabled(GL_ALPHA_TEST);
				BOOL bBlend = m_pOGL->glIsEnabled(GL_BLEND);

				m_pOGL->glDisable(GL_ALPHA_TEST);
				m_pOGL->glDisable(GL_BLEND);

				m_pOGL->glDrawPixels(	rcIClear.m_Width, rcIClear.m_Height,
										GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_pTempBuffer );

				if(bDepth)
				{
					m_pOGL->glEnable(GL_DEPTH_TEST);
				}

				if(bAlphaTest)
				{
					m_pOGL->glEnable(GL_ALPHA_TEST);
				}

				if(bBlend)
				{
					m_pOGL->glEnable(GL_BLEND);
				}

				if(m_pOGL->MultiTexSupport())
				{
					I32 iNumTexUnits = 1;
					m_pOGL->glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &iNumTexUnits);
					for(I32 i = 0; i < iNumTexUnits; i++)
					{
						if(bTexEnabled[i])
						{
							m_pOGL->glEnable(GL_TEXTURE_2D);
						}
					}
				}

				m_pOGL->glRasterPos2i(-1, -1);

				m_pOGL->glPopMatrix();
				m_pOGL->glMatrixMode(GL_MODELVIEW);
				m_pOGL->glPopMatrix();
			}
		}
	}

	m_bNeedClear = FALSE;

	return rc;
}

IFXRESULT CIFXRenderOGL::SwapOS()
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(m_pOGL->m_bValidOpenGL, "(Error): Making OpenGL call on Invalid context!");
	if(m_Window.GetDTS())
	{
		m_pOSOGL->wglSwapLayerBuffers(m_glData.m_hDC, WGL_SWAP_MAIN_PLANE);
	}
	else
	{
		HBITMAP hBM = (HBITMAP)GetCurrentObject(m_glData.m_hMemDC, OBJ_BITMAP);
		BITMAP bm;
		GetObject(hBM, sizeof(BITMAP), (void*)&bm);

		U32 uBpp = bm.bmBitsPixel >> 3;

		U32 uSize = m_rcBackBuffer.m_Width * m_rcBackBuffer.m_Height * uBpp;

		if(uSize > m_uTempBufferSize)
		{
			IFXDELETE_ARRAY(m_pTempBuffer);
			m_pTempBuffer = new U8[uSize];

			m_uTempBufferSize = uSize;
		}

		m_pOGL->glReadBuffer(GL_BACK);
		m_pOGL->glReadPixels(	0, 0, m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height,
								GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_pTempBuffer );

		U32 uY = bm.bmHeight - m_rcBackBuffer.m_Height - m_rcBackBuffer.m_Y;

		U8* pDst = (U8*)bm.bmBits + uY*bm.bmWidthBytes + m_rcBackBuffer.m_X*uBpp;

		rc = CopyImageData(m_pTempBuffer, pDst, m_rcBackBuffer.m_Width * uBpp, bm.bmWidthBytes,
							m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height, uBpp, FALSE);
	}
/*
	m_pOSOGL->wglMakeCurrent(m_glData.m_hOldDC, m_glData.m_hOldRC);
	m_glData.m_hOldDC = 0;
	m_glData.m_hOldRC = 0;
*/
	return rc;
}

IFXRESULT CIFXRenderOGL::SetHWDTSOS(BOOL bDTS)
{
	if(m_Window.GetDTS() != bDTS)
	{
		if(m_pOSOGL)
		{
			m_pOSOGL->wglMakeCurrent(0, 0);
			m_pOSOGL->m_bValidOpenGL = FALSE;
			IFXLOGOGL("\n(Warning): Have INVALID Render Context\n\n");

			if(m_glData.m_hDC)
			{
				ReleaseDC(m_glData.m_hWnd, m_glData.m_hDC);
				m_glData.m_hDC = 0;
			}

			if(m_glData.m_hWnd)
			{
				DestroyWindow(m_glData.m_hWnd);
				m_glData.m_hWnd = 0;
			}
		}
	}

	m_Window.SetDTS(bDTS);

	return IFX_OK;
}
