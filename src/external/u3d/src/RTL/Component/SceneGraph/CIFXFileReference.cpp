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
//  CIFXFileReference.cpp
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************

#include "IFXSceneGraphPCH.h"
#include "CIFXFileReference.h"


#include "IFXExportingCIDs.h"

CIFXFileReference::CIFXFileReference()
{
	m_uRefCount = 0;
}

CIFXFileReference::~CIFXFileReference()
{
}

// IFXFileReference
void CIFXFileReference::SetScopeName(const IFXString& ScopeName)
{
	m_FileReferenceParams.ScopeName = ScopeName;
}

void CIFXFileReference::GetScopeName(IFXString& rScopeName)
{
	rScopeName = m_FileReferenceParams.ScopeName;
}

void CIFXFileReference::SetFileURLs(const IFXFileURLs& FileURLs)
{
	m_FileReferenceParams.FileURLs = FileURLs;
}

void CIFXFileReference::GetFileURLs(IFXFileURLs& rFileURLs)
{
	rFileURLs = m_FileReferenceParams.FileURLs;
}

void CIFXFileReference::SetObjectFilters(const IFXObjectFilters& ObjectFilters)
{
	m_FileReferenceParams.ObjectFilters = ObjectFilters;
}

void CIFXFileReference::GetObjectFilters(IFXObjectFilters& rObjectFilters)
{
	rObjectFilters = m_FileReferenceParams.ObjectFilters;
}

void CIFXFileReference::SetCollisionPolicy(const IFXCollisionPolicy& CollisionPolicy)
{
	m_FileReferenceParams.CollisionPolicy = CollisionPolicy;
}

void CIFXFileReference::GetCollisionPolicy(IFXCollisionPolicy& rCollisionPolicy)
{
	rCollisionPolicy = m_FileReferenceParams.CollisionPolicy;
}

void CIFXFileReference::SetWorldAlias(const IFXString& WorldAlias)
{
	m_FileReferenceParams.WorldAlias = WorldAlias;
}

void CIFXFileReference::GetWorldAlias(IFXString& rWorldAlias)
{
	rWorldAlias = m_FileReferenceParams.WorldAlias;
}

// IFXMarkerX
void CIFXFileReference::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXFileReferenceEncoder, rpEncoderX);
}

// IFXUnknown
U32 CIFXFileReference::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXFileReference::Release()
{
	if (m_uRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
		return (--m_uRefCount);
}

IFXRESULT CIFXFileReference::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXMetaDataX ) 
			*ppInterface = ( IFXMetaDataX* ) this;
		else if ( interfaceId == IID_IFXFileReference ) 
			*ppInterface = ( IFXFileReference* ) this;
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

	return result;
}

IFXRESULT IFXAPI_CALLTYPE CIFXFileReference_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXFileReference *pFileReference = new CIFXFileReference;

		if (pFileReference)
		{
			// Perform a temporary AddRef for our usage of the component.
			pFileReference->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pFileReference->QueryInterface(riid, ppv);

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pFileReference->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}
