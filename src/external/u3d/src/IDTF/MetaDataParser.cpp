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
  @file MetaDataParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Includes
//***************************************************************************

#include "MetaDataParser.h"
#include "FileScanner.h"
#include "Tokens.h"
#include "MetaDataList.h"

using namespace U3D_IDTF;

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

MetaDataParser::MetaDataParser( FileScanner* pFileScanner )
: m_pScanner( pFileScanner )
{
}

MetaDataParser::~MetaDataParser()
{
}

IFXRESULT MetaDataParser::ParseMetaData( MetaDataList* pMetaDataList )
{
	IFXRESULT result = IFX_OK;

	// parse meta data block
	result = m_pScanner->ScanToken( IDTF_META_DATA );

	if( IFXSUCCESS( result ) )
	{
		result = m_pScanner->FindBlockStarter();

		if( IFXSUCCESS( result ) )
		{
			I32 count = 0;

			result = m_pScanner->ScanIntegerToken( IDTF_META_DATA_COUNT, &count );

			if( IFXSUCCESS( result ) && 0 < count )
			{
				MetaData metaData;
				I32 i;
				for( i = 0; i < count && IFXSUCCESS( result ); ++i )
				{
					I32 number = -1;
					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanIntegerToken( IDTF_META_DATA, &number );
		
					if( IFXSUCCESS( result ) )
						result = m_pScanner->FindBlockStarter();

					if( IFXSUCCESS( result ) )
						result = ParseMetaDataItem( &metaData );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->FindBlockTerminator();

					if( IFXSUCCESS( result ) )
						result = pMetaDataList->SetMetaData( &metaData );
				}
			}
		}

		result = m_pScanner->FindBlockTerminator();
	}
	else if( IFX_E_TOKEN_NOT_FOUND == result )
	{
		// no metadata
		result = IFX_OK;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


//***************************************************************************
//  Protected methods
//***************************************************************************

//***************************************************************************
//  Private methods
//***************************************************************************

IFXRESULT MetaDataParser::ParseMetaDataItem( MetaData* pMetaData )
{
	IFXRESULT result = IFX_OK;
	IFXString attribute;
	IFXString key;
	IFXString value;

	if( NULL == pMetaData )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_META_DATA_ATTRIBUTE, &attribute );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_META_DATA_KEY, &key );

	if( IFXSUCCESS( result ) )	
		result = m_pScanner->ScanStringToken( 
								IDTF_META_DATA_VALUE, &value );

	if( IFXSUCCESS( result ) )
	{
		pMetaData->SetAttribute( attribute );
		pMetaData->SetKey( key );
		pMetaData->SetStringValue( value );
	}

	return result;
}

//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
