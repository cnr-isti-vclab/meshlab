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
	@file	IFXHash.h

	A generic hash map implementation. Takes templated class arguments
	class K, representing the key. This class is expected to have assignment
	operator(=) defined and the comparison operator(==). Additionally a
	default constructor is expected.

	class T, represents the data associated with a given key value. 
		This class is expected to have assignment
		operator(=) defined and default constructor is expected.
	class H, represents a class that implements a Hash function for K
		It is expected to implement U32 operator(const K&) that returns
		the hash value for the key.

	Iteration
	@code
		Given IFXHash<int, int, IntHasher> hash;
		if(hash.GetFirst())
		{
			do
			{
				hash.GetKey();
				hash.GetData();
			}
			while(hash.GetNext())
		}
	@endcode

	Adding and removing elements during Iteration will cause the iteration
	to be invalidated and possibly in to an unstable state.
*/

#ifndef __IFXHash_H__
#define __IFXHash_H__

#include "IFXDataTypes.h"
#include "IFXDebug.h"

template<class K>
class IFXHashDefaultCmp
{
	public:
		BOOL operator()(const K& arg1, const K& arg2)
		{ return arg1 == arg2; };
};

class IFXU32Hasher
{
public:
	U32 operator()(U32 arg1)
	{ return arg1; };
};



//***************************************************************************
//	Classes, structures and types
//***************************************************************************
template <class K, class T, class H, class E = IFXHashDefaultCmp<K> >

/**
	This interface is for a generic hash map implementation. It takes templated class 
	arguments:\n
	-class K, representing the key. This class is expected to have defined an assignment
	operator(=) and a comparison operator(==). Additionally a
	default constructor is expected.\n
	-class T, representing the data associated with a given key value. This class is 
	expected to have a defined assignment operator(=) and a default constructor.\n
	-class H, representing a class that implements a Hash function for K.
	It is expected to implement a U32 operator(const K&) that returns the hash value for the key.
*/
class IFXHash
{
public:
	IFXHash(U32 in_Size = 127);
	~IFXHash();

	BOOL Set		(const K&, const T&);
	BOOL Remove		(const K&);
	BOOL Has		(const K&);
	BOOL Find		(const K&, T*&);
	T& operator[](const K&);
	void Clear();

	// Iteration
	BOOL GetFirst();
	BOOL GetNext();
	const K& GetKey();
	T& GetData();


private:
	// The hash bucket:
	struct BucketEntry
	{
		BucketEntry(const K& in_Key, BucketEntry* in_pNext)
			: Key(in_Key), pNext(in_pNext) {};
		K				Key;
		T				Data;
		BucketEntry*	pNext;
	};

	struct Bucket
	{
		Bucket() { m_pEntries = NULL;
					m_pNextFilled = NULL;
					m_pPrevFilled = NULL; };
		~Bucket()
		{
			while (m_pEntries)
			{
				BucketEntry* pTmpBucket1 = m_pEntries;
				m_pEntries = m_pEntries->pNext;
				delete pTmpBucket1;
			}
		};
		T& Get(const K& in_Key)
		{
			BucketEntry* pTmpBucket = m_pEntries;
			while (pTmpBucket)
			{
				if(m_xCompareFunc(pTmpBucket->Key,in_Key))
				{
					return pTmpBucket->Data;
				}
				pTmpBucket = pTmpBucket->pNext;
			}
			m_pEntries = new BucketEntry(in_Key, m_pEntries);
			return m_pEntries->Data;
		};

		BOOL Find(const K& in_Key, T*& out_pData)
		{
			BucketEntry* pTmpBucket = m_pEntries;
			while (pTmpBucket)
			{
				if(m_xCompareFunc(pTmpBucket->Key,in_Key))
				{
					out_pData = &(pTmpBucket->Data);
					return TRUE;
				}
				pTmpBucket = pTmpBucket->pNext;
			}
			return FALSE;
		};

		BOOL Remove(const K& in_Key)
		{
			BucketEntry* pTmpBucket = m_pEntries;
			BucketEntry* pPrevBucket = NULL;
			while (pTmpBucket)
			{
				if(m_xCompareFunc(pTmpBucket->Key,in_Key))
				{
					if(pPrevBucket)
					{
						pPrevBucket->pNext = pTmpBucket->pNext;
					}
					else
					{
						m_pEntries = pTmpBucket->pNext;
					}
					delete pTmpBucket;
					return TRUE;
				}
				pPrevBucket = pTmpBucket;
				pTmpBucket = pTmpBucket->pNext;
			}
			return FALSE;
		};

		void Clear()
		{
			m_pNextFilled = NULL;
			m_pPrevFilled = NULL;
			while (m_pEntries)
			{
				BucketEntry* pTmpBucket1 = m_pEntries;
				m_pEntries = m_pEntries->pNext;
				delete pTmpBucket1;
			}
			m_pEntries = NULL;
		};

		BucketEntry* 			m_pEntries;
		E						m_xCompareFunc;
		// pointers to the next and previous filled buckets.
		Bucket* 				m_pNextFilled;
		Bucket* 				m_pPrevFilled;
	};

	// Hash data members
	U32						m_uHashTableSize;
	Bucket*					m_pHashTable;

	// First Bucket with data
	Bucket*					m_pFirstFilled;

	// Iteration State
	Bucket*					m_pCurrentBucket;
	BucketEntry*			m_pCurrentEntry;

	H 						m_xHashFunc;
};


//***************************************************************************
//	Inline implementation:
//***************************************************************************

/** Constructs the Hash to have the specified number of buckets. 
	If no size is specified, the hash defaults to 29 buckets.
*/
template <class K, class T, class H, class E>
IFXINLINE IFXHash<K, T, H, E>::IFXHash(U32 in_Size)
{
	m_pHashTable		= new Bucket[in_Size];
	m_uHashTableSize	= in_Size;
	m_pFirstFilled		= NULL;
	m_pCurrentBucket	= NULL;
	m_pCurrentEntry		= NULL;

}

/** Destroys the hash and cleans up all the
	entries.*/
template <class K, class T, class H, class E>
IFXINLINE IFXHash<K, T, H, E>::~IFXHash()
{
	if(m_pHashTable)
	{
		delete [] m_pHashTable;
	}
}

/** Sets an existing member of the hash to a specified value. If no entry in
	the hash matches the specified key the value is NOT added and the
	function returns TRUE.*/
template <class K, class T, class H, class E>
IFXINLINE BOOL IFXHash<K, T, H, E>::Set(const K& in_Key, const T& in_Data)
{
	T* pData = NULL;
	Bucket& rBucket = m_pHashTable[m_xHashFunc(in_Key) % m_uHashTableSize];
	if(rBucket.Find(in_Key, pData))
	{
		*pData = in_Data;
		return TRUE;
	}
	return FALSE;
}

/** Returns a reference to a particular item in the hash if it does not
	exist it is added.
*/
template <class K, class T, class H, class E>
IFXINLINE T& IFXHash<K, T, H, E>::operator [](const K& in_Key)
{
	Bucket& rBucket = m_pHashTable[m_xHashFunc(in_Key) % m_uHashTableSize];
	T& retval = rBucket.Get(in_Key);
	if(!rBucket.m_pNextFilled && !rBucket.m_pPrevFilled && m_pFirstFilled != &rBucket) // if bucket not in list
	{
		rBucket.m_pPrevFilled = NULL;
		rBucket.m_pNextFilled = m_pFirstFilled;
		if(m_pFirstFilled)
		{
			m_pFirstFilled->m_pPrevFilled = &rBucket;
		}
		m_pFirstFilled = &rBucket;
	}
	return retval;
}


/** Looks for a specific Item in the hash, return a pointer to it.
	If it does not exist the pointer is not set and the function returns
	FALSE.
*/
template <class K, class T, class H, class E>
BOOL IFXHash<K, T, H, E>::Find	(const K& in_Key, T*& out_pData)
{
	return m_pHashTable[m_xHashFunc(in_Key) % m_uHashTableSize].Find(in_Key, out_pData);
}

/** Removes an entry from the hash, returns False if the entry is not found.

*/
template <class K, class T, class H, class E>
IFXINLINE BOOL IFXHash<K, T, H, E>::Remove (const K& in_Key)
{
	Bucket& rBucket = m_pHashTable[m_xHashFunc(in_Key) % m_uHashTableSize];
	BOOL retval = rBucket.Remove(in_Key);
	if(retval && !rBucket.m_pEntries)
	{
		if(!rBucket.m_pPrevFilled) // Front of list
		{
			m_pFirstFilled = rBucket.m_pNextFilled;
			if(m_pFirstFilled)
			{
				m_pFirstFilled->m_pPrevFilled = NULL;
			}
		}
		else
		{
			rBucket.m_pPrevFilled->m_pNextFilled = rBucket.m_pNextFilled;			
		}
		if(rBucket.m_pNextFilled) // !End of list
		{
			rBucket.m_pNextFilled->m_pPrevFilled = rBucket.m_pPrevFilled;
		}

		rBucket.m_pPrevFilled = NULL;
		rBucket.m_pNextFilled = NULL;

	}
	return retval;
}

/** Tests whether there is an entry with the specified key.
	Returns true if an entry is found, false other wise.
*/
template <class K, class T, class H, class E>
IFXINLINE BOOL IFXHash<K, T, H, E>::Has(const K& in_Key)
{
	T* pData = NULL;
	return m_pHashTable[m_xHashFunc(in_Key) % m_uHashTableSize].Find(in_Key, pData);
}

/** Empties the hash.
*/
template <class K, class T, class H, class E>
IFXINLINE void IFXHash<K, T, H, E>::Clear()
{
	m_pCurrentBucket = NULL;
	m_pCurrentEntry = NULL;
	m_pFirstFilled = NULL;
	U32 i;
	for(i = 0; i < m_uHashTableSize; ++i)
	{
		m_pHashTable[i].Clear();
	}
}

/** Sets the iteration status to the first item in the hash.
	Returns false if the hash is empty.
*/
template <class K, class T, class H, class E>
IFXINLINE BOOL IFXHash<K, T, H, E>::GetFirst()
{
	m_pCurrentBucket = m_pFirstFilled;
	m_pCurrentEntry = NULL;
	if(m_pCurrentBucket)
	{
		m_pCurrentEntry = m_pCurrentBucket->m_pEntries;
		return TRUE;
	}
	return FALSE;
}

/** Sets the iteration status to the next item in the hash.
*/
template <class K, class T, class H, class E>
IFXINLINE BOOL IFXHash<K, T, H, E>::GetNext()
{
	IFXASSERT(m_pCurrentEntry);
	if(m_pCurrentEntry->pNext)
	{
		m_pCurrentEntry = m_pCurrentEntry->pNext;
		return TRUE;
	}
	else
	{
		if(m_pCurrentBucket)
		{
			m_pCurrentBucket = m_pCurrentBucket->m_pNextFilled;	
		}
		if(m_pCurrentBucket)
		{
			m_pCurrentEntry = m_pCurrentBucket->m_pEntries;
			return TRUE;
		}
	}
	return FALSE;
}

/** Gets a constant reference to the current iteration item.*/
template <class K, class T, class H, class E>
IFXINLINE const K& IFXHash<K, T, H, E>::GetKey()
{
	IFXASSERT(m_pCurrentEntry);
	return m_pCurrentEntry->Key;
}

/** Get a constant reference to the current iteration item.*/
template <class K, class T, class H, class E>
IFXINLINE T& IFXHash<K, T, H, E>::GetData()
{
	IFXASSERT(m_pCurrentEntry);
	return m_pCurrentEntry->Data;
}

#endif
