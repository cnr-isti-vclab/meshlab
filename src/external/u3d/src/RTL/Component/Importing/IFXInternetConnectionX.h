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
	@file	IFXInternetConnectionX.h

			This header defines the internet connection functionality.
*/


#ifndef IFXInternetConnectionX_H
#define IFXInternetConnectionX_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "IFXResult.h"

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

class IFXSocketStream;

/**
	Creates abstraction layer on internet protocols.
	HTTP and FTP are supported now.
*/
class IFXInternetConnectionX
{
public:
	IFXInternetConnectionX();
	virtual ~IFXInternetConnectionX();

	/**
		Initialize internet connection.

		@param	pHost	Host address (null terminated char buffer).

		@param	rPort	Host port to connect (null terminated char buffer).

		@param	pUser	User name (null terminated char buffer).

		@param	pPassword	Use password (null terminated char buffer).
	*/
	virtual void InitializeX( 
					const I8* pHost, U16& rPort, 
					const I8* pUser, const I8* pPassword );

	/**
		Close internet connection.
	*/
	virtual IFXRESULT Close();

	/**
		Opens file and prepare it for reading.

		@param	pFile	File to be read (null terminated char buffer).
	*/
	virtual void OpenFileX( const I8* pFilePath ) = 0;

	/**
		Provide internet connection stream for client.

		@return	Socket which is used by this internet connection.
	*/
	virtual IFXSocketStream* GetStream() const;

protected:
	/**
		Sends request to server.

		@param	pCommand	Command to be send (null terminated char buffer).

		@param	pParameter	Command's parameter (null terminated char buffer).
	*/
	IFXRESULT SendRequest( const I8* pCommand, const I8* pParameter );

	IFXSocketStream* m_pStream; ///< Socket used by this internet connection.
};

//***************************************************************************
//	Global function prototypes
//***************************************************************************

/**
	Factory function for internet connection.

	@param	pSchema	Defines type of internet connection.
*/
IFXInternetConnectionX* MakeInternetConnectionX( const I8* pSchema );

#endif
