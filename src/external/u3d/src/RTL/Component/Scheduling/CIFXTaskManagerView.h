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
//	CIFXTaskManagerView.h
//
//	DESCRIPTION
//		This module defines the CIFXTaskManagerView class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXTaskManagerView_H__
#define __CIFXTaskManagerView_H__

#include "IFXTaskManagerView.h"
#include "IFXCoreServices.h"

//-------------------------------------------------------------------
//
//  IFXTaskManagerView Implementation
//
//-------------------------------------------------------------------

class CIFXTaskManagerView : public IFXTaskManagerView 
{
	// allow factory access to private constructor
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTaskManagerView_Factory(IFXREFIID riid, void **ppv);

private:

	// IFXCOM
	U32 m_refcount;
	U32 m_initialized;

	IFXCoreServices * m_pCoreServices;

	U32 m_allocStepSize;
	U32 m_allocated;
	U32 m_size;
	IFXTaskManagerNode ** m_ppView;

private:

	// Housekeeping
	CIFXTaskManagerView();
	virtual ~CIFXTaskManagerView();

	IFXRESULT IncreaseAlloc();

public:

	// IFXCOM
	virtual U32 IFXAPI  AddRef(void);
	virtual U32 IFXAPI  Release(void);
	virtual IFXRESULT IFXAPI  QueryInterface(IFXREFIID riid, void **ppv);

	// IFXTaskManagerView methods
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices * pCoreServices);
	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices ** ppCoreServices);

	virtual U32 IFXAPI  GetRefCount(); // for re-use purposes, clone if someone else has a lock on it
	virtual IFXRESULT IFXAPI  Reset(); // clear array, allow for re-using object

	virtual IFXRESULT IFXAPI  SetAlloc(U32 n);
	virtual IFXRESULT IFXAPI  AddTask(IFXTaskManagerNode * pNode);

	virtual IFXRESULT IFXAPI  GetSize(U32 * n);
	virtual IFXRESULT IFXAPI  GetElementAt(U32 n, IFXTaskManagerNode ** ppNode);

	virtual IFXRESULT IFXAPI  ExecuteAll(IFXTaskData * pData, BOOL submitErrors);

};

#endif // __CIFXTaskManagerView_H__

