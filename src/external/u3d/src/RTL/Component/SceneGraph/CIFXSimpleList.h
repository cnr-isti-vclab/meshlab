//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
//	CIFXSimpleList.h
//
//	DESCRIPTION
//		Header file for the path class.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXSIMPLELIST_CLASS_H__
#define __CIFXSIMPLELIST_CLASS_H__

#include "IFXSimpleList.h"

class CIFXSimpleList:public IFXSimpleList {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSimpleList_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXSimpleList function.
	IFXRESULT  IFXAPI 	 Initialize(U32 uInitialSize);
	IFXRESULT  IFXAPI 	 Add(IFXUnknown* pObject, U32* pIndex);
	IFXRESULT  IFXAPI 	 Delete(U32 uIndex);
	IFXRESULT  IFXAPI 	 Get(U32 uIndex, IFXUnknown** ppObject);
	IFXRESULT  IFXAPI 	 Set(U32 uIndex, IFXUnknown* pObject);
	IFXRESULT  IFXAPI 	 Insert(U32 uIndex, IFXUnknown* pObject);
	IFXRESULT  IFXAPI 	 Copy(IFXSimpleList* pDestList);
	IFXRESULT  IFXAPI 	 GetCount(U32* pCount);

private:
	CIFXSimpleList();
	virtual ~CIFXSimpleList();

	U32				m_uRefCount;
	IFXUnknown**	m_ppList;
	U32				m_uCount;
	U32				m_uLastElement;

};

#endif // #define __CIFXSIMPLELIST_CLASS_H__
