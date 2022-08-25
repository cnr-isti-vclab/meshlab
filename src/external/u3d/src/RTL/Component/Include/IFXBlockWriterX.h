//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
	@file	IFXBlockWriterX.h

			Declaration of IFXBlockWriterX interface.

	@note	The IFXBlockWriterX interface is supported by the CIFXBlockWriterX
			class and is used to write blocks of data to a write buffer.
*/

#ifndef IFXBLOCKWRITERX_H__
#define IFXBLOCKWRITERX_H__

#include "IFXWriteBufferX.h"
#include "IFXCoreServices.h"
#include "IFXDataBlockX.h"

// {83EBA7DE-D7DB-4267-905E-7E0123537073}
IFXDEFINE_GUID(IID_IFXBlockWriterX,
0x83eba7de, 0xd7db, 0x4267, 0x90, 0x5e, 0x7e, 0x1, 0x23, 0x53, 0x70, 0x73);

/** 
	The IFXBlockWriterX interface is supported by the CIFXBlockWriterX
	class and is used to write blocks of data to a write buffer. 
*/
class IFXBlockWriterX : public IFXUnknown
{
public:
	/** Initializes IFXBlockWriterX. */
	virtual void IFXAPI  InitializeX(IFXCoreServices& rCoreServices, IFXWriteBufferX& rWriteBufferX, U32 uWritePosition = 0) = 0;
	/** Writes the block of data. */
	virtual void IFXAPI  WriteBlockX(IFXDataBlockX& rDataBlockX) = 0;
};

#endif
