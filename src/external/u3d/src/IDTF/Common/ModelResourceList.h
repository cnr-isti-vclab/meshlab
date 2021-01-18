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
@file ModelResourceList.h

This header defines the ... functionality.

@note
*/


#ifndef ModelResourceList_H
#define ModelResourceList_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "ResourceList.h"
#include "ModelResource.h"
#include "MeshResource.h"
#include "LineSetResource.h"
#include "PointSetResource.h"

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
class ModelResourceList : public ResourceList
{
public:
	ModelResourceList();
	virtual ~ModelResourceList();

	/**
	*/
	IFXRESULT AddResource( const ModelResource* pModelResource );

	/**
	*/
	const ModelResource* GetResource( U32 index ) const;
	U32 GetResourceCount() const;

protected:

private:
	IFXArray< ModelResource* >    m_resourcePointerList;
	IFXArray< MeshResource >    m_meshResourceList;
	IFXArray< LineSetResource >   m_lineSetResourceList;
	IFXArray< PointSetResource >  m_pointSetResourceList;
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
