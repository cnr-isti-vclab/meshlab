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
//
//	CIFXBTreeNode.cpp
//
//	DESCRIPTION - Implementation of bounding volume tree node class
//
//	NOTES
//
//*****************************************************************************
//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXBTreeNode.h"
#include "CIFXAxisAlignedBBox.h"

//*****************************************************************************
//	Defines
//***************************************************************************** 

//*****************************************************************************
//	Constants
//***************************************************************************** 

//*****************************************************************************
//	Enumerations
//*****************************************************************************

//*****************************************************************************
//	Global data
//*****************************************************************************

//*****************************************************************************
//	Local data
//*****************************************************************************

//*****************************************************************************
//	Classes, structures and types
//*****************************************************************************

//-----------------------------------------------------------------------------
//	CIFXBTreeNode::CIFXBTreeNode
//
//  Constructor
//-----------------------------------------------------------------------------
CIFXBTreeNode::CIFXBTreeNode()
{
	m_pBound          = NULL;
	m_pLeftBTreeNode  = NULL;
	m_pRightBTreeNode = NULL;
}


//-----------------------------------------------------------------------------
//	CIFXBTreeNode::~CIFXBTreeNode
//
//  Destructor
//-----------------------------------------------------------------------------
CIFXBTreeNode::~CIFXBTreeNode()
{
	if( m_pBound )
	{
		CIFXAxisAlignedBBox *pBox = (CIFXAxisAlignedBBox *) m_pBound;

		IFXDELETE(pBox);
	}

	m_pBound = NULL;

	if( m_pLeftBTreeNode ) 
		IFXDELETE(m_pLeftBTreeNode);

	if( m_pRightBTreeNode ) 
		IFXDELETE(m_pRightBTreeNode);
}


//-----------------------------------------------------------------------------
//	CIFXBTreeNode::Initialize
//
//  Initialize CIFXTreeNode
//-----------------------------------------------------------------------------

IFXRESULT CIFXBTreeNode::Initialize(U32 uLevel, IFXBoundVolume* pBound)
{
	IFXRESULT result = IFX_OK;

	if( pBound )
	{
		m_pBound = pBound;

		m_uLevel = uLevel;

		m_pLeftBTreeNode  = NULL;
		m_pRightBTreeNode = NULL;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXBTreeNode::SetBound(IFXBoundVolume* pBound)
{
	IFXRESULT result = IFX_OK;

	if( pBound )
		m_pBound = pBound;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXBTreeNode::SetLeftTreeNode(CIFXBTreeNode* pLeftBTreeNode)
{
	IFXRESULT result = IFX_OK;

	if( pLeftBTreeNode )
		m_pLeftBTreeNode = pLeftBTreeNode;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXBTreeNode::SetRightTreeNode(CIFXBTreeNode* pRightBTreeNode)
{
	IFXRESULT result = IFX_OK;

	if( pRightBTreeNode )
		m_pRightBTreeNode = pRightBTreeNode;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}
