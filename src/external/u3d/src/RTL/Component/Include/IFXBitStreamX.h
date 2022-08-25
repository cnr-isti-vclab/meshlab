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
	@file	IFXBitStreamX.h

			Declaration of IFXBitStream exception-based interface
*/

#ifndef IFXBITSTREAMX_H__
#define IFXBITSTREAMX_H__

#include "IFXUnknown.h"
#include "IFXDataBlockX.h"
#include "IFXString.h"

// {CC8F78B3-38C1-4055-A9C7-18B96FF1F344}
IFXDEFINE_GUID(IID_IFXBitStreamX,
0xcc8f78b3, 0x38c1, 0x4055, 0xa9, 0xc7, 0x18, 0xb9, 0x6f, 0xf1, 0xf3, 0x44);

class IFXBitStreamX : public IFXUnknown
{
public:
	///	@name	Current position change methods
	//@{
	/// Set the current position to the next byte boundary if needed
	virtual void IFXAPI  AlignToByteX() = 0;
	/// Set the current position to the next 4 byte boundary if needed
	virtual void IFXAPI  AlignTo4ByteX() = 0;
	//@}

    /// @name	Bit writing methods
	//@{
	/// Write U8
	virtual void IFXAPI  WriteU8X(U8 Value) = 0;
	/// Write U16
	virtual void IFXAPI  WriteU16X(U16 Value) = 0;
	/// Write U32
	virtual void IFXAPI  WriteU32X(U32 Value) = 0;
	/// Write U64
	virtual void IFXAPI  WriteU64X(U64 Value) = 0;
	/// Write I32
	virtual void IFXAPI  WriteI32X(I32 Value) = 0;
	/// Write F32
	virtual void IFXAPI  WriteF32X(F32 Value) = 0;
	/// Write F64
	virtual void IFXAPI  WriteF64X(F64 Value) = 0;
	/// Write IFXString
	virtual void IFXAPI  WriteIFXStringX(IFXString& String) = 0;
	//@}

    /// @name	Bit reading methods
	//@{
	/// Read U8
	virtual void IFXAPI  ReadU8X(U8& rValue) = 0;
	/// Read U16
	virtual void IFXAPI  ReadU16X(U16& rValue) = 0;
	/// Read U32
	virtual void IFXAPI  ReadU32X(U32& rValue) = 0;
	/// Read U64
	virtual void IFXAPI  ReadU64X(U64& rValue) = 0;
	/// Read I32
	virtual void IFXAPI  ReadI32X(I32& rValue) = 0;
	/// Read F32
	virtual void IFXAPI  ReadF32X(F32& rValue) = 0;
	/// Read F64
	virtual void IFXAPI  ReadF64X(F64& rValue) = 0;
	/// Read IFXString
	virtual void IFXAPI  ReadIFXStringX(IFXString& rString) = 0;
	//@}

    /// @name	Data exchange methods using IFXDataBlockX
	//@{
    /// Get data block containing bitstream data
    virtual void IFXAPI  GetDataBlockX(IFXDataBlockX*& rpBlock) = 0;
    /// Set bitstream to use the data in a data block
    virtual void IFXAPI  SetDataBlockX(IFXDataBlockX& rBlock) = 0;
	//@}
};

#define IFX_W_BITSTREAM_ESCAPE			MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_BITSTREAM, 0x000e )

#endif
