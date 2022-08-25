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

#ifndef __CIFXGuidHashMap_H__
#define __CIFXGuidHashMap_H__


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXHashMap.h"
#include "IFXResultComponentEngine.h"

//***************************************************************************
//	Classes, structures and types
//***************************************************************************

struct IFXComponentDescriptor;

//#define IFXGUIDDEBUG
#define IFX_E_GUIDHASHMAP_ID_NOT_FOUND	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_CORE_SERVICES, 0x00100 )


class CIFXGuidHashMap
{
public:

	CIFXGuidHashMap();
	~CIFXGuidHashMap();

	IFXRESULT Initialize(
				const U32 componentNumber, 
				const IFXComponentDescriptor* pComponentDescriptorList);
	IFXRESULT Add		(const IFXComponentDescriptor* pComponentDescriptor);
	IFXRESULT Delete	(const IFXCID& compID);
	IFXRESULT Find(const IFXCID& compID, 
				   const IFXComponentDescriptor** ppComponentDescriptor) const;

#ifdef IFXGUIDDEBUG
	// For debug & hash table analysis:
	//void Print( char *, bool );
	void Print();
#endif

	// The hash bucket:
	struct IFXGUIDHashBucket
	{
		const IFXComponentDescriptor*	pCompDesc;
		IFXGUIDHashBucket*					pNext;
	};

private:

	// A reformatting of the 128 bit GUID Structure:
	struct IFXHashableGUID
	{
		U32 g1;
		U32 g2;
		U32 g3;
		U32 g4;
	};

	// Implementation methods:
	BOOL GUIDsEquivalent ( const IFXCID& compID1, const IFXCID& compID2) const;
	U32 HashFunction	 (const IFXCID& compID) const;
	IFXGUIDHashBucket*	FindHashBucket( const IFXGUID& rCompDesc, IFXGUIDHashBucket** ppPreviousBucket ) const;

	U32					m_uHashTableSize;
	IFXGUIDHashBucket*	m_pHashTable;		
};


#endif
