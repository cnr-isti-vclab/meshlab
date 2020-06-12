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
@file UrlList.h

This header defines the ... functionality.

@note
*/


#ifndef UrlList_H
#define UrlList_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "IFXArray.h"

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


/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class UrlList
{
public:
	UrlList();
	virtual ~UrlList();

	/**
	*/
	void AddUrl( const IFXString& rUrl );

	/**
	*/
	const IFXString& GetUrl( U32 index ) const;

	/**
	*/
	const U32& GetUrlCount() const;

	/**
	*/
	const IFXArray<IFXString>& GetUrlList() const;

private:
	IFXArray<IFXString> m_urlList;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE UrlList::UrlList()
{
}

IFXFORCEINLINE UrlList::~UrlList()
{
}

IFXFORCEINLINE void UrlList::AddUrl( const IFXString& rUrl )
{
	IFXString& url = m_urlList.CreateNewElement();
	url = rUrl;
}

IFXFORCEINLINE const IFXString& UrlList::GetUrl( U32 index ) const
{
	return m_urlList.GetElementConst( index );
}

IFXFORCEINLINE const U32& UrlList::GetUrlCount() const
{
	return m_urlList.GetNumberElements();
}

IFXFORCEINLINE const IFXArray<IFXString>& UrlList::GetUrlList() const
{
	return m_urlList;
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
