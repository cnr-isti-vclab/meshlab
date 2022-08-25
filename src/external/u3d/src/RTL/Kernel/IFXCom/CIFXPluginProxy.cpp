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
//
//	CIFXPluginProxy.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXPluginProxy class. 
//
//	NOTES
//*****************************************************************************


//***************************************************************************
//	Includes
//***************************************************************************

#include "CIFXPluginProxy.h"
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


//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************

CIFXPluginProxy::CIFXPluginProxy() : 
	m_refCount( 0 ),
	m_handle( 0 ),
	m_pComponentDescriptorList( NULL ),
	m_componentNumber( 0 ),
	m_didsNumber( 0 ),
	m_pCIDList( NULL ),
	m_pDIDList( NULL )
{
}

CIFXPluginProxy::~CIFXPluginProxy()
{
	m_refCount = 0;
	m_name.Clear();

	IFXDELETE_ARRAY( m_pComponentDescriptorList );
	IFXDELETE_ARRAY( m_pCIDList );
	IFXDELETE_ARRAY( m_pDIDList );
}

U32 CIFXPluginProxy::AddRef()
{
	return ++m_refCount;
}

U32 CIFXPluginProxy::Release()
{
	if(!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXPluginProxy::Initialize( const IFXString* name /*U8* name*/ )
{
	IFXRESULT result = IFX_OK;

	if( NULL != name )
		m_name.Assign(name);
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXPluginProxy::RetrieveComponentDescriptors()
{
	IFXRESULT result = IFX_OK;

	if( IFX_FALSE == IsLoaded() )
		result = Load();

	if( IFXSUCCESS(result) )
	{
		IFXPluginRegisterFunction pRegister = 
					(IFXPluginRegisterFunction)IFXGetAddress( 
					m_handle, 
					"IFXPluginRegister" );
		if( NULL != pRegister )
		{
			U32 componentNumber = 0;
			IFXComponentDescriptor* pComponentDescriptorList = NULL;

			result = pRegister( &componentNumber, &pComponentDescriptorList );

			if( IFXSUCCESS(result) )
			{
				// make pComponentDescriptorList available local because 
				// plug-ins module will be unloaded after component 
				// registration and pComponentDescriptorList in plug-in 
				// will be unavailable
				result = CopyComponentDescriptorList( 
										componentNumber, 
										pComponentDescriptorList );
			}
		}
		else
			result = IFX_E_INVALID_POINTER;

		if( IFXSUCCESS( result ) )
		{
			IFXPluginRegisterDidsFunction pRegisterDids = 
				(IFXPluginRegisterDidsFunction)IFXGetAddress( 
				m_handle, 
				"IFXPluginRegisterDids" );

			// if (NULL == pRegisterDids) this is ok too - just do nothing
			if( NULL != pRegisterDids )
			{
				U32 didsNumber = 0;
				IFXDID *pDidsList = NULL;

				result = pRegisterDids( &didsNumber, &pDidsList );

				if( IFXSUCCESS( result ) )
				{
					result = CopyDidsList( didsNumber, pDidsList );
				}
			}
		}

		// to unload plug-in after registration we should have a local copy of
		// all component descriptions, otherwise we cannot use IFXCID
		// when we try to find component by IFXCID because of plug-in 
		// was unloaded
		Unload();
	}

	return result;
}


IFXRESULT CIFXPluginProxy::CreateComponent( 
	const IFXComponentDescriptor* pComponentDescriptor, 
	const IFXCID& rComponentId,
	const IFXIID& rInterfaceId, 
	void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if( IFX_FALSE == IsLoaded() )
	{
		result = Load();
		if( IFXSUCCESS(result) )
			result = UpdateFactoryPointers();
	}

	if( IFXSUCCESS(result) )
	{
		if (pComponentDescriptor->Version >= 0)
			result = (pComponentDescriptor->pFactoryFunction)
			(rInterfaceId, ppInterface);
		else
			result = (pComponentDescriptor->pCLIFactoryFunction)
			(rComponentId, rInterfaceId, ppInterface);
	}

	return result;
}

IFXRESULT CIFXPluginProxy::Unload()
{
	IFXRESULT result = IFX_OK;

	if( IFX_TRUE == IsLoaded() )
	{
		IFXPluginCanUnloadNowFunction pCanUnloadNow = 
				(IFXPluginCanUnloadNowFunction)IFXGetAddress( 
				m_handle, 
				"IFXPluginCanUnloadNow" );
		if( NULL != pCanUnloadNow )
		{
			result = pCanUnloadNow();

			if( IFXSUCCESS(result) && m_handle )
				result = IFXReleaseLibrary(m_handle);
		}
		else
			result = IFX_E_INVALID_POINTER;
	}

	if( IFXSUCCESS(result) )
	{
		m_handle = 0;
		U32 i;

		// prevent calling factory function when plug-in is unloaded
		for( i = 0; i < m_componentNumber; ++i )
			m_pComponentDescriptorList[i].pFactoryFunction = NULL;
	}

	return result;
}


//***************************************************************************
//	Protected methods
//***************************************************************************

IFXRESULT CIFXPluginProxy::Load()
{
	IFXRESULT result = IFX_OK;

	m_handle = IFXLoadLibrary( m_name.Raw() );
	if( 0 == m_handle )
	{
		result = IFX_E_INVALID_FILE;
	}

	return result;
}

IFXRESULT CIFXPluginProxy::IsLoaded()
{
	IFXRESULT result = IFX_TRUE;

	if( 0 == m_handle )
		result = IFX_FALSE;

	return result;
}

IFXRESULT CIFXPluginProxy::CopyComponentDescriptorList( 
	const U32 componentNumber,
	const IFXComponentDescriptor* pComponentDescriptorList )
{
	IFXRESULT result = IFX_OK;

	if( 0 == componentNumber || NULL == pComponentDescriptorList)
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS(result) )
	{
		m_componentNumber = componentNumber;

		if( NULL != m_pCIDList )
			delete [] m_pCIDList;

		m_pCIDList = new IFXCID[componentNumber];

		if( NULL != m_pComponentDescriptorList )
			delete [] m_pComponentDescriptorList;

		m_pComponentDescriptorList = 
			new IFXPluginComponentDescriptor[componentNumber];

		if( NULL != m_pComponentDescriptorList && NULL != m_pCIDList )
		{
			U32 i;
			for( i = 0; i < componentNumber; ++i )
			{
				m_pCIDList[i] = *(pComponentDescriptorList[i].pComponentId);

				/*
				Actually we do not have to copy factory function pointers since
				plug-in will be unloaded after registration. But if somehow we 
				are not able to unload plug-in library after registration 
				we can meet a problem when somebody tries to create component
				and factory function is not initialized.
				Anyway we have to update factory function pointers during next 
				plug-in loading. See CreateComponent and UndateFactoryFunctions
				*/
				m_pComponentDescriptorList[i].pFactoryFunction = 
					pComponentDescriptorList[i].pFactoryFunction;

				m_pComponentDescriptorList[i].Version = 
					pComponentDescriptorList[i].Version;
				m_pComponentDescriptorList[i].pPluginProxy = this;

				// overwrite pointer to component id to use local storage
				m_pComponentDescriptorList[i].pComponentId = &m_pCIDList[i];
			}
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}

	return result;
}

IFXRESULT CIFXPluginProxy::CopyDidsList( 
	const U32 didsNumber,
	const IFXDID* pDidsList )
{
	IFXRESULT result = IFX_OK;

	if( 0 == didsNumber || NULL == pDidsList)
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS(result) )
	{
		m_didsNumber = didsNumber;

		if( NULL != m_pDIDList )
			delete [] m_pDIDList;

		m_pDIDList = new IFXDID[didsNumber];

		if( NULL != m_pDIDList )
		{
			U32 i;
			for( i = 0; i < didsNumber; ++i )
			{
				m_pDIDList[i] = pDidsList[i];
			}
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}

	return result;
}

IFXRESULT CIFXPluginProxy::UpdateFactoryPointers()
{
	IFXRESULT result = IFX_OK;

	IFXPluginRegisterFunction pRegister = 
		(IFXPluginRegisterFunction)IFXGetAddress( 
		m_handle, 
		"IFXPluginRegister" );
	if( NULL != pRegister && NULL != m_pComponentDescriptorList )
	{
		IFXComponentDescriptor* pComponentDescriptorList = NULL;
		U32 didsNumber = 0;

		result = pRegister( &didsNumber, 
			&pComponentDescriptorList );

		if( IFXSUCCESS(result) )
		{
			U32 i;
			for( i = 0; i < didsNumber; ++i )
			{
				m_pComponentDescriptorList[i].pFactoryFunction = 
					pComponentDescriptorList[i].pFactoryFunction;
			}
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
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
