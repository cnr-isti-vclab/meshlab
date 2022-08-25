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
	@file	IFXExportOptions.h

			This header defines the IFXExportOptions enumerated type.

	@note	It is used by IFXWriteManager and can be used by Converters.
*/


#ifndef IFXExportOptions_H
#define IFXExportOptions_H


//***************************************************************************
//	Includes
//***************************************************************************


//***************************************************************************
//	Defines
//***************************************************************************


//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************

enum
{
	IFXEXPORT_NONE			 = 0x0000,

	IFXEXPORT_ANIMATION      = 0x0001,
	IFXEXPORT_GEOMETRY       = 0x0002,
	IFXEXPORT_LIGHTS         = 0x0004,
	IFXEXPORT_MATERIALS      = 0x0008,
	IFXEXPORT_NODE_HIERARCHY = 0x0010,
	IFXEXPORT_SHADERS        = 0x0020,
	IFXEXPORT_TEXTURES       = 0x0040,
	IFXEXPORT_VIEWS          = 0x0080,
	IFXEXPORT_FILEREFERENCES = 0x0100,

	IFXEXPORT_EVERYTHING     = 0xFFFF
};

//***************************************************************************
//	Classes, structures and types
//***************************************************************************

typedef U32 IFXExportOptions;

#endif
