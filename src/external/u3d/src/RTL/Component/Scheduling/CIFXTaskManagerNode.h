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
	@file	CIFXTaskManagerNode.h

			This module defines the CIFXTaskManagerNode class.  It is used to...
*/

#ifndef CIFXTaskManagerNode_H
#define CIFXTaskManagerNode_H

#include "IFXTaskManager.h"
#include "IFXTaskManagerNode.h"
#include "IFXCoreServices.h"

#include "IFXTaskData.h"

///  IFXTaskManagerNode Implementation
class CIFXTaskManagerNode : public IFXTaskManagerNode 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTaskManagerNode_Factory(IFXREFIID riid, void **ppv);
private:

	// IFXCOM
	U32 m_refcount;
	U32 m_initialized;

	IFXCoreServices * m_pCoreServices;

	IFXTask * m_pTask;
	IFXTaskData * m_pTaskData;

	IFXTaskHandle m_taskHandle;
	U32 m_priority;
	BOOL m_enabled;
	BOOL m_markForDelete;
	BOOL m_busy;

	IFXTaskManager * m_pTaskManager;

	IFXTaskManagerNode * m_pPrev;
	IFXTaskManagerNode * m_pNext;

private:

	// Housekeeping
	CIFXTaskManagerNode();
	virtual ~CIFXTaskManagerNode();

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXTaskManagerNode methods
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices);
	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices ** ppCoreServices);
	virtual IFXRESULT IFXAPI  Reset();

	// IFXTaskManagerNode
	virtual IFXRESULT IFXAPI  GetPriority(U32 * pPriority);
	virtual IFXRESULT IFXAPI  SetPriority(U32 priority);
	virtual IFXRESULT IFXAPI  GetEnabled(BOOL * pEnabled);
	virtual IFXRESULT IFXAPI  SetEnabled(BOOL enabled);
	virtual IFXRESULT IFXAPI  SetMarkForDelete(BOOL mark);
	virtual IFXRESULT IFXAPI  GetMarkForDelete(BOOL *pMark);
	virtual IFXRESULT IFXAPI  SetTaskHandle(IFXTaskHandle taskHandle);
	virtual IFXRESULT IFXAPI  GetTaskHandle(IFXTaskHandle * pTaskHandle); // return IFXTASK_HANDLE_INVALID if not defined

	virtual IFXRESULT IFXAPI  SetTask(IFXTask * pTask);
	virtual IFXRESULT IFXAPI  GetTask(IFXTask ** ppTask);
	virtual IFXRESULT IFXAPI  SetTaskData(IFXTaskData * pTaskData);
	virtual IFXRESULT IFXAPI  GetTaskData(IFXTaskData ** ppTaskData);

	virtual IFXRESULT IFXAPI  Execute(IFXTaskData * pTaskData);
	virtual IFXRESULT IFXAPI  MatchPattern(IFXTaskData * pViewPattern, BOOL * pResult);

	virtual IFXRESULT IFXAPI  SetTaskManager(IFXTaskManager * pTaskManager);
	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager);

	// list management methods
	virtual IFXRESULT IFXAPI  SetNext(IFXTaskManagerNode * pNext);
	virtual IFXRESULT IFXAPI  SetPrev(IFXTaskManagerNode * pPrev);
	virtual IFXTaskManagerNode* IFXAPI GetNext_NoAddRef(); // no IFXAPI  AddRef 
	virtual IFXTaskManagerNode* IFXAPI GetPrev_NoAddRef(); // no IFXAPI  AddRef 
};

#endif
