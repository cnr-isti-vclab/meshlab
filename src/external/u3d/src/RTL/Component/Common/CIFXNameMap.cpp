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
	@file	CIFXNameMap.cpp

			Implementation for CIFXNameMap class. 
			The CIFXNameMap implements the IFXNameMap interface 
			which is used to access a downloaded file.
*/

#include "CIFXNameMap.h"
#include "IFXCoreServices.h"
#include "IFXCoreCIDs.h"
#include <time.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
//	CIFXNameMap_Factory
//
//	This is the CIFXNameMap component factory function.  The
//	CIFXNameMap component is a singleton, since it is sharing
//	global state.
//
//	RETURNS
//		IFX_OK
//		IFX_E_OUT_OF_MEMORY
//		IFX_E_INVALID_POINTER
//		IFX_E_UNSUPPORTED
//-----------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXNameMap_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXNameMap component.
		CIFXNameMap *pComponent = new CIFXNameMap;

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

// IFXUnknown...
//---------------------------------------------------------------------------
//	CIFXNameMap::AddRef
//
//	This method increments the reference count for an interface on a
//	component.	It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXNameMap::AddRef( ) 
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXNameMap::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.	It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXNameMap::Release( ) 
{
	if ( !( --m_uRefCount ) ) 
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXNameMap::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.	Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT 
CIFXNameMap::QueryInterface( IFXREFIID interfaceId, void** ppInterface ) 
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface ) 
	{
		if (interfaceId == IID_IFXNameMap)
		{
			*ppInterface = ( IFXNameMap* ) this;
			this->AddRef();
		}
		else if (interfaceId == IID_IFXUnknown)
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else 
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN( result );
}


//---------------------------------------------------------------------------
//	CIFXNameMap::CIFXNameMap
//
//	Constructor
//---------------------------------------------------------------------------

CIFXNameMap::CIFXNameMap( ) : m_uRefCount( 0 )
{
	m_bInitialized = FALSE;
}

//---------------------------------------------------------------------------
//	CIFXNameMap::~CIFXNameMap
//
//	Destructor
//---------------------------------------------------------------------------

CIFXNameMap::~CIFXNameMap( )
{
}

//---------------------------------------------------------------------------
//	CIFXNameMap::CIFXNameMap
//---------------------------------------------------------------------------

IFXRESULT CIFXNameMap::Initialize(U32 uPalettesNum)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized) {
		m_aNameHash.ResizeToAtLeast(uPalettesNum);
		m_aNameMapHash.ResizeToAtLeast(uPalettesNum);
		m_bInitialized = TRUE;
		m_uLastLoadId = 1;
	} else {
		m_aNameHash.Clear();
		m_aNameMapHash.Clear();
		m_aNameHash.ResizeToAtLeast(uPalettesNum);
		m_aNameMapHash.ResizeToAtLeast(uPalettesNum);
	}

	return rc;
}

IFXRESULT CIFXNameMap::GenerateScopeId(U32& uScopeId)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		uScopeId = m_uLastLoadId;
		m_uLastLoadId++;
	}

	return rc;
}

IFXRESULT CIFXNameMap::NewScope(U32 uScopeId, IFXCollisionPolicy uPolicy)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (m_aScope.Has(uScopeId))
		rc = IFX_E_ALREADY_INITIALIZED;

	if ((uPolicy != IFXCOLLISIONPOLICY_DISCARD) &&
		(uPolicy != IFXCOLLISIONPOLICY_REPLACE) &&
		(uPolicy != IFXCOLLISIONPOLICY_PREPENDALL) &&
		(uPolicy != IFXCOLLISIONPOLICY_PREPENDCOLLIDED) &&
		(uPolicy != IFXCOLLISIONPOLICY_POSTMANGLE))
			rc = IFX_E_BAD_PARAM;

	if (IFXSUCCESS(rc)) {
		m_aScope[uScopeId].uCollisionPolicy = uPolicy;
		m_aScope[uScopeId].sPrefix = "copy_";
		m_aScope[uScopeId].sWorldAlias = PALETTE_DEFAULT_GROUP;
		m_aScope[uScopeId].uProfile = 0;
		m_aScope[uScopeId].units = 1.0f;
	}

	return rc;
}

IFXRESULT CIFXNameMap::DeleteScope(U32 uScopeId)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		m_aScope.Remove(uScopeId);
	}

	return rc;
}

IFXRESULT CIFXNameMap::SetPrefix(U32 uScopeId, const IFXString& sPrefix)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		m_aScope[uScopeId].sPrefix = sPrefix;
	}

	return rc;
}

IFXRESULT CIFXNameMap::SetCollisionPolicy(U32 uScopeId, IFXCollisionPolicy uPolicy)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		if ((uPolicy == IFXCOLLISIONPOLICY_DISCARD) ||
			(uPolicy == IFXCOLLISIONPOLICY_REPLACE) ||
			(uPolicy == IFXCOLLISIONPOLICY_PREPENDALL) ||
			(uPolicy == IFXCOLLISIONPOLICY_PREPENDCOLLIDED) ||
			(uPolicy == IFXCOLLISIONPOLICY_POSTMANGLE))
			m_aScope[uScopeId].uCollisionPolicy = uPolicy;
		else
			rc = IFX_E_BAD_PARAM;
	}

	return rc;
}

IFXRESULT CIFXNameMap::GetWorldAlias(U32 uScopeId, IFXString& rsWorldAlias)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		rsWorldAlias = (m_aScope[uScopeId].sWorldAlias);
	}

	return rc;
}

IFXRESULT CIFXNameMap::SetWorldAlias(U32 uScopeId, const IFXString& sWorldAlias)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		m_aScope[uScopeId].sWorldAlias = sWorldAlias;
	}

	return rc;
}

IFXRESULT CIFXNameMap::GetProfile(U32 uScopeId, U32& rProfile)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		rProfile = (m_aScope[uScopeId].uProfile);
	}

	return rc;
}

IFXRESULT CIFXNameMap::SetProfile(U32 uScopeId, const U32 uProfile)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		m_aScope[uScopeId].uProfile = uProfile;
	}

	return rc;
}

IFXRESULT CIFXNameMap::GetUnits(U32 uScopeId, F64& rUnits)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		rUnits = (m_aScope[uScopeId].units);
	}

	return rc;
}

IFXRESULT CIFXNameMap::SetUnits(U32 uScopeId, const F64 units)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		m_aScope[uScopeId].units = units;
	}

	return rc;
}

IFXRESULT CIFXNameMap::Reserve(U32 uPaletteId, const IFXString& sName)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		// check if name is already reserved
		if (m_aNameHash[uPaletteId].Has(sName))
			rc = IFX_E_KEY_ALREADY_EXISTS;
		// create entry in palette name map with instance counter = 0
		if (IFXSUCCESS(rc)) {
			m_aNameHash[uPaletteId][sName] = 0;
		}
	}

	return rc;
}

BOOL CIFXNameMap::IsExist(U32 uScopeId, U32 uPaletteId, const IFXString& sName)
{
	IFXRESULT rc = TRUE;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = FALSE;

	if (TRUE == rc) {
		IFXString sEntryName = Scope(uScopeId, sName);
		if (!m_aNameMapHash[uPaletteId].Has(sEntryName))
			rc = FALSE;
	}

	return rc;
}

IFXRESULT CIFXNameMap::AddCustom(U32 uScopeId, U32 uPaletteId, const IFXString& sOldName, const IFXString& sNewName)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		// check if names are already created before
		if (IsExist(uScopeId, uPaletteId, sOldName) || m_aNameHash[uPaletteId].Has(sNewName))
			rc = IFX_E_KEY_ALREADY_EXISTS;
		if (IFXSUCCESS(rc)) {
			// initialize new entry
			IFXNameMapEntry nme;
			nme.uScopeId = uScopeId;
			nme.sOldName = sOldName;
			nme.sNewName = sNewName;
			IFXString sEntryName = Scope(uScopeId, sOldName);
			// insert this entry into name map
			m_aNameMapHash[uPaletteId][sEntryName] = nme;
			// if old name doesn't exist in either scope, create it
			if (!m_aNameHash[uPaletteId].Has(sOldName))
				m_aNameHash[uPaletteId][sOldName] = 0;
			// and same for new name
			if (sOldName != sNewName)
				m_aNameHash[uPaletteId][sNewName] = 0;
		}
	}

	return rc;
}

IFXRESULT CIFXNameMap::Map(U32 uScopeId, U32 uPaletteId, IFXString& rsName)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc) && !rsName.IsEmpty()) 
	{
		// we should do remapping only if remapping was done before or collision policy tells us to do that
		if (m_aNameHash[uPaletteId].Has(rsName) || (IFXCOLLISIONPOLICY_PREPENDALL == m_aScope[uScopeId].uCollisionPolicy)) 
		{
			IFXNameMapEntry* nme = NULL;
			IFXString sEntryName = Scope(uScopeId, rsName);
			// if remapping was done before, return remapped entry
			if (TRUE == m_aNameMapHash[uPaletteId].Find(sEntryName, nme)) 
			{
				rsName = nme->sNewName;
			} 
			else 
			{
				// switch based on collision policy
				switch (m_aScope[uScopeId].uCollisionPolicy) 
				{
					case IFXCOLLISIONPOLICY_PREPENDALL:
					case IFXCOLLISIONPOLICY_PREPENDCOLLIDED:
					{
						// prepend scope prefix
						sEntryName = m_aScope[uScopeId].sPrefix + rsName;
						// store this remapping
						AddCustom(uScopeId, uPaletteId, rsName, sEntryName);
						rsName = sEntryName;

						break;
					}
					case IFXCOLLISIONPOLICY_DISCARD:
					case IFXCOLLISIONPOLICY_REPLACE:
						break;
					case IFXCOLLISIONPOLICY_POSTMANGLE:
					{
						U32 uInstance = m_aNameHash[uPaletteId][rsName];
						// increase instance counter suffix until entry is unique
						do 
						{
							uInstance++;
							sEntryName.ToString(uInstance, 10);
							sEntryName = rsName + sEntryName;
						} while (m_aNameHash[uPaletteId].Has(sEntryName));
						// store last instance counter
						m_aNameHash[uPaletteId][rsName] = uInstance;
						// store this remapping
						AddCustom(uScopeId, uPaletteId, rsName, sEntryName);
						rsName = sEntryName;

						break;
					}
					default:
						rc = IFX_E_BAD_PARAM;
				}
			}
		} 
		else 
		{
			// if it is first occurance of entry, don't change its name
			AddCustom(uScopeId, uPaletteId, rsName, rsName);
		}
	}

	return rc;
}

IFXRESULT CIFXNameMap::Unmap(U32 uScopeId, U32 uPaletteId, const IFXString& sName)
{
	IFXRESULT rc = IFX_OK;

	if (!m_bInitialized || !m_aScope.Has(uScopeId))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) {
		IFXString sEntryName = Scope(uScopeId, sName);
		// remove remapping record
		m_aNameMapHash[uPaletteId].Remove(sEntryName);
		// check for presence of entry in other scopes
		m_aScope.GetFirst();
		BOOL bExist = FALSE;
		do {
			sEntryName = Scope(m_aScope.GetKey(), sName);
			if (m_aNameMapHash[uPaletteId].Has(sEntryName)) {
				bExist = TRUE;
				break;
			}
		} while (m_aScope.GetNext());
		// if no scope contains same entry name, remove name from palette table
		if (!bExist)
			m_aNameHash[uPaletteId].Remove(sName);
	}

	return rc;
}

//---------------------------------------------------------------------------
//	CIFXNameMap::Scope
//
//	This method provides special mangling to differentiate names
//  from different scopes.
//---------------------------------------------------------------------------

IFXString CIFXNameMap::Scope(U32 uScopeId, const IFXString& sName) 
{
	IFXString sEntryName;
	sEntryName.ToString(uScopeId, 10);
	return sEntryName + "_" + sName;
}
