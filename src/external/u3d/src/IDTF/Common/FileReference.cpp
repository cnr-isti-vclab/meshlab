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
  @file FileReference.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "FileReference.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

FileReference::FileReference() 
{
}

FileReference::~FileReference()
{
}

void FileReference::SetScopeName( const IFXString& rName )
{
	m_scopeName = rName;
}

const IFXString& FileReference::GetScopeName() const
{
	return m_scopeName;
}

void FileReference::AddUrlList( const UrlList& rUrlList )
{
	m_urlList = rUrlList;
}

const IFXArray<IFXString>& FileReference::GetUrlList() const
{
	return m_urlList.GetUrlList();
}

IFXRESULT FileReference::AddFilter( const Filter& rFilter )
{
	IFXRESULT result = IFX_OK;

	Filter& filter = m_filterList.CreateNewElement();

	const IFXString& rType = rFilter.GetType();
	filter.SetType( rType );

	if( rType == "NAME" )
	{
		filter.SetObjectName( rFilter.GetObjectName() );
	}
	else if( rType == "TYPE" )
	{
		filter.SetObjectType( rFilter.GetObjectType() );
	}
	else
		result = IFX_E_UNSUPPORTED;

	return result;
}

const Filter& FileReference::GetFilter( U32 index ) const
{
	return m_filterList.GetElementConst( index );
}

const U32& FileReference::GetFilterCount() const
{
	return m_filterList.GetNumberElements();
}

void FileReference::SetCollisionPolicy( const IFXString& rName )
{
	m_nameCollisionPolicy = rName;
}

const IFXString& FileReference::GetCollisionPolicy() const
{
	return m_nameCollisionPolicy;
}

void FileReference::SetWorldAlias( const IFXString& rWorldAlias )
{
	m_worldAlias = rWorldAlias;
}

const IFXString& FileReference::GetWorldAlias() const
{
	return m_worldAlias;
}

Filter::Filter()
: m_objectType( 0 )
{
}

Filter::~Filter()
{
}

void Filter::SetType( const IFXString& rType )
{
	m_type = rType;
}

const IFXString& Filter::GetType() const
{
	return m_type;
}

void Filter::SetObjectName( const IFXString& rName )
{
	m_objectName = rName;
}

const IFXString& Filter::GetObjectName() const
{
	return m_objectName;
}

void Filter::SetObjectType( const I32 type )
{
	m_objectType = type;
}

const I32& Filter::GetObjectType() const
{
	return m_objectType;
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
