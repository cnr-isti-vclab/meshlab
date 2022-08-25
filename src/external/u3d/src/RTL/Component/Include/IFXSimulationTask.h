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
	@file	IFXSimulationTask.h

			This header file defines the IFXSimulationTask interface and its functionality.
*/

#ifndef __IFXSimulationTask_H__
#define __IFXSimulationTask_H__

#include "IFXTask.h"
#include "IFXUnknown.h"

// {A8F70713-E8DA-4e97-818E-27D520634DBE}
IFXDEFINE_GUID(CID_IFXSimulationTask,
0xa8f70713, 0xe8da, 0x4e97, 0x81, 0x8e, 0x27, 0xd5, 0x20, 0x63, 0x4d, 0xbe);

// {5D10DBD9-542F-478c-B195-6F66B0E66C4C}
IFXDEFINE_GUID(IID_IFXSimulationTask,
0x5d10dbd9, 0x542f, 0x478c, 0xb1, 0x95, 0x6f, 0x66, 0xb0, 0xe6, 0x6c, 0x4c);


/**
	This is the main interface for IFXSimulationTask.

	IFXSimulationTask is a bare interface. It's purpose is to act as a placeholder
	in the Scenegraph palette for simulation tasks which implement the IFXTask
	interface.

	@note	The associated IID is named IID_IFXSimulationTask.
			The associated CID is named CID_IFXSimulationTask.
*/
class IFXSimulationTask : virtual public IFXTask
{
public:
	// IFXSimulationTask
};

#endif
