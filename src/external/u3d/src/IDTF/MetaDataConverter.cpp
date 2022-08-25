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
  @file MetaDataConverter.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "MetaDataConverter.h"
#include "MetaDataList.h"
#include "IFXMetaDataX.h"

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

MetaDataConverter::MetaDataConverter( 
						const MetaDataList* pIDTFMetaDataList, 
						IFXMetaDataX* pIFXMetaData ) 
: m_pIDTFMetaDataList( pIDTFMetaDataList ), m_pIFXMetaData( pIFXMetaData )
{
}

MetaDataConverter::~MetaDataConverter()
{
	m_pIDTFMetaDataList = NULL;
	m_pIFXMetaData = NULL;
}

IFXRESULT MetaDataConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	const U32 count = m_pIDTFMetaDataList->GetMetaDataCount();
	U32 i;
	for( i = 0; i < count && IFXSUCCESS( result ); ++i )
	{
		const MetaData& rIDTFMetaData = m_pIDTFMetaDataList->GetMetaData( i );
		const IFXString& rAttribute = rIDTFMetaData.GetAttribute();
		const IFXString& rKey = rIDTFMetaData.GetKey();

		if( rAttribute == IDTF_STRING_META_DATA )
		{
			m_pIFXMetaData->SetStringValueX( 
							rKey, rIDTFMetaData.GetStringValue() );
			m_pIFXMetaData->SetAttributeX( i, IFXMETADATAATTRIBUTE_STRING );
		}
		else if ( rAttribute == IDTF_BINARY_META_DATA )
		{
			m_pIFXMetaData->SetBinaryValueX( 
								rKey, 
								rIDTFMetaData.GetBinarySize(), 
								rIDTFMetaData.GetBinaryValue() );
			m_pIFXMetaData->SetAttributeX( i, IFXMETADATAATTRIBUTE_BINARY );
		}
		else
			result = IFX_E_UNSUPPORTED;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
