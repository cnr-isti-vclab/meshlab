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
	@file	CIFXScheduler.cpp

			This module contains the CIFXScheduler component.
*/

#include "CIFXScheduler.h"

#include "IFXNotificationManager.h"
#include "IFXTimeManager.h"

#include "IFXSchedulerInfo.h"
#include "IFXTaskData.h"
#include "IFXSimulationInfo.h"

#include "IFXSchedulingCIDs.h"


//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXScheduler_Factory(IFXREFIID riid, void** ppv)
{
	CIFXScheduler *pObject = NULL;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXScheduler();

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

//---------------------------------------------------------------------------
CIFXScheduler::CIFXScheduler()
{
	m_refCount = 0;
	m_initialized = FALSE;
	m_pCoreServices = NULL;
	m_pTaskManager = NULL;
	m_pSystemManager = NULL;
	m_pSimulationManager = NULL;
	m_pNotificationManager = NULL;
	m_pTimeManager = NULL;

	m_nextTaskHandle = IFXTASK_HANDLE_INVALID + 1;
}

//---------------------------------------------------------------------------
CIFXScheduler::~CIFXScheduler()
{
	IFXRELEASE(m_pSystemManager);
	IFXRELEASE(m_pSimulationManager);

	IFXRELEASE(m_pNotificationManager);
	IFXRELEASE(m_pTimeManager);

	IFXRELEASE(m_pTaskManager);

	IFXRELEASE(m_pCoreServices);
}

//---------------------------------------------------------------------------
U32 CIFXScheduler::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXScheduler::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXScheduler )
		{
			*ppInterface = ( IFXScheduler* ) this;
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::Initialize(IFXCoreServices *pCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (m_initialized)
		rc = IFX_E_ALREADY_INITIALIZED;
	else if (!pCoreServices)
		rc = IFX_E_INVALID_POINTER;

	m_initialized = TRUE;

	if (IFXSUCCESS(rc))
	{
		m_pCoreServices = pCoreServices;
		m_pCoreServices->AddRef();
	}

	if (IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXTaskManager, IID_IFXTaskManager, (void**) &m_pTaskManager);
		if (IFXSUCCESS(rc) && !m_pTaskManager)
			rc = IFX_E_OUT_OF_MEMORY;
		if (IFXSUCCESS(rc))
			rc = m_pTaskManager->Initialize(m_pCoreServices);
	}

	if (IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXSystemManager, IID_IFXSystemManager, (void**) &m_pSystemManager);
		if (IFXSUCCESS(rc) && !m_pSystemManager)
			rc = IFX_E_OUT_OF_MEMORY;
		if (IFXSUCCESS(rc))
			rc = m_pSystemManager->Initialize(m_pCoreServices);
	}

	if (IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXSimulationManager, IID_IFXSimulationManager, (void**) &m_pSimulationManager);
		if (IFXSUCCESS(rc) && !m_pSimulationManager)
			rc = IFX_E_OUT_OF_MEMORY;
		if (IFXSUCCESS(rc))
			rc = m_pSimulationManager->Initialize(m_pCoreServices);
	}

	if (IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXNotificationManager, IID_IFXNotificationManager, (void**) &m_pNotificationManager);
		if (IFXSUCCESS(rc) && !m_pNotificationManager)
			rc = IFX_E_OUT_OF_MEMORY;
		if (IFXSUCCESS(rc))
			rc = m_pNotificationManager->Initialize(m_pCoreServices);
	}

	if (IFXSUCCESS(rc))
	{
		rc = IFXCreateComponent(CID_IFXTimeManager, IID_IFXTimeManager, (void**) &m_pTimeManager);
		if (IFXSUCCESS(rc) && !m_pTimeManager)
			rc = IFX_E_OUT_OF_MEMORY;
		if (IFXSUCCESS(rc))
			rc = m_pTimeManager->Initialize(m_pCoreServices);
	}

	//
	// Setup the System Manager
	//

	if (IFXSUCCESS(rc))
	{
		// InputManager Data
		if (IFXSUCCESS(rc))
		{
			IFXTaskData * pInpMgr_Data = NULL;

			rc = IFXCreateComponent(CID_IFXTaskData, IID_IFXTaskData, (void**) &pInpMgr_Data);
			if (IFXSUCCESS(rc))
				pInpMgr_Data->Initialize(m_pCoreServices);
			if (IFXSUCCESS(rc))
				pInpMgr_Data->SetOrigin(m_pSystemManager);

			IFXRELEASE(pInpMgr_Data);
		}

		// SystemManager Data
		if (IFXSUCCESS(rc))
		{
			IFXSchedulerInfo * pSysMgr_Data = NULL;

			rc = IFXCreateComponent(CID_IFXSchedulerInfo, IID_IFXSchedulerInfo, (void**) &pSysMgr_Data);
			if (IFXSUCCESS(rc))
				pSysMgr_Data->Initialize(m_pCoreServices);
			if (IFXSUCCESS(rc))
				pSysMgr_Data->SetOrigin((IFXScheduler *) this);

			if (IFXSUCCESS(rc))
			{
				// use the convenience method
				IFXTaskHandle handle;
				rc = m_pTaskManager->AddTask(m_pSystemManager, pSysMgr_Data, 0, &handle);
			}

			IFXRELEASE(pSysMgr_Data);
		}
	}


	//
	// Setup the Simulation Manager
	//
	if (IFXSUCCESS(rc))
	{
		if (IFXSUCCESS(rc))
		{
			IFXSimulationInfo * pCollMgr_Data = NULL;

			rc = IFXCreateComponent(CID_IFXSimulationInfo, IID_IFXSimulationInfo, (void**) &pCollMgr_Data);
			if (IFXSUCCESS(rc))
			{
				if (IFXSUCCESS(rc))
					pCollMgr_Data->Initialize(m_pCoreServices);
				if (IFXSUCCESS(rc))
					pCollMgr_Data->SetOrigin(m_pSimulationManager);
			}

			IFXRELEASE(pCollMgr_Data);

		}

		// Time Manager
		if (IFXSUCCESS(rc))
		{
			IFXSimulationInfo * pTimeMgr_Data = NULL;

			if (IFXSUCCESS(rc))
			{
				rc = IFXCreateComponent(CID_IFXSimulationInfo, IID_IFXSimulationInfo, (void**) &pTimeMgr_Data);
				if (IFXSUCCESS(rc))
					pTimeMgr_Data->Initialize(m_pCoreServices);
				if (IFXSUCCESS(rc))
					pTimeMgr_Data->SetOrigin(m_pSimulationManager);
			}

			// Add Time Manager to Simulation Manager
			if (IFXSUCCESS(rc))
			{
				IFXTaskManager * pSimTaskMgr = NULL;
				rc = m_pSimulationManager->GetTaskManager(&pSimTaskMgr);
				if (IFXSUCCESS(rc))
				{
					IFXTaskHandle handle;
					rc = pSimTaskMgr->AddTask(m_pTimeManager, pTimeMgr_Data, 0, &handle);
				}
				IFXRELEASE(pSimTaskMgr);
			}

			IFXRELEASE(pTimeMgr_Data);
		}


		// Simulation Manager
		if (IFXSUCCESS(rc))
		{
			IFXSchedulerInfo * pSimMgr_Data = NULL;

			if (IFXSUCCESS(rc))
			{
				rc = IFXCreateComponent(CID_IFXSchedulerInfo, IID_IFXSchedulerInfo, (void**) &pSimMgr_Data);
				if (IFXSUCCESS(rc))
					pSimMgr_Data->Initialize(m_pCoreServices);
				if (IFXSUCCESS(rc))
					pSimMgr_Data->SetOrigin((IFXScheduler *) this);
			}

			// Add Simulation Manager to Scheduler
			if (IFXSUCCESS(rc))
			{
				// use the convenience method
				IFXTaskHandle handle;
				rc = m_pTaskManager->AddTask(m_pSimulationManager, pSimMgr_Data, 0, &handle);
			}
			IFXRELEASE(pSimMgr_Data);
		}

	}

	if (IFXSUCCESS(rc))
	{
		m_initialized = TRUE;
	}
	else
	{
		m_initialized = FALSE;

		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pTaskManager);
		IFXRELEASE(m_pSystemManager);
		IFXRELEASE(m_pSimulationManager);

		IFXRELEASE(m_pNotificationManager);
		IFXRELEASE(m_pTimeManager);
	}

	return rc;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetTaskManager(IFXTaskManager ** ppTaskManager)
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

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetNotificationManager(IFXNotificationManager **p)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (p == NULL)
		return IFX_E_INVALID_POINTER;

	*p = m_pNotificationManager;
	m_pNotificationManager->AddRef();
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetTimeManager(IFXTimeManager **p)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (p == NULL)
		return IFX_E_INVALID_POINTER;

	*p = m_pTimeManager;
	m_pTimeManager->AddRef();
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSystemManager(IFXSystemManager **p)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (p == NULL)
		return IFX_E_INVALID_POINTER;

	*p = m_pSystemManager;
	m_pSystemManager->AddRef();
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::AdvanceSimulationTimeTo(U32 newTime)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	/// @todo: need to integrate this into SimulationManager
	if (IFXSUCCESS(rc))
		rc = IFX_E_UNDEFINED;

	IFXRETURN(rc);
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSimulationFrame(U32 *pFrame)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (pFrame == NULL)
		return IFX_E_INVALID_POINTER;

	IFXClock *pClock = NULL;
	m_pSimulationManager->GetClock(&pClock);
	*pFrame = pClock->GetCurrentFrame();
	IFXRELEASE(pClock);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSimulationState(BOOL *pState)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (!pState)
		return IFX_E_INVALID_POINTER;

	m_pSimulationManager->GetSimulationState(pState);

	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSimulationManager( IFXSimulationManager **ppSimMgr )
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (!ppSimMgr)
		return IFX_E_INVALID_POINTER;

	m_pSimulationManager->AddRef();
	(*ppSimMgr) = m_pSimulationManager;

	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::AdvanceTo(U32 time)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	IFXClock *pClock = NULL;
	m_pSimulationManager->GetClock(&pClock);
	pClock->AdvanceTo(time);
	IFXRELEASE(pClock);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSimulationTime(U32 *pTime)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (pTime == NULL)
		return IFX_E_INVALID_POINTER;

	m_pSimulationManager->GetSimulationTime(pTime);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSimulationTimeDelta(U32 *pTime)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (pTime == NULL)
		return IFX_E_INVALID_POINTER;

	m_pSimulationManager->GetSimulationTimeDelta(pTime);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetSystemTime(U32 *pTime)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	if (pTime == NULL)
		return IFX_E_INVALID_POINTER;

	m_pSimulationManager->GetSystemTime(pTime);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::ResetSimulation(void)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	/// @todo	Consider resetting task times.
	//m_pTimeManager->Reset()
	m_pNotificationManager->Reset();
	IFXClock *pClock = NULL;
	m_pSimulationManager->GetClock(&pClock);
	pClock->Reset();
	IFXRELEASE(pClock);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::ResetSimulationTime(void)
{
	/// @todo: Add notification that the time has been reset.

	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	IFXClock *pClock = NULL;
	m_pSimulationManager->GetClock(&pClock);
	pClock->Reset();
	IFXRELEASE(pClock);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::NotifyPause(void)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	IFXClock *pClock = NULL;
	m_pSimulationManager->GetClock(&pClock);
	pClock->NotifyPause();
	IFXRELEASE(pClock);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::SetSimulationState(BOOL newstate, BOOL auto_reset)
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	IFXClock *pClock = NULL;
	m_pSimulationManager->GetClock(&pClock);
	pClock->SetRunning(newstate);
	pClock->SetAuto(auto_reset);
	IFXRELEASE(pClock);
	return IFX_OK;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetCoreServices(IFXCoreServices **ppCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppCoreServices = m_pCoreServices;
		(*ppCoreServices)->AddRef();
	}

	return rc;
}


//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::Service()
{
	IFXRESULT rc = IFX_OK;
	//IFXUnknown *pOrigin = NULL;
	//IFXTaskData * pData = NULL;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	// Execute TaskManager

	IFXTaskManagerView * pView = NULL;
	IFXSchedulerInfo * pInfo = NULL; /// todo: reuse
	if (IFXSUCCESS(rc))
	{
		if (IFXSUCCESS(rc))
		{
			rc = IFXCreateComponent(CID_IFXSchedulerInfo, IID_IFXSchedulerInfo, (void**) &pInfo);
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
				rc = pInfo->SetPriority(0);
		}
		if (IFXSUCCESS(rc))
		{
			rc = m_pTaskManager->CreateView(pInfo, &pView);
		}
		if (IFXSUCCESS(rc))
		{
			rc = pView->ExecuteAll(pInfo, FALSE);
		}
	}
	IFXRELEASE(pInfo);
	IFXRELEASE(pView);


	return rc;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		rc = m_pTaskManager->Reset();
	}
	if (IFXSUCCESS(rc) && m_pNotificationManager)
	{
		rc = m_pNotificationManager->Reset();
	}

	if( IFXSUCCESS(rc) && m_pSimulationManager )
	{
		rc = m_pSimulationManager->Reset();
	}

	if (IFXSUCCESS(rc))
	{
		IFXRELEASE(m_pSystemManager);
		IFXRELEASE(m_pSimulationManager);
		IFXRELEASE(m_pNotificationManager);
		IFXRELEASE(m_pTimeManager);
		IFXRELEASE(m_pTaskManager);
	}

	return rc;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXScheduler::GetNextTaskHandle(IFXTaskHandle * pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
		*pTaskHandle = m_nextTaskHandle++;

	return rc;
}
