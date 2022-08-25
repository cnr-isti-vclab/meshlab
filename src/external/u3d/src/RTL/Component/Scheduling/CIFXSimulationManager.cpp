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
	@file	CIFXSimulationManager.cpp

			This module defines the CIFXSimulationManager class.
*/

#include "CIFXSimulationManager.h"

#include "IFXSimulationInfo.h"
#include "IFXSchedulerInfo.h"
#include "IFXSceneGraph.h"
#include "IFXSimulationTask.h"

#include "IFXPalette.h"

#include "IFXSchedulingCIDs.h"
#include "IFXDebug.h"

struct IFXTaskInfo
{
	IFXTask     *pTask;
	IFXTaskHandle taskHandle;
	IFXUnknown    *pUserData;
	U32       priority;
};


//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXSimulationManager_Factory(IFXREFIID riid, void** ppv)
{
	CIFXSimulationManager *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXSimulationManager();

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
CIFXSimulationManager::CIFXSimulationManager()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;
	m_pTaskManager = NULL;
	m_pClock = NULL;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXSimulationManager::~CIFXSimulationManager()
{
	if (m_pTaskManager)
		m_pTaskManager->Reset();

	IFXRELEASE(m_pCoreServices);
	IFXRELEASE(m_pTaskManager);
	IFXRELEASE(m_pClock);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXSimulationManager::AddRef()
{
	return ++m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXSimulationManager::Release()
{
	if (1 == m_refcount)
	{
		delete this;
		return 0;
	}
	return --m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::QueryInterface
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::QueryInterface(IFXREFIID riid, void **ppv)
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
	else if (riid == IID_IFXSimulationManager)
	{
		*ppv = (IFXSimulationManager*) this;
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
// PUBLIC IFXSimulationManager::GetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetTaskManager(IFXTaskManager ** ppTaskManager)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppTaskManager)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		m_pTaskManager->QueryInterface(IID_IFXTaskManager, (void**) ppTaskManager);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::GetTaskManager
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetClock(IFXClock ** ppClock)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppClock)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		m_pClock->QueryInterface(IID_IFXClock, (void**) ppClock);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTask::Execute
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::Execute(IFXTaskData *pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTaskData)
		rc = IFX_E_INVALID_POINTER;

	IFXNotificationInfo* pNI = NULL;
	IFXSchedulerInfo* pSI = NULL;

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
		U32 startTime = m_pClock->GetFrameStart();
		U32 endTime = m_pClock->GetFrameEnd();
		U32 currTime = startTime;

		U32 numSimulators = 0;
		IFXTask** ppTaskArr = NULL;
		U32* pNextTime = NULL;
		BOOL  bSimTasks = FALSE;

		//-------------------------
		// setup the simulator list
		//-------------------------

		{
			IFXPalette * pSimPalette = NULL;
			IFXTaskManagerView * pTaskView = NULL;
			U32 paletteCount = 0;
			U32 simulationCount = 0;

			// get sim palette
			{
				IFXASSERT( m_pCoreServices );
				IFXSceneGraph *pSceneGraph = NULL;
				m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph );
				IFXASSERT( pSceneGraph );
				pSceneGraph->GetPalette( IFXSceneGraph::SIM_TASK, &pSimPalette);
				IFXRELEASE( pSceneGraph );
				IFXASSERT( pSimPalette );
			}

			// get task manager view
			//
			// these are IFXTasks registered with the TaskManager
			// as opposed to the SimPalette. What we want to do is combine
			// all into a single simulator list of IFXTasks.
			//
			{
				// pattern NULL = all registered tasks
				m_pTaskManager->CreateView(NULL,&pTaskView);
				IFXASSERT(pTaskView);
			}

			// determine number of simulators
			{
				pSimPalette->GetPaletteSize( &paletteCount );
				pTaskView->GetSize(&simulationCount);
				numSimulators = paletteCount + simulationCount;

				// create and initialize the array
				//  ppTaskArr = new IFXSimulationTask*[numSimulators+1];
				ppTaskArr = new IFXTask*[numSimulators+1];
				pNextTime = new U32[numSimulators+1];
				IFXASSERT(ppTaskArr);
				IFXASSERT(pNextTime);
				U32 i=0;
				for (i=0; i<numSimulators; i++) 
				{
					ppTaskArr[i] = NULL;
					pNextTime[i] = 0;
				}
			}

			// get the task interfaces for each simulator
			{
				U32 count=0;
				U32 uSimIndex=0;
				if( IFXSUCCESS(pSimPalette->First( &uSimIndex ) ) )
				{
					do 
					{
						IFXSimulationTask *pSimTask = NULL;

						rc = pSimPalette->GetResourcePtr( uSimIndex,
							IID_IFXSimulationTask,
							(void**)&pSimTask );
						if( IFXSUCCESS(rc) )
						{
							IFXTask *pTask = NULL;
							if (IFXSUCCESS( pSimTask->QueryInterface(IID_IFXTask, (void**) &pTask) ))
							{
								ppTaskArr[count++] = pTask;
								bSimTasks = TRUE;
							}
						}

					}
					while( IFXSUCCESS(pSimPalette->Next( &uSimIndex ) ) );
				}

				U32 i;
				// get interfaces from taskview
				for (i=0; i<simulationCount; i++) 
				{
					IFXTask * pTask = NULL;
					IFXTaskManagerNode *pNode = NULL;
					pTaskView->GetElementAt(i,&pNode);
					IFXASSERT(pNode);
					pNode->GetTask(&pTask);

					IFXASSERT(pTask);
					pTask->AddRef();
					ppTaskArr[count++] = pTask;
					bSimTasks = TRUE;

					IFXRELEASE(pNode);
					IFXRELEASE(pTask);
				}

				/// @todo	Code assumes all pSimPalette palette entries are occupied which may not always be the case.
				IFXASSERT(count == numSimulators);
			}

			IFXRELEASE( pTaskView );
			IFXRELEASE( pSimPalette );
		}


		//-------------------------
		// now our arrays are populated
		// execute the simulators
		//-------------------------
		if( bSimTasks)
		{
			//
			// todo: reuse IFXSimulationInfo
			//

			IFXSimulationInfo * pInfo = NULL;

			rc = IFXCreateComponent(CID_IFXSimulationInfo, IID_IFXSimulationInfo, (void**) &pInfo);
			if (IFXSUCCESS(rc))
				rc = pInfo->Initialize(m_pCoreServices);
			if (IFXSUCCESS(rc)) 
			{
				IFXUnknown * pUnknown=NULL;
				rc = QueryInterface(IID_IFXUnknown, (void**) &pUnknown); //this->QueryInterface
				if (IFXSUCCESS(rc))
					rc = pInfo->SetOrigin(pUnknown);
				IFXRELEASE(pUnknown);
			}

			while (currTime < endTime)
			{
				U32 i;
				U32 nextTime = endTime;
				pInfo->SetTime(currTime);

				//
				// QUERY each simulator to determine the next time
				//
				for (i=0; i<numSimulators; i++)
				{
					pInfo->SetTimeInterval(currTime,nextTime);
					pInfo->SetMsg(IFXSimulationMsg_Query);

					if (ppTaskArr[i])
					{
						ppTaskArr[i]->Execute(pInfo);

						pNextTime[i] = pInfo->GetMaxTime();
						if (pNextTime[i] < nextTime)
							nextTime = pNextTime[i];
					}
				}

				// now we know currTime and nextTime
				// ADVANCE all simulators to the currMaxTime
				for (i=0; i<numSimulators; i++)
				{
					pInfo->SetTimeInterval(currTime,nextTime);
					pInfo->SetMsg(IFXSimulationMsg_Advance);

					if (ppTaskArr[i])
					{
						ppTaskArr[i]->Execute(pInfo);
					}
				}

				// UPDATE simulation time to match currTime
				m_pClock->AdvanceTo(nextTime);

				currTime = nextTime;
			}

			IFXRELEASE(pInfo);
		}

		//-------------------------
		// update the clock
		//-------------------------
		m_pClock->AdvanceToEnd();
		m_pClock->IncrementFrame();

		//-------------------------
		// cleanup the simulator list
		//-------------------------

		{
			U32 i;
			for (i=0; i<numSimulators; i++) 
			{
				IFXRELEASE(ppTaskArr[i]);
			}
			delete [] ppTaskArr;
			delete [] pNextTime;
		}
	}

	IFXRELEASE(pNI);
	IFXRELEASE(pSI);

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::Initialize(IFXCoreServices *pCoreServices)
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
		rc = IFXCreateComponent(CID_IFXClock, IID_IFXClock, (void**) &m_pClock);
	}
	if (IFXSUCCESS(rc))
	{
		rc = m_pClock->Initialize(m_pCoreServices);
		if (IFXSUCCESS(rc))
		{
#if 0
			IFXSubject *pSubject = NULL;
			rc = m_pClock->QueryInterface(IID_IFXSubject, (void**)&pSubject );

			IFXASSERT( pSubject );
			IFXSceneGraph *pSceneGraph = NULL;
			m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph );

			IFXASSERT( pSceneGraph );
			pSceneGraph->SetSimClockSubject( pSubject );

			IFXRELEASE( pSceneGraph );
			IFXRELEASE( pSubject );
#endif
			// HACK: set clock to auto start
			m_pClock->SetRunning(FALSE);
			m_pClock->SetAuto(TRUE);
		}
	}

	if (IFXSUCCESS(rc))
	{
		m_initialized = TRUE;
	}
	else
	{
		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pTaskManager);
		IFXRELEASE(m_pClock);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->Reset();

	if (IFXSUCCESS(rc))
		rc = m_pClock->Reset();

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::XXX
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetSimulationFrame(U32 * pFrameNumber)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pFrameNumber)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pFrameNumber = m_pClock->GetCurrentFrame();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::GetSimulationState
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetSimulationState(BOOL *pState)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pState)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pState = m_pClock->IsRunning();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::GetSimulationTime
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetSimulationTime(U32 *pTime)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTime)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pTime = m_pClock->GetSimulationTime();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::GetSimulationTimeDelta
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetSimulationTimeDelta(U32 *pTime)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTime)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pTime = m_pClock->GetSimulationTimeDelta();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXSimulationManager::GetSystemTime
//---------------------------------------------------------------------
IFXRESULT CIFXSimulationManager::GetSystemTime(U32 *pTime)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTime)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		m_pClock->Update();
		*pTime = m_pClock->GetSystemTime();
	}

	return rc;
}


// convenience method
IFXRESULT CIFXSimulationManager::RegisterTask(IFXTask *pTask, U32 priority, void * pUserData, IFXTaskHandle * pTaskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pTask)
		rc = IFX_E_INVALID_POINTER;
	else if (!pTaskHandle)
		rc = IFX_E_INVALID_POINTER;

	IFXSimulationInfo * pInfo = NULL;

	if (IFXSUCCESS(rc))
		rc = IFXCreateComponent(CID_IFXSimulationInfo, IID_IFXSimulationInfo, (void**) &pInfo);
	if (IFXSUCCESS(rc))
		rc = pInfo->Initialize(m_pCoreServices);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetPriority(priority);
	if (IFXSUCCESS(rc))
		rc = pInfo->SetUserData(pUserData);
	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->AddTask(pTask, pInfo, priority, pTaskHandle); // Simulation Task

	IFXRELEASE(pInfo);

	return rc;
}

// convenience method
IFXRESULT CIFXSimulationManager::UnregisterTask(IFXTaskHandle taskHandle)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		rc = m_pTaskManager->RemoveTask(taskHandle);

	return rc;
}
