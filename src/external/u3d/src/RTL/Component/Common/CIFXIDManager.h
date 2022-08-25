//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// CIFXIDManager.h
#ifndef CIFX_IDMANAGER_H
#define CIFX_IDMANAGER_H

#include "IFXIDManager.h"

class CIFXIDManager;

class CIFXIDStack
{
public:
	friend class CIFXIDManager;
private:
	CIFXIDStack();
	~CIFXIDStack();

	U32 m_uId;
	CIFXIDStack* m_pNext;
};

class CIFXIDManager : virtual public IFXIDManager, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXIDManagerFactory(IFXREFIID intId, void** ppUnk);

	IFXREFIID IFXAPI GetCID() const;
	
	//=========================
	// IFXIDManager Methods
	//=========================
	virtual IFXRESULT IFXAPI Clear();

	virtual IFXRESULT IFXAPI GetId(U32& uOutId);
	virtual IFXRESULT IFXAPI ReleaseId(U32 uInId);

	virtual void IFXAPI OutputAllocatedIds();

protected:
	//=========================
	// CIFXIDManager Methods
	//=========================
	CIFXIDManager();
	virtual ~CIFXIDManager();

	IFXRESULT IFXAPI Push(U32 uInId);
	IFXRESULT IFXAPI Pop(U32& uOutId);
	IFXRESULT IFXAPI RemoveIdFromStack(U32 uInId);

	IFXRESULT IFXAPI Construct();

	CIFXIDStack*	m_pIDStack;
	U32				m_uNextId;
};

#endif
