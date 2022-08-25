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
	@file	IFXInternetSessionX.cpp

			This module contains the implementation of internet session
			functionality.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include <string.h>
#include <stdlib.h>

#include "IFXOSUtilities.h"
#include "IFXInternetSessionX.h"
#include "IFXInternetConnectionX.h"

#include "IFXCheckX.h"

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************

const I8 SCHEMA_TERMINATOR[] = "://";
const I8 PASSWORD_TERMINATOR = '@';
const I8 PORT_DELIMITER = ':';
const I8 USER_DELIMITER = ':';
const I8 HOST_TERMINATOR = '/';
const U32 MAX_PORT_DIGIT_COUNT = 5;
const U32 MAX_SCHEMA_LEN = 7;

//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************

struct UrlParseData
{
	I8* pSchema;
	I8* pUser;
	I8* pPassword;
	I8* pHost;
	I32 hostLen;
	U16 port;
	I8* pFilePath;
};


//***************************************************************************
//	Local function prototypes
//***************************************************************************

IFXRESULT ParseURL(
				I8* pUrl,
				struct UrlParseData& rParseData );

//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************

IFXInternetSessionX::IFXInternetSessionX() : m_pConnection( NULL )
{
}


IFXInternetSessionX::~IFXInternetSessionX()
{
	Close();
	IFXDELETE( m_pConnection );
}


IFXRESULT IFXInternetSessionX::Close()
{
	IFXRESULT result = IFX_OK;

	if( NULL != m_pConnection )
		m_pConnection->Close();

	return result;
}


void IFXInternetSessionX::OpenX(const IFXCHAR* pUrl )
{
	IFXCHECKX_RESULT( NULL != pUrl, IFX_E_INVALID_POINTER );

	I8* pUrlStr = NULL;
	U32 urlLen = 0;

	// convert path from wide char to ascii (no unicode symbols in the URL!)

	IFXCHECKX( IFXOSGetUtf8StrSize( pUrl, &urlLen ) );

	try
	{
		pUrlStr = new I8[urlLen+1];

		IFXCHECKX_RESULT( NULL != pUrlStr, IFX_E_OUT_OF_MEMORY );

		IFXCHECKX( IFXOSConvertWideCharStrToUtf8( pUrl, (U8*)pUrlStr, urlLen+1 ) );
		pUrlStr[urlLen] = 0;

		// path converted to ascii, open
		OpenX( pUrlStr );

		IFXDELETE_ARRAY( pUrlStr );
	}
	catch( IFXException& rException ) 
	{
		IFXDELETE_ARRAY( pUrlStr );
		throw rException;
	}
}


void IFXInternetSessionX::OpenX( I8* pUrl )
{
	I8* pUrlCopy = NULL;
	U32 urlLen = strlen((const char*)pUrl);
	struct UrlParseData parseData;
	memset( &parseData, 0, sizeof(UrlParseData) );

	I8* pHost = NULL;

	try
	{
		pUrlCopy = new I8[urlLen+1];
		strncpy( (char *)pUrlCopy, (const char *)pUrl, urlLen );
		pUrlCopy[urlLen] = 0;

		IFXCHECKX( ParseURL( pUrlCopy, parseData ) );

		// set host address
		if( parseData.hostLen > 0 )
		{
			pHost = new I8[parseData.hostLen+1];
			IFXCHECKX_RESULT( NULL != pHost, IFX_E_OUT_OF_MEMORY );

			strncpy( (char *)pHost, (char *)parseData.pHost, parseData.hostLen );
			pHost[parseData.hostLen] = 0;
		}

		m_pConnection = MakeInternetConnectionX( parseData.pSchema );

		m_pConnection->InitializeX( 
							pHost, parseData.port, 
							parseData.pUser, parseData.pPassword );
		m_pConnection->OpenFileX( parseData.pFilePath );

		IFXDELETE_ARRAY( pHost );
		IFXDELETE_ARRAY( pUrlCopy );
	}
	catch( IFXException& rException ) 
	{
		IFXDELETE_ARRAY( pHost );
		IFXDELETE_ARRAY( pUrlCopy );
		throw rException;
	}
}


IFXRESULT IFXInternetSessionX::Read( 
								U8* pBuffer, 
								U32 numberOfBytesToRead, 
								U32* pNumberOfBytesRead )
{
	IFXRESULT result = IFX_OK;
	IFXSocketStream* pStream = m_pConnection->GetStream();

	if( NULL != pStream )
		result = pStream->ReadBytes( pBuffer, numberOfBytesToRead, pNumberOfBytesRead );
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT IFXInternetSessionX::SetOptions( 
									  U32 option, 
									  const I8* pBuffer, 
									  I32 bufferLength )
{
	IFXRESULT result = IFX_OK;
	IFXSocketStream* pStream = m_pConnection->GetStream();

	if( NULL != pStream )
		result = pStream->SetOptions( option, pBuffer, bufferLength );
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT IFXInternetSessionX::GetOptions( 
									  U32 option, 
									  I8* pBuffer, 
									  I32* pBufferLength )
{
	IFXRESULT result = IFX_OK;
	IFXSocketStream* pStream = m_pConnection->GetStream();

	if( NULL != pStream )
		result = pStream->GetOptions( option, pBuffer, pBufferLength );
	else
		result = IFX_E_NOT_INITIALIZED;

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

/*
	schema://user:password@host:port/path/filename.u3d

	example:
		ftp://anonymous:u3d@u3d@localhost:21/files/metadata.u3d
*/
IFXRESULT ParseURL(
				I8* pUrl,
				struct UrlParseData& rParseData )
{
#ifdef _DEBUG
	size_t urlLen = strlen((const char*)pUrl);
#endif
	IFXRESULT result = IFX_OK;

	// check with assert only because it's private method
	IFXASSERT( pUrl != NULL );

	I8* pHost = NULL;
	I8* pPtr = NULL;
	UPTR len = 0;

	// parse schema
	pPtr = (I8*)strstr( (const char*)pUrl, (const char*)SCHEMA_TERMINATOR ); 
	// pPtr = "://anonymous:u3d@u3d@localhost:21/files/metadata.u3d"

	if( NULL == pPtr )
	{
		IFXTRACE_GENERIC(L"[ParseURL] Schema not found\n");
		result = IFX_E_UNDEFINED;
	}

	if( IFXSUCCESS( result ) )
	{
		len = pPtr - pUrl;
		IFXASSERT(len < urlLen);
		pUrl[len] = 0;
		rParseData.pSchema = pUrl; // ftp

		pPtr += strlen((const char*)SCHEMA_TERMINATOR); 
		// pPtr = "anonymous:u3d@u3d@localhost:21/files/metadata.u3d" OR
		// pPtr = "localhost:21/files/metadata.u3d" OR

		pHost = (I8*)strrchr( (const char*)pPtr, PASSWORD_TERMINATOR );
		// pHost = "@localhost:21/files/metadata.u3d" or NULL
	
		if( NULL == pHost )
		{
			// user and password not found, default should be used
			pHost = pPtr;
			// pHost = "localhost:21/files/metadata.u3d"
		}
		else
		{
			len = pHost - pPtr;
			IFXASSERT(len < urlLen);
			pPtr[len] = 0;
			// pPtr = "anonymous:u3d@u3d"

			// parse user and password
			I8* pPassword = (I8*)strchr( (const char*)pPtr, USER_DELIMITER );
			// pPassword = ":u3d@u3d" OR NULL

			if( NULL != pPassword )
			{
				len = pPassword - pPtr;
				IFXASSERT(len < urlLen);
				pPtr[len] = 0;
				rParseData.pUser = pPtr;

				pPassword++; // pass ':'

				len = pHost - pPassword;
				
				// don't set password ptr it's exist in URL
				if( len > 0 )
					rParseData.pPassword = pPassword;
			}
			else
			{
				// in case of there is no password and ':'
				rParseData.pUser = pPtr;
			}

			pHost++; // pass '@'
			// pHost = "localhost:21/files/metadata.u3d"
		}
	}

	if( IFXSUCCESS( result ) )
	{
		// look for port delimiter
		pPtr = (I8*)strchr( (char *)pHost, PORT_DELIMITER );

		// look for host name terminator
		I8* pFilePath = (I8*)strchr( (char *)pHost, HOST_TERMINATOR );
		
		if( NULL == pFilePath )
		{
			IFXTRACE_GENERIC(L"[ParseURL] Host name not found\n");
			result = IFX_E_UNDEFINED;
		}
		else
		{
			if( NULL != pPtr )
			{
				len = pPtr - pHost;
				IFXASSERT(len < urlLen);
				pHost[len] = 0;

				// parse port number

				I8 port[MAX_PORT_DIGIT_COUNT+1] = {0};

				pPtr = pPtr + 1; // omit port delimiter
				UPTR digitCount = pFilePath - pPtr;

				if( digitCount <= MAX_PORT_DIGIT_COUNT )
				{
					strncpy( (char *)port, (const char*)pPtr, digitCount );
					port[digitCount] = 0;

					rParseData.port = atoi( (const char*)port );
				}
				else
				{
					IFXTRACE_GENERIC(L"[ParseURL] Invalid port\n");
					result = IFX_E_UNDEFINED;
				}
			}
			else
			{
				len = pFilePath - pHost;
				IFXASSERT(len < urlLen);
			}

			rParseData.hostLen = len;
			rParseData.pHost = pHost;
			rParseData.pFilePath = pFilePath;
		}
	}

	return result;
}
