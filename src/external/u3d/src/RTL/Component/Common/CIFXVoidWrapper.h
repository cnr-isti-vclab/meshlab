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
// CIFXVoidWrapper.h

#ifndef CIFX_VOID_WRAPPER_H
#define CIFX_VOID_WRAPPER_H

#include "IFXVoidWrapper.h"

class CIFXVoidWrapper : virtual public IFXVoidWrapper, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXVoidWrapper_Factory(IFXREFIID interfaceId, void** ppInterface);

	void  IFXAPI SetData(void* pvData);
	void* IFXAPI GetData() const;

protected:
	CIFXVoidWrapper();
	virtual ~CIFXVoidWrapper() {}

	IFXRESULT IFXAPI Construct();

	void* m_pvData;
};

#endif // CIFX_VOID_WRAPPER_H

// END OF FILE

