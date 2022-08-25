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
//	CIFXClock.h
//
//	DESCRIPTION
//		This module defines the CIFXClock class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXClock_H__
#define __CIFXClock_H__

#include "IFXClock.h"
#include "IFXCoreServices.h"
#include "IFXScheduler.h"
#include "CIFXSubject.h"

typedef U32 IFXTime;

class CIFXClock : public IFXClock,  public CIFXSubject 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXClock_Factory(IFXREFIID riid, void **ppv);

private:
	U32 m_refcount;
	BOOL m_initialized;

	IFXCoreServices * m_pCoreServices;

	// MISC

	BOOL m_running;
	BOOL m_paused;	/*** running will be set before first unpaused
						ClockTick(), so we retain a flag that we were paused */

	BOOL m_autoflag;

	IFXTime m_thisTime;               // used to query system clock
//	IFXTime lastTime;               // last query to system clock (rollover)

	IFXTime m_systemTime64;           // 64-bit extension to system time
	IFXTime m_systemTime;             // real time since Clock created; not adjusted for pauses
	IFXTime m_systemTimeOffset;       // offset relative to thisTime
	IFXTime m_systemPause;            // time at last PAUSE

	IFXTime m_simulationTime64;       // 64-bit extension to simulation time
	IFXTime m_simulationTime;         // running time
	IFXTime m_simulationTimeOffset;   // offset relative to systemTime
	IFXTime m_simulationTimeDelta;    // elapsed time since start of Frame

	IFXTime m_frameStart;             // simulation time of frame start
	IFXTime m_frameEnd;               // simulation time of frame end

	IFXTime m_currentFrame;           // current frame counter

private:
	CIFXClock();
	~CIFXClock();

public: // Legacy Methods

	virtual void IFXAPI  NotifyPause();
	virtual void IFXAPI  Update();
	virtual void IFXAPI  IncrementFrame();
	virtual void IFXAPI  AdvanceTo(IFXTime newtime);
	virtual void IFXAPI  AdvanceToEnd();
	virtual void IFXAPI  JumpForward(IFXTime offset);

	virtual void IFXAPI  SetRunning(BOOL newstate);
	virtual void IFXAPI  SetAuto(BOOL newstate);
	virtual BOOL IFXAPI  IsRunning();

	virtual IFXTime IFXAPI GetSystemTime();
	virtual IFXRESULT IFXAPI  GetSystemTime64(IFXTime * pHigh, IFXTime * pLow);
	virtual IFXTime IFXAPI GetSimulationTime();
	virtual IFXRESULT IFXAPI  GetSimulationTime64(IFXTime * pHigh, IFXTime * pLow);
	virtual IFXTime IFXAPI GetSimulationTimeDelta();
	virtual IFXTime IFXAPI GetCurrentFrame();
	virtual IFXTime IFXAPI GetFrameStart();
	virtual IFXTime IFXAPI GetFrameEnd();

public:
	// IFXCOM
	virtual IFXTime IFXAPI  AddRef (void);
	virtual IFXTime IFXAPI  Release (void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXClock
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices);
	virtual IFXRESULT IFXAPI  Reset();

};


#endif
