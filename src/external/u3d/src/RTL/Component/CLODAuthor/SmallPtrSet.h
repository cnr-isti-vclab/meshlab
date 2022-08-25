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
#ifndef __SmallPtrSet_H__
#define __SmallPtrSet_H__

#include <memory.h>
#include "IFXDataTypes.h"

/// @todo: REPLACE THIS IMPL

class SmallPtrSet
{
public:
	SmallPtrSet();
	~SmallPtrSet();

	void Insert(void*);
	void Remove(void*);

	U32 Size();
	void Clear();

	void* Begin(U32&);
	void* Next(U32&);

private:
	U32 m_Size;
	U32 m_MaxSize;
	void** m_ppData;
};

// set union,  b = a union b
template<class T, class S>
inline void SmallPtrSet_Union(T* in_A, S* in_B)
{
	U32 ctx = 0;
	void* p = in_A->Begin(ctx);
	while(p)
	{
		in_B->Insert(p);
		p = in_A->Next(ctx);
	}
}

// c = a + b
template<class T, class S, class U>
inline void SmallPtrSet_Union(T* in_A, S* in_B, U* in_C)
{
	U32 ctx = 0;
	in_C->Clear();
	void* p = in_A->Begin(ctx);
	while(p)
	{
		in_C->Insert(p);
		p = in_A->Next(ctx);
	}

	p = in_B->Begin(ctx);
	while(p)
	{
		in_C->Insert(p);
		p = in_B->Next(ctx);
	}
}

// set difference, a = a - b
template<class T, class S>
inline void SmallPtrSet_Difference(T* in_A, S* in_B)
{
	U32 ctx = 0;
	void* p = in_A->Begin(ctx);
	while(p)
	{
		in_B->Remove(p);
		p = in_A->Next(ctx);
	}
}

// set difference, c = a - b
template<class T, class S, class U>
inline void SmallPtrSet_Difference(T* in_A, S* in_B, U* in_C)
{
	U32 ctx = 0;
	in_C->Clear();
	void* p = in_A->Begin(ctx);
	while(p)
	{
		in_C->Insert(p);
		p = in_A->Next(ctx);
	}

	p = in_B->Begin(ctx);
	while(p)
	{
		in_C->Remove(p);
		p = in_B->Next(ctx);
	}
}

inline SmallPtrSet::SmallPtrSet()
{
	m_Size = 0;
	m_MaxSize = 4;
	m_ppData = new void*[m_MaxSize];
}

inline SmallPtrSet::~SmallPtrSet()
{
	delete[] m_ppData;
}

inline void SmallPtrSet::Insert(void* in_pPtr)
{
	U32 i = 0;
	for(i = 0; i < m_Size ; ++i)
	{
		if(m_ppData[i] == in_pPtr)
		{ // found existing nothing to do
			return;
		}
	}

	if(m_Size+1 >= m_MaxSize)
	{
		m_MaxSize = m_MaxSize *2;
		void** ppNewData = new void*[m_MaxSize];
		memcpy(ppNewData, m_ppData, sizeof(void*) * (m_MaxSize/2) );
		delete[] m_ppData;
		m_ppData = ppNewData;
	}

	m_ppData[m_Size] = in_pPtr;
	m_Size++;
}

inline void SmallPtrSet::Remove(void* in_pPtr)
{
	U32 i;
	for(i = 0; i < m_Size; ++i)
	{
		if(m_ppData[i] == in_pPtr)
		{
			m_Size--;
			if(i != m_Size)
			{
				m_ppData[i] = m_ppData[m_Size];
#ifdef _DEBUG
				m_ppData[m_Size] = NULL;
#endif
				if(m_Size < m_MaxSize/2)
				{
					m_MaxSize = m_MaxSize/2;
					void** ppNewData = new void*[m_MaxSize];
					memcpy(ppNewData, m_ppData, sizeof(void*) * (m_MaxSize) );
					delete[] m_ppData;
					m_ppData = ppNewData;
				}
			}
			return;
		}
	}
}

inline U32 SmallPtrSet::Size()
{
	return m_Size;
}

inline void SmallPtrSet::Clear()
{
	if(m_MaxSize > 4)
	{
		delete [] m_ppData;
		m_ppData = new void*[4];
		m_MaxSize = 4;
	}
	memset(m_ppData, 0, sizeof(void*) * m_MaxSize);
	m_Size  = 0;
}

inline void* SmallPtrSet::Begin(U32& in_Cur)
{
	in_Cur = 0;
	if(m_Size == 0)
	{
		return NULL;
	}
	return m_ppData[in_Cur++];
}

inline void* SmallPtrSet::Next(U32& in_Cur)
{
	if(in_Cur >= m_Size)
	{
		return NULL;
	}
	return m_ppData[in_Cur++];
}

#endif
