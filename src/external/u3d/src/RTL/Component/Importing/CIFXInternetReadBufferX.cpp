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
	@file	CIFXInternetReadBufferX.cpp

			Implementation of the CIFXInternetReadBufferX class which implements the
			IFXReadBuffer, IFXReadBufferX and IFXStdio interfaces.
*/

#include "CIFXInternetReadBufferX.h"
#include "IFXImportingCIDs.h"
#include "IFXCheckX.h"
#include "IFXMemory.h"
#include "IFXScheduler.h"

#include "IFXOSSocket.h"

const U32 BUFFER_BLOCK_SIZE = 65536;
const U32 INTERNET_READ_TASK_PRIORITY = 4;

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXInternetReadBufferX_Factory(
								IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if (ppInterface) 
	{
		// Create the CIFXInternetReadBufferX component.
		CIFXInternetReadBufferX *pComponent = new CIFXInternetReadBufferX;

		if (pComponent) 
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface(interfaceId, ppInterface);

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} 
		else 
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

CIFXInternetReadBufferX::CIFXInternetReadBufferX() :
	IFXDEFINEMEMBER(m_pReadingCallback),
	IFXDEFINEMEMBER(m_pReadBuffer)
{
	m_refCount = 0;
	m_readPosition = 0;
	m_readTaskHandle = IFXTASK_HANDLE_INVALID;
	m_pReadBuffer = NULL;
}

CIFXInternetReadBufferX::~CIFXInternetReadBufferX()
{
	Close();
}

// IFXUnknown
U32 CIFXInternetReadBufferX::AddRef( void )
{
	return ++m_refCount;
}

U32 CIFXInternetReadBufferX::Release( void )
{
	if (1 == m_refCount) {
		delete this;
		return 0;
	}

	return --m_refCount;
}

IFXRESULT CIFXInternetReadBufferX::QueryInterface( 
									IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if (ppInterface) 
	{
		if (interfaceId == IID_IFXReadBufferX) 
		{
			*ppInterface = ( IFXReadBufferX* ) this;
		} 
		else if ( interfaceId == IID_IFXStdio ) 
		{
			*ppInterface = ( IFXStdio* )this;
		} 
		else if ( interfaceId == IID_IFXInet ) 
		{
			*ppInterface = ( IFXInet* )this;
		} 
		else if ( interfaceId == IID_IFXReadBuffer ) 
		{
			*ppInterface = ( IFXReadBuffer* )this;
		} 
		else if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* )this;
		} 
		else if ( interfaceId == IID_IFXTask ) 
		{
			*ppInterface = ( IFXTask* )this;
		} 
		else 
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS( rc ) )
			AddRef();
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

// IFXReadBuffer
IFXRESULT CIFXInternetReadBufferX::Read( U8* pBytes, U64 position, U32 count )
{
	IFXRESULT rc = IFX_OK;

	try
	{
		// Check the input pointer and file pointer
		if(NULL == pBytes) 
		{
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}

		// Note the check for the file pointer (m_hFile) is done in GetTotalSizeX
		ReadX(pBytes,position,count,rc);

		if(IFX_W_END_OF_FILE == rc) 
		{
			rc = IFX_E_END_OF_FILE; // Convert end of file warning to an error
		}
	}
	catch(IFXException& rException)
	{
		rc = rException.GetIFXResult();
	}

	return rc;
}

void CIFXInternetReadBufferX::ReadX( 
									U8* pBytes, U64 position, U32 count, 
									IFXRESULT& rWarningCode )
{
	rWarningCode = IFX_OK;
	U32 bytesRead = 0;

	// Check the input pointer and file pointer
	if (NULL == pBytes) 
	{
		IFXCHECKX(IFX_E_INVALID_POINTER);
	} 

	if (m_readPosition != position)
		IFXCHECKX(IFX_E_IO);

	// Read the data
	IFXRESULT result = m_session.Read( pBytes, count, &bytesRead );

	if (IFX_OK == result)
	{
		m_readPosition += bytesRead;
	}
	else 
	{
		// Check for end of file
		if (IFX_W_FINISHED == result)
		{
			m_readPosition += bytesRead;
			rWarningCode = IFX_W_END_OF_FILE;
		}
		else
			IFXCHECKX( result );
	}
}

IFXRESULT CIFXInternetReadBufferX::GetTotalSize( U64* pCount )
{
	IFXRESULT rc = IFX_OK;

	try 
	{
		// Check the input pointer
		if (NULL == pCount)
		{
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}
		// Note the check for the file pointer (m_hFile) is done in GetTotalSizeX

		GetTotalSizeX(*pCount);
	}
	catch(IFXException& rException) 
	{
		rc = rException.GetIFXResult();
	}

	return rc;
}

void CIFXInternetReadBufferX::GetTotalSizeX( U64& rCount )
{
	GetAvailableSizeX(rCount);
}

IFXRESULT CIFXInternetReadBufferX::GetAvailableSize( U64* pCount )
{
	IFXRESULT rc = IFX_OK;

	try	
	{
		// Check the input pointer and file pointer
		if(NULL == pCount) 
		{
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}
		// Note the check for the file pointer (m_hFile) is done in GetAvailableSizeX

		GetAvailableSizeX(*pCount);
	}
	catch (IFXException& rException) 
	{
		rc = rException.GetIFXResult();
	}

	return rc;
}

void CIFXInternetReadBufferX::GetAvailableSizeX( U64& rCount )
{
	// For InternetReadBufferX, available size and total size are the same.
	/// @todo	Get file size for CIFXInternetReadBufferX is not implemented
	//IFXInternetQueryDataAvailable(m_hFile, &rCount);
}

// IFXStdio
IFXRESULT CIFXInternetReadBufferX::Open(const IFXCHAR *pUrl)
{
	IFXRESULT rc = IFX_OK;

	try	
	{
		m_session.OpenX( pUrl );
		m_readPosition = 0;
	}
	catch (IFXException& rException) 
	{
		rc = rException.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::Close()
{
	IFXRESULT result = IFX_OK;

	result = m_session.Close();

	return result;
}

// IFXInet

/// @todo complete CIFXInternetReadBufferX::GetConnectTimeout
IFXRESULT CIFXInternetReadBufferX::GetConnectTimeout(U32& rConnectTimeout)
{
	IFXRESULT rc = IFX_E_UNSUPPORTED;

	return rc;
}

/// @todo complete CIFXInternetReadBufferX::SetConnectTimeout
IFXRESULT CIFXInternetReadBufferX::SetConnectTimeout(const U32 connectTimeout)
{
	IFXRESULT rc = IFX_E_UNSUPPORTED;

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::GetSendTimeout(U32& rSendTimeout)
{
	IFXRESULT rc = IFX_OK;

	U32 buffer;
	I32 buflen = sizeof(buffer);

	rc = m_session.GetOptions( IFXOSSOCKET_SEND_TIMEOUT, (I8*)&buffer, &buflen );

	if( IFXSUCCESS( rc ) )
		rSendTimeout = buffer;

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::SetSendTimeout(const U32 sendTimeout)
{
	IFXRESULT rc = IFX_OK;

	const I8* buffer = (const I8*)&sendTimeout;
	U32 buflen = sizeof(sendTimeout);

	rc = m_session.SetOptions( IFXOSSOCKET_SEND_TIMEOUT, buffer, buflen );

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::GetReceiveTimeout(U32& rReceiveTimeout)
{
	IFXRESULT rc = IFX_OK;

	U32 buffer;
	I32 buflen = sizeof(buffer);

	rc = m_session.GetOptions( IFXOSSOCKET_RECEIVE_TIMEOUT, (I8*)&buffer, &buflen );

	if( IFXSUCCESS( rc ) )
		rReceiveTimeout = buffer;

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::SetReceiveTimeout(const U32 receiveTimeout)
{
	IFXRESULT rc = IFX_OK;

	const I8* pBuffer = (const I8*)&receiveTimeout;
	U32 buflen = sizeof(receiveTimeout);

	rc = m_session.SetOptions( IFXOSSOCKET_RECEIVE_TIMEOUT, pBuffer, buflen );

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::InitiateRead(
										IFXCoreServices* pCoreServices, 
										IFXReadingCallback* pReadingCallback)
{
	IFXRESULT rc = IFX_OK;

	if (IFXSUCCESS(rc) && (NULL == pReadingCallback)) rc = IFX_E_INVALID_POINTER;

	m_pCoreServices = pCoreServices;
	IFXRELEASE(m_pReadingCallback);
	m_pReadingCallback = pReadingCallback;
	m_pReadingCallback->AddRef();

	m_pReadingCallback->GetURLCount(m_URLs);
	m_currentURL = 0;

	IFXDECLARELOCAL(IFXScheduler, pScheduler);

	if (IFXSUCCESS(rc))
		rc = m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);

	IFXDECLARELOCAL(IFXSystemManager, pSysMgr);

	if (IFXSUCCESS(rc))
		rc = pScheduler->GetSystemManager(&pSysMgr);

	IFXDECLARELOCAL(IFXTask, pReadTask);

	if (IFXSUCCESS(rc))
		rc = this->QueryInterface(IID_IFXTask, (void**) &pReadTask);

	if (IFXSUCCESS(rc))
	{
		rc = pSysMgr->RegisterTask(
						pReadTask, INTERNET_READ_TASK_PRIORITY, 
						NULL, &m_readTaskHandle);
	}

	return rc;
}

IFXRESULT CIFXInternetReadBufferX::Execute(IFXTaskData* pTaskData)
{
	IFXRESULT rc = IFX_OK;

	if (m_currentURL < m_URLs) 
	{
		U8*	pFile = NULL;
		U64		totalSize = 0;

		if (m_pReadBuffer == NULL)
		{
			IFXString sURL;
			rc = m_pReadingCallback->GetURL(m_currentURL, sURL);

			if(IFXSUCCESS( rc )) 
			{
				IFXDECLARELOCAL(IFXStdio, pStdio);
				U32 tmp = 0;
				totalSize = 0;

				if (IFXSUCCESS(sURL.FindSubstring(L"://", &tmp)))
					rc = this->QueryInterface(IID_IFXReadBuffer, (void**)&m_pReadBuffer);
				else 
				{
					rc = IFXCreateComponent(
									CID_IFXStdioReadBuffer, 
									IID_IFXReadBuffer, 
									(void**)&m_pReadBuffer);
					totalSize = 1;
				}

				if (IFXSUCCESS(rc))
					rc = m_pReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pStdio);

				if (IFXSUCCESS(rc))
					rc = pStdio->Open( (IFXCHAR*)sURL.Raw() );

				if (IFXFAILURE(rc)) 
				{
					// try another URL
					IFXRELEASE(m_pReadBuffer);
					m_currentURL++;
				}
			}
		}

		if (IFXSUCCESS(rc)) 
		{
			if (0 != totalSize) // local file
			{
				m_pReadBuffer->GetTotalSize(&totalSize);
				pFile = (U8*)IFXAllocate( (size_t)totalSize );

				if (NULL != pFile )
				{
					rc = m_pReadBuffer->Read( pFile, 0, (U32)totalSize );
				}
				else
				{
					rc = IFX_E_OUT_OF_MEMORY;
				}

				if( IFXSUCCESS( rc) )
				{
					rc = IFX_W_END_OF_FILE;
				}
			}
			else
			{
				U64 readSize = 0;
				U64 currentSize = 0;

				while (IFX_OK == rc) 
				{
					currentSize += BUFFER_BLOCK_SIZE;
					readSize = m_readPosition;

					pFile = (U8*)IFXReallocate( pFile, (size_t)currentSize );

					if (pFile != NULL)
					{
						rc = m_pReadBuffer->Read( 
												pFile+readSize, 
												readSize, 
												BUFFER_BLOCK_SIZE );
					}
					else
					{
						rc = IFX_E_OUT_OF_MEMORY;
						break;
					}
				}

				if (rc == IFX_E_END_OF_FILE)
				{
					totalSize = m_readPosition;
					pFile = (U8*)IFXReallocate(pFile, (size_t)totalSize);
					rc = IFX_W_END_OF_FILE;
				}
			}

			if (rc == IFX_W_END_OF_FILE) 
			{
				IFXDECLARELOCAL(IFXStdio, pStdio);
				rc = m_pReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pStdio);

				if (IFXSUCCESS(rc))
					pStdio->Close();

				IFXRELEASE(m_pReadBuffer);

				/**
					@todo	Dangerous pass of pFile buffer allocated here. It will be 
							deallocated in CIFXImageTools.
				*/
				m_pReadingCallback->AcceptSuccess(m_currentURL, pFile, (U32)totalSize);
				pFile = NULL; // deleting is left to the m_pReadingCallback
				totalSize = 0;

				IFXDECLARELOCAL(IFXScheduler, pScheduler);
				m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);

				IFXDECLARELOCAL(IFXSystemManager, pSysMgr);
				pScheduler->GetSystemManager(&pSysMgr);

				pSysMgr->UnregisterTask(m_readTaskHandle);
				m_readTaskHandle = IFXTASK_HANDLE_INVALID;
			}
		}
	} 
	else 
	{
		m_pReadingCallback->AcceptFailure(IFX_E_CANNOT_FIND);
		IFXDECLARELOCAL(IFXScheduler, pScheduler);
		m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler);
		IFXDECLARELOCAL(IFXSystemManager, pSysMgr);
		pScheduler->GetSystemManager(&pSysMgr);
		pSysMgr->UnregisterTask(m_readTaskHandle);
		m_readTaskHandle = IFXTASK_HANDLE_INVALID;
	}

	//	If one of the URL in the URL list is not valid this is okay, try another one
	if (rc == IFX_E_INVALID_FILE)
		rc = IFX_OK;

	return rc;
}
