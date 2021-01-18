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
@file IFXConnection.h

This header defines the IFXConnection interface and the
associated interface identifier.

@note The IFXConnection interface is supported by the CIFXConnector
component.  Its component id is named CID_IFXConnector.
*/


#ifndef IFXConnection_h
#define IFXConnection_h


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXUnknown.h"


//***************************************************************************
//  Interfaces and interface identifiers
//***************************************************************************


//---------------------------------------------------------------------------
/**
This interface is used to get a reference to one of the connection
server's interfaces, thus allowing the client to use it.  It's supported
by the CIFXConnector component.  It's expected to be used by client
components.

@note The associated IID is named IID_IFXConnection.

@note Client components will need to handle the possibility that the
connection server is no longer available.
*/
class IFXConnection : public IFXUnknown
{
public:

	//---------------------------------------------------------------------------
	/**
	Method used to obtain a reference to one of connection server's
	interfaces.  The interface handed back must be released and not retained
	other than for the immediate usage it's needed for.  This includes all
	interface references that were obtained by using its IFXAPI  QueryInterface 
	method.  Clients must only retain a reference to this IFXConnection
	interface for reference count cycles to be prevented.

	@param  rInterfaceId  Reference to the interface identifier supported
	by the connection server component, that a
	pointer to will be handed back if this method is
	successful.
	@param  ppInterface   Pointer to an interface pointer, that upon
	success will be initialized with a pointer to
	the requested interface supported by the
	connection server component.

	@return Upon success, IFX_OK is returned.  Otherwise, one of the
	following failures are returned:  IFX_E_CONNECTION_SEVERED.
	*/
	virtual IFXRESULT IFXAPI  GetReference(
		IFXREFIID rInterfaceId,
		void**    ppInterface ) = 0;
};

//---------------------------------------------------------------------------
/**
This IID identifies the IFXConnection interface.

@note The GUID string is {80C89A27-EDA5-4469-ACB2-2F37D15DC5A2}
*/
IFXDEFINE_GUID(IID_IFXConnection,
			   0x80c89a27, 0xeda5, 0x4469, 0xac, 0xb2, 0x2f, 0x37, 0xd1, 0x5d, 0xc5, 0xa2);


//***************************************************************************
//  Failure return codes
//***************************************************************************


/**
Connection to server was severed and a reference to it is no longer
available.

@todo: Migrate this generic error message to IFXResult.h to ensure the
value doesn't clash with others that are defined.
*/
#define IFX_E_CONNECTION_SEVERED        MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0022 )


#endif
