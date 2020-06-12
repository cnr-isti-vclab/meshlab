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
//	CIFXPerformanceTimer.cpp
//
//	DESCRIPTION
//		Implementation for the Timer component.
//
//	NOTES
//      None.
//
//***************************************************************************
#include "CIFXPerformanceTimer.h"
#include "IFXOSUtilities.h"

static CIFXPerformanceTimer	*gs_pSingleton	= NULL;

CIFXPerformanceTimer::CIFXPerformanceTimer()
{
	U32	uIndex;
	m_uRefCount = 0;

	// clear out all timings
	for(uIndex=0; uIndex<PERFORMANCE_TIMER_NUMBER_OF_TIMERS; uIndex++) {
		m_uTimeElapsed[uIndex]=0;
		bRunning[uIndex]=FALSE;
	}

	// Set the module global singleton pointer.
	gs_pSingleton	= this;
}

CIFXPerformanceTimer::~CIFXPerformanceTimer()
{
	// Clear the module global singleton pointer.
	gs_pSingleton	= NULL;
}

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXPerformanceTimer::AddRef()
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXPerformanceTimer::Release()
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXPerformanceTimer::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXPerformanceTimer ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXPerformanceTimer* ) this;
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// CIFXTimer methods...

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer::StartTimer
//
//	This method starts the timer for a given TimerID.  It does this by 
//  recording the current time (either performance counter, or timeGetTime on
//  a win32 platfrom) from the GetTime() OS specific function.  It also sets
//  a flag that can be queried to tell if the timer is on or not.  If the 
//  timer is already running, it simply resets the count and continues.
//---------------------------------------------------------------------------
IFXRESULT CIFXPerformanceTimer::StartTimer(U32 uTimerID)
{
	bRunning[uTimerID]=TRUE;

	uCounterBegin[uTimerID] = IFXOSGetTime();
	return IFX_OK;
}

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer::StopTimer
//
//	This method stops the timer for a given TimerID and records the elapsed 
//  time.  It does this by recording the current time (either performance 
//  counter, or timeGetTime on a win32 platfrom) from the GetTime() OS 
//  specific function.  It then subtracts the start value from the current 
//  time and stores that in the TimeElapsed array.
//---------------------------------------------------------------------------
IFXRESULT CIFXPerformanceTimer::StopTimer(U32 uTimerID)
{
	if(bRunning[uTimerID]==FALSE) {
		m_uTimeElapsed[uTimerID]=0;
		return IFX_OK;
	}

	uCounterEnd[uTimerID] = IFXOSGetTime();
	
	// check for roll-around
	if(uCounterEnd[uTimerID]<uCounterBegin[uTimerID]) {
		m_uTimeElapsed[uTimerID] = uCounterBegin[uTimerID] - uCounterEnd[uTimerID];
	} else
		m_uTimeElapsed[uTimerID] = uCounterEnd[uTimerID] - uCounterBegin[uTimerID];

	// Cleanup.
	bRunning[uTimerID]=FALSE;

	return IFX_OK;
}

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer::GetElapsed
//
//	This method returns the currently elapsed time of a timer, but does not
//  stop or interfere with the timer. It does this by getting the current time,
//  subtracting off the start time, and returning that value
//---------------------------------------------------------------------------
IFXRESULT CIFXPerformanceTimer::GetElapsed(U32 uTimerID, U32& uTimeElapsed)
{
	if(bRunning[uTimerID]==FALSE) {
		uTimeElapsed = m_uTimeElapsed[uTimerID];
		return IFX_OK;
	}

	// check for roll-around
	uCounterEnd[uTimerID] = IFXOSGetTime();

	if(uCounterEnd[uTimerID]<uCounterBegin[uTimerID]) {
		uTimeElapsed = uCounterBegin[uTimerID] - uCounterEnd[uTimerID];
	} else
		uTimeElapsed = uCounterEnd[uTimerID] - uCounterBegin[uTimerID];


	return IFX_OK;
}

//---------------------------------------------------------------------------
//	CIFXPerformanceTimer_Factory (singleton)
//
//	This is the CIFXPerformanceTimer component factory function.  The
//	CIFXPerformanceTimer component is a singleton, it will only be
//	instaniated once.  Attempts to perform additional instantiations will
//	hand back references to the existing component's interfaces.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXPerformanceTimer_Factory( IFXREFIID	interfaceId,
										void**		ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// Does the singleton component already exist?
		if ( gs_pSingleton )
		{
			// It does exist, so just QI for the requested interface.
			result = gs_pSingleton->QueryInterface( interfaceId, ppInterface );
		}
		else
		{
			// It doesn't exist, so try to create it.  Note:  The component
			// class sets up gs_pSingleton upon construction and NULLs it
			// upon destruction.
			CIFXPerformanceTimer	*pComponent	= new CIFXPerformanceTimer;

			if ( pComponent )
			{
				// Perform a temporary AddRef for our usage of the component.
				pComponent->AddRef();

				// Attempt to obtain a pointer to the requested interface.
				result = pComponent->QueryInterface( interfaceId, ppInterface );

				// Perform a Release since our usage of the component is now
				// complete.  Note:  If the QI fails, this will cause the
				// component to be destroyed.
				pComponent->Release();
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


