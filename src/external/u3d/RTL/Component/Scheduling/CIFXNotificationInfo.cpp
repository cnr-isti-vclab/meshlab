//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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

/**
	@file	CIFXNotificationInfo.cpp

			This module defines the CIFXNotificationInfo class.  It is used to...
*/

#include "CIFXNotificationInfo.h"
#include "IFXTaskManager.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXNotificationInfo_Factory(IFXREFIID riid, void** ppv)
{
	CIFXNotificationInfo *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXNotificationInfo();

	if (!pObject)
	{
		// allocation failed
		*ppv = NULL;
		return IFX_E_OUT_OF_MEMORY;
	}

	IFXRESULT rc = pObject->QueryInterface(riid, ppv);

	if (rc != IFX_OK && allocated)
	{
		delete pObject;
		pObject = NULL;
	}
	return rc;
}

//---------------------------------------------------------------------
// PRIVATE constructor
//---------------------------------------------------------------------
CIFXNotificationInfo::CIFXNotificationInfo()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pOrigin = NULL;
	m_pNotificationData = NULL;
	m_pUserData = NULL;
	m_pTaskManager = NULL;
	m_taskHandle = IFXTASK_HANDLE_INVALID;

	m_type          = IFXNotificationType_Nil;
	m_id            = IFXNotificationId_Nil;
	m_keyFilter     = 0; // wild
	m_bitFilter     = 0; // wild
	m_pObjectFilter = NULL; // wild
	m_time          = 0;
	// m_name does not need to be initialized
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXNotificationInfo::~CIFXNotificationInfo()
{
	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pOrigin);
	IFXRELEASE(m_pTaskManager);

	IFXRELEASE(m_pObjectFilter);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXNotificationInfo::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXNotificationInfo::Release()
{
	--m_refcount;
	if (m_refcount == 0) {
		delete this;
		return 0;
	}
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::QueryInterface
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT rc = IFX_OK;

	if (ppv == NULL)
	{
		rc = IFX_E_INVALID_POINTER;
	}
	else if (riid == IID_IFXUnknown)
	{
		*ppv = (IFXUnknown*)this; 
		AddRef();
	}
	else if (riid == IID_IFXNotificationInfo)
	{
		*ppv = (IFXNotificationInfo*) this;
		AddRef();
	}
	else
	{
		*ppv = NULL;
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::Initialize(IFXCoreServices * pCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (m_initialized)
		rc = IFX_E_ALREADY_INITIALIZED;
	else if (!pCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		m_pCoreServices = pCoreServices;
		m_pCoreServices->AddRef();
	}

	if (IFXSUCCESS(rc))
		m_initialized = TRUE;
	else
	{
		IFXRELEASE(m_pCoreServices);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::SetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetOrigin(IFXUnknown * pObject)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pObject)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pOrigin);
		m_pOrigin = pObject;
		if (m_pOrigin)
			m_pOrigin->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetOrigin(IFXUnknown ** ppObject)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppObject)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppObject = m_pOrigin;
		if (*ppObject)
			(*ppObject)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::SetNotificationData
// NULL is valid
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetNotificationData(void * pData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		m_pNotificationData = pData;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetNotificationData
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetNotificationData(void ** ppData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
		*ppData = m_pNotificationData;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::SetType
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetType(IFXNotificationType type)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_type = type;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::GetType
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetType(IFXNotificationType *pType)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pType)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pType = m_type;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::SetId
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetId(IFXNotificationId id)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_id = id;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::GetId
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetId(IFXNotificationId *pId)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pId)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pId = m_id;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::SetName
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetName(IFXString name)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_name = name;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::GetName
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetName(IFXString &name)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		name = m_name;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::SetKeyFilter
//
// Zero keyFilter is wild
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetKeyFilter(U32 keyFilter)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_keyFilter = keyFilter;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::GetKeyFilter
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetKeyFilter(U32 *pKeyFilter)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pKeyFilter)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pKeyFilter = m_keyFilter;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::SetBitFilter
//
// Zero bitFilter is wild
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetBitFilter(U32 bitFilter)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_bitFilter = bitFilter;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::GetBitFilter
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetBitFilter(U32 *pBitFilter)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pBitFilter)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pBitFilter = m_bitFilter;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::SetObjectFilter
//
// NULL pObjectFilter is wild
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetObjectFilter(IFXUnknown * pObjectFilter)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pObjectFilter);
		m_pObjectFilter = pObjectFilter;
		if (m_pObjectFilter)
			m_pObjectFilter->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationInfo::GetObjectFilter
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetObjectFilter(IFXUnknown ** ppObjectFilter)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppObjectFilter)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppObjectFilter = m_pObjectFilter;
		if (*ppObjectFilter)
			(*ppObjectFilter)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetCoreServices
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetCoreServices(IFXCoreServices ** ppCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppCoreServices = m_pCoreServices;
		if (*ppCoreServices)
			(*ppCoreServices)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::SetTime
// timestamp for events
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::SetTime(U32 time)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_time = time;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetTime
// timestamp for events
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetTime(U32 *pTime)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTime)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pTime = m_time;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::MatchPattern
// Compares this task data to a pattern. A match is determined by
// this particular type of task data, and is used for sorting
// tasks within IFXTaskManager.
//
// if pPattern is NULL, matches everything
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::MatchPattern(IFXTaskData * pPattern, BOOL * pResult)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pResult)
		rc = IFX_E_INVALID_POINTER;

	IFXNotificationInfo * pNotificationInfo = NULL;

	// assume match is true, and try to disprove it
	BOOL match = TRUE;

	IFXNotificationId id = IFXNotificationId_Nil;
	IFXNotificationType type = IFXNotificationType_Nil;
	U32 keyFilter = 0;
	U32 bitFilter = 0;
	IFXUnknown * pObjectFilter = NULL;

	// if pPattern is NULL, we will always match
	if (pPattern)
	{
		if (IFXSUCCESS(rc))
		{
			rc = pPattern->QueryInterface(IID_IFXNotificationInfo, (void**) &pNotificationInfo);
		}

		if (IFXSUCCESS(rc) && match && m_type != IFXNotificationType_Nil)
		{
			rc = pNotificationInfo->GetType(&type);
			if (IFXSUCCESS(rc)) {
				if (type != m_type)
					match = FALSE;
			}
		}

		if (IFXSUCCESS(rc) && match && m_id != IFXNotificationId_Nil)
		{
			rc = pNotificationInfo->GetId(&id);
			if (IFXSUCCESS(rc)) {
				if (id != m_id)
					match = FALSE;
			}
		}

		if (IFXSUCCESS(rc) && match && m_keyFilter)
		{
			rc = pNotificationInfo->GetKeyFilter(&keyFilter);
			if (IFXSUCCESS(rc)) {
				if (keyFilter != m_keyFilter)
					match = FALSE;
			}
		}
		if (IFXSUCCESS(rc) && match && m_bitFilter)
		{
			rc = pNotificationInfo->GetBitFilter(&bitFilter);
			if (IFXSUCCESS(rc)) {
				if ((bitFilter & m_bitFilter) != 0)
					match = FALSE;
			}
		}
		if (IFXSUCCESS(rc) && match && m_pObjectFilter != NULL)
		{
			rc = pNotificationInfo->GetObjectFilter(&pObjectFilter);
			if (IFXSUCCESS(rc)) {
				if (pObjectFilter != m_pObjectFilter)
					match = FALSE;
			}
		}
	}

	IFXRELEASE(pObjectFilter);
	IFXRELEASE(pNotificationInfo);

	if (IFXSUCCESS(rc))
	{
		if (match)
			*pResult = TRUE;
		else
			*pResult = FALSE;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetSortKey
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationInfo::GetSortKey(U32 * pKey)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pKey)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pKey = m_id; // default
	}

	return rc;
}

IFXRESULT CIFXNotificationInfo::SetTaskManager(IFXTaskManager * pTaskManager)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pTaskManager);
		m_pTaskManager = pTaskManager;
		if (m_pTaskManager)
			m_pTaskManager->AddRef();
	}

	return rc;
}

IFXRESULT CIFXNotificationInfo::GetTaskManager(IFXTaskManager ** ppTaskManager)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppTaskManager)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppTaskManager = m_pTaskManager;
		if (*ppTaskManager)
			(*ppTaskManager)->AddRef();
	}

	return rc;
}

IFXRESULT CIFXNotificationInfo::SetTaskHandle(IFXTaskHandle taskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_taskHandle = taskHandle;
	}

	return rc;
}

IFXRESULT CIFXNotificationInfo::GetTaskHandle(IFXTaskHandle * pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pTaskHandle = m_taskHandle;
	}

	return rc;
}

IFXRESULT CIFXNotificationInfo::SetUserData(void * pUserData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_pUserData = pUserData;
	}

	return rc;
}

IFXRESULT CIFXNotificationInfo::GetUserData(void ** ppUserData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppUserData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppUserData = m_pUserData;
	}

	return rc;
}


