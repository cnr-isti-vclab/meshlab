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
	@file	CIFXInternetReadBufferX.h

			Declaration of the CIFXInternetReadBufferX class which implements the
			IFXReadBuffer, IFXReadBufferX and IFXInternet interfaces.
*/

#ifndef CIFXINTERNETREADBUFFERX_H
#define CIFXINTERNETREADBUFFERX_H

#include "IFXCoreServices.h"
#include "IFXReadBuffer.h"
#include "IFXReadBufferX.h"
#include "IFXStdio.h"
#include "IFXAutoRelease.h"
#include "IFXTask.h"

#include "IFXInet.h"
#include "IFXInternetSessionX.h"

class CIFXInternetReadBufferX : public IFXReadBuffer, 
								public IFXReadBufferX, 
								public IFXStdio, 
								public IFXInet, 
								public IFXTask
{
public:
	// IFXUnknown
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXReadBuffer
	virtual IFXRESULT IFXAPI  Read( U8* pBytes, U64 position, U32 count );
	virtual IFXRESULT IFXAPI  GetTotalSize( U64* pCount );
	virtual IFXRESULT IFXAPI  GetAvailableSize( U64* pCount );

	// IFXReadBufferX
	virtual void IFXAPI  ReadX( 
								U8* pBytes, U64 position, U32 count, 
								IFXRESULT& rWarningCode );
	virtual void IFXAPI  GetTotalSizeX( U64& rCount );
	virtual void IFXAPI  GetAvailableSizeX( U64& rCount );

	// IFXStdio
	virtual IFXRESULT IFXAPI  Open(const IFXCHAR *pUrl);
	virtual IFXRESULT IFXAPI  Close();

	// IFXInet
	virtual IFXRESULT IFXAPI  GetConnectTimeout(U32& rConnectTimeout);
	virtual IFXRESULT IFXAPI  SetConnectTimeout(const U32 connectTimeout);
	virtual IFXRESULT IFXAPI  GetSendTimeout(U32& rSendTimeout);
	virtual IFXRESULT IFXAPI  SetSendTimeout(const U32 sendTimeout);
	virtual IFXRESULT IFXAPI  GetReceiveTimeout(U32& rReceiveTimeout);
	virtual IFXRESULT IFXAPI  SetReceiveTimeout(const U32 receiveTimeout);
	virtual IFXRESULT IFXAPI  InitiateRead(
									IFXCoreServices* pCoreServices, 
									IFXReadingCallback* pReadingCallback);

	// IFXTask
	virtual IFXRESULT IFXAPI  Execute(IFXTaskData* pTaskData);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXInternetReadBufferX_Factory( 
											IFXREFIID interfaceId, 
											void**	ppInterface );

private:
	// Private to enforce the use of the create instance method
	CIFXInternetReadBufferX();
	virtual ~CIFXInternetReadBufferX();

	// PRIVATE MEMBER VARIABLES
	U32		m_refCount;		// Reference counter
	IFXCoreServices* m_pCoreServices;

	U64	m_readPosition;

	// below members only for extecute and initializeread methods - review usage
	IFXTaskHandle m_readTaskHandle;
	U32 m_URLs, m_currentURL;
	IFXDECLAREMEMBER(IFXReadingCallback, m_pReadingCallback);
	IFXDECLAREMEMBER(IFXReadBuffer, m_pReadBuffer);

	IFXInternetSessionX m_session;
};

#endif
