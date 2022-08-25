//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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

#include "CIFXRenderServices.h"

#include "IFXRenderWindow.h"

//=======================================
// IFXRenderWindow Implementation
//=======================================
IFXRenderWindow::IFXRenderWindow()
{
	InitData();
}

IFXRenderWindow::~IFXRenderWindow()
{
	// EMPTY
}

void IFXRenderWindow::InitData()
{
	m_eAAMode = IFX_AA_DEFAULT;
	m_eAAEnabled = IFX_AA_DEFAULT;
	m_bDirectToScreen = TRUE;
	m_bTransparent = FALSE;
	m_rcWindow.Set(0, 0, 320, 240);
	m_pvWindow = 0;
//	m_pvDisplay = 0;
}

IFXRESULT IFXRenderWindow::SetAntiAliasingMode(IFXenum eAAMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_eAAMode != eAAMode)
	{
		switch(eAAMode)
		{
		case IFX_AA_DEFAULT:
		case IFX_AA_2X:
		case IFX_AA_3X:
		case IFX_AA_4X:
		case IFX_AA_4X_SW:
			m_eAAMode = eAAMode;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXRESULT IFXRenderWindow::SetAntiAliasingEnabled(IFXenum eAAEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_eAAEnabled != eAAEnabled)
	{
		switch(eAAEnabled)
		{
		case IFX_AA_DEFAULT:
		case IFX_AA_ENABLED:
		case IFX_AA_DISABLED:
			m_eAAEnabled = eAAEnabled;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXRESULT IFXRenderWindow::SetDTS(BOOL bDTS)
{
	m_bDirectToScreen = bDTS;

	return IFX_OK;
}

IFXRESULT IFXRenderWindow::SetTransparent(BOOL bTransparent)
{
	m_bTransparent = bTransparent;

	return IFX_OK;
}

IFXRESULT IFXRenderWindow::SetWindowSize(const IFXRect& rcWindow)
{
	IFXRESULT rc = IFX_OK;

	if(rcWindow.m_Width <= 0 || rcWindow.m_Height <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_rcWindow = rcWindow;
	}

	return rc;
}

IFXRESULT IFXRenderWindow::SetWindowPtr(IFXHANDLE pvWindow)
{
	if(pvWindow != m_pvWindow)
	{
		m_pvWindow = pvWindow;
	}
/*
	if(pvDisplay != m_pvDisplay)
	{
		m_pvDisplay = pvDisplay;
	}
*/
	return IFX_OK;
}

IFXenum IFXRenderWindow::GetAntiAliasingEnabled() const
{
	return m_eAAEnabled;
}

IFXenum IFXRenderWindow::GetAntiAliasingMode() const
{
	return m_eAAMode;
}

BOOL IFXRenderWindow::GetDTS() const
{
	return m_bDirectToScreen;
}

BOOL IFXRenderWindow::GetTransparent() const
{
	return m_bTransparent;
}

const IFXRect& IFXRenderWindow::GetWindowSize() const
{
	return m_rcWindow;
}

void* IFXRenderWindow::GetWindowPtr() const
{
	return m_pvWindow;
}

BOOL IFXAPI IFXRenderWindow::SetDirtyWindow(IFXRenderWindow& window)
{
	BOOL bDirty = FALSE;

	if(!(GetDTS() == window.GetDTS()) ||
		!(GetTransparent() == window.GetTransparent()) ||
		!(GetWindowSize() == window.GetWindowSize()) ||
		!(GetWindowPtr() == window.GetWindowPtr())) 
	{ 
		bDirty = TRUE; 
#ifdef RENDERING
		SetVisual(window.GetVisual());
		SetDrawable(window.GetDrawable());
#endif
		SetAntiAliasingEnabled(window.GetAntiAliasingEnabled());
		SetAntiAliasingMode(window.GetAntiAliasingMode());
		SetDTS(window.GetDTS());
		SetTransparent(window.GetTransparent());
		SetWindowSize(window.GetWindowSize());
		SetWindowPtr(window.GetWindowPtr());
	}

	return bDirty;
}



#ifdef RENDERING
XVisualInfo* IFXAPI IFXRenderWindow::GetVisual() const
{
	return m_pVisInfo;
}

Window IFXAPI IFXRenderWindow::GetDrawable() const
{
	return m_Window;
}

IFXRESULT IFXAPI IFXRenderWindow::SetVisual(XVisualInfo* visInfo)
{
	m_pVisInfo = visInfo;
	return IFX_OK;
}

IFXRESULT IFXAPI IFXRenderWindow::SetDrawable(const Window drawable)
{
	memcpy(&m_Window, &drawable, sizeof(Window));
	return IFX_OK;
}
#endif

/*
void* IFXRenderWindow::GetDisplay() const
{
	return m_pvDisplay;
}
*/
void IFXRenderWindow::GetWindowSizeVC(IFXRect& rcIn) const
{
/*	if(m_bDirectToScreen)
	{
		IFXHANDLE hWnd = (IFXHANDLE)m_pvWindow;
		rcIn = m_rcWindow;

//		IFXClientToScreen( hWnd, &rcIn.m_X, &rcIn.m_Y );
		Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay((Display*)hWnd));
		Window childWindow;
		int xx = rcIn.m_X, 
			yy = rcIn.m_Y;
		XTranslateCoordinates((Display*)hWnd, m_Window, rootWindow, xx, yy, (int*)&(rcIn.m_X), (int*)&(rcIn.m_Y), &childWindow);

		IFXRect rcParentRect;
		IFXGetClientRect( hWnd, &rcParentRect );
		IFXClientToScreen( hWnd, &rcParentRect.m_X, &rcParentRect.m_Y );

		I32 x = rcParentRect.m_X+rcParentRect.m_Width;
		I32 y = rcParentRect.m_Y+rcParentRect.m_Height;
		IFXClientToScreen( hWnd, &x, &y );
		rcParentRect.m_Width = x - rcParentRect.m_X;
		rcParentRect.m_Height = y - rcParentRect.m_Y;

		rcParentRect.GetIntersection(rcIn);
	}
	else*/
	{
		rcIn = m_rcWindow;
	}

	return;
}

