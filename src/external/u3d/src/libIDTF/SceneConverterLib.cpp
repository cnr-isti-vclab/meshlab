/*
 *  SceneConverter.cpp
 *  IDTFlib
 *
 *  Created by Michail Vidiassov on 26.01.09.
 *  Copyright 2009 MSU. All rights reserved.
 *
 */

#include "SceneConverterLib.h"
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
#include "Writer.h"

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

SceneConverter::SceneConverter( SceneUtilities* pSceneUtils,
							   ConverterOptions* pConverterOptions )
: m_pSceneUtils( pSceneUtils ),
  m_pOptions( pConverterOptions )
{
	IFXCHECKX_RESULT( NULL != pSceneUtils, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pConverterOptions, IFX_E_INVALID_POINTER );
}

SceneConverter::~SceneConverter()
{
}

IFXRESULT SceneConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = ConvertSceneData();
	
	if( IFXSUCCESS( result ) )
		result = ConvertFileReference();
	
	if( IFXSUCCESS( result ) )
		result = ConvertScene();

	return result;
}

void SceneConverter::Export( const char* pFileName )
{
	IFXWriter* pWriter = new IFXWriter( pFileName );
	pWriter->open();
	pWriter->restorePrecision();
	pWriter->exportFileHeader();
	pWriter->exportSceneData( &m_sceneData );
	pWriter->exportFileReference( &m_fileReference );
	pWriter->exportNodes( &m_nodeList );
	pWriter->exportResources( &m_sceneResources );
	pWriter->exportModifiers( &m_modifierList );
	pWriter->close();
}

//***************************************************************************
//  Protected methods
//***************************************************************************
IFXRESULT SceneConverter::ConvertSceneData()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		result = m_pSceneUtils->GetSceneMetaData( &pMetaData );
		
		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &m_sceneData, pMetaData );
			result = metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT SceneConverter::ConvertFileReference()
{
	IFXRESULT result = IFX_OK;

	if ( m_fileReference.GetScopeName().Length() == 0 )
		return result;

	if( IFXSUCCESS( result ) )
	{
		FileReferenceConverter fileRefConverter( 
									m_pSceneUtils, &m_fileReference );
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

