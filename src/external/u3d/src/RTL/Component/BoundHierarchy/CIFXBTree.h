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
//	CIFXBTree.h
//
//	DESCRIPTION
//		
//		Header file for class CIFXBTree.  Contains declarations for the
//		explicit tree traversal methods.
//
//	NOTES
//
//		Currently the tree is assumed to be binary.
//
//*****************************************************************************

#ifndef __CIFXBTREE_H__
#define __CIFXBTREE_H__

#include "CIFXBTreeNode.h"
#include "IFXVector3.h"

class CIFXBTree
{
public:

	IFXRESULT IntersectRayTraverse(CIFXBTreeNode* pNode, 
								   IFXVector3&    vOrigin, 
								   IFXVector3&    vDirection,
								   U32            uPickType);

	IFXRESULT IntersectTraverse(CIFXBTreeNode* pNodeA, 
								CIFXBTreeNode* pNodeB);

	IFXRESULT IntersectTraverseQuick(CIFXBTreeNode* pNodeA, 
									 CIFXBTreeNode* pNodeB,
									 IFXVector3     vMin[2], 
									 IFXVector3     vMax[2]);

	IFXRESULT IntersectBoxSphere(CIFXBTreeNode* pNodeA, 
								 CIFXBTreeNode* pNodeB, 
								 F32            fRadiusSquared,
								 IFXVector3&    vSphereCentroid, 
								 IFXVector3&    vContactPoint, 
								 IFXVector3     vContactNormal[2]);
};

#endif
