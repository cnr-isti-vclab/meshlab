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
	@file	IFXInternetConnectionX.cpp

			This module contains the implementation of internet connection
			functionality.
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXInternetConnectionX.h"
#include "IFXSocketStream.h"

#include "IFXCheckX.h"

#include <string.h>
#include <stdlib.h>

//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************

const U16 DEFAULT_HTTP_PORT = 80;
const U16 DEFAULT_FTP_PORT = 21;

const char HTTP_SCHEMA[] = "http";
const char FTP_SCHEMA[] = "ftp";

const char DEFAULT_FTP_USER[] = "anonymous";
const char DEFAULT_FTP_PASSWORD[] = "u3d@u3d";
const I32 RESPONSE_BUFFER_SIZE = 255;

const char REQUEST_TERMINATOR[] = "\r\n";
const U32 REQUEST_TERMINATOR_LENGTH = strlen(REQUEST_TERMINATOR);

const char HTTP_GET[] = "GET ";

// FTP commands
const char FTP_RETR[] = "RETR ";
const char FTP_USER[] = "USER ";
const char FTP_PASS[] = "PASS ";
const char FTP_TYPE[] = "TYPE ";
const char FTP_BINARY[] = "I";
const char FTP_PASV[] = "PASV";
const char FTP_QUIT[] = "QUIT";

// FTP command responses
const char ftpServiceOK[] = "220";
const char ftpUserOK[] = "331";
const char ftpPassOK[] = "230";
const char ftpOK[] = "200";
const char ftpPasvOK[] = "227";
const char ftpTransferStarted[] = "125";
const char ftpTransferComplete[] = "226";

//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************

/**
	Defines HTTP internet connection functionality.
*/
class IFXHTTPConnectionX : public IFXInternetConnectionX
{
public:
	IFXHTTPConnectionX();
	virtual	~IFXHTTPConnectionX();

	virtual void InitializeX( 
					const I8* pHost, U16& rPort, 
					const I8* pUser, const I8* pPassword );

	virtual void OpenFileX( const I8* pFilePath );
};

/**
	Defines HTTP internet connection functionality.
*/
class IFXFTPConnectionX : public IFXInternetConnectionX
{
public:
	IFXFTPConnectionX();
	virtual ~IFXFTPConnectionX();

	virtual void InitializeX( 
					const I8* pHost, U16& rPort, 
					const I8* pUser, const I8* pPassword );

	virtual void OpenFileX( const I8* pFilePath );

	virtual IFXRESULT Close();
	virtual IFXSocketStream* GetStream() const;

private:
	IFXSocketStream* m_pDataStream;
	const I8* m_pHost;

	void DoAuthorizationX( const I8* pUserName, const I8* pPassword );
	IFXRESULT CheckResponse( const I8* pResponseCode, I8* pResponse = NULL );

	void CreateDataConnectionX();
	void CheckFileRequestResponseX();
};

//***************************************************************************
//	Local function prototypes
//***************************************************************************

IFXRESULT ParseFtpPasvResponse( const I8* pPasvResponce, U16& rFtpDataPort );

//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************

IFXInternetConnectionX::IFXInternetConnectionX() : m_pStream( NULL )
{
}

IFXInternetConnectionX::~IFXInternetConnectionX()
{
	Close();
	IFXDELETE( m_pStream );
}

IFXRESULT IFXInternetConnectionX::Close()
{
	IFXRESULT result = IFX_OK;

	if( NULL != m_pStream )
		m_pStream->Close();

	return result;
}

void IFXInternetConnectionX::InitializeX( 
								const I8* pHost, U16& rPort, 
								const I8* pUser, const I8* pPassword )
{
	IFXCHECKX_RESULT( NULL != pHost, IFX_E_INVALID_POINTER );

	try
	{
		m_pStream = new IFXSocketStream;
		IFXCHECKX_RESULT( NULL != m_pStream, IFX_E_OUT_OF_MEMORY );

		IFXCHECKX( m_pStream->Open() );
		IFXCHECKX( m_pStream->Connect( pHost, rPort ) );
	}
	catch( IFXException& rException ) 
	{
		IFXDELETE( m_pStream );
		throw rException;
	}
}

IFXSocketStream* IFXInternetConnectionX::GetStream() const
{
	return m_pStream;
}

IFXHTTPConnectionX::IFXHTTPConnectionX()
{
}

IFXHTTPConnectionX::~IFXHTTPConnectionX()
{
}

void IFXHTTPConnectionX::InitializeX(
								const I8* pHost, U16& rPort, 
								const I8* pUser, const I8* pPassword )
{
	if( 0 == rPort )
	{
		// port was not set up by client, use default port number
		rPort = DEFAULT_HTTP_PORT;
	}

	IFXInternetConnectionX::InitializeX( pHost, rPort, pUser, pPassword );
}

void IFXHTTPConnectionX::OpenFileX( const I8* pFilePath )
{
	IFXCHECKX_RESULT( NULL != pFilePath, IFX_E_INVALID_POINTER );

	IFXCHECKX( SendRequest( (const I8*)HTTP_GET, (const I8*)pFilePath ) );
}

IFXFTPConnectionX::IFXFTPConnectionX() : m_pDataStream( NULL )
{
}

IFXFTPConnectionX::~IFXFTPConnectionX()
{
	Close();
	IFXDELETE( m_pDataStream );
}

IFXRESULT IFXFTPConnectionX::Close()
{
	IFXRESULT result = IFX_OK;

	if( NULL != m_pDataStream )
		result = m_pDataStream->Close();

	return result;
}

void IFXFTPConnectionX::InitializeX(
								const I8* pHost, U16& rPort, 
								const I8* pUser, const I8* pPassword )
{
	if( 0 == rPort )
		rPort = DEFAULT_FTP_PORT;

	IFXInternetConnectionX::InitializeX( pHost, rPort, pUser, pPassword );

	m_pHost = pHost;

	if( NULL == pUser || NULL == pPassword )
		DoAuthorizationX( (const I8*)DEFAULT_FTP_USER, (const I8*)DEFAULT_FTP_PASSWORD );
	else
		DoAuthorizationX( pUser, pPassword );
}

void IFXFTPConnectionX::OpenFileX( const I8* pFilePath )
{
	IFXCHECKX_RESULT( NULL != pFilePath, IFX_E_INVALID_POINTER );

	CreateDataConnectionX(); 

	IFXCHECKX( SendRequest( (const I8*)FTP_RETR, pFilePath ) );

	CheckFileRequestResponseX();
}

IFXSocketStream* IFXFTPConnectionX::GetStream() const
{
	return m_pDataStream;
}

//***************************************************************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************

IFXRESULT IFXInternetConnectionX::SendRequest( 
										const I8* pCommand, const I8* pParameter )
{
	IFXRESULT result = IFX_OK;

	I8* pRequest = NULL;
	U32 commandLen = strlen((const char*)pCommand);
	U32 parameterLen = strlen((const char*)pParameter);
	U32 requestLen = commandLen + parameterLen + REQUEST_TERMINATOR_LENGTH;

	try
	{
		pRequest = new I8[requestLen+1];
		IFXCHECKX_RESULT( NULL != pRequest, IFX_E_OUT_OF_MEMORY );
		memset( pRequest, 0, requestLen+1 );

		strncpy( (char*)pRequest, (const char*)pCommand, requestLen );
		strncat( (char*)pRequest, (const char*)pParameter, parameterLen );
		strncat( (char*)pRequest, REQUEST_TERMINATOR, REQUEST_TERMINATOR_LENGTH );

		IFXTRACE_GENERIC(L"[IFXInternetConnectionX::SendRequest] %hs\n", pRequest);
		result = m_pStream->SendBytes( pRequest );

		IFXDELETE_ARRAY( pRequest );
	}
	catch( IFXException& rException ) 
	{
		IFXDELETE_ARRAY( pRequest );
		result = rException.GetIFXResult();
	}

	return result;
}


void IFXFTPConnectionX::DoAuthorizationX( 
							 const I8* pUserName, 
							 const I8* pPassword )
{
	IFXCHECKX( CheckResponse( (const I8*)ftpServiceOK ) );

	IFXCHECKX( SendRequest( (const I8*)FTP_USER, pUserName ) );
	IFXCHECKX( CheckResponse( (const I8*)ftpUserOK ) );

	IFXCHECKX( SendRequest( (const I8*)FTP_PASS, pPassword ) );
	IFXCHECKX( CheckResponse( (const I8*)ftpPassOK ) );

	IFXCHECKX( SendRequest( (const I8*)FTP_TYPE, (const I8*)FTP_BINARY ) );
	IFXCHECKX( CheckResponse( (const I8*)ftpOK ) );
}

void IFXFTPConnectionX::CheckFileRequestResponseX()
{
	IFXCHECKX( CheckResponse( (const I8*)ftpTransferStarted ) );
	IFXCHECKX( SendRequest( (const I8*)FTP_QUIT, (const I8*)"" ) );

	// close ftp control connection since 
	// we do not need it anymore 
	m_pStream->Close();
	IFXDELETE( m_pStream );
}

IFXRESULT IFXFTPConnectionX::CheckResponse( const I8* pResponseCode, I8* pResponse )
{
	IFXRESULT result = IFX_E_NOT_FOUND;

	I8 pBuffer[RESPONSE_BUFFER_SIZE+1] = {0};
	I32 nread = 0;

	// receive data till nothing to read
	while( 0 != ( nread = m_pStream->Receive( pBuffer, RESPONSE_BUFFER_SIZE ) ) ) 
	{
		if( result == IFX_E_NOT_FOUND )
		{
			pBuffer[nread] = 0; // null terminator

			if( NULL != strstr( (const char*)pBuffer, (const char*)pResponseCode ) )
				result = IFX_OK;

			if( nread < RESPONSE_BUFFER_SIZE )
				break;
		}
	}

	IFXTRACE_GENERIC(L"[IFXFTPConnectionX::CheckResponse] %hs\n", pBuffer);

	if( NULL != pResponse )
		strncpy( (char*)pResponse, (const char*)pBuffer, nread );

	return result;
}

void IFXFTPConnectionX::CreateDataConnectionX()
{
	U16 ftpDataPort = 0;
	I8 pasvResponse[RESPONSE_BUFFER_SIZE+1] = {0};

	IFXCHECKX( SendRequest( (const I8*)FTP_PASV, (const I8*)"" ) );
	IFXCHECKX( CheckResponse( (const I8*)ftpPasvOK, pasvResponse ) );

	// parse ftp server data port
	IFXCHECKX( ParseFtpPasvResponse( pasvResponse, ftpDataPort ) );

	try
	{
		m_pDataStream = new IFXSocketStream;
		IFXCHECKX_RESULT( NULL != m_pDataStream, IFX_E_OUT_OF_MEMORY );

		// create ftp data connection
		IFXCHECKX( m_pDataStream->Open() );
		IFXCHECKX( m_pDataStream->Connect( m_pHost, ftpDataPort ) );
	}
	catch( IFXException& rException ) 
	{
		IFXDELETE( m_pDataStream );
		throw rException;
	}
}

//***************************************************************************
//	Global functions
//***************************************************************************


//***************************************************************************
//	Local functions
//***************************************************************************

IFXInternetConnectionX* MakeInternetConnectionX( const I8* pSchema )
{
	IFXInternetConnectionX* pConnection = NULL;

	if ( 0 == strcmp( (const char*)pSchema, FTP_SCHEMA ) )
	{
		pConnection = new IFXFTPConnectionX;
		IFXCHECKX_RESULT( NULL != pConnection, IFX_E_OUT_OF_MEMORY );
	}
	else if( 0 == strcmp( (const char*)pSchema, HTTP_SCHEMA ) )
	{
		pConnection = new IFXHTTPConnectionX;
		IFXCHECKX_RESULT( NULL != pConnection, IFX_E_OUT_OF_MEMORY );
	}
	else
	{
		pConnection = NULL;
		IFXTRACE_GENERIC(L"[MakeInternetConnection] Unsupported schema\n");
		IFXCHECKX( IFX_E_UNSUPPORTED );
	}

	return pConnection;
}

/* 
	parse ftp server data port for data connection
	
	PASV response format:
	(a1,a2,a3,a4,p1,p2), where p1 and p2 octets to form port number
*/
IFXRESULT ParseFtpPasvResponse( const I8* pPasvResponce, U16& rFtpDataPort )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT( pPasvResponce != NULL );

	// look for 4th comma (5th octet)
	I8* pPtrHi = (I8*)strchr( (const char*)pPasvResponce, ',' );

	U32 i;
	for( i = 0; i < 3; ++i )
		pPtrHi = (I8*)strchr( (const char*)pPtrHi+1, ',' );
	pPtrHi++;

	// look for next comma (6th octet)
	I8* pPtrLo = (I8*)strchr( (const char*)pPtrHi, ',' );
	pPtrLo++;

	// look for end
	I8* pPtrEnd = (I8*)strchr( (const char*)pPtrLo, ')' );

	I8 port_lo[4] = {0};
	I8 port_hi[4] = {0};

	UPTR digitCount = pPtrLo - pPtrHi - 1;

	if( digitCount <= 3 )
	{
		strncpy( (char *)port_hi, (const char *)pPtrHi, digitCount );
		port_hi[digitCount] = 0;

		rFtpDataPort = atoi( (const char *)port_hi ) * 256;

		digitCount = pPtrEnd - pPtrLo;

		if( digitCount <= 3 )
		{
			strncpy( (char *)port_lo, (const char *)pPtrLo, digitCount );
			port_lo[digitCount] = 0;

			rFtpDataPort += atoi( (const char *)port_lo );
		}
		else
		{
			IFXTRACE_GENERIC(L"[IFXSocketStream] Invalid ftp server data port\n");
			result = IFX_E_UNDEFINED;
		}
	}
	else
	{
		IFXTRACE_GENERIC(L"[IFXSocketStream] Invalid ftp server data port\n");
		result = IFX_E_UNDEFINED;
	}

	return result;
}
