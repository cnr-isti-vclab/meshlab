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
	@file	IFXTaskManagerNode.h

			This header defines the IFXTaskManagerNode interface and its functionality.
*/

#ifndef __IFXTaskManagerNode_H__
#define __IFXTaskManagerNode_H__

#include "IFXUnknown.h"
#include "IFXSchedulerTypes.h"
#include "IFXCoreServices.h"

#include "IFXTask.h"

class IFXTaskManager; // forward decl

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {5D9997B1-BC3A-11d5-9AE6-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXTaskManagerNode,
0x5d9997b1, 0xbc3a, 0x11d5, 0x9a, 0xe6, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXTaskManagerNode.\n

	In the documentation for this class, "job" refers to the registered task and
	its associated data. Each job includes an IFXTask which will be executed to handle an event,
	an IFXTaskData which serves as an event template, a priority to control order of execution
	with respect to other jobs, and an IFXTaskHandle that uniquely identifies the job. IFXTaskHandles are
	unique across all task managers.

	@note	The associated IID is named IID_IFXTaskManagerNode,
	and the CID for the component is CID_IFXTaskManagerNode.
*/
class IFXTaskManagerNode : public IFXUnknown 
{
public:
	// Initialization
	/**
		This method initializes the IFXTaskManagerNode.

		@param	pCoreServices	A pointer to the CoreServices object.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if the IFXTaskManagerNode object has already been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
	*/
	virtual IFXRESULT IFXAPI Initialize(IFXCoreServices * pCoreServices) = 0;
	/**
		This method retrieves the IFXCoreServices pointer.

		@param	ppCoreServices	Receives the IFXCoreServices pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppCoreServices is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetCoreServices(IFXCoreServices ** ppCoreServices) = 0;
	/**
		This method resets the IFXTaskManagerNode, resetting it to a just-initialized state.
		All data is cleared.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the IFXTaskManagerNode object has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI Reset() = 0;

	// IFXTaskManagerNode
	/**
		This method retrieves the priority for this job.

		@param	pPriority	Receives the priority.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pPriority is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetPriority(U32 * pPriority) = 0;
	/**
		This method sets the priority for this job.

		@param	priority	The priority.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI SetPriority(U32 priority) = 0;
	/**
		This method retrieves the enabled flag for this job.

		@param	pEnabled	Receives the enabled flag.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pEnabled is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetEnabled(BOOL * pEnabled) = 0;
	/**
		This method sets the enabled flag for this job. If the enabled flag is
		FALSE, the job is disabled and will not executed. This is useful for
		temporarily suspending a job without removing it altogether from a task manager.

		@param	enabled	The enabled flag.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI SetEnabled(BOOL enabled) = 0;
	/**
		This method retrieves the deletion flag for this job.

		@param	pFlag	Receives the deletion flag.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pFlag is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetMarkForDelete(BOOL *pFlag) = 0;
	/**
		This method marks a job for deletion. A job is sometimes busy and
		removing it would be unsafe. This flag is used to mark a job for
		deletion at the earliest safe point.

		@param	flag	The deletion flag.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI SetMarkForDelete(BOOL flag) = 0;

//	virtual IFXRESULT IFXAPI SetTaskHandle(IFXTaskHandle taskHandle) = 0;
	/**
		This method retrieves the task handle for this job. The task handle is assigned
		by the task manager when the job is registered and cannot be changed.

		@param	pTaskHandle	Receives the task handle.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTaskHandle is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetTaskHandle(IFXTaskHandle * pTaskHandle) = 0; // return INVALID_IFXTASK_HANDLE if not defined
	/**
		This method sets the task handler for this job. The task handler
		must support the IFXTask interface, and will be called via IFXTask::Execute.
		Every job must have a task handler.

		@param	pTask	The IFXTask interface of the task handler.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTask is NULL.\n
	*/
	virtual IFXRESULT IFXAPI SetTask(IFXTask * pTask) = 0;
	/**
		This method retrieves the task handler for this job.

		@note	While it is possible to retrieve the IFXTask interface and call
		its Execute method directly, it is usually preferable to use the
		IFXTaskManagerNode::Execute method instead. See IFXTaskManagerNode::Execute
		for more information.

		@param	ppTask	Receives the task handler.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppTask is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetTask(IFXTask ** ppTask) = 0;
	/**
		This method sets the task data for this job. The task data of a registered job
		serves as an event template to identify which events the job should handle.
		The task data registered in an IFXTaskManagerNode is never passed to
		IFXTask::Execute.

		@param	pTaskData	The task data (event template).\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTaskData is NULL.\n
	*/
	virtual IFXRESULT IFXAPI SetTaskData(IFXTaskData * pTaskData) = 0;
	/**
		This method retrieves the task data (event template) for this job.

		@param	ppTaskData	Receives the task data.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppTaskData is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetTaskData(IFXTaskData ** ppTaskData) = 0;

	/**
		This method calls IFXTask::Execute.\n

		The UserData stored in pTaskData is ignored. Each Node has an IFXTaskData "template" which also contains a user data field,
		and this method copies the registered user data into the actual event user data. This allows clients to specify custom data
		for each registered event which will be passed to the IFXTask::Execute method.\n

		Execute fails if the job is currently busy; that is, it is currently being executed.
		You cannot call Execute recursively.\n

		Execute also fails if the job is disabled. A job is disabled if the SetEnabled flag is false.\n

		If the IFXTask::Execute method returns an error result, the reported IFXRESULT is sent to the NotificationManager for handling; the result
		itself is then discarded and is not returned from this method. See IFXErrorInfo for more information.\n

		@param	pTaskData	The event. This IFXTaskData will be passed directly to IFXTask::Execute after setting the user data field.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTaskData is NULL.\n
		- @b IFX_E_ABORTED if the job is currently busy or if it is disabled.\n
	*/
	virtual IFXRESULT IFXAPI Execute(IFXTaskData * pTaskData) = 0;
	/**
		This method matches an event with the pattern stored in the node.\n

		An event is generated by an external source and is defined by an IFXTaskData object.
		The registered job also has an IFXTaskData object which serves as as event template.
		This method compares the template to the actual event, and if they match,
		returns TRUE. If they do not match, it returns FALSE. (See IFXTaskData)\n

		@param	pEvent	An event which is to be compared to this node's TaskData pattern.\n
		@param	pResult	Receives the result of the comparison, either TRUE or FALSE.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pViewPattern or pResult is NULL.\n
	*/
	virtual IFXRESULT IFXAPI MatchPattern(IFXTaskData * pEvent, BOOL * pResult) = 0;

	/**
		This method sets the task manager for this node. Each node keeps a pointer to the task manager that it is registered with.

		@param	pTaskManager	The task manager.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTaskManager is NULL.\n
	*/
	virtual IFXRESULT IFXAPI SetTaskManager(IFXTaskManager * pTaskManager) = 0;
	/**
		This method retrieves the task manager for this node.
		Each node keeps a pointer to the task manager that it is registered with.

		@param	ppTaskManager	Receives the task manager pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppTaskManager is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetTaskManager(IFXTaskManager ** ppTaskManager) = 0;

	// list management methods
	/**
		This is a list-management method for internal use by the task manager.
		Clients should not use this method. See the TaskManager source code.

		@param	pSetNext	An IFXTaskManagerNode pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI SetNext(IFXTaskManagerNode * pSetNext) = 0;
	/**
		This is a list-management method for internal use by the task manager.
		Clients should not use this method. See the TaskManager source code.

		@param	pArg	An IFXTaskManagerNode pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI SetPrev(IFXTaskManagerNode * pSetPrev) = 0;
	/**
		This is a list-management method for internal use by the task manager.
		Clients should not use this method. See the TaskManager source code.

		@return	Returns an IFXTaskManagerNode pointer.\n
	*/
	virtual IFXTaskManagerNode* IFXAPI GetNext_NoAddRef() = 0; // no AddRef
	/**
		This is a list-management method for internal use by the task manager.
		Clients should not use this method. See the TaskManager source code.

		@return	Returns an IFXTaskManagerNode pointer.\n
	*/
	virtual IFXTaskManagerNode* IFXAPI GetPrev_NoAddRef() = 0; // no AddRef
};

#endif
