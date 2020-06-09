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
//
// CIFXStdioWriteBuffer.h
//
// DESCRIPTION:
// Declaration of the CIFXStdioWriteBuffer class which implements the 
// IFXWriteBuffer and IFXStdio interfaces.
//
// NOTES:
// This class is intended for use by an exporter.
//*****************************************************************************
#ifndef CIFXSTDIOWRITEBUFFERX_H__
#define CIFXSTDIOWRITEBUFFERX_H__

#include <stdio.h>
#include "IFXWriteBuffer.h"
#include "IFXWriteBufferX.h"
#include "IFXStdio.h"

class CIFXStdioWriteBufferX : public IFXWriteBuffer, public IFXWriteBufferX, public IFXStdio
{
public:
	// IFXWriteBuffer
	virtual IFXRESULT IFXAPI  Write( U8* pBytes, U64 position, U32 count );

	// IFXWriteBufferX
	virtual void IFXAPI  WriteX( U8* pBytes, U64 position, U32 count );

	// IFXStdio
	virtual IFXRESULT IFXAPI  Open(const IFXCHAR *pFilename);
	virtual IFXRESULT IFXAPI  Close();

	// IFXUnknown
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void** ppv );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXStdioWriteBufferX_Factory( IFXREFIID	interfaceId, 
											  void**	ppInterface );
private:
	// Private to enforce the use of the create instance method
	CIFXStdioWriteBufferX();
	virtual ~CIFXStdioWriteBufferX();

	// PRIVATE MEMBER VARIABLES
	U32		m_uRefCount;		// Reference counter
	FILE*	m_pFile;
};

#endif
