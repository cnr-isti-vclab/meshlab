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
	@file	IFXHashMap.h

			Header file for the hash map and related classes.
*/

#ifndef __IFXHASHMAP_INTERFACES_H__
#define __IFXHASHMAP_INTERFACES_H__

#include "IFXUnknown.h"
#include "IFXString.h"

// {A14FECF2-C916-11d3-A101-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXHashMap,
0xa14fecf2, 0xc916, 0x11d3, 0xa1, 0x1, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

class IFXHashMap : public IFXUnknown 
{
public:
	// IFXHashMap methods
	virtual IFXRESULT IFXAPI  Initialize(U32 uHashSize)=0;
	virtual IFXRESULT IFXAPI  Add(IFXString* pString, U32 uID)=0;
	virtual IFXRESULT IFXAPI  Delete(IFXString* pString)=0;
	virtual IFXRESULT IFXAPI  Find(IFXString* pString, U32* pID)=0;
};

#endif
