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
  @file MaterialParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "MaterialParser.h"
#include "MaterialResource.h"
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

MaterialParser::MaterialParser( FileScanner* pScanner, 
							Material* pMaterial )
: BlockParser( pScanner ), m_pMaterial( pMaterial ), m_pScanner( pScanner )
{
	IFXCHECKX_RESULT( NULL != pScanner, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pMaterial, IFX_E_INVALID_POINTER );
}

MaterialParser::~MaterialParser()
{
	m_pMaterial = NULL;
	m_pScanner = NULL;
}

IFXRESULT MaterialParser::Parse()
{
	IFXRESULT result = IFX_OK;

	// parse material attributes
	result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_AMBIENT_ENABLED, 
								&m_pMaterial->m_ambientEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_DIFFUSE_ENABLED, 
								&m_pMaterial->m_diffuseEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_SPECULAR_ENABLED, 
								&m_pMaterial->m_specularEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_EMISSIVE_ENABLED, 
								&m_pMaterial->m_emissiveEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_REFLECTIVITY_ENABLED, 
								&m_pMaterial->m_reflectivityEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_OPACITY_ENABLED, 
								&m_pMaterial->m_opacityEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	// parse material parameters
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanColorToken( 
								IDTF_MATERIAL_AMBIENT, 
								&m_pMaterial->m_ambient );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanColorToken( 
								IDTF_MATERIAL_DIFFUSE, 
								&m_pMaterial->m_diffuse );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanColorToken( 
								IDTF_MATERIAL_SPECULAR, 
								&m_pMaterial->m_specular );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanColorToken( 
								IDTF_MATERIAL_EMISSIVE, 
								&m_pMaterial->m_emissive );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( 
								IDTF_MATERIAL_REFLECTIVITY, 
								&m_pMaterial->m_reflectivity );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( 
								IDTF_MATERIAL_OPACITY, 
								&m_pMaterial->m_opacity );

	// parse material meta-data
	if( IFXSUCCESS( result ) )
		result = ParseMetaData( m_pMaterial );

	IFXASSERT( IFXSUCCESS( result ) ); 
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
