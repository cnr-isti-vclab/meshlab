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
//	CIFXCoreServicesRef.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXCoreServicesRef class which implements the
//		IFXCoreServicesRef interface. Objects of this class are used to
//		prevent circular reference problems that might exist when multiple
//		references to the CoreServices object are created.
//
//	NOTES
//      End users should not directly create and use this CoreServices
//		reference object even though it's interface is exposed. The
//		CIFXCoreServices objects will create this reference object as a sub-
//		object. When other components are created by the CIFXCoreServices
//		component they will be Initialized with a reference to this Reference
//		object.
//
//*****************************************************************************

#include "CIFXCoreServicesRef.h"

#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"

IFXRESULT
IFXAPI_CALLTYPE CIFXCoreServicesRef_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result = IFX_E_UNDEFINED;

	if ( ppInterface )
	{
		// Create the CIFXCoreServicesRef component.
		CIFXCoreServicesRef	*pComponent	= new CIFXCoreServicesRef;

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
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}

//-----------------------------------------------------------------------------
//	CIFXCoreServicesRef::SetReference
//
//	The SetReference function is used to specify the IFXCoreServices interface
//  pointer that this object should use when fowarding work to the real Core-
//	Services component. Specify NULL if work should not be forwarded anymore.
//-----------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::SetReference( IFXCoreServices* pCoreServices )
{
	// Want a weak reference so dont AddRef, we also don't need
	// to check for a previous call to this function because we're
	// just going to overwrite the reference.
	m_pCoreServices = pCoreServices;

	IFXRETURN( IFX_OK );
}


//-----------------------------------------------------------------------------
//	CIFXCoreServicesRef::Initialize
//
//	This function wont be implemented
//-----------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::Initialize( U32 uProfile, F64 units )
{
	IFXRETURN( IFX_E_UNDEFINED );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetScheduler
//
//	Checks to see if the CoreServices pointer in the reference object has
//	been initialized and if so fowards the call on to the real CoreServices
//	object's GetScheduler function.
//
//	RETURNS
//		IFX_OK
//		IFX_E_NOT_INITIALIZED
//		Standard QueryInterface return values
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetScheduler( IFXREFIID interfaceId, void** ppv )
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetScheduler( interfaceId, ppv );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetNotificationManager
//
//	Checks to see if the CoreServices pointer in the reference object has
//	been initialized and if so fowards the call on to the real CoreServices
//	object's GetNotificationManager function.
//
//	RETURNS
//		IFX_OK
//		IFX_E_NOT_INITIALIZED
//		Standard QueryInterface return values
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetNotificationManager( IFXREFIID interfaceId, void** ppv )
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetNotificationManager( interfaceId, ppv );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetSceneGraph
//
//
//	Checks to see if the CoreServices pointer in the reference object has
//	been initialized and if so fowards the call on to the real CoreServices
//	object's GetSceneGraph function.
//
//	RETURNS
//		IFX_OK
//		IFX_E_NOT_INITIALIZED
//		Standard QueryInterface return values
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetSceneGraph( IFXREFIID interfaceId, void** ppv )
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetSceneGraph( interfaceId, ppv );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}



//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetNameMap
//
//
//	Checks to see if the CoreServices pointer in the reference object has
//	been initialized and if so fowards the call on to the real CoreServices
//	object's GetNameMap function.
//
//	RETURNS
//		IFX_OK
//		IFX_E_NOT_INITIALIZED
//		Standard QueryInterface return values
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetNameMap( IFXREFIID interfaceId, void** ppv )
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetNameMap( interfaceId, ppv );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetFileReferencePalette
//
//
//	Checks to see if the CoreServices pointer in the reference object has
//	been initialized and if so fowards the call on to the real CoreServices
//	object's GetFileReferencePalette function.
//
//	RETURNS
//		IFX_OK
//		IFX_E_NOT_INITIALIZED
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetFileReferencePalette( IFXPalette** ppv )
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetFileReferencePalette( ppv );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::SetBaseURL
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::SetBaseURL(const IFXString& sURL)
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->SetBaseURL(sURL);
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetBaseURL
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetBaseURL(IFXString& sURL)
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetBaseURL(sURL);
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetProfile
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetProfile(U32& rProfile)
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetProfile(rProfile);
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetUnits
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetUnits(F64& rUnits)
{
	IFXRESULT result = IFX_OK;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetUnits(rUnits);
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::GetWeakInterface
//
//
//	Checks to see if the CoreServices pointer in the reference object has
//	been initialized and if so fowards the call on to the real CoreServices
//	object's GetWeakInterface function.
//
//	RETURNS
//		IFX_OK
//		IFX_E_INVALID_POINTER
//		IFX_E_NOT_INITIALIZED
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::GetWeakInterface( IFXCoreServices** ppCoreServices )
{
	IFXRESULT	result;

	if ( m_pCoreServices )
		result = m_pCoreServices->GetWeakInterface( ppCoreServices );
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXRETURN( result );
}


// IFXUnknown...

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXCoreServicesRef::AddRef()
{
	
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXCoreServicesRef::Release()
{
	
	if( 1 == m_uRefCount )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT
CIFXCoreServicesRef::QueryInterface( IFXREFIID	interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXCoreServicesRef ||
			 interfaceId == IID_IFXUnknown )
		{
			*ppInterface = static_cast<IFXCoreServicesRef*>(this);
		}
		else if ( interfaceId == IID_IFXCoreServices )
		{
			*ppInterface = static_cast<IFXCoreServices*>(this);
		}
		else if ( IID_IFXMetaDataX == interfaceId ) 
		{
			*ppInterface = static_cast<IFXMetaDataX*>(this);
		} 
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}


//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::CIFXCoreServicesRef
//
//  Constructor
//---------------------------------------------------------------------------

CIFXCoreServicesRef::CIFXCoreServicesRef()
{
	m_uRefCount			= 0;
	m_pCoreServices		= NULL;
}

//---------------------------------------------------------------------------
//	CIFXCoreServicesRef::~CIFXCoreServicesRef
//
//  Destructor
//---------------------------------------------------------------------------

CIFXCoreServicesRef::~CIFXCoreServicesRef() {
}

// IFXMetaData
void CIFXCoreServicesRef::GetCountX(U32& rCount)const
{
	((CIFXCoreServices*)m_pCoreServices)->GetCountX(rCount); 
}

IFXRESULT CIFXCoreServicesRef::GetIndex(const IFXString& rKey, U32& uIndex)
{ 
	return ((CIFXCoreServices*)m_pCoreServices)->GetIndex(rKey, uIndex); 
}

void CIFXCoreServicesRef::GetKeyX(U32 index, IFXString& rOutKey)
{
	((CIFXCoreServices*)m_pCoreServices)->GetKeyX(index, rOutKey); 
}

void CIFXCoreServicesRef::GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rValueType)
{
	((CIFXCoreServices*)m_pCoreServices)->GetAttributeX(uIndex, rValueType); 
}

void CIFXCoreServicesRef::SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rValueType)
{
	((CIFXCoreServices*)m_pCoreServices)->SetAttributeX(uIndex, rValueType); 
}

void CIFXCoreServicesRef::GetBinaryX(U32 uIndex, U8* pDataBuffer)
{
	((CIFXCoreServices*)m_pCoreServices)->GetBinaryX(uIndex, pDataBuffer); 
}

void CIFXCoreServicesRef::GetBinarySizeX(U32 uIndex, U32& rSize)
{
	((CIFXCoreServices*)m_pCoreServices)->GetBinarySizeX(uIndex, rSize); 
}

void CIFXCoreServicesRef::GetStringX(U32 uIndex, IFXString& rValue)
{
	((CIFXCoreServices*)m_pCoreServices)->GetStringX(uIndex, rValue); 
}

void CIFXCoreServicesRef::GetPersistenceX(U32 uIndex, BOOL& rPersistence)
{
	((CIFXCoreServices*)m_pCoreServices)->GetPersistenceX(uIndex, rPersistence); 
}

void CIFXCoreServicesRef::SetBinaryValueX(const IFXString& rKey, U32 length, const U8* data)
{
	((CIFXCoreServices*)m_pCoreServices)->SetBinaryValueX(rKey, length, data); 
}

void CIFXCoreServicesRef::SetStringValueX(const IFXString& rKey, const IFXString& rValue)
{
	((CIFXCoreServices*)m_pCoreServices)->SetStringValueX(rKey, rValue); 
}

void CIFXCoreServicesRef::SetPersistenceX(U32 uIndex, BOOL value)
{
	((CIFXCoreServices*)m_pCoreServices)->SetPersistenceX(uIndex, value); 
}

void CIFXCoreServicesRef::DeleteX(U32 uIndex)
{
	((CIFXCoreServices*)m_pCoreServices)->DeleteX(uIndex); 
}

void CIFXCoreServicesRef::DeleteAll()
{
	((CIFXCoreServices*)m_pCoreServices)->DeleteAll(); 
}

void CIFXCoreServicesRef::AppendX(IFXMetaDataX* pSource)
{
	((CIFXCoreServices*)m_pCoreServices)->AppendX(pSource); 
}

void CIFXCoreServicesRef::GetEncodedKeyX(U32 uIndex, IFXString& rOutKey)
{
	((CIFXCoreServices*)m_pCoreServices)->GetEncodedKeyX(uIndex, rOutKey);
}

void CIFXCoreServicesRef::GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes)
{
	((CIFXCoreServices*)m_pCoreServices)->GetSubattributesCountX(uIndex, rCountSubattributes);
}

IFXRESULT CIFXCoreServicesRef::GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex)
{
	return ((CIFXCoreServices*)m_pCoreServices)->GetSubattributeIndex(uIndex, rSubattributeName, rSubattributeIndex);
}

void CIFXCoreServicesRef::GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName)
{
	((CIFXCoreServices*)m_pCoreServices)->GetSubattributeNameX(uIndex, uSubattributeIndex, rSubattributeName);
}

void CIFXCoreServicesRef::GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue)
{
	((CIFXCoreServices*)m_pCoreServices)->GetSubattributeValueX(uIndex, uSubattributeIndex, pSubattributeValue);
}

void CIFXCoreServicesRef::SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue)
{
	((CIFXCoreServices*)m_pCoreServices)->SetSubattributeValueX(uIndex, rSubattributeName, pSubattributeValue);
}

void CIFXCoreServicesRef::DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex)
{
	((CIFXCoreServices*)m_pCoreServices)->DeleteSubattributeX(uIndex, uSubattributeIndex);
}

void CIFXCoreServicesRef::DeleteAllSubattributes(U32 uIndex)
{
	((CIFXCoreServices*)m_pCoreServices)->DeleteAllSubattributes(uIndex);
}
