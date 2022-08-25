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
  @file MetaDataList.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "MetaDataList.h"
#include "Tokens.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

// MetaDataList

MetaDataList::MetaDataList() 
{
}

MetaDataList::~MetaDataList()
{
}

IFXRESULT MetaDataList::SetMetaData( const MetaData* pMetaData )
{
	IFXRESULT result = IFX_OK;

	MetaData& rMetaData = m_metaDataList.CreateNewElement();
	rMetaData = *pMetaData;

	return result;
}

const MetaData& MetaDataList::GetMetaData( const U32 index ) const
{
	return m_metaDataList.GetElementConst( index );
}


const U32& MetaDataList::GetMetaDataCount() const
{
	return m_metaDataList.GetNumberElements();
}


// StringMetaData

StringMetaData::StringMetaData()
{
}

StringMetaData::~StringMetaData()
{
}

void StringMetaData::SetStringValue( const IFXString& rValue )
{
	m_value = rValue;
}

const IFXString& StringMetaData::GetStringValue() const
{
	return m_value;
}


// Binary MetaData

BinaryMetaData::BinaryMetaData()
: m_pValue( NULL ), m_size( 0 )
{
}

BinaryMetaData::~BinaryMetaData()
{
	delete [] m_pValue;
}

BinaryMetaData::BinaryMetaData( BinaryMetaData& rBinary )
: m_pValue( new U8[rBinary.GetBinarySize()] ),
  m_size( rBinary.GetBinarySize() )
{
}

BinaryMetaData& BinaryMetaData::operator= ( const BinaryMetaData& rBinary )
{
	U8* pValue = new U8[rBinary.GetBinarySize()];
	delete m_pValue;
	m_pValue = pValue;
	return *this;
}

const U32& BinaryMetaData::GetBinarySize() const
{
	return m_size;
}

const U8* BinaryMetaData::GetBinaryValue() const
{
	return m_pValue;
}

IFXRESULT BinaryMetaData::SetBinaryValue( const U8* pValue, const U32 size )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pValue )
	{
		if( NULL != m_pValue )
			delete [] m_pValue;

		m_pValue = new U8[size];

		if( NULL != m_pValue )
		{
			U32 i;
			for( i = 0; i < size; ++i )
			{
				m_pValue[i] = pValue[i];
			}
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// MetaData

MetaData::MetaData()
{
}

MetaData::~MetaData()
{
}

void MetaData::SetKey( const IFXString& rKey )
{
	m_key = rKey;
}

const IFXString& MetaData::GetKey() const
{
	return m_key;
}

void MetaData::SetAttribute( const IFXString& rAttribute )
{
	m_attribute = rAttribute;
}

const IFXString& MetaData::GetAttribute() const
{
	return m_attribute;
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
