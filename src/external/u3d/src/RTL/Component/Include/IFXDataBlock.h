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
	@file	IFXDataBlock.h

			Declaration of IFXDataBlock interface.

	@note	The IFXBlockReader interface is supported by the CIFXDataBlock 
			class and is used to hold data that has been read in by a data block
			reader
*/

#ifndef __IFXDATABLOCK_H__
#define __IFXDATABLOCK_H__

#include "IFXUnknown.h"

// {9DCB6A23-9DE3-11d3-98A5-00A0C9902D25}
IFXDEFINE_GUID(IID_IFXDataBlock,
0x9dcb6a23, 0x9de3, 0x11d3, 0x98, 0xa5, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

class IFXDataBlock : virtual public IFXUnknown
{
public:

	// IFXDataBlock
	virtual IFXRESULT IFXAPI  SetSize( U32 count ) = 0;
	virtual IFXRESULT IFXAPI  GetSize( U32* pCount ) = 0;
	virtual IFXRESULT IFXAPI  SetBlockType( U32 BlockType ) = 0;
	virtual IFXRESULT IFXAPI  GetBlockType( U32* pBlockType ) = 0;
	virtual IFXRESULT IFXAPI  GetPointer( U8** ppData ) = 0;

	// These methods are similar to IFXMarker
	virtual U32 IFXAPI  GetPriority(void) = 0;
	virtual void IFXAPI  SetPriority(U32 uPriority) = 0;

};


#endif
