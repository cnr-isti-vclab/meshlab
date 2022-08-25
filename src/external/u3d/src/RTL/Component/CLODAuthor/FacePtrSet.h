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
#ifndef __FACEPTRSET_H__
#define __FACEPTRSET_H__

#include "SmallPtrSet.h"
#include "IFXDebug.h"
#include <memory.h>

/** 
simple set imple that will handle 2 values with out allocation
then uses doubling allocations to grow with linear searchse for the value 
with in the set. 
*/
class FacePtrSet
{
public:
	FacePtrSet();
	~FacePtrSet();

	void Insert(void*);
	void Remove(void*);

	U32 Size();
	void Clear();

	void* Begin(U32&);
	void* Next(U32&);
public:
	void** Grow();
	void Shrink();

	union PtrData
	{
		struct StaticData
		{
			void* m_pData[2];
		} sd;
		struct ArrayData
		{
			void** m_ppData;
			U32 m_MaxSize;
		} ad;
	};
	
	PtrData m_Data;
	U32 m_Size;
};

inline 	FacePtrSet::FacePtrSet()
{
	m_Size = 0;
	memset(&m_Data, 0, sizeof(PtrData));
}

inline FacePtrSet::~FacePtrSet()
{
	if(m_Size > 2)
	{
		delete [] m_Data.ad.m_ppData;
	}
}

inline void FacePtrSet::Insert(void* in_pPtr)
{
	U32 i = 0;
	//U32 cnt = 0;

	void** pData = m_Data.sd.m_pData;
	if(m_Size > 2)
	{
		pData = m_Data.ad.m_ppData;
	}

	for(i = 0; i < m_Size; ++i)
	{
		if(pData[i] == in_pPtr)
		{
			return;
		}
	}

	m_Size++;
	if(m_Size == 3 || (m_Size > 3 && m_Data.ad.m_MaxSize < m_Size))
	{
		pData = Grow();
	}
	pData[m_Size-1] = in_pPtr;
}

inline void FacePtrSet::Remove(void* in_pPtr)
{
	void** pData = m_Data.sd.m_pData;
	if(m_Size > 2)
	{
		pData = m_Data.ad.m_ppData;
	}

	U32 i;
	for(i = 0; i < m_Size; ++i)
	{
		if(pData[i] == in_pPtr)
		{
			m_Size--;
			if(m_Size)
			{
				pData[i] = pData[m_Size];
#if _DEBUG
				pData[m_Size] = NULL;
#endif
			}
			
			if(m_Size == 2 || (m_Size > 2 && m_Size < m_Data.ad.m_MaxSize/2))
			{
				Shrink();
			}
			return;
		}
	}
}

inline U32 FacePtrSet::Size()
{
	return m_Size;
}

inline void FacePtrSet::Clear()
{
	if(m_Size > 2)
	{
		delete [] m_Data.ad.m_ppData;
	}
	m_Size = 0;
}

inline void* FacePtrSet::Begin(U32& in_CurIdx)
{
	if(m_Size == 0)
	{
		return NULL;
	}
	in_CurIdx = 0;
	if(m_Size > 2)
	{
		return m_Data.ad.m_ppData[in_CurIdx++];
	}
	return m_Data.sd.m_pData[in_CurIdx++];
}

inline void* FacePtrSet::Next(U32& in_CurIdx)
{
	if(in_CurIdx >= m_Size)
	{
		return NULL;
	}
	if(m_Size > 2)
	{
		return m_Data.ad.m_ppData[in_CurIdx++];
	}
	return m_Data.sd.m_pData[in_CurIdx++];
}

inline void** FacePtrSet::Grow()
{	
	U32 NewSize = 4;
	void** ppOldData = m_Data.sd.m_pData;
	if(m_Size > 3)
	{
		NewSize = m_Data.ad.m_MaxSize * 2;
		ppOldData = m_Data.ad.m_ppData;
	}
	void** ppData = new void*[NewSize];
	memcpy( ppData, ppOldData, sizeof( void* ) * ( m_Size - 1 ) );
	if(m_Size > 3)
	{
		delete [] ppOldData;
	}
	m_Data.ad.m_ppData = ppData;
	m_Data.ad.m_MaxSize = NewSize;
	return ppData;
}

inline void FacePtrSet::Shrink()
{
	if(m_Size <= 2)
	{
		void** ppOldData = m_Data.ad.m_ppData;
		memcpy(m_Data.sd.m_pData, ppOldData, sizeof(void*) * m_Size);
		delete [] ppOldData;
		return;
	}
	void** ppOldData = m_Data.ad.m_ppData;
	U32 NewSize = m_Data.ad.m_MaxSize / 2;
	IFXASSERT(m_Size < NewSize);

	void** ppData = new void*[NewSize];
	memcpy(ppData, ppOldData, sizeof(void*) * m_Size);
	delete [] ppOldData;
	m_Data.ad.m_ppData = ppData;
	m_Data.ad.m_MaxSize = NewSize;
}

#endif
