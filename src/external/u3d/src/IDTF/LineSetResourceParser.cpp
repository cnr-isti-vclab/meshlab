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
	@file LineSetResourceParser.cpp

	This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "LineSetResourceParser.h"
#include "LineSetResource.h"
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

LineSetResourceParser::LineSetResourceParser( FileScanner* pScanner, 
											  LineSetResource* pLineSetResource )
: ModelResourceParser( pScanner, pLineSetResource ), 
  m_pLineSetResource( pLineSetResource )
{
}

LineSetResourceParser::~LineSetResourceParser()
{
}

IFXRESULT LineSetResourceParser::Parse()
{
	IFXRESULT result = IFX_OK;

	ModelDescription* pDescription = &m_pLineSetResource->m_modelDescription;
	const I32& lineCount = m_pLineSetResource->lineCount;

	result = ParseLineSetDescription();

	if( IFXSUCCESS( result ) && pDescription->shadingCount > 0 )
		result = ParseShadingDescriptions();

	if( IFXSUCCESS( result ) && lineCount > 0 )
	{
		// parse line positions
		result = ParseInt2List( IDTF_LINE_POSITION_LIST, 
						lineCount,
						m_pLineSetResource->m_linePositions);
	}

	if( IFXSUCCESS( result ) && lineCount > 0 && pDescription->normalCount > 0 )
	{
		// parse line normals
		result = ParseInt2List( IDTF_LINE_NORMAL_LIST,
								lineCount,
								m_pLineSetResource->m_lineNormals );
	}

	if( IFXSUCCESS( result ) && lineCount > 0 )
	{
		// parse line shaders
		result = ParseIntList( IDTF_LINE_SHADING_LIST,
							   lineCount,
							   m_pLineSetResource->m_lineShaders);
	}

	if( IFXSUCCESS( result ) && lineCount > 0 && pDescription->textureCoordCount > 0 )
	{
		// parse line texture coordinates
		result = ParseLineTextureCoords();
	}

	if( IFXSUCCESS( result ) && lineCount > 0 && 
		pDescription->diffuseColorCount > 0 )
	{
		// parse line vertex diffuse color list
		result = ParseInt2List( IDTF_LINE_DIFFUSE_COLOR_LIST,
								lineCount,
								m_pLineSetResource->m_lineDiffuseColors );
	}

	if( IFXSUCCESS( result ) && lineCount > 0 && pDescription->specularColorCount > 0 )
	{
		// parse line vertex diffuse color list
		result = ParseInt2List( IDTF_LINE_SPECULAR_COLOR_LIST,
								lineCount,
								m_pLineSetResource->m_lineSpecularColors );
	}

	if( IFXSUCCESS( result ) && pDescription->positionCount > 0 )
	{
		// parse positions
		result = ParsePointList( IDTF_MODEL_POSITION_LIST,
								pDescription->positionCount,
								m_pLineSetResource->m_positions );
	}

	if( IFXSUCCESS( result ) && pDescription->normalCount > 0 )
	{
		// parse normals
		result = ParsePointList( IDTF_MODEL_NORMAL_LIST,
								pDescription->normalCount,
								m_pLineSetResource->m_normals );
	}

	if( IFXSUCCESS( result ) && pDescription->diffuseColorCount > 0 )
	{
		// parse vertex colors
		result = ParseColorList( IDTF_MODEL_DIFFUSE_COLOR_LIST,
								pDescription->diffuseColorCount,
								m_pLineSetResource->m_diffuseColors );
	}

	if( IFXSUCCESS( result ) && pDescription->specularColorCount > 0 )
	{
		// parse vertex colors
		result = ParseColorList( IDTF_MODEL_SPECULAR_COLOR_LIST,
								pDescription->specularColorCount,
								m_pLineSetResource->m_specularColors );
	}

	if( IFXSUCCESS( result ) && pDescription->textureCoordCount )
		result = ParseTextureCoords();

	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT LineSetResourceParser::ParseLineSetDescription()
{
	IFXRESULT result = IFX_OK;
	I32 lineCount = 0;
	I32 positionCount = 0;
	I32 normalCount = 0;
	I32 diffuseColorCount = 0;
	I32 specularColorCount = 0;
	I32 textureCoordCount = 0;
	I32 shadingCount = 0;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_LINE_COUNT, &lineCount );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_POSITION_COUNT, &positionCount );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_NORMAL_COUNT, &normalCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_DIFFUSE_COLOR_COUNT, &diffuseColorCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_SPECULAR_COLOR_COUNT, &specularColorCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_TEXTURE_COORD_COUNT, &textureCoordCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_SHADING_COUNT, &shadingCount );
	
	if( IFXSUCCESS( result ) )
	{
		m_pLineSetResource->lineCount = lineCount;
		m_pLineSetResource->m_modelDescription.positionCount = positionCount;
		m_pLineSetResource->m_modelDescription.normalCount = normalCount;
		m_pLineSetResource->m_modelDescription.diffuseColorCount = diffuseColorCount;
		m_pLineSetResource->m_modelDescription.specularColorCount = specularColorCount;
		m_pLineSetResource->m_modelDescription.textureCoordCount = textureCoordCount;
		m_pLineSetResource->m_modelDescription.shadingCount = shadingCount;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT LineSetResourceParser::ParseLineTextureCoords()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_LINE_TEXTURE_COORD_LIST );

	if( IFXSUCCESS( result ) )
		result = ParseStarter(); // 0

	if( IFXSUCCESS( result ) )
	{
		I32 layers = 0;
		Int2 data;
		I32 lineNumber = 0;
		I32 textureLayerNumber = 0;

		// loop through all lines
		for( I32 j = 0; j < m_pLineSetResource->lineCount && IFXSUCCESS( result ); ++j )
		{
			result = m_pScanner->ScanIntegerToken( IDTF_LINE, &lineNumber );

			if( IFXSUCCESS( result ) )
				result = ParseStarter(); // 1

			if( IFXSUCCESS( result ) )
			{
				// we do not check for (meshFaceNumber == j) because there might 
				// be no texture coordinates for this line if shading 
				// description for this line defines 0 texture layers

				const I32 rShadingIndex = m_pLineSetResource->m_lineShaders.GetElementConst( j );
				const ShadingDescription& rShading = 
					m_pLineSetResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
				layers = rShading.m_textureLayerCount;//.GetTextureLayerCount();

				// create new element for line texture coordinates even it has at zero texture layers
				// it will just has 0 texture coordinate elements
				LineTexCoords& lineTexCoords = m_pLineSetResource->m_lineTextureCoords.CreateNewElement();

				I32 i;
				for( i = 0; i < layers && IFXSUCCESS( result ); ++i )
				{
					result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_LAYER, &textureLayerNumber );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanToken( IDTF_LINE_TEX_COORD );

					if( IFXSUCCESS( result ) && textureLayerNumber == i )
					{
						result = m_pScanner->ScanInt2( &data );
						if( IFXSUCCESS( result ) )
						{
							Int2& item = lineTexCoords.m_texCoords.CreateNewElement();
							item = data;
						}
					}
				}
			}
			if( IFXSUCCESS( result ) ) // 1
				result = ParseTerminator();
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator(); // 0

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
