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
	@file	CIFXHashMap.h

			Header file for the hash map and related classes.
*/

#ifndef __CIFXHASHMAP_H__
#define __CIFXHASHMAP_H__

#include "IFXHashMap.h"

class CIFXHashMap : public IFXHashMap 
{
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	// IFXHashMap methods
	IFXRESULT IFXAPI 	Initialize(U32 uHashSize);
	IFXRESULT IFXAPI 	Add(IFXString* pString, U32 uID);
	IFXRESULT IFXAPI 	Delete(IFXString* pString);
	IFXRESULT IFXAPI 	Find(IFXString* pString, U32* pID);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXHashMap_Factory( IFXREFIID interfaceId, void** ppInterface );

private:
	CIFXHashMap();
	virtual ~CIFXHashMap();
	IFXRESULT HashFunction(IFXString* pName, U32* pIndex);
	IFXRESULT DeleteTable();

	struct HashMapObject 
	{
		IFXString*			pName;
		U32					uID;
		HashMapObject*		pNext;
	};

	HashMapObject** m_ppHashTable;
	U32	m_uHashTableSize;
	U32 m_uRefCount;
};


#endif
