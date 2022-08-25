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
	@file	CIFXTaskManagerNode.cpp

			This module defines the CIFXTaskManagerNode class.  It is used to...
*/

#include "CIFXTaskManagerNode.h"

#include "IFXScheduler.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXTaskManagerNode_Factory(IFXREFIID riid, void** ppv)
{
	CIFXTaskManagerNode *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXTaskManagerNode();

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
CIFXTaskManagerNode::CIFXTaskManagerNode()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;

	m_pTask = NULL;
	m_pTaskData = NULL;

	m_taskHandle = IFXTASK_HANDLE_INVALID;
	m_priority = 0;
	m_enabled = TRUE;
	m_markForDelete = FALSE;
	m_busy = FALSE;

	m_pTaskManager = NULL;

	m_pPrev = NULL;
	m_pNext = NULL;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXTaskManagerNode::~CIFXTaskManagerNode()
{
	Reset();
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXTaskManagerNode::AddRef()
{
	return ++m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXTaskManagerNode::Release()
{
	if( 1 == m_refcount )
	{
		delete this;
		return 0;
	}
	return --m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::QueryInterface
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXTaskManagerNode)
	{
		*ppv = (IFXTaskManagerNode*) this;
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
// PUBLIC IFXTaskManagerNode::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::Initialize(IFXCoreServices * pCoreServices)
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
		IFXScheduler * pScheduler = NULL;
		rc = pCoreServices->GetScheduler(IID_IFXScheduler, (void**) &pScheduler);
		if (IFXSUCCESS(rc))
		{
			rc = pScheduler->GetNextTaskHandle(&m_taskHandle);
		}
		IFXRELEASE(pScheduler);
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
// PUBLIC IFXTaskManagerNode::Rest
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pTaskManager);

		IFXRELEASE(m_pTask);
		IFXRELEASE(m_pTaskData);

		IFXRELEASE(m_pPrev);
		IFXRELEASE(m_pNext);

		m_initialized = FALSE;
	}

	return rc;
}


//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetCoreServices
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetCoreServices(IFXCoreServices ** ppCoreServices)
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
// PUBLIC IFXTaskManagerNode::GetPriority
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetPriority(U32 * pPriority)
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

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetPriority
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetPriority(U32 priority)
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
// PUBLIC IFXTaskManagerNode::GetEnabled
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetEnabled(BOOL * pEnabled)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pEnabled)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pEnabled = m_enabled;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetEnabled
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetEnabled(BOOL enabled)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_enabled = enabled;
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetMarkForDelete
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetMarkForDelete(BOOL mark)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_markForDelete = mark;
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetMarkForDelete
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetMarkForDelete(BOOL *pMark)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pMark)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pMark = m_markForDelete;
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetTaskHandle
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetTaskHandle(IFXTaskHandle taskHandle)
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

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetTaskHandle
// Return IFXTASK_HANDLE_INVALID if not defined
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetTaskHandle(IFXTaskHandle * pTaskHandle)
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

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::IFXTask
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetTask(IFXTask * pTask)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pTask);
		m_pTask = pTask;
		if (m_pTask)
			m_pTask->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::IFXTask
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetTask(IFXTask ** ppTask)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppTask)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppTask = m_pTask;
		if (*ppTask)
			(*ppTask)->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetTaskData
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetTaskData(IFXTaskData * pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pTaskData);
		m_pTaskData = pTaskData;
		if (m_pTaskData)
			m_pTaskData->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetTaskData
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetTaskData(IFXTaskData ** ppTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppTaskData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppTaskData = m_pTaskData;
		if (*ppTaskData)
			(*ppTaskData)->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetTaskData
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::Execute(IFXTaskData * pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskData)
		rc = IFX_E_INVALID_POINTER;
	else if (m_busy)
		rc = IFX_E_ABORTED;
	else if (!m_enabled)
		rc = IFX_E_ABORTED;

	if (IFXSUCCESS(rc))
	{
		m_busy = TRUE;

		void * pUserData = NULL;

		// UserData is stored in the TaskManagerNode, and must
		// be copied into the pTaskData so that the task can see it
		rc = m_pTaskData->GetUserData(&pUserData);
		if (IFXSUCCESS(rc))
			rc = pTaskData->SetUserData(pUserData);

		// Okay, UserData is set, now we can execute the task
		if (IFXSUCCESS(rc))
			rc = m_pTask->Execute(pTaskData);

		m_busy = FALSE;
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::MatchPattern
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::MatchPattern(IFXTaskData * pViewPattern, BOOL * pResult)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pResult)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		rc = m_pTaskData->MatchPattern(pViewPattern, pResult);
	}
	return rc;
}


//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetTaskManager(IFXTaskManager * pTaskManager)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskManager)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pTaskManager);
		m_pTaskManager = pTaskManager;
		if (m_pTaskManager)
			m_pTaskManager->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetNext
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetNext(IFXTaskManagerNode * pNext)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pNext);
		m_pNext = pNext;
		if (m_pNext)
			m_pNext->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::SetPrev
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerNode::SetPrev(IFXTaskManagerNode * pPrev)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pPrev);
		m_pPrev = pPrev;
		if (m_pPrev)
			m_pPrev->AddRef();
	}
	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetNext_NoAddRef
//---------------------------------------------------------------------
IFXTaskManagerNode * CIFXTaskManagerNode::GetNext_NoAddRef()
{
	return m_pNext;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerNode::GetPrev_NoAddRef
//---------------------------------------------------------------------
IFXTaskManagerNode * CIFXTaskManagerNode::GetPrev_NoAddRef()
{
	return m_pPrev;
}
