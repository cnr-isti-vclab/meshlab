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
//
// CIFXBitStreamX.h
//
// DESCRIPTION:
//     Declaration of CIFXBitStreamX class
//*****************************************************************************

#ifndef CIFXBitStreamX_H__
#define CIFXBitStreamX_H__

#include "IFXBitStreamCompressedX.h"
#include "IFXHistogramDynamic.h"
#include "IFXAutoRelease.h"


class CIFXBitStreamX : public IFXBitStreamCompressedX
{
public:
    // Factory function.
    friend IFXRESULT IFXAPI_CALLTYPE CIFXBitStreamX_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
    U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

    // IFXBitStreamX
	void IFXAPI 	AlignToByteX();
	void IFXAPI 	 AlignTo4ByteX();
	void IFXAPI 	 WriteU8X(U8 Value);
	void IFXAPI 	 WriteU16X(U16 Value);
	void IFXAPI 	 WriteU32X(U32 Value);
	void IFXAPI 	 WriteU64X(U64 Value);
	void IFXAPI 	 WriteI32X(I32 Value);
	void IFXAPI 	 WriteF32X(F32 Value);
	void IFXAPI 	 WriteF64X(F64 Value);
	void IFXAPI 	 WriteIFXStringX(IFXString& rString);
	void IFXAPI 	 ReadU8X(U8& rValue);
	void IFXAPI 	 ReadU16X(U16& rValue);
	void IFXAPI 	 ReadU32X(U32& rValue);
	void IFXAPI 	 ReadU64X(U64& rValue);
	void IFXAPI 	 ReadI32X(I32& rValue);
	void IFXAPI 	 ReadF32X(F32& rValue);
	void IFXAPI 	 ReadF64X(F64& rValue);
	void IFXAPI 	 ReadIFXStringX(IFXString& rString);
    void IFXAPI 	 GetDataBlockX(IFXDataBlockX*& rpBlock);
	void IFXAPI 	 SetDataBlockX(IFXDataBlockX& rBlock);

    // IFXBitStreamCompresedX
	void IFXAPI 	 WriteCompressedU32X(U32 Context, U32 Value);
	void IFXAPI 	 WriteCompressedU16X(U32 Context, U16 Value);
	void IFXAPI 	 WriteCompressedU8X(U32 Context, U8 Value);
	void IFXAPI 	 ReadCompressedU32X(U32 Context, U32& rValue);
	void IFXAPI 	 ReadCompressedU16X(U32 Context, U16& rValue);
	void IFXAPI 	 ReadCompressedU8X(U32 Context, U8& rValue);
	void IFXAPI 	 SetNoCompressionMode(BOOL isNoCompression);
	void IFXAPI 	 SetElephantX(U32 uElephant);

private:
    // Member functions.
    CIFXBitStreamX();
	virtual	~CIFXBitStreamX();

	// SeekToBit:  Set the current read/write position
	void SeekToBit(U32 position);
	void SeekToBitReadOnly(U32 position);
	
	// GetBitCount:  Get the number of bits read/written
	void GetBitCount(U32& rCount);
	void GetLocal();  // Get locally cached values from data buffer
	void GetLocalNoCheck();  // Get locally cached values from data buffer	
	void PutLocal();  // Put locally cached values to data buffer
	
	void IncrementPosition();  // Increment position and update locally cached values	
	void IncrementPositionReadOnly();  // Increment position and update locally cached values
	
	void CheckPosition();  // Check position against buffer size.  Increase buffer size if needed.
	void ReadBit(U32& rValue);
	void WriteBit(U32 uValue);  // Write the least significant bit of uValue
	IFXFORCEINLINE void SwapBits8(U32& rValue);  // Reverse significance of 8 least signigicant bits
	void Read15Bits(U32& rValue);
	
	void AllocateDataBuffer(U32 uSize);  // Allocate data buffer.  (Or reallocate)
	void ValidateInput(void* p);  // Check input pointer against NULL and allocate data buffer if needed.
	void FastValidateInput();  // allocate data buffer if needed.	
	void GetContext(U32 Context, IFXHistogramDynamic*& rpHistogram);

	void WriteSymbol(U32 Context, U32 Value, BOOL& rEscape);
	void WriteSymbolContext8(U32 Value, BOOL& rEscape);
	void WriteSymbolContextStatic(U32 Context, U32 Value, BOOL& rEscape);
	void WriteSymbolContextDynamic(U32 Context, U32 Value, BOOL& rEscape);

	void ReadSymbol(U32 Context, U32& rValue);
	void ReadSymbolContext8(U32& rValue);
	void ReadSymbolContextStatic(U32 Context, U32& rValue);
	void ReadSymbolContextDynamic(U32 Context, U32& rValue);

	// Member data.
	U32	m_uRefCount;		// Number of outstanding references to the object.
	BOOL m_bNoCompression;

	// Bitstream Data members
	U32 *m_puData;		// Pointer to the data buffer (as a pointer to U32)
	U32 m_uDataSize;		// Current size of the data buffer (in U32s)
	U32 m_uDataPosition;	// Index of the currently active section of the data buffer
	U32 m_uDataLocal;		// Locally cached value from data buffer. m_uDataLocal == m_puData[m_uDataPosition]
	U32 m_uDataLocalNext;  // The next U32 after m_uDataLocal in the data buffer
	U32 m_uDataBitOffset;	// Current bit position within m_uDataLocal

	// Arithmetic compression members
	U32 m_uACHigh;
	U32 m_uACCode;
	U32 m_uACLow;
	U32 m_uACUnderflow;
	IFXHistogramDynamic **m_ppACHistogram;
	U32 m_uACContextCount;
	BOOL m_uCompressed;
	U32 m_uElephant;

	// Constants
	static const U32 m_uDataSizeInitial;  // Initial size to allocate the data buffer (in U32s)
	static const U32 m_uDataSizeIncrement;  // Size to increment the data buffer when reallocating (in U32s)

	static const U32 m_uACHalf;
	static const U32 m_uACQuarter;

	static const U32 m_uACContext8;
	static const U32 m_puSwap8[16];

	static const U32 m_puReadCount[16];
	static const U32 m_puFastNotMask[5];

	static const U32 m_uHalfMask;
	static const U32 m_uNotHalfMask ;
	static const U32 m_uQuarterMask ;
	static const U32 m_uNotThreeQuarterMask ;
#ifdef BSSTAT
	U32 d_puStats[16];
#endif

};

#endif
