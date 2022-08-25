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
#include "IFXRenderPCHDX7.h"
#include "CIFXDeviceTextureDX7.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureDX7Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTextureDX7* pPtr = new CIFXDeviceTextureDX7;
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

U32 CIFXDeviceTextureDX7::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTextureDX7::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTextureDX7::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexture)
		{
			*(CIFXDeviceTexture**)ppInterface = (CIFXDeviceTexture*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTextureDX7)
		{
			*(CIFXDeviceTextureDX7**)ppInterface = (CIFXDeviceTextureDX7*) this;
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
CIFXDeviceTextureDX7::CIFXDeviceTextureDX7()
{
	m_refCount = 0;
}

CIFXDeviceTextureDX7::~CIFXDeviceTextureDX7()
{
	m_pDevice = 0;
	IFXRELEASE(m_pTexture);
}

IFXRESULT CIFXDeviceTextureDX7::Construct()
{
	IFXRESULT rc = CIFXDeviceTexture::Construct();

	InitData();

	return rc;
}

void CIFXDeviceTextureDX7::InitData()
{
	m_pDevice = 0;
	m_pTexture = 0;
	m_pLockSurf = 0;
	memset(&m_ddLockSurfDesc, 0, sizeof(DDSURFACEDESC2));
	m_ddLockSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
	m_bNeedsRecreate = TRUE;
	m_fmtTexture.dwSize = sizeof(DDPIXELFORMAT);
	m_fmtTexture.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	m_fmtTexture.dwRGBBitCount = 32;
	m_fmtTexture.dwRBitMask = 0x00FF0000;
	m_fmtTexture.dwGBitMask = 0x0000FF00;
	m_fmtTexture.dwBBitMask = 0x000000FF;
	m_fmtTexture.dwRGBAlphaBitMask = 0xFF000000;
}

IFXRESULT CIFXDeviceTextureDX7::Initialize(CIFXRenderDeviceDX7Ptr& spDevice)
{
	IFXRESULT rc = IFX_OK;
	
	m_pDevice = spDevice.GetPointerNR();

	m_pDevice->GetCaps(m_RCaps);
	
	return rc;
}

IFXRESULT CIFXDeviceTextureDX7::SetHWTexture()
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		if(m_bNeedData)
		{
			rc = CreateD3DTexture();
		}
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureDX7::SetHWRenderFormat(IFXenum eFormat)
{
	m_fmtTexture = GetD3DTexFormat(eFormat);
	m_eRenderFormat = eFormat;


	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureDX7::Lock(	IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;

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
		memset(&m_ddLockSurfDesc, 0, sizeof(DDSURFACEDESC2));

		m_ddLockSurfDesc.dwSize = sizeof(DDSURFACEDESC2);

		HRESULT hr = m_pLockSurf->Lock(0, &m_ddLockSurfDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0);
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "Lock Failed", FALSE, TRUE);
			rc = IFX_E_SUBSYSTEM_FAILURE;
			IFXRELEASE(m_pLockSurf);
		}
	}

	if(IFXSUCCESS(rc))
	{
		rTex.m_width = m_ddLockSurfDesc.dwWidth;
		rTex.m_height = m_ddLockSurfDesc.dwHeight;
		rTex.eChannelOrder = IFX_BGRA;
		rTex.eRenderFormat = GetIFXTexFormat(m_ddLockSurfDesc.ddpfPixelFormat);
		rTex.m_pData = (U8*)m_ddLockSurfDesc.lpSurface;
		rTex.m_pitch = m_ddLockSurfDesc.lPitch;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureDX7::Unlock(	IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;

	if(m_pLockSurf)
	{
		HRESULT hr = m_pLockSurf->Unlock(0);
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "Unlock Failed", FALSE, TRUE);
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}

		IFXRELEASE(m_pLockSurf);
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureDX7::CreateD3DTexture()
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

		DDSURFACEDESC2 ddSurfDesc;
		memset(&ddSurfDesc, 0, sizeof(DDSURFACEDESC2));
		ddSurfDesc.dwSize = sizeof(DDSURFACEDESC2);
		ddSurfDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddSurfDesc.dwWidth = m_uWidth;
		ddSurfDesc.dwHeight = m_uHeight;
		ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		ddSurfDesc.ddsCaps.dwCaps2 = DDSCAPS2_D3DTEXTUREMANAGE | DDSCAPS2_HINTSTATIC;

		if(m_eMipMode != IFX_NONE)
		{
			ddSurfDesc.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
		}

		if(m_eType == IFX_TEXTURE_CUBE)
		{
			ddSurfDesc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX;
			ddSurfDesc.ddsCaps.dwCaps2 |= DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALLFACES;
		}

		ddSurfDesc.ddpfPixelFormat = m_fmtTexture;
		LPDIRECTDRAWSURFACE7 pTex = NULL;
		HRESULT hr = m_pDevice->GetDD()->CreateSurface(&ddSurfDesc, &pTex, 0);
		if(SUCCEEDED(hr))
		{
			m_pTexture = pTex;
			pTex = 0;
		}

		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "CreateSurface Failed", FALSE, TRUE);
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	return rc;
}

LPDIRECTDRAWSURFACE7 CIFXDeviceTextureDX7::GetTextureSurface(IFXenum eType, U32 uMipLevel)
{
	LPDIRECTDRAWSURFACE7	pSurf = 0;
	LPDIRECTDRAWSURFACE7	pTemp = 0;
	DDSCAPS2	ddsCaps;

	if(m_pTexture)
	{
		memset(&ddsCaps, 0, sizeof(DDSCAPS2));

		pSurf = m_pTexture;
		pSurf->AddRef();

		if(eType != IFX_TEXTURE_2D && pSurf)
		{
			ddsCaps.dwCaps2 = GetD3DCubeMapFace(eType);
			pSurf->GetAttachedSurface(&ddsCaps, &pTemp);

			pSurf->Release();
			pSurf = pTemp;
			pTemp = 0;
		}

		while(pSurf && uMipLevel)
		{
			memset(&ddsCaps, 0, sizeof(DDSCAPS2));

			ddsCaps.dwCaps = DDSCAPS_MIPMAP;

			pSurf->GetAttachedSurface(&ddsCaps, &pTemp);
			pSurf->Release();
			pSurf = pTemp;
			pTemp = 0;
			uMipLevel--;
		}
	}

	return pSurf;
}

IFXenum CIFXDeviceTextureDX7::GetIFXTexFormat(DDPIXELFORMAT& d3dFormat)
{
	IFXenum fmtReturn = IFX_RGBA_8888;

	switch(d3dFormat.dwRGBAlphaBitMask)
	{
	case 0:
		{
			switch(d3dFormat.dwRGBBitCount)
			{
			case 16:
				if(d3dFormat.dwGBitMask == 0x07e0)
				{
					fmtReturn = IFX_RGBA_5650;
				}
				else
				{
					fmtReturn = IFX_RGBA_5550;
				}
				break;
			case 32:
				fmtReturn = IFX_RGBA_8880;
				break;
			}
		}
		break;
	case 0x00008000:
		fmtReturn = IFX_RGBA_5551;
		break;
	case 0x0000f000:
		fmtReturn = IFX_RGBA_4444;
		break;
	case 0xff000000:
		fmtReturn = IFX_RGBA_8888;
		break;
	}
	
	return fmtReturn;
}

DWORD CIFXDeviceTextureDX7::GetD3DCubeMapFace(IFXenum eFace)
{
	DWORD uFace = 0;

	switch(eFace)
	{
	case IFX_TEXTURE_CUBE_PX:
		uFace = DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP;
		break;
	case IFX_TEXTURE_CUBE_PY:
		uFace = DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP;
		break;
	case IFX_TEXTURE_CUBE_PZ:
		uFace = DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP;
		break;
	case IFX_TEXTURE_CUBE_NX:
		uFace = DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP;
		break;
	case IFX_TEXTURE_CUBE_NY:
		uFace = DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP;
		break;
	case IFX_TEXTURE_CUBE_NZ:
		uFace = DDSCAPS2_CUBEMAP_NEGATIVEZ | DDSCAPS2_CUBEMAP;
		break;
	}

	return uFace;
}

DDPIXELFORMAT CIFXDeviceTextureDX7::GetD3DTexFormat(IFXenum eFormat)
{
	DDPIXELFORMAT fmtReturn;
	memset(&fmtReturn, 0, sizeof(DDPIXELFORMAT));
	fmtReturn.dwSize = sizeof(DDPIXELFORMAT);
	fmtReturn.dwFlags = DDPF_RGB;
	fmtReturn.dwRGBBitCount = 32;
	fmtReturn.dwRGBAlphaBitMask		= 0xff000000;
	fmtReturn.dwRBitMask			= 0x00ff0000;
	fmtReturn.dwGBitMask			= 0x0000ff00;
	fmtReturn.dwBBitMask			= 0x000000ff;
	
	switch(eFormat)
	{
	case IFX_RGBA_8888:
		fmtReturn.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		fmtReturn.dwRGBBitCount = 32;
		fmtReturn.dwRGBAlphaBitMask		= 0xff000000;
		fmtReturn.dwRBitMask			= 0x00ff0000;
		fmtReturn.dwGBitMask			= 0x0000ff00;
		fmtReturn.dwBBitMask			= 0x000000ff;
		break;
	case IFX_RGBA_8880:
		fmtReturn.dwFlags = DDPF_RGB;
		fmtReturn.dwRGBBitCount = 32;
		fmtReturn.dwRGBAlphaBitMask		= 0x00000000;
		fmtReturn.dwRBitMask			= 0x00ff0000;
		fmtReturn.dwGBitMask			= 0x0000ff00;
		fmtReturn.dwBBitMask			= 0x000000ff;
		break;
	case IFX_RGBA_5650:
		fmtReturn.dwFlags = DDPF_RGB;
		fmtReturn.dwRGBBitCount = 16;
		fmtReturn.dwRGBAlphaBitMask		= 0x00000000;
		fmtReturn.dwRBitMask			= 0x0000f800;
		fmtReturn.dwGBitMask			= 0x000007e0;
		fmtReturn.dwBBitMask			= 0x0000001f;
		break;
	case IFX_RGBA_5551:
		fmtReturn.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		fmtReturn.dwRGBBitCount = 16;
		fmtReturn.dwRGBAlphaBitMask		= 0x00008000;
		fmtReturn.dwRBitMask			= 0x00007c00;
		fmtReturn.dwGBitMask			= 0x000003e0;
		fmtReturn.dwBBitMask			= 0x0000001f;
		break;
	case IFX_RGBA_5550:
		fmtReturn.dwFlags = DDPF_RGB;
		fmtReturn.dwRGBBitCount = 16;
		fmtReturn.dwRGBAlphaBitMask		= 0x00000000;
		fmtReturn.dwRBitMask			= 0x00007c00;
		fmtReturn.dwGBitMask			= 0x000003e0;
		fmtReturn.dwBBitMask			= 0x0000001f;
		break;
	case IFX_RGBA_4444:
		fmtReturn.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		fmtReturn.dwRGBBitCount = 16;
		fmtReturn.dwRGBAlphaBitMask		= 0x0000f000;
		fmtReturn.dwRBitMask			= 0x00000f00;
		fmtReturn.dwGBitMask			= 0x000000f0;
		fmtReturn.dwBBitMask			= 0x0000000f;
		break;
	default:
		IFXASSERTBOX(0, "Invalid IFX texture format specified!");
		break;
	}
	
	return fmtReturn;
}


// END OF FILE

