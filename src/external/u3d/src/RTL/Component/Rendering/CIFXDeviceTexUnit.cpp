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
// CIFXDeviceTexUnit.cpp
#include "IFXRenderPCH.h"
#include "CIFXDeviceTexUnit.h"

U32 CIFXDeviceTexUnit::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTexUnit::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTexUnit::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexUnit)
		{
			*(CIFXDeviceTexUnit**)ppInterface = (CIFXDeviceTexUnit*) this;
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
IFXRESULT CIFXDeviceTexUnit::SetTexCoordSet(U32 uTCSet)
{
	IFXRESULT rc = IFX_OK;

	if(m_uTexCoordSet != uTCSet)
	{
		rc = SetHWTexCoordSet(uTCSet);
	}

	return rc;
}

IFXRESULT CIFXDeviceTexUnit::SetTexUnit(IFXRenderTexUnit& rTexUnit)
{
	IFXRESULT rc = IFX_OK;
	
	CompareDevice(rc, m_eABlendFunc, rTexUnit.GetAlphaBlendFunc(), SetHWAlphaFunc);
	CompareDevice(rc, m_eAInput0, (rTexUnit.GetAlphaInput0()|IFX_ALPHA), SetHWAlphaInput0);
	CompareDevice(rc, m_eAInput1, (rTexUnit.GetAlphaInput1()|IFX_ALPHA), SetHWAlphaInput1);
	CompareDevice(rc, m_eAInterpSrc, ((rTexUnit.GetAlphaInterpolatorSource()|IFX_ALPHA)&~IFX_INVERSE), SetHWAlphaInterpolatorSource);
	CompareDevice(rc, m_eRGBBlendFunc, rTexUnit.GetRGBBlendFunc(), SetHWRGBBlendFunc);
	CompareDevice(rc, m_eRGBInput0, rTexUnit.GetRGBInput0(), SetHWRGBInput0);
	CompareDevice(rc, m_eRGBInput1, rTexUnit.GetRGBInput1(), SetHWRGBInput1);
	CompareDevice(rc, m_eRGBInterpSrc, ((rTexUnit.GetRGBInterpolatorSource()|IFX_ALPHA)&~IFX_INVERSE), SetHWRGBInterpolatorSource);
	CompareDevice(rc, m_eTexCoordGen, rTexUnit.GetTexCoordGen(), SetHWTexCoordGen);
	CompareDevice(rc, m_mTexMatrix, rTexUnit.GetTextureTransform(), SetHWTextureTransform);
	CompareDevice(rc, m_uTexId, rTexUnit.GetTextureId(), SetHWTextureId);
	CompareDevice(rc, m_bTexRepeatU, rTexUnit.GetTexRepeatU(), SetHWTexRepeatU);
	CompareDevice(rc, m_bTexRepeatV, rTexUnit.GetTexRepeatV(), SetHWTexRepeatV);
	CompareDevice(rc, m_bTexRepeatW, rTexUnit.GetTexRepeatW(), SetHWTexRepeatW);
	CompareDevice(rc, m_iTexCoordSet, rTexUnit.GetTextureCoordinateSet(), SetHWTextureCoordinateSet);
	
	if(IFXSUCCESS(rc) && m_bDirty)
	{
		m_bDirty = FALSE;
	}

	// We always need to run this so that the DX implementations
	// can grab the filtering and mipmode settings from the texture
	// and update as necessary.
	IFXRUN(rc, SetHWTexUnit());
	
	return rc;
}


//===========================
// Protected Methods
//===========================
IFXRESULT CIFXDeviceTexUnit::SetHWRGBInput0 (IFXenum eInput)
{
	m_eRGBInput0 = eInput;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWRGBInput1 (IFXenum eInput)
{
	m_eRGBInput1 = eInput;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWRGBInterpolatorSource (IFXenum eInput)
{
	m_eRGBInterpSrc = eInput;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWRGBBlendFunc (IFXenum eFunc)
{
	m_eRGBBlendFunc = eFunc;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWAlphaInput0 (IFXenum eInput)
{
	m_eAInput0 = eInput;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWAlphaInput1 (IFXenum eInput)
{
	m_eAInput1 = eInput;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWAlphaInterpolatorSource (IFXenum eInput)
{
	m_eAInterpSrc = eInput;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWAlphaFunc (IFXenum eFunc)
{
	m_eABlendFunc = eFunc;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTextureId (U32 uTexId)
{
	m_uTexId = uTexId;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTexCoordGen (IFXenum eGenMode)
{
	m_eTexCoordGen = eGenMode;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTexCoordSet(U32 uTCSet)
{
	m_uTexCoordSet = uTCSet;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTextureCoordinateSet(I32 iTexCoordSet)
{
	m_iTexCoordSet = iTexCoordSet;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTexRepeatU(BOOL bEnabled)
{
	m_bTexRepeatU = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTexRepeatV(BOOL bEnabled)
{
	m_bTexRepeatV = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTexRepeatW(BOOL bEnabled)
{
	m_bTexRepeatW = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTextureTransform (const IFXMatrix4x4 &mTexture)
{
	m_mTexMatrix = mTexture;
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetHWTexUnit()
{	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnit::SetDefaults()
{
	IFXRESULT rc = SetHWEnabled(m_bEnabled);

	IFXRUN(rc, SetHWAlphaFunc(m_eABlendFunc));
	IFXRUN(rc, SetHWAlphaInput0(m_eAInput0));
	IFXRUN(rc, SetHWAlphaInput1(m_eAInput1));
	IFXRUN(rc, SetHWAlphaInterpolatorSource(m_eAInterpSrc));
	IFXRUN(rc, SetHWRGBBlendFunc(m_eRGBBlendFunc));
	IFXRUN(rc, SetHWRGBInput0(m_eRGBInput0));
	IFXRUN(rc, SetHWRGBInput1(m_eRGBInput1));
	IFXRUN(rc, SetHWRGBInterpolatorSource(m_eRGBInterpSrc));
	IFXRUN(rc, SetHWTexCoordGen(m_eTexCoordGen));
	IFXRUN(rc, SetHWTextureTransform(m_mTexMatrix));
	IFXRUN(rc, SetHWTextureId(m_uTexId));
	IFXRUN(rc, SetHWTexRepeatU(m_bTexRepeatU));
	IFXRUN(rc, SetHWTexRepeatV(m_bTexRepeatV));
	IFXRUN(rc, SetHWTexRepeatW(m_bTexRepeatW));
	IFXRUN(rc, SetHWTexCoordSet(m_uTexCoordSet));
	IFXRUN(rc, SetHWTexUnit());

	return rc;
}

IFXRESULT CIFXDeviceTexUnit::SetTextureId(U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	if(m_uTexId != uTexId)
	{
		MakeDeviceDirty();
		rc = SetHWTextureId(uTexId);
	}

	return rc;
}

// END OF FILE


