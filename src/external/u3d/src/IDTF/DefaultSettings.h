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
  @file DefaultSettings.h

      This header defines the ... functionality.

  @note
*/


#ifndef DefaultSettings_H
#define DefaultSettings_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include <float.h> // for FLT_MAX, MS specific

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************

static const F32 VIEW_PORT_WIDTH = 800;
static const F32 VIEW_PORT_HEIGHT = 600;
static const F32 VIEW_PORT_H_POSITION = 0;
static const F32 VIEW_PORT_V_POSITION = 0;
static const F32 VIEW_NEAR_CLIP = 1.0f;
static const F32 VIEW_FAR_CLIP = FLT_MAX;

static const I32 DEFAULT_RESOURCE_ID = -1;

//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

//***************************************************************************
//  Failure return codes
//***************************************************************************

}

#endif
