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
// CIFXDeviceTexUnitNULL.h
#include "IFXRenderPCHNULL.h"
#include "CIFXDeviceTexUnitNULL.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitNULLFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTexUnitNULL* pPtr = new CIFXDeviceTexUnitNULL;
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

U32 CIFXDeviceTexUnitNULL::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTexUnitNULL::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTexUnitNULL::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexUnit)
		{
			*(CIFXDeviceTexUnit**)ppInterface = (CIFXDeviceTexUnit*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTexUnitNULL)
		{
			*(CIFXDeviceTexUnitNULL**)ppInterface = (CIFXDeviceTexUnitNULL*) this;
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
CIFXDeviceTexUnitNULL::CIFXDeviceTexUnitNULL()
{
	m_refCount = 0;
}

CIFXDeviceTexUnitNULL::~CIFXDeviceTexUnitNULL()
{
	m_pDevice = 0;
}

IFXRESULT CIFXDeviceTexUnitNULL::Construct()
{
	IFXRESULT rc = CIFXDeviceTexUnit::Construct();
	
	InitData();
	
	return rc;
}

void CIFXDeviceTexUnitNULL::InitData()
{
	m_pDevice = 0;
	m_uTexUnitNum = 0;

	m_eTexMipMode = 0xffff;
	m_eTexMinFilter = 0xffff;
	m_eTexMagFilter = 0xffff;
}

IFXRESULT CIFXDeviceTexUnitNULL::Initialize(U32 uTexUnitNum, CIFXRenderDeviceNULLPtr& spDevice, BOOL bHW)
{
	IFXRESULT rc = IFX_OK;
	
	m_pDevice = spDevice.GetPointerNR();

	m_uTexUnitNum = uTexUnitNum;

	if(bHW)
	{
		rc = SetDefaults();
	}
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWEnabled (BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bEnabled = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWRGBInput0 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	m_eRGBInput0 = eInput;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWRGBInput1 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	m_eRGBInput1 = eInput;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWRGBInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_eRGBInterpSrc = (eInput | IFX_ALPHA) & ~IFX_INVERSE;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWRGBBlendFunc (IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;
	
	m_eRGBBlendFunc = eFunc;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWAlphaInput0 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	m_eAInput0 = eInput;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWAlphaInput1 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	m_eAInput1 = eInput;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWAlphaInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_eAInterpSrc = (eInput | IFX_ALPHA) & ~IFX_INVERSE;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWAlphaFunc (IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;
	
	m_eABlendFunc = eFunc;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTextureId (U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	m_uTexId = uTexId;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTexCoordGen(IFXenum eGenMode)
{
	IFXRESULT rc = IFX_OK;

	m_eTexCoordGen = eGenMode;

	return rc;	
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTextureTransform(const IFXMatrix4x4& mTrans)
{
	IFXRESULT rc = IFX_OK;

	if ( &m_mTexMatrix != &mTrans )
		m_mTexMatrix = mTrans;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTexRepeatU(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatU = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTexRepeatV(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatV = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTexRepeatW(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatW = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWMinFilter(IFXenum eMinFilter)
{
	IFXRESULT rc = IFX_OK;

	m_eTexMinFilter = eMinFilter;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWMagFilter(IFXenum eMagFilter)
{
	IFXRESULT rc = IFX_OK;

	m_eTexMagFilter = eMagFilter;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWMipMode(IFXenum eMipMode)
{
	IFXRESULT rc = IFX_OK;

	m_eTexMipMode = eMipMode;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTexUnit()
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitNULL::SetHWTexCoordSet(U32 uTCSet)
{
	IFXRESULT rc = IFX_OK;

	return rc;
}

CIFXDeviceTextureNULL* CIFXDeviceTexUnitNULL::GetTexture(U32 uTexId)
{
	CIFXDeviceTextureNULL* pTex = 0;
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		IFXUnknown* pUnk = 0;
		rc = m_pDevice->GetTextures()->GetData(uTexId, pUnk);

		IFXRUN(rc, pUnk->QueryInterface(CID_IFXDeviceTextureNULL, (void**)&pTex));

		IFXRELEASE(pUnk);
	}

	return pTex;
}

