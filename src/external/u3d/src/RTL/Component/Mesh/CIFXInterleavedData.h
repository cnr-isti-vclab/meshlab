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
// CIFXInterleavedData.h
#ifndef CIFX_INTERLEAVED_DATA_H
#define CIFX_INTERLEAVED_DATA_H

#include "IFXMesh.h"
#include "IFXIDManager.h"

class CIFXInterleavedData : virtual public IFXInterleavedData, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXInterleavedDataFactory(IFXREFIID intId, void** ppUnk);

	//============================
	// IFXInterleavedData Methods
	//============================
	virtual IFXRESULT IFXAPI Allocate(	U32 uNumVectors, 
								U32* puVectorSizes, 
								U32 uNumVertices);
	
	virtual IFXRESULT IFXAPI GetVectorIter(U32 uVectorNum, IFXIterator& iter);
	virtual IFXRESULT IFXAPI GetVertexIter(IFXIterator& iter);
	
	virtual U32 IFXAPI GetVersionWord(U32 uVectorNum);
	virtual U32 IFXAPI UpdateVersionWord(U32 uVectorNum);

	// WARNING!  This is dangerous to do without knowing 
	// what format the data is in!
	virtual U8* IFXAPI GetDataPtr();
	
	virtual U32 IFXAPI GetNumVertices() const;
	// Get array info to parse the data directly
	virtual IFXRESULT IFXAPI GetInfo(	U32& uNumVectors, 
								U32* puVectorSizes, 
								U32& uNumVertices);

	U32	IFXAPI GetStride() const;

	IFXRESULT IFXAPI CopyData(	IFXInterleavedData &rSrcData, 
								U32 uStartVertex, U32 uNumVertices);

	U32 IFXAPI GetId() const { return m_uId; }

	U32 IFXAPI GetTimeStamp() const;
	U32 IFXAPI SetTimeStamp(U32 uTimeStamp);

	static IFXRESULT IFXAPI Shutdown();
	
protected:
	CIFXInterleavedData();
	virtual ~CIFXInterleavedData();
	
	virtual IFXRESULT IFXAPI Construct();
	virtual IFXRESULT IFXAPI Destroy();
	virtual U32 IFXAPI GetOffset(U32 uVectorNum);

	U8* m_pBaseData;
	U8*	m_pData;
	U32	m_uNumVectors;
	U32* m_puVectorSizes;
	U32	m_uNumVertices;
	U32	m_uDataSize;
	U32 m_uVertexSize;

	U32*	m_puVersionWord;
	U32 m_uId;

	U32 m_uTimeStamp;

	static IFXIDManagerPtr ms_spIDManager;
};
typedef IFXSmartPtr<CIFXInterleavedData> CIFXInterleavedDataPtr;

#define IFX_IDM_NUM_VERSION_BITS	10
#define IFX_IDM_VERSION_MASK		((1 << IFX_IDM_NUM_VERSION_BITS)-1)


#endif // CIFX_INTERLEAVED_DATA_H

