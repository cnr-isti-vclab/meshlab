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
	@file	IFXTaskManager.h

			This header file defines the IFXTaskManager interface and its functionality.
*/

#ifndef __IFXTaskManager_H__
#define __IFXTaskManager_H__

#include "IFXUnknown.h"
#include "IFXCoreServices.h"
#include "IFXTask.h"
#include "IFXTaskData.h"

#include "IFXTaskManagerNode.h"
#include "IFXTaskManagerView.h"

#include "IFXSchedulerTypes.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {AD4A0948-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXTaskManager,
0xad4a0948, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXTaskManager.

	@note	The associated IID is named IID_IFXTaskManager,
			and the CID for the component is CID_IFXTaskManager.
*/
class IFXTaskManager : public IFXUnknown 
{
public:
	// IFXTaskManager
	/**
		This method initializes the IFXTaskManager.

		@param	pCoreServices	A pointer to the CoreServices object.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if the IFXTaskManager object has already been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *) = 0;

	/**
		This method resets the IFXTaskManager, resetting it to a just-initialized state.
		All registered tasks are removed.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the IFXTaskManager object has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  Reset() = 0;

	/**
		This method adds a new task to the task manager. Each registered Task must be
		wrapped by an IFXTaskManagerNode object. When a task is registered, the
		task manager sets the IFXTaskHandle member data on the IFXTaskManagerNode with
		the correct task handle; it can be retrieved with IFXTaskManagerNode::GetTaskHandle.

		@param	pNode	The IFXTaskManagerNode which contains the task to be added.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pNode is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  AddTask(IFXTaskManagerNode * pNode) = 0;

	/**
		This method adds a new task to the task manager. Internally this creates
		an IFXTaskManagerNode to hold the registration data. A task handle is
		returned which identifies the registration, and can be used to remove the
		task from the TaskManager.\n

		The Task Data is a template of the event this task is to handle. Fields which
		are filled with default values (zero, null, etc) are treated as wild fields and
		match any event; fields with a specific value must match the task data
		associated with a particular event in order for the IFXTask to be invoked. The
		registered IFXTaskData is not actually passed to the IFXTask::Execute method;
		rather, the IFXTaskData that is associated with the event is passed to the
		Execute method.

		@param	pTask	The IFXTask pointer.\n
		@param	pData	The IFXTaskData pointer.\n
		@param	priority	The priority for this task. Priority is used to resolve the order of execution when multiple tasks are registered to handle the same event.\n
		@param	pTaskHandle	Returns the task handle.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTask, pData, or pTaskHandle is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  AddTask(IFXTask *pTask, IFXTaskData *pData, U32 priority, IFXTaskHandle * pTaskHandle) = 0;

	/**
		This method creates an IFXTaskManagerView for a given event. An event is defined by an IFXTaskData object.
		This method matches the event to the IFXTaskData "pattern" contained in each registered IFXTaskManagerNode.
		Each match results in the node pointer being added to the IFXTaskManagerView. Thus,
		the View contains all matching task handlers for a given event. Since tasks can be added, removed, and modified at virtually any time,
		a given view is highly time sensitive and should generally not be held.\n

		It is possible for a View to contain no data (i.e., no task nodes). This simply
		indicates that there were no matching tasks.\n

		See IFXTaskManagerView for more information.

		@param	pPattern	The IFXTaskData which defines the event.\n
		@param	ppView	Returns an IFXTaskManagerView. ppView must not be NULL -- the caller must supply a valid pointer - but the pointer should not actually contain a IFXTaskManagerView.
		Rather, an IFXTaskManagerView will be created and returned when this method is called, and the contents of
		ppView will be overwritten.\n

		@note	For efficiency, the Task Manager recycles TaskManagerViews. It will not recycle a view object
		as long as anyone else has a pointer to it, and uses the reference count to verify this.
		For this reason it is very important not to keep a non-reference-counted pointer to a returned IFXTaskManagerView, as
		the contents could change without notice.

		@return	Returns an IFXRESULT code. This may also return errors from IFXCreateComponent if it is necessary to create a new IFXTaskManagerView. It may also return errors from IFXTaskManagerNode::MatchPattern\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pPattern is NULL.\n
		- @b IFX_E_INVALID_HANDLE is returned if ppView is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  CreateView(IFXTaskData * pPattern, IFXTaskManagerView ** ppView) = 0;

	/**
		This method retrieves the IFXTaskManagerNode for a registered task.
		This allows modification of registration data.

		@param	taskHandle	The task handle returned by AddTask.\n
		@param	ppNode	Receives the IFXTaskManagerNode pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppNode is NULL.\n
		- @b IFX_E_INVALID_HANDLE is returned if taskHandle is IFXTASK_HANDLE_INVALID.\n
		- @b IFX_E_NOT_FOUND if the specified task is not found. Since multiple
		components contain an IFXTaskManager, this error may indicate an attempt
		to retrieve a task from the wrong component.\n
	*/
	virtual IFXRESULT IFXAPI  GetTask(U32 taskHandle,  IFXTaskManagerNode ** ppNode) = 0;

	/**
		This method removes a task from the Task Manager.

		@param	taskHandle	The task handle returned by AddTask.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_HANDLE is returned if taskHandle is IFXTASK_HANDLE_INVALID.\n
		- @b IFX_E_NOT_FOUND if the specified task is not found. Since multiple
		components contain an IFXTaskManager, this error may indicate an attempt
		to remove a task from the wrong component.\n
	*/
	virtual IFXRESULT IFXAPI  RemoveTask(U32 taskHandle) = 0;
};

#endif
