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
	@file	CIFXTaskManager.cpp

			This module defines the CIFXTaskManager class.  It is used to...
*/

#include "CIFXTaskManager.h"
#include "IFXSchedulingCIDs.h"
#include "IFXDebug.h"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXTaskManager_Factory(IFXREFIID riid, void** ppv)
{
	CIFXTaskManager *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXTaskManager();

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

// Initialization and Cleanup 

//---------------------------------------------------------------------
// PRIVATE constructor
//---------------------------------------------------------------------
CIFXTaskManager::CIFXTaskManager()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pOrigin = NULL;

	m_pTaskList = NULL;
	m_pLastView = NULL;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXTaskManager::~CIFXTaskManager()
{
	DisposeList(&m_pTaskList);

	IFXRELEASE(m_pLastView);
	IFXRELEASE(m_pOrigin);
	IFXRELEASE(m_pCoreServices);
}

//---------------------------------------------------------------------
// PRIVATE
// Dispose of dynamic objects
//---------------------------------------------------------------------
void CIFXTaskManager::DisposeList(IFXTaskManagerNode ** ppList)
{
	IFXRESULT nrc = IFX_OK;

	IFXNotificationInfo * pInfo = NULL;
	nrc = IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo);
	if (IFXSUCCESS(nrc))
	{
		nrc = pInfo->Initialize(m_pCoreServices);
		if (IFXSUCCESS(nrc))
			nrc = pInfo->SetType(IFXNotificationType_Task);
	}

	IFXTaskManagerNode * pNode = *ppList; // we own ref to pNode

	while (pNode)
	{
		IFXTaskManagerNode * pVictim = pNode;
		pNode = pNode->GetNext_NoAddRef();
		if (pNode)
			pNode->AddRef();

		// now dispose of pVictim
		// first send a TaskReset notification
		if (IFXSUCCESS(nrc)) {
			IFXRESULT rc = IFX_OK;
			IFXTask * pTask = NULL;
			rc = pVictim->GetTask(&pTask);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetId(IFXNotificationId_TaskReset);
			if (IFXSUCCESS(rc) && pTask)
				rc = pTask->Execute(pInfo);
			IFXRELEASE(pTask);
		}
		if (IFXSUCCESS(nrc)) {
			IFXRESULT rc = IFX_OK;
			IFXTask * pTask = NULL;
			rc = pVictim->GetTask(&pTask);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetId(IFXNotificationId_TaskUnregistered);
			if (IFXSUCCESS(rc) && pTask)
				rc = pTask->Execute(pInfo);
			IFXRELEASE(pTask);
		}

		pVictim->Reset();

		pVictim->SetPrev(NULL);
		pVictim->SetNext(NULL);
		IFXRELEASE(pVictim);
	}

	IFXRELEASE(pInfo);

	*ppList = NULL; // list is destroyed, NULL the pointer
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::Initialize(IFXCoreServices *pCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (m_initialized)
		rc = IFX_E_ALREADY_INITIALIZED;
	else if (!pCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXASSERT( NULL == m_pCoreServices);
		m_pCoreServices = pCoreServices;
		m_pCoreServices->AddRef();
	}

	if (IFXSUCCESS(rc))
		m_initialized = TRUE;
	else
		IFXRELEASE(m_pCoreServices);

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		DisposeList(&m_pTaskList);

		IFXRELEASE(m_pLastView);
		// do not get rid of origin; if any component sets this,
		// it is their responsibility to set it again to NULL and
		// this get rid of any circular dependency

		// do not un-initialize; leave core services
	}

	return rc;
}

// IFXUnknown Methods 

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXTaskManager::AddRef()
{
	return ++m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXTaskManager::Release()
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
IFXRESULT CIFXTaskManager::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXTaskManager)
	{
		*ppv = (IFXTaskManager*) this;
		AddRef();
	}
	else
	{
		*ppv = NULL;
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

// PUBLIC Interface Methods 

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::CreateView
// if pPattern is NULL, get everything
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::CreateView(IFXTaskData * pPattern, IFXTaskManagerView ** ppView)
{
	IFXRESULT rc = IFX_OK;
	IFXTaskManagerView * pView = NULL;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppView)
		rc = IFX_E_INVALID_HANDLE;

	if (IFXSUCCESS(rc))
	{
		if (m_pLastView)
		{
			if (m_pLastView->GetRefCount() == 1)
			{
				// yes -- we are the only component with an pointer to the view
				m_pLastView->Reset(); // clear old view data so we can reuse the component
			}
			else
			{
				// no -- someone else is using it, so get rid of it
				IFXRELEASE(m_pLastView);
			}
		}

		if (m_pLastView)
		{
			pView = m_pLastView;
		}
		else
		{
			rc = IFXCreateComponent(CID_IFXTaskManagerView, IID_IFXTaskManagerView, (void**) &pView);
			if (IFXSUCCESS(rc))
				rc = pView->Initialize(m_pCoreServices);
			if (IFXSUCCESS(rc))
				m_pLastView = pView;
		}
	}


	if (IFXSUCCESS(rc))
	{
		IFXTaskManagerNode * pNode = m_pTaskList;

		// populate view
		while (pNode)
		{
			BOOL match = FALSE; // not a match

			if (pPattern == NULL)
				match = TRUE;
			else
				pNode->MatchPattern(pPattern, &match);

			if (match)
				pView->AddTask(pNode);

			pNode = pNode->GetNext_NoAddRef();
		}

		*ppView = pView;
		(*ppView)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::GetTask
// Get a task by its task handle
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::GetTask(U32 taskHandle,  IFXTaskManagerNode ** ppNode)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppNode)
		rc = IFX_E_INVALID_POINTER;
	else if (taskHandle == IFXTASK_HANDLE_INVALID)
		rc = IFX_E_INVALID_HANDLE;

	if (IFXSUCCESS(rc))
	{
		IFXTaskManagerNode * pNode = m_pTaskList;

		*ppNode = NULL;
		rc = IFX_E_NOT_FOUND;

		while (pNode)
		{
			IFXTaskHandle this_taskHandle;
			pNode->GetTaskHandle(&this_taskHandle);
			if (this_taskHandle == taskHandle)
			{
				*ppNode = pNode;
				(*ppNode)->AddRef();
				rc = IFX_OK;
				break;
			}
			else pNode = pNode->GetNext_NoAddRef();
		}
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::AddTask
// Add a new task to the manager
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::AddTask(IFXTaskManagerNode * pNode)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pNode)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		// add at head of list
		/// @todo	Do this properly, insert into correct position
		rc = pNode->SetNext(m_pTaskList);
		if (IFXSUCCESS(rc))
		{
			if (m_pTaskList)
				m_pTaskList->SetPrev(pNode); // be sure to set the Prev ptr
		}
		if (IFXSUCCESS(rc))
		{
			SetTaskListRoot(pNode); // release old root, set & addref
		}
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::AddTask
// This is a convenience method which creates a node implicitly and
// registers it
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::AddTask(IFXTask *pTask, IFXTaskData *pData, U32 priority, IFXTaskHandle * pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTask)
		rc = IFX_E_INVALID_POINTER;
	else if (!pData)
		rc = IFX_E_INVALID_POINTER;

	IFXTaskManagerNode * pNode = NULL;

	if (IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXTaskManagerNode, IID_IFXTaskManagerNode, (void**) &pNode);
		if (IFXSUCCESS(rc))
		{
			rc = pNode->Initialize(m_pCoreServices);
		}
		if (IFXSUCCESS(rc))
		{
			rc = pNode->SetTask(pTask);
		}
		if (IFXSUCCESS(rc))
		{
			rc = pNode->SetTaskData(pData);
		}
		if (IFXSUCCESS(rc))
		{
			rc = pNode->SetPriority(priority);
		}
	}

	if (IFXSUCCESS(rc))
	{
		rc = AddTask(pNode);
	}

	if (IFXSUCCESS(rc))
	{
		pNode->GetTaskHandle(pTaskHandle);
	}

	IFXRELEASE(pNode);

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::DeleteTask
// Delete a task by its task handle
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::RemoveTask(U32 taskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (taskHandle == IFXTASK_HANDLE_INVALID)
		rc = IFX_E_INVALID_HANDLE;

	if (IFXSUCCESS(rc))
	{
		IFXTaskManagerNode * pNode = m_pTaskList;

		rc = IFX_E_NOT_FOUND;

		while (pNode)
		{
			IFXRESULT rc1 = IFX_OK;
			IFXTaskHandle this_taskHandle;
			rc1 = pNode->GetTaskHandle(&this_taskHandle);
			if (IFXSUCCESS(rc1) && this_taskHandle == taskHandle)
			{
				rc1 = pNode->AddRef(); // keep a ref when it's removed from the list
				if (pNode == m_pTaskList)
				{
					SetTaskListRoot(pNode->GetNext_NoAddRef());
					rc1 = pNode->SetNext(NULL);
					rc1 = pNode->SetPrev(NULL);
				}
				else
				{
					IFXTaskManagerNode * pPrevNode = pNode->GetPrev_NoAddRef();
					IFXTaskManagerNode * pNextNode = pNode->GetNext_NoAddRef();

					IFXASSERT(pPrevNode); // if this isn't true, we shouldn't be in this 'else' block

					rc1 = pPrevNode->SetNext(pNextNode);
					if (pNextNode)
						pNextNode->SetPrev(pPrevNode);

					rc1 = pNode->SetPrev(NULL);
					rc1 = pNode->SetNext(NULL);
				}
				IFXRELEASE(pNode); // NOW release it
				rc = IFX_OK;
				break;
			}
			else pNode = pNode->GetNext_NoAddRef();
		}
	}

	return rc;
}

// PUBLIC Accessor Methods 

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::SetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::SetOrigin(IFXUnknown * pOrigin)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pOrigin);
		m_pOrigin = pOrigin;
		if (m_pOrigin)
			m_pOrigin->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManager::GetOrigin
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManager::GetOrigin(IFXUnknown ** ppOrigin)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppOrigin)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppOrigin = m_pOrigin;
		if (*ppOrigin)
			(*ppOrigin)->AddRef();
	}

	return rc;
}

// PRIVATE
void CIFXTaskManager::SetTaskListRoot(IFXTaskManagerNode * pNode)
{
	IFXRELEASE(m_pTaskList);
	m_pTaskList = pNode;
	if (m_pTaskList)
		m_pTaskList->AddRef();
	if (m_pTaskList)
		m_pTaskList->SetPrev(NULL); // since we're the head, we can't have a previous
}
