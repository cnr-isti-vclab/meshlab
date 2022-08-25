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
  @file MeshResourceParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "MeshResourceParser.h"
#include "MeshResource.h"
#include "FileScanner.h"
#include "Int3.h"
#include "Point.h"
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

MeshResourceParser::MeshResourceParser( FileScanner* pScanner, 
										MeshResource* pMeshResource )
: ModelResourceParser( pScanner, pMeshResource ),
  m_pMeshResource( pMeshResource )
{
}

MeshResourceParser::~MeshResourceParser()
{
}

IFXRESULT MeshResourceParser::Parse()
{
	IFXRESULT result = IFX_OK;

	ModelDescription* pDescription = &m_pMeshResource->m_modelDescription;
	const I32& faceCount =  m_pMeshResource->faceCount;

	result = ParseMeshDescription();

	if( IFXSUCCESS( result ) && pDescription->shadingCount > 0 )
		result = ParseShadingDescriptions();

	if( IFXSUCCESS( result ) && faceCount > 0 )
	{
		// parse face positions
		result = ParseInt3List( IDTF_MESH_FACE_POSITION_LIST, 
						faceCount,
						m_pMeshResource->m_facePositions );
	}

	if( IFXSUCCESS( result ) && pDescription->normalCount > 0 && faceCount > 0 )
	{
		// parse face normals
		result = ParseInt3List( IDTF_MESH_FACE_NORMAL_LIST,
								faceCount,
								m_pMeshResource->m_faceNormals );
	}

	if( IFXSUCCESS( result ) && faceCount > 0 )
	{
		// parse face shaders
		result = ParseIntList( IDTF_MESH_FACE_SHADING_LIST,
								faceCount,
								m_pMeshResource->m_faceShaders );
	}

	if( IFXSUCCESS( result ) && pDescription->textureCoordCount > 0 && faceCount > 0 )
	{
		// parse face texture coordinates
		result = ParseFaceTextureCoords();
	}

	if( IFXSUCCESS( result ) && faceCount > 0 && pDescription->diffuseColorCount > 0 )
	{
		// parse face vertex diffuse color list
		result = ParseInt3List( IDTF_MESH_FACE_DIFFUSE_COLOR_LIST,
								faceCount,
								m_pMeshResource->m_faceDiffuseColors);
	}

	if( IFXSUCCESS( result ) && faceCount > 0 && pDescription->specularColorCount > 0 )
	{
		// parse face vertex diffuse color list
		result = ParseInt3List( IDTF_MESH_FACE_SPECULAR_COLOR_LIST,
								faceCount,
								m_pMeshResource->m_faceSpecularColors);
	}

	if( IFXSUCCESS( result ) && pDescription->positionCount > 0 )
	{
		// parse positions
		result = ParsePointList( IDTF_MODEL_POSITION_LIST,
								pDescription->positionCount,
								m_pMeshResource->m_positions );
	}

	if( IFXSUCCESS( result ) && pDescription->normalCount > 0 )
	{
		// parse normals
		result = ParsePointList( IDTF_MODEL_NORMAL_LIST,
								pDescription->normalCount,
								m_pMeshResource->m_normals );
	}

	if( IFXSUCCESS( result ) && pDescription->diffuseColorCount > 0 )
	{
		// parse vertex colors
		result = ParseColorList( IDTF_MODEL_DIFFUSE_COLOR_LIST,
								pDescription->diffuseColorCount,
								m_pMeshResource->m_diffuseColors );
	}

	if( IFXSUCCESS( result ) && pDescription->specularColorCount > 0 )
	{
		// parse vertex colors
		result = ParseColorList( IDTF_MODEL_SPECULAR_COLOR_LIST,
								pDescription->specularColorCount,
								m_pMeshResource->m_specularColors );
	}

	if( IFXSUCCESS( result ) && pDescription->textureCoordCount > 0 )
		result = ParseTextureCoords();

	if( IFXSUCCESS( result ) && pDescription->boneCount > 0 )
		result = ParseSkeleton();

	if( IFXSUCCESS( result ) && pDescription->basePositionCount > 0 )
	{
		// parse base positions
		result = ParseIntList( 
						IDTF_MODEL_BASE_POSITION_LIST,
						pDescription->basePositionCount,
						m_pMeshResource->m_basePositions );
	}

	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT MeshResourceParser::ParseMeshDescription()
{
	IFXRESULT result = IFX_OK;
	I32 faceCount = 0;
	I32 positionCount = 0;
	I32 basePositionCount = 0;
	I32 normalCount = 0;
	I32 diffuseColorCount = 0;
	I32 specularColorCount = 0;
	I32 textureCoordCount = 0;
	I32 boneCount = 0;
	I32 shadingCount = 0;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_FACE_COUNT, &faceCount );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_POSITION_COUNT, &positionCount );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_BASE_POSITION_COUNT, &basePositionCount );

	if( IFX_E_TOKEN_NOT_FOUND == result )
	{
		// model does not have base positions so set it to 0
		basePositionCount = 0;
		result = IFX_OK;
	}

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_NORMAL_COUNT, &normalCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_DIFFUSE_COLOR_COUNT, &diffuseColorCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_SPECULAR_COLOR_COUNT, &specularColorCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_TEXTURE_COORD_COUNT, &textureCoordCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_BONE_COUNT, &boneCount );
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MODEL_SHADING_COUNT, &shadingCount );
	
	if( IFXSUCCESS( result ) )
	{
		m_pMeshResource->faceCount = faceCount;
		m_pMeshResource->m_modelDescription.positionCount = positionCount;
		m_pMeshResource->m_modelDescription.basePositionCount = basePositionCount;
		m_pMeshResource->m_modelDescription.normalCount = normalCount;
		m_pMeshResource->m_modelDescription.diffuseColorCount = diffuseColorCount;
		m_pMeshResource->m_modelDescription.specularColorCount = specularColorCount;
		m_pMeshResource->m_modelDescription.textureCoordCount = textureCoordCount;
		m_pMeshResource->m_modelDescription.shadingCount = shadingCount;
		m_pMeshResource->m_modelDescription.boneCount = boneCount;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


IFXRESULT MeshResourceParser::ParseFaceTextureCoords()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_MESH_FACE_TEXTURE_COORD_LIST );

	if( IFXSUCCESS( result ) )
		result = ParseStarter(); // 0

	if( IFXSUCCESS( result ) )
	{
		I32 layers = 0;
		Int3 data;
		I32 meshFaceNumber = 0;
		I32 textureLayerNumber = 0;

		// loop through all faces
		for( I32 j = 0; j < m_pMeshResource->faceCount && IFXSUCCESS( result ); ++j )
		{
			result = m_pScanner->ScanIntegerToken( IDTF_MESH_FACE, &meshFaceNumber );

			if( IFXSUCCESS( result ) )
				result = ParseStarter(); // 1

			if( IFXSUCCESS( result ) )
			{
				// we do not check for (meshFaceNumber == j) because there might 
				// be no texture coordinates for this face if shading 
				// description for this face defines 0 texture layers

				const I32 rShadingIndex = m_pMeshResource->m_faceShaders.GetElementConst( j );
				const ShadingDescription& rShading = 
					m_pMeshResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
				layers = rShading.m_textureLayerCount;//.GetTextureLayerCount();

				// create new element for face texture coordinates even it has zero texture layers
				// it will just has 0 texture coordinate elements
				FaceTexCoords& faceTexCoords = m_pMeshResource->m_faceTextureCoords.CreateNewElement();

				I32 i;
				for( i = 0; i < layers && IFXSUCCESS( result ); ++i )
				{
					result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_LAYER, &textureLayerNumber );

					if( IFXSUCCESS( result ) && textureLayerNumber == i )
					{
						result = m_pScanner->ScanToken( IDTF_MESH_FACE_TEX_COORD );

						if( IFXSUCCESS( result ) )
							result = m_pScanner->ScanInt3( &data );

						if( IFXSUCCESS( result ) )
						{
							// create new element for every texture layer
							Int3& item = faceTexCoords.m_texCoords.CreateNewElement();
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
