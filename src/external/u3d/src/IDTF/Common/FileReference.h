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
@file FileReference.h

This header defines the ... functionality.

@note
*/


#ifndef FileReference_H
#define FileReference_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "IFXArray.h"
#include "UrlList.h"

namespace U3D_IDTF
{
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


class Filter
{
public:
	Filter();
	virtual ~Filter();

	/**
	*/
	void SetType( const IFXString& rType );

	/**
	*/
	const IFXString& GetType() const;

	/**
	*/
	void SetObjectName( const IFXString& rName );

	/**
	*/
	const IFXString& GetObjectName() const;

	/**
	*/
	void SetObjectType( const I32 type );

	/**
	*/
	const I32& GetObjectType() const;

private:
	IFXString m_type;
	IFXString m_objectName;
	I32 m_objectType;
};


/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class FileReference
{
public:
	FileReference();
	virtual ~FileReference();

	/**
	*/
	void SetScopeName( const IFXString& rName );

	/**
	*/
	const IFXString& GetScopeName() const;

	/**
	*/
	void AddUrlList( const UrlList& rUrlList );

	/**
	*/
	const IFXArray<IFXString>& GetUrlList() const;

	/**
	*/
	IFXRESULT AddFilter( const Filter& rUrl );

	/**
	*/
	const Filter& GetFilter( U32 index ) const;

	/**
	*/
	const U32& GetFilterCount() const;

	/**
	*/
	void SetCollisionPolicy( const IFXString& rName );

	/**
	*/
	const IFXString& GetCollisionPolicy() const;

	/**
	*/
	void SetWorldAlias( const IFXString& rWorldAlias );

	/**
	*/
	const IFXString& GetWorldAlias() const;

private:
	IFXString m_scopeName;
	UrlList m_urlList;
	IFXArray<Filter> m_filterList;
	IFXString m_nameCollisionPolicy;
	IFXString m_worldAlias;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
