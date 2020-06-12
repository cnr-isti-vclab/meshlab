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
#ifndef __PAIRHASH_H__
#define __PAIRHASH_H__
#include "Pair.h"
#include "Vertex.h"

class PairHash
{
public:
	PairHash(U32 Numbins, Vertex* vert);
	~PairHash();

	void Initialize(U32);

	void Insert(Pair *p);
	Pair* AddPair(Vertex*v1, Vertex *v2);
	Pair* Find(Vertex*v1, Vertex *v2);

	Pair* Find(Pair *p);
	void Remove(Pair *p);
	void Delete(Pair *p);

	Pair* Begin();
	Pair* Next();  //iter stuff

	U32 Size() { return m_Size; };

	Pair* NewPair();
	void DeletePair(Pair*);

	/// @todo: define better hash function
	U32 GetHashValue( Vertex *v1, Vertex *v2 )
	{
		U32 ret = ( v1 - m_Vert + v2 - m_Vert );
		return ret;
	};

private:
	Pair** m_pBins;
	U32 m_NumBins;
	U32 m_Size;
	Vertex* m_Vert;

	Pair* m_pAllocatedPairs;
	Pair* m_pEndAllocatedPair;
	U32 m_UsedAllocatedPairs;
	U32 m_NumAllocatedPairs;

	// iteration
	Pair* m_pCurrent;
	U32   m_CurrentBin;
};



inline Pair* PairHash::NewPair()
{
	if(m_UsedAllocatedPairs < m_NumAllocatedPairs)
	{
		return m_pAllocatedPairs + m_UsedAllocatedPairs++;
	}
	return new Pair;
}

inline void PairHash::DeletePair(Pair* in_pPair)
{
	if(in_pPair < m_pEndAllocatedPair && in_pPair >= m_pAllocatedPairs)
	{
		return;
	}
	delete in_pPair;
}


/* Hashing is based on the assumption that
all vertices are only allocated once and
they are passed around by pointer
so that they*/
inline void PairHash::Insert (Pair* pPair)
{
	IFXASSERT(pPair->m_pHashNext == NULL);
	m_Size++;
	U32 hv = GetHashValue( pPair->getv1(), pPair->getv2() ) % m_NumBins;
	pPair->m_pHashNext = m_pBins[hv];
	m_pBins[hv] = pPair;
}

inline Pair* PairHash::AddPair(Vertex*v1, Vertex *v2)
{
	Pair* pPair = Find(v1, v2);
	if(!pPair)
	{
		pPair = NewPair();
		pPair->Setup(v1, v2);
		Insert(pPair);
	}
	return pPair;
}


inline Pair* PairHash::Find (Vertex* v1, Vertex* v2)
{
	U32 ind = GetHashValue( v1, v2 );
	Pair* pCur =
		m_pBins[ind % m_NumBins];

	if(v1>v2)
	{
		Vertex* pv = v1;
		v1 = v2;
		v2 = pv;
	}

	while(pCur)
	{
		if(pCur->IsEqual(v1, v2))
		{
			return pCur;
		}
		pCur = pCur->m_pHashNext;
	}
	return NULL;
}

inline Pair* PairHash::Find (Pair* in_p)
{
	Vertex* v1 = in_p->getv1();
	Vertex* v2 = in_p->getv2();
	U32 ind = GetHashValue( v1, v2 );
	Pair* pCur =
		m_pBins[ind % m_NumBins];

	if(v1>v2)
	{
		Vertex* pv = v1;
		v1 = v2;
		v2 = pv;
	}

	while(pCur)
	{
		if(pCur->IsEqual(v1, v2))
		{
			return pCur;
		}
		pCur = pCur->m_pHashNext;
	}
	return NULL;
}

inline void PairHash::Remove(Pair* in_p)
{

	Pair** ppPrev = &(m_pBins[ GetHashValue(in_p->getv1(), in_p->getv2() ) % m_NumBins ]);
	Pair* pCur = *ppPrev;
	while(pCur)
	{
		if(pCur == in_p)
		{
			*ppPrev = pCur->m_pHashNext;
			m_Size--;
			pCur->m_pHashNext = NULL;
			return;
		}
		ppPrev = &(pCur->m_pHashNext);
		pCur = pCur->m_pHashNext;
	}

	IFXASSERT(0);
}

inline void PairHash::Delete(Pair* in_p)
{

	Pair** ppPrev = &(m_pBins[ GetHashValue(in_p->getv1(), in_p->getv2()) % m_NumBins ]);
	Pair* pCur = *ppPrev;
	while(pCur)
	{
		if(pCur == in_p)
		{
			*ppPrev = pCur->m_pHashNext;
			m_Size--;
			pCur->m_pHashNext = NULL;
			DeletePair(pCur);
			return;
		}
		ppPrev = &(pCur->m_pHashNext);
		pCur = pCur->m_pHashNext;
	}

	IFXASSERT(0);
}

inline Pair* PairHash::Begin()
{
	m_CurrentBin = 0;
	m_pCurrent = m_pBins[m_CurrentBin];

	while(!m_pCurrent && (++m_CurrentBin < m_NumBins))
	{
		m_pCurrent = m_pBins[m_CurrentBin];
	}

	if(m_CurrentBin >= m_NumBins)
	{
		return NULL;
	}
	return m_pCurrent;
}

inline Pair* PairHash::Next()
{
	if(m_pCurrent)
	{
		m_pCurrent = m_pCurrent->m_pHashNext;
	}

	while(!m_pCurrent && (++m_CurrentBin < m_NumBins))
	{
		m_pCurrent = m_pBins[m_CurrentBin];
	}

	if(m_CurrentBin >= m_NumBins)
	{
		return NULL;
	}
	return m_pCurrent;
}

#endif // __PAIRHASH_H__
