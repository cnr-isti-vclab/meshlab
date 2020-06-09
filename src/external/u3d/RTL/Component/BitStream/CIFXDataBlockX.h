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
	@file	CIFXDataBlockX.h

			Declaration of the CIFXDataBlockX class which implements the 
			IFXDataBlock and IFXDataBlockX interface.

	@note
			SetSize - Allocate an internal memory block for data
			GetSize - Return the size of the internal memory block
			SetBlockType - Sets the block type which is used to identify format of data
			GetBlockType - Returns data block type
			GetPointer - Pointer to the internal memory block
*/

#ifndef __CIFXDATABLOCKX_H__
#define __CIFXDATABLOCKX_H__

#include "IFXDataBlock.h"
#include "IFXDataBlockX.h"
#include "IFXReadBuffer.h"
#include "IFXReadBufferX.h"
#include "IFXWriteBuffer.h"
#include "IFXWriteBufferX.h"

class CIFXDataBlockX : public IFXDataBlock, public IFXReadBuffer, public IFXWriteBuffer,
                       public IFXDataBlockX, public IFXReadBufferX, public IFXWriteBufferX
{
public:

	// IFXDataBlock methods
	virtual IFXRESULT IFXAPI  SetSize( U32 count );
	virtual IFXRESULT IFXAPI  GetSize( U32* pCount );
	virtual IFXRESULT IFXAPI  SetBlockType( U32 BlockType );
	virtual IFXRESULT IFXAPI  GetBlockType( U32* pBlockType );
	virtual IFXRESULT IFXAPI  GetPointer( U8**  ppData );
	virtual U32       IFXAPI  GetPriority(void) { return m_uPriority; }
	virtual void      IFXAPI  SetPriority(U32 uPriority) { m_uPriority = uPriority; }

	// IFXMetaData
	virtual void IFXAPI  GetCountX(U32& rCount)const
	{ m_pMetaData->GetCountX(rCount); }
	virtual IFXRESULT IFXAPI  GetIndex(const IFXString& rKey, U32& uIndex)
	{ return m_pMetaData->GetIndex(rKey, uIndex); }
	virtual void IFXAPI  GetKeyX(U32 index, IFXString& rOutKey)
	{ m_pMetaData->GetKeyX(index, rOutKey); }
	virtual void IFXAPI  GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rValueType)
	{ m_pMetaData->GetAttributeX(uIndex, rValueType); }
	virtual void IFXAPI  SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rValueType)
	{ m_pMetaData->SetAttributeX(uIndex, rValueType); }
	virtual void IFXAPI  GetBinaryX(U32 uIndex, U8* pDataBuffer)
	{ m_pMetaData->GetBinaryX(uIndex, pDataBuffer); }
	virtual void IFXAPI  GetBinarySizeX(U32 uIndex, U32& rSize)
	{ m_pMetaData->GetBinarySizeX(uIndex, rSize); }
	virtual void IFXAPI  GetStringX(U32 uIndex, IFXString& rValue)
	{ m_pMetaData->GetStringX(uIndex, rValue); }
	virtual void IFXAPI  GetPersistenceX(U32 uIndex, BOOL& rPersistence)
	{ m_pMetaData->GetPersistenceX(uIndex, rPersistence); }
	virtual void IFXAPI  SetBinaryValueX(const IFXString& rKey, U32 length, const U8* data)
	{ m_pMetaData->SetBinaryValueX(rKey, length, data); }
	virtual void IFXAPI  SetStringValueX(const IFXString& rKey, const IFXString& rValue)
	{ m_pMetaData->SetStringValueX(rKey, rValue); }
	virtual void IFXAPI  SetPersistenceX(U32 uIndex, BOOL value)
	{ m_pMetaData->SetPersistenceX(uIndex, value); }
	virtual void IFXAPI  DeleteX(U32 uIndex)
	{ m_pMetaData->DeleteX(uIndex); }
	virtual void IFXAPI  DeleteAll()
	{ m_pMetaData->DeleteAll(); }
	virtual void IFXAPI  AppendX(IFXMetaDataX* pSource)
	{ m_pMetaData->AppendX(pSource); }

	virtual void IFXAPI  GetEncodedKeyX(U32 uIndex, IFXString& rOutKey)
	{ m_pMetaData->GetEncodedKeyX(uIndex, rOutKey); }
	virtual void IFXAPI  GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes)
	{ m_pMetaData->GetSubattributesCountX(uIndex, rCountSubattributes); }
	virtual IFXRESULT IFXAPI  GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex)
	{ return m_pMetaData->GetSubattributeIndex(uIndex, rSubattributeName, rSubattributeIndex); }
	virtual void IFXAPI  GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName)
	{ m_pMetaData->GetSubattributeNameX(uIndex, uSubattributeIndex, rSubattributeName); }
	virtual void IFXAPI  GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue)
	{ m_pMetaData->GetSubattributeValueX(uIndex, uSubattributeIndex, pSubattributeValue); }
	virtual void IFXAPI  SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue)
	{ m_pMetaData->SetSubattributeValueX(uIndex, rSubattributeName, pSubattributeValue); }
	virtual void IFXAPI  DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex)
	{ m_pMetaData->DeleteSubattributeX(uIndex, uSubattributeIndex); }
	virtual void IFXAPI  DeleteAllSubattributes(U32 uIndex)
	{ m_pMetaData->DeleteAllSubattributes(uIndex); }

	// IFXReadBuffer
	virtual IFXRESULT IFXAPI  Read( U8* pBytes, U64 position, U32 count );
	virtual IFXRESULT IFXAPI  GetTotalSize( U64* pCount );
	virtual IFXRESULT IFXAPI  GetAvailableSize( U64* pCount );

	// IFXWriteBuffer
	virtual IFXRESULT IFXAPI  Write( U8* pBytes, U64 position, U32 count );

	// IFXDataBlockX methods
	virtual void IFXAPI  SetSizeX( U32 count );
	virtual void IFXAPI  GetSizeX( U32& rCount );
	virtual void IFXAPI  SetBlockTypeX( U32 BlockType );
	virtual void IFXAPI  GetBlockTypeX( U32& rBlockType );
	virtual void IFXAPI  GetPointerX( U8*&  rpData );
	virtual U32  IFXAPI  GetPriorityX(void) { return m_uPriority; }
	virtual void IFXAPI  SetPriorityX(U32 uPriority) { m_uPriority = uPriority; }

	// IFXReadBufferX
	virtual void IFXAPI  ReadX( U8* pBytes, U64 uPosition, U32 uCount, IFXRESULT& rWarningCode );
	virtual void IFXAPI  GetTotalSizeX( U64& ruCount );
	virtual void IFXAPI  GetAvailableSizeX( U64& ruCount );

	// IFXWriteBufferX
	virtual void IFXAPI  WriteX( U8* pBytes, U64 position, U32 count );

	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void**	ppInterface );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXDataBlockX_Factory( IFXREFIID	interfaceId, 
											void**		ppInterface );

private:
	// Private to enforce the use of the create instance method
	CIFXDataBlockX();
	virtual ~CIFXDataBlockX();

	void SetSizeActualX( U32 count );

	// PRIVATE MEMBER VARIABLES
	U32				m_uRefCount;	///< Reference counter
	U32				m_uCurrentSize;	///< Size of data block (reported to outside)
	U32				m_uBufferSize;	///< Size of data block (actual data buffer size)
	U32				m_uBlockType;	///< Data block type
	U8*				m_pData;		///< Pointer to data
	U32             m_uPriority;    ///< block priority

	IFXMetaDataX* m_pMetaData;

	// Constants
	static const U32 IFXDATABLOCK_DEFAULT_PRIORITY;
	static const U32 IFXDATABLOCK_INITIAL_SIZE;
	static const U32 IFXDATABLOCK_GROW_SIZE;
};

#endif
