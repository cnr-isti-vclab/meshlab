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
	@file	CIFXScheduler.h

			This module contains the CIFXScheduler component.
*/

#ifndef __CIFXScheduler_H__
#define __CIFXScheduler_H__

#include "IFXScheduler.h"
#include "IFXCoreServices.h"

#include "CIFXNotificationManager.h"
#include "CIFXTimeManager.h"

#include "IFXNotificationManager.h"
#include "IFXTaskManager.h"
#include "IFXSystemManager.h"


//-------------------------------------------------------------------
//
//  Scheduler Implementation
//
//-------------------------------------------------------------------

struct IFXTaskInfo;

class CIFXScheduler : public IFXScheduler
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXScheduler_Factory(IFXREFIID riid, void **ppv);

	// IFXCOM
	U32 m_refCount;

	BOOL m_initialized;

	// Managers
	IFXCoreServices        * m_pCoreServices;
	IFXTaskManager         * m_pTaskManager;
	IFXSystemManager       * m_pSystemManager; // system manager
	IFXSimulationManager   * m_pSimulationManager; // simulation manager

	IFXNotificationManager * m_pNotificationManager; // notification manager needs a tick
	IFXTimeManager         * m_pTimeManager; // simulation manager

	// Scheduler acts as a "task handle server" for all subcomponents,
	// ensuring that task handles are unique throughout the system.
	IFXTaskHandle m_nextTaskHandle;

	// Housekeeping
	CIFXScheduler();
	virtual ~CIFXScheduler();

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// Manager access methods
	virtual IFXRESULT IFXAPI  GetNotificationManager(IFXNotificationManager **p);
	virtual IFXRESULT IFXAPI  GetTimeManager(IFXTimeManager **p);
	virtual IFXRESULT IFXAPI  GetSystemManager(IFXSystemManager **p);
	virtual IFXRESULT IFXAPI  GetSimulationManager(IFXSimulationManager **ppSimMgr);

	// Scheduler methods
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *);
	virtual IFXRESULT IFXAPI  AdvanceSimulationTimeTo(U32 newTime);
	virtual IFXRESULT IFXAPI  GetSimulationFrame(U32 * pFrameNumber);
	virtual IFXRESULT IFXAPI  GetSimulationState(BOOL *pState);
	virtual IFXRESULT IFXAPI  GetSimulationTime(U32 *pTime);
	virtual IFXRESULT IFXAPI  GetSimulationTimeDelta(U32 *pTime);
	virtual IFXRESULT IFXAPI  GetSystemTime(U32 *pTime);
	virtual IFXRESULT IFXAPI  AdvanceTo(U32 time); // not an IFX interface

	virtual IFXRESULT IFXAPI  ResetSimulation(void);
	virtual IFXRESULT IFXAPI  ResetSimulationTime(void);
	virtual IFXRESULT IFXAPI  NotifyPause(void);
	virtual IFXRESULT IFXAPI  SetSimulationState(BOOL start, BOOL autoreset);

	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices **ppCoreServices);

	virtual IFXRESULT IFXAPI  Service();
	virtual IFXRESULT IFXAPI  Reset();

	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager);
	virtual IFXRESULT IFXAPI  GetNextTaskHandle(IFXTaskHandle * pTaskHandle);
};


#endif


