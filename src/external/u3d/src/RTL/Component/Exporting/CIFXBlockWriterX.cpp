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
	@file	CIFXBlockWriterX.cpp

			Implementation for CIFXBlockWriterX class. 
			The CIFXBlockWriterX implements the IFXBlockWriterX and IFXBlockWriter 
			interfaces which are used to write blocks of data to the 
			WriteBufferX and/or WriteBuffer, respectively.
*/

#include <memory.h>
#include "CIFXBlockWriterX.h"
#include "IFXBitStreamX.h"
#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"

/**	
	This is the CIFXBlockWriter component factory function.  The
	CIFXBlockWriter component can be instaniated multiple times.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXBlockWriterX_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_E_UNDEFINED;

	if ( ppInterface ) {
		// Create the CIFXBlockWriter component.
		CIFXBlockWriterX *pComponent = new CIFXBlockWriterX;

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
	Initializes the BlockWriter by giving it a reference to the WriteBuffer and 
	sets the buffer write position to zero
*/
void CIFXBlockWriterX::InitializeX(		IFXCoreServices& rCoreServices, 
										IFXWriteBufferX& rWriteBufferX, 
										U32 uWritePosition)
{
	// Check input pointer
	if (NULL != m_pWriteBufferX) 
		IFXCHECKX(IFX_E_ALREADY_INITIALIZED);

	// Store the pWriteBuffer
	rWriteBufferX.AddRef();
	m_pWriteBufferX = &rWriteBufferX;
	m_uWritePosition = uWritePosition;
	m_uMaxPriority = 0;
	m_bWithoutFileHeader = (uWritePosition != 0);

	if (!m_bWithoutFileHeader) {
		// Write the file header
		IFXDECLARELOCAL(IFXBitStreamX, pHeaderBS);
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pHeaderBS));

		IFXDECLARELOCAL(IFXDataBlockX, pHeaderDataBlock);
		pHeaderBS->WriteU32X(FileHeader_VersionCurrent);
		U32 uProfile = 0;
		IFXCHECKX(rCoreServices.GetProfile(uProfile));
		pHeaderBS->WriteU32X(uProfile); // profile
		pHeaderBS->WriteU32X(0); // declaration size
		pHeaderBS->WriteU64X(0); // file size
		pHeaderBS->WriteU32X(MIB_UTF8);
		if( uProfile & IFXPROFILE_UNITSSCALE )
		{
			F64 units;
			rCoreServices.GetUnits(units);
			pHeaderBS->WriteF64X(units);
		}
		pHeaderBS->GetDataBlockX(pHeaderDataBlock);
		pHeaderDataBlock->SetBlockTypeX(BlockType_FileHeaderU3D);

		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pCoreServicesMD);
		IFXCHECKX(pHeaderDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD));
		IFXCHECKX(rCoreServices.QueryInterface(IID_IFXMetaDataX, (void**)&pCoreServicesMD));
		pBlockMD->AppendX(pCoreServicesMD);

		WriteBlockX(*pHeaderDataBlock);
	}
}

/**
	WriteBlock will attempt to write a block of data to the WriteBuffer. 
	If the write is successful, the BlockWriter will create an IFXDataBlock, 
	put the data into the DataBlock and advance the current buffer write position.
*/
void CIFXBlockWriterX::WriteBlockX(IFXDataBlockX& rDataBlockX)
{
	const U32 BUFFERSZ  = 3;		// header is a buffer of 2 or 3 U32's
	const U32 TYPEINDEX = 0;		// Type info is buffer[0]
	const U32 SIZEINDEX = 1;		// Size info is buffer[1]
	const U32 METAINDEX	= 2;		// Metadata size info is buffer[2]

	U32 pInfoBuffer[BUFFERSZ] = {0, 0, 0};
	U8* pData = NULL;
	U8 pPadBuffer[4] = {0, 0, 0, 0};
	U32 uPadCount;

	U32				uHeaderSize = 0;
	IFXDECLARELOCAL(IFXBitStreamX, pHeaderBS);
	U32				uMetaDataSize = 0;
	IFXDECLARELOCAL(IFXBitStreamX, pMetaDataBS);
	U8*				pHeaderData = NULL;
	IFXDECLARELOCAL(IFXDataBlockX, pHeaderDataBlock);
	U8*				pMetaData = NULL;
	IFXDECLARELOCAL(IFXDataBlockX, pMetaDataBlock);

	// Check for initialization
	if (NULL == m_pWriteBufferX) 
		IFXCHECKX(IFX_E_NOT_INITIALIZED);

	// Get information from data block
	rDataBlockX.GetBlockTypeX(*(pInfoBuffer+TYPEINDEX));
	rDataBlockX.GetSizeX(*(pInfoBuffer+SIZEINDEX));
	rDataBlockX.GetPointerX(pData);

	// Get information from meta data
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXCHECKX(rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD));
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pMetaDataBS));
	U32 uIndex = 0, uCountMD = 0, uWritableCountMD = 0;
	pBlockMD->GetCountX(uCountMD);

	if (uCountMD != 0) {
		pMetaDataBS->WriteU32X(uCountMD);

		// iterate thru all metadata entries
		for (uIndex = 0; uIndex < uCountMD; uIndex++) {
			BOOL bPersistence;
			// we need only persistent ones
			pBlockMD->GetPersistenceX(uIndex, bPersistence);
			if (FALSE == bPersistence) continue;

			IFXMetaDataAttribute eAttributes;
			IFXString sKey;
			U32 bValueSize = 0;
			U8* bValue = NULL;

			// get type of entry
			pBlockMD->GetAttributeX(uIndex, eAttributes);
			pMetaDataBS->WriteU32X(eAttributes);

			// get key of entry
			pBlockMD->GetEncodedKeyX(uIndex, sKey);
			pMetaDataBS->WriteIFXStringX(sKey);

			if (eAttributes & IFXMETADATAATTRIBUTE_BINARY) {
				// get binary value
				pBlockMD->GetBinarySizeX(uIndex, bValueSize);
				bValue = new U8[bValueSize];
				pBlockMD->GetBinaryX(uIndex, bValue);
				pMetaDataBS->WriteU32X(bValueSize);
				U32 i;
				for ( i = 0; i < bValueSize; i++)
					pMetaDataBS->WriteU8X(bValue[i]);
				delete [] bValue;
			} else {
				IFXString sValue;
				// get string value
				pBlockMD->GetStringX(uIndex, sValue);
				pMetaDataBS->WriteIFXStringX(sValue);
			}

			// increase number of persistent entries
			uWritableCountMD++;
		}

		pMetaDataBS->GetDataBlockX(pMetaDataBlock);
		pMetaDataBlock->GetPointerX(pMetaData);
		pMetaDataBlock->GetSizeX(uMetaDataSize);
		// update actual number of metadata entries
		SWAP64_IF_BIGENDIAN(uWritableCountMD);
		memcpy(pMetaData, &uWritableCountMD, sizeof(U32));
		pInfoBuffer[METAINDEX] = uMetaDataSize;
	}

	// Format the block header using a bitstream object
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pHeaderBS));

	pHeaderBS->WriteU32X(pInfoBuffer[TYPEINDEX]);
	pHeaderBS->WriteU32X(pInfoBuffer[SIZEINDEX]);
	pHeaderBS->WriteU32X(pInfoBuffer[METAINDEX]);
	pHeaderBS->GetDataBlockX(pHeaderDataBlock);
	pHeaderDataBlock->GetPointerX(pHeaderData);
	pHeaderDataBlock->GetSizeX(uHeaderSize);

	// Write the block header
	m_pWriteBufferX->WriteX(pHeaderData, m_uWritePosition, uHeaderSize);
	m_uWritePosition += uHeaderSize;

	// Write the data
	if (pInfoBuffer[SIZEINDEX] != 0) {
		m_pWriteBufferX->WriteX(pData, m_uWritePosition, pInfoBuffer[SIZEINDEX]);
		m_uWritePosition += pInfoBuffer[SIZEINDEX];
	}

	// Write the padding to 4 byte boundary
	uPadCount = ((~(pInfoBuffer[SIZEINDEX])) + 1) & 3;
	if (uPadCount > 0) 
	{
		m_pWriteBufferX->WriteX(pPadBuffer, m_uWritePosition, uPadCount);
		m_uWritePosition += uPadCount;
	}

	// Write the metadata
	if (pInfoBuffer[METAINDEX] != 0) {
		m_pWriteBufferX->WriteX(pMetaData, m_uWritePosition, pInfoBuffer[METAINDEX]);
		m_uWritePosition += pInfoBuffer[METAINDEX];
	}

	// Write the padding to 4 byte boundary
	uPadCount = ((~(pInfoBuffer[METAINDEX])) + 1) & 3;
	if (uPadCount > 0) 
	{
		m_pWriteBufferX->WriteX(pPadBuffer, m_uWritePosition, uPadCount);
		m_uWritePosition += uPadCount;
	}

	if (!m_bWithoutFileHeader) {
		// Update the file size
		SWAP64_IF_BIGENDIAN(m_uWritePosition);
		m_pWriteBufferX->WriteX((U8*)&m_uWritePosition, 24, 8);
		SWAP64_IF_BIGENDIAN(m_uWritePosition);

		// Update declaration size
		U32 uPriority = rDataBlockX.GetPriorityX();
		if ((uPriority > 0) && (m_uMaxPriority == 0))
		{
			U32 uDeclarationSize = static_cast<U32>(m_uWritePosition);
			SWAP32_IF_BIGENDIAN(uDeclarationSize);
			m_pWriteBufferX->WriteX((U8*)&uDeclarationSize, 20, 4);
		}
		if (m_uMaxPriority < uPriority)
			m_uMaxPriority = uPriority;
	}
}


// IFXUnknown

//---------------------------------------------------------------------------
//	CIFXBlockWriter::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXBlockWriterX::AddRef() 
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXBlockWriter::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXBlockWriterX::Release() 
{
	if ( 1 == m_uRefCount ) {
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXBlockWriterX::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXBlockWriterX::QueryInterface( IFXREFIID interfaceId, void** ppInterface ) 
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXBlockWriterX ) {
			*ppInterface = ( IFXBlockWriterX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown ) 
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		} else {
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}


//---------------------------------------------------------------------------
//	CIFXBlockWriterX::CIFXBlockWriterX
//
//  Constructor
//---------------------------------------------------------------------------
CIFXBlockWriterX::CIFXBlockWriterX() :
	IFXDEFINEMEMBER(m_pWriteBufferX)
{
	m_uRefCount	= 0;
	m_uWritePosition = 0;
	m_uMaxPriority = 0;
	m_bWithoutFileHeader = FALSE;
}

//---------------------------------------------------------------------------
//	CIFXBlockWriterX::~CIFXBlockWriterX
//
//  Destructor
//---------------------------------------------------------------------------
CIFXBlockWriterX::~CIFXBlockWriterX() 
{
}
