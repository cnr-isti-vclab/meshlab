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
#include "IFXRenderPCHDX8.h"
#include "CIFXDeviceTextureDX8.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureDX8Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTextureDX8* pPtr = new CIFXDeviceTextureDX8;
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

U32 CIFXDeviceTextureDX8::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTextureDX8::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTextureDX8::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexture)
		{
			*(CIFXDeviceTexture**)ppInterface = (CIFXDeviceTexture*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTextureDX8)
		{
			*(CIFXDeviceTextureDX8**)ppInterface = (CIFXDeviceTextureDX8*) this;
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
CIFXDeviceTextureDX8::CIFXDeviceTextureDX8()
{
	m_refCount = 0;
}

CIFXDeviceTextureDX8::~CIFXDeviceTextureDX8()
{
	IFXRELEASE(m_pTexture);
}

IFXRESULT CIFXDeviceTextureDX8::Construct()
{
	IFXRESULT rc = CIFXDeviceTexture::Construct();
	
	InitData();
	
	return rc;
}

void CIFXDeviceTextureDX8::InitData()
{
	m_fmtTexture = D3DFMT_A8R8G8B8;
	m_pLockSurf = 0;
	m_pDevice = 0;
	m_pTexture = 0;
	m_bNeedsRecreate = TRUE;
	m_bInLock = FALSE;
}

IFXRESULT CIFXDeviceTextureDX8::Initialize(CIFXRenderDeviceDX8Ptr& spDevice)
{
	IFXRESULT rc = IFX_OK;
	
	m_pDevice = spDevice.GetPointerNR();
	
	m_pDevice->GetCaps(m_RCaps);
	
	return rc;
}

IFXRESULT CIFXDeviceTextureDX8::SetHWTexture()
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if(m_bNeedData)
		{
			switch(m_eType)
			{
			case IFX_TEXTURE_1D:
			case IFX_TEXTURE_2D:
				rc = Create2DTexture();
				break;
			case IFX_TEXTURE_CUBE:
				rc = CreateCubeTexture();
				break;
			}
		}
	}

	return IFX_W_NEED_TEXTURE_DATA;
}

IFXRESULT CIFXDeviceTextureDX8::SetHWRenderFormat(IFXenum eFormat)
{
	m_eRenderFormat = eFormat;

	m_fmtTexture = GetD3DTexFormat(eFormat);

	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureDX8::Lock(	IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;
	D3DLOCKED_RECT d3dRect;
	D3DSURFACE_DESC d3dSurfDesc;

	if(m_eType != eType && m_eType == IFX_TEXTURE_2D)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(m_eType == IFX_TEXTURE_CUBE && eType == IFX_TEXTURE_2D)
	{
		eType = IFX_TEXTURE_CUBE_PZ;
	}

	if(m_pLockSurf)
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	if(IFXSUCCESS(rc))
	{
		m_pLockSurf = GetTextureSurface(eType, uMipLevel);

		if(0 == m_pLockSurf)
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		HRESULT hr = m_pLockSurf->GetDesc(&d3dSurfDesc);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
			IFXRELEASE(m_pLockSurf);
		}
	}

	if(IFXSUCCESS(rc))
	{
		HRESULT hr = m_pLockSurf->LockRect(&d3dRect, 0, 0);
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
			IFXRELEASE(m_pLockSurf);
		}
	}

	if(IFXSUCCESS(rc))
	{
		rTex.m_width = d3dSurfDesc.Width;
		rTex.m_height = d3dSurfDesc.Height;
		rTex.eChannelOrder = IFX_BGRA;
		rTex.eRenderFormat = GetIFXTexFormat(d3dSurfDesc.Format);
		rTex.m_pData = (U8*)d3dRect.pBits;
		rTex.m_pitch = d3dRect.Pitch;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureDX8::Unlock(	IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;

	if(m_pLockSurf)
	{
		HRESULT hr = m_pLockSurf->UnlockRect();
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		m_pLockSurf->Release();
		m_pLockSurf = 0;
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureDX8::Create2DTexture()
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(m_pDevice, "Creating texture before device!");
	IFXRELEASE(m_pTexture);

	if(m_pDevice)
	{
		// The first thing we do is make sure that none of the 
		// texture units have this texture bound.  This will guarantee
		// that any current texture is fully released before we try to
		// allocate a new one.
		U32 i;
		for( i = 0; i < IFX_MAX_TEXUNITS; i++)
		{
			const CIFXDeviceTexUnitPtr& spTexUnit = m_pDevice->GetTexUnit(i);
			spTexUnit->SetTextureId(1);
		}

		LPDIRECT3DTEXTURE8 pTex = NULL;
		HRESULT hr = m_pDevice->GetD3DDevice()->CreateTexture(m_uWidth, 
															 m_uHeight, 
															 0, 
															 NULL, 
															 m_fmtTexture, 
															 D3DPOOL_MANAGED, 
															 &pTex);
		if(SUCCEEDED(hr))
		{
			hr = pTex->QueryInterface(IID_IDirect3DBaseTexture8, 
										(void**)&m_pTexture);
			IFXRELEASE(pTex);
		}

		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureDX8::CreateCubeTexture()
{
	IFXRESULT rc = IFX_OK;
	
	IFXASSERTBOX(m_pDevice, "Creating texture before device!");
	IFXRELEASE(m_pTexture);
	
	if(m_pDevice)
	{
		// The first thing we do is make sure that none of the 
		// texture units have this texture bound.  This will guarantee
		// that any current texture is fully released before we try to
		// allocate a new one.
		U32 i;
		for( i = 0; i < IFX_MAX_TEXUNITS; i++)
		{
			const CIFXDeviceTexUnitPtr& spTexUnit = m_pDevice->GetTexUnit(i);
			spTexUnit->SetTextureId(1);
		}

		LPDIRECT3DCUBETEXTURE8 pTex = NULL;
		HRESULT hr = 
			m_pDevice->GetD3DDevice()->CreateCubeTexture(m_uWidth, 
														0, 
														NULL, 
														m_fmtTexture, 
														D3DPOOL_MANAGED, 
														&pTex);
		if(SUCCEEDED(hr))
		{
			hr = pTex->QueryInterface(IID_IDirect3DBaseTexture8, 
										(void**)&m_pTexture);
			IFXRELEASE(pTex);
		}
		
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	
	return rc;
}

LPDIRECT3DSURFACE8 CIFXDeviceTextureDX8::GetTextureSurface(IFXenum eType, U32 uMipLevel)
{
	LPDIRECT3DSURFACE8 pSurf = 0;
	HRESULT hr = D3D_OK;

	if(m_pTexture)
	{
		if(m_eType == IFX_TEXTURE_2D)
		{
			LPDIRECT3DTEXTURE8 pTex = 0;
			hr = m_pTexture->QueryInterface(IID_IDirect3DTexture8, (void**)&pTex);
			if(SUCCEEDED(hr))
			{
				hr = pTex->GetSurfaceLevel(uMipLevel, &pSurf);
			}

			IFXRELEASE(pTex);
		}
		else
		{
			LPDIRECT3DCUBETEXTURE8 pTex = 0;
			hr = m_pTexture->QueryInterface(IID_IDirect3DCubeTexture8, (void**)&pTex);
			if(SUCCEEDED(hr))
			{
				hr = pTex->GetCubeMapSurface(GetD3DCubeMapFace(eType), uMipLevel, &pSurf);
			}
			
			IFXRELEASE(pTex);
		}
	}

	return pSurf;
}

IFXenum CIFXDeviceTextureDX8::GetIFXTexFormat(D3DFORMAT d3dFormat)
{
	IFXenum fmtReturn = IFX_RGBA_8888;
	switch(d3dFormat)
	{
	case D3DFMT_A8R8G8B8:
		fmtReturn = IFX_RGBA_8888;
		break;
	case D3DFMT_X8R8G8B8:
		fmtReturn = IFX_RGBA_8880;
		break;
	case D3DFMT_R5G6B5:
		fmtReturn = IFX_RGBA_5650;
		break;
	case D3DFMT_A1R5G5B5:
		fmtReturn = IFX_RGBA_5551;
		break;
	case D3DFMT_X1R5G5B5:
		fmtReturn = IFX_RGBA_5550;
		break;
	case D3DFMT_A4R4G4B4:
		fmtReturn = IFX_RGBA_4444;
		break;
	case D3DFMT_DXT1:
		fmtReturn = IFX_TEXCOMPRESS_1;
		break;
	case D3DFMT_DXT2:
		fmtReturn = IFX_TEXCOMPRESS_2;
		break;
	case D3DFMT_DXT3:
		fmtReturn = IFX_TEXCOMPRESS_3;
		break;
	case D3DFMT_DXT4:
		fmtReturn = IFX_TEXCOMPRESS_4;
		break;
	case D3DFMT_DXT5:
		fmtReturn = IFX_TEXCOMPRESS_5;
		break;
	default:
		IFXASSERTBOX(0, "Invalid D3D texture format specified!");
		break;
	}
	
	return fmtReturn;
}

D3DFORMAT CIFXDeviceTextureDX8::GetD3DTexFormat(IFXenum eFormat)
{
	D3DFORMAT fmtReturn = D3DFMT_A8R8G8B8;

	switch(eFormat)
	{
	case IFX_RGBA_8888:
		fmtReturn = D3DFMT_A8R8G8B8;
		break;
	case IFX_RGBA_8880:
		fmtReturn = D3DFMT_X8R8G8B8;
		break;
	case IFX_RGBA_5650:
		fmtReturn = D3DFMT_R5G6B5;
		break;
	case IFX_RGBA_5551:
		fmtReturn = D3DFMT_A1R5G5B5;
		break;
	case IFX_RGBA_5550:
		fmtReturn = D3DFMT_X1R5G5B5;
		break;
	case IFX_RGBA_4444:
		fmtReturn = D3DFMT_A4R4G4B4;
		break;
	case IFX_TEXCOMPRESS_1:
		fmtReturn = D3DFMT_DXT1;
		break;
	case IFX_TEXCOMPRESS_2:
		fmtReturn = D3DFMT_DXT2;
		break;
	case IFX_TEXCOMPRESS_3:
		fmtReturn = D3DFMT_DXT3;
		break;
	case IFX_TEXCOMPRESS_4:
		fmtReturn = D3DFMT_DXT4;
		break;
	case IFX_TEXCOMPRESS_5:
		fmtReturn = D3DFMT_DXT5;
		break;
	default:
		IFXASSERTBOX(0, "Invalid IFX texture format specified!");
		break;
	}
	
	return fmtReturn;
}

D3DCUBEMAP_FACES CIFXDeviceTextureDX8::GetD3DCubeMapFace(IFXenum eFace)
{
	D3DCUBEMAP_FACES d3dCubeFace = D3DCUBEMAP_FACE_POSITIVE_X;

	switch(eFace)
	{
	case IFX_TEXTURE_CUBE_PX:
		d3dCubeFace = D3DCUBEMAP_FACE_POSITIVE_X;
		break;
	case IFX_TEXTURE_CUBE_NX:
		d3dCubeFace = D3DCUBEMAP_FACE_NEGATIVE_X;
		break;
	case IFX_TEXTURE_CUBE_PY:
		d3dCubeFace = D3DCUBEMAP_FACE_POSITIVE_Y;
		break;
	case IFX_TEXTURE_CUBE_NY:
		d3dCubeFace = D3DCUBEMAP_FACE_NEGATIVE_Y;
		break;
	case IFX_TEXTURE_CUBE_PZ:
		d3dCubeFace = D3DCUBEMAP_FACE_POSITIVE_Z;
		break;
	case IFX_TEXTURE_CUBE_NZ:
		d3dCubeFace = D3DCUBEMAP_FACE_NEGATIVE_Z;
		break;
	default:
		IFXASSERTBOX(0, "Invalid IFX cubemap face identifier specified");
		break;
	}

	return d3dCubeFace;
}
