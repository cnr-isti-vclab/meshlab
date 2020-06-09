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
	@file	CIFXPluginProxy.h

			Declaration of the CIFXPluginProxy class.

	@note	
*/

#ifndef __CIFXPluginProxy_H__
#define __CIFXPluginProxy_H__

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXPlugin.h"
#include "IFXString.h"
#include "IFXOSLoader.h"

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

class CIFXPluginProxy;

/**
	IFXPluginComponentDescriptor

	Structure that expands IFXComponentDescriptor with fields that 
	plug-in does not depend on. It is used only by IFXCore
 */
struct IFXPluginComponentDescriptor : public IFXComponentDescriptor
{
	CIFXPluginProxy*	pPluginProxy;	// Pointer to the plug-in proxy
};


/**
	This class incapsulate plug-in module states (loaded or unloaded) and
	provide opaque way to work with plug-in components.
 */
class CIFXPluginProxy 
{
public:
	CIFXPluginProxy();
	~CIFXPluginProxy();

	// Com Like ref counting support
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();

	/**
		Initializes CIFXPluginProxy object with plug-in module name

		@param  name Pointer to the name of plugin module.

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return the following value:
				IFX_E_INVALID_POINTER - pointer to the plug-in module name is not valid
	 */
	IFXRESULT Initialize( const IFXString* name /*U8* name*/ );

	/**
		Retreives component descriptors from plug-ins. Makes local copy of plug-in
		component descriptor list.

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
				IFX_E_INVALID_POINTER
	 */
	IFXRESULT RetrieveComponentDescriptors();

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
	IFXRESULT CreateComponent( const IFXComponentDescriptor* pComponentDescriptor,
							   const IFXCID& rComponentId,
							   const IFXIID& rInterfaceId,
							   void** ppInterface );


	/**
		Provides local component descriptor list of corresponding plug-in

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
	 */
	IFXPluginComponentDescriptor* GetComponentDescriptorList();

	IFXDID *GetDidsList();

	/**
		This method provides number of plug-in components

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
	 */
	U32 GetComponentNumber();

	U32 GetDidsNumber();

    /**
    	Unloads plug-in module

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
     */
    IFXRESULT Unload();

protected:
	/**
		Loads plug-in module
	 */
    IFXRESULT Load();

    /**
    	Checks the status of plug-in module (loaded, unloaded)

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
     */
    BOOL	  IsLoaded();

	/**
		Makes plug-in component descriptor list available local when 
		corresponding plug-in is unloaded. Used during plug-in registration.

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
	 */
	IFXRESULT CopyComponentDescriptorList( 
					const U32 numberOfComponents,
					const IFXComponentDescriptor* pComponentDescriptorList );

	/**
		Makes plug-in DIDs list available local when 
		corresponding plug-in is unloaded. Used during plug-in registration.

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
		return one of the following value:
	*/
	IFXRESULT CopyDidsList( 
					const U32 numberOfDids,
					const IFXDID* pDidsList );
	/**
		Updates factory pointers if plug-in was unloaded. Used by 
		CreateComponent

		@return Upon success, it returns the value IFX_OK. Otherwise, it will 
				return one of the following value:
	 */
	IFXRESULT UpdateFactoryPointers();

private:
	U32 m_refCount;

	/**
		file handle, also play the role of state variable.
		if m_handle is 0 plug-in is unloaded, otherwise loaded
	 */
    IFXHANDLE m_handle;

    IFXString m_name;

	IFXPluginComponentDescriptor* m_pComponentDescriptorList;
	U32	m_componentNumber;
	U32	m_didsNumber;

	IFXCID* m_pCIDList;
	IFXDID* m_pDIDList;
};


//***************************************************************************
//	Global function prototypes
//***************************************************************************


//***************************************************************************
//	Inline functions
//***************************************************************************

IFXINLINE IFXPluginComponentDescriptor* CIFXPluginProxy::GetComponentDescriptorList()
{
	return m_pComponentDescriptorList;
}

IFXINLINE IFXDID *CIFXPluginProxy::GetDidsList()
{
	return m_pDIDList;
}

IFXINLINE U32 CIFXPluginProxy::GetComponentNumber()
{
	return m_componentNumber;
}

IFXINLINE U32 CIFXPluginProxy::GetDidsNumber()
{
	return m_didsNumber;
}

//***************************************************************************
//	Failure return codes
//***************************************************************************


#endif
