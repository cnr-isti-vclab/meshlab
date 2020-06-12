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
// CIFXStdioReadBufferX.cpp
//
// DESCRIPTION:
// Implementation of the CIFXStdioReadBufferX class which implements the
// IFXReadBuffer, IFXReadBufferX and IFXStdio interfaces.
//
// NOTES:
// This class is intended for use by an exporter.
//*****************************************************************************
#include "CIFXStdioReadBufferX.h"
#include "IFXCheckX.h"
#include "IFXOSLoader.h"
#include <stdlib.h>

/// @todo: Change use of IFX_E_INVALID_FILE to more appropriate error code.

// IFXReadBuffer
IFXRESULT CIFXStdioReadBufferX::Read( U8* pBytes, U64 position, U32 count )
{
	IFXRESULT rc = IFX_OK;

	try
	{
		// Check the input pointer and file pointer
		if(NULL == pBytes) {
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}

		// Note the check for the file pointer (m_pFile) is done in GetTotalSizeX

		ReadX(pBytes,position,count,rc);

		if(IFX_W_END_OF_FILE == rc) {
			rc = IFX_E_END_OF_FILE; // Convert end of file warning to an error
		}
	}

	catch(IFXException e)
	{
		rc = e.GetIFXResult();
	}

	IFXRETURN(rc);
}

void CIFXStdioReadBufferX::ReadX( U8* pBytes, U64 uPosition, U32 uCount, IFXRESULT& rWarningCode )
{
	rWarningCode = IFX_OK;
	U32 uBytesRead = 0;

	// Check the input pointer and file pointer
	if(NULL == pBytes) {
		IFXCHECKX(IFX_E_INVALID_POINTER);
	} else if (NULL == m_pFile) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// Seek to position
	int iReturn = fseek(m_pFile, (U32)uPosition, SEEK_SET);

	if(0 == iReturn) {  // fseek will return 0 if successful
		// Read the data
		uBytesRead = (U32) fread(pBytes,1,uCount,m_pFile);
	}

	// if there was an error (fseek or fread failed)
	if(0 != iReturn || uCount != uBytesRead) {
		// Check for read past end of file
		U64 uTotalSize = 0;
		GetTotalSizeX(uTotalSize);

		if(uPosition >= uTotalSize) {
			rWarningCode = IFX_W_END_OF_FILE;
		} else {
			IFXCHECKX(IFX_E_INVALID_FILE);
		}
	}
	return;
}

IFXRESULT CIFXStdioReadBufferX::GetTotalSize( U64* pCount )
{
	IFXRESULT rc = IFX_OK;

	try {
		// Check the input pointer
		if (NULL == pCount)
		{
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}
		// Note the check for the file pointer (m_pFile) is done in GetTotalSizeX

		GetTotalSizeX(*pCount);
	}

	catch(IFXException e) {
		rc = e.GetIFXResult();
	}

	IFXRETURN(rc);
}

void CIFXStdioReadBufferX::GetTotalSizeX( U64& rCount )
{
	// Check the file pointer
	if (NULL == m_pFile) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// Seek to end
	int iReturn = fseek(m_pFile, 0, SEEK_END);

	// Get position of end
	long lPosition = ftell(m_pFile);

	// check for error
	if(0 != iReturn || -1 == lPosition) {
		IFXCHECKX(IFX_E_INVALID_FILE);
	}

	// set output value
	rCount = (U64) lPosition;

	return;
}

IFXRESULT CIFXStdioReadBufferX::GetAvailableSize( U64* pCount )
{
	IFXRESULT rc = IFX_OK;

	try {
		// Check the input pointer and file pointer
		if(NULL == pCount) {
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}
		// Note the check for the file pointer (m_pFile) is done in GetAvailableSizeX

		GetAvailableSizeX(*pCount);
	}

	catch (IFXException e) {
		rc = e.GetIFXResult();
	}

	IFXRETURN(rc);
}

void CIFXStdioReadBufferX::GetAvailableSizeX( U64& rCount )
{
	// For StdioReadBufferX, available size and total size are the same.
	GetTotalSizeX(rCount);
}

// IFXStdio
IFXRESULT CIFXStdioReadBufferX::Open(const IFXCHAR *pFilename)
{
	IFXRESULT rc = IFX_OK;

	// Close any previously opened file
	if (NULL != m_pFile) 
	{
		rc = fclose(m_pFile);
		m_pFile = NULL;
		if(0 != rc) 
			rc = IFX_E_INVALID_FILE;
	}

	// Open the file
	if(IFXSUCCESS(rc)) 
	{
		m_pFile = IFXOSFileOpen( pFilename, L"rb" );
		if(NULL == m_pFile)
			rc = IFX_E_INVALID_FILE;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXStdioReadBufferX::Close()
{
	IFXRESULT rc = IFX_OK;

	if (NULL != m_pFile) {
		rc = fclose(m_pFile);
		m_pFile = NULL;
		if(0 != rc) {
			rc = IFX_E_INVALID_FILE;
		}
	}

	IFXRETURN(rc);
}

// IFXUnknown
U32 CIFXStdioReadBufferX::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXStdioReadBufferX::Release( void )
{
	if (1 == m_uRefCount) {
		delete this;
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXStdioReadBufferX::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if (ppInterface) {
		if (interfaceId == IID_IFXReadBufferX) {
			*ppInterface = ( IFXReadBufferX* ) this;
			this->AddRef();
		} else if ( interfaceId == IID_IFXStdio ) {
			*ppInterface = ( IFXStdio* )this;
			this->AddRef();
		} else if ( interfaceId == IID_IFXReadBuffer ) {
			*ppInterface = ( IFXReadBuffer* )this;
			this->AddRef();
		} else if ( interfaceId == IID_IFXUnknown ) {
			*ppInterface = ( IFXUnknown* )this;
			this->AddRef();
		} else {
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXStdioReadBufferX_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if(ppInterface) {
		// Create the CIFXStdioReadBufferX component.
		CIFXStdioReadBufferX *pComponent = new CIFXStdioReadBufferX;

		if (pComponent) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface(interfaceId, ppInterface);

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

CIFXStdioReadBufferX::CIFXStdioReadBufferX()
{
	m_uRefCount = 0;
	m_pFile = NULL;
}

CIFXStdioReadBufferX::~CIFXStdioReadBufferX()
{
	if(NULL != m_pFile) {
		fclose(m_pFile);
		m_pFile = NULL;
	}
}
