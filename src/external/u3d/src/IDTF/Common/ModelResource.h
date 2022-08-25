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
  @file ModelResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef ModelResource_H
#define ModelResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXArray.h"
#include "IFXVector4.h"
#include "Resource.h"
#include "Point.h"
#include "Color.h"
#include "ShadingDescriptionList.h"
#include "ModelSkeleton.h"

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
	Model description is suffucuent to allocate space for the model data and 
	create model generator object.
*/
struct ModelDescription
{
	I32 positionCount;
	I32 basePositionCount;
	I32 normalCount;
	I32 diffuseColorCount;
	I32 specularColorCount;
	I32 textureCoordCount;
	I32 boneCount;
	I32 shadingCount;
};

/**
*/
class ModelResource : public Resource
{
public:
	ModelResource();
	~ModelResource();

	void ClearModelDescription();

	IFXString m_type;
	IFXString m_name;

	ModelDescription m_modelDescription;
	ShadingDescriptionList m_shadingDescriptions;
	IFXArray< Point > m_positions;
	IFXArray< Point > m_normals;
	IFXArray< Color > m_diffuseColors;
	IFXArray< Color > m_specularColors;
	IFXArray< IFXVector4 > m_textureCoords;
	IFXArray< I32 > m_basePositions;
	ModelSkeleton m_modelSkeleton;
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
