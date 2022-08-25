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
// CIFXDeviceTextureNULL.h
#include "IFXRenderPCHNULL.h"
#include "CIFXDeviceTextureNULL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureNULLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTextureNULL* pPtr = new CIFXDeviceTextureNULL;
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

U32 CIFXDeviceTextureNULL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTextureNULL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTextureNULL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexture)
		{
			*(CIFXDeviceTexture**)ppInterface = (CIFXDeviceTexture*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTextureNULL)
		{
			*(CIFXDeviceTextureNULL**)ppInterface = (CIFXDeviceTextureNULL*) this;
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
CIFXDeviceTextureNULL::CIFXDeviceTextureNULL()
{
	m_refCount = 0;
}

CIFXDeviceTextureNULL::~CIFXDeviceTextureNULL()
{
	IFXDELETE_ARRAY(m_pTexture);
}

IFXRESULT CIFXDeviceTextureNULL::Construct()
{
	IFXRESULT rc = CIFXDeviceTexture::Construct();
	
	InitData();
	
	return rc;
}

void CIFXDeviceTextureNULL::InitData()
{
	m_pDevice = 0;
	m_pTexture = 0;
	m_uPitch = 0;
	m_bNeedsRecreate = TRUE;
	m_bInLock = FALSE;
	m_uMaxMipLevel = 0;
}

IFXRESULT CIFXDeviceTextureNULL::Initialize(CIFXRenderDeviceNULLPtr& spDevice)
{
	IFXRESULT rc = IFX_OK;
	
	m_pDevice = spDevice.GetPointerNR();
	
	m_pDevice->GetCaps(m_RCaps);
	
	return rc;
}

IFXRESULT CIFXDeviceTextureNULL::SetHWTexture()
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

IFXRESULT CIFXDeviceTextureNULL::SetHWRenderFormat(IFXenum eFormat)
{
	m_eRenderFormat = eFormat;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTextureNULL::Lock(	IFXenum eType, 
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

	if(IFXSUCCESS(rc))
	{
		rTex.m_width = m_uWidth;
		rTex.m_height = m_uHeight;
		rTex.eChannelOrder = IFX_BGRA;
		rTex.eRenderFormat = m_eRenderFormat;
		rTex.m_pData = (U8*)m_pTexture;
		rTex.m_pitch = m_uPitch;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureNULL::Unlock(IFXenum eType, 
										U32 uMipLevel, 
										STextureOutputInfo& rTex )
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXDeviceTextureNULL::Create2DTexture()
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(m_pDevice, "Creating texture before device!");

	IFXDELETE_ARRAY(m_pTexture);

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

		U32 uPixelSize = 2;
		if(m_eRenderFormat == IFX_RGBA_8888 || m_eRenderFormat == IFX_RGBA_8880)
		{
			uPixelSize = 4;
		}

		m_pTexture = new U8[m_uWidth * m_uHeight * uPixelSize];
		m_uPitch = m_uWidth * uPixelSize;
	}

	return rc;
}

IFXRESULT CIFXDeviceTextureNULL::CreateCubeTexture()
{
	IFXRESULT rc = IFX_E_UNSUPPORTED;
	
	return rc;
}

// END OF FILE


