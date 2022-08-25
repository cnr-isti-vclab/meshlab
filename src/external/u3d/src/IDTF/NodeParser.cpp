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
@file NodeParser.cpp

This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "NodeParser.h"
#include "FileScanner.h"
#include "Node.h"
#include "ViewNode.h"
#include "ViewNodeData.h"
#include "LightNode.h"
#include "ModelNode.h"

#include "ParentList.h"
#include "Tokens.h"
#include "IFXMatrix4x4.h"
#include "ParentData.h"
#include "DefaultSettings.h"

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

NodeParser::NodeParser( FileScanner* pScanner, Node* pNode )
: BlockParser( pScanner ), m_pScanner( pScanner ), m_pNode( pNode )
{
	IFXCHECKX_RESULT( NULL != pScanner, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pNode, IFX_E_INVALID_POINTER );
}

NodeParser::~NodeParser()
{
	m_pScanner = NULL;
	m_pNode = NULL;
}

IFXRESULT NodeParser::Parse()
{
	IFXRESULT result = IFX_OK;
	IFXString nodeName;

	result = ParseStarter();
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( IDTF_NODE_NAME, &nodeName );

	if( IFXSUCCESS( result ) )
	{
		ParentList parentList;
		IFXString resourceName;

		// parse parent list
		if( IFXSUCCESS( result ) )
			result = ParseParentList( &parentList );

		// parse the name of node resource
		if( IFXSUCCESS( result ) && m_pNode->GetType() != IDTF_GROUP )
			result = m_pScanner->ScanStringToken( 
									IDTF_RESOURCE_NAME, &resourceName );

		// parse node data
		if( IFXSUCCESS( result ) )
			result = ParseNodeData();

		// if everything is OK
		if( IFXSUCCESS( result ) )
		{
			if( NULL != m_pNode )
			{
				m_pNode->SetName( nodeName );
				m_pNode->SetParentList( parentList );
				m_pNode->SetResourceName( resourceName );
			}
			else
			{
				IFXASSERT( NULL != m_pNode );
				result = IFX_E_INVALID_POINTER;
			}
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( m_pNode );

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT NodeParser::ParseParentList( ParentList* pParentList )
{
	IFXRESULT result = IFX_OK;

	if( NULL == pParentList )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
		result = BlockBegin( IDTF_PARENT_LIST );

	if( IFXSUCCESS( result ) )
	{
		I32 itemCount = 0;
		I32 itemNumber = 0;

		result = m_pScanner->ScanIntegerToken( IDTF_PARENT_COUNT, &itemCount );

		if( IFXSUCCESS( result ) && itemCount > 0 )
		{
			I32 i;
			for( i = 0; i < itemCount && IFXSUCCESS( result ); ++i )
			{
				result = m_pScanner->ScanIntegerToken( IDTF_PARENT, &itemNumber );

				if( IFXSUCCESS( result ) && i == itemNumber )
				{
					ParentData parentData;
					result = ParseParentData( &parentData );

					if( IFXSUCCESS( result ) )
						pParentList->AddParentData( parentData );
				}
			}
		}
		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeParser::ParseParentData( ParentData* pParentData )
{
	IFXRESULT result = IFX_OK;
	IFXString parentName;

	if( NULL == pParentData )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
		result = ParseStarter();

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( IDTF_PARENT_NAME, &parentName );

	if( IFXSUCCESS( result ) )
	{
		IFXMatrix4x4 parentTM;
		result = m_pScanner->ScanTMToken( IDTF_PARENT_TM, &parentTM );

		if( IFXSUCCESS( result ) )
		{
			pParentData->SetParentName( parentName );
			pParentData->SetParentTM( parentTM );
		}
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeParser::ParseNodeData()
{
	IFXRESULT result = IFX_OK;

	const IFXString& nodeType = m_pNode->GetType();

	if( nodeType == IDTF_VIEW )
		result = ParseViewNodeData();
	else if( nodeType == IDTF_MODEL )
		result = ParseModelNodeData();
	else if( nodeType == IDTF_GROUP || nodeType == IDTF_LIGHT )
	{
		// nothing to parse
	}
	else
		result = IFX_E_UNDEFINED;

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeParser::ParseViewTexture( ViewTexture* pViewTexture )
{
	IFXRESULT result = IFX_OK;
	IFXString textureName;
	F32 textureBlend;
	F32 rotation, location_x, location_y;
	I32 reg_point_x, reg_point_y;
	F32 scale_x, scale_y;

	result = m_pScanner->ScanStringToken( IDTF_TEXTURE_NAME, &textureName );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( 
									IDTF_TEXTURE_BLEND, &textureBlend );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_ROTATION, &rotation );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_LOCATION_X, &location_x );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_LOCATION_Y, &location_y );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( 
									IDTF_REG_POINT_X, &reg_point_x );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( 
									IDTF_REG_POINT_Y, &reg_point_y );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_SCALE_X, &scale_x );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_SCALE_Y, &scale_y );

	if( IFXSUCCESS( result ) )
	{
		pViewTexture->m_name = textureName;
		pViewTexture->m_blend = textureBlend;
		pViewTexture->m_rotation = rotation;
		pViewTexture->m_locationX = location_x;
		pViewTexture->m_locationY = location_y;
		pViewTexture->m_regPointX = reg_point_x;
		pViewTexture->m_regPointY = reg_point_y;
		pViewTexture->m_scaleX = scale_x;
		pViewTexture->m_scaleY = scale_y;
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeParser::ParseViewNodeData()
{
	IFXRESULT result = IFX_OK;
	I32 i;

	result = BlockBegin( IDTF_VIEW_DATA );

	if( IFXSUCCESS( result ) )
	{
		ViewNodeData viewData;

		IFXString type;
		IFXString unit;
		F32 nearClip, farClip;
		F32 projection;
		F32 width, height, horPos, vertPos;

		result = m_pScanner->ScanStringToken( IDTF_VIEW_ATTRIBUTE_SCREEN_UNIT, &unit );

		if( IFX_E_TOKEN_NOT_FOUND == result )
		{
			unit = IDTF_VIEW_UNIT_PIXEL; //set default
			result = IFX_OK;
		}

		if( IFXSUCCESS( result ) )
			result = m_pScanner->ScanStringToken( IDTF_VIEW_TYPE, &type );

		if( IFX_E_TOKEN_NOT_FOUND == result )
		{
			type = IDTF_PERSPECTIVE_VIEW; //set default
			result = IFX_OK;
		}

		if( IFXSUCCESS( result ) )
		{
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_NEAR_CLIP, &nearClip );
			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				nearClip = VIEW_NEAR_CLIP; // set default
				result = IFX_OK;
			}
		}

		if( IFXSUCCESS( result ) )
		{
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_FAR_CLIP, &farClip );
			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				farClip = VIEW_FAR_CLIP; // set default
				result = IFX_OK;
			}
		}

		if( IFXSUCCESS( result ) )
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_PROJECTION, &projection );

		// parse view port
		if( IFXSUCCESS( result ) )
		{
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_PORT_WIDTH, &width );
			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				width = VIEW_PORT_WIDTH; //set default
				result = IFX_OK;
			}
		}

		if( IFXSUCCESS( result ) )
		{
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_PORT_HEIGHT, &height );
			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				height = VIEW_PORT_HEIGHT; //set default
				result = IFX_OK;
			}
		}

		if( IFXSUCCESS( result ) )
		{
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_PORT_H_POSITION, &horPos );
			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				horPos = VIEW_PORT_H_POSITION;
				result = IFX_OK;
			}
		}

		if( IFXSUCCESS( result ) )
		{
			result = m_pScanner->ScanFloatToken( IDTF_VIEW_PORT_V_POSITION, &vertPos );
			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				vertPos = VIEW_PORT_V_POSITION;
				result = IFX_OK;
			}
		}

		// parse backdrops
		if( IFXSUCCESS( result ) )
		{
			I32 count = 0;

			result = m_pScanner->ScanIntegerToken( 
									IDTF_BACKDROP_COUNT, &count );

			if( IFXSUCCESS( result ) && count > 0 )
			{
				result = BlockBegin( IDTF_BACKDROP_LIST );

				for( i = 0; i < count && IFXSUCCESS( result ); ++i )
				{
					I32 number = 0;
					ViewTexture backdrop;

					result = BlockBegin( IDTF_BACKDROP, &number );

					if( IFXSUCCESS( result ) )
						result = ParseViewTexture( &backdrop );
			
					if( IFXSUCCESS( result ) )
						result = BlockEnd();

					if( IFXSUCCESS( result ) )
						viewData.AddBackdrop( backdrop );
				}

				if( IFXSUCCESS( result ) )
					result = BlockEnd();
			}

			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				result = IFX_OK;
			}
		}

		// parse overlays
		if( IFXSUCCESS( result ) )
		{
			I32 count = 0;

			result = m_pScanner->ScanIntegerToken( 
									IDTF_OVERLAY_COUNT, &count );

			if( IFXSUCCESS( result ) && count > 0 )
			{
				result = BlockBegin( IDTF_OVERLAY_LIST );

				for( i = 0; i < count && IFXSUCCESS( result ); ++i )
				{
					I32 number = 0;
					ViewTexture overlay;

					result = BlockBegin( IDTF_OVERLAY, &number );

					if( IFXSUCCESS( result ) )
						result = ParseViewTexture( &overlay );
			
					if( IFXSUCCESS( result ) )
						result = BlockEnd();

					if( IFXSUCCESS( result ) )
						viewData.AddOverlay( overlay );
				}

				if( IFXSUCCESS( result ) )
					result = BlockEnd();
			}

			if( IFX_E_TOKEN_NOT_FOUND == result )
			{
				result = IFX_OK;
			}
		}

		if( IFXSUCCESS( result ) )
		{
			viewData.SetUnitType( unit );
			viewData.SetType( type );
			viewData.SetClipping( nearClip, farClip );
			viewData.SetProjection( projection );
			viewData.SetViewPort( width, height, horPos, vertPos );

			ViewNode* pNode = static_cast< ViewNode* >( m_pNode );
			pNode->SetViewData( viewData );
		}
	}

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeParser::ParseModelNodeData()
{
	IFXRESULT result = IFX_OK;
	IFXString visibility;

	m_pScanner->ScanStringToken( IDTF_MODEL_VISIBILITY, &visibility );

	if( IFXSUCCESS( result ) )
	{
		ModelNode* pNode = static_cast< ModelNode* >( m_pNode );
		pNode->SetVisibility( visibility );
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
