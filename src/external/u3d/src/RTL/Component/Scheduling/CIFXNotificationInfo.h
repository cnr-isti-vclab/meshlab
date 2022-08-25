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
//	CIFXNotificationInfo.h
//
//	DESCRIPTION
//		This module defines the CIFXNotificationInfo class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXNotificationInfo_H__
#define __CIFXNotificationInfo_H__

#include "IFXNotificationInfo.h"

//-------------------------------------------------------------------
//
//  IFXNotificationInfo Implementation
//
//-------------------------------------------------------------------

class CIFXNotificationInfo : public IFXNotificationInfo {

	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNotificationInfo_Factory(IFXREFIID riid, void **ppv);

private:
	// IFXCOM
	U32 m_refcount;
	U32 m_initialized;

	// IFXTaskData
	IFXCoreServices * m_pCoreServices;
	IFXUnknown      * m_pOrigin;
	IFXTaskManager  * m_pTaskManager;
	IFXTaskHandle     m_taskHandle;
	void            * m_pUserData;
	void            * m_pNotificationData;

	IFXNotificationType m_type;
	IFXNotificationId   m_id;
	U32                 m_keyFilter;
	U32                 m_bitFilter;
	IFXUnknown        * m_pObjectFilter;
	U32                 m_time;
	IFXString           m_name;

	// FILTER TYPES:
	//   m_keyFilter is an exact-match keyword filter (0 is wild)
	//   m_bitFilter is a bitwise AND filter (0 is wild)
	//   m_pObjectFilter is an exact-match object filter (NULL is wild)
	//
	// FILTER RULES:
	//   filters are user-defined
	//   registered WILD matches any submitted event, wild or non-wild
	//   registered X matches only submitted X, does not match submitted wild

private:

	// Housekeeping
	CIFXNotificationInfo();
	virtual ~CIFXNotificationInfo();

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

	virtual IFXRESULT IFXAPI  SetNotificationData(void * pObject); 
	virtual IFXRESULT IFXAPI  GetNotificationData(void ** ppObject);

	virtual IFXRESULT IFXAPI  SetTime(U32 time);
	virtual IFXRESULT IFXAPI  GetTime(U32 *pTime);
	// result is TRUE (1) if match, or FALSE (0) if no match
	virtual IFXRESULT IFXAPI  MatchPattern(IFXTaskData * pViewPattern, BOOL * pResult);
	virtual IFXRESULT IFXAPI  GetSortKey(U32 * pKey);

	// IFXNotificationInfo methods
	virtual IFXRESULT IFXAPI  SetType(IFXNotificationType type);
	virtual IFXRESULT IFXAPI  GetType(IFXNotificationType * pType);
	virtual IFXRESULT IFXAPI  SetId(IFXNotificationId id);
	virtual IFXRESULT IFXAPI  GetId(IFXNotificationId * pId);
	virtual IFXRESULT IFXAPI  SetName(IFXString name);
	virtual IFXRESULT IFXAPI  GetName(IFXString &name);

	virtual IFXRESULT IFXAPI  SetKeyFilter(U32 keyFilter);
	virtual IFXRESULT IFXAPI  GetKeyFilter(U32 *pKeyFilter);
	virtual IFXRESULT IFXAPI  SetBitFilter(U32 bitFilter);
	virtual IFXRESULT IFXAPI  GetBitFilter(U32 *pBitFilter);
	virtual IFXRESULT IFXAPI  SetObjectFilter(IFXUnknown * pObjectFilter);
	virtual IFXRESULT IFXAPI  GetObjectFilter(IFXUnknown ** ppObjectFilter);
};

#endif // __CIFXNotificationInfo_H__

