//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXBitStreamCompressedX.h

			Declaration of IFXBitStreamCompressedX exception-based interface.
*/

#ifndef IFXBITSTREAMCOMPRESSEDX_H__
#define IFXBITSTREAMCOMPRESSEDX_H__

#include "IFXBitStreamX.h"

// {8F594846-C215-48d5-A28C-D1CE3B8F0A48}
IFXDEFINE_GUID(IID_IFXBitStreamCompressedX,
0x8f594846, 0xc215, 0x48d5, 0xa2, 0x8c, 0xd1, 0xce, 0x3b, 0x8f, 0xa, 0x48);

/** 
	Declaration of IFXBitStreamCompressedX exception-based interface.
*/
class IFXBitStreamCompressedX : public IFXBitStreamX
{
public:
    /** Bit writing method. */
	virtual void IFXAPI  WriteCompressedU32X(U32 Context, U32 Value) = 0;
	/** Bit writing method. */
	virtual void IFXAPI  WriteCompressedU16X(U32 Context, U16 Value) = 0;
	/** Bit writing method. */
	virtual void IFXAPI  WriteCompressedU8X(U32 Context, U8 Value) = 0;

    /** Bit reading method. */
	virtual void IFXAPI  ReadCompressedU32X(U32 Context, U32& rValue) = 0;
	/** Bit reading method. */
	virtual void IFXAPI  ReadCompressedU16X(U32 Context, U16& rValue) = 0;
	/** Bit reading method. */
	virtual void IFXAPI  ReadCompressedU8X(U32 Context, U8& rValue) = 0;

	/** Set no compression mode. */
	virtual void IFXAPI  SetNoCompressionMode(BOOL isNoCompression) = 0;
	/** Compression context management method. */
	virtual void IFXAPI  SetElephantX(U32 uElephant) = 0;
};

#endif
