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
  @file PointSetResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef PointSetResource_H
#define PointSetResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"

#include "ModelResource.h"

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

struct PointTexCoords
{
	IFXArray< I32 > m_texCoords; // number of texture layers for this face
};

/**
*/
class PointSetResource : public ModelResource
{
public:
	PointSetResource() {};
	~PointSetResource() {};

	I32 pointCount;
	IFXArray< I32 > m_pointPositions; // number of points
	IFXArray< I32 > m_pointNormals; // number of points
	IFXArray< I32 > m_pointShaders; // number of points
	IFXArray< I32 > m_pointDiffuseColors; // number of points
	IFXArray< I32 > m_pointSpecularColors; // number of points
	IFXArray< PointTexCoords > m_pointTextureCoords; // number of points
};


//***************************************************************************
//  InPoint functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}


#endif
