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
	@file CIFXMixerConstruct.cpp
*/

#include "IFXSceneGraphPCH.h"
#include "CIFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXException.h"

//---------------------------------------------------------------------------
//	CIFXMixerConstruct::CIFXMixerConstruct
//
//	This is the constructor.  It sets up the object's initial state.
//---------------------------------------------------------------------------

CIFXMixerConstruct::CIFXMixerConstruct() :
	m_refCount( 0 )
{
	// IFXMixerConstruct attributtes...
	m_pIFXMotionResource = 0;
	m_EntryList.SetAutoDestruct(true);
	m_Duration = 0;

	m_type = NONE;
	m_pSceneGraph = NULL;
}

//---------------------------------------------------------------------------
//	CIFXMixerConstruct::~CIFXMixerConstruct
//
//	This is the destructor.  It performs any necessary cleanup activities.
//---------------------------------------------------------------------------

CIFXMixerConstruct::~CIFXMixerConstruct()
{
	IFXRELEASE(m_pIFXMotionResource);
}


//---------------------------------------------------------------------------
//	CIFXMixerConstruct_Factory
//
//	This is the CIFXMixerConstruct component factory function.  The
//	CIFXMixerConstruct component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXMixerConstruct_Factory( IFXREFIID	interfaceId, void**	ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// Create the CIFXMixerConstruct component.
		CIFXMixerConstruct	*pComponent	= new CIFXMixerConstruct;

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

	IFXRETURN(result);
}


//---------------------------------------------------------------------------
//	CIFXMixerConstruct::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXMixerConstruct::AddRef()
{
	return ++m_refCount;
}


//---------------------------------------------------------------------------
//	CIFXMixerConstruct::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXMixerConstruct::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}


//---------------------------------------------------------------------------
//	CIFXMixerConstruct::QueryInterface
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

IFXRESULT CIFXMixerConstruct::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMixerConstruct )
			*ppInterface = ( IFXMixerConstruct* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXMetaDataX ) 
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXRETURN(result);
}

// IFXMarkerX
void		CIFXMixerConstruct::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	throw IFXException(IFX_E_UNSUPPORTED);
//	CIFXMarker::GetEncoderX(CID_IFXMotionResourceEncoder, rpEncoderX);
}

void CIFXMixerConstruct::SetMotionResource(IFXMotionResource* pIFXMotionResource)
{
	IFXRELEASE(m_pIFXMotionResource);
	m_pIFXMotionResource = pIFXMotionResource;
	if (m_pIFXMotionResource)
	{
		m_pIFXMotionResource->AddRef();
		m_pIFXMotionResource->GetDuration(&m_Duration);

		U32 numTracks = 0;
		m_pIFXMotionResource->GetTrackCount(&numTracks);
		if (0==numTracks)
			m_type = NONE;
		else if (1==numTracks)
			m_type = SINGLETRACK;
		else
			m_type = MULTITRACK;
	}
	else
	{
		m_Duration = 0;
	}
}

IFXMotionResource* CIFXMixerConstruct::GetMotionResource()
{
	IFXMotionResource* rval = 0;
	if (m_pIFXMotionResource)
	{
		m_pIFXMotionResource->AddRef();
		rval = m_pIFXMotionResource;
	}
	return rval;
}

void CIFXMixerConstruct::AddMapping( IFXMixerConstruct* pMixer, IFXString* pBoneName)
{
	IFXASSERT(pMixer && pBoneName);

	// special case: an empty bone name means full mapping
	// so, clear the list and set the motion resource
	if (pBoneName->IsEmpty())
	{
		m_EntryList.Clear();
		IFXMotionResource* pResource = NULL;
		pResource = GetMotionResource();
		SetMotionResource(pResource);
		IFXRELEASE( pResource );
	}

	// first, find the larger duration between this mixer and the added mixer
	// this will prevent circular references when calculating duration
	F32 duration = pMixer->GetDuration();
	if (duration > m_Duration) m_Duration = duration;

	// now, add the mixer to the list
//	IFXMapEntry *pEntry = m_EntryList.CreateAndAppend();
	IFXMapEntry *pEntry=*(m_EntryList.Append(new IFXMapEntry));

	pEntry->m_pMixerConstruct = pMixer;
	pMixer->AddRef();
	pEntry->m_boneName = *pBoneName;

	// correct the local type
	MotionType type = pMixer->GetType();
	if ( (MULTITRACK == type) || ((SINGLETRACK == type) && (NONE == m_type)) )
		m_type = type;
}

F32 CIFXMixerConstruct::GetDuration()
{
	return m_Duration;
}

IFXMixerConstruct::MotionType CIFXMixerConstruct::GetType()
{
	return m_type;
}
