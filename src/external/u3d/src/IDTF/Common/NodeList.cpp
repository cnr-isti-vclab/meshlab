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
  @file NodeList.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "NodeList.h"
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

NodeList::NodeList() 
{
}

NodeList::~NodeList()
{
}

IFXRESULT NodeList::AddNode( const Node* pNode )
{
	IFXRESULT result = IFX_OK;
	Node* pTmpNode = NULL;

	const IFXString& rNodeType = pNode->GetType();

	if( rNodeType == IDTF_LIGHT )
	{
		LightNode& lightNode = m_lightNodeList.CreateNewElement();
		lightNode = *static_cast<const LightNode*>(pNode);
		pTmpNode = &lightNode;
	}
	else if( rNodeType == IDTF_VIEW )
	{
		ViewNode& viewNode = m_viewNodeList.CreateNewElement();
		viewNode = *static_cast<const ViewNode*>(pNode);
		pTmpNode = &viewNode;
	}
	else if( rNodeType == IDTF_MODEL )
	{
		ModelNode& modelNode = m_modelNodeList.CreateNewElement();
		modelNode = *static_cast<const ModelNode*>(pNode);
		pTmpNode = &modelNode;
	}
	else if( rNodeType == IDTF_GROUP )
	{
		Node& groupNode = m_groupNodeList.CreateNewElement();
		groupNode = *pNode;
		pTmpNode = &groupNode;
	}
	else
		result = IFX_E_UNDEFINED;

	if( IFXSUCCESS( result ) )
	{
		// add new node pointer to the node pointer list only if it has known type
		Node*& node = m_nodePointerList.CreateNewElement();
		node = pTmpNode;
	}

	return result;
}

const Node* NodeList::GetNode( U32 index ) const
{
	return m_nodePointerList.GetElementConst( index );
}

U32 NodeList::GetNodeCount() const
{
	return m_nodePointerList.GetNumberElements();
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
