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
// CIFXIDManager.cpp
#include "CIFXIDManager.h"

//=============================
// Factory Function
//=============================
IFXRESULT IFXAPI_CALLTYPE CIFXIDManagerFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXIDManager* pPtr = new CIFXIDManager;
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

U32 CIFXIDManager::AddRef()
{
	return ++m_refCount;
}

U32 CIFXIDManager::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXIDManager::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXIDManager)
		{
			*(IFXIDManager**)ppInterface = (IFXIDManager*) this;
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

//==============================
// IFXIDManager Methods
//==============================
CIFXIDManager::CIFXIDManager()
{
	m_refCount = 0;
}

CIFXIDManager::~CIFXIDManager()
{
	Clear();
}

IFXRESULT CIFXIDManager::Construct()
{
	IFXRESULT rc = IFX_OK;

	m_pIDStack = 0;
	m_uNextId = 1;

	return rc;
}

IFXRESULT CIFXIDManager::Clear()
{
	IFXDELETE(m_pIDStack);
	m_uNextId = 1;

	return IFX_OK;
}

IFXRESULT CIFXIDManager::GetId(U32& uOutId)
{
	IFXRESULT rc = IFX_OK;

	if(m_pIDStack)
	{
		rc = Pop(uOutId);
	}
	else
	{
		uOutId = m_uNextId++;
	}

	return rc;
}

IFXRESULT CIFXIDManager::ReleaseId(U32 uInId)
{
	IFXRESULT rc = IFX_OK;

	if(0 == uInId || uInId >= m_uNextId)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
#ifdef IFX_RECYCLE_IDS
		RemoveIdFromStack(uInId);
		if(uInId == (m_uNextId-1))
		{
			m_uNextId--;
		}
		else
		{
			rc = Push(uInId);
		}
#endif // IFX_RECYCLE_IDS
	}

	return rc;
}

//==============================
// CIFXIDManager Methods
//==============================
IFXRESULT CIFXIDManager::Pop(U32& uOutId)
{
	IFXRESULT rc = IFX_OK;

	if(m_pIDStack)
	{
		CIFXIDStack* pId = m_pIDStack;
		m_pIDStack = pId->m_pNext;
		pId->m_pNext = 0;

		uOutId = pId->m_uId;
		IFXDELETE(pId);
	}
	else
	{
		rc = IFX_E_CANNOT_FIND;
	}

	return rc;
}

IFXRESULT CIFXIDManager::Push(U32 uInId)
{
	IFXRESULT rc = IFX_OK;

	CIFXIDStack* pId = new CIFXIDStack;
	if(pId)
	{
		pId->m_uId = uInId;
		pId->m_pNext = m_pIDStack;
		m_pIDStack = pId;
		pId = 0;
	}
	else
	{
		rc = IFX_E_OUT_OF_MEMORY;
	}

	return rc;
}

IFXRESULT CIFXIDManager::RemoveIdFromStack(U32 uInId)
{
	IFXRESULT rc = IFX_E_CANNOT_FIND;

	BOOL bFound = FALSE;
	CIFXIDStack* pId = m_pIDStack;
	CIFXIDStack* pLastId = m_pIDStack;
	
	if(m_pIDStack)
	{
		// First Check Stack pointer
		if(m_pIDStack->m_uId == uInId)
		{
			bFound = TRUE;
		}
		else
		{
			// Then check the rest of the stack
			pId = pId->m_pNext;

			while(pId && FALSE == bFound)
			{
				if(pId->m_uId != uInId)
				{
					pLastId = pId;
					pId = pId->m_pNext;
				}
				else
				{
					bFound = TRUE;
				}
			}
		}
	}

	if(bFound && pId)
	{
		rc = IFX_OK;
		if(pId == m_pIDStack)
		{
			m_pIDStack = m_pIDStack->m_pNext;
			pId->m_pNext = 0;
		}
		else
		{
			pLastId->m_pNext = pId->m_pNext;
			pId->m_pNext = 0;
		}

		IFXDELETE(pId);
	}

	return rc;
}

void CIFXIDManager::OutputAllocatedIds()
{
#ifdef _DEBUG
#ifdef IFX_RECYCLE_IDS
	IFXRESULT rc = IFX_OK;

	U32 i;
	for( i = 1; i < m_uNextId; i++)
	{
		rc = RemoveIdFromStack(i);

		if(IFXFAILURE(rc))
		{
			IFXTRACE_GENERIC("\tObject %3d Still Allocated!\n", i);
		}
	}
#endif // IFX_RECYCLE_IDS
#endif // _DEBUG
}

//========================
// CIFXIDStack Methods
//========================
CIFXIDStack::CIFXIDStack()
{
	m_uId = 0;
	m_pNext = 0;
}

CIFXIDStack::~CIFXIDStack()
{
	IFXDELETE(m_pNext);
}



