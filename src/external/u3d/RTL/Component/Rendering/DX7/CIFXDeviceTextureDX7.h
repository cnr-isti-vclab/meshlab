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
// CIFXDeviceTextureDX7.h
#ifndef CIFX_DEVICE_TEXTUREDX7_H
#define CIFX_DEVICE_TEXTUREDX7_H

#include "CIFXDeviceTexture.h"

//==============================
// CIFXDeviceTextureDX7
//==============================
class CIFXDeviceTextureDX7 : public CIFXDeviceTexture
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureDX7Factory(IFXREFIID intId, void** ppUnk);
	
	//====================================
	// CIFXDeviceTextureDX7 methods
	//====================================
	IFXRESULT IFXAPI Initialize(CIFXRenderDeviceDX7Ptr& spDevice);

	LPDIRECTDRAWSURFACE7 IFXAPI GetTexture() { return m_pTexture; }
	IFXenum IFXAPI GetType() const { return m_eType; }
	
protected:
	CIFXDeviceTextureDX7();
	virtual ~CIFXDeviceTextureDX7();
	
	virtual IFXRESULT IFXAPI SetHWRenderFormat(IFXenum eFormat);
	virtual IFXRESULT IFXAPI SetHWTexture();

	virtual IFXRESULT IFXAPI Lock(	IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	);

	virtual IFXRESULT IFXAPI Unlock(IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	);

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();
	
	virtual IFXRESULT IFXAPI CreateD3DTexture();

	LPDIRECTDRAWSURFACE7 GetTextureSurface(IFXenum eType, U32 uMipLevel);
	
	virtual DDPIXELFORMAT IFXAPI GetD3DTexFormat(IFXenum eFormat);
	virtual IFXenum IFXAPI GetIFXTexFormat(DDPIXELFORMAT& d3dFormat);
	virtual DWORD IFXAPI GetD3DCubeMapFace(IFXenum eFace);
	
	CIFXRenderDeviceDX7* m_pDevice;
	
	DDPIXELFORMAT m_fmtTexture;
	LPDIRECTDRAWSURFACE7 m_pTexture;
	LPDIRECTDRAWSURFACE7 m_pLockSurf;
	DDSURFACEDESC2 m_ddLockSurfDesc;

	BOOL m_bNeedsRecreate;
};
typedef IFXSmartPtr<CIFXDeviceTextureDX7> CIFXDeviceTextureDX7Ptr;

#endif // CIFX_DEVICE_TEXTUREDX7_H
