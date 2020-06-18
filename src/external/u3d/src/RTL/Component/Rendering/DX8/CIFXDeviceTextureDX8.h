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
// CIFXDeviceTextureDX8.h
#ifndef CIFX_DEVICE_TEXTUREDX8_H
#define CIFX_DEVICE_TEXTUREDX8_H

#include "CIFXDeviceTexture.h"

//==============================
// CIFXDeviceTextureDX8
//==============================
class CIFXDeviceTextureDX8 : public CIFXDeviceTexture
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureDX8Factory(IFXREFIID intId, void** ppUnk);

	//====================================
	// CIFXDeviceTextureDX8 methods
	//====================================
	IFXRESULT IFXAPI Initialize(CIFXRenderDeviceDX8Ptr& spDevice);

	LPDIRECT3DBASETEXTURE8 IFXAPI GetTexture() { return m_pTexture; }
	IFXenum IFXAPI GetType() const { return m_eType; }
	
protected:
	CIFXDeviceTextureDX8();
	virtual ~CIFXDeviceTextureDX8();
	
	virtual IFXRESULT IFXAPI SetHWRenderFormat(IFXenum eFormat);
	virtual IFXRESULT IFXAPI SetHWTexture();

	virtual IFXRESULT IFXAPI Lock(	IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	);

	virtual IFXRESULT IFXAPI Unlock(IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	);

	void IFXAPI InitData();

	virtual IFXRESULT IFXAPI Create2DTexture();
	virtual IFXRESULT IFXAPI CreateCubeTexture();

	LPDIRECT3DSURFACE8 IFXAPI GetTextureSurface(IFXenum eType, U32 uMipLevel);

	virtual D3DFORMAT IFXAPI GetD3DTexFormat(IFXenum eFormat);
	virtual IFXenum IFXAPI GetIFXTexFormat(D3DFORMAT d3dFormat);
	virtual D3DCUBEMAP_FACES IFXAPI GetD3DCubeMapFace(IFXenum eFace);

	IFXRESULT IFXAPI Construct();

	CIFXRenderDeviceDX8* m_pDevice;

	D3DFORMAT m_fmtTexture;
	LPDIRECT3DBASETEXTURE8 m_pTexture;
	LPDIRECT3DSURFACE8	m_pLockSurf;
	BOOL m_bNeedsRecreate;
	BOOL m_bInLock;
};
typedef IFXSmartPtr<CIFXDeviceTextureDX8> CIFXDeviceTextureDX8Ptr;

#endif // CIFX_DEVICE_TEXTUREDX8_H
