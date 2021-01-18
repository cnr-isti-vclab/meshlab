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

// #include <Carbon/Carbon.h>

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
  m_pvWindow = NULL;
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
  if(pvWindow)
  {
    m_pvWindow = pvWindow;
    return IFX_OK;
  }

  return IFX_E_INVALID_POINTER;
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
		SetAntiAliasingEnabled(window.GetAntiAliasingEnabled());
		SetAntiAliasingMode(window.GetAntiAliasingMode());
		SetDTS(window.GetDTS());
		SetTransparent(window.GetTransparent());
		SetWindowSize(window.GetWindowSize());
		SetWindowPtr(window.GetWindowPtr());
  }

  return bDirty;
}

void IFXRenderWindow::GetWindowSizeVC(IFXRect& rcIn) const
{
  if(m_bDirectToScreen)
  {
    ///@todo: needs to be developed for MacOSX
	//
	//Rect rcScreen;
	//
    //rcScreen.left = m_rcWindow.m_X;
    //rcScreen.top = m_rcWindow.m_Y;
    //rcScreen.right = m_rcWindow.Right();
    //rcScreen.bottom = m_rcWindow.Bottom();
	//
    //LocalToGlobal((Point*)&rcScreen.top);
    //LocalToGlobal((Point*)&rcScreen.bottom);
	//
    //rcIn.Set( rcScreen.left, rcScreen.top,
    //      rcScreen.right - rcScreen.left,
    //      rcScreen.bottom - rcScreen.top );

    rcIn = m_rcWindow;
  }
  else
  {
    rcIn = m_rcWindow;
  }
}
