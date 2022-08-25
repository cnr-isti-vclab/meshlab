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
//	IFXWriteBuffer.h
//
//	DESCRIPTION:
//		Declaration of IFXWriteBuffer interface.
//		The IFXWriteBuffer is used to write to a file.
//
//	NOTES
//***************************************************************************
#ifndef IFXWRITEBUFFER_H__
#define IFXWRITEBUFFER_H__

#include "IFXUnknown.h"

// {5CFFC224-CEA4-11d3-9519-00A0C90AB136}
IFXDEFINE_GUID(IID_IFXWriteBuffer,
0x5cffc224, 0xcea4, 0x11d3, 0x95, 0x19, 0x0, 0xa0, 0xc9, 0xa, 0xb1, 0x36);

/**The IFXWriteBuffer interface is used to write to a file. */
class IFXWriteBuffer : virtual public IFXUnknown
{
public:
	/**
		Write will copy "uCount" bytes from the memory pointed to by "pBytes" to an output buffer starting at "uPosition."
		@param pBytes
		Memory location where the bytes to be written come from
		@param position
		Position within the output buffer to start the write
		@param count
		Number of bytes of data to write
	*/
	virtual IFXRESULT IFXAPI  Write( U8* pBytes, U64 position, U32 count ) = 0;
};


//***************************************************************************
//	Failure return codes
//***************************************************************************

// Return codes specific to the WriteBuffer component
// WARNINGS

// FAILURES
#define IFX_E_WROTE_PAST_END		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0001 )
#define IFX_E_IO_WRITE				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0002 )
#define IFX_E_STREAM_NOT_OPEN		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0004 )
#define IFX_E_BAD_PARAMETER			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0008 )

#endif // IFXWRITEBUFFER_H__
