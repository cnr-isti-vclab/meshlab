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
	@file	IFXInet.h
			
			Declaration of IFXInet interface.
*/

#ifndef IFXInet_H
#define IFXInet_H

#include "IFXUnknown.h"
#include "IFXString.h"

// {0EFF91D9-11C2-4e44-B97D-9580BF8D07C5}
IFXDEFINE_GUID(IID_IFXInet,
0xeff91d9, 0x11c2, 0x4e44, 0xb9, 0x7d, 0x95, 0x80, 0xbf, 0x8d, 0x7, 0xc5);

/**
	This interface provides callback functionality for internet streaming.
*/
class IFXReadingCallback : virtual public IFXUnknown
{
public:
	/**
		Retrieves number of URL which might be read.

		@param	rUrlCount	Output parameter for number of URL.
	*/
	virtual IFXRESULT IFXAPI  GetURLCount(U32& rUrlCount) = 0;

	/**
		Retrieves particular URL from list.

		@param	url		URL index in list.

		@param	rUrl	URL string from list.
	*/
	virtual IFXRESULT IFXAPI  GetURL(U32 url, IFXString& rUrl) = 0;

	/**
		Accepts success if file was read successfully.

		@param	url		URL index in list.

		@param	pData	Pointer to the buffer where read data is located.

		@param	dataSize	Size of buffer in bytes.
	*/
	virtual IFXRESULT IFXAPI  AcceptSuccess(U32 url, void* pData, U32 dataSize) = 0;

	/**
		Accepts failure happened during file read.
	*/
	virtual IFXRESULT IFXAPI  AcceptFailure(IFXRESULT errorCode) = 0;
};

/**
	This interface is used to open and close files remotely thru Internet.

	@note	The associated IID is named IID_IFXInet.
*/
class IFXInet : virtual public IFXUnknown
{
public:
	/** 
		Retrieves a value that contains the time-out value to use for
		Internet connection requests. Units are in milliseconds.
	*/
	virtual IFXRESULT IFXAPI  GetConnectTimeout(U32& rConnectTimeout) = 0;

	/** 
		Sets a value that contains the time-out value to use for
		Internet connection requests. Units are in milliseconds. If a connection
		request takes longer than this time-out value, the request is canceled.
		When attempting to connect to multiple IP addresses for a single host
		(a multihome host), the timeout limit is cumulative for all of the IP
		addresses.
	*/
	virtual IFXRESULT IFXAPI  SetConnectTimeout(const U32 connectTimeout) = 0;

	/** 
		Retrieves a value that contains the time-out value to send a
		request. Units are in milliseconds. 
	*/
	virtual IFXRESULT IFXAPI  GetSendTimeout(U32& rSendTimeout) = 0;

	/** 
		Sets a value that contains the time-out value to send a
		request. Units are in milliseconds. If the send takes longer than this
		time-out value, the send is canceled. 
	*/
	virtual IFXRESULT IFXAPI  SetSendTimeout(const U32 sendTimeout) = 0;

	/**
		Retrieves a value that contains the time-out value, in
		milliseconds, to receive a response to a request.
	*/
	virtual IFXRESULT IFXAPI  GetReceiveTimeout(U32& rReceiveTimeout) = 0;

	/**
		Sets a value that contains the time-out value, in
		milliseconds, to receive a response to a request. If the response takes
		longer than this time-out value, the request is canceled.
	*/
	virtual IFXRESULT IFXAPI  SetReceiveTimeout(const U32 receiveTimeout) = 0;

	/**
		Initiates file reading thru Internet.

		@param	pCoreServices		IFXCoreServices interface which is used to 
									register Internet read task.

		@param	pReadingCallback	IFXReadingCallback interface which is used
									to get URL during reading and accept reading
									result.
	*/
	virtual IFXRESULT IFXAPI  InitiateRead(
									IFXCoreServices* pCoreServices, 
									IFXReadingCallback* pReadingCallback) = 0;
};

#endif
