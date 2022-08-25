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
	@file	IFXInternetSessionX.h

			This header defines the internet session functionality.
*/


#ifndef IFXInternetSessionX_H
#define IFXInternetSessionX_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXSocketStream.h"

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

class IFXInternetConnectionX;

/**
	This class is used to encapsulate Internet session operations.
	Only HTTP and FTP are supported.
*/
class IFXInternetSessionX
{
public:

	IFXInternetSessionX();
	~IFXInternetSessionX();

	/**
		Closes internet session
	*/
	IFXRESULT Close();

	/**
		Opens internet session with file which location defined by URL.

		@param	pUrl	URL to read (null terminated char buffer).
	*/
	void OpenX( I8* pUrl );

	/**
		Opens internet session with file which location defined by URL.

		@param	pUrl	URL to read (null terminated wide-char buffer).
	*/
	void OpenX(const IFXCHAR* pUrl );

	/**
		Reads data from file to buffer

		@param pBuffer				Data buffer preallocated by client.

		@param numberOfBytesToRead	Number of bytes to read.

		@param pNumberOfBytesRead	Pointer to number of bytes actually read.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
				following failures are returned:  IFX_E_NOT_INITIALIZED.
	*/
	IFXRESULT Read( U8* pBuffer, U32 numberOfBytesToRead, U32* pNumberOfBytesRead );

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


private:
	IFXInternetConnectionX* m_pConnection;
};

#endif
