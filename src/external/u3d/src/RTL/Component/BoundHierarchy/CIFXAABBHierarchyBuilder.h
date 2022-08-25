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
//	CIFXAABBHierarchyBuilder.h
//
//	DESCRIPTION 
//
//		Header file for axis aligned bounding box hierarchy builder class.
//		The builder class is responsible for constructing each axis aligned
//      bounding box node in the hierarchy.
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXAABBHIERARCHYBUILDER_H__
#define __CIFXAABBHIERARCHYBUILDER_H__

#include "IFXBoundHierarchy.h"
#include "CIFXBTreeNode.h"
#include "CIFXAxisAlignedBBox.h"

class CIFXAABBHierarchyBuilder
{
public:

	IFXRESULT Build(IFXBoundHierarchy** pBoundHierarchy, 
					U32                 uType, 
					IFXMeshGroup*       pMeshGroup);

private:

	IFXRESULT CreateNode(CIFXBTreeNode *pNextNode);
};

#endif

