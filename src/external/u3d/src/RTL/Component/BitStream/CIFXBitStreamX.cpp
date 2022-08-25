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
	@file	CIFXBitStreamX.cpp

			Implementation of CIFXBitStreamX class.
			CIFXBitStreamX is used to read and write basic types to a buffer for 
			purposes of bitstream formatting.
*/

#include "CIFXBitStreamX.h"
#include "IFXACContext.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXCOM.h"

#include <memory.h>

const U32 CIFXBitStreamX::m_uDataSizeInitial = 0x00000010;
const U32 CIFXBitStreamX::m_uDataSizeIncrement = 0x000023F8;

const U32 CIFXBitStreamX::m_uACHalf = 0x00008000;
const U32 CIFXBitStreamX::m_uACQuarter= 0x00004000;

const U32 CIFXBitStreamX::m_uACContext8 = 0;

const U32 CIFXBitStreamX::m_puSwap8[16] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
const U32 CIFXBitStreamX::m_puReadCount[16] = {4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
const U32 CIFXBitStreamX::m_puFastNotMask[5] = {0xFFFFFFFF, 0x7FFF7FFF, 0x3FFF3FFF, 0x1FFF1FFF, 0x0FFF0FFF};

const U32 CIFXBitStreamX::m_uHalfMask = 0x80008000;
const U32 CIFXBitStreamX::m_uNotHalfMask = 0x7FFF7FFF;
const U32 CIFXBitStreamX::m_uQuarterMask = 0x40004000;
const U32 CIFXBitStreamX::m_uNotThreeQuarterMask = 0x3FFF3FFF;

// IFXUnknown
U32 CIFXBitStreamX::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXBitStreamX::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXBitStreamX::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;
	if(ppInterface) {
		if (IID_IFXUnknown == interfaceId) {
			*ppInterface = (IFXUnknown*) this;
			this->AddRef();
		} else if (IID_IFXBitStreamX == interfaceId) {
			*ppInterface = (IFXBitStreamX*) this;
			this->AddRef();
		} else if (IID_IFXBitStreamCompressedX == interfaceId) {
			*ppInterface = (IFXBitStreamCompressedX*) this;
			this->AddRef();
		} else {
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

// IFXBitStreamX

// AlignToByteX:  Set the current position to the next byte boundary if needed
void CIFXBitStreamX::AlignToByteX()
{
	//IFXRESULT rc = IFX_OK;

	// Check input(s)
	CIFXBitStreamX::FastValidateInput();

	U32 uBitCount = 0;

	CIFXBitStreamX::GetBitCount(uBitCount);
	uBitCount = (8 - (uBitCount & 7)) & 7;
	m_uDataBitOffset += uBitCount;

	if(m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPosition();
	}
}

// AlignTo4Byte:  Set the current position to the next 4 byte boundary if needed
void CIFXBitStreamX::AlignTo4ByteX()
{
	if(m_uDataBitOffset > 0) {
		m_uDataBitOffset = 0;
		CIFXBitStreamX::IncrementPosition();
	}
}

// Write U8:
void CIFXBitStreamX::WriteU8X(U8 Value)
{
	// Check that local data buffer is allocated
	CIFXBitStreamX::FastValidateInput();

	U32 uSymbol = (U32) Value;
	CIFXBitStreamX::SwapBits8(uSymbol);

	BOOL bEscape = FALSE;
	CIFXBitStreamX::WriteSymbol(m_uACContext8,uSymbol,bEscape);
}

// Write U16:
void CIFXBitStreamX::WriteU16X(U16 Value)
{
	CIFXBitStreamX::WriteU8X((U8) (0x00ffu & Value));
	CIFXBitStreamX::WriteU8X((U8) (0x00ffu & (Value >> 8)));
}

// Write U32:
void CIFXBitStreamX::WriteU32X(U32 Value)
{
	CIFXBitStreamX::WriteU16X((U16) (0x0000ffffu & Value));
	CIFXBitStreamX::WriteU16X((U16) (0x0000ffffu & (Value >> 16)));
}

// Write U64:
void CIFXBitStreamX::WriteU64X(U64 Value)
{
	CIFXBitStreamX::WriteU32X((U32) (0x00000000ffffffffu & Value));
	CIFXBitStreamX::WriteU32X((U32) (0x00000000ffffffffu & (Value >> 32)));
}

// Write I32:
void CIFXBitStreamX::WriteI32X(I32 Value)
{
	CIFXBitStreamX::WriteU32X((U32) Value);
}

// Write F32:
void CIFXBitStreamX::WriteF32X(F32 Value)
{
	CIFXBitStreamX::WriteU32X(* (U32*) &Value);
}

// Write F64:
void CIFXBitStreamX::WriteF64X(F64 Value)
{
	CIFXBitStreamX::WriteU64X(* (U64*) &Value);
}

void CIFXBitStreamX::WriteIFXStringX(IFXString& rString)
{
	U8* pValue = NULL;

	try 
	{
		U32 len = 0;
		IFXCHECKX( rString.GetLengthU8( &len ) );
		CIFXBitStreamX::WriteU16X((U16)len);

		if( len > 0 )
		{
			pValue = new U8[len+1]; // + null terminator for conversion
			IFXCHECKX_RESULT( pValue != NULL, IFX_E_OUT_OF_MEMORY );

			IFXCHECKX( rString.ConvertToRawU8( pValue, len+1 ) );

			U32 i;
			for (i = 0; i < len; ++i)
				CIFXBitStreamX::WriteU8X(pValue[i]);

			IFXDELETE_ARRAY(pValue);
		}
	}
	catch(IFXException e)
	{
		IFXDELETE_ARRAY(pValue);
		throw e;
	}
}

// Read U8:
void CIFXBitStreamX::ReadU8X(U8& rValue)
{
	// Read the data and assign it to the output
	U32 uSymbol = 0;
	CIFXBitStreamX::ReadSymbolContext8(uSymbol);
	rValue = (U8) uSymbol;
}

// Read U16:
void CIFXBitStreamX::ReadU16X(U16& rValue)
{
	// Read the data and assign it to the output
	U8 u8Low = 0, u8High = 0;
	CIFXBitStreamX::ReadU8X(u8Low);

	CIFXBitStreamX::ReadU8X(u8High);

	rValue = ((U16) u8Low) | (((U16) u8High) << 8);
}

// Read U32:
void CIFXBitStreamX::ReadU32X(U32& rValue)
{
	// Read the data and assign it to the output
	U16 u16Low = 0, u16High = 0;
	CIFXBitStreamX::ReadU16X(u16Low);

	CIFXBitStreamX::ReadU16X(u16High);

	rValue = ((U32) u16Low) | (((U32) u16High) << 16);
}

// Read U64:
void CIFXBitStreamX::ReadU64X(U64& rValue)
{
	// Read the data and assign it to the output
	U32 u32Low = 0, u32High = 0;
	CIFXBitStreamX::ReadU32X(u32Low);

	CIFXBitStreamX::ReadU32X(u32High);

	rValue = ((U64) u32Low) | (((U64) u32High) << 32);
}

// Read I32:
void CIFXBitStreamX::ReadI32X(I32& rValue)
{
	U32 uValue = 0;

	// Read the data and assign it to the output
	CIFXBitStreamX::ReadU32X(uValue);
	rValue = (I32) uValue;
}

// Read F32:
void CIFXBitStreamX::ReadF32X(F32& rValue)
{
	U32 uValue = 0;

	// Read the data and assign it to the output
	CIFXBitStreamX::ReadU32X(uValue);
	rValue = * (F32*) &uValue;
}

// Read F64:
void CIFXBitStreamX::ReadF64X(F64& rValue)
{
	U64 uValue = 0;

	// Read the data and assign it to the output
	CIFXBitStreamX::ReadU64X(uValue);
	rValue = * (F64*) &uValue;
}

void CIFXBitStreamX::ReadIFXStringX(IFXString& rString)
{
	U16 length = 0;
	U8* pValue = NULL;

	try 
	{
		CIFXBitStreamX::ReadU16X(length);

		pValue = new U8[length+1]; // + null terminator
		IFXCHECKX_RESULT(pValue != NULL,IFX_E_OUT_OF_MEMORY);

		U32 i;
		for (i = 0; i < length; ++i)
			CIFXBitStreamX::ReadU8X((U8&)pValue[i]);

		pValue[length] = '\0'; // Null terminator to mark end of string

		IFXCHECKX(rString.Assign( pValue ));

		IFXDELETE_ARRAY(pValue);
	}
	catch(IFXException e) 
	{
		IFXDELETE_ARRAY(pValue);
		throw e;
	}
}

// GetDataBlock:  Get data block containing bitstream data
void CIFXBitStreamX::GetDataBlockX(IFXDataBlockX*& rpBlock)
{
	U32 size = 0;

	// Check input values
	CIFXBitStreamX::FastValidateInput();

	if (m_uCompressed) {
		WriteU32X(0);
		m_uCompressed = FALSE;
	}

	// Create a data block
	IFXCHECKX(IFXCreateComponent(CID_IFXDataBlockX, IID_IFXDataBlockX, (void**)&rpBlock));

	// Get the number of bytes in the bitstream
	CIFXBitStreamX::AlignToByteX();

	CIFXBitStreamX::GetBitCount(size);
	size >>= 3;  // convert number of bits to number of bytes

	// Copy the bytes from the bitstream to the data block
	(rpBlock)->SetSizeX(size);

	U8* pData = NULL;

	rpBlock->GetPointerX(pData);

	CIFXBitStreamX::PutLocal();

	memcpy(pData,m_puData,size);
}

// SetDataBlock:  Set bitstream to use the data in a data block
void CIFXBitStreamX::SetDataBlockX(IFXDataBlockX& rBlock)
{
	U32 size = 0;

	// Get size of data block
	rBlock.GetSizeX(size);

	// Allocate local data buffer
	if (size > 0)
	{
		CIFXBitStreamX::AllocateDataBuffer(((size+3)>>2)+5);
	}

	// Copy from data block to local data buffer
	U8 *pSrc = NULL;
	rBlock.GetPointerX(pSrc);
	memcpy(m_puData,pSrc,size);

	// Move cursor to start of bitstream
	m_uDataPosition = 0;
	m_uDataBitOffset = 0;
	CIFXBitStreamX::GetLocal();
}

// IFXBitStreamCompresedX
// Bit writing methods

// WriteCompressedU32X
void CIFXBitStreamX::WriteCompressedU32X(U32 Context, U32 Value)
{
	if (m_bNoCompression)
	{
		WriteU32X(Value);
	}
	else
	{
		BOOL bEscape = FALSE;
		m_uCompressed = TRUE;

		if(Context && Context < uACMaxRange)
		{
			CIFXBitStreamX::WriteSymbol(Context,Value+1,bEscape);

			if(bEscape)
			{
				CIFXBitStreamX::WriteU32X(Value);

				if(m_uACContext8 == Context || uACStaticFull < Context)
				{
					//((CHistogramStaticFull*)pContext)->CHistogramStaticFull::AddSymbol(Value+1);
				}

				else
				{
					IFXHistogramDynamic *pContext = NULL;
					CIFXBitStreamX::GetContext(Context,pContext);
					pContext->AddSymbol(Value+1);
				}
			}
		}
		else
		{
			WriteU32X(Value);
		}
	}
}

// WriteCompressedU16X
void CIFXBitStreamX::WriteCompressedU16X(U32 Context, U16 Value)
{
	if (m_bNoCompression) 
	{
		WriteU16X(Value);
	}
	else
	{
		BOOL bEscape = FALSE;
		m_uCompressed = TRUE;

		if(Context && Context < uACMaxRange)
		{
			CIFXBitStreamX::WriteSymbol(Context,(U32)Value+1,bEscape);

			if(bEscape)
			{
				CIFXBitStreamX::WriteU16X(Value);

				if(m_uACContext8 == Context || uACStaticFull < Context)
				{
					//((CHistogramStaticFull*)pContext)->CHistogramStaticFull::AddSymbol(Value+1);
				}

				else
				{
					IFXHistogramDynamic *pContext = NULL;
					CIFXBitStreamX::GetContext(Context,pContext);
					pContext->AddSymbol(Value+1);
				}
			}
		}
		else
		{
			WriteU16X(Value);
		}
	}
}

// WriteCompressedU8X
void CIFXBitStreamX::WriteCompressedU8X(U32 Context, U8 Value)
{
	if (m_bNoCompression) 
	{
		WriteU8X(Value);
	}
	else
	{
		BOOL bEscape = FALSE;
		m_uCompressed = TRUE;

		if(Context && Context < uACMaxRange)
		{
			CIFXBitStreamX::WriteSymbol(Context,(U32)Value+1,bEscape);

			if(bEscape)
			{
				CIFXBitStreamX::WriteU8X(Value);

				if(m_uACContext8 == Context || uACStaticFull < Context)
				{
					//((CHistogramStaticFull*)pContext)->CHistogramStaticFull::AddSymbol((U32)Value+1);
				}

				else
				{
					IFXHistogramDynamic *pContext = NULL;
					CIFXBitStreamX::GetContext(Context,pContext);
					pContext->AddSymbol((U32)Value+1);
				}
			}
		}
		else
		{
			WriteU8X(Value);
		}
	}
}

// Bit reading methods

// ReadCompressedU32X
void CIFXBitStreamX::ReadCompressedU32X(U32 Context, U32& rValue)
{
	if (m_bNoCompression) 
	{
		ReadU32X(rValue);
	}
	else
	{
		if(Context && Context < uACMaxRange)
		{
			CIFXBitStreamX::ReadSymbol(Context,rValue);
			U32 uValue = rValue;
			if(0 != uValue)
			{
				rValue = uValue - 1;
			}

			else
			{
				CIFXBitStreamX::ReadU32X(rValue);
				if(uACStaticFull < Context)
				{
					//((CHistogramStaticFull*)pContext)->CHistogramStaticFull::AddSymbol((*pValue)+1);
				}

				else
				{
					IFXHistogramDynamic *pContext = NULL;
					CIFXBitStreamX::GetContext(Context,pContext);
					pContext->AddSymbol(rValue+1);
				}
			}
		}
		else
		{
			ReadU32X(rValue);
		}
	}
}

// ReadCompressedU16
void CIFXBitStreamX::ReadCompressedU16X(U32 Context, U16& rValue)
{
	if (m_bNoCompression) 
	{
		ReadU16X(rValue);
	}
	else
	{
		U32 uSymbol = 0;

		if(Context && Context < uACMaxRange)
		{
			CIFXBitStreamX::ReadSymbol(Context,uSymbol);

			//U16 uValue = (U16) uSymbol;
			if(0 != uSymbol)
			{
				rValue = uSymbol - 1;
			}

			else
			{
				CIFXBitStreamX::ReadU16X(rValue);
				if(uACStaticFull < Context)
				{
					//((CHistogramStaticFull*)pContext)->CHistogramStaticFull::AddSymbol((U32)(*pValue)+1);
				}

				else
				{
					IFXHistogramDynamic *pContext = NULL;
					CIFXBitStreamX::GetContext(Context,pContext);
					pContext->AddSymbol((U32)(rValue)+1);
				}
			}
		}
		else
		{
			ReadU16X(rValue);
		}
	}
}

// ReadCompressedU8X
void CIFXBitStreamX::ReadCompressedU8X(U32 Context, U8& rValue)
{
	if (m_bNoCompression) 
	{
		ReadU8X(rValue);
	}
	else
	{
		U32 uSymbol = 0;

		if(Context && Context < uACMaxRange)
		{
			CIFXBitStreamX::ReadSymbol(Context,uSymbol);
			if(0 != uSymbol)
			{
				rValue = (U8) (uSymbol - 1);
			}

			else
			{
				CIFXBitStreamX::ReadU8X(rValue);
				if(uACStaticFull < Context)
				{
					//((CHistogramStaticFull*)pContext)->CHistogramStaticFull::AddSymbol((U32)(*pValue)+1);
				}

				else
				{
					IFXHistogramDynamic *pContext = NULL;
					CIFXBitStreamX::GetContext(Context,pContext);
					pContext->AddSymbol((U32)(rValue)+1);
				}
			}
		}
		else
		{
			CIFXBitStreamX::ReadU8X(rValue);
		}
	}
}

void CIFXBitStreamX::SetNoCompressionMode(BOOL isNoCompression)
{
	m_bNoCompression = isNoCompression;
}

void CIFXBitStreamX::SetElephantX(U32 uElephant)
{
	m_uElephant = uElephant;
}


//***************************************************************************
//  Private methods
//***************************************************************************

//---------------------------------------------------------------------------
//  CIFXBitStreamX::CIFXBitStreamX
//
//  This is the constructor.  It sets up the object's initial state.
//---------------------------------------------------------------------------
CIFXBitStreamX::CIFXBitStreamX( )
{
	m_uRefCount = 0;
	m_puData = NULL;
	m_uDataSize = 0;
	m_uDataPosition = 0;
	m_uDataLocal = 0;
	m_uDataLocalNext = 0;
	m_uDataBitOffset = 0;
	m_bNoCompression = FALSE;

	m_uACHigh = 0x0000ffff;
	m_uACCode = 0;
	m_uACLow = 0;
	m_uACUnderflow = 0;
	m_ppACHistogram = NULL;
	m_uACContextCount = 0;
	m_uElephant = 0x00001FFF;
	m_uCompressed = 0;

#ifdef BSSTAT
	memset(d_puStats,0,16*sizeof(U32));
#endif
}

//---------------------------------------------------------------------------
//  CIFXBitStreamX::~CIFXBitStreamX
//
//  This is the destructor.  It performs any necessary cleanup activities.
//---------------------------------------------------------------------------
CIFXBitStreamX::~CIFXBitStreamX()
{
	/// @todo: possible memory leak issues here....

#ifdef BSSTAT
	{
		FILE *fp = NULL;
		fp = fopen("bsStatlog.txt","a+");
		U32 ud;
		fprintf(fp,"S:\t");
		for(ud=0;ud<16;ud++) {
			fprintf(fp,"%10d ",d_puStats[ud]);
		}
		fprintf(fp,"\n");
		fclose(fp);
	}
#endif

	IFXDELETE_ARRAY(m_puData);

	U32 i;

	if(NULL != m_ppACHistogram)
	{
		for(i=0; i < m_uACContextCount; i++)
		{
			if(NULL != m_ppACHistogram[i])
			{
				if(m_uACContext8 == i || uACStaticFull < i)
				{
					//delete ((CHistogramStaticFull*)m_ppACHistogram[i]);
				}

				else
				{
					delete ((IFXHistogramDynamic*)m_ppACHistogram[i]);
				}

				m_ppACHistogram[i] = NULL;
			}
		}

		delete [] m_ppACHistogram;
		m_ppACHistogram = NULL;
	}
}

// SeekToBitX:  Set the current read/write position
void CIFXBitStreamX::SeekToBit(U32 position)
{
	CIFXBitStreamX::PutLocal();
	m_uDataPosition = position >> 5;
	m_uDataBitOffset = position & 0x0000001f;
	CIFXBitStreamX::GetLocal();
}

// SeekToBit ReadOnly:  Set the current read/write position
void CIFXBitStreamX::SeekToBitReadOnly(U32 position)
{
	m_uDataPosition = position >> 5;
	m_uDataBitOffset = position & 0x0000001f;
	CIFXBitStreamX::GetLocalNoCheck();
}

// GetBitCount:  Get the number of bits read/written
void CIFXBitStreamX::GetBitCount(U32& rCount)
{
	rCount = (m_uDataPosition << 5) + m_uDataBitOffset;
}

// Get locally cached values from data buffer
void CIFXBitStreamX::GetLocal()
{
	CIFXBitStreamX::CheckPosition();
	m_uDataLocal = m_puData[m_uDataPosition];
	m_uDataLocalNext = m_puData[m_uDataPosition+1];
	SWAP32_IF_BIGENDIAN(m_uDataLocal);
	SWAP32_IF_BIGENDIAN(m_uDataLocalNext);
}

// Get locally cached values from data buffer
void CIFXBitStreamX::GetLocalNoCheck()
{
	m_uDataLocal = m_puData[m_uDataPosition];
	m_uDataLocalNext = m_puData[m_uDataPosition+1];
	SWAP32_IF_BIGENDIAN(m_uDataLocal);
	SWAP32_IF_BIGENDIAN(m_uDataLocalNext);
}

// Put locally cached values to data buffer
void CIFXBitStreamX::PutLocal()
{
	CIFXBitStreamX::CheckPosition();
	SWAP32_IF_BIGENDIAN(m_uDataLocal);
	SWAP32_IF_BIGENDIAN(m_uDataLocalNext);
	m_puData[m_uDataPosition] = m_uDataLocal;
	m_puData[m_uDataPosition+1] = m_uDataLocalNext;
}

// Increment position and update locally cached values
void CIFXBitStreamX::IncrementPosition()
{
	m_uDataPosition++;
	CIFXBitStreamX::CheckPosition();
	SWAP32_IF_BIGENDIAN(m_uDataLocal);
	m_puData[m_uDataPosition-1] = m_uDataLocal;
	m_uDataLocal = m_uDataLocalNext;
	m_uDataLocalNext = m_puData[m_uDataPosition+1];
	SWAP32_IF_BIGENDIAN(m_uDataLocalNext);
}


void CIFXBitStreamX::IncrementPositionReadOnly()
{
	m_uDataPosition++;
	IFXASSERT(m_uDataPosition + 2 <= m_uDataSize);
	m_uDataLocal = m_uDataLocalNext;
	m_uDataLocalNext = m_puData[m_uDataPosition+1];
	SWAP32_IF_BIGENDIAN(m_uDataLocalNext);
}

// Check position against buffer size.  Increase buffer size if needed.
void CIFXBitStreamX::CheckPosition()
{
	if(m_uDataPosition + 2 > m_uDataSize) {
		CIFXBitStreamX::AllocateDataBuffer(m_uDataPosition + 2 + m_uDataSizeIncrement);
	}
}

void CIFXBitStreamX::ReadBit(U32& rValue)
{
	U32 uValue = 0;

	// Note:  Shift operations on U32s are only valid for shifts of 0 to 31 bits.
	uValue = m_uDataLocal >> m_uDataBitOffset;
	uValue &= 1;
	m_uDataBitOffset ++;
	if(m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		IncrementPositionReadOnly();
	}

	rValue = uValue;
}

// Write the least significant bit of uValue
void CIFXBitStreamX::WriteBit(U32 uValue)
{
	// Note:  Shift operations on U32s are only valid for shifts of 0 to 31 bits.
	U32 mask = 1;
	uValue &= mask;

	m_uDataLocal &= ~(mask << m_uDataBitOffset);
	m_uDataLocal |= (uValue << m_uDataBitOffset);

	m_uDataBitOffset += 1;
	if(m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPosition();
	}
}

// Reverse significance of 8 least signigicant bits
void CIFXBitStreamX::SwapBits8(U32& rValue)
{
	IFXASSERT(rValue < 256);
	rValue = (m_puSwap8[(rValue) & 0xf] << 4) | ( m_puSwap8[(rValue) >> 4]);
}

void CIFXBitStreamX::Read15Bits(U32& rValue)
{
	// Note:  Shift operations on U32s are only valid for shifts of 0 to 31 bits.
	U32 uValue = m_uDataLocal >> m_uDataBitOffset;
	IFXASSERT(m_uDataBitOffset < 32);

	if(m_uDataBitOffset > 17)
	{
		uValue |= (m_uDataLocalNext << (32 - m_uDataBitOffset));
	}

	uValue += uValue;

	uValue = (m_puSwap8[(uValue >> 12) & 0xf])
		| ((m_puSwap8[(uValue >> 8) & 0xf ]) << 4 )
		| ((m_puSwap8[(uValue >> 4) & 0xf]) << 8 )
		| ((m_puSwap8[uValue & 0xf]) << 12 );

	//  uValue >>= 1;
	rValue = uValue;
	m_uDataBitOffset += 15;
	if(m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPositionReadOnly();
	}

	IFXASSERT(uValue < 0x00008000);
}

// Allocate data buffer.  (Or reallocate)
void CIFXBitStreamX::AllocateDataBuffer(U32 uSize)
{

	U32 *puOldData = NULL;
	U32 uOldDataSize = 0;

	try
	{
		// Store an old buffer if it exists
		if(NULL != m_puData)
		{
			puOldData = m_puData;
			uOldDataSize = m_uDataSize;
		}

		// Allocate the new buffer
		m_uDataSize = uSize;
		m_puData = new U32[m_uDataSize];
		if (NULL == m_puData)
		{
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}
		// Clear the new buffer
		memset(m_puData,0,m_uDataSize*sizeof(U32));

		// Copy data from an old buffer if it exists
		if(NULL != puOldData)
		{
			U32 uCopySize = 0;
			if(uOldDataSize < m_uDataSize)
			{
				uCopySize = (U32)sizeof(U32) * uOldDataSize;
			}

			else
			{
				uCopySize = (U32)sizeof(U32) * m_uDataSize;
			}

			memcpy(m_puData,puOldData,uCopySize);
		}

		// delete the old buffer
		IFXDELETE_ARRAY(puOldData);
	}

	catch(IFXException e)
	{
		IFXDELETE_ARRAY(puOldData);
		throw e ;
	}
}

// Check input pointer against NULL and allocate data buffer if needed.
void CIFXBitStreamX::ValidateInput(void* p)
{
	// Check input values
	if(NULL == p)
	{
		IFXCHECKX(IFX_E_INVALID_POINTER);
	}

	// Allocate a new data buffer (only if needed)
	if(NULL == m_puData)
	{
		CIFXBitStreamX::GetLocal();  // GetLocal will call CheckPosition which will call AllocateBuffer
	}
}

void CIFXBitStreamX::FastValidateInput()
{
	// Allocate a new data buffer (only if needed)
	if(NULL == m_puData)
	{
		CIFXBitStreamX::GetLocal();  // GetLocal will call CheckPosition which will call AllocateBuffer
	}
}

void CIFXBitStreamX::GetContext(U32 Context, IFXHistogramDynamic*& rpHistogram)
{
	// Check if array is big enough
	if(Context >= m_uACContextCount)
	{
		IFXHistogramDynamic **ppOldArray = m_ppACHistogram;
		m_ppACHistogram = new IFXHistogramDynamic*[Context+37];

		if(NULL == m_ppACHistogram)
		{
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}

		//U32 i=0;
		memcpy(m_ppACHistogram,ppOldArray,m_uACContextCount*sizeof(IFXHistogramDynamic*));
		memset(m_ppACHistogram+m_uACContextCount,0,(Context+37-m_uACContextCount)*sizeof(IFXHistogramDynamic*));
		m_uACContextCount = Context+37;

		IFXDELETE_ARRAY(ppOldArray);
	}

	// Allocate context if needed
	if((NULL == m_ppACHistogram[Context]))
	{
		if(m_uACContext8 == Context)
		{
			//m_ppACHistogram[Context] = (IHistogram*) new CHistogramStaticFull(256);
		}

		else if (uACStaticFull < Context)
		{
			//m_ppACHistogram[Context] = (IHistogram*) new CHistogramStaticFull(Context-uACStaticFull);
		}

		else
		{
			{
				m_ppACHistogram[Context] = new IFXHistogramDynamic(m_uElephant);
			}

			if(NULL == m_ppACHistogram[Context])
			{
				IFXCHECKX(IFX_E_OUT_OF_MEMORY);
			}
		}
	}

	// Set the output value
	rpHistogram = m_ppACHistogram[Context];
}

void CIFXBitStreamX::WriteSymbol(U32 Context, U32 Value, BOOL& rEscape)
{
	rEscape = FALSE;

	CIFXBitStreamX::FastValidateInput(); // Check that local data buffer is allocated

	if(m_uACContext8 == Context)
	{
		CIFXBitStreamX::WriteSymbolContext8(Value,rEscape);
		//CIFXBitStreamX::WriteSymbolContextStatic(uACStaticFull+256,Value+1,rEscape);
		//rc = CIFXBitStreamX::WriteSymbolContextStatic(uACStaticFull+256,Value);
	}

	else if (uACStaticFull < Context)
	{
		CIFXBitStreamX::WriteSymbolContextStatic(Context,Value,rEscape);
	}

	else
	{
		CIFXBitStreamX::WriteSymbolContextDynamic(Context,Value,rEscape);
	}
}

void CIFXBitStreamX::WriteSymbolContext8(U32 Value, BOOL& rEscape)
{
	rEscape = FALSE;

	// Check for no compression case
	if ( 0x0000FFFF == m_uACHigh && 0x00000000 == m_uACLow && 0 == m_uACUnderflow )
	{
		U32 uBits = (Value & 0x000000FF);
		CIFXBitStreamX::SwapBits8(uBits);

		// set the bits
		m_uDataLocal |= (uBits << m_uDataBitOffset);
		m_uDataBitOffset += 8;

		if ( m_uDataBitOffset >= 32)
		{
			m_uDataBitOffset -= 32;
			CIFXBitStreamX::IncrementPosition();

			if ( m_uDataBitOffset > 0 )
			{
				m_uDataLocal = (uBits >> (8 - m_uDataBitOffset));
			}
		}
#ifdef BSLOG
		bslog(0,0,Value);
#endif
	}

	else
	{
		CIFXBitStreamX::WriteSymbolContextStatic(uACStaticFull+256,Value+1,rEscape);
	}
}

void CIFXBitStreamX::WriteSymbolContextStatic(U32 Context, U32 Value, BOOL& rEscape)
{
	rEscape = FALSE;

	U32 uTotalCumFreq = 0;
	U32 uValueCumFreq = 0;
	U32 uValueFreq = 1;
	//BOOL bEscaped = false;

	// Get frequency counts (to calculate probabilites)
	U32 uNumSymbols = Context-uACStaticFull;
	uTotalCumFreq = uNumSymbols;

	if (Value > uNumSymbols)
	{
		IFXASSERT(0);
		Value = 0;
		uValueCumFreq = 0;
	}
	else
	{
		uValueCumFreq = Value - 1;
	}

	if(0 == Value)
	{
		IFXASSERT(0);
		rEscape = TRUE;
	}

	// Update state
	U32 uRange = m_uACHigh + 1 - m_uACLow;
	IFXASSERT(uRange);
	IFXASSERT(uValueFreq);
	m_uACHigh = m_uACLow - 1 + uRange * (uValueCumFreq + uValueFreq) / uTotalCumFreq;
	m_uACLow  = m_uACLow + uRange * (uValueCumFreq) / uTotalCumFreq;
	IFXASSERT(m_uACHigh+1>m_uACLow);

	// Update context
	//rc = pContext->AddSymbol(Value);

	U32 bit = m_uACLow >> 15;

	while (((m_uACHigh & m_uACHalf) == (m_uACLow & m_uACHalf)))
	{
		m_uACHigh &= ~ m_uACHalf;
		m_uACHigh += m_uACHigh + 1;
		CIFXBitStreamX::WriteBit(bit);

		while((m_uACUnderflow > 0))
		{
			m_uACUnderflow--;
			CIFXBitStreamX::WriteBit((~bit)&1);
		}

		m_uACLow &= ~ m_uACHalf;
		m_uACLow += m_uACLow;
		bit = m_uACLow >> 15;
	}

	// Check for underflow
	while ((0 == (m_uACHigh & m_uACQuarter))
		&& (m_uACQuarter == (m_uACLow & m_uACQuarter)) )
	{
		m_uACHigh &= ~ m_uACHalf;
		m_uACHigh <<= 1;
		m_uACLow <<= 1;
		m_uACHigh |= m_uACHalf;
		m_uACHigh |= 1;
		m_uACLow &= ~ m_uACHalf;
		m_uACUnderflow++;
	}

	IFXASSERT(m_uACHigh+1>m_uACLow);

#ifdef BSLOG
	bslog(0,Context,Value);
#endif
}

void CIFXBitStreamX::WriteSymbolContextDynamic(U32 Context, U32 Value, BOOL& rEscape)
{
	rEscape = FALSE;

	IFXHistogramDynamic *pContext=NULL;
	U32 uTotalCumFreq = 0;
	U32 uValueCumFreq = 0;
	U32 uValueFreq = 0;
	// BOOL bEscaped = false;

	// get context
	CIFXBitStreamX::GetContext(Context,pContext);
	IFXASSERT(pContext);


	// Get frequency counts (to calculate probabilites)
	uTotalCumFreq = pContext->GetTotalSymbolFreq();
	uValueCumFreq = pContext->GetCumSymbolFreq(Value);
	uValueFreq = pContext->GetSymbolFreq(Value);

	if(0 == uValueFreq)
	{
		Value = 0;
		uValueCumFreq = pContext->GetCumSymbolFreq(Value);
		uValueFreq = pContext->GetSymbolFreq(Value);
	}

	if(0 == Value)
	{
		rEscape = TRUE;
	}


	// Update state
	U32 uRange = m_uACHigh + 1 - m_uACLow;
	IFXASSERT(uRange);
	IFXASSERT(uValueFreq);
	m_uACHigh = m_uACLow - 1 + uRange * (uValueCumFreq + uValueFreq) / uTotalCumFreq;
	m_uACLow  = m_uACLow + uRange * (uValueCumFreq) / uTotalCumFreq;
	IFXASSERT(m_uACHigh+1>m_uACLow);

	// Update context
	pContext->AddSymbol(Value);


	// Write bits
	U32 bit = m_uACLow >> 15;

	while (((m_uACHigh & m_uACHalf) == (m_uACLow & m_uACHalf)))
	{
		m_uACHigh &= ~ m_uACHalf;
		m_uACHigh += m_uACHigh + 1;
		CIFXBitStreamX::WriteBit(bit);

		while((m_uACUnderflow > 0))
		{
			m_uACUnderflow--;
			CIFXBitStreamX::WriteBit((~bit)&1);
		}

		m_uACLow &= ~ m_uACHalf;
		m_uACLow += m_uACLow;
		bit = m_uACLow >> 15;
	}

	// Check for underflow
	while ((0 == (m_uACHigh & m_uACQuarter))
		&& (m_uACQuarter == (m_uACLow & m_uACQuarter)) )
	{
		m_uACHigh &= ~ m_uACHalf;
		m_uACHigh <<= 1;
		m_uACLow <<= 1;
		m_uACHigh |= m_uACHalf;
		m_uACHigh |= 1;
		m_uACLow &= ~ m_uACHalf;
		m_uACUnderflow++;
	}


	IFXASSERT(m_uACHigh+1>m_uACLow);

#ifdef BSLOG
	bslog(0,Context,Value);
#endif
}

void CIFXBitStreamX::ReadSymbol(U32 Context, U32& rValue)
{
	CIFXBitStreamX::FastValidateInput();  // Check that local data buffer is allocated;

	if(m_uACContext8 == Context)
	{
		//rc = CIFXBitStreamX::ReadSymbolContext8(pValue);
		CIFXBitStreamX::ReadSymbolContextStatic(uACStaticFull+256,rValue);
	}

	else if (uACStaticFull < Context)
	{
		CIFXBitStreamX::ReadSymbolContextStatic(Context,rValue);
	}

	else
	{
		CIFXBitStreamX::ReadSymbolContextDynamic(Context,rValue);
	}

}

void CIFXBitStreamX::ReadSymbolContext8(U32& rValue)
{
	CIFXBitStreamX::FastValidateInput();  // Check that local data buffer is allocated;

	// Check for no compression case
	if ( 0x0000FFFF == m_uACHigh && 0x00000000 == m_uACLow && 0 == m_uACUnderflow )
	{
		// Note:  Shift operations on U32s are only valid for shifts of 0 to 31 bits.
		U32 uValue = m_uDataLocal >> m_uDataBitOffset;
		IFXASSERT(m_uDataBitOffset < 32);

		if(m_uDataBitOffset > 24)
		{
			uValue |= (m_uDataLocalNext << (32 - m_uDataBitOffset));
		}

		uValue &= 0x000000FF;
		rValue = uValue;

		m_uDataBitOffset += 8;

		if ( m_uDataBitOffset >= 32)
		{
			m_uDataBitOffset -= 32;
			CIFXBitStreamX::IncrementPositionReadOnly();
		}
#ifdef BSLOG
		bslog(1,0,uValue);
#endif
	}
	else
	{
		CIFXBitStreamX::ReadSymbolContextStatic(uACStaticFull+256,rValue);
		rValue--;
		IFXASSERT(rValue < 256);
		CIFXBitStreamX::SwapBits8(rValue);
	}
}

void CIFXBitStreamX::ReadSymbolContextStatic(U32 Context, U32& rValue)
{
	U32 uValue = 0;

	// Fill in the code word
	U32 uPosition = 0;
	CIFXBitStreamX::GetBitCount(uPosition);
	CIFXBitStreamX::ReadBit(m_uACCode);
	m_uDataBitOffset += m_uACUnderflow;

	while (m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPositionReadOnly();
	}

	U32 uTemp = 0;
	CIFXBitStreamX::Read15Bits(uTemp);
	m_uACCode <<= 15;
	m_uACCode |= uTemp;
	CIFXBitStreamX::SeekToBitReadOnly(uPosition);

	// Get total count (to calculate probabilites)
	U32 uNumSymbols = Context-uACStaticFull;
	U32 uTotalCumFreq = 0;
	uTotalCumFreq = uNumSymbols;

	// Get the cumulative frequency of the current symbol
	U32 uRange = m_uACHigh + 1 - m_uACLow;
	IFXASSERT(uRange);

	// The relationship:
	// uCodeCumFreq <= (uTotalCumFreq * (m_uACCode - m_uACLow)) / uRange
	// is used to calculate the cumulative frequency of the current symbol.
	// The +1 and -1 in the line below are used to counteract finite word length
	// problems resulting from the division by uRange.
	U32 uCodeCumFreq = ((uTotalCumFreq) * (1 + m_uACCode - m_uACLow) - 1) / (uRange);

	// Get the current symbol
	IFXASSERT(uCodeCumFreq < uNumSymbols);
	//uValue = pContext->CHistogramStaticFull::GetSymbolFromFreq(uCodeCumFreq);
	//uValue = uCodeCumFreq >= uNumSymbols ? (uNumSymbols - 1) : uCodeCumFreq;
	//I32 iMask = (uNumSymbols-1) - uCodeCumFreq;
	//iMask >>= 31;
	//uValue = (iMask & (uNumSymbols-1) - uCodeCumFreq) + uCodeCumFreq;
	uValue = uCodeCumFreq + 1;

	IFXASSERT(uValue <= uNumSymbols);

	// Update state
	//iMask = (uNumSymbols-1) - uValue;
	//iMask >>= 31;
	//U32 uValueFreq = pContext->CHistogramStaticFull::GetSymbolFreq(uValue);
	//U32 uValueFreq = uValue >= uNumSymbols ? 0 : 1;
	//U32 uValueFreq = (iMask & -1) + 1;
	U32 uValueFreq = 1;

	//U32 uValueCumFreq = pContext->CHistogramStaticFull::GetCumSymbolFreq(uValue);
	//U32 uValueCumFreq = uValue >= uNumSymbols ? uNumSymbols : uValue;
	//U32 uValueCumFreq = (iMask & (uNumSymbols - uValue)) + uValue ;
	U32 uValueCumFreq = uValue - 1;

	// Update context
	//rc = pContext->CHistogramStaticFull::AddSymbol(uValue);

	U32 uLow = m_uACLow;
	U32 uHigh = m_uACHigh;

	uHigh = uLow - 1 + uRange * (uValueCumFreq + uValueFreq) / uTotalCumFreq;
	uLow  = uLow + uRange * (uValueCumFreq) / uTotalCumFreq;

	U32 uState = uLow << 16 | uHigh ;
	U32 uBitCount;
	U32 uMaskedState;

	// Count bits to read

	// Fast count the first 4 bits
	uBitCount = CIFXBitStreamX::m_puReadCount[(((uLow >> 12) ^ (uHigh >> 12)) & 0x0000000F)];
	uState &= CIFXBitStreamX::m_puFastNotMask[uBitCount];
	uState <<= uBitCount;
	uState |= ((1 << uBitCount) - 1);

	// Fast count the next 4 bits
	U32 uBitCount2 = CIFXBitStreamX::m_puReadCount[(((uState >> 12) ^ (uState >> 28)) & 0x0000000F)];
	uState &= CIFXBitStreamX::m_puFastNotMask[uBitCount2];
	uState <<= uBitCount2;
	uBitCount += uBitCount2;
	uState |= ((1 << uBitCount2) - 1);

	// Regular count the rest
	uMaskedState = m_uHalfMask & uState;

	while (0 == uMaskedState || m_uHalfMask == uMaskedState)
	{
		uState = ((m_uNotHalfMask & uState) << 1) | 1;
		//uState &= m_uNotHalfMask;
		//uState += uState;
		//uState |= 1;
		uMaskedState = m_uHalfMask & uState;
		uBitCount++;
	}

#ifdef BSSTAT
#ifdef BSSTAT_BitCountStatic
	if(uBitCount <16)
	{
		d_puStats[uBitCount] +=1;
	}

	else
	{
		d_puStats[15] +=1;
	}
#endif
#endif

	U32 uSavedBits = uMaskedState;

	if(uBitCount > 0)
	{
		uBitCount += m_uACUnderflow;
		m_uACUnderflow = 0;
	}

	uMaskedState = m_uQuarterMask & uState;
	U32 uUnderflow = 0;

	while( 0x40000000 == uMaskedState)
	{
		//uState = ((uState & m_uNotThreeQuarterMask) << 1) | 1;
		uState &= m_uNotThreeQuarterMask;
		uState += uState;
		uState |= 1;
		uMaskedState = m_uQuarterMask & uState;
		uUnderflow++;
	}

#ifdef BSSTAT
#ifdef BSSTAT_UnderflowStatic
	if(uUnderflow <16)
	{
		d_puStats[uUnderflow] +=1;
	}

	else
	{
		d_puStats[15] +=1;
	}
#endif
#endif

	m_uACUnderflow += uUnderflow;

	uState |= uSavedBits;

	m_uACLow = uState >> 16;
	m_uACHigh = uState & 0x0000FFFF;

	m_uDataBitOffset += uBitCount;

	while(m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPositionReadOnly();
	}

	// Set return value
	rValue = uValue;

#ifdef BSLOG
	bslog(1,Context,uValue);
#endif
}


void CIFXBitStreamX::ReadSymbolContextDynamic(U32 Context, U32& rValue)
{
	IFXHistogramDynamic *pContext=NULL;
	U32 uValue = 0;

	// Fill in the code word
	U32 uPosition = 0;
	CIFXBitStreamX::GetBitCount(uPosition);
	CIFXBitStreamX::ReadBit(m_uACCode);
	m_uDataBitOffset += m_uACUnderflow;

	while (m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPositionReadOnly();
	}

	U32 uTemp = 0;
	CIFXBitStreamX::Read15Bits(uTemp);
	m_uACCode <<= 15;
	m_uACCode |= uTemp;
	CIFXBitStreamX::SeekToBitReadOnly(uPosition);

	// get context
	CIFXBitStreamX::GetContext(Context,pContext);
	IFXASSERT(pContext);

	// Get total count (to calculate probabilites)
	U32 uTotalCumFreq = pContext->GetTotalSymbolFreq();

	// Get the cumulative frequency of the current symbol
	U32 uRange = m_uACHigh + 1 - m_uACLow;
	IFXASSERT(uRange);

	// The relationship:
	// uCodeCumFreq <= (uTotalCumFreq * (m_uACCode - m_uACLow)) / uRange
	// is used to calculate the cumulative frequency of the current symbol.
	// The +1 and -1 in the line below are used to counteract finite word length
	// problems resulting from the division by uRange.
	U32 uCodeCumFreq = ((uTotalCumFreq) * (1 + m_uACCode - m_uACLow) - 1) / (uRange);

	// Get the current symbol
	uValue = pContext->GetSymbolFromFreq(uCodeCumFreq);

	// Update state and context
	U32 uValueCumFreq = pContext->GetCumSymbolFreq(uValue);
	U32 uValueFreq = pContext->GetSymbolFreq(uValue);

	U32 uLow = m_uACLow;
	U32 uHigh = m_uACHigh;

	uHigh = uLow - 1 + uRange * (uValueCumFreq + uValueFreq) / uTotalCumFreq;
	uLow  = uLow + uRange * (uValueCumFreq) / uTotalCumFreq;
	pContext->AddSymbol(uValue);

	U32 uState = uLow << 16 | uHigh ;
	U32 uBitCount;
	U32 uMaskedState;

	// Count bits to read

	// Fast count the first 4 bits
	uBitCount = CIFXBitStreamX::m_puReadCount[(((uLow >> 12) ^ (uHigh >> 12)) & 0x0000000F)];
	uState &= CIFXBitStreamX::m_puFastNotMask[uBitCount];
	uState <<= uBitCount;
	uState |= ((1 << uBitCount) - 1);

	// Regular count the rest
	uMaskedState = m_uHalfMask & uState;

	while (0 == uMaskedState || m_uHalfMask == uMaskedState)
	{
		uState = ((m_uNotHalfMask & uState) << 1) | 1;
		uMaskedState = m_uHalfMask & uState;
		uBitCount++;
	}

#ifdef BSSTAT
#ifdef BSSTAT_BitCount
	if(uBitCount <16)
	{
		d_puStats[uBitCount] +=1;
	}

	else
	{
		d_puStats[15] +=1;
	}
#endif
#endif

	U32 uSavedBits = uMaskedState;

	if(uBitCount > 0)
	{
		uBitCount += m_uACUnderflow;
		m_uACUnderflow = 0;
	}

	// Count underflow bits
	uMaskedState = m_uQuarterMask & uState;
	U32 uUnderflow = 0;

	while( 0x40000000 == uMaskedState)
	{
		//uState = ((uState & m_uNotThreeQuarterMask) << 1) | 1;
		uState &= m_uNotThreeQuarterMask;
		uState += uState;
		uState |= 1;
		uMaskedState = m_uQuarterMask & uState;
		uUnderflow++;
	}

#ifdef BSSTAT
#ifdef BSSTAT_Underflow
	if(uUnderflow <16)
	{
		d_puStats[uUnderflow] +=1;
	}

	else
	{
		d_puStats[15] +=1;
	}
#endif
#endif

	// Store the state
	m_uACUnderflow += uUnderflow;
	uState |= uSavedBits;
	m_uACLow = uState >> 16;
	m_uACHigh = uState & 0x0000FFFF;

	// Update bit read position
	m_uDataBitOffset += uBitCount;

	while(m_uDataBitOffset >= 32)
	{
		m_uDataBitOffset -= 32;
		CIFXBitStreamX::IncrementPositionReadOnly();
	}

	// Set return value
	rValue = uValue;

#ifdef BSLOG
	bslog(1,Context,uValue);
#endif
}

//---------------------------------------------------------------------------
//  CIFXBitStream_Factory
//
//  This is the CIFXBitStreamX component factory function.  The
//  CIFXBitStreamX component can be instaniated multiple times.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXBitStreamX_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if(ppInterface) {
		// Create the CIFXBitStreamX component.
		CIFXBitStreamX *pComponent = new  CIFXBitStreamX;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

// EOF
