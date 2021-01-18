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
//	CIFXPerformanceTimer.h
//
//	DESCRIPTION
//		CIFXPerformanceTimer interface.
//
//	NOTES
//
//
//***************************************************************************
#ifndef __CIFXPERFORMANCETIMER_INTERFACE_H__
#define __CIFXPERFORMANCETIMER_INTERFACE_H__

#include "IFXPerformanceTimer.h"

const U32 PERFORMANCE_TIMER_NUMBER_OF_TIMERS=128;

class CIFXPerformanceTimer:public IFXPerformanceTimer {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void); 	
	U32 IFXAPI  Release (void); 	
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid, void **ppv);

	// IFXTimer methods
	IFXRESULT IFXAPI   StartTimer(U32 uTimerID);
	IFXRESULT IFXAPI   StopTimer(U32 uTimerID);
	IFXRESULT IFXAPI   GetElapsed(U32 uTimerID, U32& uTimeElapsed);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXPerformanceTimer_Factory( IFXREFIID interfaceId, void** ppInterface );

private:
	CIFXPerformanceTimer();
	virtual ~CIFXPerformanceTimer();
	U32	m_uRefCount;
	U32 m_uTimeElapsed[PERFORMANCE_TIMER_NUMBER_OF_TIMERS];
	
	BOOL			m_bUseQPC;
//	LARGE_INTEGER	m_qwFrequency;	// performance frequency of machine
	
//	LARGE_INTEGER	qwCounterBegin[PERFORMANCE_TIMER_NUMBER_OF_TIMERS];	// beginning time when using query performance counter 
//	LARGE_INTEGER	qwCounterEnd[PERFORMANCE_TIMER_NUMBER_OF_TIMERS];	// ending time when using query performance counter 
	U32				uCounterBegin[PERFORMANCE_TIMER_NUMBER_OF_TIMERS];	// beginning time when using timeGetTime();
	U32				uCounterEnd[PERFORMANCE_TIMER_NUMBER_OF_TIMERS];	// ending time when using timeGetTime();
	BOOL			bRunning[PERFORMANCE_TIMER_NUMBER_OF_TIMERS];
};



#endif // #define __CIFXPERFORMANCETIMER_INTERFACE_H__
