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
  @file UrlListParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "UrlListParser.h"
#include "UrlList.h"
#include "FileScanner.h"
#include "Tokens.h"

#include "IFXCheckX.h"

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

UrlListParser::UrlListParser( 
							FileScanner* pScanner, 
							UrlList* pUrlList )
: BlockParser( pScanner ), m_pScanner( pScanner ), m_pUrlList( pUrlList )
{
	IFXCHECKX_RESULT( NULL != pScanner, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pUrlList, IFX_E_INVALID_POINTER );
}

UrlListParser::~UrlListParser()
{
	m_pUrlList = NULL;
	m_pScanner = NULL;
}


IFXRESULT UrlListParser::Parse()
{
	IFXRESULT result = IFX_OK;
	I32 urlCount = 0;

	result = m_pScanner->ScanIntegerToken( IDTF_URL_COUNT, &urlCount );

	if( IFXSUCCESS( result ) && 0 < urlCount )
	{
		result = BlockBegin( IDTF_URL_LIST );

		I32 i;
		for( i = 0; i < urlCount && IFXSUCCESS( result ); ++i )
		{
			IFXString url;
			I32 number = -1;

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanIntegerToken( IDTF_URL, &number );

			if( IFXSUCCESS( result ) && i == number )
				result = m_pScanner->ScanString( &url );

			if( IFXSUCCESS( result ) )
				m_pUrlList->AddUrl( url );
		}

		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}

	return result;
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
