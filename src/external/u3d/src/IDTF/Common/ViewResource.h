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
  @file ViewResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef ViewResource_H
#define ViewResource_H

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
*/
class ViewResource : public Resource
{
public:
	void AddRootNode( const IFXString& rRootNodeName );
	const IFXString& GetRootNode( U32 index ) const;
	U32 GetRootNodeCount() const;

private:
	IFXArray< IFXString > m_rootNodeList;
};


//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void ViewResource::AddRootNode( const IFXString& rRootNodeName )
{
	IFXString& rRootNode = m_rootNodeList.CreateNewElement();
	rRootNode = rRootNodeName;
}

IFXFORCEINLINE const IFXString& ViewResource::GetRootNode( U32 index ) const
{
	return m_rootNodeList.GetElementConst( index );
}

IFXFORCEINLINE U32 ViewResource::GetRootNodeCount() const
{
	return m_rootNodeList.GetNumberElements();
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
