//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
//  CIFXFileReferenceEncoder.cpp
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************

#include "CIFXFileReferenceEncoder.h"
#include "IFXCoreCIDs.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"

//-----------------------------------------------------------------------------
// IFXUnknown
//-----------------------------------------------------------------------------
U32 CIFXFileReferenceEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXFileReferenceEncoder::Release()
{
	if (1 == m_uRefCount) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXFileReferenceEncoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//-----------------------------------------------------------------------------
// IFXEncoderX
//-----------------------------------------------------------------------------

// Provide the encoder with a pointer to the object which is to be encoded.
void CIFXFileReferenceEncoder::SetObjectX(IFXUnknown &rObject)
{
	// Release any previous FileReferenceResource
	IFXRELEASE(m_pFileReference);

	// If possible, store the object's IFXFileReference interface
	IFXCHECKX(rObject.QueryInterface(IID_IFXFileReference,(void**)&m_pFileReference));

	return;
}

// Initialize and get a reference to the core services
void CIFXFileReferenceEncoder::InitializeX(IFXCoreServices &rCoreServices)
{
	// Release any previous core services pointer
	// and store the new core services reference
	rCoreServices.AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = &rCoreServices;
}

// Encode data into data blocks and place these blocks in the queue
void CIFXFileReferenceEncoder::EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units)
{
	// Verify m_pFileReference is valid
	if(NULL == m_pFileReference) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStreamX));
	IFXASSERT(pBitStreamX);

	IFXFileReferenceParams FR;
	m_pFileReference->GetScopeName(FR.ScopeName);
	m_pFileReference->GetFileURLs(FR.FileURLs);
	m_pFileReference->GetObjectFilters(FR.ObjectFilters);
	m_pFileReference->GetCollisionPolicy(FR.CollisionPolicy);
	m_pFileReference->GetWorldAlias(FR.WorldAlias);

	U32 i, n;
	// 1. Scope Name
	pBitStreamX->WriteIFXStringX(FR.ScopeName);

	// 2.
	/// @todo: Attributes
	pBitStreamX->WriteU32X(0);

	// 3.
	/// @todo: Bounding Sphere
	// 4.
	/// @todo: Axis-Aligned Bounding Box

	// 5. Number of URLs
	n = FR.FileURLs.GetNumberElements();
	pBitStreamX->WriteU32X(n);
	// 6. URLs
	for (i = 0; i < n; i++)
		pBitStreamX->WriteIFXStringX(FR.FileURLs[i]);
	// 4. Number of filters
	n = FR.ObjectFilters.GetNumberElements();
	pBitStreamX->WriteU32X(n);
	// 7. Filters
	for (i = 0; i < n; i++) {
		// 7.1. Filter type
		pBitStreamX->WriteU8X(FR.ObjectFilters[i].FilterType);
		switch (FR.ObjectFilters[i].FilterType) {
	  case IFXOBJECTFILTER_NAME:
		  // 7.2a. Object name
		  pBitStreamX->WriteIFXStringX(FR.ObjectFilters[i].ObjectNameFilterValue);
		  break;
	  case IFXOBJECTFILTER_TYPE:
		  // 7.2b. Object type
		  pBitStreamX->WriteU32X(FR.ObjectFilters[i].ObjectTypeFilterValue);
		  break;
	  default:
		  IFXASSERT(0);
		  break;
		}
	}
	// 8. Name collision policy
	pBitStreamX->WriteU8X(FR.CollisionPolicy);

	// 9. World alias name
	pBitStreamX->WriteIFXStringX(FR.WorldAlias);

	// Get the data block
	IFXDECLARELOCAL(IFXDataBlockX, pDataBlockX);
	pBitStreamX->GetDataBlockX(pDataBlockX);
	IFXASSERT(pDataBlockX);

	// Set block type
	pDataBlockX->SetBlockTypeX(BlockType_FileReferenceU3D);
	// Set block priority
	pDataBlockX->SetPriorityX(0);

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pFileReference->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pBlockMD->AppendX(pObjectMD);

	// Add block to queue
	rDataBlockQueue.AppendBlockX(*pDataBlockX);
}

//-----------------------------------------------------------------------------
// Factory Method
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXFileReferenceEncoder_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXFileReferenceEncoder *pComponent = new CIFXFileReferenceEncoder;
		if ( pComponent ) {
			pComponent->AddRef();
			rc = pComponent->QueryInterface( interfaceId, ppInterface );
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

//-----------------------------------------------------------------------------
//  Private methods
//-----------------------------------------------------------------------------

// Constuctor
CIFXFileReferenceEncoder::CIFXFileReferenceEncoder() :
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pFileReference)
{
	m_uRefCount = 0;
}

// Destuctor
CIFXFileReferenceEncoder::~CIFXFileReferenceEncoder()
{

}
