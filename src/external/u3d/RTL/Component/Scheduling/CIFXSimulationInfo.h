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
//	CIFXSimulationInfo.h
//
//	DESCRIPTION
//		This module defines the CIFXSimulationInfo class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXSimulationInfo_H__
#define __CIFXSimulationInfo_H__

#include "IFXSimulationInfo.h"

//-------------------------------------------------------------------
//
//  IFXSimulationInfo Implementation
//
//-------------------------------------------------------------------

class CIFXSimulationInfo : public IFXSimulationInfo 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSimulationInfo_Factory(IFXREFIID riid, void **ppv);

private:
	// IFXCOM
	U32 m_refcount;

	U32 m_initialized;
	IFXCoreServices * m_pCoreServices;

	// IFXTaskData
	IFXUnknown      * m_pOrigin;
	IFXTaskManager  * m_pTaskManager;
	IFXTaskHandle     m_taskHandle;
	void            * m_pUserData;
	IFXUnknown      * m_pObject;
	U32               m_time;

	U32               m_priority;

	// SimulationInfo
	IFXSimulationMsg m_msg;
	U32 m_startTime;
	U32 m_endTime;
	U32 m_maxTime;

private:

	// Housekeeping
	CIFXSimulationInfo();
	virtual ~CIFXSimulationInfo();

public:
	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXTaskData methods
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices);
	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices ** ppCoreServices);
	virtual IFXRESULT IFXAPI  SetOrigin(IFXUnknown * pObject);
	virtual IFXRESULT IFXAPI  GetOrigin(IFXUnknown ** ppObject);
	virtual IFXRESULT IFXAPI  SetTaskManager(IFXTaskManager * pTaskManager);
	virtual IFXRESULT IFXAPI  GetTaskManager(IFXTaskManager ** ppTaskManager);
	virtual IFXRESULT IFXAPI  SetTaskHandle(IFXTaskHandle taskHandle);
	virtual IFXRESULT IFXAPI  GetTaskHandle(IFXTaskHandle * pTaskHandle);
	virtual IFXRESULT IFXAPI  SetUserData(void * pObject);
	virtual IFXRESULT IFXAPI  GetUserData(void ** ppObject);

	virtual IFXRESULT IFXAPI  SetTime(U32 time);
	virtual IFXRESULT IFXAPI  GetTime(U32 *pTime);
	// result is TRUE (1) if match, or FALSE (0) if no match
	virtual IFXRESULT IFXAPI  MatchPattern(IFXTaskData * pViewPattern, BOOL * pResult);
	virtual IFXRESULT IFXAPI  GetSortKey(U32 * pKey);

	// IFXSimulationInfo
	// (1) SimulationManager sets the time
	// (2) call the Integrator
	// (3) if integrator cannot complete, set time to amount processed
	// (4) SimulationManager checks returned time
	virtual IFXRESULT IFXAPI  SetPriority(U32 priority);
	virtual IFXRESULT IFXAPI  GetPriority(U32 *pPriority);

	// used by SimulationManager
	virtual IFXRESULT IFXAPI  SetMsg(IFXSimulationMsg msg);
	virtual IFXSimulationMsg IFXAPI GetMsg();
	virtual IFXRESULT IFXAPI  SetTimeInterval(U32 start, U32 end);
	virtual IFXRESULT IFXAPI  GetTimeInterval(U32 * pBegin, U32 * pEnd);
	// used to return max time value
	virtual IFXRESULT IFXAPI  SetMaxTime(U32 maxTime);
	virtual U32 IFXAPI  GetMaxTime();
};

#endif // __CIFXSimulationInfo_H__

