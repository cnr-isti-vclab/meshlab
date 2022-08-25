//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// CIFXSimpleHash.h
#ifndef CIFX_SIMPLEHASH_H
#define CIFX_SIMPLEHASH_H

#include "IFXSimpleHash.h"

class CIFXSimpleHashData;

class CIFXSimpleHash : virtual public IFXSimpleHash, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXSimpleHashFactory(IFXREFIID intId, void** ppvUnk);

	//==============================
	// IFXSimpleHash methods
	//==============================
	IFXRESULT IFXAPI Initialize(U32 uTableSize);

	IFXRESULT IFXAPI Clear();

	IFXRESULT IFXAPI AddData(U32 uId, IFXUnknownPtr& spUnk);

	IFXRESULT IFXAPI AddData(U32 uId, IFXUnknown* pUnk);

	IFXRESULT IFXAPI GetData(U32 uId, IFXUnknownPtr& spUnk);

	IFXRESULT IFXAPI GetData(U32 uId, IFXUnknown*& pUnk);

	IFXRESULT IFXAPI GetLowestId(U32& uId, IFXUnknownPtr& spUnk);

	IFXRESULT IFXAPI GetLowestId(U32& uId, IFXUnknown*& pUnk);

	IFXRESULT IFXAPI ExtractData(U32 uId, IFXUnknownPtr& spUnk);

	IFXRESULT IFXAPI ExtractData(U32 uId, IFXUnknown*& pUnk);

	IFXRESULT IFXAPI ExtractLowestId(U32& uId, IFXUnknownPtr& spUnk);

	IFXRESULT IFXAPI ExtractLowestId(U32& uId, IFXUnknown*& pUnk);

	IFXRESULT IFXAPI RemoveData(U32 uId);

protected:
	//=========================
	// CIFXSimpleHash Methods
	//=========================
	CIFXSimpleHash();
	virtual ~CIFXSimpleHash();

	virtual IFXRESULT IFXAPI Construct();
	
	CIFXSimpleHashData* IFXAPI FindData(U32 uId);
	U32 FindLowestId();

	U32 Hash(U32 uId);

	//=========================
	// CIFXSimpleHash Data
	//=========================
	U32 m_uTableSize;
	U32 m_uHashMask;
	CIFXSimpleHashData* m_pTable;

	IFXRESULT m_rcInitialized;
};

class CIFXSimpleHashData
{
public:
	friend class CIFXSimpleHash;

private:
	CIFXSimpleHashData();
	~CIFXSimpleHashData();

	void IFXAPI RemoveFromTable();

	IFXUnknownPtr	m_pUnk;
	U32				m_uId;

	CIFXSimpleHashData* m_pNext;
	CIFXSimpleHashData* m_pPrev;
};

//============================
// CIFXSimpleHashData Methods
//============================
IFXINLINE CIFXSimpleHashData::CIFXSimpleHashData()
{
	m_uId = 0;
	m_pNext = 0;
	m_pPrev = 0;
}

IFXINLINE CIFXSimpleHashData::~CIFXSimpleHashData()
{
	IFXDELETE(m_pNext);
}

//=========================
// CIFXSimpleHash Methods
//=========================
IFXINLINE U32 CIFXSimpleHash::Hash(U32 uId)
{
	return (uId & m_uHashMask);
}

#endif
