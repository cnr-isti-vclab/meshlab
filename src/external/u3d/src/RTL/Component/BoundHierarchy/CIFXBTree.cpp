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
//	CIFXBTree.cpp
//
//	DESCRIPTION
//
//		Source file for class CIFXBTree.  Contains the definitions for 
//		explicit tree traversal routines.
//
//	NOTES
//
//*****************************************************************************

//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXBTree.h"
#include "CIFXBoundHierarchy.h"
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
//	CIFXBTree::IntersectRayTraverse
//
//  Traverse CIFXBTree's and test for intersection of IFXBoundVolume and the
//  specified ray.
//
//-----------------------------------------------------------------------------

IFXRESULT CIFXBTree::IntersectRayTraverse(CIFXBTreeNode* pNode, 
										  IFXVector3&    vOrigin, 
										  IFXVector3&    vDirection,
										  U32            uPickType)
{
	IFXRESULT result = IFX_OK;

	if( pNode )
	{
		IFXBoundVolume *pBound = pNode->GetBound();

		if( pBound )
		{
			result = pBound->IntersectRay(vOrigin, vDirection, uPickType);

			if( result == IFX_TRUE )
			{
				CIFXBTreeNode *pNodeALChild = pNode->GetLeftTreeNode();
				CIFXBTreeNode *pNodeARChild = pNode->GetRightTreeNode();

				// Check if leaf node has been reached
				if( !pNodeALChild && !pNodeARChild )
					result = pBound->IntersectRayTriangle(vOrigin, vDirection, uPickType);
				else
				{
					if( pNodeALChild )
						result = IntersectRayTraverse(pNodeALChild, vOrigin, vDirection, uPickType);

					if( pNodeARChild )
						result = IntersectRayTraverse(pNodeARChild, vOrigin, vDirection, uPickType);
				}
			}
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXBTree::IntersectTraverse
//
//  Traverse CIFXBTree's and test for intersection.  Passed in nodes are
//	assumed to be from different CIFXBoundHierarchies
//-----------------------------------------------------------------------------

IFXRESULT CIFXBTree::IntersectTraverse(CIFXBTreeNode *pNodeA, CIFXBTreeNode *pNodeB)
{
	IFXRESULT result = IFX_OK;

	if( pNodeA && pNodeB )
	{
		// Retrieve boundingVolumes from each node
		IFXBoundVolume *pBoundA = pNodeA->GetBound();
		IFXBoundVolume *pBoundB = pNodeB->GetBound();

		if( pBoundA && pBoundB )
		{
			// Test pBoundA - pBoundB intersection
			result = pBoundA->IntersectBound(pBoundB);

			if( result == IFX_TRUE )
			{
				CIFXBTreeNode *pNodeALChild = pNodeA->GetLeftTreeNode();
				CIFXBTreeNode *pNodeARChild = pNodeA->GetRightTreeNode();

				// Check if leaf node has been reached
				if( !pNodeALChild && !pNodeARChild )
				{
					CIFXBTreeNode *pNodeBLChild = pNodeB->GetLeftTreeNode();
					CIFXBTreeNode *pNodeBRChild = pNodeB->GetRightTreeNode();

					if( !pNodeBLChild && !pNodeBRChild )
						result = pBoundA->IntersectTriangle(pBoundB);
					else
					{
						// Recurse down left branch of pBoxB
						if( pNodeBLChild)
							IntersectTraverse(pNodeA, pNodeBLChild);

						// Recurse down right branch of pBoxB
						if( pNodeBRChild )
							IntersectTraverse(pNodeA, pNodeBRChild);
					}
				}
				else
				{
					// Recurse down left branch
					if( pNodeALChild )
						IntersectTraverse(pNodeALChild, pNodeB);

					// Recurse down right branch
					if( pNodeARChild )
						IntersectTraverse(pNodeARChild, pNodeB);
				}
			}
		}
		else 
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXBTree::IntersectTraverseQuick
//
//  Traverse first level of CIFXBTree's and test for intersection.  Passed in 
//  nodes are assumed to be from different CIFXBoundHierarchies.  If a collision
//  is detected, the min/max vertices of each box are passed back.
//-----------------------------------------------------------------------------


IFXRESULT CIFXBTree::IntersectTraverseQuick( CIFXBTreeNode *pNodeA, 
											 CIFXBTreeNode *pNodeB,
											 IFXVector3     vMin[2], 
											 IFXVector3     vMax[2] )
{
	IFXRESULT result = IFX_OK;

	if( pNodeA && pNodeB )
	{
		// Retrieve boundingVolumes from each node
		IFXBoundVolume *pBoundA = pNodeA->GetBound();
		IFXBoundVolume *pBoundB = pNodeB->GetBound();

		if( pBoundA && pBoundB )
		{
			// Test pBoundA - pBoundB intersection
			result = pBoundA->IntersectBound(pBoundB);

			if( IFX_TRUE == result )
			{
				CIFXAxisAlignedBBox* pBox = NULL;

				// Cast pBound to a CIFXAxisAlignedBBox
				pBox = (CIFXAxisAlignedBBox *) pBoundA;
				
				pBox->GetMinPosition(&vMin[0]);
				pBox->GetMaxPosition(&vMax[0]);

				pBox = (CIFXAxisAlignedBBox *) pBoundB;
				
				pBox->GetMinPosition(&vMin[1]);
				pBox->GetMaxPosition(&vMax[1]);
			}
		}
		else 
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}




//-----------------------------------------------------------------------------
//	CIFXBTree::IntersectTraverseQuick
//
//  Traverse first level of CIFXBTree's and test for intersection.  Passed in 
//  nodes are assumed to be from different CIFXBoundHierarchies.  If a collision
//  is detected, the min/max vertices of each box are passed back.
//-----------------------------------------------------------------------------


IFXRESULT CIFXBTree::IntersectBoxSphere( CIFXBTreeNode *pNodeA, 
										 CIFXBTreeNode *pNodeB,
										 F32            fRadiusSquared,
										 IFXVector3&   vSphereCentroid,
										 IFXVector3&   vContactPoint,
										 IFXVector3    vContactNormal[2] )
{
	IFXRESULT result = IFX_OK;

	if( pNodeA && pNodeB )
	{
		// Retrieve boundingVolumes from each node
		IFXBoundVolume *pBoundA = pNodeA->GetBound();
		IFXBoundVolume *pBoundB = pNodeB->GetBound();

		if( pBoundA && pBoundB )
		{
			// Cast pBound to a CIFXAxisAlignedBBox
			CIFXAxisAlignedBBox* pBox = (CIFXAxisAlignedBBox *) pBoundA;

			// Test pBoundA - sphere intersection
			result = pBox->IntersectBoxSphere( pBoundB, 
											   fRadiusSquared, 
											   vSphereCentroid, 
											   vContactPoint, 
											   vContactNormal );
		}
		else 
			result = IFX_E_INVALID_POINTER;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}
