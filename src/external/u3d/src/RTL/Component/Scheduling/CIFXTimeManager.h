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
//  CIFXTimeManager.h
//
//  DESCRIPTION
//  This module defines the CIFXTimeManager class.  The class implements
//  user-defined timers which trigger system notifications. The Time Manager
//  is intended to be plugged into a Simulation Manager. It does not
//  support a look-ahead or rewind mechanism, so should always be the
//  last item in the Sim Mgr chain. (see Sim Mgr for details)
//
//  Use SetTimer to create a new timer, and KillTimer to delete a timer.
//  Timers which reach their maximum duration are automatically removed.
//
//  The Execute method expects an IFXTaskData with a valid "time". This
//  time is used to determine which timers to activate.
//
//  NOTES
//
//  The Time Manager relies on IFXNotificationManager to dispatch
//  system notifications.
//
//***************************************************************************

#ifndef __CIFXTimeManager_H__
#define __CIFXTimeManager_H__


#include "CIFXClock.h"
#include "CIFXScheduler.h"

#include "IFXClock.h"
#include "IFXCoreServices.h"
#include "IFXScheduler.h"
#include "IFXTaskManager.h"


//-------------------------------------------------------------------
//
//  Time Manager Implementation
//
//-------------------------------------------------------------------

struct CIFXTimer {
	U32 timer_id;
	IFXNotificationId message_id;

	U32 start;
	U32 period;
	U32 duration;

	U32 lastTime;
	U32 nextTime;

	BOOL busy; // needed?
	BOOL kill; // needed?

	BOOL auto_nm; // auto-register with notification manager
	IFXTaskHandle auto_handle;

	CIFXTimer * pNext;
};

//-------------------------------------------------------------------
//
//  Time Manager Implementation
//
//-------------------------------------------------------------------

class CIFXTimeManager : public IFXTimeManager 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTimeManager_Factory(IFXREFIID riid, void **ppv);

	// allow creation/access from SchedulerImpl
	friend class CIFXScheduler;

private:
	U32 m_refcount;
	BOOL m_initialized;

	IFXCoreServices * m_pCoreServices;
	IFXTaskManager * m_pTaskManager;

private:
	// Housekeeping
	CIFXTimeManager();
	virtual ~CIFXTimeManager();

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXTask
	virtual IFXRESULT IFXAPI  Execute(IFXTaskData * pTaskData);

	// Manager methods
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices);
	virtual IFXRESULT IFXAPI  Reset();
	virtual IFXRESULT IFXAPI  Cleanup();

	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager);

private:
	CIFXTimer * m_pTimerList; // array of timers
	CIFXTimer * m_pTimerFreeList;
	U32 m_sizeFreeList;
	U32 m_nextTimerId;
	BOOL m_busy;

public:
	virtual IFXRESULT IFXAPI  SetTimer(IFXNotificationId id, U32 start, U32 period, U32 duration, U32 * pTimerId);
	virtual IFXRESULT IFXAPI  SetAutoTimer(IFXNotificationId id, U32 start, U32 period, U32 duration, U32 * pTimerId, BOOL bAuto, IFXTaskHandle taskHandle);
	virtual IFXRESULT IFXAPI  KillTimer(U32 timerId);
	virtual IFXRESULT IFXAPI  InsertTimer(CIFXTimer *pTimer);
	virtual CIFXTimer * PopTimer();
	virtual IFXRESULT IFXAPI  DeleteTimer(CIFXTimer *pTimer);
	virtual CIFXTimer * NewTimer();

	// convenience
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask *pTask, U32 start, U32 period, U32 duration, IFXTaskHandle * pTaskHandle);
	virtual IFXRESULT IFXAPI  UnregisterTask(IFXTaskHandle taskHandle);

protected:
	IFXRESULT QueryInterval(U32 start, U32 end, U32 *pMax);
	IFXRESULT AdvanceInterval(U32 start, U32 end);
};


#endif


