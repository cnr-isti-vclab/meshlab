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
  @file SceneConverter.cpp

      This module defines ...
*/

//***************************************************************************
//  Includes
//***************************************************************************

#include "SceneConverter.h"
#include "FileParser.h"
#include "ConverterOptions.h"
#include "SceneData.h"
#include "FileReference.h"
#include "SceneUtilities.h"
#include "NodeConverter.h"
#include "ResourceConverter.h"
#include "ModifierConverter.h"
#include "MetaDataConverter.h"
#include "FileReferenceConverter.h"
#include "Tokens.h"
#include "IFXDebug.h"
#include "IFXCheckX.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************

namespace U3D_IDTF
{
	const I32	IDTF_MIN_VERSION = 100;
	const IFXCHAR IDTF_FORMAT_NAME[] = L"IDTF";
}

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

SceneConverter::SceneConverter( FileParser* pParser, 
								SceneUtilities* pSceneUtils,
								ConverterOptions* pConverterOptions )
: m_pSceneUtils( pSceneUtils ),
  m_pParser( pParser ), 
  m_pOptions( pConverterOptions )
{
	IFXCHECKX_RESULT( NULL != pParser, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pSceneUtils, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pConverterOptions, IFX_E_INVALID_POINTER );
}

SceneConverter::~SceneConverter()
{
}

IFXRESULT SceneConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	result = CheckFileHeader();

	if( IFXSUCCESS( result ) )
		result = ConvertSceneData();
	
	if( IFXSUCCESS( result ) )
		result = ConvertFileReference();
	
	if( IFXSUCCESS( result ) )
		result = ConvertScene();

	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************
IFXRESULT SceneConverter::CheckFileHeader()
{
	IFXRESULT result = IFX_OK;
	IFXString formatName;
	I32 versionNumber = 0;

	result = m_pParser->ParseFileHeader( &formatName, &versionNumber );
	if( IFXSUCCESS( result ) )
	{
		if( formatName != IDTF_FORMAT_NAME )
		{
			result = IFX_E_WRONG_FORMAT;
			IFXTRACE_GENERIC( L"Wrong format name\n" );
		}
		if( versionNumber < IDTF_MIN_VERSION )
		{
			result = IFX_E_WRONG_VERSION_NUMBER;
			IFXTRACE_GENERIC( L"Wrong format version\n" );
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


IFXRESULT SceneConverter::ConvertSceneData()
{
	IFXRESULT result = IFX_OK;

	SceneData sceneData;

	result = m_pParser->ParseSceneData( &sceneData );

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		result = m_pSceneUtils->GetSceneMetaData( &pMetaData );
		
		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &sceneData, pMetaData );
			result = metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT SceneConverter::ConvertFileReference()
{
	IFXRESULT result = IFX_OK;
	FileReference fileReference;

	result = m_pParser->ParseFileReference( &fileReference );

	if( IFXSUCCESS( result ) )
	{
		FileReferenceConverter fileRefConverter( 
									m_pSceneUtils, &fileReference );
		result = fileRefConverter.Convert();
	}
	else if( IFX_E_TOKEN_NOT_FOUND == result )
	{
		// no file reference
		result = IFX_OK;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT SceneConverter::ConvertScene()
{
	IFXRESULT result = IFX_OK;

	result = m_pParser->ParseNodes( &m_nodeList );

	if( IFXSUCCESS( result ) )
		result = m_pParser->ParseResources( &m_sceneResources );

	if( IFXSUCCESS( result ) )
		result = m_pParser->ParseModifiers( &m_modifierList );

	if( IFXSUCCESS( result ) )
	{
		ResourceConverter resourceConverter( 
								&m_sceneResources, m_pSceneUtils, 
								m_pOptions );
		result = resourceConverter.Convert();
	}

	if( IFXSUCCESS( result ) && 0 != m_nodeList.GetNodeCount() )
	{
		NodeConverter nodeConverter( 
							&m_nodeList, 
							m_pSceneUtils );

		result = nodeConverter.Convert();
	}

	if( IFXSUCCESS( result ) && 0 != m_modifierList.GetModifierCount() )
	{
		ModifierConverter modifierConverter( &m_modifierList, m_pSceneUtils );
		result = modifierConverter.Convert();
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
