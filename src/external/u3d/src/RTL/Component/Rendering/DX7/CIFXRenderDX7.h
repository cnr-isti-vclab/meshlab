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
// CIFXRenderDX7.h
#ifndef CIFX_RENDERDX7_H
#define CIFX_RENDERDX7_H

#include "CIFXRender.h"
#include "CIFXDeviceLightDX7.h"
#include "CIFXDeviceTextureDX7.h"
#include "CIFXDeviceTexUnitDX7.h"
#include "CIFXRenderDeviceDX7.h"

#define DX7_NAME  "DirectX7"

class CIFXRenderDX7 : public CIFXRender
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderDX7Factory(IFXREFIID intId, void** ppUnk);
	
	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);
		
	IFXRESULT IFXAPI GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals) {return IFX_OK;}
	
	IFXRESULT IFXAPI ReleaseVidMem();

	IFXRESULT IFXAPI Swap();
	
protected:
	CIFXRenderDX7();
	virtual ~CIFXRenderDX7();
	
	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();
	
	IFXRESULT IFXAPI CreateTexture(CIFXDeviceTexturePtr& rTexture);
	
	// Window Settings
	IFXRESULT IFXAPI MakeHWCurrent();
	IFXRESULT IFXAPI ClearHW(const IFXRenderClear& rClear);
	IFXRESULT IFXAPI SetVSyncEnabled(BOOL bVSyncEnabled) { m_bVSyncEnabled = bVSyncEnabled; return IFX_OK; }
	IFXRESULT IFXAPI SetHWWindow(BOOL bDirty);
	IFXRESULT IFXAPI MoveDTSWindow(const IFXRect& rcWindow);
	IFXRESULT IFXAPI SetDTSWindow(BOOL bDirty);
	IFXRESULT IFXAPI SetNonDTSWindow(BOOL bDirty);
	IFXRESULT IFXAPI SetAAWindow(BOOL bDirty);
	IFXRESULT IFXAPI SetHWAntiAliasingMode(IFXenum eAAMode);
	IFXRESULT IFXAPI CreateBackBuffer();
	HRESULT   IFXAPI SwapAA(RECT& rcScreen, RECT& rcLocal);
	IFXenum	  IFXAPI GetIFXRenderFormat(DDPIXELFORMAT ddpf);

	// Window Data
	LPDIRECTDRAWSURFACE7		m_pBackBuffer;
	LPDIRECTDRAWSURFACE7		m_pDepthBuffer;
	LPDIRECTDRAWSURFACE7		m_pAABuffer;
	LPDIRECTDRAWCLIPPER			m_pClipper;
	HWND						m_hWnd;
	HDC							m_hDC;
	BOOL						m_bNeedClear;
	
	CIFXDeviceLightDX7Ptr		m_pspLightsDX7[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitDX7Ptr		m_pspTexUnitsDX7[IFX_MAX_TEXUNITS];
	CIFXRenderDeviceDX7Ptr		m_spDeviceDX7;
	
};

typedef IFXSmartPtr<CIFXRenderDX7> CIFXRenderDX7Ptr;


#endif // CIFX_RENDERDX7_H
