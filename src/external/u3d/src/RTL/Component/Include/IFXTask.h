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
//	IFXTask.h
//
//	DESCRIPTION
//		This module defines the IFXTask class.
//
//	NOTES
//
//***************************************************************************

/**
	@file	IFXTask.h

			This header defines the IFXTask interface and its functionality.
*/

#ifndef __IFXTask_H__
#define __IFXTask_H__

#include "IFXTaskData.h"
#include "IFXUnknown.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {76E95C82-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXTask,
0x76e95c82, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXTask.

	@note	The associated IID is named IID_IFXTask.
*/
class IFXTask : virtual public IFXUnknown
{
public:
	/**
		This method is executed by the object with which the task is registered.
		The IFXTaskData interface may be extended to include additional data if
		needed.

		@param	pTaskData	A pointer to an implementation of the IFXTaskData
							interface. It contains all required invocation data
							for the Execute method.

		@return	Returns an IFXRESULT code. Besides the following standard
			return codes, the Execute method may return an implementation
			specific error code. Errors returned by an Execute method are
			ignored unless an error listener has been registered with the
			Notification Manager.\n

		- @b IFX_OK upon success.\n

		- @b IFX_E_INVALID_POINTER is returned if pTaskData is invalid.\n
	*/
	virtual IFXRESULT IFXAPI  Execute(IFXTaskData * pTaskData) = 0;

};

// forward declaration is required, since IFXTask is needed by IFXScheduler
class IFXScheduler;

#endif // __IFXTask_H__


