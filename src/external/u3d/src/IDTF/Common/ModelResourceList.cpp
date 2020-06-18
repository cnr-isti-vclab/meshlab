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
  @file ModelResourceList.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ModelResourceList.h"
#include "Tokens.h"

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

ModelResourceList::ModelResourceList() 
{
}

ModelResourceList::~ModelResourceList()
{
}

IFXRESULT ModelResourceList::AddResource( const ModelResource* pModelResource )
{
	IFXRESULT result = IFX_OK;
	ModelResource* pTmpModelResource = NULL;

	const IFXString& rModelResourceType = pModelResource->m_type;

	if( rModelResourceType == IDTF_MESH )
	{
		MeshResource& meshResource = m_meshResourceList.CreateNewElement();
		meshResource = *static_cast<const MeshResource*>(pModelResource);
		pTmpModelResource = &meshResource;
	}
	else if( rModelResourceType == IDTF_LINE_SET )
	{
		LineSetResource& lineSetResource = m_lineSetResourceList.CreateNewElement();
		lineSetResource = *static_cast<const LineSetResource*>(pModelResource);
		pTmpModelResource = &lineSetResource;
	}
	else if( rModelResourceType == IDTF_POINT_SET )
	{
		PointSetResource& pointSetResource = m_pointSetResourceList.CreateNewElement();
		pointSetResource = *static_cast<const PointSetResource*>(pModelResource);
		pTmpModelResource = &pointSetResource;
	}
	else
		result = IFX_E_UNDEFINED;

	if( IFXSUCCESS( result ) )
	{
		// add new node pointer to the model resource list
		ModelResource*& resource = m_resourcePointerList.CreateNewElement();
		resource = pTmpModelResource;
	}

	return result;
}

const ModelResource* ModelResourceList::GetResource( U32 index ) const
{
	return m_resourcePointerList.GetElementConst( index );
}

U32 ModelResourceList::GetResourceCount() const
{
	return m_resourcePointerList.GetNumberElements();
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
