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
//  CIFXClock.cpp
//
//  DESCRIPTION
//    This module defines the CIFXClock class.  It is used to...
//
//  NOTES
//
//***************************************************************************

#include "IFXSchedulerTypes.h"
#include "CIFXClock.h"
#include "IFXOSUtilities.h"

// maximum possible U32 time value
// const U32 MAX_TIME = 0xFFFFFFFF;

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXClock_Factory(IFXREFIID riid, void ** ppv)
{
	CIFXClock *pObject = NULL;
	IFXRESULT rc = IFX_OK;
	BOOL allocated = FALSE;

	if (ppv == NULL)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		pObject = new CIFXClock();

		if (!pObject)
		{
			// allocation failed
			*ppv = NULL;
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	if (IFXSUCCESS(rc))
		rc = pObject->QueryInterface(riid, ppv);

	if (rc != IFX_OK && allocated)
	{
		delete pObject;
		pObject = NULL;
	}

	return rc;
}

//---------------------------------------------------------------------
/// @todo: document
//---------------------------------------------------------------------
CIFXClock::CIFXClock()
{
	IFXOSInitialize();
	m_refcount = 0;
	m_initialized = FALSE;
	m_pCoreServices = NULL;
}

//---------------------------------------------------------------------
/// @todo: document
//---------------------------------------------------------------------
CIFXClock::~CIFXClock()
{
	IFXRELEASE(m_pCoreServices);

	IFXOSUninitialize();
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXClock::AddRef()
{
	++m_refcount;
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXClock::Release()
{
	--m_refcount;
	if (m_refcount == 0) {
		delete this;
		return 0;
	}
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::QueryInterface
//---------------------------------------------------------------------
IFXRESULT CIFXClock::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT rc = IFX_OK;

	if (ppv == NULL)
	{
		rc = IFX_E_INVALID_POINTER;
	}
	else if (riid == IID_IFXUnknown)
	{
		*ppv = this; // do not typecast to IFXUnknown, multiple inheritance confuses the compiler
		AddRef();
	}
	else if (riid == IID_IFXClock)
	{
		*ppv = (IFXClock*) this;
		AddRef();
	}
	else if (riid == IID_IFXSubject)
	{
		*ppv = (IFXSubject*) this;
		AddRef();
	}
	else
	{
		*ppv = NULL;
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXClock::Initialize(IFXCoreServices *pCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (m_initialized)
		rc = IFX_E_ALREADY_INITIALIZED;
	else if (!pCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		m_pCoreServices = pCoreServices;
		m_pCoreServices->AddRef();
	}

	if (IFXSUCCESS(rc))
	{
		m_initialized = TRUE;

		rc = Reset();
	}
	else
	{
		IFXRELEASE(m_pCoreServices);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXClock::Reset()
{
	if (!m_initialized)
		return IFX_E_NOT_INITIALIZED;

	m_thisTime = IFXOSGetTime();  // system clock
	//  m_lastTime = thisTime;

	m_running  = FALSE;
	m_autoflag = FALSE;
	m_paused   = FALSE;

	m_systemTime64 = 0;
	m_systemTime = 0;              // actual ms since simulation started
	m_systemTimeOffset = m_thisTime; // relative to the system clock
	m_systemPause = 0;             // pause time

	m_simulationTime64 = 0;
	m_simulationTime = 0;          // virtual ms since simulation started, minus pauses
	m_simulationTimeOffset = 0;    // relative to system time
	m_simulationTimeDelta = 0;     // time since last frame

	m_frameStart = 0;              // based on simulationTime
	m_frameEnd = 0;                // based on simulationTime

	m_currentFrame = 0;            // count

	return IFX_OK;
}

void CIFXClock::NotifyPause(void)
{
	if (!m_initialized)
		return;

	//  paused=TRUE;
	SetRunning(FALSE);
	SetAuto(TRUE);
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::Update
//
// update the current System Time (ie. Real Time)
//---------------------------------------------------------------------
void CIFXClock::Update()
{
	if (!m_initialized)
		return;

	//  U32 elapsed = 0;
	//  m_lastTime = m_thisTime;

	m_thisTime = IFXOSGetTime();

	if (m_paused)
	{
		m_paused = FALSE;
		//    m_lastTime = m_thisTime;
	}

	//  // check for rollover
	//  if (thisTime < lastTime)
	//  {
	//    // clock has rolled over
	//    simulationTimeHigh++;
	//    elapsed = lastTime - thisTime;
	//  }
	//  else
	//  {
	//    elapsed = thisTime - lastTime;
	//  }

	// systemTime starts at zero and is incremented by elaped intervals,
	// rather than being computed directly using systemTimeOffset. This
	// is to ensure that clock rollover is handled invisibly. Since
	// systemTime is a U32 and has millisecond accuracy, it will still
	// roll over approx. every 49 days. The Time Manager will also roll
	// over as it "counts up" so everything should continue running,
	// although after the rollover it will be about 49 days before any
	// scheduled tasks get another timeslice. We could probably come up
	// with a way to handle this event, but it seems too rare to be
	// worthwhile.

	// systemTime is REAL TIME and never pauses. However it is
	// normalized with respect to the start of the simulation, so that
	// we have the full 32-bit range before we encounter rollover.

	// simulationTime is based on systemTime, but is adjusted for
	// account for pauses.

	//  systemTime += elapsed;

	m_systemTime = m_thisTime - m_systemTimeOffset;
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::IncrementFrame
//
// Advances to the next frame and updates the system time
// This is the main driver for the simulation
//---------------------------------------------------------------------
void CIFXClock::IncrementFrame()
{
	if (!m_initialized)
		return;

	if (m_autoflag && !m_running)
	{
		SetRunning(TRUE);
		m_autoflag = FALSE;
	}

	Update(); // get current system time

	if (m_running)
	{
		// start of frame = end of last frame
		m_frameStart = m_frameEnd;
		// end of frame = now (adjusted for any pauses)
		// since this is simulation time, adjust for pauses
		m_frameEnd = m_systemTime - m_simulationTimeOffset;
		// increment frame number (first frame is #1)
		m_currentFrame++;
	}

	if (m_autoflag && m_running)
	{
		SetRunning(FALSE);
		m_autoflag = FALSE;
	}
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::JumpForward
//
// Jumps the SYSTEM time forward by X milliseconds.
// This makes the clock think that time has elapsed when it has not.
//---------------------------------------------------------------------
void CIFXClock::JumpForward(U32 offset)
{
	if (!m_initialized)
		return;

	/// @todo: update simulation time and frame times as well?

	m_systemTime += offset;
	m_systemTimeOffset += offset;
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::AdvanceTo
//
// Advances the clock to a specific time in milliseconds
//---------------------------------------------------------------------
void CIFXClock::AdvanceTo(U32 newTime)
{
	if (!m_initialized)
		return;

	// cannot run backwards
	IFXASSERT(newTime >= m_simulationTime);
	// cannot advance past the end of the current frame
	IFXASSERT(newTime <= m_frameEnd);

	if (m_running)
	{
		m_simulationTime = newTime;
		m_simulationTimeDelta = m_simulationTime - m_frameStart;
		PostChanges(IFX_CLOCK_NOTIFY_TIME_CHANGED);
	}
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::AdvanceToEnd
//
// Advances the simulation time to the end of the frame
//---------------------------------------------------------------------
void CIFXClock::AdvanceToEnd()
{
	if (!m_initialized)
		return;

	if (m_running)
	{
		// advance all the way to the current frameEnd
		m_simulationTime = m_frameEnd;
		m_simulationTimeDelta = m_simulationTime - m_frameStart;
		PostChanges(IFX_CLOCK_NOTIFY_TIME_CHANGED);
	}
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::SetRunning
//
// TRUE starts the clock, FALSE stops it
//
// changing the running state resets the auto flag
//---------------------------------------------------------------------
void CIFXClock::SetRunning(BOOL newstate)
{
	if (!m_initialized)
		return;

	Update();
	if (m_running && !newstate)
		m_systemPause = m_systemTime;
	else if (!m_running && newstate)
	{
		// Add the difference to the simulationTime.
		// This prevents the simulationTime from advancing during
		// the paused interval, while allowing the real time clock
		// to continue...
		m_simulationTimeOffset += (m_systemTime - m_systemPause);
	}

	m_running = newstate;
	m_autoflag = FALSE;
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::SetAuto
//
// If "auto" is set and the clock is running, it will automatically
// stop on the next update.
//
// If "auto" us set and the clock is stopped, it will automatically
// start on the next update.
//---------------------------------------------------------------------
void CIFXClock::SetAuto(BOOL newstate)
{
	if (!m_initialized)
		return;

	m_autoflag = newstate;
}

//---------------------------------------------------------------------
// PUBLIC IFXClock::IsRunning
//---------------------------------------------------------------------
BOOL CIFXClock::IsRunning()
{
	if (!m_initialized)
		return FALSE;

	return m_running;
}


// FORMERY INLINE METHODS


IFXRESULT CIFXClock::GetSystemTime64(IFXTime * pHigh, IFXTime * pLow)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		if (pHigh)
			*pHigh = m_systemTime64;
		if (pLow)
			*pLow = m_systemTime;
	}

	return rc;
}
U32 CIFXClock::GetSystemTime()
{
	if (!m_initialized)
		return 0;

	return m_systemTime;
}
IFXRESULT CIFXClock::GetSimulationTime64(IFXTime * pHigh, IFXTime * pLow)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		if (pHigh)
			*pHigh = m_simulationTime64;
		if (pLow)
			*pLow = m_simulationTime;
	}

	return rc;
}

U32 CIFXClock::GetSimulationTime()
{
	if (!m_initialized)
		return 0;

	return m_simulationTime;
}
U32 CIFXClock::GetSimulationTimeDelta()
{
	if (!m_initialized)
		return 0;

	return m_simulationTimeDelta;
}

U32 CIFXClock::GetCurrentFrame()
{
	if (!m_initialized)
		return 0;

	return m_currentFrame;
}
U32 CIFXClock::GetFrameStart()
{
	if (!m_initialized)
		return 0;

	return m_frameStart;
}
U32 CIFXClock::GetFrameEnd()
{
	if (!m_initialized)
		return 0;

	return m_frameEnd;
}
