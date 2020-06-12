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
	@file	IFXClock.h

			This header defines the IFXClock interface and its functionality.
*/

#ifndef __IFXClock_H__
#define __IFXClock_H__

#include "IFXUnknown.h"
#include "IFXCoreServices.h"

//-------------------------------------------------------------------
// IFXCOM Stuff
//-------------------------------------------------------------------

// {AD4A0946-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(IID_IFXClock,
0xad4a0946, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

#define IFX_CLOCK_NOTIFY_TIME_CHANGED 0x00000001

//-------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------

/**
	This is the main interface for IFXClock.

	@note	The associated IID is named IID_IFXClock.
*/

typedef U32 IFXTime;

class IFXClock : public IFXUnknown 
{
public:
	// IFXClock

	/**
		This method initializes the clock.

		@param	pCoreServices	A pointer to an implementation of the IFXCoreServices interface.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		  
		- @b IFX_E_INVALID_POINTER is returned if pCoreServices is invalid.\n
	*/
	virtual IFXRESULT IFXAPI Initialize(IFXCoreServices *pCoreServices) = 0;

	/**
		This method resets the clock.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		- @b IFX_E_ALREADY_INITIALIZED if the clock has already been initialized.\n
		- @b IFX_E_INVALID_POINTER if the pCoreServices pointer is NULL.\n
	*/
	virtual IFXRESULT IFXAPI Reset() = 0;

	/**
		This method pauses the clock. The clock will automatically restart upon the next call to Update().

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the clock has not been initialized.\n
	*/
	virtual void IFXAPI NotifyPause() = 0;

	/**
		This method updates the system time.
	*/
	virtual void IFXAPI Update() = 0;

	/**
		This method begins a new frame. It increments the frame count and computes new starting and ending frame times.
		The frame times are always in simulation time.
	*/
	virtual void IFXAPI IncrementFrame() = 0;

	/**
		This method advances the simulation time to the specified new time.
		The new time must between the current simulation time and the end of the frame, inclusive.
		The clock cannot run backwards.

		@param	newtime	The simulation time the clock is to advance to.
	*/
	virtual void IFXAPI AdvanceTo(IFXTime newtime) = 0;

	/**
		This method advances the simulation time to the end of the frame.
	*/
	virtual void IFXAPI AdvanceToEnd() = 0;

	/**
		This method advances the system time to the specified new time.
		This causes the clock to think that real time has passed.

		@param	newtime	The system time the clock is to advance to.
	*/
	virtual void IFXAPI JumpForward(IFXTime newtime) = 0;

	/**
		This method starts or stops the simulation clock.
		Note that system time and frame count continue to advance while the clock
		is stopped, but the simulation time does not advance.

		@param	newstate	TRUE to start the clock, FALSE to stop it.
	*/
	virtual void IFXAPI SetRunning(BOOL newstate) = 0;

	/**
		This method sets the auto-reset flag on the clock.
		If auto-reset is true, the clock automatically inverts it's running state
		upon the next call to IncrementFrame. That is, if it is stopped, it automatically
		starts, and if it is running, it automatically stops. The auto-reset flag is
		cleared after it has been triggered, so the clock will not oscillate between
		running and stopped.

		@param	newstate	TRUE to set the auto-reset feature, FALSE to clear it.
	*/
	virtual void IFXAPI SetAuto(BOOL newstate) = 0;

	/**
		This method indicates whether the clock is running or stopped.

		@return	TRUE if the clock is running, FALSE if it is stopped.
	*/
	virtual BOOL IFXAPI IsRunning() = 0;

	/**
		This method returns the lower 32 bits of the system time, as of the last Update.

		@return	The current system time.
	*/
	virtual IFXTime IFXAPI GetSystemTime() = 0;

	/**
		This method returns the upper 32 bits of the system time.

		@param	pHigh	Returns the upper 32 bits of the system time. If NULL, this parameter is ignored.
		@param	pLow	Returns the lower 32 bits of the system time. If NULL, this parameter is ignored.

		@return	Returns an IFXRESULT code.\n
	
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the clock has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI GetSystemTime64(IFXTime * pHigh, IFXTime * pLow) = 0;

	/**
		This method returns the lower 32 bits of the current simulation time.

		@return	The current simulation time.
	*/
	virtual IFXTime IFXAPI GetSimulationTime() = 0;

	/**
		This method returns the upper 32 bits of the current simulation time.

		@param	pHigh	Returns the upper 32 bits of the simulation time. If NULL, this parameter is ignored.
		@param	pLow	Returns the lower 32 bits of the simulation time. If NULL, this parameter is ignored.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		- @b IFX_E_NOT_INITIALIZED if the clock has not been initialized.\n
	*/
	virtual IFXRESULT IFXAPI GetSimulationTime64(IFXTime * pHigh, IFXTime * pLow) = 0;

	/**
		This method returns the difference between the current simulation time and the start of the current frame.

		@return	The current simulation time delta.
	*/
	virtual IFXTime IFXAPI GetSimulationTimeDelta() = 0;

	/**
		This method returns the current frame number.

		@return	The current frame number.
	*/
	virtual IFXTime IFXAPI GetCurrentFrame() = 0;

	/**
		This method returns the start of the current frame, in simulation time.

		@return	The start of the current frame.
	*/
	virtual IFXTime IFXAPI GetFrameStart() = 0;

	/**
		This method returns the end of the current frame, in simulation time.

		@return	The end of the current frame.
	*/
	virtual IFXTime IFXAPI GetFrameEnd() = 0;
};

#endif
