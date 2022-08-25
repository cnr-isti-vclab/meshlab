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
	@file	CIFXSchedulerInfo.cpp

			This module defines the CIFXSchedulerInfo class.  It is used to...
*/

#include "CIFXSchedulerInfo.h"
#include "IFXTaskManager.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXSchedulerInfo_Factory(IFXREFIID riid, void** ppv)
{
	CIFXSchedulerInfo *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXSchedulerInfo();

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
CIFXSchedulerInfo::CIFXSchedulerInfo()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pOrigin = NULL;
	m_pUserData = NULL;
	m_time = 0;
	m_priority = 0;
	m_pTaskManager = NULL;
	m_taskHandle = IFXTASK_HANDLE_INVALID;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXSchedulerInfo::~CIFXSchedulerInfo()
{
	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pOrigin);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXSchedulerInfo::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::Initialize(IFXCoreServices *pCoreServices)
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
	{
		m_initialized = TRUE;
	}
	else
	{
		IFXRELEASE(m_pCoreServices);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXSchedulerInfo::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXSchedulerInfo::Release()
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
IFXRESULT CIFXSchedulerInfo::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXSchedulerInfo)
	{
		*ppv = (IFXSchedulerInfo*) this;
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
// PUBLIC IFXTaskData::GetCoreServices
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::GetCoreServices(IFXCoreServices ** ppCoreServices)
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
// PUBLIC IFXTaskData::SetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::SetOrigin(IFXUnknown * pObject)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pObject)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pOrigin); // release any old pointer
		m_pOrigin = pObject;
		if (m_pOrigin)
			m_pOrigin->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::GetOrigin(IFXUnknown ** ppObject)
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
// PUBLIC IFXTaskData::SetUserData
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::SetUserData(void * pData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_pUserData = pData;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetUserData
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::GetUserData(void ** ppData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppData = m_pUserData;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::SetTime
// timestamp for events
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::SetTime(U32 time)
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
IFXRESULT CIFXSchedulerInfo::GetTime(U32 *pTime)
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
IFXRESULT CIFXSchedulerInfo::MatchPattern(IFXTaskData * pPattern, BOOL * pResult)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pResult)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pResult = TRUE; // always matches everything
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetSortKey
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::GetSortKey(U32 * pKey)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pKey)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pKey = m_priority; // default
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSchedulerInfo::SetPriority
// priority zero = last, priority FFFFFFFF = first
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::SetPriority(U32 priority)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_priority = priority;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSchedulerInfo::GetPriority
// priority zero = last, priority FFFFFFFF = first
//---------------------------------------------------------------------
IFXRESULT CIFXSchedulerInfo::GetPriority(U32 *pPriority)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pPriority)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pPriority = m_priority;
	}

	return rc;
}


IFXRESULT CIFXSchedulerInfo::SetTaskManager(IFXTaskManager * pTaskManager)
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

IFXRESULT CIFXSchedulerInfo::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

IFXRESULT CIFXSchedulerInfo::SetTaskHandle(IFXTaskHandle taskHandle)
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

IFXRESULT CIFXSchedulerInfo::GetTaskHandle(IFXTaskHandle * pTaskHandle)
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
