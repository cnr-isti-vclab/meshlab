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
	@file	CIFXBlockReaderX.h

			Declaration of the CIFXBlockReaderX class which implements the 
			IFXBlockReaderX interface.

	@note	InitializeX - Sets up the Block Reader with a ReadBuffer
			ReadBlockX  - Reads a block of data from the ReadBuffer into a DataBlock
*/
#ifndef CIFXBLOCKREADERX_H__
#define CIFXBLOCKREADERX_H__

#include "IFXBlockReaderX.h"
#include "IFXAutoRelease.h"

class CIFXBlockReaderX : public IFXBlockReaderX
{
public:
	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void** ppInterface );

	// IFXBlockReaderX methods
	virtual void IFXAPI  InitializeX(IFXCoreServices& rCoreServices, IFXReadBufferX& rReadBufferX, U32 uReadPosition);
	virtual void IFXAPI  ReadBlockX(IFXDataBlockX*& rpDataBlockX, IFXRESULT& rWarningCode);
	virtual void IFXAPI  GetFileSizeX(U64& rFileSize);
	virtual void IFXAPI  GetFileSizeLoadedX(U64& rFileSizeLoaded);
	virtual void IFXAPI  GetFileVersionX(U32& rFileType, U32& rFileVersion);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXBlockReaderX_Factory(IFXREFIID interfaceId, void**	ppInterface);

private:
	// Private to enforce the use of the factory method
	CIFXBlockReaderX();
	virtual ~CIFXBlockReaderX();

	IFXFORCEINLINE void CheckFileHeaderX(IFXRESULT& rWarningCode);

	// PRIVATE MEMBER VARIABLES
	U32	m_uRefCount;		///< Reference counter
	U64 m_uReadPosition;	///< Current position in the ReadBuffer
	IFXDECLAREMEMBER(IFXReadBufferX,m_pReadBufferX);		///< Local pointer to the ReadBuffer
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);		///< Local pointer to the ReadBuffer
	BOOL m_bHasValidFileHeader;  ///< True if ReadBuffer has valid file header
	U32 m_uFileType;
	U32 m_uFileVersion;
	U32 m_uDeclarationSize;
	U64 m_uFileSize;
	U32 m_uCharEncoding;
	U8* m_FirstBlock;
};

#endif
