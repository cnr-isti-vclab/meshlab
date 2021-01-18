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
#include "PairHash.h"
#include "Pair.h"


PairHash::PairHash( U32 n, Vertex *vert )
{
	m_Size = 0;
	
	m_pCurrent = NULL;
	m_CurrentBin= 0;
	m_Vert = vert;

	m_NumBins = n;
	m_pBins = new Pair*[m_NumBins];
	memset(m_pBins, 0, m_NumBins * sizeof(Pair*));

	m_pAllocatedPairs = NULL;
	m_pEndAllocatedPair = NULL;
	m_NumAllocatedPairs = 0;
	m_UsedAllocatedPairs = 0;

}

PairHash::~PairHash()
{

	Pair* pNext = NULL,* pCur = NULL;
	U32 i;
	for( i = 0; i < m_NumBins; ++i)
	{
		pNext = m_pBins[i];
		while(pNext)
		{
			pCur = pNext;
			pNext = pCur->m_pHashNext;
			DeletePair(pCur);
		}
	}
	delete [] m_pBins;
	delete [] m_pAllocatedPairs;
}

void PairHash::Initialize(U32 in_Size)
{
	IFXASSERT(!m_pAllocatedPairs);
	m_NumAllocatedPairs = in_Size;
	m_pAllocatedPairs = new Pair[in_Size];
	m_pEndAllocatedPair = m_pAllocatedPairs + in_Size;
	m_UsedAllocatedPairs = 0;
}
