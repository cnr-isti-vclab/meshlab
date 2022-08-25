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
//	CIFXSimulationManager.h
//
//		This module defines the CIFXSimulationManager class.  It is used to...
//
//***************************************************************************

#ifndef __CIFXSimulationManager_H__
#define __CIFXSimulationManager_H__

#include "IFXSimulationManager.h"

#include "IFXCoreServices.h"
#include "IFXScheduler.h"
#include "IFXClock.h"
#include "IFXTaskManager.h"

struct IFXTaskInfo;


class CIFXSimulationManager : public IFXSimulationManager 
{
public:
	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void** ppv);

	// IFXTask
	virtual IFXRESULT IFXAPI  Execute(IFXTaskData* pTaskData);

	// IFXSimulationManager
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices*);
	virtual IFXRESULT IFXAPI  Reset();
	virtual IFXRESULT IFXAPI  GetSimulationFrame(U32* pFrameNumber);
	virtual IFXRESULT IFXAPI  GetSimulationState(BOOL* pState);
	virtual IFXRESULT IFXAPI  GetSimulationTime(U32* pTime);
	virtual IFXRESULT IFXAPI  GetSimulationTimeDelta(U32* pTime);
	virtual IFXRESULT IFXAPI  GetSystemTime(U32* pTime);

	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager** ppTaskManager);
	virtual IFXRESULT IFXAPI  GetClock(IFXClock** ppClock);

	virtual IFXRESULT IFXAPI  RegisterTask(
									IFXTask* pTask, U32 priority, 
									void* pUserData, IFXTaskHandle* pTaskHandle);
	virtual IFXRESULT IFXAPI  UnregisterTask(IFXTaskHandle taskHandle);

	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSimulationManager_Factory(
												IFXREFIID riid, void **ppv);

private:
	CIFXSimulationManager();
	virtual ~CIFXSimulationManager();

	// IFXCOM
	U32 m_refcount;
	U32 m_initialized;

	IFXCoreServices* m_pCoreServices;
	IFXTaskManager* m_pTaskManager;

	IFXClock* m_pClock;
};

#endif
