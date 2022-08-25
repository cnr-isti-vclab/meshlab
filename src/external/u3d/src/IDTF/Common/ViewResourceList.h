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
@file ViewResourceList.h

This header defines the ... functionality.

@note
*/


#ifndef ViewResourceList_H
#define ViewResourceList_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "ResourceList.h"
#include "ViewResource.h"

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
class ViewResourceList : public ResourceList
{
public:
	ViewResourceList() {};
	virtual ~ViewResourceList() {};

	/**
	*/
	void AddResource( const ViewResource& rResource );

	/**
	*/
	const ViewResource& GetResource( U32 index ) const;

	U32 GetResourceCount() const;

protected:

private:
	IFXArray< ViewResource > m_resourceList;
};


//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void ViewResourceList::AddResource( const ViewResource& rResource )
{
	ViewResource& resource = m_resourceList.CreateNewElement();
	resource = rResource;
}

IFXFORCEINLINE const ViewResource& ViewResourceList::GetResource( U32 index ) const
{
	return m_resourceList.GetElementConst( index );
}

IFXFORCEINLINE U32 ViewResourceList::GetResourceCount() const
{
	return m_resourceList.GetNumberElements();
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
