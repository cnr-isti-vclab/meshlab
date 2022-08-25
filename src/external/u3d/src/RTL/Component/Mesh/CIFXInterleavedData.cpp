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
// CIFXInterleavedData.cpp

#include <stdlib.h>
#include <memory.h>
#include "IFXMesh.h"
#include "CIFXInterleavedData.h"
#include "IFXCoreCIDs.h"
#include "IFXMemory.h"

// I have yet to find any cases where this is worth the extra
// 33% in memory cost.
//#define IFX_PAD_IFXVECTOR3

IFXIDManagerPtr CIFXInterleavedData::ms_spIDManager;

//=============================
// Factory Function
//=============================
IFXRESULT IFXAPI_CALLTYPE CIFXInterleavedDataFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXInterleavedData* pPtr = new CIFXInterleavedData;
		if (pPtr)
		{
			rc = pPtr->Construct();
			if (IFXFAILURE(rc))
				IFXDELETE(pPtr);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if (IFXSUCCESS(rc))
		{
			pPtr->AddRef();
			rc = pPtr->QueryInterface(intId, ppUnk);
			pPtr->Release();
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}
	return rc;
}

U32 CIFXInterleavedData::AddRef()
{
	return ++m_refCount;
}

U32 CIFXInterleavedData::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXInterleavedData::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXInterleavedData)
		{
			*(IFXInterleavedData**)ppInterface = (IFXInterleavedData*) this;
		}
		else
			if (interfaceId == IID_IFXUnknown)
			{
				*(IFXUnknown**)ppInterface = (IFXUnknown*) this;
			}
			else
			{
				*ppInterface = NULL;
				result = IFX_E_UNSUPPORTED;
			}
			if (IFXSUCCESS(result))
				AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}
	return result;
}

//================================
// Static Methods
//================================
IFXRESULT IFXAPI CIFXInterleavedData::Shutdown()
{
	if(ms_spIDManager.IsValid())
	{
		IFXTRACE_GENERIC(L"\nError: IFXInterleavedData Instances Still Allocated!\n");
		ms_spIDManager->OutputAllocatedIds();
		ms_spIDManager = 0;
	}

	return IFX_OK;
}

//================================
// Public Methods
//================================
CIFXInterleavedData::CIFXInterleavedData()
{
	m_pBaseData = NULL;
	m_pData = NULL;
	m_puVectorSizes = NULL;
	m_puVersionWord = NULL;
	m_uNumVectors = 0;
	m_uNumVertices = 0;
	m_uDataSize = 0;
	m_uVertexSize = 0;
	m_refCount = 0;
}

CIFXInterleavedData::~CIFXInterleavedData()
{
	Destroy();

	if(ms_spIDManager.IsValid())
		ms_spIDManager->ReleaseId(m_uId);
	ms_spIDManager.DecRef();
}

IFXRESULT CIFXInterleavedData::Allocate(U32 uNumVectors,
										U32* puVectorSizes,
										U32 uNumVertices)
{
	IFXRESULT rVal = IFX_OK;

	IFXASSERTBOX(0 != puVectorSizes, "Invalid vector size array!");
	if(0 == puVectorSizes)
	{
		rVal = IFX_E_INVALID_POINTER;
	}

	U32 uVertexSize = 0;
	U32 uDataSize = 0;

	if(IFXSUCCESS(rVal))
	{
		U32 i;
		for( i = 0; i < uNumVectors; i++)
		{
			uVertexSize += puVectorSizes[i];
		}

		// This is a special optimization for 3 component vectors.
		// This will allow us to pad 3 component vectors to 4 components.
#ifdef IFX_PAD_IFXVECTOR3
		if(uNumVectors == 1 && uVertexSize == 12)
		{
			uVertexSize = 16;
		}
#endif

		uDataSize = (uVertexSize * uNumVertices) + IFX_CACHE_BLOCK_SIZE;

		if(uDataSize > m_uDataSize)
		{
			U8* pData = m_pBaseData;
			m_pBaseData = 0;
			rVal = Destroy();

			if(IFXSUCCESS(rVal))
			{
				m_pBaseData = (U8*)IFXReallocate(pData, uDataSize);
			}

			IFXASSERTBOX(0 != m_pBaseData, "Could not allocate memory for interleaved data");
			if(NULL != m_pBaseData)
				memset( m_pBaseData, 0, uDataSize );
			else
			{
				rVal = IFX_E_OUT_OF_MEMORY;
			}
		}
	}

	if(IFXSUCCESS(rVal))
	{
		m_uDataSize = uDataSize;
		m_uVertexSize = uVertexSize;
		IFXDELETE_ARRAY(m_puVectorSizes);

		m_puVectorSizes = new U32[uNumVectors];

		IFXASSERTBOX(0 != m_puVectorSizes, "Could not allocate memory for vector size array");
		if(0 == m_puVectorSizes)
		{
			rVal = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(rVal))
	{
		m_uTimeStamp = 0;
		IFXDELETE_ARRAY(m_puVersionWord);
		m_puVersionWord = new U32[uNumVectors];

		IFXASSERTBOX(0 != m_puVersionWord, "Could not allocate memory for version word array");
		if(0 == m_puVersionWord)
		{
			rVal = IFX_E_OUT_OF_MEMORY;
		}
		else
		{
			U32 i;
			for( i = 0; i < uNumVectors; i++)
			{
				m_puVersionWord[i] = rand();
			}
		}
	}

	if(IFXSUCCESS(rVal))
	{
		m_uNumVectors = uNumVectors;
		m_uNumVertices = uNumVertices;
		U32 i;
		for( i = 0; i < m_uNumVectors; i++)
		{
			m_puVectorSizes[i] = puVectorSizes[i];
		}

		// Align data to 32 byte boundary
		// m_pData = (m_pBaseData & 0xffffffe0) + 0x00000020
		UPTR uBaseData = (UPTR)m_pBaseData;
		m_pData = (U8*)((uBaseData & ~(UPTR)(IFX_CACHE_BLOCK_SIZE - 1)) + IFX_CACHE_BLOCK_SIZE);
	}

	if(IFXFAILURE(rVal))
	{
		Destroy();
	}


	return rVal;
}

IFXRESULT CIFXInterleavedData::GetVectorIter(U32 uVectorNum, IFXIterator& iter)
{
	IFXRESULT rVal = IFX_OK;

	IFXASSERTBOX(uVectorNum < m_uNumVectors, "Invalid vector iterator request!");
	if(uVectorNum >= m_uNumVectors)
	{
		rVal = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rVal))
	{
		IFXASSERTBOX(iter.GetDataSize() <= m_puVectorSizes[uVectorNum],
			"Iterator data size is bigger than actual data size!");
		if(iter.GetDataSize() > m_puVectorSizes[uVectorNum])
		{
			rVal = IFX_E_INVALID_RANGE;
		}
	}

	if(IFXSUCCESS(rVal))
	{
		U32 uOffset = GetOffset(uVectorNum);
		iter.SetData(m_pData + uOffset);
		iter.SetStride(m_uVertexSize);
	}

	return rVal;
}

IFXRESULT CIFXInterleavedData::GetVertexIter(IFXIterator& iter)
{
	IFXRESULT rVal = IFX_OK;

	IFXASSERTBOX(iter.GetDataSize() <= m_uVertexSize,
		"Iterator data size is bigger than actual data size!");
	if(iter.GetDataSize() > m_uVertexSize)
	{
		rVal = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rVal))
	{
		iter.SetData(m_pData);
		iter.SetStride(m_uVertexSize);
	}

	return rVal;
}

U32 CIFXInterleavedData::GetVersionWord(U32 uVectorNum)
{
	IFXASSERTBOX((m_uNumVectors == 0) || (uVectorNum < m_uNumVectors), "Invalid Version Word request!");
	if(uVectorNum < m_uNumVectors)
	{
		return (m_uId << IFX_IDM_NUM_VERSION_BITS) |
			(m_puVersionWord[uVectorNum] & IFX_IDM_VERSION_MASK);
	}

	return 0;
}

U32 CIFXInterleavedData::UpdateVersionWord(U32 uVectorNum)
{
	m_uTimeStamp = 0;

	IFXASSERTBOX(uVectorNum < m_uNumVectors, "Invalid Version Word request!");
	if(uVectorNum < m_uNumVectors)
	{
		m_puVersionWord[uVectorNum]++;
		return (m_uId << IFX_IDM_NUM_VERSION_BITS) |
			(m_puVersionWord[uVectorNum] & IFX_IDM_VERSION_MASK);
	}

	return 0;
}

U8* CIFXInterleavedData::GetDataPtr()
{
	return m_pData;
}

IFXRESULT CIFXInterleavedData::GetInfo(U32& uNumVectors,
									   U32* puVectorSizes,
									   U32& uNumVertices)
{
	IFXRESULT rVal = IFX_OK;

	uNumVectors = m_uNumVectors;
	uNumVertices = m_uNumVertices;

	if(puVectorSizes)
	{
		U32 i;
		for( i = 0; i < m_uNumVectors; i++)
		{
			puVectorSizes[i] = m_puVectorSizes[i];
		}
	}

	return rVal;
}

U32 CIFXInterleavedData::GetNumVertices() const
{
	return m_uNumVertices;
}

U32 CIFXInterleavedData::GetStride() const
{
	return m_uVertexSize;
}

IFXRESULT CIFXInterleavedData::CopyData(IFXInterleavedData &rSrcData,
										U32 uStartVertex, U32 uNumVertices)
{
	IFXRESULT rc = IFX_OK;

	/// @todo: Do data size checking!!

	U8* pDst = GetDataPtr();
	U8* pSrc = rSrcData.GetDataPtr();

	U32 uOffset = m_uVertexSize * uStartVertex;

	pDst = &pDst[uOffset];
	pSrc = &pSrc[uOffset];

	U32 uSize = uNumVertices * m_uVertexSize;

	/// @todo: use IFX functions for memory operations
	memcpy(pDst, pSrc, uSize);

	U32 i;
	for( i = 0; i< m_uNumVectors; i++)
		UpdateVersionWord(i);

	return rc;
}

U32 CIFXInterleavedData::GetTimeStamp() const
{
	return m_uTimeStamp;
}

U32 CIFXInterleavedData::SetTimeStamp(U32 uTimeStamp)
{
	return m_uTimeStamp = uTimeStamp;
}

//===============================
// Protected Methods
//===============================
IFXRESULT CIFXInterleavedData::Construct()
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		if(ms_spIDManager.IsValid())
		{
			ms_spIDManager.IncRef();
		}
		else
		{
			rc = ms_spIDManager.Create(CID_IFXIDManager, IID_IFXIDManager);
		}
	}

	IFXRUN(rc, ms_spIDManager->GetId(m_uId));

	if(IFXSUCCESS(rc))
	{
		m_pBaseData = 0;
		m_pData = 0;
		m_uNumVectors = 0;
		m_uNumVertices = 0;
		m_puVectorSizes = 0;
		m_uDataSize = 0;
		m_uVertexSize = 0;
		m_puVersionWord = 0;
		m_uTimeStamp = 0;
	}

	return rc;
}

IFXRESULT CIFXInterleavedData::Destroy()
{
	if(m_pBaseData)
		IFXDeallocate(m_pBaseData);
	IFXDELETE_ARRAY(m_puVectorSizes);
	IFXDELETE_ARRAY(m_puVersionWord);

	m_pBaseData = 0;
	m_pData = 0;
	m_uNumVectors = 0;
	m_uNumVertices = 0;
	m_puVectorSizes = 0;
	m_uDataSize = 0;
	m_uVertexSize = 0;
	m_puVersionWord = 0;

	return IFX_OK;
}

U32 CIFXInterleavedData::GetOffset(U32 uVectorNum)
{
	IFXASSERTBOX(uVectorNum < m_uNumVectors, "Invalid vector offset request!");
	U32 uOffset = 0;
	U32 i;
	for( i = 0; i < uVectorNum; i++)
	{
		uOffset += m_puVectorSizes[i];
	}

	return uOffset;
}
