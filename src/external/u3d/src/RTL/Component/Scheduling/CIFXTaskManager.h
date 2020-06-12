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
//	CIFXTaskManager.h
//
//	DESCRIPTION
//		This module defines the CIFXTaskManager class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXTaskManager_H__
#define __CIFXTaskManager_H__

#include "IFXSchedulerTypes.h"
#include "IFXCoreServices.h"
#include "IFXScheduler.h"
#include "IFXTaskManager.h"

//-------------------------------------------------------------------
//
//  IFXTaskManager Implementation
//
//-------------------------------------------------------------------

class CIFXTaskManager : public IFXTaskManager 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTaskManager_Factory(IFXREFIID riid, void **ppv);

private:

	// IFXCOM
	U32 m_refcount;
	U32 m_initialized;

	IFXCoreServices * m_pCoreServices;
	IFXUnknown * m_pOrigin;

	IFXTaskManagerNode * m_pTaskList;
	IFXTaskManagerView * m_pLastView;

	// funnel all changes to m_pTaskList through this method
	// so that we can make sure it gets refcounted properly
	void SetTaskListRoot(IFXTaskManagerNode * pNode);

private:

	// Housekeeping
	CIFXTaskManager();
	virtual ~CIFXTaskManager();

	void DisposeList(IFXTaskManagerNode ** ppNode);

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXTaskManager
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *);
	virtual IFXRESULT IFXAPI  Reset();
	virtual IFXRESULT IFXAPI  SetOrigin(IFXUnknown * pOrigin);
	virtual IFXRESULT IFXAPI  GetOrigin(IFXUnknown ** ppOrigin);

	virtual IFXRESULT IFXAPI  AddTask(IFXTaskManagerNode * pNode);
	virtual IFXRESULT IFXAPI  AddTask(IFXTask *pTask, IFXTaskData *pData, U32 priority, IFXTaskHandle * pTaskHandle);
	virtual IFXRESULT IFXAPI  CreateView(IFXTaskData * pPattern, IFXTaskManagerView ** ppView);
	virtual IFXRESULT IFXAPI  GetTask(U32 taskHandle,  IFXTaskManagerNode ** ppNode);
	virtual IFXRESULT IFXAPI  RemoveTask(U32 taskHandle);
};

#endif // __CIFXTaskManager_H__

