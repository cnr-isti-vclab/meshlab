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
	@file	CArrayList.h

			This simple data structure manages a growing array and has a 
			somewhat STL-like list interface.  This data structure provides 
			cleaner code by encapsulating code complexity and also 
			increasing speed.  It has improved cache coherence over an STL 
			list for most operations.
*/


#ifndef _CARRAY_H_
#define _CARRAY_H_


#include "IFXDataTypes.h"
#include "IFXDebug.h"
#include "IFXMemory.h"
#include <memory.h>
#include <stdlib.h>


const U32 DEFAULT_ARRAY_GROWSIZE = 8;


template <class T>
/**
	CArrayList is a simple data structure that manages a growing array and 
	has a somewhat STL-like list interface.  This data structure provides 
	cleaner code, by encapsulating code complexity, and increases speed.

	@note	Removal functionality does not maintain element ordering.

	@note	No arbitrary insertion functionality is provided.
*/
class CArrayList
{
private:
	T*  m_data;
	U32 m_allocatedMem;
	U32 m_dataCnt;
	U32 m_growSize;

public:
	 CArrayList(U32 grow = DEFAULT_ARRAY_GROWSIZE)
	 {
		 m_data			= NULL;
		 m_allocatedMem = 0;
		 m_dataCnt		= 0;
		 m_growSize		= grow;
	 }

	 CArrayList(T* pdata, U32 mem, U32 cnt = 0, U32 grow = DEFAULT_ARRAY_GROWSIZE)
	 {
		 m_data			= pdata;
		 m_allocatedMem = mem;
		 m_dataCnt		= cnt;
		 m_growSize		= grow;
	 }

	~CArrayList()
	{
		if( NULL != m_data )
		{
			IFXDeallocate( m_data );
			m_data = NULL;
		}

		m_allocatedMem = 0;
		m_dataCnt = 0;
	}

	void initialize(U32 grow = DEFAULT_ARRAY_GROWSIZE)
	{
		if( NULL != m_data )
		{
			IFXDeallocate( m_data );
			m_data = NULL;
		}

		m_dataCnt		= 0;
		m_allocatedMem	= 0;
		m_growSize		= grow;
	}

	void destroy()
	{
		if( NULL != m_data )
		{
			IFXDeallocate( m_data );
			m_data = NULL;
		}

		m_allocatedMem = 0;
		m_dataCnt = 0;
	}

	BOOL empty() const
	{
		return m_dataCnt == 0 ? TRUE : FALSE;
	}

	U32  begin() const
	{
		return 0;
	}

	U32  end()  const
	{
		return m_dataCnt;
	}

	U32  size() const
	{
		return m_dataCnt;
	}

	void clear()
	{
		destroy();
		m_dataCnt = 0;
	}

	BOOL remove( U32 index )
	{
		BOOL found = FALSE;

		if( m_dataCnt && index < m_dataCnt )
		{
			m_dataCnt--;
			IFXASSERT(index<m_allocatedMem);
			m_data[index] = m_data[m_dataCnt];
			found = TRUE;
		}

		return found;
	}

	BOOL remove( const T& data)
	{
		U32 index =0;
		BOOL found = FALSE;

		if( m_dataCnt && find(data,index))
			found  = remove(index);

		return found;
	}

	U32 push_back(const T& data)
	{
		if(0 == m_allocatedMem || NULL == m_data)
		{
			m_data = (T*)IFXAllocate(sizeof(T)*(m_growSize+m_allocatedMem));

			if( NULL != m_data )
			{
				m_allocatedMem += m_growSize;
				m_dataCnt = 0;
			}
		}

		IFXASSERT(m_data);

		if(m_dataCnt >= m_allocatedMem)
		{
			m_data = (T*)IFXReallocate(m_data, sizeof(T)*(m_allocatedMem+m_growSize));
			IFXASSERT(m_data);
			m_allocatedMem += m_growSize;
		}

		IFXASSERT(m_dataCnt<m_allocatedMem);
		m_data[m_dataCnt] = data;

		return m_dataCnt++;
	}

	T pop()
	{
		m_dataCnt--;
		IFXASSERT(m_dataCnt<m_allocatedMem);
		return m_data[m_dataCnt];
	}

	BOOL find(T data, U32& index)
	{
		BOOL found = FALSE;

		for( index = 0; index != m_dataCnt; index++ )
		{
			if( m_data[index] == data )
			{
				found = TRUE;
				break;
			}
		}

		return found;
	}

	T& operator[](U32 i)
	{
		if(i >= m_allocatedMem)
		{
			m_allocatedMem = ((i/m_growSize) + 1)*m_growSize;
			m_data = (T*)IFXReallocate(m_data, sizeof(T)*(m_allocatedMem));
			IFXASSERT(m_data);
		}

		if(i >= m_dataCnt)
		{
			IFXASSERT( i < m_allocatedMem );
			m_dataCnt = i + 1;
		}

		IFXASSERT(i<m_allocatedMem);
		return m_data[i];
	}

	void growToAtLeast(U32 size)
	{
		if(size >= m_allocatedMem)
		{
			m_allocatedMem = ((size/m_growSize) + 1)*m_growSize;
			m_data = (T*)IFXReallocate(m_data, sizeof(T)*(m_allocatedMem));
			IFXASSERT(m_data);
		}
	}

	void append(const T* pdata,U32 uInSize)
	{
		if(uInSize > 0)
		{
			if(0 == m_allocatedMem)
			{
				m_allocatedMem = ((m_dataCnt + uInSize)/m_growSize + 1)*m_growSize;
				m_data = (T*)IFXAllocate(sizeof(T)*m_allocatedMem);
				IFXASSERT(m_data);
			}
			else if(m_dataCnt + uInSize >= m_allocatedMem)
			{
				m_allocatedMem = ((m_dataCnt + uInSize)/m_growSize + 1)*m_growSize;
				m_data = (T*)IFXReallocate(m_data, sizeof(T)*(m_allocatedMem));
				IFXASSERT(m_data);
			}

			IFXASSERT(m_dataCnt<m_allocatedMem);
			memcpy((U8*)&m_data[m_dataCnt],(U8*)pdata, sizeof(T)*uInSize);
			m_dataCnt += uInSize;
		}
	}

	void append(const CArrayList<T>* inList)
	{
		append(inList->m_data,inList->size());
	}

	const T* getDataPtr()
	{
		return m_data;
	}

	#ifdef _DEBUG
	void dump()
	{
		U32 index = 0;

		if(m_dataCnt)
		{
			FILE* fp;

			fp = fopen("ArrayListDump.txt", "at");

			fprintf(fp,"\n\n ListDump... \nAddress  %x \n",this);

			for(index = 0; index != m_dataCnt; ++index )
			{
				fprintf(fp,"\n i= %d, data[i]= %x",index,m_data[index]);
			}

			fclose(fp);
		}
	}
	#endif

	void swap( F64* pKeys, U32 i, U32 j )
	{
		T Temp = m_data[i];
		m_data[i] = m_data[j];
		m_data[j] = Temp;

		F64 fTemp = pKeys[i];
		pKeys[i] = pKeys[j];
		pKeys[j] = fTemp;
	}

	void swap( F32* pKeys, U32 i, U32 j )
	{
		T Temp = m_data[i];
		m_data[i] = m_data[j];
		m_data[j] = Temp;

		F32 fTemp = pKeys[i];
		pKeys[i] = pKeys[j];
		pKeys[j] = fTemp;
	}
};

#endif
