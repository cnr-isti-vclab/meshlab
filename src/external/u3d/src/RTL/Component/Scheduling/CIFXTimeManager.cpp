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
	@file	CIFXTimeManager.cpp

			This module defines the CIFXTimeManager class.  It is used to...
*/

#include "CIFXScheduler.h"
#include "IFXSimulationInfo.h"
#include "IFXSchedulingCIDs.h"

// How many items shall we allow in the free list? Too few and we allocate
// memory too often; too many and we hold on to unused memory unnecessarily.
/// @todo: make this dynamic, based on statistical analysis?
#define MAX_FREE_LIST 10


//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXTimeManager_Factory(IFXREFIID riid, void ** ppv)
{
	CIFXTimeManager *pObject = NULL;
	IFXRESULT rc = IFX_OK;
	BOOL allocated = FALSE;

	if (ppv == NULL)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		pObject = new CIFXTimeManager();

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
CIFXTimeManager::CIFXTimeManager()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pTaskManager = NULL;

	// legacy

	m_pTimerList = NULL;
	m_pTimerFreeList = NULL;
	m_sizeFreeList = 0;
	m_nextTimerId = 1;
	m_busy = FALSE;
}

//---------------------------------------------------------------------
// PRIVATE
//---------------------------------------------------------------------
CIFXTimeManager::~CIFXTimeManager()
{
	Cleanup();

	if (m_pTimerFreeList) {
		CIFXTimer * pTemp, *pTimer = m_pTimerFreeList;
		while (pTimer) {
			pTemp = pTimer;
			pTimer = pTimer->pNext;
			delete pTemp;
		}
	}

	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pTaskManager);
}

//---------------------------------------------------------------------
// PUBLIC IFXTimeManager::Cleanup
//
// clear out data
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::Cleanup()
{
	IFXRESULT rc = IFX_OK;

	if (m_pTimerList) {
		CIFXTimer * pTemp, *pTimer = m_pTimerList;
		while (pTimer) {
			pTemp = pTimer;
			pTimer = pTimer->pNext;
			if (m_sizeFreeList < MAX_FREE_LIST) {
				pTemp->pNext = m_pTimerFreeList; // move to freelist
				m_pTimerFreeList = pTemp;
				m_sizeFreeList++;
			} else {
				delete pTemp;
			}
		}
		m_pTimerList = NULL;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXTimeManager::AddRef()
{
	// increment this manager
	++m_refcount;

	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXTimeManager::Release()
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
IFXRESULT CIFXTimeManager::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXTimeManager)
	{
		*ppv = (IFXTimeManager*) this;
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
// PUBLIC IFXTimeManager::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::Initialize(IFXCoreServices * pCoreServices)
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
		m_pTaskManager->Initialize(m_pCoreServices);
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
// PUBLIC IFXTimeManager::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = Cleanup();

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTimeManager::GetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::GetTaskManager(IFXTaskManager ** ppTaskManager)
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
// PUBLIC IFXTask::Execute
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::Execute(IFXTaskData *pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskData)
		rc = IFX_E_INVALID_POINTER;

	IFXNotificationManager * pNM = NULL;
	IFXNotificationInfo * pNI = NULL;
	IFXSimulationInfo * pSI = NULL;

	if (IFXSUCCESS(rc))
	{
		rc = pTaskData->QueryInterface(IID_IFXNotificationInfo, (void**) &pNI);
		if (IFXFAILURE(rc))
			rc = pTaskData->QueryInterface(IID_IFXSimulationInfo, (void**) &pSI);
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
		U32 startTime = 0;
		U32 endTime = 0;

		if (IFXSUCCESS(rc))
		{
			rc = pSI->GetTimeInterval(&startTime,&endTime);

			if (IFXSUCCESS(rc))
			{
				IFXSimulationMsg msg = pSI->GetMsg();
				if (msg == IFXSimulationMsg_Query)
				{
					U32 maxTime = 0;
					rc = QueryInterval(startTime,endTime,&maxTime);
					if (IFXSUCCESS(rc))
						rc = pSI->SetMaxTime(maxTime);
				}
				else if (msg == IFXSimulationMsg_Advance)
				{
					rc = AdvanceInterval(startTime,endTime);
				}
				else
					rc = IFX_E_INVALID_HANDLE;
			}
		}

	}

	IFXRELEASE(pNM);
	IFXRELEASE(pNI);
	IFXRELEASE(pSI);

	return rc;
}

IFXRESULT CIFXTimeManager::QueryInterval(U32 startTime, U32 endTime, U32 *pMaxTime)
{
	IFXRESULT rc = IFX_OK;

	// Default to end. If we find something before the end,
	// update it accordingly.
	*pMaxTime = endTime;

	m_busy = TRUE;
	if (m_pTimerList)
	{
		// The timer list is always sorted by time; the first item in
		// the list is the next item due to execute. If the nextTime
		// for that item occurs before the endTime, report that as
		// the correct maxTime.

		if (m_pTimerList->nextTime <= endTime)
			*pMaxTime = m_pTimerList->nextTime;
	}
	m_busy = FALSE;

	return rc;
}
IFXRESULT CIFXTimeManager::AdvanceInterval(U32 startTime, U32 endTime)
{
	IFXRESULT rc = IFX_OK;

	IFXNotificationManager *pNotificationManager = NULL;

	if (IFXSUCCESS(rc))
		rc = m_pCoreServices->GetNotificationManager(IID_IFXNotificationManager, (void**) &pNotificationManager);

	if (IFXSUCCESS(rc))
	{
		m_busy = TRUE;
		if (m_pTimerList)
		{
			while (m_pTimerList && m_pTimerList->nextTime <= endTime)
			{
				// cannot fail if m_pTimerList is non-NULL
				CIFXTimer * pTimer = PopTimer();

				pTimer->busy = TRUE;
#ifdef _DEBUG				
				IFXRESULT nmrc = 
#endif				
				pNotificationManager->SubmitEvent(
					IFXNotificationType_Time, pTimer->message_id, pTimer->nextTime, (void*) (uintptr_t) pTimer->timer_id);
				IFXASSERT(nmrc == IFX_OK);
				pTimer->busy = FALSE;
				pTimer->lastTime = pTimer->nextTime;
				pTimer->nextTime += pTimer->period;
				if (pTimer->duration == 0 || pTimer->nextTime < pTimer->duration)
					InsertTimer(pTimer);
				else
				{
					// timer has expired
					// if it has an associated Notification Event, unregister it
					if (pTimer->auto_nm) {
						if (!pNotificationManager)
							rc = m_pCoreServices->GetNotificationManager(IID_IFXNotificationManager, (void**) &pNotificationManager);
						IFXTaskHandle taskHandle = pTimer->auto_handle;
						pNotificationManager->UnregisterTask(taskHandle);
					}
					// delete the timer
					DeleteTimer(pTimer);
				}
			}
		}
		m_busy = FALSE;
	}
	IFXRELEASE(pNotificationManager);

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTask::SetTimer
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::SetAutoTimer(IFXNotificationId id, U32 start, U32 period, U32 duration, U32 * pTimerId, BOOL bAuto, IFXTaskHandle taskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTimerId)
		rc = IFX_E_INVALID_POINTER;
	else if (id == IFXNotificationId_Nil)
		rc = IFX_E_INVALID_RANGE;

	CIFXTimer * pNewTimer = NULL;

	if (IFXSUCCESS(rc))
	{
		pNewTimer = NewTimer();
		if (!pNewTimer)
			rc = IFX_E_OUT_OF_MEMORY;
	}

	if (IFXSUCCESS(rc))
	{
		pNewTimer->timer_id = m_nextTimerId++;
		pNewTimer->message_id = id;
		pNewTimer->start = start;
		pNewTimer->period = period;
		pNewTimer->duration = duration;
		pNewTimer->lastTime = 0;
		pNewTimer->nextTime = start;
		pNewTimer->busy = FALSE;
		pNewTimer->kill = FALSE;
		pNewTimer->auto_nm = bAuto;
		pNewTimer->auto_handle = taskHandle;
		pNewTimer->pNext = NULL;

		*pTimerId = pNewTimer->timer_id;
	}
	if (IFXSUCCESS(rc))
	{
		rc = InsertTimer(pNewTimer);
	}

	return rc;
}

IFXRESULT CIFXTimeManager::SetTimer(IFXNotificationId id, U32 start, U32 period, U32 duration, U32 * pTimerId)
{
	return SetAutoTimer(id, start, period, duration, pTimerId, FALSE, IFXTASK_HANDLE_INVALID);
}

//---------------------------------------------------------------------
// PUBLIC IFXTask::KillTimer
//---------------------------------------------------------------------
IFXRESULT CIFXTimeManager::KillTimer(U32 timer_id)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (timer_id == 0)
		rc = IFX_E_INVALID_HANDLE;

	// search for timer(s) matching id
	if (IFXSUCCESS(rc))
	{
		if (m_pTimerList)
		{
			CIFXTimer * pLast = NULL;
			CIFXTimer * pTimer = m_pTimerList;

			while (pTimer)
			{
				CIFXTimer * pTemp = NULL;
				if (pTimer->timer_id == timer_id)
					pTemp = pTimer;

				pLast = pTimer;
				pTimer = pTimer->pNext;

				if (pTemp)
				{
					if (pTemp == m_pTimerList)
						m_pTimerList = pTimer;
					else
						pLast->pNext = pTimer;

					pTemp->pNext = NULL;
					rc = DeleteTimer(pTemp);
				}
			}
		}
	}

	return rc;
}

// PRIVATE -- Add a timer to the Timer List in time-sorted order.
IFXRESULT CIFXTimeManager::InsertTimer(CIFXTimer *pNewTimer)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pNewTimer)
		rc = IFX_E_INVALID_POINTER;

	CIFXTimer* pLast = NULL;
	CIFXTimer* pTimer = NULL;

	if (IFXSUCCESS(rc))
	{
		if (!m_pTimerList)
		{
			// list is empty; add as singleton
			//
			// m_pTimerList -> pNewTimer -> NULL

			m_pTimerList = pNewTimer;
		}
		else if (pNewTimer->nextTime < m_pTimerList->nextTime)
		{
			// add at start of list
			//
			// m_pTimerList -> pNewTimer -> ...

			pNewTimer->pNext = m_pTimerList;
			m_pTimerList = pNewTimer;
		}
		else
		{
			pTimer = m_pTimerList;
			// locate correct position in list
			while (pTimer && pTimer->nextTime <= pNewTimer->nextTime)
			{
				pLast = pTimer;
				pTimer = pTimer->pNext;
			}

			// now add between 'pLast' and 'pTimer'
			//
			// pLast -> pNewTimer -> pTimer

			IFXASSERT(pLast);
			pNewTimer->pNext = pTimer;
			pLast->pNext = pNewTimer;
		}
	}

	return rc;
}

// PRIVATE -- remove an return the next timer from the Timer List.
CIFXTimer * CIFXTimeManager::PopTimer()
{
	CIFXTimer *pTimer = m_pTimerList;

	if (pTimer)
	{
		m_pTimerList = m_pTimerList->pNext;
		pTimer->pNext = NULL;
	}

	return pTimer;
}

// PRIVATE -- delete a Timer node.
IFXRESULT CIFXTimeManager::DeleteTimer(CIFXTimer *pTimer)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTimer)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		if (m_sizeFreeList < MAX_FREE_LIST)
		{
			pTimer->pNext = m_pTimerFreeList; // move to freelist
			m_pTimerFreeList = pTimer;
			m_sizeFreeList++;
		}
		else
		{
			delete pTimer;
		}
	}
	return rc;
}

// PRIVATE -- allocate and initialize a new Timer node.
CIFXTimer * CIFXTimeManager::NewTimer()
{
	CIFXTimer * pNewTimer = NULL;

	if (m_pTimerFreeList)
	{
		pNewTimer = m_pTimerFreeList;
		m_pTimerFreeList = m_pTimerFreeList->pNext;
		pNewTimer->pNext = NULL;
		m_sizeFreeList--;
	}
	else
	{
		pNewTimer = new CIFXTimer;
	}

	if (pNewTimer)
	{
		pNewTimer->timer_id   = 0xCDCDCDCD;
		pNewTimer->message_id = IFXNotificationId_Nil;
		pNewTimer->start      = 0xCDCDCDCD;
		pNewTimer->period     = 0xCDCDCDCD;
		pNewTimer->duration   = 0xCDCDCDCD;
		pNewTimer->lastTime   = 0xCDCDCDCD;
		pNewTimer->nextTime   = 0xCDCDCDCD;
		pNewTimer->busy  = FALSE;
		pNewTimer->kill  = FALSE;
		pNewTimer->auto_nm = FALSE;
		pNewTimer->pNext = NULL;
	}

	return pNewTimer;
}


// CONVENIENCE METHODS

IFXRESULT CIFXTimeManager::RegisterTask(IFXTask *pTask, U32 start, U32 period, U32 duration, IFXTaskHandle * pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTask)
		rc = IFX_E_INVALID_POINTER;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	IFXNotificationManager * pNM = NULL;
	IFXTaskManager * pTM = NULL;
	IFXNotificationInfo * pInfo = NULL;

	IFXNotificationId notification_id = IFXNotificationId_Nil;
	U32 timer_id = 0;
	IFXTaskHandle taskHandle = IFXTASK_HANDLE_INVALID;

	// define a new notification id
	if (IFXSUCCESS(rc))
		rc = m_pCoreServices->GetNotificationManager(IID_IFXNotificationManager, (void**) &pNM);
	if (IFXSUCCESS(rc))
		//    rc = pNM->GetNextUserId(&id); // create a notification id for this task
		rc = pNM->GetNextId(&notification_id);

	// create a new notification task
	if (IFXSUCCESS(rc))
		rc = IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**) &pInfo);
	if (IFXSUCCESS(rc))
		rc = pInfo->Initialize(m_pCoreServices);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetType(IFXNotificationType_Time);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetId(notification_id);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetUserData((void*) (uintptr_t) timer_id);
	if (IFXSUCCESS(rc))
		rc = pNM->GetTaskManager(&pTM);
	if (IFXSUCCESS(rc))
		rc = pTM->AddTask(pTask, pInfo, 0, &taskHandle);

	// create a new timer
	if (IFXSUCCESS(rc))
		rc = SetAutoTimer(notification_id, start, period, duration, &timer_id, TRUE, taskHandle);

	if (pTaskHandle)
		*pTaskHandle = taskHandle;

	IFXRELEASE(pInfo);
	IFXRELEASE(pTM);
	IFXRELEASE(pNM);

	return rc;
}

IFXRESULT CIFXTimeManager::UnregisterTask(IFXTaskHandle taskHandle)
{
	IFXRESULT rc = IFX_OK;

	IFXNotificationManager * pNM = NULL;
	IFXTaskManager * pTM = NULL;
	IFXTaskData * pTaskData = NULL;
	IFXTaskManagerNode * pNode = NULL;
	IFXNotificationInfo * pInfo = NULL;

	//IFXNotificationId id = IFXNotificationId_Nil;
	U32 timer_id = 0;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	// get the timer id
	if (IFXSUCCESS(rc))
		rc = m_pCoreServices->GetNotificationManager(IID_IFXNotificationManager, (void**) &pNM);
	if (IFXSUCCESS(rc))
		rc = pNM->GetTaskManager(&pTM);
	if (IFXSUCCESS(rc))
		rc = pTM->GetTask(taskHandle, &pNode);
	if (IFXSUCCESS(rc))
		rc = pNode->GetTaskData(&pTaskData);
	if (IFXSUCCESS(rc))
		rc = pTaskData->QueryInterface(IID_IFXNotificationInfo, (void**) &pInfo);
	if (IFXSUCCESS(rc))
		rc = pInfo->GetUserData((void**) &timer_id);

	// kill the notification task
	if (IFXSUCCESS(rc))
		rc = pTM->RemoveTask(taskHandle);

	// kill the timer
	if (IFXSUCCESS(rc))
		rc = KillTimer(timer_id);

	IFXRELEASE(pTaskData);
	IFXRELEASE(pInfo);
	IFXRELEASE(pNode);
	IFXRELEASE(pTM);
	IFXRELEASE(pNM);

	return rc;
}
