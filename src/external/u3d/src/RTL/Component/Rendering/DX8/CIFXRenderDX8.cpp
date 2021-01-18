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
// CIFXRenderDX8.cpp

#include "IFXRenderPCHDX8.h"
#include "CIFXRenderDX8.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderDX8Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderDX8* pPtr = new CIFXRenderDX8;
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

U32 CIFXRenderDX8::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderDX8::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderDX8::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRender)
		{
			*(IFXRender**)ppInterface = (IFXRender*) this;
		}
		else
		if (interfaceId == CID_IFXRenderDX8)
		{
			*(CIFXRenderDX8**)ppInterface = (CIFXRenderDX8*) this;
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
CIFXRenderDX8::CIFXRenderDX8()
{
	m_refCount = 0;
	m_rcBackBuffer.Set( 0, 0, 0, 0 );
}

CIFXRenderDX8::~CIFXRenderDX8()
{
	ReleaseVidMem();
	if(m_spDeviceDX8.IsValid())
	{
		m_spDeviceDX8->UnregisterWindow(this);
	}
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}
}

IFXRESULT CIFXRenderDX8::Construct()
{
	IFXRESULT rc = CIFXRender::Construct();

	InitData();

	if(IFXSUCCESS(rc))
	{
		m_idRenderDevice = CID_IFXRenderDeviceDX8;
	}

	return rc;
}

void CIFXRenderDX8::InitData()
{
	m_pSwapChain = 0;
	m_pBackBuffer = 0;
	m_pDepthBuffer = 0;
	m_pSysMemBBCopy = 0;
	m_hWnd = 0;
	m_hDC = 0;
	m_bNeedClear = TRUE;
}

IFXRESULT CIFXRenderDX8::ReleaseVidMem()
{
	IFXRELEASE(m_pSwapChain);
	IFXRELEASE(m_pSysMemBBCopy);
	IFXRELEASE(m_pBackBuffer);
	IFXRELEASE(m_pDepthBuffer);

	return IFX_OK;
}

IFXRESULT CIFXRenderDX8::Initialize(U32 uMonitorNum)
{
	IFXRESULT rc = CIFXRender::Initialize(uMonitorNum);

	IFXRUN(rc, m_spDevice.QI(m_spDeviceDX8, CID_IFXRenderDeviceDX8));

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_LIGHTS; i++)
		{
			rc = m_pspLights[i].QI(m_pspLightsDX8[i], CID_IFXDeviceLightDX8);
		}
	}

	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; IFXSUCCESS(rc) && i < IFX_MAX_TEXUNITS; i++)
		{
			rc |= m_pspTexUnits[i].QI(m_pspTexUnitsDX8[i], CID_IFXDeviceTexUnitDX8);
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
		m_spDeviceDX8->RegisterWindow(this);
	}

	if(IFXSUCCESS(rc))
	{
		m_iInitialized = IFX_OK;
	}

	SetHWAntiAliasingMode(m_eAAMode);

	return rc;
}

IFXRESULT CIFXRenderDX8::SetHWAntiAliasingMode(IFXenum eAAMode)
{
	m_eAAMode = eAAMode;

	IFXRenderCaps Caps;
	m_spDevice->GetCaps(Caps);

	m_uAAMode = D3DMULTISAMPLE_NONE;
	switch(eAAMode)
	{
	case IFX_AA_2X:
		if(Caps.m_bAA2X)
			m_uAAMode = D3DMULTISAMPLE_2_SAMPLES;
		break;
	case IFX_AA_3X:
		if(Caps.m_bAA3X)
			m_uAAMode = D3DMULTISAMPLE_3_SAMPLES;
		break;
	case IFX_AA_4X:
		if(Caps.m_bAA4X)
			m_uAAMode = D3DMULTISAMPLE_4_SAMPLES;
		break;
	}

	return IFX_OK;
}

IFXRESULT CIFXRenderDX8::SetHWDepthStencilFormat(IFXenum eDepthStencil)
{
	IFXRESULT rc = IFX_OK;

	m_eDepthStencil = eDepthStencil;

	return rc;
}

IFXRESULT CIFXRenderDX8::MoveDTSWindow(const IFXRect& rcWindow)
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
			m_hWnd = CreateWindow(	IFX_RENDER_WINDOW_CLASS,
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

		}


	}

	return rc;
}

IFXRESULT CIFXRenderDX8::SetHWWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid() && m_spDeviceDX8->GetD3DDevice())
	{
		/* Not sure if Software4X is possible in DX8
		if(m_eAAMode == IFX_AA_4X_SW)
		{
			rc = SetAAWindow(bDirty);
		}
		else
		*/
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

IFXRESULT CIFXRenderDX8::SetDTSWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	// Release non-DTS back buffer
	IFXRELEASE(m_pBackBuffer);
	IFXRELEASE(m_pSysMemBBCopy);

	// Reset device if needed
	rc = m_spDeviceDX8->ResetDevice();

	IFXRect rcBackSave = m_rcBackBuffer;

	RECT rcBB;
	GetClientRect((HWND)m_Window.GetWindowPtr(), &rcBB);
	IFXRect rcMon = m_rcDevice;
	ScreenToClient((HWND)m_Window.GetWindowPtr(), (LPPOINT)&rcMon.m_X);

	if(IFXSUCCESS(rc))
	{
		m_rcBackBuffer.Set( rcBB.left, rcBB.top, rcBB.right - rcBB.left, rcBB.bottom - rcBB.top );
		rc = SizeBackBuffer(rcMon);
	}
	if(!(rcBackSave == m_rcBackBuffer))
	{
		MoveDTSWindow(m_rcBackBuffer);
	}

	if(!(rcBackSave.m_Width == m_rcBackBuffer.m_Width &&
		 rcBackSave.m_Height == m_rcBackBuffer.m_Height)
		|| !m_pSwapChain || bDirty)
	{
		if(IFXSUCCESS(rc))
		{
			IFXRELEASE(m_pSwapChain);

			D3DDISPLAYMODE d3ddm;
			m_spDeviceDX8->GetD3D()->GetAdapterDisplayMode( m_uDeviceNum, &d3ddm );

			D3DPRESENT_PARAMETERS d3dpp;
			memset( &d3dpp, 0, sizeof(d3dpp) );
			d3dpp.Windowed   = TRUE;
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
			if(m_bVSyncEnabled)
			{
				d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
			}
			d3dpp.MultiSampleType = m_uAAMode;
			if(m_uAAMode != D3DMULTISAMPLE_NONE)
			{
				d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			}
			d3dpp.BackBufferFormat = d3ddm.Format;
			d3dpp.hDeviceWindow = m_hWnd;
			m_spDeviceDX8->GetD3DDevice()->CreateAdditionalSwapChain(&d3dpp, &m_pSwapChain);

			if(m_pSwapChain)
			{
				LPDIRECT3DSURFACE8 pRendTarget = 0;
				m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pRendTarget);

				rc = CreateDepthBuffer(pRendTarget);

				IFXRELEASE(pRendTarget);
			}
			else
				rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		IFXRUN(rc, MakeHWCurrent());
	}

	return rc;
}

IFXRESULT CIFXRenderDX8::SetNonDTSWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;

	// Release DTS back buffer
	IFXRELEASE(m_pSwapChain);

	// Destroy the DTS window
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}

	// Reset device if needed
	rc = m_spDeviceDX8->ResetDevice();

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
			IFXRELEASE(m_pBackBuffer);
			IFXRELEASE(m_pSysMemBBCopy);

			D3DDISPLAYMODE d3ddm;
			m_spDeviceDX8->GetD3D()->GetAdapterDisplayMode( m_uDeviceNum, &d3ddm );

			m_spDeviceDX8->GetD3DDevice()->CreateRenderTarget(m_rcBackBuffer.m_Width,
															m_rcBackBuffer.m_Height,
															d3ddm.Format,
															m_uAAMode,
															TRUE, &m_pBackBuffer);

			if(m_pBackBuffer)
			{
				m_spDeviceDX8->GetD3DDevice()->CreateImageSurface(m_rcBackBuffer.m_Width,
															   m_rcBackBuffer.m_Height,
															   d3ddm.Format,
															   &m_pSysMemBBCopy);
				if(m_pSysMemBBCopy)
				{
					rc = CreateDepthBuffer(m_pBackBuffer);
				}
				else
				{
					rc = IFX_E_SUBSYSTEM_FAILURE;
				}

			}
			else
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}

			IFXRUN(rc, MakeHWCurrent());
		}
	}

	return rc;
}
/* Not sure if software4X is possible in DX8 !
IFXRESULT CIFXRenderDX8::SetAAWindow(BOOL bDirty)
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;

	// Release any DTS Swapchains
	IFXRELEASE(m_pSwapChain);

	// Reset device if needed
	rc = m_spDeviceDX8->ResetDevice();

	IFXRect rcBackSave = m_rcBackBuffer;

	if(m_Window.GetDTS())
	{
		RECT rcBB;
		GetClientRect((HWND)m_Window.GetWindowPtr(), &rcBB);
		IFXRect rcMon = m_rcDevice;
		ScreenToClient((HWND)m_Window.GetWindowPtr(), (LPPOINT)&rcMon.m_X);

		if(IFXSUCCESS(rc))
		{
			m_rcBackBuffer.SetWinRect(rcBB);
			rc = SizeBackBuffer(rcMon);
		}

		if(!(rcBackSave == m_rcBackBuffer))
		{
			MoveDTSWindow(m_rcBackBuffer);
		}

	}
	else
	{
		m_hDC = (HDC)m_Window.GetWindowPtr();

		HBITMAP hBM = (HBITMAP)GetCurrentObject(m_hDC, OBJ_BITMAP);
		BITMAP bitmap;
		GetObject(hBM, sizeof(BITMAP), (void*)&bitmap);

		m_rcBackBuffer.Set(0, 0, bitmap.bmWidth, bitmap.bmHeight);

		rc = SizeBackBuffer(m_Window.GetWindowSize());
	}

	if(!(rcBackSave.m_Width == m_rcBackBuffer.m_Width &&
		 rcBackSave.m_Height == m_rcBackBuffer.m_Height)
		|| !m_pBackBuffer || bDirty)
	{
		if(IFXSUCCESS(rc))
		{
			IFXRELEASE(m_pBackBuffer);
			IFXRELEASE(m_pSysMemBBCopy);

			D3DDISPLAYMODE d3ddm;
			m_spDeviceDX8->GetD3D()->GetAdapterDisplayMode( m_uDeviceNum, &d3ddm );

			hr = m_spDeviceDX8->GetD3DDevice()->CreateRenderTarget( m_rcBackBuffer.m_Width * IFX_AA_SS_FACTOR,
																	m_rcBackBuffer.m_Height * IFX_AA_SS_FACTOR,
																	d3ddm.Format,
																	D3DMULTISAMPLE_NONE,
																	TRUE, &m_pBackBuffer);

			IFXASSERTBOX(SUCCEEDED(hr), "Failed to create #software4X back buffer");

			if(m_pBackBuffer)
			{
				m_spDeviceDX8->GetD3DDevice()->CreateImageSurface(m_rcBackBuffer.m_Width * IFX_AA_SS_FACTOR,
															   m_rcBackBuffer.m_Height * IFX_AA_SS_FACTOR,
															   d3ddm.Format,
															   &m_pSysMemBBCopy);
				if(m_pSysMemBBCopy)
				{
					rc = CreateDepthBuffer(m_pBackBuffer);
				}
				else
				{
					rc = IFX_E_SUBSYSTEM_FAILURE;
				}

			}
			else
			{
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}

			IFXRUN(rc, MakeHWCurrent());
		}
	}


	return IFX_OK;
}
*/

D3DFORMAT CIFXRenderDX8::GetDepthFormat(IFXenum eDepthFormat)
{
	D3DFORMAT uVal = D3DFMT_D16;

	switch(eDepthFormat)
	{
	case IFX_DEPTH_D16S0:
		uVal = D3DFMT_D16;
		break;
	case IFX_DEPTH_D32S0:
		uVal = D3DFMT_D24X8;
		break;
	case IFX_DEPTH_D24S8:
		uVal = D3DFMT_D24S8;
		break;
	}

	return uVal;
}

IFXRESULT CIFXRenderDX8::CreateDepthBuffer(LPDIRECT3DSURFACE8 pRendTarget)
{
	IFXRESULT rc = IFX_OK;

	m_bNeedClear = TRUE;

	D3DSURFACE_DESC d3dSurfDesc;
	memset(&d3dSurfDesc, 0, sizeof(D3DSURFACE_DESC));
	pRendTarget->GetDesc(&d3dSurfDesc);

	IFXRELEASE(m_pDepthBuffer);
	HRESULT hr = D3D_OK;

		m_spDeviceDX8->GetD3DDevice()->CreateDepthStencilSurface(d3dSurfDesc.Width, d3dSurfDesc.Height,
					GetDepthFormat(m_eDepthStencil), m_uAAMode, &m_pDepthBuffer);

	IFXASSERTBOXEX(SUCCEEDED(hr), "Create D24S8 Depth/Stencil Failed", FALSE, TRUE);

	if(SUCCEEDED(hr))
	{
		hr = m_spDeviceDX8->GetD3DDevice()->SetRenderTarget(pRendTarget, m_pDepthBuffer);
	}

	if(FAILED(hr))
	{
		IFXRELEASE(m_pDepthBuffer);
		m_eDepthStencil = IFX_DEPTH_D16S0;
		hr = m_spDeviceDX8->GetD3DDevice()->CreateDepthStencilSurface(d3dSurfDesc.Width, d3dSurfDesc.Height,
			GetDepthFormat(m_eDepthStencil), m_uAAMode, &m_pDepthBuffer);

		IFXASSERTBOXEX(SUCCEEDED(hr), "Create D16 Depth/Stencil Failed", FALSE, TRUE);

		if(SUCCEEDED(hr))
		{
			hr = m_spDeviceDX8->GetD3DDevice()->SetRenderTarget(pRendTarget, m_pDepthBuffer);

			IFXASSERTBOXEX(SUCCEEDED(hr), "SetRenderTarget  D16 Depth/Stencil Failed", FALSE, TRUE);
		}
	}

	if(FAILED(hr))
	{
		rc = IFX_E_SUBSYSTEM_FAILURE;
	}

	return rc;
}

IFXRESULT CIFXRenderDX8::ClearHW(const IFXRenderClear& rClear)
{
	HRESULT hr = D3D_OK;
	IFXRESULT rc = IFX_OK;

	if(m_spDevice.IsValid() && m_spDeviceDX8->GetD3DDevice())
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
		U32 uColor = D3DCOLOR_ARGB(	255,
									(U32)(rClear.GetColorValue().XC() * 255),
									(U32)(rClear.GetColorValue().YC() * 255),
									(U32)(rClear.GetColorValue().ZC() * 255)
								  );
		if ( uBuffers )
		{
			IFXRect rcIClear = m_spDevice->GetVisibleViewport();

			RECT rcClear;
			rcClear.left = rcIClear.m_X;
			rcClear.top = rcIClear.m_Y;
			rcClear.right = rcIClear.m_X + rcIClear.m_Width;
			rcClear.bottom = rcIClear.m_Y + rcIClear.m_Height;
			hr = m_spDeviceDX8->GetD3DDevice()->Clear(1,
													  (const D3DRECT*)&(rcClear),
													  uBuffers, uColor,
													  rClear.GetDepthValue(),
													  rClear.GetStencilValue()	);

			if(SUCCEEDED(hr))
			{
				if((m_bNeedClear || rClear.GetColorCleared()) && (!m_Window.GetDTS() && m_Window.GetTransparent()))
				{
					D3DLOCKED_RECT d3dRect;
					m_pSysMemBBCopy->LockRect(&d3dRect, &rcClear, 0);

					// Copy image data
					HBITMAP hBM = (HBITMAP)GetCurrentObject(m_hDC, OBJ_BITMAP);
					BITMAP bm;
					GetObject(hBM, sizeof(BITMAP), (void*)&bm);

					U32 uBpp = bm.bmBitsPixel >> 3;
					U32 uY = bm.bmHeight - m_rcBackBuffer.m_Height - m_rcBackBuffer.m_Y;
					U8* pSrc = ((U8*)bm.bmBits) + uY*bm.bmWidthBytes + m_rcBackBuffer.m_X*uBpp;

					rc = CopyImageData(pSrc, (U8*)d3dRect.pBits, bm.bmWidthBytes, d3dRect.Pitch,
									   rcIClear.m_Width, rcIClear.m_Height, uBpp, FALSE);

					m_pSysMemBBCopy->UnlockRect();

					if(IFXSUCCESS(rc))
					{
						hr = m_spDeviceDX8->GetD3DDevice()->CopyRects(m_pSysMemBBCopy, NULL, 0, m_pBackBuffer, NULL);
					}
				}
			}
		}
		if(SUCCEEDED(hr))
		{
			rc = m_spDeviceDX8->BeginScene();
		}
		else
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		m_bNeedClear = FALSE;
	}
	return rc;
}

IFXRESULT CIFXRenderDX8::MakeHWCurrent()
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		if(m_uAAMode)
		{
			rc = m_spDeviceDX8->SetRS(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		}
		else
		{
			rc = m_spDeviceDX8->SetRS(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
		}
	}

	if(m_Window.GetDTS())
	{
		if(m_pSwapChain)
		{
			LPDIRECT3DSURFACE8 pRendTarget = 0;
			m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pRendTarget);

			m_spDeviceDX8->GetD3DDevice()->SetRenderTarget(pRendTarget, m_pDepthBuffer);

			IFXRELEASE(pRendTarget);
		}
	}
	else if(m_pBackBuffer)
	{
		m_spDeviceDX8->GetD3DDevice()->SetRenderTarget(m_pBackBuffer, m_pDepthBuffer);
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

IFXRESULT CIFXRenderDX8::Swap()
{
	IFXRESULT rc = IFX_OK;

	if(m_spDeviceDX8.IsValid() && m_spDeviceDX8->GetD3DDevice())
	{
		m_spDeviceDX8->EndScene();
	}

	BOOL bNeedNewInit = FALSE;

	if(m_Window.GetDTS())
	{
		if(m_pSwapChain)
		{
			RECT rcWin = {0, 0, m_rcVisibleWindow.m_Width, m_rcVisibleWindow.m_Height };
			HRESULT hr = m_pSwapChain->Present(&rcWin, &rcWin, NULL, NULL);
			if(hr == D3DERR_DEVICELOST)
			{
				rc = m_spDeviceDX8->ResetDevice();

				if(IFXSUCCESS(rc))
				{
					bNeedNewInit = TRUE;
				}
			}
		}
		else
		{
			bNeedNewInit = TRUE;
		}
	}
	else
	{
		if(m_pBackBuffer)
		{
			// Copy image from back buffer to m_hDC
			D3DLOCKED_RECT d3dRect;

			RECT rcLock = {0, 0, m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height };

			m_spDeviceDX8->GetD3DDevice()->CopyRects(m_pBackBuffer, NULL, 0, m_pSysMemBBCopy, NULL);

			m_pSysMemBBCopy->LockRect(&d3dRect, &rcLock, 0/*D3DLOCK_READONLY*/);

			// Copy image data
			HBITMAP hBM = (HBITMAP)GetCurrentObject(m_hDC, OBJ_BITMAP);
			BITMAP bm;
			GetObject(hBM, sizeof(BITMAP), (void*)&bm);

			U32 uBpp = bm.bmBitsPixel >> 3;
			U32 uY = bm.bmHeight - m_rcBackBuffer.m_Height - m_rcBackBuffer.m_Y;
			U8* pDst = ((U8*)bm.bmBits) + uY*bm.bmWidthBytes + m_rcBackBuffer.m_X*uBpp;

			rc = CopyImageData((U8*)d3dRect.pBits, pDst, d3dRect.Pitch, bm.bmWidthBytes,
								m_rcBackBuffer.m_Width, m_rcBackBuffer.m_Height, uBpp, TRUE);

			m_pSysMemBBCopy->UnlockRect();
		}
		else
		{
			bNeedNewInit = TRUE;
		}
	}

	if(bNeedNewInit)
	{
		rc = SetHWWindow(TRUE);
	}

	return rc;
}

IFXenum CIFXRenderDX8::GetIFXRenderFormat( D3DFORMAT d3dFormat )
{
	switch(d3dFormat)
	{
	case D3DFMT_A1R5G5B5:
		return IFX_RGBA_5551;
	case D3DFMT_X1R5G5B5:
		return IFX_RGBA_5550;
	case D3DFMT_R5G6B5:
		return IFX_RGBA_5650;
	case D3DFMT_A4R4G4B4:
		return IFX_RGBA_4444;
    case D3DFMT_R8G8B8:
	case D3DFMT_X8R8G8B8:
		return IFX_RGBA_8880;
	case D3DFMT_A8R8G8B8:
		return IFX_RGBA_8888;
	}

	IFXASSERTBOX(1, "Unknown D3D color format!");

	return IFX_RGBA_5650;
}

IFXRESULT CIFXRenderDX8::SetVSyncEnabled(BOOL bVSyncEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_bVSyncEnabled != bVSyncEnabled)
	{
		m_bVSyncEnabled = bVSyncEnabled;
		rc = SetHWWindow(TRUE);
	}

	return rc;
}

IFXRESULT CIFXRenderDX8::CreateTexture(CIFXDeviceTexturePtr& rTexture)
{
	IFXRESULT rc = IFX_OK;

	if(m_spDeviceDX8.IsValid())
	{
		CIFXDeviceTextureDX8Ptr rTexDX8;
		rc = rTexDX8.Create(CID_IFXDeviceTextureDX8, CID_IFXDeviceTextureDX8);
		if(IFXSUCCESS(rc))
		{
			rc = rTexDX8->Initialize(m_spDeviceDX8);
		}
		if(IFXSUCCESS(rc))
		{
			rc = rTexDX8.QI(rTexture, CID_IFXDeviceTexture);
		}
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

