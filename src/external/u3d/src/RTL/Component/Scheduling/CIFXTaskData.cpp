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
	@file	CIFXTaskData.cpp

			This module defines the CIFXTaskData class.  It is used to...
*/

#include "CIFXTaskData.h"
#include "IFXTaskManager.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXTaskData_Factory(IFXREFIID riid, void** ppv)
{
	CIFXTaskData *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXTaskData();

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
CIFXTaskData::CIFXTaskData()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pOrigin = NULL;
	m_pUserData = NULL;
	m_time = 0;

	m_pTaskManager = NULL;
	m_taskHandle = IFXTASK_HANDLE_INVALID;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXTaskData::~CIFXTaskData()
{
	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pOrigin);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXTaskData::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXTaskData::Release()
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
IFXRESULT CIFXTaskData::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT rc = IFX_OK;

	if (ppv == NULL)
	{
		rc = IFX_E_INVALID_POINTER;
	}
	else if (riid == IID_IFXUnknown)
	{
		*ppv = this; // do not typecast to IFXUnknown, multiple inheritance confuses the compiler
		AddRef();
	}
	else if (riid == IID_IFXTaskData)
	{
		*ppv = (IFXTaskData*) this;
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
IFXRESULT CIFXTaskData::Initialize(IFXCoreServices * pCoreServices)
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
// PUBLIC IFXTaskData::SetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXTaskData::SetOrigin(IFXUnknown * pObject)
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
IFXRESULT CIFXTaskData::GetOrigin(IFXUnknown ** ppObject)
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
IFXRESULT CIFXTaskData::SetUserData(void * pData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		m_pUserData = pData;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetUserData
//---------------------------------------------------------------------
IFXRESULT CIFXTaskData::GetUserData(void ** ppData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
		*ppData = m_pUserData;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetCoreServices
//---------------------------------------------------------------------
IFXRESULT CIFXTaskData::GetCoreServices(IFXCoreServices ** ppCoreServices)
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
IFXRESULT CIFXTaskData::SetTime(U32 time)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_time = 0;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetTime
// timestamp for events
//---------------------------------------------------------------------
IFXRESULT CIFXTaskData::GetTime(U32 *pTime)
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
// pPattern NULL is always a match
//---------------------------------------------------------------------
IFXRESULT CIFXTaskData::MatchPattern(IFXTaskData * pPattern, BOOL * pResult)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pResult)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pResult = TRUE; // default, matches every pattern
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetSortKey
//---------------------------------------------------------------------
IFXRESULT CIFXTaskData::GetSortKey(U32 * pKey)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pKey)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pKey = 0; // default, no sort ordering
	}

	return rc;
}


IFXRESULT CIFXTaskData::SetTaskManager(IFXTaskManager * pTaskManager)
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

IFXRESULT CIFXTaskData::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

IFXRESULT CIFXTaskData::SetTaskHandle(IFXTaskHandle taskHandle)
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

IFXRESULT CIFXTaskData::GetTaskHandle(IFXTaskHandle * pTaskHandle)
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
