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
//	CIFXBTreeNode.h
//
//	DESCRIPTION - Class interface for bounding volume tree node
//
//	NOTES
//
//*****************************************************************************

#ifndef __CIFXBTREENODE_H__
#define __CIFXBTREENODE_H__

#include "IFXBoundVolume.h"

class CIFXBTreeNode
{
public:

	CIFXBTreeNode();
	~CIFXBTreeNode();
	
	IFXRESULT Initialize(U32             uLevel, 
						 IFXBoundVolume* pBound);

	IFXRESULT SetBound(IFXBoundVolume *pBound);
	
	IFXRESULT SetLeftTreeNode(CIFXBTreeNode *pLeftBTreeNode);
	
	IFXRESULT SetRightTreeNode(CIFXBTreeNode *pRightBTreeNode);

	// Inlines
	IFXBoundVolume* GetBound()           { return m_pBound;          };
	CIFXBTreeNode*  GetRightTreeNode()   { return m_pRightBTreeNode; };
	CIFXBTreeNode*  GetLeftTreeNode()    { return m_pLeftBTreeNode;  };

	U32             GetLevel()           { return m_uLevel;   };
	void            SetLevel(U32 uLevel) { m_uLevel = uLevel; };

private:

	IFXBoundVolume *m_pBound;
	CIFXBTreeNode  *m_pLeftBTreeNode,
				   *m_pRightBTreeNode;
	U32             m_uLevel;
};

#endif
