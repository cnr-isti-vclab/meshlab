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
  @file FileParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Includes
//***************************************************************************

#include "FileParser.h"
#include "FileScanner.h"
#include "Tokens.h"
#include "SceneData.h"
#include "IFXVector4.h"

#include "NodeList.h"
#include "Node.h"
#include "LightNode.h"
#include "ViewNode.h"
#include "ModelNode.h"

#include "ModifierList.h"
#include "Modifier.h"
#include "ShadingModifier.h"
#include "AnimationModifier.h"
#include "BoneWeightModifier.h"
#include "CLODModifier.h"
#include "SubdivisionModifier.h"
#include "GlyphModifier.h"

#include "SceneResources.h"

#include "NodeParser.h"
#include "ResourceListParser.h"
#include "ModifierParser.h"
#include "MetaDataParser.h"
#include "FileReferenceParser.h"

#include "IFXCheckX.h"

using namespace U3D_IDTF;

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

namespace U3D_IDTF
{
Node* MakeNode( const IFXString& rNodeType );
Modifier* MakeModifier( const IFXString& rModifierType );
}

//***************************************************************************
//  Public methods
//***************************************************************************

FileParser::FileParser()
: BlockParser( &m_scanner )
{
}

FileParser::~FileParser()
{
}

IFXRESULT FileParser::Initialize( const IFXCHAR* pFileName )
{
	return m_scanner.Initialize( pFileName );
}

IFXRESULT FileParser::ParseFileHeader( IFXString* pFormatName, 
									   I32* pVersionNumber)
{
	IFXRESULT result = IFX_OK;

	IFXString token;
	result = m_scanner.Scan( &token );
	if( IFXSUCCESS( result ) && token == IDTF_FORMAT )
		result = m_scanner.ScanString( pFormatName );

	if( IFXSUCCESS( result ) )
		result = m_scanner.Scan( &token );

	if( IFXSUCCESS( result ) && token == IDTF_VERSION )
		result = m_scanner.ScanInteger( pVersionNumber );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT FileParser::ParseSceneData( SceneData* pSceneData )
{
	IFXRESULT result = IFX_OK;
	//Color ambientLight;

	// parse scene block
	result = BlockBegin( IDTF_SCENE );

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( pSceneData );

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // no scene data

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT FileParser::ParseFileReference( FileReference* pFileReference )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pFileReference )
	{
		FileReferenceParser fileRefParser( 
								&m_scanner,
								pFileReference );

		result = fileRefParser.Parse();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) ); 
	}

	return result;
}

IFXRESULT FileParser::Parse()
{
	return IFX_OK;
}


IFXRESULT FileParser::ParseNodes( NodeList* pNodeList )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pNodeList )
	{
		IFXString nodeType;

		while( IFXSUCCESS( m_scanner.ScanStringToken( IDTF_NODE, &nodeType ) ) )
		{
			Node* pNode = MakeNode( nodeType );

			if( NULL != pNode )
			{
				pNode->SetType( nodeType );

				result = ParseNode( pNode );
				if( IFXSUCCESS( result ) )
					result = pNodeList->AddNode( pNode );

				delete pNode;
			}
			else
				result = IFX_E_INVALID_POINTER; // can be unknown node type
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


IFXRESULT FileParser::ParseResources( SceneResources* pSceneResources )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pSceneResources )
	{
		IFXString resourceType;

		while( IFXSUCCESS( m_scanner.ScanStringToken( IDTF_RESOURCE_LIST, &resourceType ) ) 
			   && IFXSUCCESS( result ) )
		{
			ResourceList* pResourceList = 
				pSceneResources->GetResourceList( resourceType );

			if( NULL != pResourceList )
			{
				pResourceList->SetType( resourceType );

				result = ParseResourceList( pResourceList );
			}
			else
				result = IFX_E_INVALID_POINTER; // can be unknown resource type
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT FileParser::ParseModifiers( ModifierList* pModifierList )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pModifierList )
	{
		IFXString modifierType;

		while( IFXSUCCESS( BlockBegin( IDTF_MODIFIER, &modifierType ) ) 
			   && IFXSUCCESS( result ) )
		{
			Modifier* pModifier = MakeModifier( modifierType );

			if( NULL != pModifier )
			{
				pModifier->SetType( modifierType );

				result = ParseModifier( pModifier );

				if( IFXSUCCESS( result ) )
					result = BlockEnd();

				if( IFXSUCCESS( result ) )
					result = pModifierList->AddModifier( pModifier );

				delete pModifier;
			}
			else
				result = IFX_E_INVALID_POINTER; // can be unknown modifier type
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT FileParser::ParseNode( Node* pNode )
{
	IFXRESULT result = IFX_OK;

	NodeParser nodeParser( &m_scanner, pNode );

	result = nodeParser.Parse();

	return result;
}

IFXRESULT FileParser::ParseResourceList( ResourceList* pResourceList )
{
	IFXRESULT result = IFX_OK;

	ResourceListParser resourceListParser( &m_scanner, pResourceList );

	result = resourceListParser.Parse();

	return result;
}

IFXRESULT FileParser::ParseModifier( Modifier* pModifier )
{
	IFXRESULT result = IFX_OK;

	ModifierParser modifierParser( &m_scanner, pModifier );

	result = modifierParser.Parse();

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

namespace U3D_IDTF
{

Node* MakeNode( const IFXString& rNodeType )
{
	if( rNodeType == IDTF_LIGHT )
		return new LightNode;
	else if( rNodeType == IDTF_VIEW )
		return new ViewNode;
	else if( rNodeType == IDTF_MODEL )
		return new ModelNode;
	else if( rNodeType == IDTF_GROUP )
		return new Node;
	else
		return NULL;
}

Modifier* MakeModifier( const IFXString& rModifierType )
{
	if( rModifierType == IDTF_SHADING_MODIFIER )
		return new ShadingModifier;
	else if( rModifierType == IDTF_BONE_WEIGHT_MODIFIER )
		return new BoneWeightModifier;
	else if( rModifierType == IDTF_ANIMATION_MODIFIER )
		return new AnimationModifier;
	else if( rModifierType == IDTF_CLOD_MODIFIER )
		return new CLODModifier;
	else if( rModifierType == IDTF_SUBDIVISION_MODIFIER )
		return new SubdivisionModifier;
	else if( rModifierType == IDTF_GLYPH_MODIFIER )
		return new GlyphModifier;
	else
		return NULL;
}

}
