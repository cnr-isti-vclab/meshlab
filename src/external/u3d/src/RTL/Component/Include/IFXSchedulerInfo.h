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
//	IFXSchedulerInfo.h
//
//	DESCRIPTION
//		This module defines the IFXSchedulerInfo class.  It is used to...
//
//	NOTES
//
//***************************************************************************

/**
	@file	IFXSchedulerInfo.h

			This header defines the IFXSchedulerInfo interface and its functionality.
*/

#ifndef __IFXSchedulerInfo_H__
#define __IFXSchedulerInfo_H__

#include "IFXUnknown.h"
#include "IFXTaskData.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {76E95C86-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXSchedulerInfo,
0x76e95c86, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXSchedulerInfo.

	@note	The associated IID is named IID_IFXSchedulerInfo,
	and the CID for the component is CID_IFXSchedulerInfo.
*/
class IFXSchedulerInfo : public IFXTaskData {
public:
	/**
		This method sets the priority for this event.

		@param	priority	The priority.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n

		@note	Priority values that are larger have higher priority.  So, 
				priority zero is the last priority and priority 0xFFFFFFFF 
				is the first priority.
	*/
	virtual IFXRESULT IFXAPI  SetPriority(U32 priority) = 0;
	/**
		This method retrieves the priority for this event.

		@param	pPriority	Receives the priority.\n

		@return	Returns an IFXRESULT code.\n
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the component has not been initialized.\n
		- @b IFX_E_INVALID_POINTER is returned if pPriority is NULL.\n

		@note	Priority values that are larger have higher priority.  So, 
				priority zero is the last priority and priority 0xFFFFFFFF 
				is the first priority.
	*/
	virtual IFXRESULT IFXAPI  GetPriority(U32 *pPriority) = 0;
};

#endif
