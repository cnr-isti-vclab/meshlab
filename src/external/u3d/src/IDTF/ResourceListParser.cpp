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
@file ResourceListParser.cpp

This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ResourceListParser.h"
#include "ViewResourceList.h"
#include "LightResourceList.h"
#include "ModelResourceList.h"
#include "ShaderResourceList.h"
#include "MaterialResourceList.h"
#include "TextureResourceList.h"
#include "ShadingDescriptionList.h"
#include "MotionResourceList.h"
#include "ShaderParser.h"
#include "MaterialParser.h"
#include "TextureParser.h"
#include "MeshResourceParser.h"
#include "LineSetResourceParser.h"
#include "PointSetResourceParser.h"
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

ResourceListParser::ResourceListParser( 
							FileScanner* pScanner, 
							ResourceList* pResourceList )
: BlockParser( pScanner ), 
  m_pScanner( pScanner ), 
  m_pResourceList( pResourceList )
{
	IFXCHECKX_RESULT( NULL != pScanner, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pResourceList, IFX_E_INVALID_POINTER );
}

ResourceListParser::~ResourceListParser()
{
	m_pScanner = NULL;
	m_pResourceList = NULL;
}

IFXRESULT ResourceListParser::Parse()
{
	IFXRESULT result = IFX_OK;
	IFXRESULT (ResourceListParser::*ParseResource)() = NULL;

	const IFXString& type = m_pResourceList->GetType();

	if( type == IDTF_LIGHT )
		ParseResource = &ResourceListParser::ParseLightResource;
	else if( type == IDTF_VIEW )
		ParseResource = &ResourceListParser::ParseViewResource;
	else if( type == IDTF_MODEL )
		ParseResource = &ResourceListParser::ParseModelResource;
	else if( type == IDTF_SHADER )
		ParseResource = &ResourceListParser::ParseShaderResource;
	else if( type == IDTF_MATERIAL )
		ParseResource = &ResourceListParser::ParseMaterialResource;
	else if( type == IDTF_TEXTURE )
		ParseResource = &ResourceListParser::ParseTextureResource;
	else if( type == IDTF_MOTION )
		ParseResource = &ResourceListParser::ParseMotionResource;
	else
		result = IFX_E_UNSUPPORTED;

	if( IFXSUCCESS( result ) )
		result = ParseResourceList( ParseResource );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT ResourceListParser::ParseResourceList( 
	IFXRESULT (ResourceListParser::*ParseResource)() )
{
	IFXRESULT result = IFX_OK;
	I32 resourceCount = 0;

	// block starter for resource list
	result = m_pScanner->FindBlockStarter();
		
	if( IFXSUCCESS( result ) )
		m_pScanner->ScanIntegerToken( IDTF_RESOURCE_COUNT, &resourceCount );

	if( NULL != ParseResource && 0 < resourceCount )
	{
		I32 resourceNumber = -1;

		I32 i;
		for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
		{
			result = BlockBegin( IDTF_RESOURCE, &resourceNumber );

			if( IFXSUCCESS( result ) && resourceNumber == i )
				result = m_pScanner->ScanStringToken( 
										IDTF_RESOURCE_NAME, &m_resourceName );

			if( IFXSUCCESS( result ) )
				result = (this->*ParseResource)();

			if( IFXSUCCESS( result ) )
				result = BlockEnd();
		}
	}

	if( IFXSUCCESS( result ) )
		result = m_pScanner->FindBlockTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseLightResource()
{
	IFXRESULT result = IFX_OK;
	LightResource lightResource;
	IFXString type;
	Color color;
	Point attenuation;
	F32 intensity = 0.0f;
	F32 spotAngle = 0.0f;

	result = m_pScanner->ScanStringToken( IDTF_LIGHT_TYPE, &type );

	if( IFXSUCCESS( result ) ) 
		if ( type != IDTF_POINT_LIGHT && type != IDTF_SPOT_LIGHT &&
			type != IDTF_AMBIENT_LIGHT && type != IDTF_DIRECTIONAL_LIGHT ) 
			result = IFX_E_UNKNOWN_LIGHT_TYPE;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanColorToken( IDTF_LIGHT_COLOR, &color );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanPointToken( 
									IDTF_LIGHT_ATTENUATION, &attenuation );

	if( IFXSUCCESS( result ) && type == IDTF_SPOT_LIGHT )
		result = m_pScanner->ScanFloatToken( 
									IDTF_LIGHT_SPOT_ANGLE, &spotAngle );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( 
									IDTF_LIGHT_INTENSITY, &intensity );

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( &lightResource );

	if( IFXSUCCESS( result ) )
	{
		lightResource.m_type = type;
		lightResource.m_attenuation = attenuation;
		lightResource.m_color = color;
		lightResource.m_intensity = intensity;
		lightResource.m_spotAngle = spotAngle;
		lightResource.SetName( m_resourceName );
		LightResourceList* pResourceList = 
			static_cast< LightResourceList* >( m_pResourceList );
		pResourceList->AddResource( lightResource );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseViewResource()
{
	IFXRESULT result = IFX_OK;
	ViewResource viewResource;
	I32 passCount = 0;

	result = m_pScanner->ScanIntegerToken( IDTF_VIEW_PASS_COUNT, &passCount );

	// parse view passes
	if( IFXSUCCESS( result) && 0 != passCount )
	{
		result = m_pScanner->ScanToken( IDTF_VIEW_ROOT_NODE_LIST );
		if( IFXSUCCESS( result ) )
		{
			IFXString rootNodeName;
			I32 rootNodeNumber;

			result = ParseStarter();

			I32 i;
			for( i = 0; i < passCount && IFXSUCCESS( result ); ++i )
			{
				result = m_pScanner->ScanIntegerToken( 
										IDTF_ROOT_NODE, &rootNodeNumber );
				if( IFXSUCCESS( result ) && rootNodeNumber == i )
				{
					result = ParseStarter();

					if( IFXSUCCESS( result) )
						result = m_pScanner->ScanStringToken( 
										IDTF_ROOT_NODE_NAME, &rootNodeName );

					if( IFXSUCCESS( result) )
					{
						if( rootNodeName == L"<NULL>" )
							rootNodeName.Assign( L"" ); // world group

						viewResource.AddRootNode( rootNodeName );
					}

					if( IFXSUCCESS( result) )
						result = ParseTerminator();
				}
			}
			if( IFXSUCCESS( result) )
				result = ParseTerminator();
		}
	}

	if( IFXSUCCESS( result ) )
	{
		Color clearColor;

		result = m_pScanner->ScanColorToken( 
								L"VIEW_CLEAR_COLOR", &clearColor );

		if( IFX_E_TOKEN_NOT_FOUND == result )
			result = IFX_OK;
	}

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( &viewResource );

	if( IFXSUCCESS( result ) )
	{
		viewResource.SetName( m_resourceName );

		ViewResourceList* pResourceList = 
			static_cast< ViewResourceList* >( m_pResourceList );
		pResourceList->AddResource( viewResource );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseModelResource()
{
	IFXRESULT result = IFX_OK;
	IFXString type;

	result = m_pScanner->ScanStringToken( IDTF_MODEL_TYPE, &type );

	if( IFXSUCCESS( result ) )
	{
		if( type == IDTF_MESH )
			result = ParseMeshResource();
		else if( type == IDTF_POINT_SET )
			result = ParsePointSetResource();
		else if( type == IDTF_LINE_SET ) 
			result = ParseLineSetResource();
		else
			result = IFX_E_UNKNOWN_MODEL_TYPE;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseMeshResource()
{
	IFXRESULT result = IFX_OK;
	MeshResource meshResource;
	meshResource.m_type = IDTF_MESH;

	result = BlockBegin( IDTF_MESH );

	if( IFXSUCCESS( result ) )
	{
		MeshResourceParser parser( m_pScanner, &meshResource );
		result = parser.Parse();
	}

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( &meshResource );

	if( IFXSUCCESS( result ) )
	{
		meshResource.SetName( m_resourceName );
		ModelResourceList* pResourceList = 
			static_cast< ModelResourceList* >( m_pResourceList );
		const ModelResource* modelResource = &meshResource;
		pResourceList->AddResource( modelResource );		
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParsePointSetResource()
{
	IFXRESULT result = IFX_OK;
	PointSetResource pointSetResource;
	pointSetResource.m_type = IDTF_POINT_SET;

	result = BlockBegin( IDTF_POINT_SET );

	if( IFXSUCCESS( result ) )
	{
		PointSetResourceParser parser( m_pScanner, &pointSetResource );
		result = parser.Parse();
	}

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( &pointSetResource );

	if( IFXSUCCESS( result ) )
	{
		pointSetResource.SetName( m_resourceName );
		ModelResourceList* pResourceList = 
			static_cast< ModelResourceList* >( m_pResourceList );
		const ModelResource* modelResource = &pointSetResource;
		pResourceList->AddResource( modelResource );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseLineSetResource()
{
	IFXRESULT result = IFX_OK;
	LineSetResource lineSetResource;
	lineSetResource.m_type = IDTF_LINE_SET;

	result = BlockBegin( IDTF_LINE_SET );

	if( IFXSUCCESS( result ) )
	{
		LineSetResourceParser parser( m_pScanner, &lineSetResource );
		result = parser.Parse();
	}

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( &lineSetResource );

	if( IFXSUCCESS( result ) )
	{
		lineSetResource.SetName( m_resourceName );
		ModelResourceList* pResourceList = 
			static_cast< ModelResourceList* >( m_pResourceList );
		const ModelResource* modelResource = &lineSetResource;
		pResourceList->AddResource( modelResource );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseShaderResource()
{
	IFXRESULT result = IFX_OK;
	Shader shader;
	ShaderParser parser( m_pScanner, &shader );

	result = parser.Parse();

	if( IFXSUCCESS( result ) )
	{
		shader.SetName( m_resourceName );
		ShaderResourceList* pResourceList = 
			static_cast< ShaderResourceList* >( m_pResourceList );
		pResourceList->AddResource( shader );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseMaterialResource()
{
	IFXRESULT result = IFX_OK;
	Material material;
	MaterialParser parser( m_pScanner, &material );

	result = parser.Parse();

	if( IFXSUCCESS( result ) )
	{
		material.SetName( m_resourceName );
		MaterialResourceList* pResourceList = 
			static_cast< MaterialResourceList* >( m_pResourceList );
		pResourceList->AddResource( material );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseTextureResource()
{
	IFXRESULT result = IFX_OK;
	Texture texture;
	TextureParser parser( m_pScanner, &texture );

	result = parser.Parse();

	if( IFXSUCCESS( result ) )
	{
		texture.SetName( m_resourceName );
		TextureResourceList* pResourceList = 
			static_cast< TextureResourceList* >( m_pResourceList );
		pResourceList->AddResource( texture );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ResourceListParser::ParseMotionResource()
{
	IFXRESULT result = IFX_OK;

	MotionResource motion;
	I32 motionTrackCount = 0;

	result = m_pScanner->ScanIntegerToken( 
							IDTF_MOTION_TRACK_COUNT, &motionTrackCount );

	if( IFXSUCCESS( result ) )
	{
		result = BlockBegin( IDTF_MOTION_TRACK_LIST );

		I32 i;
		for( i = 0; i < motionTrackCount && IFXSUCCESS( result ); ++i )
		{
			MotionTrack track;
			I32 motionTrackNumber;
			I32 sampleCount = 0;

			result = BlockBegin( IDTF_MOTION_TRACK, &motionTrackNumber );

			if( IFXSUCCESS( result ) && motionTrackNumber == i )
			{
				result = m_pScanner->ScanStringToken( 
									IDTF_MOTION_TRACK_NAME, &track.m_name );

				if( IFXSUCCESS( result ) )
					result = m_pScanner->ScanIntegerToken( 
									IDTF_MOTION_TRACK_SAMPLE_COUNT,  
									&sampleCount );

				if( IFXSUCCESS( result ) )
				{
					I32 keyFrameNumber;
					KeyFrame keyFrame;

					result = BlockBegin( IDTF_KEY_FRAME_LIST );

					I32 j;
					for( j = 0;  j < sampleCount && IFXSUCCESS( result ); ++j )
					{
						result = BlockBegin( IDTF_KEY_FRAME, &keyFrameNumber );

						if( IFXSUCCESS( result ) && keyFrameNumber == j )
						{
							result = m_pScanner->ScanFloatToken( 
												IDTF_KEY_FRAME_TIME, 
												&keyFrame.m_time );

							if( IFXSUCCESS( result ) )
								result = m_pScanner->ScanPointToken( 
												IDTF_KEY_FRAME_DISPLACEMENT,
												&keyFrame.m_displacement );

							if( IFXSUCCESS( result ) )
								result = m_pScanner->ScanQuatToken( 
												IDTF_KEY_FRAME_ROTATION, 
												&keyFrame.m_rotation );

							if( IFXSUCCESS( result ) )
								result = m_pScanner->ScanPointToken( 
												IDTF_KEY_FRAME_SCALE, 
												&keyFrame.m_scale );

							if( IFXSUCCESS( result ) )
								track.AddKeyFrame( keyFrame );

							if( IFXSUCCESS( result ) )
								result = BlockEnd();
						}
						else
							break;
					}
					if( IFXSUCCESS( result ) )
						result = BlockEnd();
				}
				if( IFXSUCCESS( result ) )
				{
					motion.AddMotionTrack( track );
					result = BlockEnd();
				}
			}
			else
				break;
		}

		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( &motion );

	if( IFXSUCCESS( result ) )
	{
		motion.SetName( m_resourceName );
		MotionResourceList* pResourceList = 
			static_cast< MotionResourceList* >( m_pResourceList );
		pResourceList->AddResource( motion );
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
