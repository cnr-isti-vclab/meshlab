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

#ifndef __IFXSet_H__
#define __IFXSet_H__
#include "IFXDataTypes.h"


/**
 *	The IFXSet class provides set operations on typed data sets, using templates.  The set
 *	does not hold any references to the elements it manages.
 */

template<class T>
class IFXSet
{
public:
	IFXSet();
	~IFXSet();

	/**
	 *	This allows the insert of a new element into the set.  Nothing happens if the 
	 *	object is already in the set
	 *
	 *	@param	in_pPtr	Input pointer to new element.
	 */
	void Insert(const T& in_pPtr);

	/**
	 *	This allows the removal of an element from the set.  Nothing happens if the 
	 *	object is not already in the set
	 *
	 *	@param	in_pPtr	Input pointer to element to be removed.
	 */
	void Remove(const T& in_pPtr);

	/**
	 *	Returns the number of elements currently in the set.
	 *
	 *	@return	The count of items in the set.
	 */
	U32 Size();

	/**
	 *	Clears the current set, and minimizes memory usage of the internal data structures.
	 *
	 */
	void Clear();

	/**
	 *	Queries the set for the specified element.
	 *
	 *	@param	in_pPtr	Input pointer to element in question.
	 *
	 *	@return	TRUE if the element is found, FALSE otherwise.
	 */
	BOOL Has(const T& in_pPtr);

	/**
	 *	Returns the first element in the set, and sets in_Cur to the index the next element.
	 *
	 *	@param	in_Cur	Output U32 pointing to the 2nd element.
	 *
	 *	@return	The first element in the set.
	 */
	T* Begin(U32& in_Cur);

	/**
	 *	Returns the indexed element in the set, and sets in_Cur to the index the next element.
	 *
	 *	@param	in_Cur	Input/Output U32 pointing to the 2nd element.
	 *
	 *	@return	The current element in the set.
	 */
	T* Next(U32& in_Cur);

private:
	U32 m_Size;
	U32 m_MaxSize;
	T* m_ppData;
};


/*******************************************************************************/


/**
 *	Helper function to compute the union of 2 sets, appending one set to the other:  B = A union B.
 *
 *	@param	in_A	Input	Set A.
 *	@param	in_B	Input/Output Set B, which is updated to include the elements of Set A.
 */
template<class T, class S>
inline void IFXSet_Union(T* in_A, S* in_B)
{
	U32 ctx = 0;
	void* p = in_A->Begin(ctx);
	while( p )
	{
		in_B->Insert(p);
		p = in_A->Next(ctx);
	}
}

/**
 *	Helper function to create a new set, consisting of the union of 2 other sets:  C = A union B.
 *
 *	@param	in_A	Input	Set A.
 *	@param	in_B	Input	Set B.
 *	@param	in_C	Output	Set C, containing A union B.
 */
template<class T, class S, class U>
inline void IFXSet_Union(T* in_A, S* in_B, U* in_C)
{
	U32 ctx = 0;
	in_C->Clear();
	void* p = in_A->Begin(ctx);
	while( p )
	{
		in_C->Insert(p);
		p = in_A->Next(ctx);
	}

	p = in_B->Begin(ctx);
	while( p )
	{
		in_C->Insert(p);
		p = in_B->Next(ctx);
	}
}

/**
 *	Helper function to compute the difference of 2 sets, removing one set to the other:  A = A - B.
 *
 *	@param	in_A	Input/Output	Set A, which is updated to remove the elements of Set B.
 *	@param	in_B	Input Set B.
 */
template<class T, class S>
inline void IFXSet_Difference(T* in_A, S* in_B)
{
	U32 ctx = 0;
	void* p = in_A->Begin(ctx);
	while( p )
	{
		in_B->Remove( p );
		p = in_A->Next(ctx);
	}
}

/**
 *	Helper function to create a new set, consisting of the difference of 2 other sets:  C = A - B.
 *
 *	@param	in_A	Input	Set A.
 *	@param	in_B	Input	Set B.
 *	@param	in_C	Output	Set C, containing A - B.
 */
template<class T, class S, class U>
inline void IFXSet_Difference(T* in_A, S* in_B, U* in_C)
{
	U32 ctx = 0;
	in_C->Clear();
	void* p = in_A->Begin( ctx );
	while(p)
	{
		in_C->Insert( p );
		p = in_A->Next(ctx);
	}

	p = in_B->Begin( ctx );
	while(p)
	{
		in_C->Remove( p );
		p = in_B->Next( ctx );
	}
}

/*******************************************************************************/

template<class T>
inline IFXSet<T>::IFXSet()
{
	m_Size = 0;
	m_MaxSize = 1;
	m_ppData = new T[m_MaxSize];
}

template<class T>
inline IFXSet<T>::~IFXSet()
{
	delete[] m_ppData;
}


template<class T>
inline void IFXSet<T>::Insert(const T& in_pPtr)
{
	U32 i = 0;
	for( i = 0; i < m_Size; ++i )
	{
		if( m_ppData[i] == in_pPtr )
		{	// found existing -> nothing to do
			return;
		}
	}

	if( m_Size+1 >= m_MaxSize )
	{
		m_MaxSize = m_MaxSize *2;
		T* ppNewData = new T[m_MaxSize];
		U32 i;
		for( i = 0; i < m_Size; ++i )
		{
			ppNewData[i] = m_ppData[i];
		}
		delete[] m_ppData;
		m_ppData = ppNewData;
	}

	m_ppData[m_Size] = in_pPtr;
	m_Size++;
}

template<class T>
inline void IFXSet<T>::Remove(const T& in_pPtr)
{
	U32 i;
	for( i = 0; i < m_Size; ++i )
	{
		if(m_ppData[i] == in_pPtr)
		{
			m_Size--;
			if(i != m_Size)
			{
				m_ppData[i] = m_ppData[m_Size];
				if( m_Size < m_MaxSize/2 )
				{
					m_MaxSize = m_MaxSize/2;
					T* ppNewData = new T[m_MaxSize];
					U32 j;
					for( j = 0; j < m_Size; ++j )
					{
						ppNewData[j] = m_ppData[j];
					}
					delete[] m_ppData;
					m_ppData = ppNewData;
				}
			}
			return;
		}
	}
}

template<class T>
inline BOOL IFXSet<T>::Has(const T& in_Item)
{
	U32 i;
	for( i = 0; i < m_Size; ++i )
	{
		if(m_ppData[i] == in_Item)
		{
			return TRUE;
		}
	}
	return FALSE;
}

template<class T>
inline U32 IFXSet<T>::Size()
{
	return m_Size;
}

template<class T>
inline void IFXSet<T>::Clear()
{
	if(m_MaxSize > 4)
	{
		delete [] m_ppData;
		m_ppData = new T[4];
		m_MaxSize = 4;
	}
	memset(m_ppData, 0, sizeof(T) * m_MaxSize);
	m_Size	= 0;
}

template<class T>
inline T* IFXSet<T>::Begin(U32& in_Cur)
{
	in_Cur = 0;
	if(m_Size == 0)
	{
		return NULL;
	}
	return &(m_ppData[in_Cur++]);
}

template<class T>
inline T* IFXSet<T>::Next(U32& in_Cur)
{
	if(in_Cur >= m_Size)
	{
		return NULL;
	}
	return &(m_ppData[in_Cur++]);
}

#endif
