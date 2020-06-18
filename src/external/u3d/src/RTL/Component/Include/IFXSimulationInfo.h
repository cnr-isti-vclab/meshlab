
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
	@file	IFXSimulationInfo.h

			This module defines the IFXSimulationInfo class.
*/

#ifndef __IFXSimulationInfo_H__
#define __IFXSimulationInfo_H__

#include "IFXUnknown.h"
#include "IFXTaskData.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {76E95C87-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXSimulationInfo,
0x76e95c87, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

enum IFXSimulationMsg {
	IFXSimulationMsg_Nil = 0,
	IFXSimulationMsg_Query = 1,
	IFXSimulationMsg_Advance = 2
};

/**
	This interface is used by the Simulation Manager to check on timing of 
	a task.
*/
class IFXSimulationInfo : public IFXTaskData 
{
public:

	// (1) SimulationManager sets the time
	// (2) call the Integrator
	// (3) if integrator cannot complete, set time to amount processed
	// (4) SimulationManager checks returned time

	/**
		Sets the priority of a task.

		@note	Priority values that are larger have higher priority.  So, 
				priority zero is the last priority and priority 0xFFFFFFFF 
				is the first priority.
	*/
	virtual IFXRESULT IFXAPI SetPriority(U32 priority) = 0;

	/**
		Returns the priority of a task.


		@note	Priority values that are larger have higher priority.  So, 
				priority zero is the last priority and priority 0xFFFFFFFF 
				is the first priority.
	*/
	virtual IFXRESULT IFXAPI GetPriority(U32 *pPriority) = 0;

	// used by SimulationManager
	// NOTE: had to use GetMsg instead of GetMessage since it is defined as a macro
	/** Sets the simulation message.*/
	virtual IFXRESULT IFXAPI SetMsg(IFXSimulationMsg msg) = 0;

	/** Returns the simulation message.*/
	virtual IFXSimulationMsg IFXAPI GetMsg() = 0;

	/**Gets the time interval.*/
	virtual IFXRESULT IFXAPI SetTimeInterval(U32 start, U32 end) = 0;

	/**Sets the time interval.*/
	virtual IFXRESULT IFXAPI GetTimeInterval(U32 * pBegin, U32 * pEnd) = 0;

	/**Sets the maximum time value.*/
	virtual IFXRESULT IFXAPI SetMaxTime(U32 maxTime) = 0;

	/**Returns the maximum time value.*/
	virtual U32 IFXAPI GetMaxTime() = 0;
};

#endif
