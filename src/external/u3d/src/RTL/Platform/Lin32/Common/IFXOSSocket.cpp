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
	@file	IFXOSSocket.cpp

			This module contains the plug-in specific functions required to
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXOSSocket.h"
#include "IFXOSUtilities.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

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
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Global functions
//***************************************************************************

extern "C"
I32 IFXAPI_CALLTYPE IFXOSSocketOpen()
{
	int	descriptor = socket( AF_INET, SOCK_STREAM, 0 );
	
	if( -1 == descriptor )
		IFXTRACE_GENERIC(L"[IFXOSSocket] Failed to open socket\n");
	
	return descriptor;
}


extern "C" 
IFXRESULT IFXAPI_CALLTYPE IFXOSSocketClose( int descriptor )
{
	IFXRESULT result = IFX_OK;

	if( 0 != close( descriptor ) )
	{
		IFXTRACE_GENERIC(
			L"[IFXOSSocket] Failed to terminate use of socket library\n");
		result = IFX_E_UNDEFINED;
	}

	return result;
}

extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSSocketConnect( 
									int descriptor, 
									const char* pHost, 
									U16 port )
{
	IFXRESULT result = IFX_OK;
	struct sockaddr_in addr;

	if( NULL == pHost )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
	{
		memset( &addr, 0, sizeof( struct sockaddr_in ) );
		addr.sin_family = AF_INET;
		addr.sin_port = htons( port );

		struct hostent* pHostEnt = gethostbyname( pHost );

		if( NULL != pHostEnt )
		{
			memcpy( &addr.sin_addr, pHostEnt->h_addr_list[0] , sizeof( struct in_addr ) );
		}
		else
		{
			IFXTRACE_GENERIC(L"[IFXOSSocket] Host not found\n");
			result = IFX_E_UNDEFINED;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		if( -1 == connect( descriptor, (struct sockaddr*)&addr, sizeof( addr ) ) )
		{
			IFXTRACE_GENERIC(
				L"[IFXOSSocket] Failed to establish connection to socket\n");
			result = IFX_E_UNDEFINED;
		}
	}

	return result;
}

extern "C" int IFXAPI_CALLTYPE IFXOSSocketReceive ( 
										int descriptor, 
										char* pBuffer, 
										int nbytes, 
										int flags )
{
	return recv( descriptor, pBuffer, nbytes, flags );
}

extern "C" int IFXAPI_CALLTYPE IFXOSSocketSend ( 
										int descriptor, 
										const char* pBuffer, 
										int nbytes, 
										int flags )
{
	return send( descriptor, pBuffer, nbytes, flags );
}

extern "C" 
IFXRESULT IFXAPI_CALLTYPE IFXOSSocketSetOptions( 
										int descriptor, 
										U32 option, 
										const char* pBuffer, 
										int bufferLength )
{
	IFXRESULT result = IFX_OK;
	int opt = 0;

	switch( option )
	{
	case IFXOSSOCKET_SEND_TIMEOUT:
		opt = SO_SNDTIMEO;
		break;
	case IFXOSSOCKET_RECEIVE_TIMEOUT:
		opt = SO_RCVTIMEO;
		break;
	default:
		opt = 0;
	}

	if( 0 != setsockopt( descriptor, SOL_SOCKET, opt, pBuffer, bufferLength ) )
	{
		IFXTRACE_GENERIC(L"[IFXOSSocket] Failed to set socket option\n");
		result = IFX_E_UNDEFINED;
	}

	return result;
}

extern "C" 
IFXRESULT IFXAPI_CALLTYPE IFXOSSocketGetOptions( 
										int descriptor, 
										U32 option, 
										char* pBuffer, 
										int* pBufferLength )
{
	IFXRESULT result = IFX_OK;
	int opt = 0;

	switch( option )
	{
	case IFXOSSOCKET_SEND_TIMEOUT:
		opt = SO_SNDTIMEO;
		break;
	case IFXOSSOCKET_RECEIVE_TIMEOUT:
		opt = SO_RCVTIMEO;
		break;
	default:
		opt = 0;
	}

	if( 0 != getsockopt( descriptor, SOL_SOCKET, opt, pBuffer, (socklen_t*)pBufferLength ) )
	{
		IFXTRACE_GENERIC(L"[IFXOSSocket] Failed to get socket option\n");
		result = IFX_E_UNDEFINED;
	}

	return result;
}

//***************************************************************************
//	Local functions
//***************************************************************************

