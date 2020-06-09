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
//	IFXTaskManagerView.h
//
//	DESCRIPTION
//		This module defines the IFXTaskManagerView class.  It is used to...
//
//	NOTES
//
//***************************************************************************

/**
	@file	IFXTaskManagerView.h

			This header file defines the IFXTaskManagerView interface and its functionality.
*/

#ifndef __IFXTaskManagerView_H__
#define __IFXTaskManagerView_H__

#include "IFXUnknown.h"
#include "IFXCoreServices.h"

#include "IFXTaskManagerNode.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {5D9997B3-BC3A-11d5-9AE6-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXTaskManagerView,
0x5d9997b3, 0xbc3a, 0x11d5, 0x9a, 0xe6, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXTaskManagerView.

	@note	The associated IID is named IID_IFXTaskManagerView,
	and the CID for the component is CID_IFXTaskManagerView.
*/

class IFXTaskManagerView : public IFXUnknown {
public:
	// Initialization
	/**
		This method initializes the IFXTaskManagerView.

		@param	pCoreServices	A pointer to the CoreServices object.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if the IFXTaskManagerView object has already been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices) = 0;
	/**
		This method retrieves the IFXCoreServices pointer.

		@param	ppCoreServices	Receives the IFXCoreServices pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppCoreServices is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices ** ppCoreServices) = 0;

	// IFXTaskManagerView
	/**
		This method returns the reference count for this component.
		It is not intended for client use.

		@return	Returns the current reference count.\n
	*/
	virtual U32 IFXAPI  GetRefCount() = 0; // for re-use purposes, clone if someone else has a lock on it
	/**
		This method resets the component, clearing all data and releasing any nodes it contains.
		It is used by the task manager to recycle a View for reuse.

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the View object has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI  Reset() = 0; // clear array, allow for re-using object

	/**
		This method sets the allocation step size for this component.
		It is used by the TaskManager and is not intended for client use.
		Changing with this value could cause severe memory inefficiency in
		a recycled IFXTaskManagerView. A small value can result in many small
		allocations, while a large value can result in allocating too much memory.
		Actual allocation is handled internally by the View object. This method may
		trigger a reallocation.

		@note	Clients should never change this value.

		@param	n	The new allocation step size.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_OUT_OF_MEMORY is returned if a reallocation failed.\n
	*/
	virtual IFXRESULT IFXAPI  SetAlloc(U32 n) = 0; // set allocation size
	/**
		This method adds a node to the View. From time to time this may result
		in memory reallocation if the internal buffer is not large enough.
		Nodes are refcounted by the View.

		@param	pNode	The node to be added.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pNode is NULL.\n
		- @b IFX_E_OUT_OF_MEMORY is returned if a reallocation failed.\n
	*/
	virtual IFXRESULT IFXAPI  AddTask(IFXTaskManagerNode * pNode) = 0; // set allocation size

	/**
		This method returns the current number of nodes contained in this view.
		This will be zero after a reset, or if a IFXTaskManager::CreateView
		resulted in an empty handler list.

		@param	pSize	Receives size.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pSize is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  GetSize(U32 * pSize) = 0;
	/**
		This method retrieves the specified node.

		@param	index	The index of the node to retrieve. This value must greater than or equal to zero, and less than the view size. The ViewSize can be queried through the GetSize method.\n
		@param	ppNode	Receives the specified node pointer.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if ppNode is NULL.\n
		- @b IFX_E_INVALID_RANGE is returned if index is less than zero, or greater than or equal to the view size.\n
	*/
	virtual IFXRESULT IFXAPI  GetElementAt(U32 index, IFXTaskManagerNode ** ppNode) = 0;

	/**
		This method executes all IFXTasks in the view. This is a convenience method
		which performs the normal case of applying an IFXTaskData to all tasks in the view.
		The IFXTaskData submitted to this method is passed directly to the Execute methods
		of each IFXTask in the view. It is safe to call this method on an empty view (that is, one with a size of zero).

		@note	This method actually calls the Execute method on each IFXTaskManagerNode within the view,
		rather than calling IFXTask::Execute directly. See IFXTaskManagerNode for more information.

		@param	pData			The task data which defines an event.
		@param	submitErrors	If TRUE, an error will be submitted to the 
								Notification Manager for each task whose 
								IFXTask::Execute function fails.  Otherwise, 
								such errors are discarded.

		@return	Returns an IFXRESULT code. May return other error codes from IFXTaskManagerNode::Execute.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the view has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pData is NULL.\n
	*/
	virtual IFXRESULT IFXAPI  ExecuteAll(IFXTaskData * pData, BOOL submitErrors) = 0;
};

#endif

