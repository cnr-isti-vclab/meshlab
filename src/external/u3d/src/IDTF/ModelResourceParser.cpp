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
  @file ModelResourceParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ModelResourceParser.h"
#include "ModelResource.h"
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

ModelResourceParser::ModelResourceParser( FileScanner* pScanner, 
										  ModelResource* pModelResource )
: BlockParser( pScanner ), 
  m_pScanner( pScanner ), 
  m_pModelResource( pModelResource )
{
}

ModelResourceParser::~ModelResourceParser()
{
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT ModelResourceParser::ParseShadingDescriptions()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_MODEL_SHADING_DESCRIPTION_LIST );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
	{
		I32 shadingDescriptionNumber = 0;
		I32 textureLayerCount = 0;
		I32 textureLayerNumber = 0;
		I32 shaderId = 0;

		I32 i;
		for( i = 0; i < m_pModelResource->m_modelDescription.shadingCount; ++i )
		{
			ShadingDescription shadingDescription;
			result = m_pScanner->ScanIntegerToken( IDTF_SHADING_DESCRIPTION, 
													&shadingDescriptionNumber );

			if( IFXSUCCESS( result ) )
				result = ParseStarter();

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_LAYER_COUNT, &textureLayerCount );

			if( IFXSUCCESS( result ) && textureLayerCount > 0 )
			{
				I32 textureLayerDimension = 0;

				result = m_pScanner->ScanToken( IDTF_TEXTURE_COORD_DIMENSION_LIST );

				if( IFXSUCCESS( result ) )
					result = ParseStarter();

				for( I32 j = 0; j < textureLayerCount && IFXSUCCESS( result ); ++j )
				{
					result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_LAYER, &textureLayerNumber );

					if( IFXSUCCESS( result ) && textureLayerNumber == j )
						result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_LAYER_DIMENSION, &textureLayerDimension );
		
					if( IFXSUCCESS( result ) )
						shadingDescription.AddTextureCoordDimension( textureLayerDimension );
				}

				if( IFXSUCCESS( result ) )
					result = ParseTerminator();
			}

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanIntegerToken( IDTF_SHADER_ID, &shaderId );

			if( IFXSUCCESS( result ) )
			{
				shadingDescription.m_textureLayerCount = textureLayerCount;
				shadingDescription.m_shaderId = shaderId;

				m_pModelResource->m_shadingDescriptions.AddShadingDescription( shadingDescription );
			}
			if( IFXSUCCESS( result ) )
				result = ParseTerminator();
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParseInt2List( const IFXCHAR* pToken, const I32 count, 
											  IFXArray< Int2 >& rList)
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( pToken );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
	{
		Int2 data;

		I32 i;
		for( i = 0; i < count; ++i )
		{
			result = m_pScanner->ScanInt2( &data );
			if( IFXSUCCESS( result ) )
			{
				Int2& item = rList.CreateNewElement();
				item = data;
			}
			else
				break;
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParseInt3List( const IFXCHAR* pToken, const I32 count, 
											  IFXArray< Int3 >& rList)
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( pToken );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
	{
		Int3 data;

		I32 i;
		for( i = 0; i < count; ++i )
		{
			result = m_pScanner->ScanInt3( &data );
			if( IFXSUCCESS( result ) )
			{
				Int3& item = rList.CreateNewElement();
				item = data;
			}
			else
				break;
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParsePointList( const IFXCHAR* pToken, const I32 count, 
											  IFXArray< Point >& rList)
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( pToken );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
	{
		Point data;

		I32 i;
		for( i = 0; i < count; ++i )
		{
			result = m_pScanner->ScanPoint( &data );
			if( IFXSUCCESS( result ) )
			{
				Point& item = rList.CreateNewElement();
				item = data;
			}
			else
				break;
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParseColorList( const IFXCHAR* pToken, const I32 count, 
											  IFXArray< Color >& rList)
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( pToken );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
	{
		Color data;

		I32 i;
		for( i = 0; i < count; ++i )
		{
			result = m_pScanner->ScanColor( &data );
			if( IFXSUCCESS( result ) )
			{
				Color& item = rList.CreateNewElement();
				item = data;
			}
			else
				break;
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParseIntList( const IFXCHAR* pToken, const I32 count, 
											 IFXArray< I32 >& rList)
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( pToken );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
	{
		I32 data;

		I32 i;
		for( i = 0; i < count; ++i )
		{
			result = m_pScanner->ScanInteger( &data );
			if( IFXSUCCESS( result ) )
			{
				I32& item = rList.CreateNewElement();
				item = data;
			}
			else
				break;
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParseTextureCoords()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_MODEL_TEXTURE_COORD_LIST );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	I32 i;
	for( i = 0; 
		 i < m_pModelResource->m_modelDescription.textureCoordCount && 
		 IFXSUCCESS( result );
		 ++i )
	{
		IFXVector4 data;

		result = m_pScanner->ScanVector4( &data );
		if( IFXSUCCESS( result ) )
		{
			IFXVector4& item = m_pModelResource->m_textureCoords.CreateNewElement();
			item = data;
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModelResourceParser::ParseSkeleton()
{
	IFXRESULT result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_MODEL_SKELETON );

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	I32 i;
	for( i = 0; 
		 i < m_pModelResource->m_modelDescription.boneCount 
			 && IFXSUCCESS( result ); 
		 ++i )
	{
		BoneInfo boneInfo;
		I32 boneNumber = -1;

		result = m_pScanner->ScanIntegerToken( IDTF_BONE, &boneNumber );

		if( IFXSUCCESS( result ) && i == boneNumber )
		{
			result = ParseStarter();

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanStringToken( IDTF_BONE_NAME, &boneInfo.name );

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanStringToken( 
											IDTF_PARENT_BONE_NAME, 
											&boneInfo.parentName );

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanFloatToken( IDTF_BONE_LENGTH, &boneInfo.length );

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanPointToken( 
											IDTF_BONE_DISPLACEMENT, 
											&boneInfo.displacement );

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanQuatToken( 
											IDTF_BONE_ORIENTATION, 
											&boneInfo.orientation );

			if( IFXSUCCESS( result ) )
				result = ParseTerminator();

			if( IFXSUCCESS( result ) )
				m_pModelResource->m_modelSkeleton.AddBoneInfo( boneInfo );
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

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
