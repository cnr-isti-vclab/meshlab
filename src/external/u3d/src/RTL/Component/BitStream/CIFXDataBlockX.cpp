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
//
//  CIFXDataBlockX.cpp
//
//  DESCRIPTION:
//    Implementation for CIFXDataBlockX class.
//    The CIFXDataBlockX implements the IFXDataBlock and IFXDataBlockX
//    interfaces which are used to store a data block that has been read.
//
//*****************************************************************************

//*****************************************************************************
//  Includes
//*****************************************************************************
#include "CIFXDataBlockX.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXCOM.h"
#include <memory.h>

// set the value for the static member variable...
const U32 CIFXDataBlockX::IFXDATABLOCK_DEFAULT_PRIORITY = 256;
const U32 CIFXDataBlockX::IFXDATABLOCK_INITIAL_SIZE = 4096;
const U32 CIFXDataBlockX::IFXDATABLOCK_GROW_SIZE = 4096;


// IFXDataBlock

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::SetSize
//
// Setsize will cause an internal memory block of size "count" bytes to be
// allocated
// Pointers previously retrieved by GetPointer may become invalid, since
// this may trigger a realloc.
// Manages m_uBufferSize, which reflects the allocated buffer size, some
// of which will be invalid data.
//-----------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::SetSize( U32 count )
{
  IFXRESULT rc = IFX_OK;

  try {
    SetSizeX(count);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::SetSizeX
//
// SetsizeX will cause an internal memory block of size "count" bytes to be
// allocated
// Pointers previously retrieved by GetPointer may become invalid, since
// this may trigger a realloc.
// Manages m_uBufferSize, which reflects the allocated buffer size, some
// of which will be invalid data.
//-----------------------------------------------------------------------------
void CIFXDataBlockX::SetSizeX( U32 count )
{
  //IFXASSERT( m_uCurrentSize == 0 );

  if(count > m_uBufferSize) {
    SetSizeActualX(count);
  }

  m_uCurrentSize = count;
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetSize
//
// GetSize will return the total number of bytes in the datablock.
// Manages m_uBufferSize, which reflects the allocated buffer size, some
// of which will be invalid data.
//-----------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::GetSize( U32* pCount )
{
  IFXRESULT rc = IFX_OK;

  try {
    GetSizeX(*pCount);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetSizeX
//
// GetSizeX will return the total number of bytes in the datablock.
// Manages m_uBufferSize, which reflects the allocated buffer size, some
// of which will be invalid data.
//-----------------------------------------------------------------------------
void CIFXDataBlockX::GetSizeX( U32& rCount )
{
  rCount = m_uCurrentSize;
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetTotalSize
//
// Get number of bytes in entire stream, which may _not_ be fully populated
// with valid data.
//-----------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::GetTotalSize( U64* pCount )
{
  IFXRESULT rc = IFX_OK;

  try {
    if(NULL == pCount) {
      IFXCHECKX(IFX_E_INVALID_POINTER);
    }
    GetTotalSizeX(*pCount);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetTotalSizeX
//
// Get number of bytes in entire stream, which may _not_ be fully populated
// with valid data.
//-----------------------------------------------------------------------------
void CIFXDataBlockX::GetTotalSizeX( U64& rCount )
{
  IFXCHECKX(IFX_E_UNSUPPORTED);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetAvailableSize
//
// Set number of bytes in stream that are valid data
//-----------------------------------------------------------------------------

IFXRESULT CIFXDataBlockX::GetAvailableSize( U64* pCount )
{
  IFXRESULT rc = IFX_OK;

  try {
    if(NULL == pCount) {
      IFXCHECKX(IFX_E_INVALID_POINTER);
    }
    GetAvailableSizeX(*pCount);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

void CIFXDataBlockX::GetAvailableSizeX( U64& ruCount )
{
  ruCount = m_uCurrentSize; // number of bytes that contain valid data
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::SetBlockType
//
// All IFXDataBlocks have a BlockType which is used to identify the format of
// the contents of the data block. SetBlockType is used to set this info
//-----------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::SetBlockType( U32 BlockType )
{
  IFXRESULT rc = IFX_OK;

  try {
    SetBlockTypeX(BlockType);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::SetBlockTypeX
//
// All IFXDataBlocks have a BlockType which is used to identify the format of
// the contents of the data block. SetBlockTypeX is used to set this info
//-----------------------------------------------------------------------------
void CIFXDataBlockX::SetBlockTypeX( U32 BlockType )
{
  m_uBlockType = BlockType;
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetBlockType
//
// GetBlockType returns the BlockType of this data block
//-----------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::GetBlockType( U32* pBlockType )
{
  IFXRESULT rc = IFX_OK;

  try {
    if(NULL == pBlockType) {
      IFXCHECKX(IFX_E_INVALID_POINTER);
    }
    GetBlockTypeX(*pBlockType);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetBlockTypeX
//
// GetBlockType returns the BlockType of this data block
//-----------------------------------------------------------------------------
void CIFXDataBlockX::GetBlockTypeX( U32& rBlockType )
{
  rBlockType = m_uBlockType;
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetPointer
//
// GetPointer will pass back the location of the data in a datablock
//-----------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::GetPointer( U8** ppData )
{
  IFXRESULT rc = IFX_OK;

  try {
    if(NULL == ppData) {
      IFXCHECKX(IFX_E_INVALID_POINTER);
    }
    GetPointerX(*ppData);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
//  CIFXDataBlockX::GetPointerX
//
// GetPointer will pass back the location of the data in a datablock
//-----------------------------------------------------------------------------
void CIFXDataBlockX::GetPointerX( U8*& rpData )
{
  rpData = m_pData;
}



// IFXReadBuffer
IFXRESULT CIFXDataBlockX::Read( U8* pBytes, U64 position, U32 count )
{
  IFXRESULT rc = IFX_OK;

  try {
    ReadX(pBytes,position,count,rc);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

void CIFXDataBlockX::ReadX( U8* pBytes, U64 uPosition, U32 uCount, IFXRESULT& rWarningCode )
{
  rWarningCode = IFX_OK;

  if(NULL == pBytes) {
    IFXCHECKX(IFX_E_INVALID_POINTER);
  }

  // Make sure read will not exceed number of available bytes
  U32 needSize = (U32)uPosition + uCount;
  if (needSize > m_uCurrentSize) {
    rWarningCode = IFX_W_DATA_NOT_AVAILABLE;
  } else {
    memcpy(pBytes, m_pData + uPosition, uCount);
  }

  return;
}


// IFXWriteBuffer
IFXRESULT CIFXDataBlockX::Write( U8* pBytes, U64 position, U32 count )
{
  IFXRESULT rc = IFX_OK;

  try {
    WriteX( pBytes,position,count);
  }

  catch(IFXException e) {
    rc = e.GetIFXResult();
  }

  IFXRETURN(rc);
}

void CIFXDataBlockX::WriteX( U8* pBytes, U64 position, U32 count )
{
  if(NULL == pBytes) {
    IFXCHECKX(IFX_E_INVALID_POINTER);
  }

  // Ensure buffer is large enough
  U32 needSize = (U32)position + count;

  if (needSize > m_uBufferSize) {
    // We need to allocate a new buffer.
    // First compute the new size of the buffer with
    // extra space to grow into.
    U32 newSize = 0;
    if (m_uBufferSize == 0) {
      newSize = needSize + IFXDATABLOCK_INITIAL_SIZE;
    } else {
      newSize = needSize + IFXDATABLOCK_GROW_SIZE;
    }

    SetSizeActualX(newSize);
  }

  // Write the data
  memcpy(m_pData + position, pBytes, count);
  // Keep track of farthest byte written
  if ((U32)position + count > m_uCurrentSize) {
    m_uCurrentSize = (U32)position + count;
  }

  return;
}


// IFXUnknown...

//---------------------------------------------------------------------------
//  CIFXDataBlockX::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32
CIFXDataBlockX::AddRef()
{
  return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXDataBlockX::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32
CIFXDataBlockX::Release()
{
  if ( 1 == m_uRefCount )
  {
    delete this;

    // This second return point is used so that the deleted object's
    // reference count isn't referenced after the memory is released.
    return 0;
  }

  return --m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXDataBlockX::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXDataBlockX::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT rc = IFX_OK;

  if ( ppInterface )
  {
	if ( IID_IFXUnknown == interfaceId ) {
	  *ppInterface = ( IFXUnknown* ) this;
	  this->AddRef();
	}

	else if ( IID_IFXDataBlock == interfaceId  )
    {
      *ppInterface = ( IFXDataBlock* ) this;
      this->AddRef();
    }

    else if( IID_IFXReadBuffer == interfaceId )
    {
      *ppInterface = ( IFXReadBuffer* ) this;
      this->AddRef();
    }

    else if ( IID_IFXWriteBuffer == interfaceId )
    {
      *ppInterface = ( IFXWriteBuffer* ) this;
      this->AddRef();
    }

    else if ( IID_IFXDataBlockX == interfaceId )
    {
      *ppInterface = ( IFXDataBlockX* ) this;
      this->AddRef();
    }

    else if( IID_IFXReadBufferX == interfaceId )
    {
      *ppInterface = ( IFXReadBufferX* ) this;
      this->AddRef();
    }

    else if ( IID_IFXWriteBufferX == interfaceId )
    {
      *ppInterface = ( IFXWriteBufferX* ) this;
      this->AddRef();
    }

    else if ( IID_IFXMetaDataX == interfaceId )
    {
      *ppInterface = ( IFXMetaDataX* ) this;
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

//-----------------------------------------------------------------------------
//  CIFXDataBlock_Factory
//
//  This is the CIFXDataBlockX component factory function.  The
//  CIFXDataBlockX component can be instaniated multiple times.
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXDataBlockX_Factory( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT rc = IFX_OK;

  if ( ppInterface )
  {
    // Create the CIFXDataBlockX component.
    CIFXDataBlockX  *pComponent = new CIFXDataBlockX;

    if ( pComponent )
    {
      // Perform a temporary AddRef for our usage of the component.
      pComponent->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      rc = pComponent->QueryInterface( interfaceId, ppInterface );

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

//---------------------------------------------------------------------------
//  CIFXDataBlockX::CIFXDataBlockX
//
//  Constructor
//---------------------------------------------------------------------------
CIFXDataBlockX::CIFXDataBlockX()
{
  m_uRefCount   = 0;    // Reference counter
  m_uCurrentSize  = 0;    // Size of data block
  m_uBufferSize = 0;
  m_uBlockType  = 0x0;    // Data block type
  m_pData     = NULL;   // Pointer to data

  // set default block priority
  m_uPriority      = IFXDATABLOCK_DEFAULT_PRIORITY;

  m_pMetaData = NULL;
  IFXCHECKX(IFXCreateComponent(CID_IFXMetaDataX, IID_IFXMetaDataX, (void**)&m_pMetaData));
}

//---------------------------------------------------------------------------
//  CIFXDataBlockX::~CIFXDataBlockX
//
//  Destructor
//---------------------------------------------------------------------------
CIFXDataBlockX::~CIFXDataBlockX()
{
  if( m_pData ) {
    delete [] m_pData;
    m_pData = NULL;
  }
  IFXRELEASE(m_pMetaData);
  m_pMetaData = NULL;
}

void CIFXDataBlockX::SetSizeActualX(U32 count)
{
  // If the current buffer is not big enough
  if(count > m_uBufferSize) {
    // Allocate new memory
    U8 *pNewData = NULL;
    pNewData = new U8[count];

    if(NULL == pNewData) {
      IFXCHECKX(IFX_E_OUT_OF_MEMORY);
    }

    // Initialize the data to nothing
    memset(pNewData, 0, count );

    // Copy data from old memory to new memory
    if ( m_pData ) {
      memcpy(pNewData,m_pData,m_uBufferSize);
      // Delete the old memory
      IFXDELETE_ARRAY(m_pData);
    }

    // Set the member variables
    m_pData = pNewData;
    m_uBufferSize = count;
  }
}
