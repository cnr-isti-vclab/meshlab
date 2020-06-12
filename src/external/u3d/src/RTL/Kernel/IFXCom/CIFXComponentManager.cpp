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
	@file	CIFXComponentManager.cpp

			This module defines the CIFXComponentManager class. It is used to
			manage all operations with components and component database.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "CIFXComponentManager.h"
#include "CIFXGuidHashMap.h"
#include "CIFXPluginProxy.h"
#include "IFXOSFileIterator.h"
#include "IFXCOM.h"

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


//***************************************************************************
//	Global data
//***************************************************************************

extern IFXComponentDescriptor g_coreComponentDescriptorList[];
extern U32 g_coreComponentNumber;

//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************

CIFXComponentManager::CIFXComponentManager()
{
	m_refCount = 0;
	m_pGuidHashMap = NULL;
	m_pPluginProxyList = NULL;
	m_pluginNumber = 0;
	m_pDidsList = NULL;
}

CIFXComponentManager::~CIFXComponentManager()
{
	if( NULL != m_pGuidHashMap )
		delete m_pGuidHashMap;
	m_pGuidHashMap = NULL;

	if( NULL != m_pPluginProxyList )
	{
		delete [] m_pPluginProxyList;
		m_pPluginProxyList = NULL;
	}

	m_pluginNumber = 0;

	if( NULL != m_pDidsList )
	{
		m_pDidsList->Clear();
		delete m_pDidsList;
	}
}

U32 CIFXComponentManager::AddRef()
{
	return ++m_refCount;
}

U32 CIFXComponentManager::Release()
{
	if(!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXComponentManager::Initialize()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
	{
		if( NULL != m_pDidsList )
		{
			m_pDidsList->Clear();
			delete m_pDidsList;
		}

		m_pDidsList = new IFXArray<IFXDID*>;

		if( NULL == m_pDidsList )
			result = IFX_E_OUT_OF_MEMORY;
	}

	if ( NULL != m_pGuidHashMap)
	{
		delete m_pGuidHashMap;
	}

	m_pGuidHashMap = new CIFXGuidHashMap;

	if ( NULL != m_pGuidHashMap)
	{
		// initialize component database and register core components
		result = m_pGuidHashMap->Initialize( g_coreComponentNumber,
											 g_coreComponentDescriptorList);
		// Search for Plugins. 
		if(IFXSUCCESS(result))
			result = FindPlugins();

		// Load Plug-in Components
		if(IFXSUCCESS(result))
			result = RegisterPlugins();
	}
	else
		result = IFX_E_OUT_OF_MEMORY;

	return result;
}

IFXRESULT CIFXComponentManager::RegisterComponent ( 
			const IFXComponentDescriptor* pComponentDescriptor)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pGuidHashMap);

	if ( NULL != m_pGuidHashMap )
		result = m_pGuidHashMap->Add( pComponentDescriptor );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXASSERTBOX( (result == IFX_OK),
		"CIFXComponentManager::RegisterComponent - component was not registered");

	return result;
}


IFXRESULT CIFXComponentManager::CreateComponent( const IFXCID& rComponentId, 
												 const IFXIID& rInterfaceId, 
												 void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if( NULL != m_pGuidHashMap )
	{
		const IFXComponentDescriptor* pComponentDescriptor;

		result = m_pGuidHashMap->Find( rComponentId, &pComponentDescriptor );

		if( IFXSUCCESS(result) )
		{
			if( NULL != pComponentDescriptor->pFactoryFunction )
			{
				// creation request for core/client component
				// or for loaded plug-in components
				if (pComponentDescriptor->Version >= 0)
					result = (pComponentDescriptor->pFactoryFunction)
								(rInterfaceId, ppInterface);
				else
					result = (pComponentDescriptor->pCLIFactoryFunction)
								(rComponentId, rInterfaceId, ppInterface);
			}
			else
			{
				const IFXPluginComponentDescriptor* pPluginComponentDescriptor = 
					static_cast<const IFXPluginComponentDescriptor*>(pComponentDescriptor);

				CIFXPluginProxy* pPluginProxy = 
					pPluginComponentDescriptor->pPluginProxy;

				// creation request for unloaded plugin component, so we have
				// to use plugin proxy object to load plug-in module before we
				// can actually create component
				if( NULL != pPluginProxy )
				{
					result = pPluginProxy->CreateComponent( 
									pComponentDescriptor, rComponentId,
									rInterfaceId, ppInterface );
				}
			}
		}
		else
			result = IFX_E_COMPONENT;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

IFXRESULT CIFXComponentManager::UnloadAllPlugins()
{
	IFXRESULT result = IFX_OK;

	U32 i;
	for( i = 0; i < m_pluginNumber; ++i )
	{
		if( IFXFAILURE( m_pPluginProxyList[i].Unload()) )
			result = IFX_W_CANNOT_UNLOAD;
    }

	return result;
}


//***************************************************************************   
//	Protected methods
//***************************************************************************

IFXRESULT CIFXComponentManager::FindPlugins()
{
	IFXRESULT result = IFX_OK;

// Hack to avoid that plugins dir issue and search tem as normal libs
    IFXString path = "";
#ifdef RENDERING
#ifdef WIN32
    IFXString plugins = "IFXExporting.dll:IFXImporting.dll:IFXScheduling.dll:IFXRendering.dll:";
#else
	IFXString plugins = "libIFXExporting.so:libIFXImporting.so:libIFXScheduling.so:libIFXRendering.so:";
#endif
	m_pluginNumber = 4;
#else
#ifdef WIN32
    IFXString plugins = "IFXExporting.dll:IFXImporting.dll:IFXScheduling.dll:";
#else
    IFXString plugins = "libIFXExporting.so:libIFXImporting.so:libIFXScheduling.so:";
#endif
	m_pluginNumber = 3;
#endif
/*
    // create recursive file iterator with default parameters
    IFXOSFileIterator fr;

    // pointer to the string with plugins (where they are stored with relative paths)
    // find all plugins, get pointer to the string with their paths and plugins number:
    IFXString plugins, path;

	result = fr.GetPlugins( plugins, m_pluginNumber );

	if( IFXSUCCESS(result) )
	{
		// get path to plugins directory
		fr.GetPluginsLocation( path );    // pointer to the string with path to plugins directory
	}
*/

    if( 0 != m_pluginNumber )
    {
		IFXString current;  // here we will store current plugin (with full path)

        IFXDELETE_ARRAY( m_pPluginProxyList );
        m_pPluginProxyList = new CIFXPluginProxy[m_pluginNumber];

        if( NULL != m_pPluginProxyList )
        {
			U32 index1 = 0, index2 = 0;
			U32 length = plugins.Length(); // length of plugins string

			IFXCHAR* workString = (IFXCHAR*)IFXAllocate( (length+1)*sizeof(IFXCHAR) ); // allocate editable string...

			if( NULL != workString )
				memcpy( workString, plugins.Raw(), (length + 1) * sizeof(IFXCHAR) ); // ...and copy data here...
			else
				result = IFX_E_OUT_OF_MEMORY;

			U32 i;
            for( i = 0; i < m_pluginNumber && IFXSUCCESS(result); i++ )
            {
                // index1 stores index of first char of current plugin's name
                // index2 stores last one
                index2++;
                while( workString[index2] != IFXOSFI_DELIM[0] )
                {
                    if( 0 == workString[index2] )
                    {
                        // if we are here then it means we have reached the 
                        // end of the string with names sooner then found all plugins
                        result = IFX_E_INVALID_RANGE;
                        break;
                    }
                    index2++;
                }

                // we have found delimiter (':' by default). place zero instead of it...
                workString[index2] = 0;
                // ...initialize path to current plugin with path to plugin directory...
                current.Assign( &path );
                // ...and append plugin's name with subdirectories (if any)
                current.Concatenate( &workString[index1] );

                // init proxy list's entry with full name of the plugin
                m_pPluginProxyList[i].AddRef();
                m_pPluginProxyList[i].Initialize( &current/*.RawU8()*/ );

                // step to the next entry in the string
                index1 = index2 + 1;
            }

			// free memory
			if( NULL != workString )
				IFXDeallocate( workString );
        }
        else
        {
            result = IFX_E_OUT_OF_MEMORY;
        }
    }

	return result;
}


IFXRESULT CIFXComponentManager::RegisterPlugins()
{
	IFXRESULT result = IFX_OK;

	/*
	return IFX_OK because we can work without any plug-ins
	*/
	if( NULL != m_pPluginProxyList && 0 != m_pluginNumber )
	{
		// registration loop for all plugins
		U32 i;
		for( i = 0; i < m_pluginNumber; ++i )
		{
			result = m_pPluginProxyList[i].RetrieveComponentDescriptors();

			if( IFXSUCCESS(result) )
			{
				const IFXPluginComponentDescriptor* pluginComponentDescriptorList = 
					m_pPluginProxyList[i].GetComponentDescriptorList();

				U32 j, limit = m_pPluginProxyList[i].GetComponentNumber();
				for( j = 0; j < limit && IFXSUCCESS( result ); ++j )
				{
					result = RegisterComponent( &pluginComponentDescriptorList[j] );
				}
			}

			if( IFXSUCCESS( result ) )
			{
				IFXDID *pDIDsList = m_pPluginProxyList[i].GetDidsList();

				U32 j, limit = m_pPluginProxyList[i].GetDidsNumber();

				if( m_pDidsList )
				{
					for( j = 0; j < limit; j++ )
					{
						m_pDidsList->CreateNewElement();
						m_pDidsList->GetElement(j) = &pDIDsList[j];
					}
				}
				else
					result = IFX_E_NOT_INITIALIZED;
			}
		}
	}

	return result;
}


IFXArray<IFXDID*> *CIFXComponentManager::GetPluginsDids()
{
	return m_pDidsList;
}

//***************************************************************************
//	Private methods
//***************************************************************************


//***************************************************************************
//	Global functions
//***************************************************************************


//***************************************************************************
//	Local functions
//***************************************************************************
