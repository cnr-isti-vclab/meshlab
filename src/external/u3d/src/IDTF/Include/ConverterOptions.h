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
@file ConverterOptions.h

This header defines the ... functionality.

@note
*/


#ifndef ConverterOptions_H
#define ConverterOptions_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXExportOptions.h"
#include "IFXString.h"

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
struct ConverterOptions
{
	U32 positionQuality;
	U32 texCoordQuality;
	U32 normalQuality;
	U32 diffuseQuality;
	U32 specularQuality;
	U32 geoQuality;
	U32 textureQuality;
	U32 animQuality;
	U32 textureLimit;
	BOOL removeZeroAreaFaces;
	F32 zeroAreaFaceTolerance;
	BOOL excludeNormals;
};

struct FileOptions
{
	IFXString   inFile;
	IFXString   outFile;
	IFXExportOptions    exportOptions;
	U32 profile;
	F32 scalingFactor;
	U32 debugLevel;
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
