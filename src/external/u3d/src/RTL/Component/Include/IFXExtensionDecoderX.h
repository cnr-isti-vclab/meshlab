//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
@file IFXExtensionDecoderX.h
	Declaration of IFXExtensionDecoderX interface.
	This interface is used by the Load Manager to provide data blocks to a decoder.
*/
//*****************************************************************************
#ifndef IFXExtensionDecoderX_H__
#define IFXExtensionDecoderX_H__

#include "IFXCoreServices.h"
#include "IFXDataBlockX.h"
#include "IFXNameMap.h"
#include "IFXSceneGraph.h"
#include "IFXDecoderX.h"

// {008AFCC9-56A9-413d-980A-1682C7E53726}
IFXDEFINE_GUID(IID_IFXExtensionDecoderX, 
0x8afcc9, 0x56a9, 0x413d, 0x98, 0xa, 0x16, 0x82, 0xc7, 0xe5, 0x37, 0x26);

class  IFXExtensionDecoderX : virtual public IFXDecoderX
{
public:
	virtual void IFXAPI  SetContBlockTypes( U32 *blockTypes, U32 typesCount ) = 0;
};

#endif
