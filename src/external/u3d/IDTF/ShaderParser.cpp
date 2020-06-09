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
  @file ShaderParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ShaderParser.h"
#include "ShaderResource.h"
#include "FileScanner.h"
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

ShaderParser::ShaderParser( FileScanner* pScanner, 
							Shader* pShader )
: BlockParser( pScanner ), m_pScanner( pScanner ), m_pShader( pShader )
{
}

ShaderParser::~ShaderParser()
{
	m_pShader = NULL;
	m_pScanner = NULL;
}

IFXRESULT ShaderParser::Parse()
{
	IFXRESULT result = IFX_OK;

	I32 textureLayerCount = 0;

	// begin parse shader parameters
	result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_LIGHTING_ENABLED, 
								&m_pShader->m_lightingEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_ALPHA_TEST_ENABLED, 
								&m_pShader->m_alphaTestEnabled );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_USE_VERTEX_COLOR, 
								&m_pShader->m_useVertexColor );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( 
								IDTF_SHADER_ALPHA_TEST_REFERENCE, 
								&m_pShader->m_alphaTestReference );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_SHADER_ALPHA_TEST_FUNCTION, 
								&m_pShader->m_alphaTestFunction );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_SHADER_COLOR_BLEND_FUNCTION, 
								&m_pShader->m_colorBlendFunction );

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	// end parse shader parameters

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_SHADER_MATERIAL_NAME, 
								&m_pShader->m_materialName );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_SHADER_ACTIVE_TEXTURE_COUNT, &textureLayerCount );

	if( IFXSUCCESS( result ) && textureLayerCount > 0 )
	{
		// begin parse texture layer list
		result = BlockBegin( IDTF_SHADER_TEXTURE_LAYER_LIST );
	
		if( IFXSUCCESS( result ) )
		{
			TextureLayer textureLayer;
			
			I32 i;
			for( i = 0; i < textureLayerCount && IFXSUCCESS( result ); ++i )
			{
				// begin parse texture layer
				result = BlockBegin( IDTF_TEXTURE_LAYER, &textureLayer.m_channel );

				if( IFXSUCCESS( result ) )
				{
					result = m_pScanner->ScanFloatToken( 
												IDTF_TEXTURE_LAYER_INTENSITY, 
												&textureLayer.m_intensity );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanStringToken( 
												IDTF_TEXTURE_LAYER_BLEND_FUNCTION, 
												&textureLayer.m_blendFunction );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanStringToken( 
												IDTF_TEXTURE_LAYER_BLEND_SOURCE, 
												&textureLayer.m_blendSource );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
												IDTF_TEXTURE_LAYER_BLEND_CONSTANT, 
												&textureLayer.m_blendConstant );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanStringToken( 
												IDTF_TEXTURE_LAYER_MODE, 
												&textureLayer.m_mode );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanStringToken( 
												IDTF_TEXTURE_LAYER_ALPHA_ENABLED, 
												&textureLayer.m_alphaEnabled );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanStringToken( 
												IDTF_TEXTURE_LAYER_REPEAT, 
												&textureLayer.m_repeat );

					if( IFX_E_TOKEN_NOT_FOUND == result )
						result = IFX_OK; // use default

					if( IFXSUCCESS( result ) )
					{
						result = m_pScanner->ScanStringToken( 
											IDTF_TEXTURE_NAME, 
											&textureLayer.m_textureName );
					}

					if( IFXSUCCESS( result ) )
						m_pShader->AddTextureLayer( textureLayer );

					// end parse texture layer
					if( IFXSUCCESS( result ) )
						result = BlockEnd();
				}
			}
		}

		// end parse texture layer list
		if( IFXSUCCESS( result ) )
			result = ParseTerminator();
	}

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( m_pShader );

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
