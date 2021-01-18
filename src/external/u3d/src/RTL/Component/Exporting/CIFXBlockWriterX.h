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
	@file	CIFXBlockWriterX.h

			Declaration of the CIFXBlockWriterX class which implements the 
			IFXBlockWriterX and IFXBlockWriter interface.

	@note	InitializeX - Sets up the Block Writer with a WriteBuffer
			WriteBlockX - Writes a block of data from the DataBlock into the
						  WriteBuffer
*/
#ifndef CIFXBLOCKWRITERX_H__
#define CIFXBLOCKWRITERX_H__

#include "IFXBlockWriterX.h"
#include "IFXAutoRelease.h"

class CIFXBlockWriterX : public IFXBlockWriterX
{
public:
	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void** ppv );

	// IFXBlockWriterX methods
	virtual void IFXAPI  InitializeX(IFXCoreServices& rCoreServices, IFXWriteBufferX& rWriteBufferX, U32 uWritePosition);
	virtual void IFXAPI  WriteBlockX(IFXDataBlockX& rDataBlockX);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXBlockWriterX_Factory( IFXREFIID interfaceId, void** ppInterface );

private:
	// Private to enforce the use of the create instance method
	CIFXBlockWriterX();
	virtual ~CIFXBlockWriterX();

	// PRIVATE MEMBER VARIABLES
	U32	m_uRefCount;		///< Reference counter
	U64	m_uWritePosition;	///< Current position in the WriteBuffer
	U32 m_uMaxPriority;
	BOOL m_bWithoutFileHeader;
	IFXDECLAREMEMBER(IFXWriteBufferX,m_pWriteBufferX); ///< Local pointer to the passed in WriteBuffer
};

#endif
