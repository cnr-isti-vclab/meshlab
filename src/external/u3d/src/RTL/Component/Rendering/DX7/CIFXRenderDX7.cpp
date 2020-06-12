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
// CIFXRenderDX7.cpp

#include "IFXRenderPCHDX7.h"
#include "CIFXRenderDX7.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderDX7Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderDX7* pPtr = new CIFXRenderDX7;
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

U32 CIFXRenderDX7::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDX7::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDX7::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRender)
		{
			*(IFXRender**)ppInterface = (IFXRender*) this;
		}
		else
		if (interfaceId == CID_IFXRenderDX7)
		{
			*(CIFXRenderDX7**)ppInterface = (CIFXRenderDX7*) this;
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
// Public Methods
//===========================
CIFXRenderDX7::CIFXRenderDX7()
{
	m_refCount = 0;
	m_rcBackBuffer.Set( 0, 0, 0, 0 );
}

CIFXRenderDX7::~CIFXRenderDX7()
{
	ReleaseVidMem();
	IFXRELEASE(m_pClipper);
	if(m_spDevice.IsValid())
	{
		m_spDeviceDX7->UnregisterWindow(this);
	}
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}
}

IFXRESULT CIFXRenderDX7::Construct()
{
	IFXRESULT rc = CIFXRender::Construct();

	InitData();

	if(IFXSUCCESS(rc))
	{
		m_idRenderDevice = CID_IFXRenderDeviceDX7;
	}

	return rc;
}

void CIFXRenderDX7::InitData()
{
	m_pBackBuffer = 0;
	m_pDepthBuffer = 0;
	m_pAABuffer = 0;
	m_pClipper = 0;
	m_hWnd = 0;
	m_hDC = 0;
	m_bNeedClear = TRUE;
}

IFXRESULT CIFXRenderDX7::Initialize(U32 uMonitorNum)
{
	IFXRESULT rc = CIFXRender::Initialize(uMonitorNum);

	IFXRUN(rc, m_spDevice.QI(m_spDeviceDX7, CID_IFXRenderDeviceDX7));

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLights[i].QI(m_pspLightsDX7[i], CID_IFXDeviceLightDX7);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc |= m_pspTexUnits[i].QI(m_pspTexUnitsDX7[i], CID_IFXDeviceTexUnitDX7);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 uNumMonitors = 0;
		const IFXMonitor* pMons = m_pServices->GetMonitors(uNumMonitors);
		m_rcDevice = pMons[uMonitorNum].m_rcScreen;
	}

	if(IFXSUCCESS(rc))
	{
		m_spDeviceDX7->RegisterWindow(this);

		HRESULT hr = m_spDeviceDX7->GetDD()->CreateClipper(0, &m_pClipper, 0);
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "CreateClipper Failed", FALSE, TRUE);
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		m_iInitialized = IFX_OK;
	}

	return rc;
}

IFXRESULT CIFXRenderDX7::SetHWWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid() && m_spDeviceDX7->GetD3DDevice())
	{
		if(m_Window.GetDTS())
		{
			rc = SetDTSWindow(bDirty);
		}
		else
		{
			rc = SetNonDTSWindow(bDirty);
		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		CalcVisibleWindow();
	}

	return rc;
}

IFXRESULT CIFXRenderDX7::MoveDTSWindow(const IFXRect& rcWindow)
{
	IFXRESULT rc = IFX_OK;

	if(m_Window.GetDTS())
	{
		if(m_hWnd)
		{
			SetWindowPos(m_hWnd, HWND_NOTOPMOST, rcWindow.m_X, rcWindow.m_Y,
				rcWindow.m_Width, rcWindow.m_Height, SWP_NOZORDER);
		}
		else
		{
			m_hWnd = CreateWindow(	LPCWSTR(IFX_RENDER_WINDOW_CLASS),
									L"",
									WS_CHILD | WS_VISIBLE,
									rcWindow.m_X,
									rcWindow.m_Y,
									rcWindow.m_Width,
									rcWindow.m_Height,
									(HWND)m_Window.GetWindowPtr(),
									NULL,
									GetModuleHandle(NULL),
									NULL							);

			m_pClipper->SetHWnd(0, m_hWnd);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDX7::SetDTSWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = S_OK;

	RECT rcBB;
	GetClientRect((HWND)m_Window.GetWindowPtr(), &rcBB);
	IFXRect rcMon = m_rcDevice;
	ScreenToClient((HWND)m_Window.GetWindowPtr(), (LPPOINT)&rcMon.m_X);

	IFXRect rcBackSave = m_rcBackBuffer;
	m_rcBackBuffer.Set( rcBB.left, rcBB.top, rcBB.right - rcBB.left, rcBB.bottom - rcBB.top );
	rc = SizeBackBuffer(rcMon);

	if(!(rcBackSave == m_rcBackBuffer))
	{
		rc = MoveDTSWindow(m_rcBackBuffer);
	}

	if(!(rcBackSave.m_Width == m_rcBackBuffer.m_Width &&
		 rcBackSave.m_Height == m_rcBackBuffer.m_Height)
		|| !m_pBackBuffer || bDirty)
	{
		if(IFXSUCCESS(rc))
		{
			rc = CreateBackBuffer();
		}

		IFXRUN(rc, MakeHWCurrent());
	}

	return rc;
}

IFXRESULT CIFXRenderDX7::SetNonDTSWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = S_OK;

	// Destroy the DTS window
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}

	IFXRect rcBackSave = m_rcBackBuffer;

	m_hDC = (HDC)m_Window.GetWindowPtr();

	HBITMAP hBM = (HBITMAP)GetCurrentObject(m_hDC, OBJ_BITMAP);
	BITMAP bitmap;
	GetObject(hBM, sizeof(BITMAP), (void*)&bitmap);

	m_rcBackBuffer.Set(0, 0, bitmap.bmWidth, bitmap.bmHeight);

	rc = SizeBackBuffer(m_Window.GetWindowSize());

	if(!(rcBackSave.m_Width == m_rcBackBuffer.m_Width &&
		 rcBackSave.m_Height == m_rcBackBuffer.m_Height)
		|| !m_pBackBuffer || bDirty)
	{
		if(IFXSUCCESS(rc))
		{
			rc = CreateBackBuffer();
		}

		IFXRUN(rc, MakeHWCurrent());
	}

	return rc;
}


IFXRESULT CIFXRenderDX7::Swap()
{
	IFXRESULT rc = IFX_OK;

	if(m_spDeviceDX7.IsValid() && m_spDeviceDX7->GetD3DDevice())
	{
		rc = m_spDeviceDX7->EndScene();
		if(IFXSUCCESS(rc))
		{
			if(m_pBackBuffer)
			{
				HRESULT hr = DD_OK;
				RECT rcLocal = {0, 0, m_rcVisibleWindow.m_Width, m_rcVisibleWindow.m_Height };
				RECT rcScreen = rcLocal;
				ClientToScreen(m_hWnd, (LPPOINT)&rcScreen.left);
				ClientToScreen(m_hWnd, (LPPOINT)&rcScreen.right);
				// Need to move from screen coordinates to device coordinates
				rcScreen.left -= m_rcDevice.m_X;
				rcScreen.right -= m_rcDevice.m_X;
				rcScreen.top -= m_rcDevice.m_Y;
				rcScreen.bottom -= m_rcDevice.m_Y;

				// Wait for VSync if needed
				if(m_bVSyncEnabled)
				{
					hr = m_spDeviceDX7->GetDD()->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
				}

				if(!(m_eAAMode == IFX_AA_4X_SW))
				{
					if(m_Window.GetDTS())
					{
						hr = m_spDeviceDX7->GetPrimarySurface()->Blt(&rcScreen, m_pBackBuffer, &rcLocal, DDBLT_WAIT, 0);
					}
					else
					{
						HDC hBBDC = 0;
						hr = m_pBackBuffer->GetDC(&hBBDC);

						if(SUCCEEDED(hr))
						{
							BitBlt(m_hDC, m_rcBackBuffer.m_X, m_rcBackBuffer.m_Y,
									m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height,
									hBBDC, 0, 0, SRCCOPY);
						}

						m_pBackBuffer->ReleaseDC(hBBDC);
					}
				}
				else
					hr = SwapAA(rcScreen, rcLocal);

				if(FAILED(hr))
				{
					rc = m_spDeviceDX7->ResetDevice();
					return IFX_E_NEED_RESET;
				}
			}
		}
		else
		{
			rc = m_spDeviceDX7->ResetDevice();
			return IFX_E_NEED_RESET;
		}
	}
	return IFX_OK;
}

IFXenum	CIFXRenderDX7::GetIFXRenderFormat(DDPIXELFORMAT ddpf)
{
	IFXenum eFormat = IFX_RGBA_8880;

	if(ddpf.dwRGBBitCount == 16)
	{
		if(ddpf.dwRGBAlphaBitMask)
		{
			eFormat = IFX_RGBA_5551;
		}
		else if(ddpf.dwGBitMask == 0x7e0)
		{
			eFormat = IFX_RGBA_5650;
		}
		else
			eFormat = IFX_RGBA_5550;
	}

	return eFormat;
}

HRESULT CIFXRenderDX7::SwapAA(RECT& rcScreen, RECT& rcLocal)
{
	HRESULT hr = D3D_OK;

	hr = m_pAABuffer->Blt(0, m_pBackBuffer, 0, DDBLT_WAIT, 0);


	DDSURFACEDESC2 srcSurf;

	if (SUCCEEDED(hr))
	{
		memset(&srcSurf, 0, sizeof(srcSurf));
		srcSurf.dwSize = sizeof(srcSurf);
		hr = m_pAABuffer->Lock(0, &srcSurf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL);
	}

	if(SUCCEEDED(hr))
	{
		IFXRect rcBuffer;
		rcBuffer.Set( rcLocal.left, rcLocal.top, rcLocal.right - rcLocal.left, rcLocal.bottom - rcLocal.top );
//*
		IFXAAFilterImage(	(U8*)srcSurf.lpSurface,
							(U8*)srcSurf.lpSurface,
							srcSurf.lPitch,
							srcSurf.lPitch,
							rcBuffer.m_Width,
							rcBuffer.m_Height,
							GetIFXRenderFormat(srcSurf.ddpfPixelFormat));
/**/
		hr = m_pAABuffer->Unlock(0);
	}

	if(m_Window.GetDTS())
	{
		if(SUCCEEDED(hr))
		{
			hr = m_spDeviceDX7->GetPrimarySurface()->Blt(	&rcScreen, m_pAABuffer,
															&rcLocal, DDBLT_WAIT, 0);
		}
	}
	else
	{
		HDC hBBDC = 0;
		hr = m_pAABuffer->GetDC(&hBBDC);

		if(SUCCEEDED(hr))
		{
			BitBlt(m_hDC, m_rcBackBuffer.m_X, m_rcBackBuffer.m_Y,
					m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height,
					hBBDC, 0, 0, SRCCOPY);
		}

		m_pAABuffer->ReleaseDC(hBBDC);
	}

	if (FAILED(hr))
		IFXASSERTBOXEX(SUCCEEDED(hr), "SwapAA Failed", FALSE, TRUE);

	return hr;
}

IFXRESULT CIFXRenderDX7::CreateBackBuffer()
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	m_bNeedClear = TRUE;

	IFXRELEASE(m_pDepthBuffer);
	IFXRELEASE(m_pBackBuffer);
	IFXRELEASE(m_pAABuffer);

	DDSURFACEDESC2 ddSurfDesc;
	memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
	ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);

	U32 uWidth = m_rcBackBuffer.m_Width;
	U32 uHeight = m_rcBackBuffer.m_Height;

	if(m_eAAMode == IFX_AA_4X_SW)
	{
		uWidth  *= IFX_AA_SS_FACTOR;
		uHeight *= IFX_AA_SS_FACTOR;
	}

	hr = m_spDeviceDX7->GetPrimarySurface()->GetSurfaceDesc(&ddSurfDesc);

	if(SUCCEEDED(hr))
	{
		ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
		ddSurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
		ddSurfDesc.dwHeight = uHeight;
		ddSurfDesc.dwWidth = uWidth;
		ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;

		hr = m_spDeviceDX7->GetDD()->CreateSurface(&ddSurfDesc, &m_pBackBuffer, 0);
	}

	if(SUCCEEDED(hr))
	{
		ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
		ddSurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
		ddSurfDesc.dwHeight = uHeight;
		ddSurfDesc.dwWidth = uWidth;
		ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		m_spDeviceDX7->GetDepthFormat(m_eDepthStencil, ddSurfDesc.ddpfPixelFormat);
		hr = m_spDeviceDX7->GetDD()->CreateSurface(&ddSurfDesc, &m_pDepthBuffer, 0);

		if(SUCCEEDED(hr))
		{
			hr = m_pBackBuffer->AddAttachedSurface(m_pDepthBuffer);
		}

		if(SUCCEEDED(hr))
		{
			hr = m_spDeviceDX7->GetD3DDevice()->SetRenderTarget(m_pBackBuffer, 0);
		}

		if(FAILED(hr))
		{
			IFXRELEASE(m_pDepthBuffer);
			m_eDepthStencil = IFX_DEPTH_D16S0;
			m_spDeviceDX7->GetDepthFormat(m_eDepthStencil, ddSurfDesc.ddpfPixelFormat);
			hr = m_spDeviceDX7->GetDD()->CreateSurface(&ddSurfDesc, &m_pDepthBuffer, 0);
			if(SUCCEEDED(hr))
			{
				hr = m_pBackBuffer->AddAttachedSurface(m_pDepthBuffer);
			}

			if(SUCCEEDED(hr))
			{
				hr = m_spDeviceDX7->GetD3DDevice()->SetRenderTarget(m_pBackBuffer, 0);
			}

		}
	}

	if(SUCCEEDED(hr) && m_eAAMode == IFX_AA_4X_SW)
	{
		memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
		ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
		hr = m_spDeviceDX7->GetPrimarySurface()->GetSurfaceDesc(&ddSurfDesc);
		if(SUCCEEDED(hr))
		{
			ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
			ddSurfDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
			ddSurfDesc.dwHeight = uHeight;
			ddSurfDesc.dwWidth = uWidth;
			ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;

			hr = m_spDeviceDX7->GetDD()->CreateSurface(&ddSurfDesc, &m_pAABuffer, 0);
		}

	}

	if(SUCCEEDED(hr))
	{
		m_spDeviceDX7->SetSWAA(m_eAAMode == IFX_AA_4X_SW);
		hr = m_spDeviceDX7->GetPrimarySurface()->SetClipper(m_pClipper);
	}

	if(FAILED(hr))
	{
		rc = IFX_E_SUBSYSTEM_FAILURE;
	}

	return rc;
}

IFXRESULT CIFXRenderDX7::ClearHW(const IFXRenderClear& rClear)
{
	HRESULT hr = D3D_OK;
	IFXRESULT rc = IFX_OK;

	if(m_spDeviceDX7.IsValid() && m_spDeviceDX7->GetD3DDevice())
	{
		U32 uBuffers = 0;
		if(m_bNeedClear)
		{
			uBuffers = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
			if(m_eDepthStencil == IFX_DEPTH_D24S8)
			{
				uBuffers |= D3DCLEAR_STENCIL;
			}
		}
		if(rClear.GetColorCleared() && !(!m_Window.GetDTS() && m_Window.GetTransparent()))
		{
			uBuffers |= D3DCLEAR_TARGET;
		}
		if(rClear.GetDepthCleared())
		{
			uBuffers |= D3DCLEAR_ZBUFFER;
		}
		if(rClear.GetStencilCleared() && m_eDepthStencil == IFX_DEPTH_D24S8)
		{
			uBuffers |= D3DCLEAR_STENCIL;
		}

		if ( uBuffers )
		{
			U32 uColor = RGBA_MAKE(	(U32)(rClear.GetColorValue().XC() * 255),
									(U32)(rClear.GetColorValue().YC() * 255),
									(U32)(rClear.GetColorValue().ZC() * 255),
									255
									);
			IFXRect rcIClear = m_spDevice->GetVisibleViewport();
			RECT rcClear;
			rcClear.left = rcIClear.m_X;
			rcClear.top = rcIClear.m_Y;
			rcClear.right = rcIClear.m_X + rcIClear.m_Width;
			rcClear.bottom = rcIClear.m_Y + rcIClear.m_Height;
			if(m_eAAMode == IFX_AA_4X_SW)
			{
				rcClear.top *= IFX_AA_SS_FACTOR;
				rcClear.bottom *= IFX_AA_SS_FACTOR;
				rcClear.right *= IFX_AA_SS_FACTOR;
				rcClear.left *= IFX_AA_SS_FACTOR;
			}
			hr = m_spDeviceDX7->GetD3DDevice()->Clear(	1,
														(D3DRECT*)&(rcClear),
														uBuffers, uColor,
														rClear.GetDepthValue(),
														rClear.GetStencilValue() );
			if(SUCCEEDED(hr))
			{
				if((m_bNeedClear || rClear.GetColorCleared()) && (!m_Window.GetDTS() && m_Window.GetTransparent()))
				{
					DDSURFACEDESC2 ddSurfDesc;
					memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
					ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
					HDC hBBDC = 0;
					hr = m_pBackBuffer->GetDC(&hBBDC);

					if(SUCCEEDED(hr))
					{
						if(m_eAAMode != IFX_AA_4X_SW)
						{
							BitBlt( hBBDC,
									rcIClear.m_X,
									rcIClear.m_Y,
									rcIClear.m_Width,
									rcIClear.m_Height,
									m_hDC,
									m_rcBackBuffer.m_X + rcIClear.m_X,
									m_rcBackBuffer.m_Y + rcIClear.m_Y,
									SRCCOPY);
						}
						else
						{
							StretchBlt( hBBDC,
										rcIClear.m_X * IFX_AA_SS_FACTOR,
										rcIClear.m_Y * IFX_AA_SS_FACTOR,
										rcIClear.m_Width * IFX_AA_SS_FACTOR,
										rcIClear.m_Height * IFX_AA_SS_FACTOR,
										m_hDC,
										m_rcBackBuffer.m_X + rcIClear.m_X,
										m_rcBackBuffer.m_Y + rcIClear.m_Y,
										rcIClear.m_Width,
										rcIClear.m_Height,
										SRCCOPY);
						}
					}

					m_pBackBuffer->ReleaseDC(hBBDC);
				}
			}
		}
		if(SUCCEEDED(hr))
		{
			rc = m_spDeviceDX7->BeginScene();
		}
		else
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		m_bNeedClear = FALSE;
	}
	return rc;
}

IFXRESULT CIFXRenderDX7::MakeHWCurrent()
{
	IFXRESULT rc = IFX_OK;

	if(m_pBackBuffer)
	{
		rc = m_spDeviceDX7->EndScene();

		if(IFXFAILURE(rc))
		{
			rc = m_spDeviceDX7->ResetDevice();
			return IFX_E_NEED_RESET;
		}

		if(IFXSUCCESS(rc))
		{
			rc = m_spDeviceDX7->GetD3DDevice()->SetRenderTarget(m_pBackBuffer, 0);
		}

		if(IFXSUCCESS(rc))
		{
			rc = m_spDeviceDX7->GetPrimarySurface()->SetClipper(m_pClipper);
		}
	}

	return rc;
}

IFXRESULT CIFXRenderDX7::SetHWAntiAliasingMode(IFXenum eAAMode)
{
	IFXRESULT rc = IFX_OK;

	if(eAAMode == IFX_AA_2X)
	{
		if(m_spDeviceDX7.IsValid())
		{
			rc = m_spDeviceDX7->SetRS(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);
		}
	}
	else if(m_spDeviceDX7.IsValid())
	{
		rc = m_spDeviceDX7->SetRS(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
	}

	m_spDeviceDX7->SetSWAA(eAAMode == IFX_AA_4X_SW);

	m_eAAMode = eAAMode;

	return rc;
}

IFXRESULT CIFXRenderDX7::ReleaseVidMem()
{
	IFXRELEASE(m_pAABuffer);
	IFXRELEASE(m_pDepthBuffer);
	IFXRELEASE(m_pBackBuffer);

	return IFX_OK;
}



IFXRESULT CIFXRenderDX7::CreateTexture(CIFXDeviceTexturePtr& rTexture)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDeviceDX7.IsValid())
	{
		CIFXDeviceTextureDX7Ptr rTexDX7;
		rc = rTexDX7.Create(CID_IFXDeviceTextureDX7, CID_IFXDeviceTextureDX7);
		if(IFXSUCCESS(rc))
		{
			rc = rTexDX7->Initialize(m_spDeviceDX7);
		}
		if(IFXSUCCESS(rc))
		{
			rc = rTexDX7.QI(rTexture, CID_IFXDeviceTexture);
		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

