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
	@file	CIFXBTTHash.h

			This is a hash table tailored for storing BlockType information
			for the load process.  The hash uses buckets (lists) at each location
			in the hash to handle overloading, and this is completely hidden
			from anyone using the IFXBTTHash interface to the component.

			Please refer to IFXBTTHash.h for a description of the memory
			management policy for this component -- it is very important
			to understand this when using the IFXBTTHash interface.
*/

#ifndef CIFXBTTHASH_H
#define CIFXBTTHASH_H

//-------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------

#include "IFXBTTHash.h"
#include "IFXSceneGraph.h"

//-------------------------------------------------------------------
//	Classes, structures and types
//-------------------------------------------------------------------

class CIFXBTTHash : virtual public IFXBTTHash
{
public:

	// IFXUnknown methods
	U32 IFXAPI  AddRef (void); 	
	U32 IFXAPI  Release (void); 	
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid, void **ppv);

	IFXRESULT IFXAPI	Initialize() ;
	IFXRESULT IFXAPI	Initialize( U32						uHashSize,
									U32						numEntries, 
									const IFXBTTHashEntry*	pEntryList ) ;

	IFXRESULT IFXAPI	Add	( const IFXBTTHashEntry* pEntry ) ;
	IFXRESULT IFXAPI	Delete( U32 ukey ) ;
	IFXRESULT IFXAPI	Find( U32 ukey, IFXBTTHashEntry* pEntry ) ;

	/// The hash link
	typedef struct IFXHashU32Link_struct
	{
		IFXBTTHashEntry*		pEntry ;
		IFXHashU32Link_struct*	pNext ;
	} IFXHashU32Link;

private:

	CIFXBTTHash();
	virtual ~CIFXBTTHash();
	
	friend IFXRESULT IFXAPI_CALLTYPE CIFXBTTHash_Factory( 
											IFXREFIID interfaceId, 
											void** ppInterface );

	void ClearHash() ;

	U32 m_uRefCount;

	U32 HashFunction (U32 ukey);

	IFXHashU32Link*	FindHashLink( 
						U32 ukey, 
						IFXHashU32Link** ppPreviousLink);

	U32					m_uHashTableSize;
	IFXHashU32Link*		m_pHashTable;		

	static IFXBTTHash::IFXBTTHashEntry	m_DefaultBlockTypes[] ;
	static U32							m_uDefaultBlockTypeCount ;

};

#endif

