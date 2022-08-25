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
//	CIFXBoundHierarchy.cpp
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************

//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXBoundHierarchy.h"
#include "CIFXBTree.h"
#include "CIFXBTreeNode.h"
#include "CIFXResultAllocator.h"
#include "CIFXCollisionResult.h"
#include "IFXMesh.h"

//*****************************************************************************
//	Defines
//***************************************************************************** 
#define IFX_EPSILON 1e-6f
#define POS_INF 99999.0f;
#define NEG_INF -99999.0f;

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
//	CIFXBoundHierarchy::CIFXBoundHierarchy
//
//  Constructor
//-----------------------------------------------------------------------------

CIFXBoundHierarchy::CIFXBoundHierarchy()
{
	//IFXUnknown Interface
	m_uRefCount            = 0;

	//IFXBoundHierarchy Interface
	m_pRoot                = NULL;
	m_pMeshGroup           = NULL;
	m_pVertexList          = NULL;
	m_pFreeList            = NULL;
	m_pCollisionResult[0]  = NULL;
	m_pCollisionResult[1]  = NULL;
	m_puPositionCounts     = NULL;
	m_puFaceCounts         = NULL;
	m_uModelIndex          = 1;
	m_uNumFaces            = 0;
	m_uNumVerts            = 0;
	m_uNumMeshes           = 0;

	// Store separate translation/scale/rotation values
	m_vTranslation[0].Set(0,0,0);
	m_vTranslation[1].Set(0,0,0);

	m_vScaleFactor[0].Set(1,1,1);
	m_vScaleFactor[1].Set(1,1,1);

	m_mUnscaledWorldMatrix[0].Reset();
	m_mUnscaledWorldMatrix[1].Reset();

	m_mWorldMatrix[0].Reset();
	m_mWorldMatrix[1].Reset();

	m_mTransposeMatrix[0].Reset();
	m_mTransposeMatrix[1].Reset();
}


//-----------------------------------------------------------------------------
//	CIFXBoundHierarchy::~CIFXBoundHierarchy
//
//  Destructor
//-----------------------------------------------------------------------------

CIFXBoundHierarchy::~CIFXBoundHierarchy()
{
	IFXDELETE(m_pRoot);
	IFXDELETE_ARRAY(m_pVertexList);

	DeallocateResultList();

	IFXDELETE(m_pFreeList);

	// Delete ChangeCount arrays
	IFXDELETE_ARRAY(m_puPositionCounts);
	IFXDELETE_ARRAY(m_puFaceCounts);
}


IFXRESULT IFXAPI_CALLTYPE CIFXBoundHierarchy_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXBoundHierarchy component.
		CIFXBoundHierarchy *pBoundHierarchy = new CIFXBoundHierarchy;

		if ( pBoundHierarchy )
		{
			// Perform a temporary AddRef for our usage of the component.
			pBoundHierarchy->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pBoundHierarchy->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pBoundHierarchy->Release();
		}
		else 
			result = IFX_E_OUT_OF_MEMORY;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown interface support...
U32 CIFXBoundHierarchy::AddRef(void) 
{
	
	return ++m_uRefCount;
}


U32 CIFXBoundHierarchy::Release(void) 
{
	
	if ( !( --m_uRefCount ) )
	{
		delete this;
		return 0;
	}

	return m_uRefCount;
} 


IFXRESULT CIFXBoundHierarchy::QueryInterface( IFXREFIID interfaceId, 
                                              void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXBoundHierarchy )
			*ppInterface = ( IFXBoundHierarchy* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXBoundHierarchy::DeallocateResultList()
//
//  Reset various components of the bounding hierarchy
//-----------------------------------------------------------------------------

void CIFXBoundHierarchy::DeallocateResultList()
{
	// Clean m_pCollisionResult[0]
	CIFXCollisionResult* pCollisionResult = m_pCollisionResult[0];

	while( pCollisionResult )
	{
		CIFXCollisionResult* pTmp = pCollisionResult->GetNext();

		m_pFreeList->Deallocate(pCollisionResult);

		pCollisionResult = pTmp;
	}

	m_pCollisionResult[0] = NULL;

	// Clean m_pCollisionResult[1]
	pCollisionResult = m_pCollisionResult[1];

	while( pCollisionResult )
	{
		CIFXCollisionResult* pTmp = pCollisionResult->GetNext();

		m_pFreeList->Deallocate(pCollisionResult);

		pCollisionResult = pTmp;
	}

	m_pCollisionResult[1] = NULL;

	// Reset the results counters
	m_uNumResults[0] = 0;
	m_uNumResults[1] = 0;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetCollisionResultPointer
//
// Return a pointer to the collision result list
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::GetCollisionResultPointer(U32 uModelIndex, CIFXCollisionResult** ppResultPointer)
{
	IFXASSERT(uModelIndex == 0 || uModelIndex == 1);

	IFXRESULT result = IFX_OK;

	if( ppResultPointer )
		*ppResultPointer = m_pCollisionResult[uModelIndex];
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetFaceCounts
//
// Return the array of face change counts.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::GetFaceCounts(U32** ppFaceCounts)
{
	IFXRESULT result = IFX_OK;

	if( ppFaceCounts )
		*ppFaceCounts = m_puFaceCounts;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetMatrixComponents
//
// Copy out the matrix components
//-----------------------------------------------------------------------------

void CIFXBoundHierarchy::GetMatrixComponents( U32           uIndex,
											  IFXVector3&   mTranslation,
											  IFXVector3&   mScaleFactor,
											  IFXMatrix4x4& mTargetMatrix )
{
	IFXASSERT(uIndex == 0 || uIndex == 1);

	mTargetMatrix = m_mUnscaledWorldMatrix[uIndex];
	mTranslation  = m_vTranslation[uIndex];
	mScaleFactor  = m_vScaleFactor[uIndex];
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetPositionCounts
//
// Return the array of position change counts.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::GetPositionCounts(U32** ppPositionCounts)
{
	IFXRESULT result = IFX_OK;

	if( ppPositionCounts )
		*ppPositionCounts = m_puPositionCounts;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetRoot
//
// Return the pointer to the root node of the tree
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::GetRoot(CIFXBTreeNode** ppRoot)
{ 
	IFXRESULT result = IFX_OK;

	if( ppRoot )
	{
		if( m_pRoot )
			*ppRoot = m_pRoot;
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetScaleFactor
//
// Return the matrix scaleFactor
//-----------------------------------------------------------------------------

void CIFXBoundHierarchy::GetScaleFactor(U32 uIndex, IFXVector3& vScaleFactor)
{
	IFXASSERT(uIndex == 0 || uIndex == 1);

	vScaleFactor = m_vScaleFactor[uIndex];
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetTransposeMatrix
//
// Copy contents of m_transposeMatrix to pTargetMatrix
//-----------------------------------------------------------------------------

void CIFXBoundHierarchy::GetTransposeMatrix(U32 uIndex, IFXMatrix4x4& mTransposeMatrix) 
{ 
	IFXASSERT(uIndex == 0 || uIndex == 1);

	mTransposeMatrix = m_mTransposeMatrix[uIndex];
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::GetWorldMatrix
//
// Copy out the matrix components
//-----------------------------------------------------------------------------

void CIFXBoundHierarchy::GetWorldMatrix( U32           uIndex,
										 IFXMatrix4x4& mWorldMatrix )
{
	IFXASSERT(uIndex == 0 || uIndex == 1);

	mWorldMatrix = m_mWorldMatrix[uIndex];
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::InitHierarchy
//
// Initialize the bounding hierarchy
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::InitHierarchy( IFXMeshGroup* pMeshGroup, 
											 IFXVector3**  ppVertexList )
{
	IFXRESULT result = IFX_OK;

	if( pMeshGroup && ppVertexList )
	{
		m_pVertexList = *ppVertexList;
		m_pMeshGroup  = pMeshGroup;

		// Compute total faces in m_pMeshGroup
		m_uNumMeshes = m_pMeshGroup->GetNumMeshes();

		if( m_uNumMeshes > 0 )
		{
			IFXMesh *pMesh = NULL;
			U32      i;

			for(i=0; i<m_uNumMeshes; i++)
			{
				m_pMeshGroup->GetMesh(i, pMesh);

				if( pMesh )
				{
					m_uNumFaces += pMesh->GetNumFaces();
					m_uNumVerts += pMesh->GetNumVertices();
				}

				IFXRELEASE(pMesh);
			}
			
			if( m_uNumFaces > 0 )
			{
				// Initialize hierarchy depth
				m_uDepth = 0;

				// Initialize bound spliting method
				m_uSplitType = MIDPOINT;

				// Zero out number of results
				m_uNumResults[0] = 0;
				m_uNumResults[1] = 0;

				// Let's do some memory allocation:
				//
				//   1. Results lists
				//   2. BH root node
				//   3. Change counts

				m_pFreeList        = new CIFXResultAllocator(50, 50);
				m_pRoot            = new CIFXBTreeNode;
				m_puPositionCounts = new U32 [m_uNumMeshes];
				m_puFaceCounts     = new U32 [m_uNumMeshes];

				if( m_pFreeList &&  m_pRoot && m_puPositionCounts && m_puFaceCounts )
				{
					for(i=0; i<m_uNumMeshes; i++)
					{
						m_puPositionCounts[i] = (U32)-1;
						m_puFaceCounts[i]     = (U32)-1;
					}
				}
				else
				{
					// Cleanup memory that may have been allocated
					IFXDELETE_ARRAY(m_puPositionCounts);
					IFXDELETE_ARRAY(m_puFaceCounts);
					IFXDELETE_ARRAY(m_pFreeList);
					IFXDELETE(m_pRoot);

					result = IFX_E_OUT_OF_MEMORY;
				}
			}
			else
				// No face information available - Not necessarily an error because
				// it could be due to streaming state.  Calling function must deal
				// with IFX_CANCEL code.
				result = IFX_CANCEL;
		}
		else
			// No meshes in meshGroup
			result = IFX_E_UNDEFINED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}

//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::IntersectBoxSphere
//
// Test for intersection between a sphere and the top level OBB.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::IntersectBoxSphere( IFXBoundHierarchy* pHierarchy, 
												  F32                 fRadiusSquared,
												  IFXVector3&        vSphereCentroid,
		 										  IFXVector3&        vContactPoint, 
												  IFXVector3         vContactNormal[2] )
{
	IFXRESULT result = IFX_OK;

	if( pHierarchy )
	{
		// Remove any results that may still be in the list
		DeallocateResultList();
		pHierarchy->DeallocateResultList();

		CIFXBTreeNode *pRootNode = NULL;
		result = pHierarchy->GetRoot(&pRootNode);

		if( pRootNode )
		{
			CIFXBTree bTree;

			result = bTree.IntersectBoxSphere( m_pRoot, 
											   pRootNode, 
											   fRadiusSquared, 
											   vSphereCentroid,
											   vContactPoint, 
											   vContactNormal );
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::IntersectHierarchy
//
// Test for intersection between two bounding hierarchies
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::IntersectHierarchy(IFXBoundHierarchy* pHierarchy)
{
	IFXRESULT result = IFX_OK;

	if( pHierarchy )
	{
		m_bCollisionFound = FALSE;

		// Remove any results that may still be in the list
		DeallocateResultList();
		pHierarchy->DeallocateResultList();

		CIFXBTreeNode *pRootNode = NULL;
		pHierarchy->GetRoot(&pRootNode);

		if( pRootNode )
		{
			CIFXBTree bTree;
			bTree.IntersectTraverse(m_pRoot, pRootNode);

			if( m_bCollisionFound )
				result = IFX_TRUE;
			else
				result = IFX_FALSE;
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::IntersectHierarchy
//
// Test for intersection between two bounding hierarchies
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::IntersectHierarchyQuick( IFXBoundHierarchy* pHierarchy,
													   IFXVector3         vMin[2], 
													   IFXVector3         vMax[2])
{
	IFXRESULT result = IFX_OK;

	if( pHierarchy )
	{
		// Remove any results that may still be in the list
		DeallocateResultList();
		pHierarchy->DeallocateResultList();

		CIFXBTreeNode *pRootNode = NULL;
		result = pHierarchy->GetRoot(&pRootNode);

		if( pRootNode )
		{
			CIFXBTree bTree;

			result = bTree.IntersectTraverseQuick( m_pRoot, 
												   pRootNode, 
												   vMin, 
												   vMax );
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::IntersectRay
//
// Test for intersection between the bounding hierarchy and a ray from pOrigin 
// in pDirection.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::IntersectRay( IFXVector3& vOrigin, 
										    IFXVector3& vDirection,
										    U32         uPickType, 
											CIFXCollisionResult** ppResultPointer )
{
	IFXRESULT result = IFX_OK;

	if( ppResultPointer )
	{
		// Reset intersection flag
		m_bCollisionFound = FALSE;

		// Remove any results that may still be in the list
		DeallocateResultList();

		CIFXBTree bTree;
		result = bTree.IntersectRayTraverse(m_pRoot, vOrigin, vDirection, uPickType);

		// Return the list of collision results
		*ppResultPointer = m_pCollisionResult[0];

		if( m_bCollisionFound )
			result = IFX_TRUE;
		else
			result = IFX_FALSE;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::SetMatrix
//
// Copy contents of pMatrix into m_mWorldMatrix 
//-----------------------------------------------------------------------------

void CIFXBoundHierarchy::SetMatrix(U32 uIndex, IFXMatrix4x4& mMatrix, IFXVector3& vScale) 
{
	IFXASSERT(uIndex == 0 || uIndex == 1);

	m_uModelIndex = uIndex;

	// Store the matrix as components rather than as a
	// single entity.  This will facilitate usage for
	// overlap testing.

	// Store the internal matrix and set it's translation to (0,0,0)
	m_mUnscaledWorldMatrix[m_uModelIndex]     = mMatrix;
	m_mUnscaledWorldMatrix[m_uModelIndex][12] = 0.0f;
	m_mUnscaledWorldMatrix[m_uModelIndex][13] = 0.0f;
	m_mUnscaledWorldMatrix[m_uModelIndex][14] = 0.0f;
	m_mUnscaledWorldMatrix[m_uModelIndex][15] = 1.0f;

	// Store the translation
	m_vTranslation[m_uModelIndex][0] = mMatrix[12];
	m_vTranslation[m_uModelIndex][1] = mMatrix[13];
	m_vTranslation[m_uModelIndex][2] = mMatrix[14];

	// Store the scale factor
	m_vScaleFactor[m_uModelIndex] = vScale;

	// Store the full on matrix
	m_mWorldMatrix[m_uModelIndex] = mMatrix;
	m_mWorldMatrix[m_uModelIndex].Scale(vScale);

	// Compute the transpose of the input matrix
	m_mTransposeMatrix[m_uModelIndex].MakeIdentity();

	m_mTransposeMatrix[m_uModelIndex][0]  = mMatrix[0];
	m_mTransposeMatrix[m_uModelIndex][1]  = mMatrix[4];
	m_mTransposeMatrix[m_uModelIndex][2]  = mMatrix[8];

	m_mTransposeMatrix[m_uModelIndex][4]  = mMatrix[1];
	m_mTransposeMatrix[m_uModelIndex][5]  = mMatrix[5];
	m_mTransposeMatrix[m_uModelIndex][6]  = mMatrix[9];

	m_mTransposeMatrix[m_uModelIndex][8]  = mMatrix[2];
	m_mTransposeMatrix[m_uModelIndex][9]  = mMatrix[6];
	m_mTransposeMatrix[m_uModelIndex][10] = mMatrix[10];

}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::SetResult
//
// Sets the passed in MeshID, FaceID, the approximate triangle/triangle 
// intersection point in the next available slot in the results array.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::SetResult( U32         uModelIndex, 
										 U32         uMeshID, 
										 U32         uFaceID, 
										 IFXVector3& vIntersectPoint,
										 IFXVector3& vIntersectNormal )
{
	IFXRESULT result = IFX_OK;
	
	CIFXCollisionResult* pCollisionResult = m_pCollisionResult[uModelIndex];
	BOOL                 bFound = FALSE;

	// Check if mesh/face pair already in list
	while( pCollisionResult && !bFound )
	{
		if( (pCollisionResult->GetMeshID() == uMeshID) && 
			(pCollisionResult->GetFaceID() == uFaceID) ) 
		{
			bFound = TRUE;
			break;
		}
		else
			pCollisionResult = pCollisionResult->GetNext();
	}

	// if the mesh/face pair DOES NOT exist, create a new object
	if( !bFound )
	{
		pCollisionResult = NULL;

		m_pFreeList->Allocate(&pCollisionResult);

		if( pCollisionResult )
		{
			(*pCollisionResult).SetIDs(uMeshID, uFaceID);
			(*pCollisionResult).SetIntersectPoint(vIntersectPoint);
			(*pCollisionResult).SetIntersectNormal(vIntersectNormal);

			// Check if list is NULL
			if( !m_pCollisionResult[uModelIndex] ) 
				m_pCollisionResult[uModelIndex] = pCollisionResult;
			else
			{
				// Insert at head of list
				pCollisionResult->SetNext(m_pCollisionResult[uModelIndex]);
				m_pCollisionResult[uModelIndex] = pCollisionResult;
			}

			m_uNumResults[uModelIndex] ++;
			m_bCollisionFound = TRUE;
		}
		else 
			result = IFX_E_OUT_OF_MEMORY;
	}

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundHierarchy::SetResult
//
// Sets the passed in MeshID, FaceID, Barycentric Coordinates and the distance
// to the ray/triangle intersection point in next available slot in the 
// results array.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundHierarchy::SetResult( U32        uMeshID, 
										 U32        uFaceID, 
										 IFXVector3 vVerts[3], 
										 F32        fU, 
										 F32        fV, 
										 F32        fT )
{
	IFXRESULT result = IFX_OK;

	CIFXCollisionResult *pCollisionResult = NULL;

	result = m_pFreeList->Allocate(&pCollisionResult);

	if( IFXSUCCESS(result) )
	{
		(*pCollisionResult).SetIDs(uMeshID, uFaceID);
		(*pCollisionResult).SetUVTCoords(fU, fV, fT);
		(*pCollisionResult).SetVertices(vVerts[0], vVerts[1], vVerts[2]);

		// Check if list is NULL
		if( !m_pCollisionResult[0] )
			m_pCollisionResult[0] = pCollisionResult;
		else
		{
			F32 fDistance = m_pCollisionResult[0]->GetDistance();

			if( IFXSUCCESS(result) )
			{
				// Check for insert at head of list
				if( fT < fDistance )
				{
					pCollisionResult->SetNext(m_pCollisionResult[0]);
					m_pCollisionResult[0] = pCollisionResult;
				}
				else
				{
					CIFXCollisionResult *pPrev    = m_pCollisionResult[0];
					CIFXCollisionResult *pCurrent = pPrev->GetNext();

					while( pCurrent && IFXSUCCESS(result) )
					{
						F32 fDistance = pCurrent->GetDistance();

						if( IFXSUCCESS(result) )
						{
							if( fT < fDistance )
								break;

							pCurrent = pCurrent->GetNext();
						}
					}
					
					pPrev->SetNext(pCollisionResult);
					pCollisionResult->SetNext(pCurrent);
				}
			}
		}

		m_uNumResults[0] ++;
	}

	return result;
}


