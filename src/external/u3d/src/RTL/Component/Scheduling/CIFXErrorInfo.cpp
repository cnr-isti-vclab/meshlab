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
	@file	CIFXErrorInfo.cpp

			This module defines the CIFXErrorInfo class.  It is used to...
*/

#include "CIFXErrorInfo.h"
#include "IFXTaskManager.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXErrorInfo_Factory(IFXREFIID riid, void** ppv)
{
	CIFXErrorInfo *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXErrorInfo();

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
CIFXErrorInfo::CIFXErrorInfo()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pOrigin = NULL;
	m_pUserData = NULL;
	m_time = 0;

	m_pErrorTask = NULL;
	m_errorTaskHandle = IFXTASK_HANDLE_INVALID;

	m_pUserData = NULL;
	m_pTaskManager = NULL;
	m_taskHandle = IFXTASK_HANDLE_INVALID;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXErrorInfo::~CIFXErrorInfo()
{
	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pOrigin);
	IFXRELEASE(m_pErrorTask);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXErrorInfo::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXErrorInfo::Release()
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
IFXRESULT CIFXErrorInfo::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT rc = IFX_OK;

	if (ppv == NULL)
	{
		rc = IFX_E_INVALID_POINTER;
	}
	else if (riid == IID_IFXUnknown)
	{
		*ppv = (IFXUnknown*) this; 
		AddRef();
	}
	else if (riid == IID_IFXErrorInfo)
	{
		*ppv = (IFXErrorInfo*) this;
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
// PUBLIC IFXErrorInfo::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::Initialize(IFXCoreServices *pCoreServices)
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
// PUBLIC IFXTaskData::GetCoreServices
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::GetCoreServices(IFXCoreServices ** ppCoreServices)
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
IFXRESULT CIFXErrorInfo::SetOrigin(IFXUnknown * pObject)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

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
IFXRESULT CIFXErrorInfo::GetOrigin(IFXUnknown ** ppObject)
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
// PUBLIC IFXErrorInfo::SetErrorTask
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::SetErrorTask(IFXTask * pErrorTask)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pErrorTask);
		m_pErrorTask = pErrorTask;
		if (m_pErrorTask)
			m_pErrorTask->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXErrorInfo::GetErrorTask
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::GetErrorTask(IFXTask ** ppErrorTask)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppErrorTask)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppErrorTask = m_pErrorTask;
		if (*ppErrorTask)
			(*ppErrorTask)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXErrorInfo::SetErrorTaskHandle
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::SetErrorTaskHandle(IFXTaskHandle errorTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		m_errorTaskHandle = errorTaskHandle;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXErrorInfo::GetErrorTaskHandle
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::GetErrorTaskHandle(IFXTaskHandle *pErrorTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pErrorTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
		*pErrorTaskHandle = m_errorTaskHandle;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXErrorInfo::SetErrorCode
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::SetErrorCode(IFXRESULT errorCode)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		m_errorCode = errorCode;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXErrorInfo::GetErrorCode
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::GetErrorCode(IFXRESULT *pErrorCode)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pErrorCode)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
		*pErrorCode = m_errorCode;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::SetTime
// timestamp for events
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::SetTime(U32 time)
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
IFXRESULT CIFXErrorInfo::GetTime(U32 *pTime)
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
IFXRESULT CIFXErrorInfo::MatchPattern(IFXTaskData * pPattern, BOOL * pResult)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pResult)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXErrorInfo * pErrorInfo = NULL;
		if (!pPattern)
		{
			*pResult = TRUE;
		}
		else
		{
			rc = pPattern->QueryInterface(IID_IFXErrorInfo, (void**) &pErrorInfo);
			if (IFXSUCCESS(rc))
			{
				IFXTaskHandle handle = 0;

				rc = pErrorInfo->GetTaskHandle(&handle);
				if (IFXSUCCESS(rc))
				{
					if (m_taskHandle == handle)
						*pResult = TRUE;
					else
						*pResult = FALSE;
				}
			}
		}
		IFXRELEASE(pErrorInfo);
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetSortKey
//---------------------------------------------------------------------
IFXRESULT CIFXErrorInfo::GetSortKey(U32 * pKey)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pKey)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pKey = m_taskHandle; // default
	}

	return rc;
}

IFXRESULT CIFXErrorInfo::SetTaskManager(IFXTaskManager * pTaskManager)
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

IFXRESULT CIFXErrorInfo::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

IFXRESULT CIFXErrorInfo::SetTaskHandle(IFXTaskHandle taskHandle)
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

IFXRESULT CIFXErrorInfo::GetTaskHandle(IFXTaskHandle * pTaskHandle)
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

IFXRESULT CIFXErrorInfo::SetUserData(void * pUserData)
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

IFXRESULT CIFXErrorInfo::GetUserData(void ** ppUserData)
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
