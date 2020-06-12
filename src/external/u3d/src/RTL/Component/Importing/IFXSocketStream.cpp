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
	@file	IFXSocketStream.cpp

			This module contains functionality to wrap Socket API. 
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXSocketStream.h"
#include "IFXOSUtilities.h"
#include "IFXOSSocket.h"

#include <string.h>
#include <stdlib.h>

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


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************

IFXSocketStream::IFXSocketStream() : m_socket(0)
{
}


IFXSocketStream::~IFXSocketStream()
{
	Close();
}


IFXRESULT IFXSocketStream::Open()
{
	IFXRESULT result = IFX_OK;

	m_socket = IFXOSSocketOpen();
	
	if( -1 == m_socket )
		result = IFX_E_INVALID_HANDLE;

	return result;
}

IFXRESULT IFXSocketStream::Connect( const I8* pServer, U16 port )
{
	return IFXOSSocketConnect( m_socket, (const char *)pServer, port );
}

I32 IFXSocketStream::Receive( I8* pBuffer, I32 nBytes )
{
	return IFXOSSocketReceive( m_socket, (char *)pBuffer, nBytes, 0 );
}

I32 IFXSocketStream::Send( const I8* pBuffer, I32 nBytes )
{
	return IFXOSSocketSend( m_socket, (const char *)pBuffer, nBytes, 0 );
}

IFXRESULT IFXSocketStream::SendBytes( const I8* pBuffer )
{
	IFXRESULT result = IFX_OK;
	I32 len = strlen( (const char *)pBuffer );

	I32 bytesSent = IFXOSSocketSend( m_socket, (const char *)pBuffer, len, 0 );

	if( (-1 == bytesSent) || (bytesSent != len) )
	{
		IFXTRACE_GENERIC(L"[IFXSocketStream] Send failed\n" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}

IFXRESULT IFXSocketStream::ReadBytes( 
								U8* pBuffer, 
								U32 numberOfBytesToRead, 
								U32* pNumberOfBytesRead )
{
	IFXRESULT result = IFX_OK;
	I32 nread = 0;

	if( NULL == pBuffer || NULL == pNumberOfBytesRead )
		result = IFX_E_INVALID_POINTER;

	if( 0 == m_socket )
		result = IFX_E_NOT_INITIALIZED;

	if( 0 == numberOfBytesToRead )
		result = IFX_E_BAD_PARAM;

	if( IFXSUCCESS( result ) )
	{
		I32 nleft = numberOfBytesToRead;

		I8* pBufferPtr = (I8*)pBuffer;

		// receive data till nothing to read
		while( nleft > 0 ) 
		{
			pBufferPtr += nread;
			nread = IFXOSSocketReceive( m_socket, (char *)pBufferPtr, nleft, 0);

			if ( nread < 0 ) // read error
			{
				IFXTRACE_GENERIC(L"[IFXSocketStream] Read failed\n");
				result = IFX_E_READ_FAILED;
				nread = 0;
				break;
			} 
			else if (nread == 0)
				break; // EOF

			nleft -= nread;
		}

		*pNumberOfBytesRead = numberOfBytesToRead - nleft;
	}

	if( IFXSUCCESS( result ) && (numberOfBytesToRead != *pNumberOfBytesRead) )
	{
		// Check for read past end of file
		if (nread == 0)
		{
			// End of file
			result = IFX_W_FINISHED;
		}
		else
		{
			IFXTRACE_GENERIC(L"[IFXSocketStream] Invalid file\n");
			result = IFX_E_INVALID_FILE;
		}
	}

	return result;
}

IFXRESULT IFXSocketStream::SetOptions( U32 option, const I8* pBuffer, I32 bufferLength )
{
	return IFXOSSocketSetOptions( m_socket, option, (const char *)pBuffer, bufferLength );
}


IFXRESULT IFXSocketStream::GetOptions( U32 option, I8* pBuffer, I32* pBufferLength )
{
	return IFXOSSocketGetOptions( m_socket, option, (char *)pBuffer, pBufferLength );
}


IFXRESULT IFXSocketStream::Close()
{
	IFXRESULT result = IFX_OK;

	if( 0 != m_socket )
	{
		result = IFXOSSocketClose( m_socket );
		m_socket = 0;
	}

	return result;
}

//***************************************************************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************

//***************************************************************************
//	Global functions
//***************************************************************************


//***************************************************************************
//	Local functions
//***************************************************************************

