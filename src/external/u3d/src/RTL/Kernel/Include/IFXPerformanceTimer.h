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
	@file	IFXPerformanceTimer.h
		
			IFXPerformanceTimer interface.
*/

#ifndef __IFXPERFORMANCETIMER_INTERFACE_H__
#define __IFXPERFORMANCETIMER_INTERFACE_H__


#include "IFXUnknown.h"

// {DEC1B7A2-3DC7-11d3-81F4-0004AC2EDBCD}
IFXDEFINE_GUID(IID_IFXPerformanceTimer,
0xdec1b7a2, 0x3dc7, 0x11d3, 0x81, 0xf4, 0x0, 0x4, 0xac, 0x2e, 0xdb, 0xcd);

/** This interface is used to time performance. */
class IFXPerformanceTimer : public IFXUnknown 
{
public:
	/**Starts the timer.*/
	virtual IFXRESULT IFXAPI  StartTimer(U32 uTimerID)=0;
	/**Stops the timer.*/
	virtual IFXRESULT IFXAPI  StopTimer(U32 uTimerID)=0;
	/**Returns the amount of time that has elapsed.*/
	virtual IFXRESULT IFXAPI  GetElapsed(U32 uTimerID, U32& uTimeElapsed)=0;
};

#endif
