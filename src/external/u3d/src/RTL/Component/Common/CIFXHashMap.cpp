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
	@file	CIFXHashMap.cpp

			Main implementation file for the hash map subsystem.
*/

#include <string.h>
#include "CIFXHashMap.h"
#include "FNVPlusPlus.h"


//---------------------------------------------------------------------------
//	CIFXHashMap constructor
//
//	This method initializes the refCount to 0
//---------------------------------------------------------------------------
CIFXHashMap::CIFXHashMap() 
{
	m_uRefCount=0;
	m_ppHashTable=NULL;
	m_uHashTableSize=0;
}

//---------------------------------------------------------------------------
//	CIFXHashMap destructor
//
//	This method does nothing!
//---------------------------------------------------------------------------
CIFXHashMap::~CIFXHashMap() 
{
	DeleteTable();
}

//---------------------------------------------------------------------------
//	CIFXHashMap::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXHashMap::AddRef(void) 
{
	return ++m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXHashMap::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXHashMap::Release(void) 
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXHashMap::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXHashMap::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXHashMap ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( CIFXHashMap* ) this;
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// IFXHashMap methods
//---------------------------------------------------------------------------
//	CIFXHashMap::Initialize
//
//	This method creates a table of pointers to HashMap structs and initalizes
//  them all to NULL as well as sets up all other variables.
//---------------------------------------------------------------------------
IFXRESULT CIFXHashMap::Initialize(U32 uHashSize) 
{
	IFXRESULT iResult=IFX_OK;

	if(uHashSize==0)
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) 
	{
		if(m_ppHashTable) 
			DeleteTable();
		
		m_ppHashTable=new HashMapObject*[uHashSize];
		
		if(m_ppHashTable==NULL)
			iResult=IFX_E_OUT_OF_MEMORY;
		
		if(IFXSUCCESS(iResult)) 
		{
			m_uHashTableSize=uHashSize;
			U32 uIndex;
			for(uIndex=0; uIndex<m_uHashTableSize; uIndex++) 
				m_ppHashTable[uIndex]=NULL;
		}
	}

	return iResult;
}

//---------------------------------------------------------------------------
//	CIFXHashMap::Add
//
//	This method provides adds a new hash object to the hash table in
//	the appropriate bucket.
//---------------------------------------------------------------------------
IFXRESULT CIFXHashMap::Add(IFXString* pString, U32 uID) 
{
	IFXRESULT iResult=IFX_OK;

	if(m_ppHashTable==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(pString==NULL)
		iResult=IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) 
	{
		U32 uHashIndex=0;
		HashMapObject* pHashEntry=new HashMapObject();
		if(pHashEntry==NULL)
			iResult=IFX_E_OUT_OF_MEMORY;

		if (IFXSUCCESS(iResult)) 
		{
			pHashEntry->pName=new IFXString(pString);
			if (pHashEntry->pName == NULL)
				iResult=IFX_E_INVALID_POINTER;
			else
				pHashEntry->uID=uID;
				pHashEntry->pNext=NULL;	
		}

		if (IFXSUCCESS(iResult))
			iResult=HashFunction(pString, &uHashIndex);

		// repoint the hash table to account for this new object and fix the objects
		// next pointer
		if (IFXSUCCESS(iResult)) 
		{
			pHashEntry->pNext=(HashMapObject*)m_ppHashTable[uHashIndex];
			m_ppHashTable[uHashIndex]=pHashEntry;
		}
	}
	return iResult;
}

//---------------------------------------------------------------------------
//	CIFXHashMap::Delete
//
//	This method provides removes and deallocates a hash object stored in a
//	hash table bucket.  
//---------------------------------------------------------------------------
IFXRESULT CIFXHashMap::Delete(IFXString* pString)
{
	IFXRESULT iResult=IFX_OK;

	if(m_ppHashTable==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(pString==NULL)
		iResult=IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) 
	{
		U32	uHashIndex;
		
		// hash the object name to get what bucket it should be in
		iResult=HashFunction(pString, &uHashIndex);
		if(IFXSUCCESS(iResult)) 
		{
			// item doesn't exist because bucket is empty
			if(m_ppHashTable[uHashIndex]==NULL) 
			{
				iResult=IFX_E_CANNOT_FIND;
			}
			
			// bucket not empty, scan the chain for the right value
			BOOL bFound=FALSE;
			HashMapObject* pCurrent=m_ppHashTable[uHashIndex];
			HashMapObject* pPrevious=m_ppHashTable[uHashIndex];

			// is it the first element in the bucket?
			if(pCurrent) 
			{
				if(pCurrent->pName->Compare(pString)== 0) 
				{
					bFound=TRUE;
					m_ppHashTable[uHashIndex]=pCurrent->pNext;
					if(pCurrent->pName)
						delete pCurrent->pName;
					delete pCurrent;
				} 
				else 
				{
					// no, so scan down the bucket to find it
					while(!bFound && pCurrent!=NULL) 
					{
						if(pCurrent->pName->Compare(pString)==0) 
						{
							bFound=TRUE;
							pPrevious->pNext=pCurrent->pNext;
							if(pCurrent->pName)
								delete pCurrent->pName;
							delete pCurrent;
						} 
						else 
						{
							pPrevious=pCurrent;
							pCurrent=pCurrent->pNext;
						}
					}
				}
			}
			if(!bFound) 
			{
				iResult=IFX_E_CANNOT_FIND;
			}
		}
	}

	return iResult;
}

//---------------------------------------------------------------------------
//	CIFXHashMap::Find
//
//	This method finds a hash object based on its name, and returns a pointer
//  to the appropriate hash object.  It addref()'s the object, so it is 
//	necissary to release it when done using it.
//---------------------------------------------------------------------------
IFXRESULT CIFXHashMap::Find(IFXString* pString, U32* pID)
{
	IFXRESULT iResult=IFX_OK;

	if(pID==NULL)
		iResult=IFX_E_INVALID_POINTER;

	if(m_ppHashTable==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(pString==NULL)
		iResult=IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) 
	{
		U32	uHashIndex;
		iResult=HashFunction(pString, &uHashIndex);
		if(IFXSUCCESS(iResult)) 
		{
			BOOL bFound=IFX_FALSE;
			HashMapObject* pCurrent=m_ppHashTable[uHashIndex];

			// scan down bucket until you hit the element or null
			while(!bFound && pCurrent!=NULL) 
			{
				if(pCurrent->pName->Compare(pString)==0) 
					bFound=IFX_TRUE;
				else
					pCurrent=pCurrent->pNext;
			}

			if(bFound) 
			{
				*pID=pCurrent->uID;
				iResult=IFX_OK;
			} 
			else 
			{
				*pID=0;
				iResult=IFX_E_CANNOT_FIND;
			}
		}
	}

	return iResult;
}

//---------------------------------------------------------------------------
//	CIFXHashMap::HashFunction
//
//	This method hashes the current string value and returns an index between 
//  0 and m_uHashTableSize.
//
//  The particular hashing function used is the Fowler/Noll/Vo- hash, 
//	specifically the 32-bit FNV-1a version.  Refer to the module in which 
//	it's defined for additional details.
//---------------------------------------------------------------------------

IFXRESULT CIFXHashMap::HashFunction(IFXString* pName, U32* pIndex) 
{
	IFXRESULT iResult=IFX_OK;

	if(pIndex==NULL) 
		iResult=IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) 
	{
		if(m_uHashTableSize==1) 
		{
			*pIndex=0;
		} 
		else 
		{
			*pIndex = fnv_32_str_with_hash_size( 
						( char* ) pName->Raw(), 
						FNV1_32A_INIT, 
						m_uHashTableSize );
		}
	}

	return iResult;
}


//---------------------------------------------------------------------------
//	CIFXHashMap::DeleteTable
//
//	This method deletes all the entries in the hash table, and then the
//  table itself afterwards to clean up all memory
//---------------------------------------------------------------------------
IFXRESULT CIFXHashMap::DeleteTable() {
	IFXRESULT iResult=IFX_OK;

	if( m_ppHashTable )
	{
		HashMapObject*	pCurrent=NULL;
		HashMapObject*	pNext=NULL;
		U32				uIndex;

		for(uIndex=0; uIndex<m_uHashTableSize; uIndex++)
		{
			pCurrent=m_ppHashTable[uIndex];
			m_ppHashTable[uIndex] = NULL;
			while ( pCurrent )
			{
				pNext=pCurrent->pNext;
				if(pCurrent->pName)
					delete pCurrent->pName;

				delete pCurrent;

				pCurrent=pNext;
			}

		}
		delete[] m_ppHashTable;
	}

	return iResult;
}


//---------------------------------------------------------------------------
//	CIFXHashMap_Factory (non-singleton)
//
//	This is the CIFXHashMap component factory function.  The
//	CIFXHashMap component is NOT a singleton.  This function creates the
//  HashMap object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXHashMap_Factory( IFXREFIID	interfaceId,
								void**		ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXHashMap	*pComponent	= new CIFXHashMap;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}
