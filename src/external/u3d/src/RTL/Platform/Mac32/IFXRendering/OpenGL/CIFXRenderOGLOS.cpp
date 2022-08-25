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
//	Public methods
//***************************************************************************

IFXRESULT CIFXRenderDeviceOGL::GetSupported(U32 uDeviceNum, BOOL& bSupported)
{
	///@todo: develop for MacOSX
	
	IFXRESULT rc = IFX_OK;
	
	bSupported = TRUE;
	
	return rc;
}

IFXRESULT CIFXRenderOGL::SetVSyncEnabledOS(BOOL bEnabled)
{
	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::DestroyOS()
{
	if(m_glData.m_hAGL)
	{
		m_pOSOGL->aglDestroyPixelFormat( m_glData.m_pixFmt );
		m_pOSOGL->aglSetDrawable(m_glData.m_hAGL, nil);
		m_pOSOGL->aglSetCurrentContext(nil);
		m_pOSOGL->m_bValidOpenGL = FALSE;
		m_pOSOGL->aglDestroyContext(m_glData.m_hAGL);
		m_glData.m_hAGL = 0;
	}
	
	if(m_glData.m_window)
	{
		DisposeWindow(m_glData.m_window);
		m_glData.m_window = 0;
		m_glData.m_port = 0;
	}
	  
	IFXRELEASE(m_pOSOGL);

	return IFX_OK;
}

IFXRESULT CIFXRenderDeviceOGL::InitContextOS()
{
	IFXRESULT rc = IFX_OK;

	m_glData.m_lPixelsize = 24;//IFXGetColorBufferDepth();
	  
	GLint attributes[] = { AGL_ACCELERATED, 
						   AGL_RGBA, 
						   AGL_DOUBLEBUFFER, 
						   AGL_PIXEL_SIZE, m_glData.m_lPixelsize,
						   AGL_DEPTH_SIZE, 24,
						   AGL_NONE	};
	  
	if(m_glData.m_pixFmt)
	{
		m_pOSOGL->aglDestroyPixelFormat(m_glData.m_pixFmt);
	}
	  
	m_glData.m_pixFmt = m_pOSOGL->aglChoosePixelFormat(NULL, 0, attributes );
	if(!m_glData.m_pixFmt)
	{
		rc = IFX_E_SUBSYSTEM_FAILURE;
	}
	  
	if(IFXSUCCESS(rc))
	{
		GLboolean   bSuccess = GL_FALSE;
		GLint		iAccelerated = GL_FALSE;
	    
		bSuccess = m_pOSOGL->aglDescribePixelFormat( m_glData.m_pixFmt, 
													AGL_ACCELERATED, 
													&iAccelerated );
	    
		if( !bSuccess || !iAccelerated )
		{
			m_pOSOGL->aglDestroyPixelFormat( m_glData.m_pixFmt );
			m_glData.m_pixFmt = 0;
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	  
	if(IFXSUCCESS(rc))
	{
		m_glData.m_hAGL = m_pOSOGL->aglCreateContext( m_glData.m_pixFmt, NULL );
		
		if(!m_glData.m_hAGL)
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	  
	if(IFXSUCCESS(rc))
	{
		m_glData.m_port = GetWindowPort(m_glData.m_window);
		m_glData.m_hOldAGL = m_pOSOGL->aglGetCurrentContext();
		m_pOSOGL->aglSetCurrentContext(m_glData.m_hAGL);
		m_pOGL->m_bValidOpenGL = TRUE;
		m_pOSOGL->aglSetDrawable( m_glData.m_hAGL, m_glData.m_port );
		m_pOSOGL->aglEnable( m_glData.m_hAGL, AGL_BUFFER_RECT );
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::InitWindowOS()
{
	IFXRESULT rc = IFX_OK;

	if (NULL == m_pOSOGL)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc) && NULL == m_glData.m_window)
	{
		Rect viewRect = {0, 0, 1, 1};
		WindowPtr window = NULL;

		CreateNewWindow( kDocumentWindowClass, kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute, &viewRect, &window );

		ShowWindow( window );

		m_glData.m_window = window;
		m_glData.m_port = GetWindowPort(window);
	}

	return rc;
}

IFXRESULT CIFXRenderDeviceOGL::MakeDeviceCurrent()
{
	if(m_pOSOGL && m_pOSOGL->aglSetCurrentContext)
	{
		m_pOSOGL->aglSetCurrentContext(m_glData.m_hAGL);
		m_pOGL->m_bValidOpenGL = TRUE;
	}
	  
	if(m_pOSOGL && m_pOSOGL->aglSetDrawable && m_glData.m_hAGL)
	{
		m_pOSOGL->aglSetDrawable(m_glData.m_hAGL, nil);
		m_pOGL->m_bValidOpenGL = TRUE; //FALSE;
	}
	  
	return IFX_OK;
}

IFXRESULT CIFXRenderOGL::DestroyOS()
{
	// We need to make current before we call any openg methods
	// Unfortunately, the window has been destroyed already, so
	// doing a regular make current on our own window won't work.
	// So, we do a make current on the window that is owned by the
	// device.  This will allow the textures to be deleted using
	// a valid rendering context.

	if(m_glData.m_hAGL)
	{
		if(m_pOSOGL)
		{
			m_spDeviceOGL->MakeDeviceCurrent();
			m_glData.m_hAGL = 0;
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

		m_glData.m_pixFmt = glData.m_pixFmt;
	    
		m_glData.m_hAGL = glData.m_hAGL;
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::MakeCurrentOS()
{
	IFXRESULT rc = IFX_OK;
  
	if(m_pOSOGL && m_glData.m_port && m_glData.m_hAGL)
	{
		rc = ResizeBufferOS();
	    
		if(IFXSUCCESS(rc))
		CalcVisibleWindow();
	}

	return rc;
}

IFXRESULT CIFXRenderOGL::SetHWWindowOS(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;
  
	if(m_spDevice.IsValid() && m_Window.GetWindowPtr() && bDirty)
	{
		m_glData.m_window = (WindowPtr)m_Window.GetWindowPtr();
		m_glData.m_port = GetWindowPort(m_glData.m_window);
		m_glData.m_lPixelsize = 24;//IFXGetColorBufferDepth();
	    
		rc = ResizeBufferOS();
	}
	  
	return rc;
}

IFXRESULT CIFXRenderOGL::ResizeBufferOS()
{
	IFXRESULT rc = IFX_OK;
  
	m_glData.m_hOldAGL = m_pOSOGL->aglGetCurrentContext();
	    
	m_glData.m_oldPort = m_pOSOGL->aglGetDrawable(m_glData.m_hAGL);
	    
	if(m_glData.m_hAGL != m_glData.m_hOldAGL)
		m_pOSOGL->aglSetCurrentContext(m_glData.m_hAGL);
	      
	m_pOGL->m_bValidOpenGL = TRUE;
	      
	if(m_glData.m_port != m_glData.m_oldPort)
		m_pOSOGL->aglSetDrawable( m_glData.m_hAGL, m_glData.m_port );  

	///@todo: needs to be developed for MacOSX
	//
	//Rect rcWindowSize;
	//GetPortBounds(m_glData.m_port, &rcWindowSize);
	//    
	//IFXRect rcIFXWindow = m_Window.GetWindowSize();
	//
	//Rect rcWindow;	
	//rcWindow.left = rcIFXWindow.m_X;
	//rcWindow.right = rcIFXWindow.m_X + rcIFXWindow.m_Width;
	//rcWindow.top = rcIFXWindow.m_Y;
	//rcWindow.bottom = rcIFXWindow.m_Y + rcIFXWindow.m_Height;
	//    
	//GetPortClipRegion(m_glData.m_port, m_glData.m_clipRgn);	
	//GetRegionBounds(m_glData.m_clipRgn, &rcClipRgn);
	//
	//Rect rcTemp;
	//SectRect(&rcWindow, &rcClipRgn, &rcTemp);
	//    
	//Rect rcBuffer;
	//SectRect(&rcWindowSize, &rcTemp, &rcBuffer);
	//
	//GetWindowPortBounds(m_glData.m_window, &rcClipRgn);
	//
	//m_glData.m_pBufferRect[0] = rcBuffer.left - rcWindowSize.left;
	//m_glData.m_pBufferRect[1] = rcWindowSize.bottom - rcBuffer.bottom;
	//m_glData.m_pBufferRect[2] = rcBuffer.right - rcBuffer.left; 
	//m_glData.m_pBufferRect[3] = rcBuffer.bottom - rcBuffer.top;
	//
	//m_pOSOGL->aglSetInteger( m_glData.m_hAGL, AGL_BUFFER_RECT, m_glData.m_pBufferRect );	
	//
	//m_pOSOGL->aglUpdateContext( m_glData.m_hAGL );
	//
	//m_rcBackBuffer.SetMacRect(rcBuffer);

	short w, h;
	Rect rectPort;
	int kBufferRectInset = 10;
  
	GetWindowPortBounds(m_glData.m_window, &rectPort);
	w = rectPort.right - rectPort.left;
	h = rectPort.bottom - rectPort.top;

	w -= kBufferRectInset * 2;
    h -= kBufferRectInset * 2;

	m_glData.m_pBufferRect[0] = kBufferRectInset;
	m_glData.m_pBufferRect[1] = kBufferRectInset;
	m_glData.m_pBufferRect[2] = w; 
	m_glData.m_pBufferRect[3] = h;

	m_pOSOGL->aglSetInteger( m_glData.m_hAGL, AGL_BUFFER_RECT, m_glData.m_pBufferRect );	

	m_pOSOGL->aglUpdateContext( m_glData.m_hAGL );

	m_rcBackBuffer.m_X = m_glData.m_pBufferRect[0];
	m_rcBackBuffer.m_Y = m_glData.m_pBufferRect[1];
	m_rcBackBuffer.m_Width = m_glData.m_pBufferRect[2];
	m_rcBackBuffer.m_Height = m_glData.m_pBufferRect[3];

	return rc;
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
	m_pOSOGL->aglSwapBuffers(m_glData.m_hAGL);
  
	return IFX_OK;
}

IFXRESULT CIFXRenderOGL::SetHWDTSOS(BOOL bDTS)
{
	m_Window.SetDTS(bDTS);
  
	return IFX_OK;
}

