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
// CIFXVoidWrapper.cpp

#include "CIFXVoidWrapper.h"

IFXRESULT IFXAPI_CALLTYPE CIFXVoidWrapper_Factory( IFXREFIID intId, void** ppUnk )
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXVoidWrapper* pPtr = new CIFXVoidWrapper;
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

U32 CIFXVoidWrapper::AddRef()
{
	return ++m_refCount;
}

U32 CIFXVoidWrapper::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXVoidWrapper::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXVoidWrapper)
		{
			*(IFXVoidWrapper**)ppInterface = (IFXVoidWrapper*) this;
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

CIFXVoidWrapper::CIFXVoidWrapper()
{
	m_refCount = 0;
}

IFXRESULT CIFXVoidWrapper::Construct()
{
	IFXRESULT rc = IFX_OK;

	m_pvData = 0;

	return rc;
}

void CIFXVoidWrapper::SetData(void* pvData)
{
	m_pvData = pvData;
}

void* CIFXVoidWrapper::GetData() const
{
	return m_pvData;
}

// END OF FILE


