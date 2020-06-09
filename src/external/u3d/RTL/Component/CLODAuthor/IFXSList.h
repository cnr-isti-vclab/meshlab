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
	@file	IFXSList.h

	A generic singly linked list implementation, provided to be simpler, lighter
	easier to use list implementation than the IFXList, which is much more
	robust and full featured but heavier weight and more dificult to use.
	class T, represents the data associated with a given key value. This class is expected
	to have assignment operator(=) defined, the comparison operator(==) defined
	and a default constructor is expected.

	@code
	Iteration
	Given IFXSList<int> list;
	if(list.GetFirst())
	{
		do
		{
		list.GetCurrentData();
		}
		while(list.GetNext())
	}
	Accessors
	@endcode

	@todo: Modification During Iteration
*/

#ifndef __IFXSList_H__
#define __IFXSList_H__

#include "IFXDataTypes.h"
#include "IFXDebug.h"

//***************************************************************************
//  Classes, structures and types
//***************************************************************************
template <class T>
class IFXSList
{
public:
	IFXSList();
	~IFXSList();

	BOOL Insert(const T&);
	BOOL Push(const T&);
	BOOL Pop();
	T    Head();

	BOOL Remove(const T&);
	void Clear();

	U32 Size();

	// Iteration
	BOOL GetFirst();
	BOOL GetNext();
	T& GetCurrentData();

	// These manipulators can be used safely during iteration
	// they return True if the list still has items to iterate
	// False if the Action moved the iterator to the end of the
	// list.
	BOOL RemoveCurrent();
	BOOL InsertAfter(const T&);
	BOOL InsertBefore(const T&);

private:
	// The List Node:
	struct Node
	{
		Node(const T& in_Data, Node* in_pNext)
			: Data(in_Data), pNext(in_pNext) {};
		T       Data;
		Node*   pNext;
	};

	// List Head
	Node*   m_pFirst;
	U32     m_Size;

	// Iteration State
	Node** m_ppPrev;
	Node* m_pCur;
};


//***************************************************************************
//  Inline implementation:
//***************************************************************************

template <class T>
IFXINLINE IFXSList<T>::IFXSList()
{
	m_pFirst = NULL;
	m_ppPrev = NULL;
	m_pCur = NULL;
	m_Size = 0;
}

template <class T>
IFXINLINE IFXSList<T>::~IFXSList()
{
	Clear();
}

template <class T>
IFXINLINE BOOL IFXSList<T>::Insert(const T& in_Data)
{
	m_pFirst = new Node(in_Data, m_pFirst);
	++m_Size;
	return TRUE;
}

template <class T>
IFXINLINE BOOL IFXSList<T>::Push(const T& in_Data)
{
	m_pFirst = new Node(in_Data, m_pFirst);
	++m_Size;
	return TRUE;
}

template <class T>
IFXINLINE BOOL IFXSList<T>::Pop()
{
	if(m_pFirst)
	{
		Node* pNode = m_pFirst;
		m_pFirst = pNode->pNext;
		delete pNode;
		--m_Size;
		return TRUE;
	}
	return FALSE;
}

template <class T>
IFXINLINE T IFXSList<T>::Head()
{
	if(m_pFirst)
	{
		return m_pFirst->Data;
	}

	T result;
	return result; // return Dummy data
}

template <class T>
IFXINLINE BOOL IFXSList<T>::Remove (const T& in_Data)
{
	Node* pCur = m_pFirst;
	Node** ppPrev = &m_pFirst;
	while(pCur)
	{
		if(pCur->Data == in_Data)
		{
			*ppPrev = pCur->pNext;
			delete pCur;
			--m_Size;
			return TRUE;
		}

		ppPrev = &(pCur->pNext);
		pCur = pCur->pNext;
	}
	return FALSE;
}

template <class T>
IFXINLINE void IFXSList<T>::Clear()
{
	while(m_pFirst)
	{
		Node* t = m_pFirst->pNext;
		delete m_pFirst;
		m_pFirst = t;
	}
	m_Size = 0;
}

template <class T>
IFXINLINE U32 IFXSList<T>::Size()
{
	return m_Size;
}

template <class T>
IFXINLINE BOOL IFXSList<T>::GetFirst()
{
	m_ppPrev = &m_pFirst;
	m_pCur = m_pFirst;
	return m_pCur != NULL;
}

template <class T>
IFXINLINE BOOL IFXSList<T>::GetNext()
{
	IFXASSERT(m_pCur);
	m_ppPrev = &(m_pCur->pNext);
	m_pCur = m_pCur->pNext;
	return m_pCur != NULL;
}

template <class T>
IFXINLINE T& IFXSList<T>::GetCurrentData()
{
	IFXASSERT(m_pCur);
	return m_pCur->Data;
}

template <class T>
BOOL IFXSList<T>::RemoveCurrent()
{
	IFXASSERT(m_pCur);
	IFXASSERT(m_ppPrev);
	Node* t = m_pCur;
	m_pCur = m_pCur->pNext;
	delete t;
	*m_ppPrev = m_pCur;
	--m_Size;
	return m_pCur != NULL;
}

template <class T>
BOOL IFXSList<T>::InsertAfter(const T& in_Data)
{
	IFXASSERT(m_pCur);
	m_pCur->pNext = new Node(in_Data, m_pCur->pNext);
	++m_Size;
	return TRUE;
}

template <class T>
BOOL IFXSList<T>::InsertBefore(const T& in_Data)
{
	IFXASSERT(m_pCur);
	IFXASSERT(m_ppPrev);
	*m_ppPrev = new Node(in_Data, m_pCur);
	++m_Size;
	return TRUE;
}

#endif
