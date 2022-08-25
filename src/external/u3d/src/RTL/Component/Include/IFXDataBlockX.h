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
	@file	IFXDataBlockX.h
			Declaration of IFXDataBlockX exception-based interface.
*/

#ifndef IFXDATABLOCKX_H__
#define IFXDATABLOCKX_H__

#include "IFXUnknown.h"
#include "IFXMetaDataX.h"

// {F00B0C40-2F50-46c6-94D8-C5E1366DE45C}
IFXDEFINE_GUID(IID_IFXDataBlockX,
0xf00b0c40, 0x2f50, 0x46c6, 0x94, 0xd8, 0xc5, 0xe1, 0x36, 0x6d, 0xe4, 0x5c);

class IFXDataBlockX : public IFXMetaDataX
{
public:

	virtual void IFXAPI  SetSizeX( U32 count ) = 0;
	virtual void IFXAPI  GetSizeX( U32& rCount ) = 0;
	virtual void IFXAPI  SetBlockTypeX( U32 BlockType ) = 0;
	virtual void IFXAPI  GetBlockTypeX( U32& rBlockType ) = 0;
	virtual void IFXAPI  GetPointerX( U8*& rpData ) = 0;

	virtual U32 IFXAPI  GetPriorityX(void) = 0;
	virtual void IFXAPI  SetPriorityX(U32 uPriority) = 0;
};

#endif
