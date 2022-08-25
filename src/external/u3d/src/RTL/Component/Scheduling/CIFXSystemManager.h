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
//	CIFXSystemManager.h
//
//	DESCRIPTION
//		This module defines the CIFXSystemManager class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXSystemManager_H__
#define __CIFXSystemManager_H__

#include "IFXSchedulerTypes.h"
#include "IFXSystemManager.h"
#include "IFXCoreServices.h"

struct IFXTaskInfo;

//-------------------------------------------------------------------
//
//  IFXSystemManager Implementation
//
//-------------------------------------------------------------------

class CIFXSystemManager : public IFXSystemManager 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSystemManager_Factory(IFXREFIID riid, void **ppv);

private:

	// IFXCOM
	U32 m_refcount;
	U32 m_initialized;

	IFXCoreServices * m_pCoreServices;
	IFXTaskManager * m_pTaskManager;

private:

	// Housekeeping
	CIFXSystemManager();
	virtual ~CIFXSystemManager();

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXTask
	virtual IFXRESULT IFXAPI  Execute(IFXTaskData * pTaskData);

	// IFXSystemManager
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *);
	virtual IFXRESULT IFXAPI  Reset();

	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager);
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask *pTask, U32 priority, void * pUserData, IFXTaskHandle * pTaskHandle);
	virtual IFXRESULT IFXAPI  UnregisterTask(IFXTaskHandle taskHandle);
};

#endif // __CIFXSystemManager_H__

