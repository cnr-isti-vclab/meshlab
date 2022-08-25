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
//	CIFXNotificationManager.h
//
//	DESCRIPTION
//		This module defines the CIFXNotificationManager class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXNotificationManager_H__
#define __CIFXNotificationManager_H__

#include "CIFXScheduler.h"

#include "IFXCoreServices.h"
#include "IFXScheduler.h"
#include "IFXErrorInfo.h"
#include "IFXTaskManager.h"

struct IFXEventIdEntry
{
	U32 key;
	IFXNotificationId id;
	IFXEventIdEntry * pNext;
};

//-------------------------------------------------------------------
//
//  Notification Manager Implementation
//
//-------------------------------------------------------------------
struct JobExt
{
	// basics
	IFXTaskHandle         taskHandle;
	IFXTask             * pTask;
	IFXUnknown          * pUserData; // replaces pAffectedObject, extendable data

	// filtering stuff
	U32                   priority;
	IFXNotificationType   type;
	IFXNotificationId     id;
	U32                   keyFilter;
	U32                   bitFilter;
	IFXUnknown          * pObjectFilter; // replaces pAffectedObject

	// tree/list structure
	JobExt              * pPrep;
	JobExt              * pPost;
	JobExt              * pNext;
	JobExt              * pParent;

	// statistics
	U32                   max_repetition;
	U32                   cur_repetition;
	U32                   last_execution_timestamp;

	// state
	BOOL                  expired;    // flag for delete
	BOOL                  busy;       // job is busy when being executed
	BOOL                  disabled;   // job can be disabled without unregistering
	BOOL                  persistent; // job can be persistent, ignores Reset
};

// NOTE: *** TimeManager now generates Notifications ***
// NotificationManager handles registrations and such


class CIFXNotificationManager : public IFXNotificationManager
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNotificationManager_Factory(IFXREFIID riid, void **ppv);

	// allow creation/access from SchedulerImpl
	friend class CIFXScheduler;

private:
	// IFXCOM
	U32 m_refcount;

	BOOL m_initialized;
	IFXCoreServices * m_pCoreServices;
	IFXTaskManager * m_pTaskManager;

	IFXNotificationType m_nextAutoType;
	IFXNotificationId m_nextAutoId;
	IFXEventIdEntry *m_pIdTable;

	// Housekeeping
	CIFXNotificationManager();
	virtual ~CIFXNotificationManager();

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void **ppv);

	// IFXTask
	virtual IFXRESULT IFXAPI  Execute(IFXTaskData * pTaskData);

	// Manager methods
	virtual IFXRESULT IFXAPI  GetNextType(IFXNotificationType * pType);
	virtual IFXRESULT IFXAPI  GetNextId(IFXNotificationId * pId);
	virtual IFXRESULT IFXAPI  AddEventId(U32 key, IFXNotificationId id);
	virtual IFXRESULT IFXAPI  FindEventId(U32 key, IFXNotificationId * pId);

public:
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *pCoreServices);
	virtual IFXRESULT IFXAPI  Reset();
	virtual IFXRESULT IFXAPI  Cleanup();
	virtual IFXRESULT IFXAPI  SubmitEvent(IFXNotificationInfo * pInfo);
	virtual IFXRESULT IFXAPI  SubmitEvent(IFXNotificationType type, IFXNotificationId id, U32 time, void * pUserData);
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask * pTask, IFXNotificationType type, IFXNotificationId id, IFXTaskHandle *pTaskHandle);
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask * pTask, IFXNotificationType type, IFXNotificationId id, void * pUserData, IFXUnknown *pObject, IFXTaskHandle *pTaskHandle);
	virtual IFXRESULT IFXAPI  UnregisterTask(IFXTaskHandle taskHandle);
	virtual IFXRESULT IFXAPI  SubmitError(IFXErrorInfo * pErrInfo);

	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager);
};


#endif
