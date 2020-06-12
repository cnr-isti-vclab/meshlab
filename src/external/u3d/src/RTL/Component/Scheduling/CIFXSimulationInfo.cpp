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
	@file	CIFXSimulationInfo.cpp

			This module defines the CIFXSimulationInfo class.  It is used to...
*/

#include "CIFXSimulationInfo.h"
#include "IFXTaskManager.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXSimulationInfo_Factory(IFXREFIID riid, void** ppv)
{
	CIFXSimulationInfo *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXSimulationInfo();

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
CIFXSimulationInfo::CIFXSimulationInfo()
{
	m_refcount = 0;

	m_initialized = FALSE;
	m_pCoreServices = NULL;

	m_pOrigin = NULL;
	m_pUserData = NULL;

	m_pObject = NULL;
	m_time = 0;
	m_priority = 0;

	m_pTaskManager = NULL;
	m_taskHandle = IFXTASK_HANDLE_INVALID;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXSimulationInfo::~CIFXSimulationInfo()
{
	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pOrigin);
	IFXRELEASE(m_pObject);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXSimulationInfo::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXSimulationInfo::Release()
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
IFXRESULT CIFXSimulationInfo::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXSimulationInfo)
	{
		*ppv = (IFXSimulationInfo*) this;
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
// PUBLIC IFXSimulationInfo::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::Initialize(IFXCoreServices * pCoreServices)
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
		m_time = 0;

		m_initialized = TRUE;
	}
	else
	{
		IFXRELEASE(m_pCoreServices);
	}

	return rc;
}


//---------------------------------------------------------------------
// PUBLIC IFXSimulationInfo::SetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::SetOrigin(IFXUnknown * pObject)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pObject);
		m_pObject = pObject;
		if (m_pObject)
			m_pObject->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationInfo::GetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::GetOrigin(IFXUnknown ** ppObject)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppObject)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppObject = m_pObject;
		if (*ppObject)
			(*ppObject)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationInfo::SetUserData
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::SetUserData(void * pData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		m_pUserData = pData;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationInfo::GetUserData
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::GetUserData(void ** ppData)
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
IFXRESULT CIFXSimulationInfo::GetCoreServices(IFXCoreServices ** ppCoreServices)
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
IFXRESULT CIFXSimulationInfo::SetTime(U32 time)
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
IFXRESULT CIFXSimulationInfo::GetTime(U32 *pTime)
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
//
// Compare m_time of the two IFXSimulationInfo's.
//
// After executing an IntegratorTask, the SimulationManager should
// SetTime() to the next execution time.
//
// if pPattern is NULL, always matches
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::MatchPattern(IFXTaskData * pPattern, BOOL * pResult)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pResult)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXSimulationInfo * pSimulationInfo = NULL;
		if (!pPattern)
		{
			*pResult = TRUE; // pPattern was NULL
		}
		else
		{
			rc = pPattern->QueryInterface(IID_IFXSimulationInfo, (void**) &pSimulationInfo);
			if (IFXSUCCESS(rc))
			{
				U32 pattern_time = 0;
	
				rc = pSimulationInfo->GetTime(&pattern_time);
				if (IFXSUCCESS(rc))
				{
					if (m_time == pattern_time)
						*pResult = TRUE;
					else
						*pResult = FALSE;
				}
			}
		}
		IFXRELEASE(pSimulationInfo);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskData::GetSortKey
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::GetSortKey(U32 * pKey)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pKey)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pKey = m_time; // sort by time
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationInfo::SetPriority
// priority zero = last, priority FFFFFFFF = first
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::SetPriority(U32 priority)
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
// PUBLIC IFXSimulationInfo::GetPriority
// priority zero = last, priority FFFFFFFF = first
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationInfo::GetPriority(U32 *pPriority)
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


IFXRESULT CIFXSimulationInfo::SetTaskManager(IFXTaskManager * pTaskManager)
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

IFXRESULT CIFXSimulationInfo::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

IFXRESULT CIFXSimulationInfo::SetTaskHandle(IFXTaskHandle taskHandle)
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

IFXRESULT CIFXSimulationInfo::GetTaskHandle(IFXTaskHandle * pTaskHandle)
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

IFXRESULT CIFXSimulationInfo::SetMsg(IFXSimulationMsg msg)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	m_msg = msg;
	return IFX_OK;
}
IFXSimulationMsg CIFXSimulationInfo::GetMsg()
{
	return m_msg;
}
IFXRESULT CIFXSimulationInfo::SetTimeInterval(U32 start, U32 end)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;
	m_startTime = start;
	m_endTime = end;
	m_maxTime = end;
	return IFX_OK;
}
IFXRESULT CIFXSimulationInfo::GetTimeInterval(U32 * pStart, U32 * pEnd)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;
	if (pStart)
		*pStart = m_startTime;
	if (pEnd)
		*pEnd = m_endTime;
	return IFX_OK;
}
IFXRESULT CIFXSimulationInfo::SetMaxTime(U32 maxTime)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;
	m_maxTime = maxTime;
	return IFX_OK;
}
U32 CIFXSimulationInfo::GetMaxTime()
{
	return m_maxTime;
}
