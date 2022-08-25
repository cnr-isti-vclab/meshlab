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
// CIFXStdioWriteBufferX.h
//
// DESCRIPTION:
// Implementation of the CIFXStdioWriteBufferX class which implements the
// IFXWriteBuffer, IFXWriteBufferX and IFXStdio interfaces.
//
// NOTES:
// This class is intended for use by an exporter.
//*****************************************************************************
#include "IFXException.h"
#include "CIFXStdioWriteBufferX.h"
#include "IFXOSLoader.h"
#include <stdlib.h>

/// @todo: Change use of IFX_E_INVALID_FILE to more appropriate error code.

// IFXWriteBuffer
IFXRESULT CIFXStdioWriteBufferX::Write( U8* pBytes, U64 position, U32 count )
{
	IFXRESULT rc = IFX_OK;

	try
	{
		// Check the input pointer and file pointer
		if(NULL == pBytes)
		{
			rc = IFX_E_INVALID_POINTER;
		}

		// Note the check for the file pointer (m_pFile) is done in GetTotalSizeX

		WriteX(pBytes,position,count);
	}

	catch(IFXException e)
	{
		rc = e.GetIFXResult();
	}

	catch(...)
	{
		throw;
	}

	IFXRETURN(rc);
}

// IFXWriteBufferX
void CIFXStdioWriteBufferX::WriteX( U8* pBytes, U64 position, U32 count )
{
	IFXRESULT rc = IFX_OK; // used for stdio results

	try
	{
		// Check the input pointer and file pointer
		if(NULL == pBytes)
		{
			throw IFXException(IFX_E_INVALID_POINTER);
		}

		else if (NULL == m_pFile)
		{
			throw IFXException(IFX_E_NOT_INITIALIZED);
		}

		// Seek to position
#ifdef STDIO_HACK
		if (m_pFile==stdout && (position==20 || position==24)) return;
#endif
		rc = fseek(m_pFile, (U32)position, SEEK_SET);

#ifdef STDIO_HACK
		if(0 != rc && m_pFile!=stdout)
#else
		if(0 != rc)
#endif
		{
			throw IFXException(IFX_E_INVALID_FILE);
		}


		// Write the data
		rc = fwrite(pBytes,1,count,m_pFile);

		if(count != (U32)rc)
		{
			// NOTE: insufficient disk space is not guaranteed to be the
			//       cause of fwrite returning a value not equal to count.
			rc = IFX_E_WRITE_FAILED;
		}

		else
		{
			// This isn't really needed (it's not even returned to the caller)
			// but it's perserved for possible future modifications that will
			// require the result code.
			rc = IFX_OK;
		}
	}

	catch(...)
	{
		throw;
	}
}

// IFXStdio
IFXRESULT CIFXStdioWriteBufferX::Open(const IFXCHAR *pFilename)
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
		m_pFile = IFXOSFileOpen( pFilename, L"wb" );
		if(NULL == m_pFile) 
			rc = IFX_E_INVALID_FILE;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXStdioWriteBufferX::Close()
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
U32 CIFXStdioWriteBufferX::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXStdioWriteBufferX::Release( void )
{
	if(!(--m_uRefCount)) {
		delete this;
		return 0;
	}

	return m_uRefCount;
}

IFXRESULT CIFXStdioWriteBufferX::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if(ppInterface)
	{
		if (interfaceId == IID_IFXWriteBuffer)
		{
			*ppInterface = (IFXWriteBuffer*) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else if (interfaceId == IID_IFXWriteBufferX)
		{
			*ppInterface = (IFXWriteBufferX*) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXStdio )
		{
			*ppInterface = (IFXStdio*) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	}

	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXStdioWriteBufferX_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if(ppInterface) {
		// Create the CIFXStdioWriteBufferX component.
		CIFXStdioWriteBufferX *pComponent = new CIFXStdioWriteBufferX;

		if(pComponent) {
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

CIFXStdioWriteBufferX::CIFXStdioWriteBufferX()
{
	m_uRefCount = 0;
	m_pFile = NULL;
}

CIFXStdioWriteBufferX::~CIFXStdioWriteBufferX()
{
	if(NULL != m_pFile) {
		fclose(m_pFile);
		m_pFile = NULL;
	}
}
