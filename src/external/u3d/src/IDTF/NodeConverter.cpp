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
  @file NodeConverter.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "NodeConverter.h"
#include "NodeList.h"
#include "SceneUtilities.h"
#include "MetaDataConverter.h"

#include "LightNode.h"

#include "IFXCheckX.h"

#include "Tokens.h"

#ifndef LIBIDTF
extern FILE *stdmsg;
#endif

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

NodeConverter::NodeConverter( 
					NodeList* pNodeList, 
					SceneUtilities* pSceneUtils ) 
:	m_pNodeList( pNodeList ), 
	m_pSceneUtils( pSceneUtils )
{
	IFXCHECKX(NULL == pNodeList);
	IFXCHECKX(NULL == pSceneUtils);
}

NodeConverter::~NodeConverter()
{
}

IFXRESULT NodeConverter::Convert()
{
	IFXRESULT result = IFX_OK;
	const U32 nodeCount = m_pNodeList->GetNodeCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != nodeCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Nodes (%d)\t\t\t", nodeCount);
#else
	{
		dbgMessage.ToString(nodeCount,10);
		dbgMessage = IFXString(L"[Converter] Nodes (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t\t\t";
	}
#endif

	U32 index;
	for( index = 0; index < nodeCount && IFXSUCCESS( result ); ++index )
	{
		Node* pNode = const_cast<Node*>(m_pNodeList->GetNode( index ));
		const IFXString& type = pNode->GetType();

		if( type == IDTF_MODEL )
			result = ConvertModel( static_cast< const ModelNode* >( pNode ) );
		else if( type == IDTF_LIGHT )
			result = ConvertLight( static_cast< const LightNode* >( pNode ) );
		else if( type == IDTF_VIEW )
			result = ConvertView( static_cast< const ViewNode* >( pNode ) );
		else if( type == IDTF_GROUP )
			result = ConvertGroup( pNode );
		else
		{
			result = IFX_E_UNKNOWN_NODE_TYPE;
			break;
		}

		// convert node's meta-data
		if( IFXSUCCESS( result ) )
		{
			IFXDECLARELOCAL( IFXNode, pIFXNode );
			IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

			result = m_pSceneUtils->FindNode( pNode->GetName(), &pIFXNode );

			if( IFXSUCCESS( result ) )
				result = pIFXNode->QueryInterface( IID_IFXMetaDataX, (void**)&pMetaData );

			if( IFXSUCCESS( result ) )
			{
				MetaDataConverter metaDataConverter( pNode, pMetaData );
				metaDataConverter.Convert();
			}
		}

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != nodeCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************

IFXRESULT NodeConverter::ConvertModel( const ModelNode* pModel )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXModel, pIFXModel );

	const IFXString& rVisibility = pModel->GetVisibility();

	// Create a model node to hold all the information we have gathered, and hook
	// it up to the Model Resource
	if( m_pSceneUtils && IFXSUCCESS( result ) )
		result = m_pSceneUtils->CreateModelNode( pModel->GetName(),
												 pModel->GetResourceName(),
												 &pIFXModel );

	// Convert parent list
	if( IFXSUCCESS(result) )
		result = ConvertParentList( pIFXModel, pModel->GetParentList() );

	if( IFXSUCCESS(result) )
	{
		if( rVisibility == L"FRONT" )
			pIFXModel->SetVisibility( 1 );
		else if( rVisibility == L"BOTH" )
			pIFXModel->SetVisibility( 3 );
		else if( rVisibility == L"NONE" )
			pIFXModel->SetVisibility( 0 );
		else if( rVisibility == L"BACK" )
			pIFXModel->SetVisibility( 2 );
		else // unsupported, set to default
			pIFXModel->SetVisibility( 1 );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeConverter::ConvertLight( const LightNode* pIDTFLight )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXLight, pIFXLight );

	// Create a light node with this name
	result = m_pSceneUtils->CreateLightNode( 
									pIDTFLight->GetName(), 
									pIDTFLight->GetResourceName(),
									&pIFXLight );

	// Convert parent list
	if( IFXSUCCESS(result) )
		result = ConvertParentList( pIFXLight, pIDTFLight->GetParentList() );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeConverter::ConvertView( const ViewNode* pIDTFView )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXView, pIFXView );
	const ViewNodeData& rIDTFViewData = pIDTFView->GetViewData();

	// Create a camera node with this name
	if( m_pSceneUtils )
	{
		result = m_pSceneUtils->CreateViewNode( 
									pIDTFView->GetName(),
									pIDTFView->GetResourceName(),
									&pIFXView );
	}

	// Convert parent list
	if( IFXSUCCESS(result) )
		result = ConvertParentList( pIFXView, pIDTFView->GetParentList() );

	// Convert view clipping
	if( IFXSUCCESS(result) )
	{
		F32 nearClip, farClip;

		rIDTFViewData.GetClipping( &nearClip, &farClip );

		result = pIFXView->SetFarClip( farClip );
	
		if( IFXSUCCESS(result) )
			result = pIFXView->SetNearClip( nearClip );
	}

	// Convert view port
	if( IFXSUCCESS(result) )
	{
		IFXF32Rect rcView;
		rIDTFViewData.GetViewPort( 
							&rcView.m_Width, &rcView.m_Height, 
							&rcView.m_X, &rcView.m_Y );

		result = pIFXView->SetViewport( rcView );
	}

	// Convert backdrops
	if( IFXSUCCESS(result) )
	{
		U32 i;
		U32 count = rIDTFViewData.GetBackdropCount();

		for( i = 0; i < count && IFXSUCCESS( result ); ++i )
		{
			const ViewTexture& rBackdrop = rIDTFViewData.GetBackdrop( i );
			IFXViewLayer layer;
			IFXString textureName( rBackdrop.m_name );

			// Add the texture name to the palette if not found
			result = m_pSceneUtils->AddTexturePaletteEntry( 
										&textureName, 
										&layer.m_uTextureId );

			if( IFXSUCCESS( result ) )
			{
				layer.m_fBlend = rBackdrop.m_blend;
				layer.m_fRotation = rBackdrop.m_rotation;
				layer.m_iLocX = rBackdrop.m_locationX;
				layer.m_iLocY = rBackdrop.m_locationY;
				layer.m_iRegX = rBackdrop.m_regPointX;
				layer.m_iRegY = rBackdrop.m_regPointY;
				layer.m_vScale.X() = rBackdrop.m_scaleX;
				layer.m_vScale.Y() = rBackdrop.m_scaleY;
			}

			IFXRect viewport(0,0,1,1);
			result = pIFXView->AddLayer( 
								IFX_VIEW_BACKDROP, layer, &viewport, 1, 1 );
		}
	}

	// Convert overlays
	if( IFXSUCCESS(result) )
	{
		U32 i;
		U32 count = rIDTFViewData.GetOverlayCount();

		for( i = 0; i < count && IFXSUCCESS( result ); ++i )
		{
			const ViewTexture& rOverlay = rIDTFViewData.GetOverlay( i );
			IFXViewLayer layer;
			IFXString textureName( rOverlay.m_name );

			// Add the texture name to the palette if not found
			result = m_pSceneUtils->AddTexturePaletteEntry( 
										&textureName, 
										&layer.m_uTextureId );

			if( IFXSUCCESS( result ) )
			{
				layer.m_fBlend = rOverlay.m_blend;
				layer.m_fRotation = rOverlay.m_rotation;
				layer.m_iLocX = rOverlay.m_locationX;
				layer.m_iLocY = rOverlay.m_locationY;
				layer.m_iRegX = rOverlay.m_regPointX;
				layer.m_iRegY = rOverlay.m_regPointY;
				layer.m_vScale.X() = rOverlay.m_scaleX;
				layer.m_vScale.Y() = rOverlay.m_scaleY;
			}

			IFXRect viewport(0,0,1,1);
			result = pIFXView->AddLayer( 
								IFX_VIEW_OVERLAY, layer, &viewport, 1, 1 );
		}
	}

	// Set up the camera attributes and its projection
	if( IFXSUCCESS(result) )
	{
		U32 attributes = 0;

		const IFXString& type = rIDTFViewData.GetType();
		const IFXString& unitType = rIDTFViewData.GetUnitType();
		const F32& tFov = rIDTFViewData.GetProjection();

		if( type == IDTF_ORTHO_VIEW )
		{
			attributes |= 2; // ortho P.M.
			result = pIFXView->SetOrthoHeight(tFov);
		}
		else
		{
			// default attributes: 3-point perspective P.M.
			result = pIFXView->SetProjection(tFov);
		}

		if( unitType == IDTF_VIEW_UNIT_PERCENT )
			attributes |= 1;

		if( IFXSUCCESS(result) )
			pIFXView->SetAttributes( attributes );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeConverter::ConvertGroup( const Node* pNode )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXNode, pIFXNode );

	// Create a group node using this name
	result = m_pSceneUtils->CreateGroupNode( 
									pNode->GetName(),			 
									&pIFXNode );

	if( IFXSUCCESS(result) )
		result = ConvertParentList( pIFXNode, pNode->GetParentList() );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT NodeConverter::ConvertParentList( 
								IFXNode* pIFXNode, 
								const ParentList& rParentList )
{
	IFXRESULT result = IFX_OK;
	U32 parentCount = rParentList.GetParentCount();

	// convert node's parents data
	if( pIFXNode )
	{
		U32 i;
		for( i = 0; i < parentCount && IFXSUCCESS( result ); ++i )
		{
			const ParentData& rParentData = rParentList.GetParentData( i );
			const IFXString& rParentName = rParentData.GetParentName();
			IFXDECLARELOCAL( IFXNode, pParentNode );

			// Try to find, by name, the node we claim is its parent.
			result = m_pSceneUtils->FindNode( rParentName, &pParentNode );

			// Set the parent node
			if( IFXSUCCESS( result ) )
				result = pIFXNode->AddParent( pParentNode );

			// Set the group node's local transformation matrix relative to 
			// parent i
			if( IFXSUCCESS( result ) )
			{
				IFXMatrix4x4 pTM( rParentData.GetParentTM() );
				result = pIFXNode->SetMatrix( i, &pTM );
			}
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
