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
	@file	IFXConnectionServer.h

			This header defines the IFXConnectionServer interface and the 
			associated interface identifier.

	@note	The IFXConnectionServer interface is supported by the 
			CIFXConnector component.  Its component id is named 
			CID_IFXConnector.
*/


#ifndef IFXConnectionServer_h
#define IFXConnectionServer_h


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXUnknown.h"


//***************************************************************************
//	Interfaces and interface identifiers
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This interface is used by the connection server to manage connections 
	with its clients in such a fashion that reference count cycles are 
	prevented, system robustness isn't compromised and clients don't have to 
	be aware of dangerous IFXAPI  AddRef  and Release rule exceptions.  It's 
	supported by the CIFXConnector component.  It's expected to be used by 
	connection server components that wish to prevent reference count cycles.

	@note	The associated IID is named IID_IFXConnectionServer.

	@note	This interface only manages a single component connection.
*/
class IFXConnectionServer : public IFXUnknown
{
	public:

		//---------------------------------------------------------------------------
		/**
			Method used to establish a connection to the connection server's 
			IFXUnknown interface.  Before the connection server (the component 
			instance that supports the IFXUnknown interface) is destroyed, this 
			method must be called again to disconnect it by specifying a NULL
			reference.

			@param	pUnknown	Pointer to the IFXUnknown interface of a component 
								to establish a connection with.  Specify a NULL 
								pointer to disconnect a component.

			@return	Always succeeds and returns IFX_OK.
		*/
		virtual	IFXRESULT IFXAPI 	SetReference( IFXUnknown* pUnknown ) = 0;
};

//---------------------------------------------------------------------------
/**
	This IID identifies the IFXConnectionServer interface.

	@note	The GUID string is {F8E071BB-3B56-4747-B3E8-E5297D1216D9}
*/
IFXDEFINE_GUID(IID_IFXConnectionServer, 
0xf8e071bb, 0x3b56, 0x4747, 0xb3, 0xe8, 0xe5, 0x29, 0x7d, 0x12, 0x16, 0xd9);


#endif


