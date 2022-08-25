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
  @file LineSetResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef LineSetResource_H
#define LineSetResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"

#include "ModelResource.h"
#include "Int2.h"

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

struct LineTexCoords
{
	IFXArray< Int2 > m_texCoords; // number of texture layers for this face
};

/**
*/
class LineSetResource : public ModelResource
{
public:
	LineSetResource() {};
	~LineSetResource() {};

	I32 lineCount;
	IFXArray< Int2 > m_linePositions; // number of lines
	IFXArray< Int2 > m_lineNormals; // number of lines
	IFXArray< I32 >  m_lineShaders; // number of lines
	IFXArray< Int2 > m_lineDiffuseColors; // number of lines
	IFXArray< Int2 > m_lineSpecularColors; // number of lines
	IFXArray< LineTexCoords > m_lineTextureCoords; // number of lines
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
