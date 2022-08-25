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
  @file LightResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef LightResource_H
#define LightResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "Point.h"
#include "Color.h"

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
class LightResource : public Resource
{
public:
	IFXString m_type;
	Color m_color;
	Point m_attenuation;
	F32 m_spotAngle;
	F32 m_intensity;
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
