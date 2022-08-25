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
	@file	IFXSocketStream.h

			This header defines the socket stream functionality.
*/


#ifndef IFXSocketStream_H
#define IFXSocketStream_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXDataTypes.h"

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************

/**
	This class is used to interact with socket API.
*/
class IFXSocketStream
{
public:
	IFXSocketStream();
	~IFXSocketStream();

	/**
		Initializes socket library and open socket for reading.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
				following failures are returned: IFX_E_INVALID_HANDLE.
	*/
	IFXRESULT Open();

	/**
		Makes connection on socket.

		@param	pServer	Name of server to connect.

		@param	port	Port number on @e pServer.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
				following failures are returned: IFX_E_INVALID_POINTER or 
				IFX_E_UNDEFINED.
	*/
	IFXRESULT Connect( const I8* pServer, U16 port );

	/**
		Receives data from socket.

		@param	pBuffer	Buffer where to read data.

		@param	nBytes	Number of bytes to read in @e pBuffer.

		@return Number of bytes actually read to @e pBuffer. It might not be equal
				to the @e nBytes.
	*/
	I32 Receive( I8* pBuffer, I32 nBytes );

	/**
		Sends data to socket.

		@param	pBuffer	Buffer with data to send.

		@param	nBytes	Number of bytes in @e pBuffer.

		@return Number of bytes actually sent from @e pBuffer. 
				It might not be equal to @e nBytes.
	*/
	I32 Send( const I8* pBuffer, I32 nBytes );

	/**
		Reads data portion from file to buffer.

		@param	pBuffer				Buffer where to read portion of data.

		@param	numberOfBytesToRead	Number of bytes to read in @e pBuffer.

		@param	pNumberOfBytesRead	Number of bytes actually read to @e pBuffer.

		@return	Upon success, IFX_OK or IFX_W_FINISHED is returned. Otherwise, one of 
		the following failures are returned: IFX_E_INVALID_POINTER, IFX_E_BAD_PARAM,
		IFX_E_INVALID_FILE, IFX_E_READ_FAILED.
	*/
	IFXRESULT ReadBytes( U8* pBuffer, U32 numberOfBytesToRead, U32* pNumberOfBytesRead );

	/**
		Sends data portion to file from null terminated buffer.

		@param	pBuffer	Null terminated buffer with data to send.
	*/
	IFXRESULT SendBytes( const I8* pBuffer );

	/**
		Sets a socket option.

		@param	option			Socket option for which the value is to be set. 
								@note See setsockopt() function from Socket API.

		@param	pBuffer			Pointer to the buffer in which the value for the
								requested option is specified.
								@note See setsockopt() function from Socket API.

		@param	bufferLength	Size of the @e pBuffer buffer, in bytes.
								@note See setsockopt() function from Socket API.
	*/
	IFXRESULT SetOptions( U32 option, const I8* pBuffer, I32 bufferLength );

	/**
		Gets a socket option.

		@param	option			Socket option for which the value is to be retrieved.
								@note See getsockopt() function from Socket API.

		@param	pBuffer			Pointer to the buffer in which the value for the 
								requested option is to be returned. 
								@note See getsockopt() function from Socket API.

		@param	pBufferLength	Pointer to the size of the @e pBuffer buffer, in bytes. 
								@note See getsockopt() function from Socket API.
	*/
	IFXRESULT GetOptions( U32 option, I8* pBuffer, I32* pBufferLength );

	/**
		Closes socket.
	*/
	IFXRESULT Close();

private:
	I32 m_socket;
};

#endif
