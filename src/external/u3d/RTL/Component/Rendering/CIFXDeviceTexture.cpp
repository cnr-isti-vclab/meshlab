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
// CIFXDeviceTexture.cpp
#include "IFXRenderPCH.h"
#include "CIFXDeviceTexture.h"
#include "IFXAutoRelease.h"
#include "IFXDids.h"
#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"
#include "IFXTextureImageTools.h"
#include "IFXReadBuffer.h"
#include "IFXModifier.h"
#include "IFXRenderServices.h"

U32 CIFXDeviceTexture::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTexture::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTexture::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexture)
		{
			*(CIFXDeviceTexture**)ppInterface = (CIFXDeviceTexture*) this;
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
IFXRESULT CIFXDeviceTexture::SetTexture(IFXTextureObject& rTexture)
{
	IFXRESULT rc = IFX_OK;

	IFXDECLARELOCAL(IFXModifier, pModifier);
	IFXDECLARELOCAL(IFXModifierChain, pModChain);
	IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);
	if (IFXSUCCESS(rc))
		rc = rTexture.QueryInterface(IID_IFXModifier, (void**)&pModifier);
	if (IFXSUCCESS(rc))
		rc = pModifier->GetModifierChain(&pModChain);
	if (IFXSUCCESS(rc))
		rc = pModChain->GetDataPacket(pDataPacket);

	U32 did = 0;
	STextureOutputInfo* pImageInfo = NULL;
	if (IFXSUCCESS(rc))
		rc = pDataPacket->GetDataElementIndex(DID_IFXTexture, did);
	if (IFXSUCCESS(rc))
		rc = pDataPacket->GetDataElement(did, (void**)&pImageInfo);

	if (rc == IFX_W_READING_NOT_COMPLETE) {
		pDataPacket->InvalidateDataElement(did);
		m_bNeedData = TRUE;
		m_bDirty = TRUE;
		return rc;
	}

	if (pImageInfo == NULL)
		return IFX_OK;

	if (IFXSUCCESS(rc) && m_bNeedData && (pImageInfo->m_pData == NULL))
	{
		pDataPacket->InvalidateDataElement(did);
		IFXRELEASE(pDataPacket);
		pModChain->GetDataPacket(pDataPacket);
		pDataPacket->GetDataElement(did, (void**)&pImageInfo);
	}

	IFXenum eVal = 0;
	IFXRUN(rc, rTexture.GetTextureType(&eVal));

	if(IFXSUCCESS(rc) && eVal != m_eType)
	{
		m_bNeedData = TRUE;
		m_bDirty = TRUE;
		rc = SetHWType(eVal);
	}

	//IFXRUN(rc, rTexture.GetRenderFormat(eVal));
	eVal = pImageInfo->eRenderFormat;

	if(IFXSUCCESS(rc))
	{
		if(eVal == IFX_FMT_DEFAULT)
		{
			IFXRenderServicesPtr spServices;
			spServices.Create(CID_IFXRenderServices, IID_IFXRenderServices);
			eVal = spServices->GetTextureRenderFormat();
		}

		eVal = GetCompatibleRenderFormat(eVal);
	}

	if(IFXSUCCESS(rc) && (eVal != m_eRenderFormat))
	{
		m_bNeedData = TRUE;
		m_bDirty = TRUE;
		rc = SetHWRenderFormat(eVal);
	}

	IFXRUN(rc, rTexture.GetMinFilterMode(&eVal));

	if(IFXSUCCESS(rc) && eVal != m_eMinFilter)
	{
		rc = SetHWMinFilter(eVal);
	}

	IFXRUN(rc, rTexture.GetMagFilterMode(&eVal));

	if(IFXSUCCESS(rc) && eVal != m_eMagFilter)
	{
		rc = SetHWMagFilter(eVal);
	}

	IFXRUN(rc, rTexture.GetMipMode(&eVal));

	if(IFXSUCCESS(rc) && eVal != m_eMipMode)
	{
		m_bNeedData = TRUE;
		m_bDirty = TRUE;
		rc = SetHWMipMode(eVal);
	}

	U32 uVal = 0;

//	IFXRUN(rc, rTexture.GetWidth(&uVal));
	uVal = pImageInfo->m_width;

	if(uVal == 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		if(uVal > m_RCaps.m_uMaxTextureWidth)
		{
			uVal = m_RCaps.m_uMaxTextureWidth;
		}
		else
		{
			U32 uTmp = m_RCaps.m_uMaxTextureWidth;
			while(!(uTmp & uVal))
			{
				uTmp >>= 1;
			}
			if((uVal & (uTmp >> 1)) && uTmp < m_RCaps.m_uMaxTextureWidth)
			{
				uTmp <<= 1;
			}
			uVal = uTmp;
		}
	}

	if(IFXSUCCESS(rc) && uVal != m_uWidth)
	{
		m_bNeedData = TRUE;
		m_bDirty = TRUE;
		rc = SetHWWidth(uVal);
	}

//	IFXRUN(rc, rTexture.GetHeight(&uVal));
	uVal = pImageInfo->m_height;

	if(uVal == 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		if(uVal > m_RCaps.m_uMaxTextureHeight)
		{
			uVal = m_RCaps.m_uMaxTextureHeight;
		}
		else
		{
			U32 uTmp = m_RCaps.m_uMaxTextureHeight;
			while(!(uTmp & uVal))
			{
				uTmp >>= 1;
			}
			if((uVal & (uTmp >> 1)) && uTmp < m_RCaps.m_uMaxTextureHeight)
			{
				uTmp <<= 1;
			}
			uVal = uTmp;
		}
	}

	if(IFXSUCCESS(rc) && uVal != m_uHeight)
	{
		m_bNeedData = TRUE;
		m_bDirty = TRUE;
		rc = SetHWHeight(uVal);
	}

	if(m_bDirty)
	{
		rc = SetHWTexture();
	}

	if(m_uLastSetId != rTexture.GetVersion())
	{
		m_bNeedData = TRUE;
		m_uLastSetId = rTexture.GetVersion();
	}

	if(m_bNeedData)
	{
		STextureOutputInfo tex;
		if(m_eType == IFX_TEXTURE_CUBE)
		{
			// Download cubemap textures
			for(IFXenum eFace = IFX_TEXTURE_CUBE_PX;
				IFXSUCCESS(rc) && (eFace <= IFX_TEXTURE_CUBE_NZ); eFace++)
			{
				m_bNeedData = TRUE;
				IFXTextureObject* pFaceTexture = 0;
				rc = rTexture.GetCubeMapTexture(eFace, &pFaceTexture);

				U32 uMaxMip = GetMaxRenderMipLevel();
				U32 i;
				for( i = 0; IFXSUCCESS(rc) && (i <= uMaxMip) && m_bNeedData; i++)
				{
					BOOL bInLock = FALSE;
					IFXRUN(rc, Lock(eFace, i, tex));
					if(IFXSUCCESS(rc))
					{
						bInLock = TRUE;
					}

					IFXRUN(rc, pFaceTexture->GetRenderImage(tex));

					if(bInLock)
					{
						rc = Unlock(eFace, i, tex);
					}
				}

				rc = pFaceTexture->PurgeRenderImage();

				IFXRELEASE(pFaceTexture);
			}

		}
		else
		{
			// Download regular texture
			U32 uMaxMip = GetMaxRenderMipLevel();
			U32 i;
			for( i = 0; IFXSUCCESS(rc) && (i <= uMaxMip) && m_bNeedData; i++)
			{
				BOOL bInLock = FALSE;
				IFXRUN(rc, Lock(m_eType, i, tex));
				if(IFXSUCCESS(rc))
				{
					bInLock = TRUE;
				}

				U32 fmt = 0;
				rTexture.GetFormat(&fmt);
				IFXDECLARELOCAL(IFXTextureImageTools, pTextureIT);
				IFXCreateComponent(CID_IFXTextureImageTools, IID_IFXTextureImageTools,(void**)&pTextureIT);
				pTextureIT->SetTexels(pImageInfo->m_width, pImageInfo->m_height, fmt, pImageInfo->m_pData);
				pTextureIT->CopyRenderImage(&tex);

				if(bInLock)
				{
					rc = Unlock(m_eType, i, tex);
				}
			}

			rc = rTexture.PurgeRenderImage();
		}
	}

	if(IFXSUCCESS(rc)) {
		m_bDirty = FALSE;
		m_bNeedData = FALSE;
	} else {
		m_bDirty = TRUE;
		m_bNeedData = TRUE;
	}

	return rc;
}

//=============================
// Protected Methods
//=============================

IFXRESULT CIFXDeviceTexture::SetDefaults()
{
	IFXRESULT rc = SetHWType(m_eType);

	IFXRUN(rc, SetHWRenderFormat(m_eRenderFormat));
	IFXRUN(rc, SetHWWidth(m_uWidth));
	IFXRUN(rc, SetHWHeight(m_uHeight));
	IFXRUN(rc, SetHWDepth(m_uDepth));
	IFXRUN(rc, SetHWMaxMipLevel(m_uMaxMipLevel));
	IFXRUN(rc, SetHWMinFilter(m_eMinFilter));
	IFXRUN(rc, SetHWMagFilter(m_eMagFilter));
	IFXRUN(rc, SetHWMipMode(m_eMipMode));
	IFXRUN(rc, SetHWDynamic(m_bDynamic));

	return rc;
}

IFXRESULT CIFXDeviceTexture::SetHWType(IFXenum eType)
{
	m_eType = eType;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWRenderFormat(IFXenum eFormat)
{
	m_eRenderFormat = eFormat;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWWidth(U32 uWidth)
{
	m_uWidth = uWidth;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWHeight(U32 uHeight)
{
	m_uHeight = uHeight;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWDepth(U32 uDepth)
{
	m_uDepth = uDepth;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWMaxMipLevel(U32 uMipLevel)
{
	m_uMaxMipLevel = uMipLevel;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWMinFilter(IFXenum eFilter)
{
	m_eMinFilter = eFilter;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWMagFilter(IFXenum eFilter)
{
	m_eMagFilter = eFilter;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWMipMode(IFXenum eMode)
{
	m_eMipMode = eMode;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWDynamic(BOOL bDyanmic)
{
	m_bDynamic = bDyanmic;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexture::SetHWTexture()
{
	return IFX_OK;
}

U32 CIFXDeviceTexture::GetMaxRenderMipLevel() const
{
	U32 uMipLevel = 0;

	if(m_eMipMode != IFX_NONE)
	{
		U32 uMaxSize = m_uWidth > m_uHeight ? m_uWidth : m_uHeight;
		while(uMaxSize >>= 1)
		{
			uMipLevel++;
		}

		if(uMipLevel > m_uMaxMipLevel)
		{
			uMipLevel = m_uMaxMipLevel;
		}
	}

	return uMipLevel;
}

IFXenum IFXAPI CIFXDeviceTexture::GetCompatibleRenderFormat(IFXenum eFormat)
{
	switch(eFormat)
	{
	case IFX_RGBA_8888:
		if(m_RCaps.m_bTex8888)
			return eFormat;
		else if(m_RCaps.m_bTex4444)
			return IFX_RGBA_4444;
		else
			return IFX_RGBA_5551;
		break;
	case IFX_RGBA_8880:
		if(m_RCaps.m_bTex8880)
			return eFormat;
		else if(m_RCaps.m_bTex8888)
			return IFX_RGBA_8888;
		else if(m_RCaps.m_bTex5650)
			return IFX_RGBA_5650;
		else if(m_RCaps.m_bTex5550)
			return IFX_RGBA_5550;
		else if(m_RCaps.m_bTex5551)
			return IFX_RGBA_5551;
		else
			return IFX_RGBA_4444;
		break;
	case IFX_RGBA_5650:
		if(m_RCaps.m_bTex5650)
			return eFormat;
		else if(m_RCaps.m_bTex5550)
			return IFX_RGBA_5550;
		else if(m_RCaps.m_bTex5551)
			return IFX_RGBA_5551;
		else if(m_RCaps.m_bTex8880)
			return IFX_RGBA_8880;
		else if(m_RCaps.m_bTex8888)
			return IFX_RGBA_8888;
		else
			return IFX_RGBA_4444;
		break;
	case IFX_RGBA_5550:
		if(m_RCaps.m_bTex5550)
			return eFormat;
		else if(m_RCaps.m_bTex5650)
			return IFX_RGBA_5650;
		else if(m_RCaps.m_bTex5551)
			return IFX_RGBA_5551;
		else if(m_RCaps.m_bTex8880)
			return IFX_RGBA_8880;
		else if(m_RCaps.m_bTex8888)
			return IFX_RGBA_8888;
		else
			return IFX_RGBA_4444;
		break;
	case IFX_RGBA_5551:
		if(m_RCaps.m_bTex5551)
			return eFormat;
		else if(m_RCaps.m_bTex4444)
			return IFX_RGBA_4444;
		else
			return IFX_RGBA_8888;
		break;
	case IFX_RGBA_4444:
		if(m_RCaps.m_bTex4444)
			return eFormat;
		else if(m_RCaps.m_bTex8888)
			return IFX_RGBA_8888;
		else
			return IFX_RGBA_5551;
		break;
	}

	return eFormat;
}


CIFXDeviceTexture::CIFXDeviceTexture()
{
	// EMPTY
}

CIFXDeviceTexture::~CIFXDeviceTexture()
{
	// EMPTY
}

IFXRESULT CIFXDeviceTexture::Construct()
{
	IFXRESULT rc = IFX_OK;

	CIFXDeviceBase::InitData();
	InitData();

	return rc;
}

void CIFXDeviceTexture::InitData()
{
	m_eType = IFX_TEXTURE_2D;
	m_eRenderFormat = IFX_RGBA_8888;
	m_uWidth = 0;
	m_uHeight = 0;
	m_uDepth = 0;
	m_uMaxMipLevel = 12;
	m_eMipMode = IFX_NONE;
	m_eMinFilter = IFX_LINEAR;
	m_eMagFilter = IFX_LINEAR;
	m_bNeedData = TRUE;
	m_bDynamic = FALSE;
}
