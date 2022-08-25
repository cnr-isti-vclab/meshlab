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
// CIFXRenderDX8.h
#ifndef CIFX_RENDERDX8_H
#define CIFX_RENDERDX8_H

#include "CIFXRender.h"
#include "CIFXDeviceLightDX8.h"
#include "CIFXDeviceTextureDX8.h"
#include "CIFXDeviceTexUnitDX8.h"
#include "CIFXRenderDeviceDX8.h"

#define DX8_NAME  "DirectX8"

class CIFXRenderDX8 : public CIFXRender
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderDX8Factory(IFXREFIID intId, void** ppUnk);

	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	IFXRESULT IFXAPI GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals) {return IFX_OK;}
		
	IFXRESULT IFXAPI ReleaseVidMem();
	IFXRESULT IFXAPI Swap();

protected:
	CIFXRenderDX8();
	virtual ~CIFXRenderDX8();

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	IFXRESULT IFXAPI CreateTexture(CIFXDeviceTexturePtr& rTexture);

	// Window Settings
	IFXRESULT IFXAPI SetHWAntiAliasingMode(IFXenum eAAMode);
	IFXRESULT IFXAPI SetHWDepthStencilFormat(IFXenum eDepthStencil);
	D3DFORMAT IFXAPI GetDepthFormat(IFXenum eDepthStencil);
	IFXenum	  IFXAPI GetIFXRenderFormat(D3DFORMAT d3dFormat);
	IFXRESULT IFXAPI MakeHWCurrent();
	IFXRESULT IFXAPI ClearHW(const IFXRenderClear& rClear);
	IFXRESULT IFXAPI SetVSyncEnabled(BOOL bVSyncEnabled);
	IFXRESULT IFXAPI SetHWWindow(BOOL bDirty);
	IFXRESULT IFXAPI SetDTSWindow(BOOL bDirty);
	IFXRESULT IFXAPI SetNonDTSWindow(BOOL bDirty);
	IFXRESULT IFXAPI MoveDTSWindow(const IFXRect& rcWindow);
	IFXRESULT IFXAPI CreateDepthBuffer(LPDIRECT3DSURFACE8 pRendTarget);

	// Window Data
	LPDIRECT3DSWAPCHAIN8	m_pSwapChain;
	LPDIRECT3DSURFACE8		m_pDepthBuffer;
	LPDIRECT3DSURFACE8		m_pBackBuffer;
	LPDIRECT3DSURFACE8		m_pSysMemBBCopy;
	HWND					m_hWnd;
	HDC						m_hDC;
	BOOL					m_bNeedClear;
	D3DMULTISAMPLE_TYPE		m_uAAMode;
	
	CIFXDeviceLightDX8Ptr		m_pspLightsDX8[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitDX8Ptr		m_pspTexUnitsDX8[IFX_MAX_TEXUNITS];
	CIFXRenderDeviceDX8Ptr		m_spDeviceDX8;
};

typedef IFXSmartPtr<CIFXRenderDX8> CIFXRenderDX8Ptr;


#endif // CIFX_RENDERDX8_H
