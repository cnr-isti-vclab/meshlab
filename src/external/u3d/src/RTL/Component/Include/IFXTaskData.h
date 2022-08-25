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
	@file	IFXTaskData.h

			This header defines the IFXTaskData interface and its
			functionality. There is a default implementation for the
			IFXTaskData interface that is also defined by this header.
*/

#ifndef IFXTaskData_H
#define IFXTaskData_H

#include "IFXUnknown.h"
#include "IFXCoreServices.h"
#include "IFXSchedulerTypes.h"

class IFXTaskManager;

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {76E95C83-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXTaskData,
0x76e95c83, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXTaskData.

	@note	The associated IID is named IID_IFXTaskData, and the
			CID for the default implementation is CID_IFXTaskData.
*/
class IFXTaskData : public IFXUnknown 
{
public:
	/**
		This method is used to initialize an IFXTaskData object.

		@param	pCoreServices	A pointer to an IFXCoreServices interface.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if this object has already been initialized.\n
		- @b IFX_E_INVALID_POINTER if pCoreServices is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices* pCoreServices) = 0;

	/**
		This method returns an IFXCoreServices pointer.

		@param	ppCoreServices	A pointer to a pointer to an IFXCoreServices interface.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if ppCoreServices is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices** ppCoreServices) = 0;

	/**
		This method sets the Origin pointer. The Origin is the object which
		caused the IFXTask::Execute to be invoked, and usually the object
		which created the IFXTaskData object.

		@param	pObject	A pointer to the Origin. NULL is valid.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  SetOrigin(IFXUnknown* pObject) = 0;

	/**
		This method retrieves the Origin pointer.

		@param	pObject	A pointer to a variable which will receive the Origin.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if ppObject is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetOrigin(IFXUnknown** ppObject) = 0;

	/**
		This method sets the TaskManager that manages this TaskData object, if any.
		THIS IS NORMALLY SET AUTOMATICALLY, rather than by the client.

		@param	pTaskManager	A pointer to the TaskManager. NULL is valid.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  SetTaskManager(IFXTaskManager* pTaskManager) = 0;

	/**
		This method retrieves the TaskManager pointer.

		@param	ppTaskManager	A pointer to a variable which will receive the Task Manager.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if ppTaskManager is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager** ppTaskManager) = 0;

	/**
		This method sets the TaskHandle of the registration associated with this
		IFXTaskData instance. The default value is IFXTASK_HANDLE_INVALID.
		THIS IS NORMALLY SET AUTOMATICALLY, rather than by the client.

		@param	taskHandle	An IFXTaskHandle.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  SetTaskHandle(IFXTaskHandle taskHandle) = 0;

	/**
		This method retrieves the TaskHandle associated with this IFXTaskData instance.

		@param	pTaskHandle	A pointer to a variable which will receive the IFXTaskHandle.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if pTaskHandle is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetTaskHandle(IFXTaskHandle* pTaskHandle) = 0;

	/**
		This method sets the UserData pointer. The UserData pointer is ignored
		completely by the system, and if used, is completely the responsibility
		of the client. Clients may use this field to store any value that does
		not exceed the size of a void pointer.

		@param	pUserData	The user data pointer to set. NULL is valid.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  SetUserData(void* pUserData) = 0;

	/**
		This method retrieves the UserData pointer.

		@param	ppUserData	A pointer to a variable which will receive the User Data.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if ppUserData is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetUserData(void** ppUserData) = 0;

	/**
		This method sets the time associated with this IFXTaskData instance.

		@param	time	A time value in milliseconds.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  SetTime(U32 time) = 0;

	/**
		This method retrieves the time associated with this IFXTaskData instance.

		@param	pTime	A pointer to a variable which will receive the time.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if pTime is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetTime(U32* pTime) = 0;

	/**
		This method retrieves the sort key for this IFXTaskData instance. No
		set method is provided, as the sort key is defined and generated by
		the implementation.

		@param	pKey	A pointer to a variable which will receive the key.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if pKey is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetSortKey(U32* pKey) = 0;

	/**
		This method compares this IFXTaskData instance to a reference instance.
		The specific matching algorithm and success conditions are implementation
		defined. If the two instances are determined to match, pResult is set
		to TRUE; if they do not match, pResult is set to FALSE.

		@param	pViewPattern	An interface pointer to another IFXTaskData, against
		which this instance will be compared. If this pointer is NULL, then the
		match is automatically considered to be a success.

		@param	pResult	A pointer to a variable which will be set to TRUE or FALSE,
		according to the results of the match.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if this object has not yet been initialized.\n
		- @b IFX_E_INVALID_POINTER if pViewPattern or pResult is NULL.\n
		- @b IFX_E_UNSUPPORTED if pViewPattern does not support a required interface.\n
	*/
	virtual IFXRESULT IFXAPI  MatchPattern(IFXTaskData* pViewPattern, BOOL* pResult) = 0;
};

#endif

