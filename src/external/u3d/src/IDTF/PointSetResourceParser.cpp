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
	@file PointSetResourceParser.cpp

	This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "PointSetResourceParser.h"
#include "PointSetResource.h"
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

PointSetResourceParser::PointSetResourceParser( 
									FileScanner* pScanner, 
									PointSetResource* pPointSetResource )
: ModelResourceParser( pScanner, pPointSetResource ), 
  m_pPointSetResource( pPointSetResource )
{
}

PointSetResourceParser::~PointSetResourceParser()
{
}

IFXRESULT PointSetResourceParser::Parse()
{
	IFXRESULT result = IFX_OK;

	ModelDescription* pDescription = &m_pPointSetResource->m_modelDescription;
	const I32& pointCount = m_pPointSetResource->pointCount;

	result = ParsePointSetDescription();

	if( IFXSUCCESS( result ) && pDescription->shadingCount > 0 )
		result = ParseShadingDescriptions();

	if( IFXSUCCESS( result ) && pointCount > 0 )
	{
		// parse point positions
		result = ParseIntList( IDTF_POINT_POSITION_LIST, 
						pointCount,
						m_pPointSetResource->m_pointPositions);
	}

	if( IFXSUCCESS( result ) && pointCount > 0 && pDescription->normalCount > 0 )
	{
		// parse point normals
		result = ParseIntList( IDTF_POINT_NORMAL_LIST,
								pointCount,
								m_pPointSetResource->m_pointNormals );
	}

	if( IFXSUCCESS( result ) && pointCount > 0 )
	{
		// parse point shaders
		result = ParseIntList( IDTF_POINT_SHADING_LIST,
							   pointCount,
							   m_pPointSetResource->m_pointShaders);
	}

	if( IFXSUCCESS( result ) && pointCount > 0 && 
		pDescription->textureCoordCount > 0 )
	{
		// parse point texture coordinates
		result = ParsePointTextureCoords();
	}

	if( IFXSUCCESS( result ) && pointCount > 0 && 
		pDescription->diffuseColorCount > 0 )
	{
		// parse point vertex diffuse color list
		result = ParseIntList( IDTF_POINT_DIFFUSE_COLOR_LIST,
								pointCount,
								m_pPointSetResource->m_pointDiffuseColors );
	}

	if( IFXSUCCESS( result ) && pointCount > 0 &&
		pDescription->specularColorCount > 0 )
	{
		// parse point vertex diffuse color list
		result = ParseIntList( IDTF_POINT_SPECULAR_COLOR_LIST,
								pointCount,
								m_pPointSetResource->m_pointSpecularColors );
	}

	if( IFXSUCCESS( result ) && pDescription->positionCount > 0 )
	{
		// parse positions
		result = ParsePointList( IDTF_MODEL_POSITION_LIST,
								pDescription->positionCount,
								m_pPointSetResource->m_positions );
	}

	if( IFXSUCCESS( result ) && pDescription->normalCount > 0 )
	{
		// parse normals
		result = ParsePointList( IDTF_MODEL_NORMAL_LIST,
								pDescription->normalCount,
								m_pPointSetResource->m_normals );
	}

	if( IFXSUCCESS( result ) && pDescription->diffuseColorCount > 0 )
	{
		// parse vertex colors
		result = ParseColorList( IDTF_MODEL_DIFFUSE_COLOR_LIST,
								pDescription->diffuseColorCount,
								m_pPointSetResource->m_diffuseColors );
	}

	if( IFXSUCCESS( result ) && pDescription->specularColorCount > 0 )
	{
		// parse vertex colors
		result = ParseColorList( IDTF_MODEL_SPECULAR_COLOR_LIST,
								pDescription->specularColorCount,
								m_pPointSetResource->m_specularColors );
	}

	if( IFXSUCCESS( result ) && pDescription->textureCoordCount )
		result = ParseTextureCoords();

	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT PointSetResourceParser::ParsePointSetDescription()
{
	IFXRESULT result = IFX_OK;
	I32 pointCount = 0;
	I32 positionCount = 0;
	I32 normalCount = 0;
	I32 diffuseColorCount = 0;
	I32 specularColorCount = 0;
	I32 textureCoordCount = 0;
	I32 shadingCount = 0;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_POINT_COUNT, &pointCount );

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
		m_pPointSetResource->pointCount = pointCount;
		m_pPointSetResource->m_modelDescription.positionCount = positionCount;
		m_pPointSetResource->m_modelDescription.normalCount = normalCount;
		m_pPointSetResource->m_modelDescription.diffuseColorCount = diffuseColorCount;
		m_pPointSetResource->m_modelDescription.specularColorCount = specularColorCount;
		m_pPointSetResource->m_modelDescription.textureCoordCount = textureCoordCount;
		m_pPointSetResource->m_modelDescription.shadingCount = shadingCount;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT PointSetResourceParser::ParsePointTextureCoords()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_POINT_TEXTURE_COORD_LIST );

	if( IFXSUCCESS( result ) )
		result = ParseStarter(); // 0

	if( IFXSUCCESS( result ) )
	{
		I32 layers = 0;
		I32 data;
		I32 pointNumber = 0;
		I32 textureLayerNumber = 0;

		// loop through all points
		for( I32 j = 0; j < m_pPointSetResource->pointCount && IFXSUCCESS( result ); ++j )
		{
			result = m_pScanner->ScanIntegerToken( IDTF_POINT, &pointNumber );

			if( IFXSUCCESS( result ) )
				result = ParseStarter(); // 1

			if( IFXSUCCESS( result ) )
			{
				// we do not check for (meshFaceNumber == j) because there might 
				// be no texture coordinates for this point if shading 
				// description for this point defines 0 texture layers

				const I32 rShadingIndex = m_pPointSetResource->m_pointShaders.GetElementConst( j );
				const ShadingDescription& rShading = 
					m_pPointSetResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
				layers = rShading.m_textureLayerCount;//.GetTextureLayerCount();

				// create new element for point texture coordinates even it has at zero texture layers
				// it will just has 0 texture coordinate elements
				PointTexCoords& pointTexCoords = m_pPointSetResource->m_pointTextureCoords.CreateNewElement();

				I32 i;
				for( i = 0; i < layers && IFXSUCCESS( result ); ++i )
				{
					result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_LAYER, &textureLayerNumber );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanToken( IDTF_POINT_TEX_COORD );

					if( IFXSUCCESS( result ) && textureLayerNumber == i )
					{
						result = m_pScanner->ScanInteger( &data );
						if( IFXSUCCESS( result ) )
						{
							I32& item = pointTexCoords.m_texCoords.CreateNewElement();
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
