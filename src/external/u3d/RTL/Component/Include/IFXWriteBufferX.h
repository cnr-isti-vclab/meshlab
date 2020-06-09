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
	@file IFXWriteBufferX.h
	Declaration of IFXWriteBufferX exception-based interface.
    The IFXWriteBufferX is used to write to a file or memory buffer.
  */


#ifndef IFXWRITEBUFFERX_H__
#define IFXWRITEBUFFERX_H__

#include "IFXUnknown.h"

// {42956896-27AE-47c6-9384-0B9EECD65692}
IFXDEFINE_GUID(IID_IFXWriteBufferX,
0x42956896, 0x27ae, 0x47c6, 0x93, 0x84, 0xb, 0x9e, 0xec, 0xd6, 0x56, 0x92);

/** IFXWriteBufferX is used to write data. Depending on the object that implements
the interface, the data can be written to an external file or an internal memory buffer.*/
class IFXWriteBufferX : virtual public IFXUnknown
{
public:
	/**
		WriteX will copy "uCount" bytes from the memory pointed to by "pBytes" to an output buffer starting at "uPosition."
		@param pBytes
		Memory location where the bytes to be written come from
		@param uPosition
		Position within the output buffer to start the write
		@param uCount
		Number of bytes of data to write
	*/
	virtual void IFXAPI  WriteX( U8* pBytes, U64 uPosition, U32 uCount ) = 0;
};


//***************************************************************************
//	Failure return codes
//***************************************************************************

// Return codes specific to the WriteBufferX interface
// WARNINGS

// FAILURES
#define IFX_E_WROTE_PAST_END		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0001 )
#define IFX_E_IO_WRITE				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0002 )
#define IFX_E_STREAM_NOT_OPEN		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0004 )
#define IFX_E_BAD_PARAMETER			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_WRITE_BUFFER, 0x0008 )

#endif // IFXWRITEBUFFERX_H__
