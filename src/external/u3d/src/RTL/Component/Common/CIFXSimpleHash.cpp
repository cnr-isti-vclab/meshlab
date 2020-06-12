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
// CIFXSimpleHash.cpp

#include "CIFXSimpleHash.h"

//==============================
// Factory Function
//==============================
IFXRESULT IFXAPI_CALLTYPE CIFXSimpleHashFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXSimpleHash* pPtr = new CIFXSimpleHash;
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

U32 CIFXSimpleHash::AddRef()
{
	return ++m_refCount;
}

U32 CIFXSimpleHash::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXSimpleHash::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXSimpleHash)
		{
			*(IFXSimpleHash**)ppInterface = (IFXSimpleHash*) this;
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

CIFXSimpleHash::CIFXSimpleHash()
{
	m_refCount = 0;
}

CIFXSimpleHash::~CIFXSimpleHash()
{
	IFXDELETE_ARRAY(m_pTable);
}

IFXRESULT CIFXSimpleHash::Construct()
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_pTable = 0;
		m_uTableSize = 0;
		m_uHashMask = 0;
		m_rcInitialized = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::Clear()
{
	IFXRESULT rc = IFX_OK;

	IFXDELETE_ARRAY(m_pTable);

	if(m_uTableSize)
	{
		m_pTable = new CIFXSimpleHashData[m_uTableSize];
		if(0 == m_pTable)
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	return rc;
}

// This will overwrite any stored data!
IFXRESULT CIFXSimpleHash::Initialize(U32 uTableSize)
{
	IFXRESULT rc = IFX_OK;

	// Table size MUST be a power of 2!
	// Maximum size is 65536.
	U32 uNewSize = 0x10000;
	while(uNewSize && (0 == (uTableSize & uNewSize)))
	{
		uNewSize >>= 1;
	}

	if(uNewSize < 2)
	{
		uNewSize = 2;
	}

	if(IFXSUCCESS(rc))
	{
		m_uTableSize = uNewSize;
		m_uHashMask = uNewSize-1;

		rc = Clear();
	}

	if(IFXSUCCESS(rc))
	{
		m_rcInitialized = IFX_OK;
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::AddData(U32 uId, IFXUnknownPtr& spUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		rc = AddData(uId, spUnk.GetPointerNR());
	}

	return rc;
}


IFXRESULT CIFXSimpleHash::AddData(U32 uId, IFXUnknown* pUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(0 == pUnk)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc))
	{
		CIFXSimpleHashData* pData = FindData(uId);
		if(pData)
		{
			pData->m_pUnk = pUnk;
		}
		else
		{
			pData = &m_pTable[Hash(uId)];

			while(pData->m_pUnk.IsValid())
			{
				if(!pData->m_pNext)
				{
					CIFXSimpleHashData* pNewData = new CIFXSimpleHashData;
					if(NULL == pNewData)
					{
						rc = IFX_E_OUT_OF_MEMORY;
						break;
					}
					else
					{
						pNewData->m_pPrev = pData;
						pData->m_pNext = pNewData;
					}
				}

				pData = pData->m_pNext;
			}

			if(IFXSUCCESS(rc))
			{
				pData->m_pUnk = pUnk;
				pData->m_uId = uId;
			}
		}

	}

	return rc;
}

IFXRESULT CIFXSimpleHash::GetData(U32 uId, IFXUnknownPtr& spUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		IFXUnknown* pUnk = 0;
		if(IFXSUCCESS(rc = GetData(uId, pUnk)))
		{
			spUnk = pUnk;
			pUnk->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::GetData(U32 uId, IFXUnknown*& pUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		CIFXSimpleHashData* pData = FindData(uId);
		rc = IFX_E_CANNOT_FIND;

		if(pData)
		{
			if(pData->m_pUnk.IsValid())
			{
				pUnk = pData->m_pUnk.GetPointerAR();
				rc = IFX_OK;
			}
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::GetLowestId(U32& uId, IFXUnknownPtr& spUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		IFXUnknown* pUnk = 0;
		rc = GetLowestId(uId, pUnk);
		if(IFXSUCCESS(rc))
		{
			spUnk = pUnk;
			pUnk->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::GetLowestId(U32& uId, IFXUnknown*& pUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		rc = IFX_E_CANNOT_FIND;
		U32 uLowId = FindLowestId();
		CIFXSimpleHashData* pData = FindData(uLowId);

		if(pData)
		{
			if(pData->m_pUnk.IsValid())
			{
				uId = uLowId;
				pUnk = pData->m_pUnk.GetPointerAR();
				rc = IFX_OK;
			}
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::ExtractData(U32 uId, IFXUnknownPtr& spUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		IFXUnknown* pUnk = 0;
		rc = ExtractData(uId, pUnk);
		if(IFXSUCCESS(rc))
		{
			spUnk = pUnk;
			pUnk->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::ExtractData(U32 uId, IFXUnknown*& pUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		CIFXSimpleHashData* pData = FindData(uId);
		rc = IFX_E_CANNOT_FIND;

		if(pData)
		{
			if(pData->m_pUnk.IsValid())
			{
				pUnk = pData->m_pUnk.GetPointerAR();
				pData->RemoveFromTable();
				rc = IFX_OK;
			}
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::ExtractLowestId(U32& uId, IFXUnknownPtr& spUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		IFXUnknown* pUnk = 0;
		rc = ExtractLowestId(uId, pUnk);
		if(IFXSUCCESS(rc))
		{
			spUnk = pUnk;
			pUnk->Release();
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::ExtractLowestId(U32& uId, IFXUnknown*& pUnk)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		rc = IFX_E_CANNOT_FIND;
		U32 uLowId = FindLowestId();
		CIFXSimpleHashData* pData = FindData(uLowId);

		if(pData)
		{
			if(pData->m_pUnk.IsValid())
			{
				uId = uLowId;
				pUnk = pData->m_pUnk.GetPointerAR();
				pData->RemoveFromTable();
				rc = IFX_OK;
			}
		}
	}

	return rc;
}

IFXRESULT CIFXSimpleHash::RemoveData(U32 uId)
{
	IFXRESULT rc = m_rcInitialized;

	if(IFXSUCCESS(rc))
	{
		CIFXSimpleHashData* pData = FindData(uId);
		rc = IFX_E_CANNOT_FIND;

		if(pData)
		{
			pData->RemoveFromTable();
			rc = IFX_OK;
		}
	}

	return rc;
}

CIFXSimpleHashData* CIFXSimpleHash::FindData(U32 uId)
{
	if(0 == m_pTable)
	{
		return 0;
	}

	CIFXSimpleHashData* pData = &m_pTable[Hash(uId)];

	while(pData)
	{
		if(pData->m_uId == uId)
		{
			break;
		}

		pData = pData->m_pNext;
	}

	return pData;
}

U32 CIFXSimpleHash::FindLowestId()
{
	if(0 == m_pTable)
	{
		return 0;
	}

	U32 uLowestId = 0xffffffff;

	CIFXSimpleHashData* pData = 0;

	U32 i;
	for( i = 0; i < m_uTableSize; i++)
	{
		pData = &m_pTable[i];

		while(pData)
		{
			if(pData->m_pUnk.IsValid())
			{
				if(pData->m_uId < uLowestId)
				{
					uLowestId = pData->m_uId;
				}
			}
			pData = pData->m_pNext;
		}
	}

	return uLowestId;
}

//==============================
// CIFXSimpleHashData Methods
//==============================
void CIFXSimpleHashData::RemoveFromTable()
{
	if( (0 == m_pPrev) && (0 == m_pNext) )
	{
		m_pUnk = 0;
		m_uId = 0;
	}
	else if( 0 == m_pPrev )
	{
		m_uId = m_pNext->m_uId;
		m_pUnk = m_pNext->m_pUnk;
		m_pNext->RemoveFromTable();
	}
	else if( 0 == m_pNext )
	{
		m_uId = 0;
		m_pUnk = 0;
		m_pPrev->m_pNext = 0;
		m_pPrev = 0;
		
		delete this;
	}
	else
	{
		m_uId = 0;
		m_pUnk = 0;
		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;

		m_pPrev = 0;
		m_pNext = 0;

		delete this;
	}
}
