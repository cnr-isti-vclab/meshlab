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

#include "IFXArray.h"

IFXCoreArray::IFXCoreArray(U32 preallocation)
{
	m_pDeallocate=NULL;
	m_elementsUsed=0;
	m_elementsAllocated=0;
	m_array=NULL;
	m_prealloc=0;
	m_contiguous=NULL;
}


void IFXCoreArray::Clear(U32 m_preallocation)
{
	DestructAll();
	Preallocate(m_preallocation);
	if(m_preallocation>0)
		Resize((m_preallocation>IFXARRAY_MIN)? m_preallocation: IFXARRAY_MIN);
}


void IFXCoreArray::ResizeToExactly(U32 set)
{
	if(m_elementsAllocated<set)
		ResizeToAtLeast(set);
	else
	{
		U32 m;
		// if m_elementsUsed < set
		for(m=m_elementsUsed;m<set;m++)
			Construct(m);
		// if set < m_elementsUsed
		for(m=set;m<m_elementsUsed;m++)
			Destruct(m);

		m_elementsUsed=set;

		/// @todo: deallocate space for pointers
	}
}


void IFXCoreArray::ResizeToAtLeast(U32 required)
{
	if(m_elementsAllocated<required)
		Resize(required);

	U32 m;
	for(m=m_elementsUsed;m<required;m++)
		Construct(m);

	if(m_elementsUsed<required)
		m_elementsUsed=required;
}
