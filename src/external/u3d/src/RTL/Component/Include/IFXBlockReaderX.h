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
	@file	IFXBlockReaderX.h

			Declaration of IFXBlockReaderX interface.

	@note	The IFXBlockReaderX interface is supported by the CIFXBlockReaderX
			class and is used to read blocks of data from a read buffer
*/

#ifndef IFXBLOCKREADERX_H__
#define IFXBLOCKREADERX_H__

#include "IFXReadBufferX.h"
#include "IFXCoreServices.h"
#include "IFXDataBlockX.h"

// {17C0B170-7177-4000-850E-C9ED9CE164DA}
IFXDEFINE_GUID(IID_IFXBlockReaderX,
0x17c0b170, 0x7177, 0x4000, 0x85, 0xe, 0xc9, 0xed, 0x9c, 0xe1, 0x64, 0xda);

/** 
	The IFXBlockReaderX interface is supported by the CIFXBlockReaderX
	class and is used to read blocks of data from a read buffer. 
*/
class IFXBlockReaderX : public IFXUnknown
{
public:
	/** Initializes IFXBlockReader. */
	virtual void IFXAPI  InitializeX(IFXCoreServices& rCoreServices, IFXReadBufferX& rReadBufferX, U32 uReadPosition = 0) = 0;
	/** Reads the block of data. */
	virtual void IFXAPI  ReadBlockX(IFXDataBlockX*& rpDataBlockX, IFXRESULT& rWarningCode) = 0;
	/** Get total file size from header block **/
	virtual void IFXAPI  GetFileSizeX(U64& rFileSize) = 0;
	/** Get current read position (same as loaded bytes) **/
	virtual void IFXAPI  GetFileSizeLoadedX(U64& rFileSizeLoaded) = 0;
	/** Get file type and version **/
	virtual void IFXAPI  GetFileVersionX(U32& rFileType, U32& rFileVersion) = 0;
};

#endif
