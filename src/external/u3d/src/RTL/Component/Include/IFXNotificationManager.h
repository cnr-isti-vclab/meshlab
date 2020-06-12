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
	@file	IFXNotificationManager.h

			This header defines the IFXNotificationManager interface and its 
			functionality.
*/


#ifndef IFXNotificationManager_h
#define IFXNotificationManager_h


#include "IFXCoreServices.h"
#include "IFXErrorInfo.h"
#include "IFXNotificationInfo.h"
#include "IFXSchedulerTypes.h"
#include "IFXTask.h"
#include "IFXTaskManager.h"
#include "IFXUnknown.h"


// {AD4A0940-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXNotificationManager,
0xad4a0940, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);


/**
	This is the main interface for IFXNotificationManager.

	@note	The associated IID is named IID_IFXNotificationManager, and the CID
	for the Notification Manager component is CID_IFXNotificationManager.
*/
class IFXNotificationManager : public IFXTask {
public:
	/**
		This method retrieves the next available IFXNotificationType.
		See IFXNotificationInfo for details and usage.\n

		@param	pType	Receives the IFXNotificationType.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pType is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetNextType(IFXNotificationType * pType) = 0;
	/**
		This method retrieves the next available IFXNotificationId.
		See IFXNotificationInfo for details and usage.\n

		@param	pId	Receives the IFXNotificationId.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pId is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetNextId(IFXNotificationId * pId) = 0;

	/** Internal Use */
	virtual IFXRESULT IFXAPI  AddEventId(U32 key, IFXNotificationId id) = 0;
	/** Internal Use */
	virtual IFXRESULT IFXAPI  FindEventId(U32 key, IFXNotificationId * pId) = 0;


	// NEW NOTIFICATION MANAGER

	/**
		This method initializes the Scheduler component.

		@param	pCoreServices	A pointer to an IFXCoreServices interface.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
		- @b IFX_E_ALREADY_INITIALIZED if the Scheduler has already been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices) = 0;

	/**
		This method resets the Notification Manager.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the Notification Manager has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  Reset() = 0;

	/** Internal Use. */
	virtual IFXRESULT IFXAPI  Cleanup() = 0;

	/**
		This method accepts an event defined by IFXNotificationInfo and
		invokes any IFXTasks registered with the Task Manager for that event.
		The IFXNotificationInfo passed to this method is matched against
		the notification info associated with each task registration, and
		in the case of a match, is passed on to IFXTask::Execute().

		@param	pInfo	A valid IFXNotificationInfo instance which will be passed to the IFXTask Execute method.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the Notification Manager has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if pInfo is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  SubmitEvent(IFXNotificationInfo * pInfo) = 0;

	/**
		This method accepts an error event defined by IFXErrorInfo and 
		invokes any IFXTasks registered with the Task Manager for events 
		with type IFXNotificationType_Error.  A IFXNotificationInfo is 
		generated from the contained information and is matched against
		the notification info associated with each task registration, and
		in the case of a match, is passed on to IFXTask::Execute().

		@param	pInfo	A valid IFXErrorInfo instance which will be passed 
						to the IFXTask Execute method.

		@return	Upon success, IFX_OK is returned.  Otherwise one of the 
				following errors is returned:  IFX_E_INVALID_POINTER and 
				IFX_E_NOT_INITIALIZED.
	*/
	virtual IFXRESULT IFXAPI  SubmitError(IFXErrorInfo * pErrInfo) = 0;

	/**
		This is a convenience method which creates an IFXNotificationInfo
		instance, initializes it with the provided data, and then submits
		it. See SubmitEvent(IFXNotificationInfo) for more details on
		submitting events. This method is recommended when additional
		functionality is not required, since the Notification Manager
		may

		@param	type	The notification type for this event.\n
		@param	id		The notification id for this event.\n
		@param	time	The time associated with this event. If no time is associated with this event, this should be zero.\n
		@param	pUserData	The user data for this event. This is not interpreted by the system, and may be NULL.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the Notification Manager has not yet been initialized.\n
		- @b IFX_E_OUT_OF_MEMORY if the IFXNotificationInfo instance could not be created.\n
	*/
	virtual IFXRESULT IFXAPI  SubmitEvent(IFXNotificationType type, IFXNotificationId id, U32 time, void * pUserData) = 0;
	/**
		This is a convenience method which creates an IFXTaskManagerNode object,
		initializes it with the provided data, and registers it with the Task Manager.
		The task handle is returned.

		@param	pTask	A pointer to the IFXTask interface which is to be registered.\n
		@param	type	The notification type which will trigger the Task. IFXNotificationType_Nil is a 'wildcard' which matches any type.\n
		@param	id		The notification id which will trigger the Task. IFXNotificationId_Nil is a 'wildcard' which matches any id.\n
		@param	pTaskHandle	A pointer to a variable which will receive the task handle.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_INVALID_POINTER if pTask or pTaskHandle is NULL.\n
		- @b IFX_E_NOT_INITIALIZED if the Notification Manager has not yet been initialized.\n
		- @b IFX_E_OUT_OF_MEMORY if the IFXTaskManagerNode could not be created.\n
	*/
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask * pTask, IFXNotificationType type, IFXNotificationId id, IFXTaskHandle *pTaskHandle) = 0;
	/** Same as Register Task, but also allows specifying optional user data and Object Filter. See IFXNotificationInfo for more details. */
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask * pTask, IFXNotificationType type, IFXNotificationId id, void * pUserData, IFXUnknown *pObject, IFXTaskHandle *pTaskHandle) = 0;
	/**
		A convenience method which calls IFXTaskManager::RemoveTask.
		The task handle to unregister a task was previously obtained from RegisterTask,
		or from IFXTaskManager::AddTask.

		@param	taskHandle	The taskhandle of the task to unregister.\n

		@return	Returns an IFXRESULT code. Since this method also calls the
		IFXTaskManager::RemoveTask method internally, any of the result codes
		returned by that method may also be returned by this method.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  UnregisterTask(IFXTaskHandle taskHandle) = 0;

	/**
		This method retrieves the IFXTaskManager interface from the associated
		task manager.

		@param	ppTaskManager	A pointer to a variable which will receive
		the IFXTaskManager interface.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_INVALID_POINTER if ppTaskManager is NULL.\n
		- @b IFX_E_NOT_INITIALIZED if the Notification Manager has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager) = 0;
};


#endif


