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
	@file IFXPackWeights.h 
*/

#ifndef IFXPACKWEIGHTS_H
#define IFXPACKWEIGHTS_H

#include <memory.h>
#include "IFXDataTypes.h"

/// @todo check this with icl 
#ifdef __ICL
	#include <xmmintrin.h>
#endif

#define IFXPW_CHECK_READ_OVERRUN    _DEBUG
#define IFXPW_CHECK_WRITE_OVERRUN   _DEBUG
#define IFXPW_ASSERT_ON_OVERRUN     FALSE   ///< ignored if not checking

#ifdef GMARKS
	#pragma message( "Error: Not expected to have macro GMARKS" )
	#pragma message( "defined here, see " __FILE__ )
#endif

// Enable bounds guard marks and checking
#ifdef _DEBUG
	#define GMARKS(code) code
#else
	#define GMARKS(code)
#endif

class IFXPackVertex
{
public:
	void    SetVertexIndex(U16 set) { m_vertexId = set; }
	U16     GetVertexIndex(void) const { return m_vertexId; }

	void    SetNumWeights(U8 set) { m_weights = set; }
	void    IncNumWeights(void) { ++m_weights; }
	U8      GetNumWeights(void) const { return m_weights; }

	F32 *Offset(void) { return m_offset; }
	F32 const *OffsetConst(void) const { return m_offset; }

	F32 *NormalOffset(void) { return m_normalOffset; }
	F32 const *NormalOffsetConst(void) const { return m_normalOffset; }

private:
	IFXGeomIndex m_vertexId;
	F32 m_offset[3];
	F32 m_normalOffset[3];
	U8 m_weights;
};

class IFXPackBoneWeight
{
public:
	void SetBoneIndex(U16 set)           { m_boneId = set; }
	U16 GetBoneIndex(void) const        { return m_boneId; }

	void SetBoneWeight(F32 set)          { m_weight = set; }
	F32 GetBoneWeight(void) const       { return m_weight; }

private:
	F32 m_weight;
	U16 m_boneId;
};

/**
	Used to match M x (vertex <-> N x pairs (bone, weight))
*/
class IFXPackWeights
{
public:
	IFXPackWeights(void)
	{
		m_pCurrentRead = NULL;
		m_pCurrentWrite = NULL;
		m_pBuffer = NULL;
		m_size = 0;
		m_verticesAllocated = 0;
		m_weightsAllocated = 0;
		m_verticesWritten = 0;
		m_wroteWeights = true;
		m_readWeights = true;
	}

	virtual ~IFXPackWeights(void)
	{
		if(m_pBuffer)
			IFXDeallocate(m_pBuffer);
	}

	IFXRESULT Allocate(U32 vertices, U32 weights)
	{
		IFXRESULT res = IFX_OK;

		if(m_pBuffer)
			IFXDeallocate(m_pBuffer);

		m_verticesAllocated = vertices;
		m_weightsAllocated = weights;
		m_size = vertices * sizeof(IFXPackVertex)
			+ weights * sizeof(IFXPackBoneWeight);

		GMARKS(m_size += (vertices + weights)); // space for markers

		m_pBuffer = (U8 *)IFXAllocate(m_size);
		if (!m_pBuffer) {
			res = IFX_E_OUT_OF_MEMORY;
		}

		RewindForRead();
		RewindForWrite();

		return res;
	}
	
	void operator=(IFXPackWeights const &rh)
	{
		IFXRESULT res = IFX_OK;

		res = Allocate(rh.m_verticesAllocated, rh.m_weightsAllocated);
		IFXASSERT(res);
		if(IFXSUCCESS(res))
			memcpy(m_pBuffer, rh.m_pBuffer, m_size);
	}

	U32 GetNumberWeightsAllocated(void) const
	{
		return m_weightsAllocated; 
	}

	U32 GetNumberVerticesConst(void) const
	{
		return m_verticesWritten; 
	}

	void RewindForRead(void)
	{
		m_pCurrentRead = m_pBuffer;
		m_readWeights = true;
	}

	void RewindForWrite(void)
	{
		m_pCurrentWrite = m_pBuffer;
		m_verticesWritten = 0;
		m_wroteWeights = true;
	}

#ifdef __ICL // Following functions are in use in IFXSkin_p3.cpp
		/**
			@note assuming we're on the vertex at the moment
		*/
		void PrefetchNextPackVertex() 
		{
			_mm_prefetch((char const *) m_pCurrentRead, _MM_HINT_T0);
		}

		/**
			@note assuming we're on the vertex at the moment
		*/
		IFXPackBoneWeight *PrefetchNextPackWeight() 
		{
			_mm_prefetch(((char const *) m_pCurrentRead)+sizeof(IFXPackVertex),
				_MM_HINT_T0);
			return (IFXPackBoneWeight*) (m_pCurrentRead +sizeof(IFXPackVertex));
		}
#endif // __ICL

	U32 GetReadOffset(void) const
	{
		return (U32)(m_pCurrentRead-m_pBuffer); 
	}

	void const *const GetBufferAddr(void) const
	{
		return m_pBuffer; 
	}

	/// Pass over weights block
	IFXRESULT skipWeights(U32 const wCount)
	{
		IFXRESULT rc = IFX_OK;

		U32 m;
		for(m = 0; (m < wCount) && IFXSUCCESS(rc); ++m)
		{
			IFXPackBoneWeight const *pw = NULL;

			pw = NextPackBoneWeightForRead();

			if(!pw)
				rc = IFX_E_READ_FAILED;
		}

		return rc;
	}

	IFXPackVertex const *NextPackVertexForRead(void)
	{
//      IFXTRACE_GENERIC(L"NextPackVertexForRead*() called\n");

		IFXASSERT(m_pCurrentRead);
		IFXASSERT(m_readWeights);

		#if IFXPW_CHECK_READ_OVERRUN
			if((U32)(m_pCurrentRead-m_pBuffer) >= m_size)
			{
				if(m_size && (m_pCurrentRead-m_pBuffer-m_size > 0))
				{
					IFXTRACE_GENERIC(L"NextPackVertex*() read overrun "
						L"%d+%d bytes\n",
						(U32)(m_pCurrentRead-m_pBuffer),
						(U32)(m_pCurrentRead-m_pBuffer-m_size));
				}

				#if IFXPW_ASSERT_ON_OVERRUN
					IFXASSERT(FALSE);
				#endif
				
				return NULL;
			}

			if(m_pCurrentRead >= m_pCurrentWrite)
			{
				if(m_size)
					IFXTRACE_GENERIC(L"NextPackVertex*() read"
					L" unwritten "
					L"data plus %d bytes\n",
					(U32)(m_pCurrentRead-m_pBuffer));
				
				#if IFXPW_ASSERT_ON_OVERRUN
					IFXASSERT(FALSE);
				#endif
	
				return NULL;
			}
		#endif //IFXPW_CHECK_READ_OVERRUN

		IFXPackVertex *pResult = NULL;

		if((U32)(m_pCurrentRead-m_pBuffer) < m_size)
		{
			GMARKS(IFXASSERT(IFXPackWeights::PVHeader_1== *m_pCurrentRead);)
			GMARKS(++m_pCurrentRead;)

			pResult = (IFXPackVertex *)m_pCurrentRead;
			m_pCurrentRead += sizeof(IFXPackVertex);

			m_readWeights = false;
		}

		return pResult;
	}

	IFXPackBoneWeight const *NextPackBoneWeightForRead(void)
	{
//      IFXTRACE_GENERIC(L"NextPackBoneWeightForRead*() called\n");

		IFXASSERT(m_pCurrentRead);
		
		GMARKS(IFXASSERT(IFXPackWeights::PBWHeader_2==*m_pCurrentRead);)
		GMARKS(++m_pCurrentRead;)

		#if IFXPW_CHECK_READ_OVERRUN
			if((U32)(m_pCurrentRead-m_pBuffer)>=m_size)
			{
				IFXTRACE_GENERIC(L"NextPackWeight*() read overrun "
								L"plus %d bytes\n",
								(U32)(m_pCurrentRead-m_pBuffer));
				#if IFXPW_ASSERT_ON_OVERRUN
					IFXASSERT(FALSE);
				#endif
				
				return NULL;
			}

			if(m_pCurrentRead >= m_pCurrentWrite)
			{
				IFXTRACE_GENERIC(L"NextPackWeight*() read unwritten "
				L"data plus %d bytes\n",
				(U32)(m_pCurrentRead-m_pBuffer));

				#if IFXPW_ASSERT_ON_OVERRUN
					IFXASSERT(FALSE);
				#endif
			
				return NULL;
			}
		#endif //IFXPW_CHECK_READ_OVERRUN

		IFXPackBoneWeight *pResult = NULL;

		if((U32)(m_pCurrentRead-m_pBuffer) < m_size)
		{
			pResult = (IFXPackBoneWeight *)m_pCurrentRead;
			m_pCurrentRead += sizeof(IFXPackBoneWeight);

			m_readWeights = true;
		}

		return pResult;
	}

	IFXPackVertex *NextPackVertexForWrite(void)
	{
//      IFXTRACE_GENERIC(L"NextPackVertexForWrite*() called\n");

		IFXASSERT(m_pCurrentWrite);
		IFXASSERT(m_wroteWeights);
		
		GMARKS( *(m_pCurrentWrite++) = IFXPackWeights::PVHeader_1; )

		
		#if IFXPW_CHECK_READ_OVERRUN
			if((U32)(m_pCurrentWrite-m_pBuffer)>=m_size)
			{
				IFXTRACE_GENERIC(L"NextPackVertex*() write overrun "
				L"plus %d bytes\n", (U32)(m_pCurrentRead-m_pBuffer));
				
				#if IFXPW_ASSERT_ON_OVERRUN
					IFXASSERT(FALSE);
				#endif

				return NULL;
			} //if
		#endif //IFXPW_CHECK_READ_OVERRUN
							
		IFXPackVertex *pResult = (IFXPackVertex *)m_pCurrentWrite;
		m_pCurrentWrite += sizeof(IFXPackVertex);
		++m_verticesWritten;

		m_wroteWeights = false;

		return pResult;
	}

	IFXPackBoneWeight *NextPackBoneWeightForWrite(void)
	{
//      IFXTRACE_GENERIC(L"NextPackBoneWeightForWrite*() called\n");

		IFXASSERT(m_pCurrentWrite);

		GMARKS( *m_pCurrentWrite = IFXPackWeights::PBWHeader_2; )
		GMARKS(++m_pCurrentWrite;)

		#if IFXPW_CHECK_READ_OVERRUN
			if((U32)(m_pCurrentWrite-m_pBuffer) >= m_size)
			{
				IFXTRACE_GENERIC(L"NextPackWeight*() write overrun "
				L"plus %d bytes\n",
				(U32)(m_pCurrentRead-m_pBuffer));

				#if IFXPW_ASSERT_ON_OVERRUN
					IFXASSERT(FALSE);
				#endif
				
				return NULL;
			}
		#endif //IFXPW_CHECK_READ_OVERRUN

		IFXPackBoneWeight *pResult= (IFXPackBoneWeight *)m_pCurrentWrite;
		
		m_pCurrentWrite += sizeof(IFXPackBoneWeight);
		m_wroteWeights = true;

		return pResult;
	}

private:
	U8 *m_pCurrentRead;///< Current read position marker (where to read ptr)
	U8 *m_pCurrentWrite;///< Current write position marker (were to write ptr)
	U8 *m_pBuffer;  /**< Memory block has following format: 
						 IFXPackVertex, then releated N*IFXPackBoneWeight,
						 next IFXPackVertex, ...
					*/

	U32 m_size; ///< Buffer size (*m_pBuffer) 
	U32 m_verticesAllocated;
	U32 m_weightsAllocated;
	U32 m_verticesWritten;
	bool m_wroteWeights; /**< Simple IFXPackVertex/IFXPackBoneWeight write order
						 checking in m_pBuffer: should be true before
						 NextPackVertexForWrite */
	bool m_readWeights; /**< Simple IFXPackVertex/IFXPackBoneWeight read order
						checking in m_pBuffer: should be true before
						NextPackVertexForRead */

	GMARKS(U8 static const PVHeader_1 = 1;)
	GMARKS(U8 static const PBWHeader_2 = 2;)
};

#undef GMARKS

#endif
