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
	@file	IFXReadBufferX.h

			Declaration of IFXReadBufferX exception-based interface.
			IFXReadBufferX is used to read from a file or memory buffer.
*/

#ifndef __IFXREADBUFFERX_H__
#define __IFXREADBUFFERX_H__

#include "IFXUnknown.h"

// {99C40294-24F5-42b2-B342-237B45EBEC09}
IFXDEFINE_GUID(IID_IFXReadBufferX,
0x99c40294, 0x24f5, 0x42b2, 0xb3, 0x42, 0x23, 0x7b, 0x45, 0xeb, 0xec, 0x9);

/// IFXReadBufferX is used to read data.
/// Depending on the object that implements the interface,
/// the data can be read from an external file or an internal memory buffer.
class IFXReadBufferX : virtual public IFXUnknown
{
public:
	/**
		ReadX will copy "uCount" bytes into the memory pointed to by "pBytes" from an input buffer starting at "uPosition."
		@param pBytes
		Memory location where the bytes are to be copied
		@param uPosition
		Position within the input buffer to start the read
		@param uCount
		Number of bytes of data to read
		@param rWarningCode
		rWarningCode is an output value.
		If the read cannot be completed during normal operation (non-error),
		rWarningCode will be set to a warning code.
		rWarningCode will be set to IFX_W_END_OF_FILE
		if the read was attempted beyond the end of the file.
		rWarningCode will be set to IFX_W_DATA_NOT_AVAILABLE
		if the read attempted beyond the available portion of the file.
		rWarningCode will be set of IFX_OK if the read is successful.
		If the read cannot be completed due to an error condition, ReadX will throw an IFXException.
	*/
	virtual void IFXAPI  ReadX( U8* pBytes, U64 uPosition, U32 uCount, IFXRESULT& rWarningCode ) = 0;

	/** 
		Get the total size of the buffer in bytes
		@param ruCount ruCount will be set to the total size of the buffer (or file).
	*/
	virtual void IFXAPI  GetTotalSizeX( U64& ruCount ) = 0;

	/** 
		Get the size of the buffer which is available for reading.  The available size may be less than the total size.
		@param ruCount ruCount will be set to the number of available bytes in the buffer (or file).
	*/
	virtual void IFXAPI  GetAvailableSizeX( U64& ruCount ) = 0;
};


//***************************************************************************
//	Failure return codes
//***************************************************************************

// Return codes specific to the ReadBufferX component
// WARNINGS
#define IFX_W_DATA_NOT_AVAILABLE	MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_READ_BUFFER, 0x0001 )
#define IFX_W_END_OF_FILE			MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_READ_BUFFER, 0x0002 )
#define IFX_W_READING_NOT_COMPLETE	MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_READ_BUFFER, 0x0003 )

// FAILURES
#define IFX_E_END_OF_FILE			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_READ_BUFFER, 0x0002 )
#define IFX_E_IO					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_READ_BUFFER, 0x0003 )
#define IFX_E_BAD_FILENAME			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_READ_BUFFER, 0x0004 )
#define IFX_E_READ_AHEAD_TO_FAR		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_READ_BUFFER, 0x0005 )

#endif
