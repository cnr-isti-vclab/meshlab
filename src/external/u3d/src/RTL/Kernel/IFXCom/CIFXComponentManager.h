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
	@file	CIFXComponentManager.h

			This module declares the CIFXComponentManager class. It is used to
			manage all operations with components and component database.
	@note	
*/
//***************************************************************************

#ifndef __CIFXComponentManager_H__
#define __CIFXComponentManager_H__

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXGUID.h"
#include "IFXArray.h"

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

class CIFXGuidHashMap;
class CIFXPluginProxy;
struct IFXComponentDescriptor;

/**
	This class manages all operations with components (creation, registration),
	and component database (add).

	@note
 */
class CIFXComponentManager
{
public:
	CIFXComponentManager();
	virtual ~CIFXComponentManager();

	// Com Like ref counting support
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();

	/**
		Initializes component database and registers core components

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
				IFX_E_OUT_OF_MEMORY - not enough memory to initialize component
									  database
	 */
	IFXRESULT Initialize();

	/**
		Registers component

		In the case of component being registered already registered the 
		component database registers new component only if it has newer 
		version

		@param  pComponentDescriptor Pointer to a component descriptor which
				is registered.

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
				IFX_E_NOT_INITIALIZED - component manager was not properly 
										initialized
				IFX_E_UNDEFINED - component database error
	 */
	IFXRESULT RegisterComponent ( 
				const IFXComponentDescriptor* pComponentDescriptor);

	/**
	Creates component

	@param	rComponentId	Reference to the identifier of the component to be
						created.
	@param	rInterfaceId	Reference to the interface identifier supported by
						the component, that a pointer to will be handed back
						if this method is successful.
	@param	ppInterface	Pointer to an interface pointer, that upon success
						will be initialized with a pointer to the requested
						interface supported by the newly created component.

	@return	Upon success, it returns the value IFX_OK.  Otherwise, it will
			return one of the following values:  IFX_E_COMPONENT,
			IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or IFX_E_UNSUPPORTED.
			It's also possible for component specific construction values to
			be returned.
	 */
	IFXRESULT CreateComponent(  const IFXCID& rComponentId, 
								const IFXIID& rInterfaceId, 
								void** ppInterface);

	/**
  		Release  all plug-ins

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
				IFX_W_CANNOT_UNLOAD - this warning means that not all components
									were released and some plug-ins cannot be
									unloaded.
	 */
	IFXRESULT UnloadAllPlugins();

	/**
		Return DIDs which were extracted from plugins.
	*/
	IFXArray<IFXDID*> *GetPluginsDids();

protected:
	/**
		Find plug-in modules

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
				IFX_E_INVALID_RANGE
				IFX_E_OUT_OF_MEMORY
	 */
	IFXRESULT FindPlugins();

	/**
		Register plug-in components

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
				IFX_E_NOT_INITIALIZED
				IFX_E_UNDEFINED
	 */
	IFXRESULT RegisterPlugins();

private:
	U32 m_refCount;

	CIFXPluginProxy* m_pPluginProxyList;

	U32				 m_pluginNumber;

	CIFXGuidHashMap* m_pGuidHashMap;

	IFXArray<IFXDID*> *m_pDidsList;
};


//***************************************************************************
//	Interfaces and interface identifiers
//***************************************************************************


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Inline functions
//***************************************************************************


//***************************************************************************
//	Failure return codes
//***************************************************************************

#endif
