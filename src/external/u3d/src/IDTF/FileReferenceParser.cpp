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
  @file FileReferenceParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Includes
//***************************************************************************

#include "FileReferenceParser.h"
#include "FileScanner.h"
#include "UrlListParser.h"
#include "Tokens.h"
#include "FileReference.h"
#include "IFXCheckX.h"

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

FileReferenceParser::FileReferenceParser( 
						FileScanner* pFileScanner, 
						FileReference* pFileReference )
: BlockParser( pFileScanner ),
  m_pScanner( pFileScanner ),
  m_pFileReference( pFileReference )
  
{
	IFXCHECKX_RESULT( NULL != pFileScanner, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pFileReference, IFX_E_INVALID_POINTER );
}

FileReferenceParser::~FileReferenceParser()
{
	m_pScanner = NULL;
	m_pFileReference = NULL;
}

IFXRESULT FileReferenceParser::Parse()
{
	IFXRESULT result = IFX_OK;

	// parse file reference block
	result = BlockBegin( IDTF_FILE_REFERENCE );

	if( IFXSUCCESS( result ) )
	{
		IFXString scopeName;
		IFXString collisionPolicy;
		IFXString worldAlias;

		result = m_pScanner->ScanStringToken( IDTF_SCOPE_NAME, &scopeName );

		if( IFXSUCCESS( result ) )
		{
			UrlList urlList;

			UrlListParser urlListParser( m_pScanner, &urlList );

			result = urlListParser.Parse();

			if( IFXSUCCESS( result ) )
				m_pFileReference->AddUrlList( urlList );
		}

		if( IFXSUCCESS( result ) )
			result = ParseFilterList();

		if( IFXSUCCESS( result ) )
			result = m_pScanner->ScanStringToken( 
									IDTF_NAME_COLLISION_POLICY, 
									&collisionPolicy );

		if( IFXSUCCESS( result ) )
			result = m_pScanner->ScanStringToken( 
									IDTF_WORLD_ALIAS_NAME, 
									&worldAlias );

		if( IFXSUCCESS( result ) )
			result = BlockEnd();

		if( IFXSUCCESS( result ) )
		{
			m_pFileReference->SetScopeName( scopeName );
			m_pFileReference->SetCollisionPolicy( collisionPolicy );
			m_pFileReference->SetWorldAlias( worldAlias );
		}

		IFXASSERT( IFXSUCCESS( result ) ); 
	}

	return result;
}


//***************************************************************************
//  Protected methods
//***************************************************************************

//***************************************************************************
//  Private methods
//***************************************************************************

IFXRESULT FileReferenceParser::ParseFilter( Filter& rFilter )
{
	IFXRESULT result = IFX_OK;
	IFXString filterType;

	result = m_pScanner->ScanStringToken( 
									IDTF_FILTER_TYPE,
									&filterType );

	if( IFXSUCCESS( result ) )
	{
		IFXString filterObjectName;
		I32 filterObjectType;

		if( filterType == IDTF_FILTER_TYPE_NAME )
		{
			result = m_pScanner->ScanStringToken( 
									IDTF_FILTER_OBJECT_NAME,
									&filterObjectName);

			if( IFXSUCCESS( result ) )
				rFilter.SetObjectName( filterObjectName );
		}
		else if( filterType == IDTF_FILTER_TYPE_TYPE )
		{
			result = m_pScanner->ScanHexToken( 
									IDTF_FILTER_OBJECT_TYPE, 
									&filterObjectType );

			if( IFXSUCCESS( result ) )
				rFilter.SetObjectType( filterObjectType );
		}
		else
			result = IFX_E_UNSUPPORTED;
	}

	if( IFXSUCCESS( result ) )
		rFilter.SetType( filterType );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT FileReferenceParser::ParseFilterList()
{
	IFXRESULT result = IFX_OK;
	I32 filterCount = 0;

	result = m_pScanner->ScanIntegerToken( IDTF_FILTER_COUNT, &filterCount );

	if( IFXSUCCESS( result ) && 0 < filterCount )
	{
		result = BlockBegin( IDTF_FILTER_LIST );

		I32 i;
		for( i = 0; i < filterCount && IFXSUCCESS( result ); ++i )
		{
			Filter filter;
			I32 number = -1;

			result = BlockBegin( IDTF_FILTER, &number );

			if( IFXSUCCESS( result ) && i == number )
				result = ParseFilter( filter );

			if( IFXSUCCESS( result ) )
				result = BlockEnd();

			if( IFXSUCCESS( result ) )
				result = m_pFileReference->AddFilter( filter );
		}

		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
