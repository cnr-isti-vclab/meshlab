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
//	IFXSystemManager.h
//
//	DESCRIPTION
//		This module defines the IFXSystemManager class.
//
//
//	NOTES
//
//***************************************************************************

/**
	@file	IFXSystemManager.h

			This header file defines the IFXSystemManager interface and its functionality.
*/

#ifndef __IFXSystemManager_H__
#define __IFXSystemManager_H__

#include "IFXUnknown.h"
#include "IFXCoreServices.h"

#include "IFXTask.h"
#include "IFXTaskManager.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {AD4A0944-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXSystemManager,
0xad4a0944, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXSystemManager.

	@note	The associated IID is named IID_IFXSystemManager,
	and the CID for the component is CID_IFXSystemManager.
*/
class IFXSystemManager : public IFXTask {
public:
	// IFXSystemManager
	/**
		This method initializes the IFXSystemManager.

		@param	pCoreServices	A pointer to the CoreServices object.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if the IFXSystemManager object has already been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *) = 0;
	/**
		This method resets the IFXSystemManager, resetting it to a just-initialized state.
		All registered jobs are removed.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the IFXSystemManager object has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  Reset() = 0;
	/**
		This method retrieves the IFXTaskManager for this component.

		@param	ppTaskManager	Receives the IFXTaskManager pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppTaskManager is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager) = 0;

	/**
		This is a convenience method that registers a new System Task.
		It creates an IFXSchedulerInfo object and calls the AddTask method
		on the internal IFXTaskManager. See IFXTaskManager::AddTask for
		more information.

		@param	pTask	The IFXTask pointer.\n
		@param	priority	The priority for this task registration.\n
		@param	pUserData	The user data for this task registration.\n
		@param	pTaskHandle	Returns the task handle.\n

		@return	Returns an IFXRESULT code. Besides this result codes, it
		may also return errors from the IFXTaskManager::AddTask() method.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTask or pTaskHandle is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  RegisterTask(IFXTask *pTask, U32 priority, void * pUserData, IFXTaskHandle * pTaskHandle) = 0;
	/**
		This is a convenience method that unregisters a System Task.
		It calls the RemoveTask method on the internal IFXTaskManager.
		See IFXTaskManager::RemoveTask for more information.

		@param	taskHandle	The task handle.\n

		@return	Returns an IFXRESULT code. Besides this result codes, it
		may also return errors from the IFXTaskManager::RemoveTask() method.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  UnregisterTask(IFXTaskHandle taskHandle) = 0;
};

#endif
