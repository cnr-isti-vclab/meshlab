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
// CIFXDirectX8.cpp
#define INITGUID
#include "CIFXDirectX8.h"

CIFXDirectX8* CIFXDirectX8::ms_pSingleton = 0;

//============================
// Factory Function
//============================
IFXRESULT IFXAPI_CALLTYPE CIFXDirectX8Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;

	if(ppUnk)
	{
		CIFXDirectX8* pPtr = CIFXDirectX8::GetSingleton();
		if(!pPtr)
		{
			pPtr = new CIFXDirectX8;
			if(pPtr)
			{
				rc = pPtr->Construct();
			}
			else
			{
				rc = IFX_E_OUT_OF_MEMORY;
			}
		}

		if(IFXSUCCESS(rc))
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

U32 CIFXDirectX8::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDirectX8::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		ms_pSingleton = NULL;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDirectX8::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXDirectX8)
		{
			*(IFXDirectX8**)ppInterface = (IFXDirectX8*) this;
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

CIFXDirectX8::CIFXDirectX8()
{
	m_refCount = 0;
}

CIFXDirectX8::~CIFXDirectX8()
{
	if(m_hD3D8Lib)
	{
		FreeLibrary(m_hD3D8Lib);
		m_hD3D8Lib = 0;
	}

	Direct3DCreate8 = 0;
	ms_pSingleton = 0;
}

IFXRESULT CIFXDirectX8::Construct()
{
	IFXRESULT rc = IFX_OK;

	m_hD3D8Lib = 0;
	Direct3DCreate8 = 0;
	ms_pSingleton = this;

	return rc;
}

CIFXDirectX8* IFXAPI CIFXDirectX8::GetSingleton()
{
	return ms_pSingleton;
}

IFXRESULT CIFXDirectX8::LoadDX8()
{
	IFXRESULT rc = IFX_OK;

	if(0 == m_hD3D8Lib)
	{
		m_hD3D8Lib = LoadLibrary(L"D3D8");
		if(0 == m_hD3D8Lib)
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	if(IFXSUCCESS(rc))
	{
		if(0 == Direct3DCreate8)
		{
			Direct3DCreate8 = (PFNDirect3DCreate8)
								GetProcAddress(	m_hD3D8Lib,
												"Direct3DCreate8");
		}

		if(0 == Direct3DCreate8)
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	return rc;
}

// END OF FILE

