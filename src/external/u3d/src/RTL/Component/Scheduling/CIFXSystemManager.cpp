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
//  CIFXSystemManager.cpp
//
//  DESCRIPTION
//    This module defines the CIFXSystemManager class.  It is used to...
//
//  NOTES
//
//***************************************************************************

#include "CIFXSystemManager.h"
#include "IFXNotificationInfo.h"
#include "IFXSchedulerInfo.h"

#include "IFXSchedulingCIDs.h"

struct IFXTaskInfo
{
	IFXTask * pTask;
	IFXTaskHandle taskHandle;
	IFXUnknown *pUserData;
	U32 priority;
};


//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXSystemManager_Factory(IFXREFIID riid, void** ppv)
{
	CIFXSystemManager *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXSystemManager();

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
CIFXSystemManager::CIFXSystemManager()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pTaskManager = NULL;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXSystemManager::~CIFXSystemManager()
{
	if (m_pTaskManager)
		m_pTaskManager->Reset();

	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXSystemManager::AddRef()
{
	return ++m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXSystemManager::Release()
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
IFXRESULT CIFXSystemManager::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXSystemManager)
	{
		*ppv = (IFXSystemManager*) this;
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
// PUBLIC IFXSystemManager::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXSystemManager::Initialize(IFXCoreServices *pCoreServices)
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
// PUBLIC IFXTask::Execute
//---------------------------------------------------------------------
IFXRESULT CIFXSystemManager::Execute(IFXTaskData *pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskData)
		rc = IFX_E_INVALID_POINTER;

	IFXNotificationInfo * pNI = NULL;
	IFXSchedulerInfo * pSI = NULL;

	if (IFXSUCCESS(rc))
	{
		rc = pTaskData->QueryInterface(IID_IFXNotificationInfo, (void**) &pNI);
		if (IFXFAILURE(rc))
			rc = pTaskData->QueryInterface(IID_IFXSchedulerInfo, (void**) &pSI);
		if (IFXFAILURE(rc))
			rc = IFX_E_UNSUPPORTED;
	}

	if (IFXSUCCESS(rc) && pNI)
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

	if (IFXSUCCESS(rc) && pSI)
	{
		IFXTaskManagerView * pView = NULL;
		IFXTaskData * pInfo = NULL; /// @todo: reuse

		rc = IFXCreateComponent(CID_IFXTaskData, IID_IFXTaskData, (void**) &pInfo);
		if (IFXSUCCESS(rc))
			rc = pInfo->Initialize(m_pCoreServices);
		if (IFXSUCCESS(rc))
		{
			IFXUnknown * pUnknown=NULL;
			rc = QueryInterface(IID_IFXUnknown, (void**)&pUnknown);
			if (IFXSUCCESS(rc))
				rc = pInfo->SetOrigin(pUnknown);
			IFXRELEASE(pUnknown);
		}
		if (IFXSUCCESS(rc))
			rc = pInfo->SetTime(0);

		if (IFXSUCCESS(rc))
			rc = m_pTaskManager->CreateView(pInfo, &pView);
		if (IFXSUCCESS(rc))
			rc = pView->ExecuteAll(pInfo, TRUE);

		IFXRELEASE(pInfo);
		IFXRELEASE(pView);
	}
	IFXRELEASE(pNI);
	IFXRELEASE(pSI);

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSystemManager::GetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXSystemManager::GetTaskManager(IFXTaskManager ** ppTaskManager)
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
// PUBLIC IFXSystemManager::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXSystemManager::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->Reset();

	return rc;
}

IFXRESULT CIFXSystemManager::RegisterTask(IFXTask *pTask, U32 priority, void * pUserData, IFXTaskHandle * pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTask)
		rc = IFX_E_INVALID_POINTER;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	IFXSchedulerInfo * pInfo = NULL;

	if (IFXSUCCESS(rc))
		rc = IFXCreateComponent(CID_IFXSchedulerInfo, IID_IFXSchedulerInfo, (void**) &pInfo);
	if (IFXSUCCESS(rc))
		rc = pInfo->Initialize(m_pCoreServices);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetPriority(priority);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetUserData(pUserData);
	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->AddTask(pTask, pInfo, priority, pTaskHandle); // System Task

	IFXRELEASE(pInfo);

	return rc;
}

IFXRESULT CIFXSystemManager::UnregisterTask(IFXTaskHandle taskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->RemoveTask(taskHandle);

	return rc;
}
