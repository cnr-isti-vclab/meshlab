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
	@file	CIFXConnector.h

			This header defines the CIFXConnector component.
*/

#ifndef CIFXConnector_h
#define CIFXConnector_h


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXConnection.h"
#include "IFXConnectionServer.h"


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the implementation of a component that is used to establish 
	connections between one component and another when it's expected that 
	there will be a reference count cycle.  Such a possibility is prevented 
	by using an instance of the CIFXConnector component somewhere in the 
	cycle.

	@note	This component's id is named CID_IFXConnector.

	@note	This component supports the following interfaces.
			- IFXConnection
			- IFXConnectionServer
			- IFXUnknown

	@note	This component can be instantiated multiple times.  Each 
			instance represents another connection.

	@note	This component is used in the following manner:
			- It's created by a component (called the connection server) 
			which maintains a reference to it until the connection server is 
			destroyed.  Again, this can be any component involved in the 
			reference count cycle that's expected.
			- The IFXConnectionServer interface is used by the connection 
			server to specify a reference to its IFXUnknown interface.
			- Whenever references to any of the connection server's 
			interfaces are needed by subcomponents or other subsystems 
			(in other words, clients), they are handed a reference to the 
			Connector's IFXConnection interface.
			- Client components use the IFXConnection interface to get a 
			@e temporary reference to any of the real interfaces supported 
			by the connection server.  With them, client's may also use the 
 IFXAPI 			QueryInterface  method to obtain other real interfaces desired.  
			All such obtained interface references must be released and not 
			kept to prevent a reference count cycle.
			- Client components will need to handle the possibility that the 
			connection server is no longer available.  This is indicated by 
			the IFXConnection::GetReference function returning 
			IFX_E_CONNECTION_SEVERED.
			- In the connection server component's destructor, its 
			Connector's IFXConnectionServer interface is used to clear the 
			IFXUnknown interface reference previously specified.  The 
			connection server then releases all of its references to its 
			Connector component.
*/
class CIFXConnector : public IFXConnection, public IFXConnectionServer
{

		// Member functions:  IFXUnknown.

		U32 IFXAPI 			AddRef ();
		U32 IFXAPI 			Release ();
		IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

		// Member functions:  IFXConnection.

		IFXRESULT	IFXAPI   GetReference(
											IFXREFIID	rInterfaceId,
											void**		ppInterface );

		// Member functions:  IFXConnectionServer.

		IFXRESULT	IFXAPI   SetReference( IFXUnknown* pUnknown );

		// Factory function.

		friend IFXRESULT IFXAPI_CALLTYPE CIFXConnector_Factory( IFXREFIID interfaceId, void** ppInterface );

	private:

		// Member functions.

				CIFXConnector();
		virtual	~CIFXConnector();

		// Member data.

		U32			m_refCount;			///< Number of outstanding references to the component.
		IFXUnknown	*m_pServerUnknown;	///< Pointer to the connection server's IFXUnknown interface.  This is a weak reference and isn't IFXAPI  AddRef 'd, so never Release it.
};


#endif


