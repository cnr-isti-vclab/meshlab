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
	@file	IFXSimulationManager.h

			This header file defines the IFXSimulationManager interface and its functionality.

			a.k.a. "Integration Manager"
*/

#ifndef __IFXSimulationManager_H__
#define __IFXSimulationManager_H__

#include "IFXUnknown.h"
#include "IFXCoreServices.h"

#include "IFXTask.h"
#include "IFXTaskManager.h"
#include "IFXClock.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {76E95C80-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXSimulationManager,
0x76e95c80, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXSimulationManager.

	@note	The associated IID is named IID_IFXSimulationManager,
	and the CID for the component is CID_IFXSimulationManager.
*/
class IFXSimulationManager : public IFXTask 
{
public:
	// IFXSimulationManager
	/**
		This method initializes the IFXSimulationManager.

		@param	pCoreServices	A pointer to the CoreServices object.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if the IFXSimulationManager object has already been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
	*/
	virtual IFXRESULT IFXAPI Initialize(IFXCoreServices *) = 0;

	/**
		This method resets the IFXSimulationManager, resetting it to a just-initialized state.
		All registered jobs are removed.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the IFXSimulationManager object has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI Reset() = 0;

	/**
		This method retrieves the IFXTaskManager for this component.

		@param	ppTaskManager	Receives the IFXTaskManager pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppTaskManager is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetTaskManager(IFXTaskManager ** ppTaskManager) = 0;

	/**
		This method retrieves the current frame for the simulation.

		@param	pFrameNumber	Receives the frame index.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pFrameNumber is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetSimulationFrame(U32 * pFrameNumber) = 0;

	/**
		This method retrieves the current state for this simulation.
		The state is TRUE if the simulation is running, and FALSE if it is stopped.

		@param	pState	Receives the simulation state.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pState is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetSimulationState(BOOL *pState) = 0;

	/**
		This method retrieves the currnt time for this simulation.
		The simulation time is measured in milliseconds, and is measured from
		the beginning of the simulation (zero).

		@param	pTime	Receives the simulation time.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTime is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetSimulationTime(U32 *pTime) = 0;
	/**
		This method retrieves the time delta for this simulation.
		The time delta is the time since the beginning of the current frame.

		@param	pTimeDelta	Receives the time delta.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTimeDelta is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetSimulationTimeDelta(U32 *pTimeDelta) = 0;

	/**
		This method retrieves the system time. The system time is the actual
		milliseconds since the simulation began. Simulations can be stopped and
		restarted, which is treated as a pause; during a pause, simulation time
		does not advance, but system time does continue to advance.

		@param	)	Receives the system time.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ) is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetSystemTime(U32 *pTime) = 0;

	/**
		This is a convenience method which registers a new simulation task.
		Internally this creates a new IFXSimulationInfo and calls IFXTaskManager::AddTask.

		@param	pTask	The IFXTask which is to be registered.\n
		@param	priority	The required priority of the new task.\n
		@param	pUserData	A user data pointer.\n
		@param	pTaskHandle	Receives a unique task handle which identifies
		the registration and can be used to unregister the task.\n

		@return	Returns an IFXRESULT code. Since this method also calls the
		IFXTaskManager::AddTask method internally, any of the result codes
		returned by that method may also be returned by this method.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pTask or pTaskHandle is NULL.\n
	*/
	virtual IFXRESULT IFXAPI RegisterTask(IFXTask *pTask, U32 priority, void * pUserData, IFXTaskHandle * pTaskHandle) = 0;

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
	virtual IFXRESULT IFXAPI UnregisterTask(IFXTaskHandle taskHandle) = 0;

	/**
		This method retrieves the IFXClock for this component.
		Each simulation has an IFXClock which controls the simulation.

		@param	ppClock	Receives the IFXClock pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppClock is NULL.\n
	*/
	virtual IFXRESULT IFXAPI GetClock(IFXClock ** ppClock) = 0;
};

#endif
