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
	@file	CIFXBlockReaderX.cpp

			Implementation for CIFXBlockReaderX class. 
			The CIFXBlockReaderX implements the IFXBlockReaderX interface 
			which is used to read blocks of data from a ReadBuffer
*/

#include <memory.h>
#include "CIFXBlockReaderX.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXBitStreamX.h"

IFXRESULT IFXAPI_CALLTYPE CIFXBlockReaderX_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_E_UNDEFINED;

	if ( ppInterface ) {
		// Create the CIFXBlockReader component.
		CIFXBlockReaderX *pComponent	= new CIFXBlockReaderX;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

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

	return rc;
}

/**
	Initializes the BlockReader by giving it a reference to the ReadBuffer and 
	sets the buffer read position to zero
*/
void CIFXBlockReaderX::InitializeX(	IFXCoreServices& rCoreServices, 
								    IFXReadBufferX& rReadBufferX, 
									U32 uReadPosition)
{
	rReadBufferX.AddRef();
	IFXRELEASE(m_pReadBufferX);
	m_pReadBufferX = &rReadBufferX;

	rCoreServices.AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = &rCoreServices;

	m_uReadPosition = 0;
	m_uFileType = 0;
	m_uFileSize = 0;
	m_uDeclarationSize = 0;
	m_uCharEncoding = 0;

	// Very basic IFX file header check
	IFXRESULT iWarningCode = IFX_OK;
	if (0 == uReadPosition) {
		CheckFileHeaderX(iWarningCode);
	} else {
		U64 uAvailSize = 0;
		rReadBufferX.GetAvailableSizeX(uAvailSize);
		m_uReadPosition = uReadPosition;
		m_uFileType = BlockType_FileHeaderU3D;
		m_uFileVersion = FileHeader_VersionCurrent;
		m_uFileSize = uAvailSize;
		m_uDeclarationSize = (U32)uAvailSize;
		m_bHasValidFileHeader = TRUE;
	}
}

/**
	Returns total file size which was specified in header block
*/
void CIFXBlockReaderX::GetFileSizeX(U64& rFileSize)
{
	if (NULL == m_pReadBufferX)
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	rFileSize = m_uFileSize;
}

/**
	Returns current read position (which is same as current loaded bytes)
*/
void CIFXBlockReaderX::GetFileSizeLoadedX(U64& rFileSizeLoaded)
{
	if (NULL == m_pReadBufferX)
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	rFileSizeLoaded = m_uReadPosition;
}

/**
	Returns file type and version
*/
void CIFXBlockReaderX::GetFileVersionX(U32& rFileType, U32& rFileVersion)
{
	if (NULL == m_pReadBufferX)
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	rFileType = m_uFileType;
	rFileVersion = m_uFileVersion;
}

/** 
	Reads the first 4 bytes from the ReadBuffer and checks for a 
	valid file header.  Also checks for existence of read buffer.
*/
void CIFXBlockReaderX::CheckFileHeaderX(IFXRESULT& rWarningCode)
{
	U32 uFileType = 0, uDataSize = 0, uMetaDataSize = 0, uHeaderSize = 8;
	rWarningCode = IFX_OK;

	// If the file header has not been validated yet
	if (FALSE == m_bHasValidFileHeader) {
		// and if initialized
		if (NULL != m_pReadBufferX) {
			// then read the first 4 bytes from the read buffer
			m_pReadBufferX->ReadX((U8*)&uFileType, 0, 4, rWarningCode);
			SWAP32_IF_BIGENDIAN(uFileType);
			m_pReadBufferX->ReadX((U8*)&uDataSize, 4, 4, rWarningCode);
			SWAP32_IF_BIGENDIAN(uDataSize);
			// if new file type, then metadata exists
			if (BlockType_FileHeaderU3D == uFileType) {
				m_pReadBufferX->ReadX((U8*)&uMetaDataSize, 8, 4, rWarningCode);
				SWAP32_IF_BIGENDIAN(uMetaDataSize);
				uHeaderSize = 12;
			} else
				IFXCHECKX(IFX_E_INVALID_FILE);
			// align sizes to 4-byte boundary
			uDataSize = (((uDataSize - 1) >> 2) + 1) << 2;
			uMetaDataSize = (((uMetaDataSize - 1) >> 2) + 1) << 2;

			// allocate buffer to hold 1st block (file header)
			IFXASSERT(m_FirstBlock == NULL);
			m_FirstBlock = new U8[uHeaderSize + uDataSize + uMetaDataSize];

			// store size values in this buffer
			memcpy(m_FirstBlock, &uFileType, 4);
			memcpy(m_FirstBlock+4, &uDataSize, 4);
			if (BlockType_FileHeaderU3D == uFileType)
				memcpy(m_FirstBlock+8, &uMetaDataSize, 4);
		} else {
			IFXCHECKX(IFX_E_NOT_INITIALIZED);
		}
		m_uFileType = uFileType;
		// if the read was successful
		if (IFX_OK == rWarningCode) {
			// read rest of block to buffer
			m_pReadBufferX->ReadX(m_FirstBlock + uHeaderSize, uHeaderSize, uDataSize + uMetaDataSize, rWarningCode);
			// check for a valid file type
			if (BlockType_FileHeaderU3D == uFileType) {
				m_bHasValidFileHeader = TRUE;
				memcpy(&m_uFileVersion, m_FirstBlock + uHeaderSize + 0, 4);
				memcpy(&m_uDeclarationSize, m_FirstBlock + uHeaderSize + 8, 4);
				memcpy(&m_uFileSize, m_FirstBlock + uHeaderSize + 12, 8);
				memcpy(&m_uCharEncoding, m_FirstBlock + uHeaderSize + 20, 4);
			} else {
				IFXCHECKX(IFX_E_INVALID_FILE);
			}
		} 
		// Note that data not available and end of file are not failures
	}

	return;
}

/**
	ReadBlock will attempt to read a block of data from the ReadBuffer. If the 
	read is successful, the BlockReader will create an IFXDataBlockX, put the 
	data into the DataBlock and advance the current buffer read position.
*/
void CIFXBlockReaderX::ReadBlockX(IFXDataBlockX*&  rpDataBlockX, IFXRESULT& rWarningCode) 
{
	const int BUFFERSZZ	= 3;		// header is a buffer of 2 or 3 U32's
	int BUFFERSZ		= 2;		// actual number of buffers for current file
	const int TYPEINDEX = 0;		// Type info is buffer[0]
	const int SIZEINDEX = 1;		// Size info is buffer[1]
	const int METAINDEX	= 2;		// Metadata size info is buffer[2]

	IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
	U32 pInfoBuffer[BUFFERSZZ];
    U64 uTmpReadPosition = m_uReadPosition;
	U8* pData = NULL;

	// Do some sanity checking
	CheckFileHeaderX(rWarningCode);
	if (BlockType_FileHeaderU3D == m_uFileType)
		BUFFERSZ++;

	// Note:  IFXSUCCESS(IFX_W_DATA_NOT_AVAILABLE) == TRUE
	// Note:  IFXSUCCESS(IFX_W_END_OF_FILE) == TRUE

	// Need to create a datablock 	
	IFXCHECKX(IFXCreateComponent(CID_IFXDataBlockX, IID_IFXDataBlockX, (void**)&pDataBlockX));

	if (IFX_OK == rWarningCode) {
		// Check for end of file
		if(m_uReadPosition >= m_uFileSize) {
			rWarningCode = IFX_W_END_OF_FILE;
		}
	}

	if (IFX_OK == rWarningCode) {
		// 2. Second, read the DataBlock header information... type and size
		if (0 != m_uReadPosition)
		{
			m_pReadBufferX->ReadX( (U8*)pInfoBuffer, m_uReadPosition, sizeof(U32) * BUFFERSZ , rWarningCode);
			SWAP32_IF_BIGENDIAN(pInfoBuffer[TYPEINDEX]);
			SWAP32_IF_BIGENDIAN(pInfoBuffer[SIZEINDEX]);
			SWAP32_IF_BIGENDIAN(pInfoBuffer[METAINDEX]);
		}
		else
			memcpy(pInfoBuffer, m_FirstBlock, sizeof(U32) * BUFFERSZ);
        uTmpReadPosition += sizeof(U32) * BUFFERSZ;
	}

	if (IFX_OK == rWarningCode) {
		// 3. Third, check to see if the file will be big enough
		// to create a DataBlock of the size parameter specified above
		if ( pInfoBuffer[SIZEINDEX] + uTmpReadPosition > m_uFileSize ) {
			IFXASSERT(0); // Does this branch ever get taken?
			rWarningCode = IFX_W_END_OF_FILE;
		}
	}

	if ((IFX_OK == rWarningCode) && (pInfoBuffer[SIZEINDEX] != 0)) {
		// Set up the type and size just read into our local DataBlock
		pDataBlockX->SetBlockTypeX( pInfoBuffer[TYPEINDEX] );
		pDataBlockX->SetSizeX( pInfoBuffer[SIZEINDEX] );
		// Update our temp current position ptr to reflect header read

		// 4. Get the memory location of the DataBlock's 'data' section
		pDataBlockX->GetPointerX( pData );

		// 5. Read the bytes from the ReadBuffer into the 'data' section  
		// of the DataBlock. This is effectively a write to the DataBlock
		if (0 != m_uReadPosition)
			m_pReadBufferX->ReadX(pData, uTmpReadPosition, pInfoBuffer[SIZEINDEX], rWarningCode);
		else
			memcpy(pData, m_FirstBlock + uTmpReadPosition, pInfoBuffer[SIZEINDEX]);
		uTmpReadPosition += pInfoBuffer[SIZEINDEX];

		U32 pad = ((U32)uTmpReadPosition & 3), tmp;
		if ((0 != m_uReadPosition) && pad) {
			m_pReadBufferX->ReadX((U8*)&tmp, uTmpReadPosition, 4-pad, rWarningCode);
			uTmpReadPosition += 4-pad;
		}
	}

	if ((IFX_OK == rWarningCode) && (BlockType_FileHeaderU3D == m_uFileType) && (pInfoBuffer[METAINDEX] != 0)) {
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMetaData);
		IFXDECLARELOCAL(IFXDataBlockX, pMetaDataBlockX);
		IFXDECLARELOCAL(IFXBitStreamX, pMetaDataBS);
		U8* pMetaData = NULL;

		// create needed components
		IFXCHECKX(pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMetaData));
		IFXCHECKX(IFXCreateComponent(CID_IFXDataBlockX, IID_IFXDataBlockX, (void**)&pMetaDataBlockX));
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pMetaDataBS));

		// initialize metadata block
		pMetaDataBlockX->SetSizeX(pInfoBuffer[METAINDEX]);
		pMetaDataBlockX->GetPointerX(pMetaData);

		// read metadata block
		if (0 != m_uReadPosition)
			m_pReadBufferX->ReadX(pMetaData, uTmpReadPosition, pInfoBuffer[METAINDEX], rWarningCode);
		else
			memcpy(pMetaData, m_FirstBlock + uTmpReadPosition, pInfoBuffer[METAINDEX]);
		uTmpReadPosition += pInfoBuffer[METAINDEX];

		U32 pad = ((U32)uTmpReadPosition & 3), tmp;
		if ((0 != m_uReadPosition) && pad) {
			m_pReadBufferX->ReadX((U8*)&tmp, uTmpReadPosition, 4-pad, rWarningCode);
			uTmpReadPosition += 4-pad;
		}

		// initialize bitstream with metadata block
		pMetaDataBS->SetDataBlockX(*pMetaDataBlockX);

		// get number of metadata entries
		U32 uCountMD = 0;
		pMetaDataBS->ReadU32X(uCountMD);

		// enumerate all metadata entries
		U32 i;
		for ( i = 0; i < uCountMD; i++) {
			IFXMetaDataAttribute eAttributes;
			IFXString sKey;
			IFXString sValue;
			U32 uValueSize = 0;
			U8* bValue = NULL;

			// read type & key
			pMetaDataBS->ReadU32X((U32&)eAttributes);
			pMetaDataBS->ReadIFXStringX(sKey);

			// read value of appropriate type
			if (eAttributes & IFXMETADATAATTRIBUTE_BINARY) {
				pMetaDataBS->ReadU32X(uValueSize);
				bValue = new U8[uValueSize];
				U32 i;
				for ( i = 0; i < uValueSize; i++)
					pMetaDataBS->ReadU8X(bValue[i]);
				pBlockMetaData->SetBinaryValueX(sKey, uValueSize, bValue);
				delete bValue;
			} else {
				pMetaDataBS->ReadIFXStringX(sValue);
				pBlockMetaData->SetStringValueX(sKey, sValue);
			}
			U32 uIndex = 0;
			pBlockMetaData->GetIndex(sKey, uIndex);
			pBlockMetaData->SetAttributeX(uIndex, eAttributes);
		}
		if (0 == m_uReadPosition) {
			IFXDECLARELOCAL(IFXMetaDataX, pCoreServicesMD);
			IFXCHECKX(m_pCoreServices->QueryInterface(IID_IFXMetaDataX, (void**)&pCoreServicesMD));
			pCoreServicesMD->AppendX(pBlockMetaData);
			IFXRELEASE(m_pCoreServices);
		}
	}

	if (IFX_OK == rWarningCode) {
		// If we get here we have successfully 
		// read the data block so we can addref
		// the DataBlock and do other clean up
		pDataBlockX->AddRef();
		IFXRELEASE(rpDataBlockX);
		rpDataBlockX = pDataBlockX;

        // update the current position pointer to account for block header
		m_uReadPosition = uTmpReadPosition;
	}

	return;
}

// IFXUnknown

U32 CIFXBlockReaderX::AddRef() 
{
	return ++m_uRefCount;
}

U32 CIFXBlockReaderX::Release() 
{
	if ( 1 == m_uRefCount  ) {
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXBlockReaderX::QueryInterface( IFXREFIID	interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXBlockReaderX )
		{
			*ppInterface = ( IFXBlockReaderX* ) this;
			AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		} 
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}


//---------------------------------------------------------------------------
//	CIFXBlockReaderX::CIFXBlockReaderX
//
//  Constructor
//---------------------------------------------------------------------------
CIFXBlockReaderX::CIFXBlockReaderX() :
	IFXDEFINEMEMBER(m_pReadBufferX),
	IFXDEFINEMEMBER(m_pCoreServices)
{
	m_uRefCount		= 0;		// Reference counter
	m_uReadPosition = 0;		
	m_bHasValidFileHeader = FALSE;
	m_FirstBlock = NULL;
}

//---------------------------------------------------------------------------
//	CIFXBlockReaderX::~CIFXBlockReaderX
//
//  Destructor
//---------------------------------------------------------------------------
CIFXBlockReaderX::~CIFXBlockReaderX() 
{
	if (m_FirstBlock != NULL) {
		delete m_FirstBlock;
		m_FirstBlock = NULL;
	}
}
