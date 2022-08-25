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
  @file MeshResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef MeshResource_H
#define MeshResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXArray.h"
#include "ModelResource.h"
#include "Int3.h"
#include "ShadingDescriptionList.h"

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

struct FaceTexCoords
{
	IFXArray< Int3 > m_texCoords; // number of texture layers for this face
};

/**
*/
class MeshResource : public ModelResource
{
public:
	MeshResource() {};
	~MeshResource() {};

	I32 faceCount;
	IFXArray< Int3 > m_facePositions; // number of faces
	IFXArray< Int3 > m_faceNormals; // number of faces
	IFXArray< I32 >  m_faceShaders; // number of faces
	IFXArray< Int3 > m_faceDiffuseColors; // number of faces
	IFXArray< Int3 > m_faceSpecularColors; // number of faces
	IFXArray< FaceTexCoords > m_faceTextureCoords; // number of faces
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
