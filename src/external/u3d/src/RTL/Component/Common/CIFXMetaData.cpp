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
@file	CIFXMetaData.cpp

  This module defines the CIFXMetaData component.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include "CIFXMetaData.h"
#include "IFXMemory.h"
#include "IFXCheckX.h"
#include <ctype.h>
#include <string.h>

//***************************************************************************
//	Public methods
//***************************************************************************

//---------------------------------------------------------------------------
U32 CIFXMetaData::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXMetaData::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;
		
		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}
	
	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXMetaData::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;
	
	if ( ppInterface )
	{
		if ( (interfaceId == IID_IFXMetaDataX) ||
			(interfaceId == IID_IFXUnknown) )
		{
			*ppInterface = static_cast< IFXMetaDataX* >( this );
		}
		else
		{
			*ppInterface = NULL;
			
			result = IFX_E_UNSUPPORTED;
		}
		
		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;
	
	return result;
}

void CIFXMetaData::GetCountX(U32& rCount) const
{
	rCount = m_uMDCount;
}

IFXRESULT CIFXMetaData::GetIndex(const IFXString& rKey, U32& rIndex)
{
	U32 index;
	if (FindTheKey(rKey, &index)) {
		rIndex = index;
		return IFX_OK;
	}

	return IFX_E_NO_METADATA;
}

void CIFXMetaData::GetKeyX(U32 uIndex, IFXString& rOutKey)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);
	
	rOutKey = tmpMD->Key;
}

void CIFXMetaData::GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rAttribute)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	rAttribute = tmpMD->Attribute;
}

void CIFXMetaData::SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rAttribute)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	tmpMD->Attribute = rAttribute;
}

void CIFXMetaData::GetBinaryX(U32 uIndex, U8* pDataBuffer)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	IFXCHECKX_RESULT(
		tmpMD->Attribute & IFXMETADATAATTRIBUTE_BINARY,
		IFX_E_NO_BINARY_METADATA );

	memcpy(pDataBuffer, tmpMD->pBuffer, tmpMD->Size);
}

void CIFXMetaData::GetBinarySizeX(U32 uIndex, U32& rSize)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	IFXCHECKX_RESULT(
		tmpMD->Attribute & IFXMETADATAATTRIBUTE_BINARY,
		IFX_E_NO_BINARY_METADATA );

	rSize = tmpMD->Size;
}

void CIFXMetaData::GetStringX(U32 uIndex, IFXString& rValue)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	IFXCHECKX_RESULT(
		(tmpMD->Attribute & IFXMETADATAATTRIBUTE_BINARY) == 0,
		IFX_E_NO_STRING_METADATA);

	rValue = (IFXString*)tmpMD->pBuffer;
}

void CIFXMetaData::GetPersistenceX(U32 uIndex, BOOL& rPersistence)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	rPersistence = tmpMD->Persistence;
}

void CIFXMetaData::SetBinaryValueX(const IFXString& rKey, U32 size, const U8* pData)
{
	IFXString key = rKey;
	IFXSubattributes sa;
	UnpackKey(key, sa);

	U32 index;
	IFXMetaDataContainer* ExistingMD = FindTheKey(key, &index);

	if (ExistingMD) {
		ExistingMD->Subattributes = sa;
		if (ExistingMD->Attribute & IFXMETADATAATTRIBUTE_BINARY)
			delete [] (U8*)ExistingMD->pBuffer;
		else 
			delete (IFXString*)ExistingMD->pBuffer;
		ExistingMD->pBuffer = (U8*)new U8[size];
		memcpy(ExistingMD->pBuffer, pData, size);
		ExistingMD->Size = size;
		ExistingMD->Attribute = (IFXMetaDataAttribute)(ExistingMD->Attribute | IFXMETADATAATTRIBUTE_BINARY);
		ExistingMD->Persistence = IFX_TRUE;
	} else {
		m_pEmptyMetaData->Key = key;
		m_pEmptyMetaData->Subattributes = sa;
		m_pEmptyMetaData->pBuffer = (U8*)new U8[size];
		memcpy(m_pEmptyMetaData->pBuffer, pData, size);
		m_pEmptyMetaData->Size = size;
		m_pEmptyMetaData->Attribute = (IFXMetaDataAttribute)(m_pEmptyMetaData->Attribute | IFXMETADATAATTRIBUTE_BINARY);
		m_pEmptyMetaData->Persistence = IFX_TRUE;

		m_pEmptyMetaData->pNext = new(IFXMetaDataContainer);
		m_pEmptyMetaData->pNext->pPrev = m_pEmptyMetaData;
		m_pEmptyMetaData = m_pEmptyMetaData->pNext;

		m_uMDCount++;
	}
}

void CIFXMetaData::SetStringValueX(const IFXString& rKey, const IFXString& rValue)
{
	IFXString key = rKey;
	IFXSubattributes sa;
	UnpackKey(key, sa);

	U32 index;
	IFXMetaDataContainer* ExistingMD = FindTheKey(key, &index);

	if (ExistingMD) {
		ExistingMD->Subattributes = sa;
		if (ExistingMD->Attribute & IFXMETADATAATTRIBUTE_BINARY)
			delete [] (U8*)ExistingMD->pBuffer;
		else 
			delete (IFXString*)ExistingMD->pBuffer;
		ExistingMD->pBuffer = new IFXString;
		(*(IFXString*)(ExistingMD->pBuffer)) = rValue;
		ExistingMD->Attribute = (IFXMetaDataAttribute)(ExistingMD->Attribute & ~IFXMETADATAATTRIBUTE_BINARY);
		ExistingMD->Persistence = IFX_TRUE;
	} else {
		m_pEmptyMetaData->Key = key;
		m_pEmptyMetaData->Subattributes = sa;
		m_pEmptyMetaData->pBuffer = new IFXString;
		(*(IFXString*)(m_pEmptyMetaData->pBuffer)) = rValue;
		m_pEmptyMetaData->Attribute = (IFXMetaDataAttribute)(m_pEmptyMetaData->Attribute & ~IFXMETADATAATTRIBUTE_BINARY);
		m_pEmptyMetaData->Persistence = IFX_TRUE;

		m_pEmptyMetaData->pNext = new(IFXMetaDataContainer);
		m_pEmptyMetaData->pNext->pPrev = m_pEmptyMetaData;
		m_pEmptyMetaData = m_pEmptyMetaData->pNext;

		m_uMDCount++;
	}
}

void CIFXMetaData::SetPersistenceX(U32 uIndex, BOOL value)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD;
	tmpMD = GetMetaData(uIndex);

	tmpMD->Persistence = value;
}

void CIFXMetaData::DeleteX(U32 uIndex)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	if (uIndex == 0) {
		IFXMetaDataContainer* tempMD = m_pMetaData->pNext;
		if (m_pMetaData->Attribute & IFXMETADATAATTRIBUTE_BINARY)
			delete [] (U8*)m_pMetaData->pBuffer;
		else 
			delete (IFXString*)m_pMetaData->pBuffer;
		m_pMetaData->Subattributes.Clear();
		delete(m_pMetaData);
		m_pMetaData = tempMD;
		m_pMetaData->pPrev = NULL;
	} else {
		IFXMetaDataContainer* tempMD = m_pMetaData->pNext;
		U32 i;
		for ( i = 0; i < uIndex-1; i++) {
			tempMD = tempMD->pNext;
		}
		if (tempMD->Attribute & IFXMETADATAATTRIBUTE_BINARY)
			delete [] (U8*)tempMD->pBuffer;
		else 
			delete (IFXString*)tempMD->pBuffer;
		tempMD->pNext->pPrev = tempMD->pPrev;
		tempMD->pPrev->pNext = tempMD->pNext;
		tempMD->Subattributes.Clear();
		delete(tempMD);
	}
	m_uMDCount--;
}

void CIFXMetaData::DeleteAll()
{
	if (m_uMDCount) {
		IFXMetaDataContainer* tempMD;
		while (m_pEmptyMetaData->pPrev) {
			tempMD = m_pEmptyMetaData->pPrev;
			m_pEmptyMetaData->pPrev = tempMD->pPrev;
			if (tempMD->Attribute & IFXMETADATAATTRIBUTE_BINARY)
				delete [] (U8*)tempMD->pBuffer;
			else 
				delete (IFXString*)tempMD->pBuffer;
			tempMD->Subattributes.Clear();
			delete(tempMD);
		}
		m_pMetaData = m_pEmptyMetaData;
		m_uMDCount = 0;
	}
}

void CIFXMetaData::AppendX(IFXMetaDataX* pSource)
{
	IFXString sKey, sEncodedKey, sValue;
	U32 srcCount, srcIndex, srcSize, dstIndex;
	IFXMetaDataAttribute srcType;
	U8* pBuffer;
	IFXRESULT rc;
	BOOL Persistance = true;
	
	IFXCHECKX_RESULT( pSource, IFX_E_NOT_INITIALIZED );
	pSource->GetCountX(srcCount);
	
	for (srcIndex = 0; srcIndex < srcCount; srcIndex++) {
		pSource->GetKeyX(srcIndex, sKey);
		pSource->GetEncodedKeyX(srcIndex, sEncodedKey);
		pSource->GetAttributeX(srcIndex, srcType);

		if (IsKeyExists(sKey)) {
			rc = GetIndex(sKey, dstIndex);
			IFXCHECKX_RESULT( IFXSUCCESS( rc ), IFX_E_NO_METADATA );
			DeleteX(dstIndex);
		}

		if (srcType & IFXMETADATAATTRIBUTE_BINARY) {
			pSource->GetBinarySizeX(srcIndex, srcSize);
			pBuffer = (U8*)new U8[srcSize];
			IFXCHECKX_RESULT( pBuffer, IFX_E_NOT_INITIALIZED );
			pSource->GetBinaryX(srcIndex, pBuffer);
			SetBinaryValueX(sEncodedKey, srcSize, pBuffer);
			delete [] pBuffer;
		} else {
			pSource->GetStringX(srcIndex, sValue);
			SetStringValueX(sEncodedKey, sValue);
		}
		GetIndex(sKey, dstIndex);
		SetAttributeX(dstIndex, srcType);

		pSource->GetPersistenceX(srcIndex, Persistance);
		rc = GetIndex(sKey, dstIndex);
		IFXCHECKX_RESULT(IFXSUCCESS( rc ), IFX_E_NO_METADATA);
		SetPersistenceX(dstIndex, Persistance);
	}
}

void CIFXMetaData::GetEncodedKeyX(U32 uIndex, IFXString& rOutKey)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	rOutKey = tmpMD->Key;
	U32 l = tmpMD->Subattributes.GetNumberElements();
	U32 i;
	for ( i = 0; i < l; i++) {
		rOutKey += L"#";
		rOutKey += tmpMD->Subattributes.GetElement(i).Name;
		if (!tmpMD->Subattributes.GetElement(i).NoValue) {
			rOutKey += L"=";
			bool quoted = false;
			IFXString value0 = tmpMD->Subattributes.GetElement(i).Value, value = L"";
			U32 len = value0.Length();
			U32 j;
			for ( j = 0; j < len; j++) {
				IFXCHAR ch;
				value0.GetAt(j, &ch);
				if ( ch == L' ' || ( ch >= 0x09 && ch <= 0x0D ) || ch == L'#' || ch == L'"' ) quoted = true;
				if (ch == L'"') value += L"\"";
				IFXString tmp = L" ";
				tmp.SetAt(0, &ch);
				value += tmp;
			}
			if (quoted)
				value = IFXString(L"\"") + value + IFXString(L"\"");
			rOutKey += value;
		}
	}
}

void CIFXMetaData::GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	rCountSubattributes = tmpMD->Subattributes.GetNumberElements();
}

IFXRESULT CIFXMetaData::GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex)
{
	if (m_uMDCount == 0) return IFX_E_NO_METADATA;
	if (uIndex >= m_uMDCount) return IFX_E_OUT_OF_METADATA;

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	U32 l = tmpMD->Subattributes.GetNumberElements();
	U32 i;
	for ( i = 0; i < l; i++)
		if (tmpMD->Subattributes.GetElement(i).Name == rSubattributeName) {
			rSubattributeIndex = i;
			return IFX_OK;
		}

	return IFX_E_NO_METADATA;
}

void CIFXMetaData::GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	IFXCHECKX_RESULT( uSubattributeIndex < tmpMD->Subattributes.GetNumberElements(), IFX_E_OUT_OF_METADATA );

	rSubattributeName = tmpMD->Subattributes.GetElement(uSubattributeIndex).Name;
}

void CIFXMetaData::GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );
	IFXCHECKX_RESULT( pSubattributeValue, IFX_E_INVALID_POINTER );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	IFXCHECKX_RESULT( uSubattributeIndex < tmpMD->Subattributes.GetNumberElements(), IFX_E_OUT_OF_METADATA );

	IFXMetaDataSubattribute& sa = tmpMD->Subattributes.GetElement(uSubattributeIndex);
	if (sa.NoValue)
		pSubattributeValue = NULL;
	else
		*pSubattributeValue = sa.Value;
}

void CIFXMetaData::SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	if (IsSubattributeExists(tmpMD->Subattributes, rSubattributeName)) {
		IFXMetaDataSubattribute* pSA = GetSubattribute(tmpMD->Subattributes, rSubattributeName);
		pSA->NoValue = (pSubattributeValue == NULL);
		pSA->Value = (pSubattributeValue == NULL) ? L"" : *pSubattributeValue;
	} else {
		IFXMetaDataSubattribute& sa = tmpMD->Subattributes.CreateNewElement();
		sa.Name = rSubattributeName;
		sa.NoValue = (pSubattributeValue == NULL);
		sa.Value = (pSubattributeValue == NULL) ? L"" : *pSubattributeValue;
	}
}

void CIFXMetaData::DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	IFXCHECKX_RESULT( uSubattributeIndex < tmpMD->Subattributes.GetNumberElements(), IFX_E_OUT_OF_METADATA );

	tmpMD->Subattributes.DeleteElement(uSubattributeIndex);
}

void CIFXMetaData::DeleteAllSubattributes(U32 uIndex)
{
	IFXCHECKX_RESULT( m_uMDCount != 0, IFX_E_NO_METADATA );
	IFXCHECKX_RESULT( uIndex < m_uMDCount, IFX_E_OUT_OF_METADATA );

	IFXMetaDataContainer* tmpMD = GetMetaData(uIndex);

	tmpMD->Subattributes.Clear();
}

//***************************************************************************
//	Private methods
//***************************************************************************

IFXMetaDataContainer* CIFXMetaData::FindTheKey(const IFXString& rValue, U32* pIndex)
{
	IFXMetaDataContainer* currMD = m_pMetaData;
	U32 i;
	for (i = 0; i < m_uMDCount; i++) {
		if (currMD->Key.Compare(&rValue) == 0) {
			*pIndex = i;
			break;
		}
		currMD = currMD->pNext;
	}
	if (currMD == m_pEmptyMetaData)
		currMD = NULL;

	return currMD;
}

BOOL CIFXMetaData::IsKeyExists(const IFXString& rValue)
{
	BOOL rc = IFX_FALSE;
	U32 index;
	
	if (FindTheKey(rValue, &index)) {
		rc = IFX_TRUE;
	}

	return rc;
}

IFXMetaDataContainer* CIFXMetaData::GetMetaData(U32 uIndex)
{
	IFXMetaDataContainer* rtrnMD = m_pMetaData;
	
	U32 i;
	for ( i = 0; i < uIndex; i++) {
		rtrnMD = rtrnMD->pNext;
	}

	return rtrnMD;
}

void CIFXMetaData::UnpackKey(IFXString& rKey, IFXSubattributes& rSubattributes)
{
	enum EParsingState { PARSING_STATE_SEARCHING = 0, PARSING_STATE_NAME = 1, PARSING_STATE_VALUE = 2 };

	U32 uFirstAttribute = 0;
	rSubattributes.Clear();
	IFXString key = rKey;
	if (IFXFAILURE(key.FindSubstring(L"#", &uFirstAttribute))) return;
	U32 uKeyLength = 0, i = uFirstAttribute-1;
	BOOL quotes = FALSE;
	EParsingState state = PARSING_STATE_SEARCHING;
	uKeyLength = key.Length();
	IFXString name = L"", value = L"";

	while (++i < uKeyLength) {
		IFXCHAR ch;
		key.GetAt(i, &ch);
		if (state == PARSING_STATE_SEARCHING) {
			if (ch != L'#') continue;
			state = PARSING_STATE_NAME;
			name = L"";
			if (i == uKeyLength-1) break;
			key.GetAt(++i, &ch);
		}
		if (state == PARSING_STATE_NAME) {
			if ((ch != L'=') && !( ch == L' ' || ( ch >= 0x09 && ch <= 0x0D ) ) && (ch != L'#')) {
				IFXString tmp = L" ";
				tmp.SetAt(0, &ch);
				name += tmp;
				continue;
			}
			if (ch == L'#') {
				IFXMetaDataSubattribute* sa = NULL;
				if (IsSubattributeExists(rSubattributes, name)) {
					sa = GetSubattribute(rSubattributes, name);
				} else {
					sa = &rSubattributes.CreateNewElement();
					sa->Name = name;
				}
				sa->NoValue = TRUE;
				sa->Value.Clear();
				name = L"";
				continue;
			}
			state = PARSING_STATE_VALUE;
			value = L"";
			if (i == uKeyLength-1) continue;
			key.GetAt(++i, &ch);
		}
		if (state == PARSING_STATE_VALUE) {
			if (ch == L'"') {
				if (!quotes && (value == L"")) {
					quotes = TRUE;
					continue;
				}
				if (quotes && (i < uKeyLength-1)) {
					IFXCHAR ch2;
					key.GetAt(i+1, &ch2);
					if (ch2 == L'"') {
						value += L"\"";
						i++;
						continue;
					}
				}
			}
			if ( ch == L'"' || (!quotes && ( ch == L' ' || ( ch >= 0x09 && ch <= 0x0D ) || ch == L'#'))) {
				IFXMetaDataSubattribute* sa = NULL;
				if (IsSubattributeExists(rSubattributes, name)) {
					sa = GetSubattribute(rSubattributes, name);
				} else {
					sa = &rSubattributes.CreateNewElement();
					sa->Name = name;
				}
				sa->NoValue = FALSE;
				sa->Value = value;
				quotes = FALSE;
				state = (ch == L'#') ? PARSING_STATE_NAME : PARSING_STATE_SEARCHING;
				name = L"";
				continue;
			}
			IFXString tmp = L" ";
			tmp.SetAt(0, &ch);
			value += tmp;
		}
	}
	if (state != PARSING_STATE_SEARCHING) {
		IFXMetaDataSubattribute* sa = NULL;
		if (IsSubattributeExists(rSubattributes, name)) {
			sa = GetSubattribute(rSubattributes, name);
		} else {
			sa = &rSubattributes.CreateNewElement();
			sa->Name = name;
		}
		if (state == PARSING_STATE_NAME) {
			sa->NoValue = TRUE;
			sa->Value.Clear();
		} else {
			sa->NoValue = FALSE;
			sa->Value = value;
		}
	}
	key.Substring(&rKey, 0, uFirstAttribute);
}

BOOL CIFXMetaData::IsSubattributeExists(IFXSubattributes& rSubattributes, const IFXString& rName)
{
	U32 l = rSubattributes.GetNumberElements();
	U32 i;
	for ( i = 0; i < l; i++)
		if (rSubattributes.GetElement(i).Name == rName)
			return TRUE;
	return FALSE;
}

IFXMetaDataSubattribute* CIFXMetaData::GetSubattribute(IFXSubattributes& rSubattributes, const IFXString& rName)
{
	U32 l = rSubattributes.GetNumberElements();
	U32 i;
	for ( i = 0; i < l; i++)
		if (rSubattributes.GetElement(i).Name == rName)
			return &rSubattributes.GetElement(i);
	return NULL;
}

//---------------------------------------------------------------------------
/**
This is the component constructor.  It doesn't do anything significant.

  @note	Significant state initialization is done by the RobustConstructor
  method if it's available or an interface specific Initialize
  method if one's supported.
*/
CIFXMetaData::CIFXMetaData() :
m_refCount( 0 )
{
	m_uMDCount = 0;
	m_pMetaData = new(IFXMetaDataContainer);
	m_pMetaData->pPrev = NULL;
	m_pEmptyMetaData = m_pMetaData;
}

//---------------------------------------------------------------------------
/**
This is the component destructor.  After all references to the component
are released, it performs any necessary cleanup activities.
*/
CIFXMetaData::~CIFXMetaData()
{
	if (m_uMDCount)
		DeleteAll();
	delete(m_pMetaData);
}

//***************************************************************************
//	Global functions
//***************************************************************************

//---------------------------------------------------------------------------
/**
This is the CIFXMetaData component factory function.  It's used by
IFXCreateComponent to create an instance of the component and hand back
a pointer to a supported interface that's requested.

  @note	This component can be instaniated multiple times.
  
	@param	interfaceId	Reference to the interface id that's supported by the
	component that will be handed back.
	@param	ppInterface	Pointer to the interface pointer that will be
	initialized upon successful creation of the
	component.
	
	  @return	Upon success, IFX_OK is returned.  Otherwise, one of the
	  following failures are returned:  IFX_E_COMPONENT,
	  IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or
	  IFX_E_UNSUPPORTED.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXMetaData_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;
	
	if ( ppInterface )
	{
		// Create the CIFXMetaData component.
		CIFXMetaData	*pComponent	= new CIFXMetaData;
		
		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();
			
			// Call the component's robust constructor that can return a
			// result code to indicate success or failure.
			//			result = pComponent->RobustConstructor();
			
			// Attempt to obtain a pointer to the requested interface.
			if ( IFXSUCCESS( result ) )
				result = pComponent->QueryInterface( interfaceId, ppInterface );
			
			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the RobustConstructor or QI fails,
			// this will cause the component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;
	
	return result;
}

