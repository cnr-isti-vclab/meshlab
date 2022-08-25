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
// CIFXDirectX7.cpp

#define INITGUID
#include "CIFXDirectX7.h"

CIFXDirectX7* CIFXDirectX7::ms_pSingleton = 0;

//============================
// Factory Function
//============================
IFXRESULT IFXAPI_CALLTYPE CIFXDirectX7Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;

	if(ppUnk)
	{
		CIFXDirectX7* pPtr = CIFXDirectX7::GetSingleton();
		if(!pPtr)
		{
			pPtr = new CIFXDirectX7;
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

U32 CIFXDirectX7::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDirectX7::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		ms_pSingleton = NULL;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDirectX7::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXDirectX7)
		{
			*(IFXDirectX7**)ppInterface = (IFXDirectX7*) this;
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

CIFXDirectX7::CIFXDirectX7()
{
	m_refCount = 0;
}

CIFXDirectX7::~CIFXDirectX7()
{
	if(m_hDDrawLib)
	{
		FreeLibrary(m_hDDrawLib);
		m_hDDrawLib = 0;
	}

	m_DirectDrawEnumerateEx = 0;
	m_DirectDrawCreateEx = 0;
	ms_pSingleton = 0;
}

IFXRESULT CIFXDirectX7::Construct()
{
	IFXRESULT rc = IFX_OK;

	m_hDDrawLib = 0;
	m_DirectDrawEnumerateEx = 0;
	m_DirectDrawCreateEx = 0;
	ms_pSingleton = this;

	return rc;
}

CIFXDirectX7* IFXAPI CIFXDirectX7::GetSingleton()
{
	return ms_pSingleton;
}

IFXRESULT CIFXDirectX7::LoadDX7()
{
	IFXRESULT rc = IFX_OK;

	if(0 == m_hDDrawLib)
	{
		m_hDDrawLib = LoadLibrary(L"DDRAW");
		if(0 == m_hDDrawLib)
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	if(IFXSUCCESS(rc))
	{
		if(0 == m_DirectDrawEnumerateEx)
		{
#ifdef UNICODE
			m_DirectDrawEnumerateEx = (PFNDirectDrawEnumerateEx) GetProcAddress(m_hDDrawLib, "DirectDrawEnumerateExW");
#else // !UNICODE
			m_DirectDrawEnumerateEx = (PFNDirectDrawEnumerateEx) GetProcAddress(m_hDDrawLib, "DirectDrawEnumerateExA");
#endif // UNICODE
		}

		if(0 == m_DirectDrawEnumerateEx)
		{
			rc = IFX_E_UNSUPPORTED;
		}

		if(0 == m_DirectDrawCreateEx)
		{
			m_DirectDrawCreateEx = (PFNDirectDrawCreateEx) GetProcAddress(m_hDDrawLib, "DirectDrawCreateEx");
		}

		if(0 == m_DirectDrawCreateEx)
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	return rc;
}

// END OF FILE

