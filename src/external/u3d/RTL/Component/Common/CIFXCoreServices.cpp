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
	@file	CIFXCoreServices.cpp

	Implementation of the CIFXCoreServices class which implements
	the IFXCoreServices interface. This object is responsible for
	maintaining and handing out references to the none-singleton
	components.
*/

//*****************************************************************************
//  Includes
//*****************************************************************************
#include "CIFXCoreServices.h"
#include "IFXCoreServicesRef.h"

#include "IFXCOM.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXRenderable.h"
#include "IFXSchedulingCIDs.h"
#include "IFXImportingCIDs.h"

//-----------------------------------------------------------------------------
//  CIFXCoreServices_Factory
//
//  This is the CIFXCoreServices component factory function.  The
//  CIFXCoreServices component can be instaniated multiple times.
//-----------------------------------------------------------------------------

IFXRESULT
IFXAPI_CALLTYPE CIFXCoreServices_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT   result = IFX_E_UNDEFINED;

	if ( ppInterface )
	{
		// Create the CIFXCoreServices component.
		CIFXCoreServices    *pComponent = new CIFXCoreServices;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN( result );
}

//-----------------------------------------------------------------------------
//  CIFXCoreServices::Initialize
//
//  Initializes the CoreServices component by setting up a weak reference
//  object and creating all of the interfaces that it will dole out later.
//-----------------------------------------------------------------------------

IFXRESULT
CIFXCoreServices::Initialize( U32 uProfile, F64 units )
{
	IFXRESULT   result = IFX_OK;

	if ( !m_bInitialized )
	{
		IFXCoreServices*    pWeakCS = NULL;      // Weak IFXCoreServices
		IFXCoreServicesRef* pCSRef;              // IFXCoreServicesRef object

		// Try to create an 'CIFXCoreServiceRef' subobject and get it's
		// private IFXCoreServicesRef interface. Also get it's IFXCoreServices
		// interface. The CIFXCoreServicesRef subobject will maintain and
		// expose a weak reference to the CIFXCoreServices object. Using this
		// subobject methodology will prevent circuler reference problems.
		if ( IFXSUCCESS( IFXCreateComponent(CID_IFXCoreServicesRef,
			IID_IFXCoreServicesRef,
			(void**)&pCSRef))           &&
			IFXSUCCESS( pCSRef->QueryInterface( IID_IFXCoreServices,
			(void**) &pWeakCS )) )
		{
			IFXCoreServices* pCS;

			// After getting the Weak reference, QI for the local
			// IFXCoreServices interface and do a SetReference on the
			// IFXCoreServicesRef object with the 'said' interface

			// Get a pointer to the main CoreServices interface
			if( IFXSUCCESS(QueryInterface( IID_IFXCoreServices, (void**)&pCS)))
			{
				// Attach the main IFXCoreServices iface to the
				// reference object so that it properly fowards
				// the IFXCoreServices methods.

				result = pCSRef->SetReference( pCS );

				if( IFXSUCCESS( result ) )
				{
					// We're done so store the ptr to the subobjects
					// IFXCoreServices interface for handing out to
					// new components created in the CreateComponents
					// method. Set the initialized flag to indicate
					// component is initialized
					IFXRELEASE( m_pWeakCS );

					m_pWeakCS = pWeakCS;
					m_bInitialized = TRUE;
				}
				else
					result = IFX_E_UNDEFINED;

				// Now that we are done with the our local
				// IFXCoreServices interface, release it
				pCS->Release();
			}
			else
				result = IFX_E_UNDEFINED;

			pCSRef->Release();

		}
		else
			result = IFX_E_UNDEFINED;

		// If an error has occured release the weak CoreServices Interface
		if ( IFXFAILURE( result) && pWeakCS )
			pWeakCS->Release();
	}
	else
		result = IFX_E_ALREADY_INITIALIZED;

	m_uProfile = uProfile;
	m_units = units;

	// Now construct the other components
	if ( IFXSUCCESS ( result ) )
	{
		result = CreateComponents();
	}
	// If this fails release our weak reference and set the Main object
	// to unitialized.
	if ( IFXFAILURE( result ) )
	{
		m_bInitialized = FALSE;
		m_pWeakCS->Release();
	}

	return IFX_OK;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::GetScheduler
//
//  Checks to see if the core services is initialized and if so does a
//  QueryInterface on the Scheduler component and returns the interface
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServices::GetScheduler( IFXREFIID interfaceId, void** ppv )
{
	IFXASSERT( ppv );
	IFXRESULT   result = IFX_OK;

	if ( m_bInitialized && m_pScheduler )
	{
		if( ppv )
			result = m_pScheduler->QueryInterface( interfaceId, ppv );
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;


	return result;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::GetNotificationManager
//
//  Checks to see if the core services is initialized and if so does a
//  QueryInterface on the NotificationManager component and returns the interface
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServices::GetNotificationManager( IFXREFIID interfaceId, void** ppv )
{
	IFXASSERT( ppv );
	IFXRESULT   result = IFX_OK;

	if ( m_bInitialized && m_pScheduler )
	{
		if ( ppv )
		{
			IFXNotificationManager *pNotifyMgr = NULL;
			result = m_pScheduler->GetNotificationManager( &pNotifyMgr );
			if ( IFXSUCCESS( result ) )
			{
				result = pNotifyMgr->QueryInterface( interfaceId, ppv );
			}
			IFXRELEASE( pNotifyMgr );
		}
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;


	return result;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::GetSceneGraph
//
//  Checks to see if the core services is initialized and if so does a
//  QueryInterface on the SceneGraph component and returns the interface
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServices::GetSceneGraph( IFXREFIID interfaceId, void** ppv )
{
	IFXASSERT ( ppv );
	IFXRESULT   result = IFX_OK;

	if( m_bInitialized )
	{
		if( ppv )
			result = m_pSceneGraph->QueryInterface( interfaceId, ppv );
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


//---------------------------------------------------------------------------
//  CIFXCoreServices::GetWeakInterface
//
//  This method is used to get a reference to the IFXCoreServices interface
//  like QI does, except the interface returned is to a special object
//  designed to prevent circular reference counting on the CIFXCoreServices
//  object.  The interface handed back by this method should be released just
//  like the ones handed back by QI.
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServices::GetWeakInterface( IFXCoreServices** ppCoreServices )
{
	IFXRESULT   result  = IFX_OK;

	if ( ppCoreServices )
	{
		if ( m_bInitialized )
		{
			IFXASSERT( m_pWeakCS );

			// We're handing back a reference to an IFXCOM object
			// interface, so make sure to call its AddRef method.  The caller
			// is responsible for releasing it when its finished (this is
			// nothing new or unusual).
			m_pWeakCS->AddRef();

			// Hand back the weak object's IFXCoreServices interface pointer.
			*ppCoreServices = m_pWeakCS;
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::GetNameMap
//
//  Checks to see if the core services is initialized and if so does a
//  QueryInterface on the NameMap component and returns the interface
//---------------------------------------------------------------------------
IFXRESULT
CIFXCoreServices::GetNameMap( IFXREFIID interfaceId, void** ppv )
{
	IFXASSERT( ppv );
	IFXRESULT   result = IFX_OK;

	if ( m_bInitialized )
	{
		if( ppv )
			result = m_pNameMap->QueryInterface( interfaceId, ppv );
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::GetFileReferencePalette
//
//  Checks to see if the core services is initialized and if so
//  returns pointer to palette of file reference objects
//---------------------------------------------------------------------------
IFXRESULT
CIFXCoreServices::GetFileReferencePalette( IFXPalette** ppv )
{
	IFXASSERT( ppv );
	IFXRESULT   result = IFX_OK;

	if ( m_bInitialized )
	{
		if( ppv )
			result = m_pFileReferencePalette->QueryInterface( IID_IFXPalette, (void**)ppv );
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::GetDidRegistry
//---------------------------------------------------------------------------

IFXRESULT CIFXCoreServices::GetDidRegistry( IFXREFIID interfaceId, void** ppv )
{
	IFXASSERT( ppv );
	IFXRESULT   result = IFX_OK;

	if ( m_bInitialized )
	{
		if( ppv )
			result = m_pDidRegistry->QueryInterface( interfaceId, ppv );
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------

// IFXUnknown...

//---------------------------------------------------------------------------
//  CIFXCoreServices::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXCoreServices::AddRef()
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXCoreServices::Release()
{
	if ( 1 == m_uRefCount )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServices::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT   result  = IFX_OK;

	if ( ppInterface )
	{

		if(interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* )((IFXMetaDataX*) this);
		}
		else if ( interfaceId == IID_IFXCoreServices )
		{
			*ppInterface = ( IFXCoreServices* ) this;
		}
		else if ( IID_IFXMetaDataX == interfaceId )
		{
			*ppInterface = ( IFXMetaDataX* ) this;
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
		{
			AddRef();
		}

	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}


//---------------------------------------------------------------------------
//  CIFXCoreServices::CIFXCoreServices
//
//  Constructor
//---------------------------------------------------------------------------

CIFXCoreServices::CIFXCoreServices()
{
	m_pWeakCS           = NULL;
	m_bInitialized      = FALSE;
	m_uRefCount         = 0;        // Reference counter
	m_pScheduler        = NULL;     // Scheduler Interface
	m_pSceneGraph       = NULL;     // SceneGraph Interface
	m_pNameMap      = NULL;
	m_pDidRegistry      = NULL;
	m_pMetaData     = NULL;
	m_pFileReferencePalette = NULL;
	m_uProfile      = 0;
	m_units			= 1.0f;
}

//---------------------------------------------------------------------------
//  CIFXCoreServices::~CIFXCoreServices
//
//  Destructor
//---------------------------------------------------------------------------

CIFXCoreServices::~CIFXCoreServices()
{
	if (m_pScheduler)
		m_pScheduler->Reset(); // clear out any circular dependencies
	IFXRELEASE( m_pScheduler );
	IFXRELEASE( m_pSceneGraph );
	IFXRELEASE( m_pNameMap );
	IFXRELEASE( m_pFileReferencePalette );
	IFXRELEASE( m_pDidRegistry );
	IFXRELEASE( m_pMetaData );

	if ( m_bInitialized )
	{
		IFXRESULT           result              = IFX_OK;
		IFXCoreServicesRef* pCoreServicesRef    = NULL;

		// If initialized, assume that m_pWeakCS is valid.
		IFXASSERT( m_pWeakCS );
		result = m_pWeakCS->QueryInterface( IID_IFXCoreServicesRef,
			(void**)&pCoreServicesRef );

		if ( IFXSUCCESS( result ) )
		{
			// Detach the main object from the subobject. This will
			// prevent access violations if the client still has any
			// outstanding references to the subobject.
			IFXASSERT( pCoreServicesRef );
			pCoreServicesRef->SetReference( NULL );
			pCoreServicesRef->Release();
		}

		m_pWeakCS->Release();
	}
}


IFXRESULT
CIFXCoreServices::CreateComponents()
{
	IFXRESULT rc = IFX_OK;

	if ( FALSE == m_bInitialized )
		rc = IFX_E_ALREADY_INITIALIZED ;

	// Create the various components: If any one component
	// creation fails all of the components created ahead
	// of the failure will be subsequently released in the
	// opposite order of their creation


	// this Did registry initialization must occur before
	// the scene graph is created.
	if(IFXSUCCESS(rc))
		rc = IFXCreateComponent( CID_IFXDidRegistry,
		IID_IFXDidRegistry,
		(void**) &m_pDidRegistry);

	if(IFXSUCCESS(rc))
	{ // register info for known dids
		m_pDidRegistry->AddDID(DID_IFXRenderableGroup, IFX_DID_UNKNOWN | IFX_DID_RENDERABLE);
		m_pDidRegistry->AddDID(DID_IFXRenderableGroupBounds, IFX_DID_BOUND | IFX_DID_UNKNOWN);

		m_pDidRegistry->AddDID(DID_IFXBoundFrame, IFX_DID_UNKNOWN | IFX_DID_RENDERABLE);
		m_pDidRegistry->AddDID(DID_IFXBoundFrameBounds, IFX_DID_BOUND | IFX_DID_UNKNOWN);

		m_pDidRegistry->AddDID(DID_IFXLightSet, IFX_DID_UNKNOWN);
		m_pDidRegistry->AddDID(DID_IFXNeighborMesh, IFX_DID_UNKNOWN);
		m_pDidRegistry->AddDID(DID_IFXTexture, IFX_DID_UNKNOWN);
	}

	if( IFXSUCCESS( rc ) )
	{
		IFXArray<IFXDID*> *pDidsList = NULL;
		U32 didsCount;

		IFXGetPluginsDids( pDidsList );
		didsCount = pDidsList->GetNumberElements();

		U32 i;
		for( i = 0; i < didsCount; i++ )
		{
			m_pDidRegistry->AddDID( *(pDidsList->GetElement(i)), IFX_DID_UNKNOWN );
		}
	}

	m_pMetaData = NULL;
	if (IFXSUCCESS(rc))
		rc = IFXCreateComponent(CID_IFXMetaDataX, IID_IFXMetaDataX, (void**)&m_pMetaData);

	// Scheduler
	if (IFXSUCCESS(rc)) 
	{
		IFXRESULT rc = IFX_OK;
		rc = IFXCreateComponent ( CID_IFXScheduler,
			IID_IFXScheduler,
			(void**)&m_pScheduler );
		if (IFXSUCCESS(rc))
			rc = m_pScheduler->Initialize( m_pWeakCS );
		else
			m_pScheduler = NULL;
	}

	// Scene Graph
	if (IFXSUCCESS(rc))
		rc = IFXCreateComponent( CID_IFXSceneGraph,
		IID_IFXSceneGraph,
		(void**)&m_pSceneGraph );

	if (IFXSUCCESS(rc))
		rc = m_pSceneGraph->Initialize( m_pWeakCS );

	if (IFXSUCCESS(rc)) 
	{
		IFXRESULT rc = IFX_OK;
		rc = IFXCreateComponent( CID_IFXNameMap,
			IID_IFXNameMap,
			(void**)&m_pNameMap );
		if (IFXSUCCESS(rc)) 
		{
			// All palettes are initialized except FileReference palette,
			// because FileReference objects doesn't have their own names,
			// so they are not a subject to do name mapping.
			rc = m_pNameMap->Initialize(IFXSceneGraph::NUMBER_OF_PALETTES);
			m_pNameMap->Reserve(IFXSceneGraph::MATERIAL, PALETTE_DEFAULT_MATERIAL);
			m_pNameMap->Reserve(IFXSceneGraph::GENERATOR, PALETTE_DEFAULT_MODEL);
			m_pNameMap->Reserve(IFXSceneGraph::SHADER, PALETTE_DEFAULT_SHADER);
			m_pNameMap->Reserve(IFXSceneGraph::MOTION, PALETTE_DEFAULT_IFX_MOTION);
			m_pNameMap->Reserve(IFXSceneGraph::MIXER, PALETTE_DEFAULT_MOTION);
			m_pNameMap->Reserve(IFXSceneGraph::TEXTURE, PALETTE_DEFAULT_TEXTURE);
			m_pNameMap->Reserve(IFXSceneGraph::NODE, PALETTE_DEFAULT_GROUP);
			m_pNameMap->Reserve(IFXSceneGraph::LIGHT, PALETTE_DEFAULT_LIGHT);
			m_pNameMap->Reserve(IFXSceneGraph::VIEW, PALETTE_DEFAULT_CAMERA);
		} else
			m_pNameMap = NULL;
	}

	if (IFXSUCCESS(rc))
		rc = IFXCreateComponent( CID_IFXPalette,
		IID_IFXPalette,
		(void**)&m_pFileReferencePalette );

	if (IFXSUCCESS(rc))
		rc = m_pFileReferencePalette->Initialize(10);

	if (IFXFAILURE(rc))
	{
		IFXRELEASE( m_pFileReferencePalette );
		IFXRELEASE( m_pNameMap );
		IFXRELEASE( m_pSceneGraph );
		if (m_pScheduler)
			m_pScheduler->Reset(); // clear out any circular dependencies
		IFXRELEASE( m_pScheduler );
		IFXRELEASE( m_pDidRegistry );
	}

	return rc;
}

// IFXMetaData
void CIFXCoreServices::GetCountX(U32& rCount)const
{
	m_pMetaData->GetCountX(rCount);
}

IFXRESULT CIFXCoreServices::GetIndex(const IFXString& rKey, U32& uIndex)
{
	return m_pMetaData->GetIndex(rKey, uIndex);
}

void CIFXCoreServices::GetKeyX(U32 index, IFXString& rOutKey)
{
	m_pMetaData->GetKeyX(index, rOutKey);
}

void CIFXCoreServices::GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rValueType)
{
	m_pMetaData->GetAttributeX(uIndex, rValueType);
}

void CIFXCoreServices::SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rValueType)
{
	m_pMetaData->SetAttributeX(uIndex, rValueType);
}

void CIFXCoreServices::GetBinaryX(U32 uIndex, U8* pDataBuffer)
{
	m_pMetaData->GetBinaryX(uIndex, pDataBuffer);
}

void CIFXCoreServices::GetBinarySizeX(U32 uIndex, U32& rSize)
{
	m_pMetaData->GetBinarySizeX(uIndex, rSize);
}

void CIFXCoreServices::GetStringX(U32 uIndex, IFXString& rValue)
{
	m_pMetaData->GetStringX(uIndex, rValue);
}

void CIFXCoreServices::GetPersistenceX(U32 uIndex, BOOL& rPersistence)
{
	m_pMetaData->GetPersistenceX(uIndex, rPersistence);
}

void CIFXCoreServices::SetBinaryValueX(const IFXString& rKey, U32 length, const U8* data)
{
	m_pMetaData->SetBinaryValueX(rKey, length, data);
}

void CIFXCoreServices::SetStringValueX(const IFXString& rKey, const IFXString& rValue)
{
	m_pMetaData->SetStringValueX(rKey, rValue);
}

void CIFXCoreServices::SetPersistenceX(U32 uIndex, BOOL value)
{
	m_pMetaData->SetPersistenceX(uIndex, value);
}

void CIFXCoreServices::DeleteX(U32 uIndex)
{
	m_pMetaData->DeleteX(uIndex);
}

void CIFXCoreServices::DeleteAll()
{
	m_pMetaData->DeleteAll();
}

void CIFXCoreServices::AppendX(IFXMetaDataX* pSource)
{
	m_pMetaData->AppendX(pSource);
}

void CIFXCoreServices::GetEncodedKeyX(U32 uIndex, IFXString& rOutKey)
{
	m_pMetaData->GetEncodedKeyX(uIndex, rOutKey);
}

void CIFXCoreServices::GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes)
{
	m_pMetaData->GetSubattributesCountX(uIndex, rCountSubattributes);
}

IFXRESULT CIFXCoreServices::GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex)
{
	return m_pMetaData->GetSubattributeIndex(uIndex, rSubattributeName, rSubattributeIndex);
}

void CIFXCoreServices::GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName)
{
	m_pMetaData->GetSubattributeNameX(uIndex, uSubattributeIndex, rSubattributeName);
}

void CIFXCoreServices::GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue)
{
	m_pMetaData->GetSubattributeValueX(uIndex, uSubattributeIndex, pSubattributeValue);
}

void CIFXCoreServices::SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue)
{
	m_pMetaData->SetSubattributeValueX(uIndex, rSubattributeName, pSubattributeValue);
}

void CIFXCoreServices::DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex)
{
	m_pMetaData->DeleteSubattributeX(uIndex, uSubattributeIndex);
}

void CIFXCoreServices::DeleteAllSubattributes(U32 uIndex)
{
	m_pMetaData->DeleteAllSubattributes(uIndex);
}
