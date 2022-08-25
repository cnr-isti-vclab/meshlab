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
//
//  CIFXNotificationManager.cpp
//
//  DESCRIPTION
//    This module defines the CIFXNotificationManager class.  It is used to...
//
//  NOTES
//
//***************************************************************************

#include "CIFXScheduler.h"
#include "IFXNotificationInfo.h"
#include "IFXSchedulingCIDs.h"

// const U32 kFIRST_USER_ID = 1; // leave 'zero' unused, since that means "no event id"

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXNotificationManager_Factory(IFXREFIID riid, void ** ppv)
{
	CIFXNotificationManager *pObject = NULL;
	IFXRESULT rc = IFX_OK;
	BOOL allocated = FALSE;

	if (ppv == NULL)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		pObject = new CIFXNotificationManager();

		if (!pObject)
		{
			// allocation failed
			*ppv = NULL;
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	if (IFXSUCCESS(rc))
		rc = pObject->QueryInterface(riid, ppv);

	if (rc != IFX_OK && allocated)
	{
		delete pObject;
		pObject = NULL;
	}

	return rc;
}

//---------------------------------------------------------------------
// PRIVATE
//---------------------------------------------------------------------
CIFXNotificationManager::CIFXNotificationManager()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pTaskManager = NULL;

	m_nextAutoType = IFXNotificationType_Auto;
	m_nextAutoId = IFXNotificationId_Auto;
	m_pIdTable = NULL;
}

//---------------------------------------------------------------------
// PRIVATE
//---------------------------------------------------------------------
CIFXNotificationManager::~CIFXNotificationManager()
{
	Cleanup();

	if (m_pTaskManager)
		m_pTaskManager->Reset();

	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXNotificationManager::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXNotificationManager::Release()
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
IFXRESULT CIFXNotificationManager::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXNotificationManager)
	{
		*ppv = (IFXNotificationManager*) this;
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
// PUBLIC IFXNotificationManager::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::Initialize(IFXCoreServices * pCoreServices)
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
		rc = IFXCreateComponent(CID_IFXTaskManager, IID_IFXTaskManager, (void**) &m_pTaskManager);
	}
	if (IFXSUCCESS(rc))
	{
		rc = m_pTaskManager->Initialize(m_pCoreServices);
	}

	if (IFXSUCCESS(rc))
	{
		m_initialized = TRUE;
	}
	else
	{
		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pTaskManager);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationManager::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->Reset();

	if (IFXSUCCESS(rc))
		rc = Cleanup(); // same as destructor

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC Cleanup
//
// This is used by the destructor for cleanup
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::Cleanup()
{
	IFXRESULT rc = IFX_OK;

	// LEGACY
	while (m_pIdTable)
	{
		IFXEventIdEntry *p = m_pIdTable;
		m_pIdTable = m_pIdTable->pNext;
		delete p;
	}

	m_nextAutoType = IFXNotificationType_Auto;
	m_nextAutoId = IFXNotificationId_Auto;

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationManager::GetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

IFXRESULT CIFXNotificationManager::GetNextType(IFXNotificationType * pType)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pType)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc)) {
		if (m_nextAutoType == IFXNotificationType_Invalid)
			rc = IFX_E_UNDEFINED; // out of resources
		else
			*pType = m_nextAutoType;
		m_nextAutoType = IFXNotificationType(m_nextAutoType + 1);
	}
	IFXRETURN(rc);
}

IFXRESULT CIFXNotificationManager::GetNextId(IFXNotificationId * pId)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pId)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc)) 
	{
		IFXASSERT(m_nextAutoId != IFXNotificationId_Invalid);
		
		if (m_nextAutoId == IFXNotificationId_Invalid) 
		{
			rc = IFX_E_UNDEFINED; // out of resources
		}
		else
		{
			*pId = m_nextAutoId;
			m_nextAutoId = IFXNotificationId(m_nextAutoId + 1);
		}
	}
	IFXRETURN(rc);
}


//---------------------------------------------------------------------
// PUBLIC IFXNotificationManager::AddEventId
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::AddEventId(U32 key, IFXNotificationId id)
{
	IFXRESULT rc = IFX_OK;
	// make sure we don't enter a duplicate key
	IFXEventIdEntry *p = m_pIdTable;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	while (IFXSUCCESS(rc) && p)
	{
		if (p->key == key)
			rc = IFX_E_ALREADY_INITIALIZED;
		else
			p = p->pNext;
	}

	// now add the key
	if (IFXSUCCESS(rc))
	{
		IFXEventIdEntry *pEntry = new IFXEventIdEntry;
		if (pEntry)
		{
			pEntry->key = key;
			pEntry->id = id;
			pEntry->pNext = m_pIdTable;
			m_pIdTable = pEntry;
		}
		else rc = IFX_E_OUT_OF_MEMORY;
	}

	IFXRETURN(rc);
}

//---------------------------------------------------------------------
// PUBLIC IFXNotificationManager::FindEventId
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::FindEventId(U32 key, IFXNotificationId *pId)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pId)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		rc = IFX_E_NOT_FOUND;
		IFXEventIdEntry *p = m_pIdTable;
		while (IFXFAILURE(rc) && p)
		{
			if (p->key == key)
			{
				*pId = p->id;
				rc = IFX_OK;
			}
			else
				p = p->pNext;
		}
	}

	IFXRETURN(rc);
}


//---------------------------------------------------------------------
// PUBLIC IFXNotificationManager::SubmitEvent
//
// Using the provided IFXNotificationInfo, notify all registered
// listeners of the event.
//
// (1) Get an IFXTaskManagerView from the IFXTaskManager
// (2) Submit the event to everyone in the view
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::SubmitEvent(IFXNotificationInfo * pInfo)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pInfo)
		rc = IFX_E_INVALID_POINTER;

	IFXTaskManagerView * pView = NULL;

	if (IFXSUCCESS(rc))
	{
		// Specify the Notification Manager as the event's originator.
		rc = pInfo->SetOrigin( ( IFXUnknown* ) this );
	}

	if (IFXSUCCESS(rc))
	{
		rc = m_pTaskManager->CreateView(pInfo, &pView);

		if (IFXSUCCESS(rc))
		{
			rc = pView->ExecuteAll(pInfo, TRUE);
		}

		IFXRELEASE(pView);
	}

	return rc;
}

IFXRESULT CIFXNotificationManager::SubmitError(IFXErrorInfo * pErrInfo)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pErrInfo)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXTaskHandle handle = 0;
		IFXTask * pTask = NULL;
		IFXUnknown	*pTaskUnknown	= NULL;
		IFXRESULT errcode = IFX_OK;
		void *pUserData = NULL;
		U32	time	= 0;
		IFXNotificationInfo * pInfo = NULL;

		rc = pErrInfo->GetErrorTaskHandle(&handle);
		if (IFXSUCCESS(rc))
			rc = pErrInfo->GetErrorTask(&pTask);
		if (IFXSUCCESS(rc))
			rc = pTask->QueryInterface( IID_IFXUnknown, ( void** ) &pTaskUnknown );
		if (IFXSUCCESS(rc))
			rc = pErrInfo->GetErrorCode(&errcode);
		if (IFXSUCCESS(rc))
			rc = pErrInfo->GetUserData(&pUserData);
		if (IFXSUCCESS(rc))
			rc = pErrInfo->GetTime( &time );

		if (IFXSUCCESS(rc))
			rc = IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo);
		if (IFXSUCCESS(rc))
		{
			rc = pInfo->Initialize(m_pCoreServices);

			// It isn't necessary to set the origin since the general 
			// SubmitEvent function will set it to the Notification Manager.

			// UserData is actually stored at registration time and copied into the task data
			// at the time of execution. So there is no point in setting user data here, since
			// it will be overwritten. Instead, put our event-specific user data into the
			// NotificationData field.
			if (IFXSUCCESS(rc))
				rc = pInfo->SetNotificationData(pUserData);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetType(IFXNotificationType_Error);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetId((IFXNotificationId) handle);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetKeyFilter(errcode);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetObjectFilter( pTaskUnknown );
			if (IFXSUCCESS(rc))
				rc = pInfo->SetTime( time );
			if (IFXSUCCESS(rc))
				rc = SubmitEvent(pInfo);

		}

		IFXRELEASE( pTask );
		IFXRELEASE( pTaskUnknown );
		IFXRELEASE( pInfo );
	}

	return rc;
}

IFXRESULT CIFXNotificationManager::SubmitEvent(IFXNotificationType type, IFXNotificationId id, U32 time, void * pUserData)
{
	IFXRESULT rc = IFX_OK;

	/// @todo: reuse the event object (pattern after IFXTaskManagerview)

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		IFXNotificationInfo * pInfo = NULL;
		rc = IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo);
		if (IFXSUCCESS(rc))
		{
			rc = pInfo->Initialize(m_pCoreServices);

			// It isn't necessary to set the origin since the general 
			// SubmitEvent function will set it to the Notification Manager.

			// UserData is actually stored at registration time and copied into the task data
			// at the time of execution. So there is no point in setting user data here, since
			// it will be overwritten. Instead, put our event-specific user data into the
			// NotificationData field.
			if (IFXSUCCESS(rc))
				rc = pInfo->SetNotificationData(pUserData);

			if (IFXSUCCESS(rc))
				rc = pInfo->SetType(type);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetId(id);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetTime(time);

			if (IFXSUCCESS(rc))
				rc = SubmitEvent(pInfo);
		}
		IFXRELEASE(pInfo);
	}

	return rc;
}

IFXRESULT CIFXNotificationManager::RegisterTask(IFXTask * pTask, IFXNotificationType type, IFXNotificationId id, void * pUserData, IFXUnknown *pObject, IFXTaskHandle *pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXNotificationInfo * pInfo = NULL;

		rc = IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo);

		if (IFXSUCCESS(rc))
			rc = pInfo->Initialize(m_pCoreServices);
		// No point in setting the origin for a registered task
		if (IFXSUCCESS(rc))
			rc = pInfo->SetType(type);
		if (IFXSUCCESS(rc))
			rc = pInfo->SetId(id);
		if (IFXSUCCESS(rc))
			rc = pInfo->SetUserData(pUserData);
		if (IFXSUCCESS(rc))
			rc = pInfo->SetObjectFilter(pObject);

		if (IFXSUCCESS(rc))
		{
			IFXTaskManagerNode * pNode = NULL;

			rc = IFXCreateComponent(CID_IFXTaskManagerNode, IID_IFXTaskManagerNode, (void**) &pNode);
			if (IFXSUCCESS(rc))
				rc = pNode->Initialize(m_pCoreServices);
			if (IFXSUCCESS(rc))
				rc = pNode->SetTask(pTask);
			if (IFXSUCCESS(rc))
				rc = pNode->SetTaskData(pInfo);

			if (IFXSUCCESS(rc))
				rc = m_pTaskManager->AddTask(pNode); // Notification Task

			if (IFXSUCCESS(rc))
				rc = pNode->GetTaskHandle(pTaskHandle);

			IFXRELEASE(pNode);
		}

		IFXRELEASE(pInfo);
	}

	return rc;
}
IFXRESULT CIFXNotificationManager::RegisterTask(IFXTask * pTask, IFXNotificationType type, IFXNotificationId id, IFXTaskHandle *pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		IFXNotificationInfo * pInfo = NULL;

		rc = IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo);

		if (IFXSUCCESS(rc))
			rc = pInfo->Initialize(m_pCoreServices);
		// No point in setting the origin for a registered task
		if (IFXSUCCESS(rc))
			rc = pInfo->SetType(type);
		if (IFXSUCCESS(rc))
			rc = pInfo->SetId(id);

		if (IFXSUCCESS(rc))
		{
			IFXTaskManagerNode * pNode = NULL;

			rc = IFXCreateComponent(CID_IFXTaskManagerNode, IID_IFXTaskManagerNode, (void**) &pNode);
			if (IFXSUCCESS(rc))
				rc = pNode->Initialize(m_pCoreServices);
			if (IFXSUCCESS(rc))
				rc = pNode->SetTask(pTask);
			if (IFXSUCCESS(rc))
				rc = pNode->SetTaskData(pInfo);

			if (IFXSUCCESS(rc))
				rc = m_pTaskManager->AddTask(pNode); // Notification Task

			if (IFXSUCCESS(rc))
				rc = pNode->GetTaskHandle(pTaskHandle);

			IFXRELEASE(pNode);
		}

		IFXRELEASE(pInfo);
	}

	return rc;
}

IFXRESULT CIFXNotificationManager::UnregisterTask(IFXTaskHandle taskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->RemoveTask(taskHandle);

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTask::Execute
//---------------------------------------------------------------------
IFXRESULT CIFXNotificationManager::Execute(IFXTaskData *pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskData)
		rc = IFX_E_INVALID_POINTER;

	IFXNotificationInfo * pNI = NULL;

	if (IFXSUCCESS(rc))
	{
		rc = pTaskData->QueryInterface(IID_IFXNotificationInfo, (void**) &pNI);
		if (IFXFAILURE(rc))
			rc = IFX_E_UNSUPPORTED;
	}

	if (IFXSUCCESS(rc))
	{
		IFXNotificationType type = IFXNotificationType_Nil;
		IFXNotificationId id = IFXNotificationId_Nil;

		rc = pNI->GetType(&type);
		if (IFXSUCCESS(rc))
			rc = pNI->GetId(&id);
		if (IFXSUCCESS(rc) && type == IFXNotificationType_Task && id == IFXNotificationId_TaskReset)
		{
			Reset();
		}
	}

	IFXRELEASE(pNI);

	return rc;
}
