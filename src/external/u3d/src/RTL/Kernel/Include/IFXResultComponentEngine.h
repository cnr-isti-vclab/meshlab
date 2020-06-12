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
	@file	IFXResultComponentEngine.h

	This module defines the IFXResult_ComponentEngine enumeration.  It is
	used mainly to encode and decode IFXRESULT return codes for engine
	specific components.
*/


#ifndef IFXRESULTCOMPONENTENGINE_H
#define IFXRESULTCOMPONENTENGINE_H


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXResult.h"


//***************************************************************************
//  Enumerations
//***************************************************************************


//---------------------------------------------------------------------------
//  IFXResult_ComponentEngine
//
//  Defines the major engine components that will generate return codes.
//---------------------------------------------------------------------------

/// @todo: Rename enumerator to use COMPONENT in the name that don't already.
enum IFXResult_ComponentEngine
{
	IFXRESULT_COMPONENT_AUTHORGEOM      = IFXRESULT_COMPONENT_NEXT + 0x100,
	IFXRESULT_COMPONENT_BITSTREAM,
	IFXRESULT_COMPONENT_CORE_SERVICES,
	IFXRESULT_COMPONENT_DATAPACKET,
	IFXRESULT_COMPONENT_IMAGE_TOOLS,
	IFXRESULT_COMPONENT_LOADER,
	IFXRESULT_COMPONENT_MODIFIER_DATAPACKET,
	IFXRESULT_COMPONENT_MODIFIER,
	IFXRESULT_COMPONENT_MODIFIERCHAIN,
	IFXRESULT_COMPONENT_PALETTE_MANAGER,
	IFXRESULT_COMPONENT_READ_BUFFER,
	IFXRESULT_COMPONENT_SCENEGRAPH,
	IFXRESULT_COMPONENT_SCHEDULER,
	IFXRESULT_COMPONENT_TEXTURE_MANAGER,
	IFXRESULT_COMPONENT_WRITE_BUFFER,
	IFXRESULT_COMPONENT_WRITER,
	IFXRESULT_CONVERTERS,
	IFXRESULT_NEIGHBORMESH,
	IFXRESULT_SUBDIV
};


#endif
