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
	@file	CIFXStdioReadBufferX.h

			Declaration of the CIFXStdioReadBufferX class which implements the 
			IFXReadBuffer, IFXReadBufferX and IFXStdio interfaces.

	@note	This class is intended for use by an exporter.
*/
#ifndef CIFXSTDIOREADBUFFERX_H__
#define CIFXSTDIOREADBUFFERX_H__

#include <stdio.h>
#include "IFXReadBuffer.h"
#include "IFXReadBufferX.h"
#include "IFXStdio.h"

class CIFXStdioReadBufferX : public IFXReadBuffer, public IFXReadBufferX, public IFXStdio
{
public:
	// IFXReadBuffer
	virtual IFXRESULT IFXAPI  Read( U8* pBytes, U64 position, U32 count );
	virtual IFXRESULT IFXAPI  GetTotalSize( U64* pCount );
	virtual IFXRESULT IFXAPI  GetAvailableSize( U64* pCount );

	// IFXReadBufferX
	virtual void IFXAPI  ReadX( U8* pBytes, U64 uPosition, U32 uCount, IFXRESULT& rWarningCode );
	virtual void IFXAPI  GetTotalSizeX( U64& rCount );
	virtual void IFXAPI  GetAvailableSizeX( U64& rCount );

	// IFXStdio
	virtual IFXRESULT IFXAPI  Open(const IFXCHAR *pFilename);
	virtual IFXRESULT IFXAPI  Close();

	// IFXUnknown
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void** ppv );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXStdioReadBufferX_Factory( IFXREFIID	interfaceId, 
											  void**	ppInterface );
private:
	// Private to enforce the use of the create instance method
	CIFXStdioReadBufferX();
	virtual ~CIFXStdioReadBufferX();

	// PRIVATE MEMBER VARIABLES
	U32		m_uRefCount;		// Reference counter
	FILE*	m_pFile;
};

#endif
