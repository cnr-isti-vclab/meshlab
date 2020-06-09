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
//	CIFXBoundHierarchy.h
//
//	DESCRIPTION
//
//		Header file for class CIFXBoundHierarchy
//	NOTES
//
//*****************************************************************************
#ifndef CIFXBOUNDHIERARCHY_DOT_H
#define CIFXBOUNDHIERARCHY_DOT_H

#include "IFXBoundHierarchy.h"

class CIFXBoundHierarchy : virtual public IFXBoundHierarchy
{
	CIFXBoundHierarchy();
	virtual ~CIFXBoundHierarchy();

	friend IFXRESULT IFXAPI_CALLTYPE CIFXBoundHierarchy_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown Interface
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);

	// IFXBoundHierarchy Interface
	void      IFXAPI 	DeallocateResultList();

	IFXRESULT IFXAPI 	GetCollisionResultPointer( U32                   uModelIndex, 
										 CIFXCollisionResult** ppResultPointer );

	IFXRESULT IFXAPI 	GetFaceCounts(U32** puFaceCounts);

	void      IFXAPI 	GetMatrixComponents( U32           uIndex,
								   IFXVector3&   mTranslation,
								   IFXVector3&   mScaleFactor,
								   IFXMatrix4x4& mTargetMatrix );

	IFXRESULT IFXAPI 	GetPositionCounts(U32** puPositionCounts);

	IFXRESULT IFXAPI 	GetRoot(CIFXBTreeNode** ppRoot);

	void      IFXAPI 	GetScaleFactor( U32         uIndex, 
							  IFXVector3& vScaleFactor );

	void      IFXAPI 	GetTransposeMatrix( U32           uIndex, 
								  IFXMatrix4x4& mTransposeMatrix );

	void      IFXAPI 	GetWorldMatrix( U32           uIndex,
							  IFXMatrix4x4& mWorldMatrix );

	IFXRESULT IFXAPI 	InitHierarchy( IFXMeshGroup* pMeshGroup, 
							 IFXVector3**  ppVertexList );

	IFXRESULT IFXAPI 	IntersectBoxSphere( IFXBoundHierarchy* pHierarchy, 
											F32                fRadiusSquared,
		 									IFXVector3&        vSphereCentroid, 
											IFXVector3&        vContactPoint, 
											IFXVector3         vContactNormal[2] );

	IFXRESULT IFXAPI 	IntersectHierarchy(IFXBoundHierarchy* pHierarchy);
	
	IFXRESULT IFXAPI 	IntersectHierarchyQuick( IFXBoundHierarchy* pHierarchy, 
													IFXVector3         vMin[2], 
													IFXVector3         vMax[2] );
	
	IFXRESULT IFXAPI 	IntersectRay( IFXVector3&           vOrigin, 
										IFXVector3&           vDirection, 
										U32                   uPickType, 
										CIFXCollisionResult** ppResultPointer );

	void      IFXAPI 	SetMatrix( U32           uIndex, 
									IFXMatrix4x4& mMatrix, 
									IFXVector3&   vScale );

	IFXRESULT IFXAPI 	SetResult( U32         uModelIndex, 
									U32         uMeshID, 
									U32         uFaceID, 
				  					IFXVector3& vIntersectPoint, 
									IFXVector3& vIntersectNormal );

	IFXRESULT IFXAPI 	SetResult( U32        uMeshID, 
									U32        uFaceID, 
					 				IFXVector3 vVerts[3], 
									F32        fU, 
									F32        fV, 
									F32        fT );

	// Inlines
	IFXVector3* IFXAPI GetVertexList()           { return m_pVertexList;         };
	U32   GetDepth()                { return m_uDepth;              };
	U32	  IFXAPI 	GetNumResults(U32 uIndex) { return m_uNumResults[uIndex]; };
	U32	  IFXAPI 	GetNumVerts()             { return m_uNumVerts;           };
	U32	  IFXAPI 	GetSplitType()            { return m_uSplitType;          };

	void  IFXAPI 	SetSplitType(SplitType uSplitType)    { m_uSplitType = uSplitType; };

private:

	// IFXUnknown Interface
	U32 m_uRefCount;

	// IFXBoundHierarchy Interface
	CIFXResultAllocator* m_pFreeList; 	         // FreeList for collision results
	CIFXCollisionResult* m_pCollisionResult[2];
	CIFXBTreeNode*       m_pRoot;                // Pointer to the bounding hierarchy tree
	IFXMeshGroup*        m_pMeshGroup;           // IFXMeshGroup associated with this hierarchy
	IFXVector3*          m_pVertexList;
	IFXMatrix4x4         m_mWorldMatrix[2];
	IFXMatrix4x4         m_mUnscaledWorldMatrix[2];
	IFXMatrix4x4         m_mTransposeMatrix[2];
	IFXVector3           m_vScaleFactor[2];
	IFXVector3			 m_vTranslation[2];
	SplitType            m_uSplitType;
	BOOL                 m_bCollisionFound;
	U32                  m_uNumResults[2];
	U32                  m_uModelIndex;
	U32                  m_uDepth;               // Depth of the bounding hierarchy
	U32                  m_uNumVerts;
	U32                  m_uNumFaces;            // Total faces contained in m_pMeshGroup
	U32                  m_uNumMeshes;
	U32*                 m_puPositionCounts;
	U32*                 m_puFaceCounts;
};

#endif
