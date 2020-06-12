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
//	CIFXCIFXAxisAlignedBBox.cpp
//
//	DESCRIPTION
//
//		Source file for CIFXAxisAlignedBBox class.  This class defines the
//		methods that act on an axis aligned bounding box.
//
//*****************************************************************************

//*****************************************************************************
//	Includes
//*****************************************************************************

#include "CIFXAxisAlignedBBox.h"
#include "CIFXBoundUtil.h"
#include "CIFXPrimitiveOverlap.h"
#include <float.h>

//*****************************************************************************
//	Defines
//***************************************************************************** 
#ifdef IFX_EPSILON
#undef IFX_EPSILON
#endif
#define IFX_EPSILON 1e-5f
#define IFX_MAX_BIN_SIZE   8
#define SwapFloats(fA, fB) \
{                          \
	F32 fC = fA;           \
	fA = fB;               \
	fB = fC;               \
}

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
//	CIFXAxisAlignedBBox::CIFXAxisAlignedBBox
//
//  Constructor
//-----------------------------------------------------------------------------

CIFXAxisAlignedBBox::CIFXAxisAlignedBBox()
{
	m_pBoundHierarchy  = NULL;
	m_pFaceList        = NULL;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::~CIFXAxisAlignedBBox
//
//  Destructor
//-----------------------------------------------------------------------------

CIFXAxisAlignedBBox::~CIFXAxisAlignedBBox()
{
	m_pBoundHierarchy = NULL;

	if( m_pFaceList ) 
		IFXDELETE_ARRAY(m_pFaceList);
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::Build
//
//  Compute the extents of the axis aligned bounding box
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::Build()
{
	IFXRESULT result = IFX_OK;

	if( m_pFaceList )
	{
		// Retrieve a pointer to the vertex list
		IFXVector3* pVertexList = m_pBoundHierarchy->GetVertexList();
		BOOL*       uIndices    = NULL;
		U32         uNumVerts;

		if( pVertexList )
		{
			uNumVerts = m_pBoundHierarchy->GetNumVerts();

			// Allocate memory index check array
			uIndices = new BOOL[uNumVerts];

			if( uIndices )
				memset((void*)uIndices, FALSE, sizeof(BOOL)*uNumVerts);
			else 
				result = IFX_E_OUT_OF_MEMORY;
		}
		else 
			result = IFX_E_INVALID_POINTER;

		if( IFXSUCCESS(result) )
		{
			U32 i;

			// Compute Axis Aligned Box extents
			for(i=0; i<m_uNumFaces; i++)
			{
				IFXU32Face pFace;
				m_pFaceList[i].GetFace(&pFace);

				U32 uIndex = pFace.Vertex(0);

				if( !uIndices[uIndex] )
				{
					if( pVertexList[uIndex][0] > m_vMax[0] ) m_vMax[0] = pVertexList[uIndex][0];
					if( pVertexList[uIndex][1] > m_vMax[1] ) m_vMax[1] = pVertexList[uIndex][1];
					if( pVertexList[uIndex][2] > m_vMax[2] ) m_vMax[2] = pVertexList[uIndex][2];

					if( pVertexList[uIndex][0] < m_vMin[0] ) m_vMin[0] = pVertexList[uIndex][0];
					if( pVertexList[uIndex][1] < m_vMin[1] ) m_vMin[1] = pVertexList[uIndex][1];
					if( pVertexList[uIndex][2] < m_vMin[2] ) m_vMin[2] = pVertexList[uIndex][2];
						
					uIndices[uIndex] = TRUE;
				}

				uIndex = pFace.Vertex(1);

				if( !uIndices[uIndex] )
				{
					if( pVertexList[uIndex][0] > m_vMax[0] ) m_vMax[0] = pVertexList[uIndex][0];
					if( pVertexList[uIndex][1] > m_vMax[1] ) m_vMax[1] = pVertexList[uIndex][1];
					if( pVertexList[uIndex][2] > m_vMax[2] ) m_vMax[2] = pVertexList[uIndex][2];

					if( pVertexList[uIndex][0] < m_vMin[0] ) m_vMin[0] = pVertexList[uIndex][0];
					if( pVertexList[uIndex][1] < m_vMin[1] ) m_vMin[1] = pVertexList[uIndex][1];
					if( pVertexList[uIndex][2] < m_vMin[2] ) m_vMin[2] = pVertexList[uIndex][2];
						
					uIndices[uIndex] = TRUE;
				}

				uIndex = pFace.Vertex(2);

				if( !uIndices[uIndex] )
				{
					if( pVertexList[uIndex][0] > m_vMax[0] ) m_vMax[0] = pVertexList[uIndex][0];
					if( pVertexList[uIndex][1] > m_vMax[1] ) m_vMax[1] = pVertexList[uIndex][1];
					if( pVertexList[uIndex][2] > m_vMax[2] ) m_vMax[2] = pVertexList[uIndex][2];

					if( pVertexList[uIndex][0] < m_vMin[0] ) m_vMin[0] = pVertexList[uIndex][0];
					if( pVertexList[uIndex][1] < m_vMin[1] ) m_vMin[1] = pVertexList[uIndex][1];
					if( pVertexList[uIndex][2] < m_vMin[2] ) m_vMin[2] = pVertexList[uIndex][2];
						
					uIndices[uIndex] = TRUE;
				}
			}

			// Compute box centroid
			m_vCentroid.Add(m_vMin, m_vMax);
			m_vCentroid.Scale(0.5f);

			// Compute box halfWidths
			m_vHalfWidth.Subtract(m_vMax, m_vMin );
			m_vHalfWidth.Scale(0.5f);

			// Deallocate uIndices memory
			IFXDELETE_ARRAY(uIndices);

			// If mesh contains less than IFX_MAX_BIN_SIZE triangles
			// we will not split any further
			if( m_uNumFaces <= IFX_MAX_BIN_SIZE ) 
				result = IFX_BV_SINGLE_TRI;
			else 
				result = IFX_BV_MULTI_TRI;
		}
	}
	else 
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::GetCentroid
//
//  Return the centroid of the bounding box as an IFXVector3.  This is a 
//  4-component vector vs. the 3-component IFXVector3
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::GetCentroid(IFXVector3 *pvCentroid)
{ 
	IFXRESULT result = IFX_OK;

	if( pvCentroid )
	{
		(*pvCentroid).X() = m_vCentroid.X();
		(*pvCentroid).Y() = m_vCentroid.Y();
		(*pvCentroid).Z() = m_vCentroid.Z();
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::GetHalfWidth
//
//  Return a pointer to the positive {x,y,z} halfWidths of the bounding box
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::GetHalfWidth(IFXVector3 *pvHalfWidth)
{ 
	IFXRESULT result = IFX_OK;

	if( pvHalfWidth ) 
		*pvHalfWidth = m_vHalfWidth;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::GetMaxPosition
//
//  Return the maximum {x,y,z} corner coordinate as an IFXVector3
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::GetMaxPosition(IFXVector3 *pvPosition)
{ 
	IFXRESULT result = IFX_OK;

	if( pvPosition )
	{
		(*pvPosition).X() = m_vMax.X();
		(*pvPosition).Y() = m_vMax.Y();
		(*pvPosition).Z() = m_vMax.Z();
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::GetMinPosition
//
//  Return the minimum {x,y,z} corner coordinate as IFXVector3
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::GetMinPosition(IFXVector3 *pvPosition)
{ 
	IFXRESULT result = IFX_OK;

	if( pvPosition )
	{
		(*pvPosition).X() = m_vMin.X();
		(*pvPosition).Y() = m_vMin.Y();
		(*pvPosition).Z() = m_vMin.Z();
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::Initialize
//
//  Initialize the axis aligned bounding box
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::Initialize(IFXBoundHierarchy *pHierarchy)
{
	IFXRESULT result = IFX_OK;

	// Set the parent hierarchy
	if( pHierarchy )
	{
		m_pBoundHierarchy = pHierarchy;

		// Initialize min/max box coordinates
		m_vMin.X() =  FLT_MAX; m_vMin.Y() =  FLT_MAX; m_vMin.Z() =  FLT_MAX;
		m_vMax.X() = -FLT_MAX; m_vMax.Y() = -FLT_MAX; m_vMax.Z() = -FLT_MAX;

		m_vCentroid.X() = 0;
		m_vCentroid.Y() = 0;
		m_vCentroid.Z() = 0;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::IntersectBound
//
//  Test for intersection of two axis aligned bounding boxes.  Boxes are
//  assumed to be from different CIFXBoundHierarchys
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::IntersectBound(IFXBoundVolume *pBound)
{
	IFXRESULT result = IFX_OK;

	if( pBound && m_pBoundHierarchy )
	{
		// Cast pBound to a CIFXAxisAlignedBBox - pBoxB
		CIFXAxisAlignedBBox* pBoxB = (CIFXAxisAlignedBBox *) pBound;

		// Get pBoxB's boundHierarchy
		IFXBoundHierarchy* pBoundHierarchyB = pBoxB->GetBoundHierarchy();

		if( pBoundHierarchyB )
		{
			IFXMatrix4x4 mTransposeMatrixA;
			IFXMatrix4x4 mUnscaledMatrixA;
			IFXMatrix4x4 mWorldMatrixA;
			IFXVector3   vTranslationA;
			IFXVector3   vScaleFactorA;
			IFXVector3   vHalfWidthA;
			IFXVector3   vNewCentroidA;

			//
			// BoxA
			//
			m_pBoundHierarchy->GetMatrixComponents(0, vTranslationA, vScaleFactorA, mUnscaledMatrixA);
			m_pBoundHierarchy->GetTransposeMatrix(0, mTransposeMatrixA);
			m_pBoundHierarchy->GetWorldMatrix(0, mWorldMatrixA);

			// Compute boxA's new centroid
			mWorldMatrixA.TransformVector(m_vCentroid, vNewCentroidA);

			// Compute correctly scaled halfwidth
			vHalfWidthA.Multiply(m_vHalfWidth, vScaleFactorA);

			//
			// BoxB
			//
			IFXMatrix4x4 mUnScaledMatrixB;
			IFXMatrix4x4 mWorldMatrixB;
			IFXVector3   vTranslationB;
			IFXVector3   vScaleFactorB;
			IFXVector3   vHalfWidthB;
			IFXVector3   vCentroidB;
			IFXVector3   vNewCentroidB;

			pBoundHierarchyB->GetMatrixComponents(1, vTranslationB, vScaleFactorB, mUnScaledMatrixB);
			pBoundHierarchyB->GetWorldMatrix(1, mWorldMatrixB);
			pBoxB->GetHalfWidth(&vHalfWidthB);
			pBoxB->GetCentroid(&vCentroidB);

			// Compute boxB's new centroid
			mWorldMatrixB.TransformVector(vCentroidB, vNewCentroidB);

			// Compute correctly scaled halfwidth
			vHalfWidthB.Multiply(vScaleFactorB);

			IFXMatrix4x4 mRotationBtoA;
			IFXVector3   vTranslateBtoA;
			IFXVector3   vTmp;

			// Compute the relative rotation and translation from B to A 
			vTmp.Subtract(vNewCentroidB, vNewCentroidA);

			mUnScaledMatrixB[12] = vTmp[0];
			mUnScaledMatrixB[13] = vTmp[1];
			mUnScaledMatrixB[14] = vTmp[2];

			mRotationBtoA.Multiply(mTransposeMatrixA, mUnScaledMatrixB);
			vTranslateBtoA.Set( mRotationBtoA[12],
								mRotationBtoA[13],
								mRotationBtoA[14] );

			// Do OBB/OBB overlap testing
			IFXMatrix4x4 MatrixABS;
			F32          fLeftSide;
			F32          fRightSide;

			MatrixABS[0]  = fabsf(mRotationBtoA[0])  + IFX_EPSILON;
			MatrixABS[1]  = fabsf(mRotationBtoA[1])  + IFX_EPSILON;
			MatrixABS[2]  = fabsf(mRotationBtoA[2])  + IFX_EPSILON;

			MatrixABS[4]  = fabsf(mRotationBtoA[4])  + IFX_EPSILON;
			MatrixABS[5]  = fabsf(mRotationBtoA[5])  + IFX_EPSILON;
			MatrixABS[6]  = fabsf(mRotationBtoA[6])  + IFX_EPSILON;

			MatrixABS[8]  = fabsf(mRotationBtoA[8])  + IFX_EPSILON;
			MatrixABS[9]  = fabsf(mRotationBtoA[9])  + IFX_EPSILON;
			MatrixABS[10] = fabsf(mRotationBtoA[10]) + IFX_EPSILON;

			result = IFX_TRUE;

			// Test 3 edges from ObbA
			fLeftSide  = vTranslateBtoA[0];
			fRightSide = vHalfWidthA[0] + (vHalfWidthB[0] * MatrixABS[0]) 
									    + (vHalfWidthB[1] * MatrixABS[1])									  
									    + (vHalfWidthB[2] * MatrixABS[2]);
			if( fLeftSide > fRightSide )
			{
				return IFX_FALSE;
			}

			fLeftSide  = vTranslateBtoA[1];
			fRightSide = vHalfWidthA[1] + (vHalfWidthB[0] * MatrixABS[4])
									    + (vHalfWidthB[1] * MatrixABS[5])								  
									    + (vHalfWidthB[2] * MatrixABS[6]);
			if( fLeftSide > fRightSide ) 
			{
				return IFX_FALSE;
			}

			fLeftSide  = vTranslateBtoA[2];
			fRightSide = vHalfWidthA[2] + (vHalfWidthB[0] * MatrixABS[8])
									    + (vHalfWidthB[1] * MatrixABS[9])									  
									    + (vHalfWidthB[2] * MatrixABS[10]);
			if( fLeftSide > fRightSide ) 
			{
				return IFX_FALSE;
			}

			// Test 3 edges from ObbB
			fLeftSide  = fabsf(vTranslateBtoA[0] * mRotationBtoA[0] + 
							   vTranslateBtoA[1] * mRotationBtoA[1] +
							   vTranslateBtoA[2] * mRotationBtoA[2]);
			fRightSide = vHalfWidthB[0] + (vHalfWidthA[0] * MatrixABS[0]) +
						 				  (vHalfWidthA[1] * MatrixABS[4]) +
										  (vHalfWidthA[2] * MatrixABS[8]);
			if( fLeftSide > fRightSide ) 
			{
				return IFX_FALSE;
			}

			fLeftSide  = fabsf(vTranslateBtoA[0] * mRotationBtoA[4] + 
							   vTranslateBtoA[1] * mRotationBtoA[5] +
							   vTranslateBtoA[2] * mRotationBtoA[6]);
			fRightSide = vHalfWidthB[1] + (vHalfWidthA[0] * MatrixABS[1]) +
				 		 				  (vHalfWidthA[1] * MatrixABS[5]) +
										  (vHalfWidthA[2] * MatrixABS[9]);
			if( fLeftSide > fRightSide ) 
			{
				return IFX_FALSE;
			}

			fLeftSide  = fabsf(vTranslateBtoA[0] * mRotationBtoA[8] + 
							   vTranslateBtoA[1] * mRotationBtoA[9] +
							   vTranslateBtoA[2] * mRotationBtoA[10]);
			fRightSide = vHalfWidthB[2] + (vHalfWidthA[0] * MatrixABS[2]) +
						 				  (vHalfWidthA[1] * MatrixABS[6]) +
										  (vHalfWidthA[2] * MatrixABS[10]);
			if( fLeftSide > fRightSide ) 
			{
				return IFX_FALSE;
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
// CIFXPrimitiveOverlap::IntersectBoxSphere
//
// Test for intersection between an oriented bounding box and a sphere.
//
// The code is based on Thomas Moeller's algorithm described in
// the book "Real-Time Rendering".
//
// The idea is to find the point on the OBB that is closest to the center of
// sphere.
// 
//    1. Translate the sphere into the modelSpace of the box
//    2. Use the AABB/Sphere overlap test
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::IntersectBoxSphere(IFXBoundVolume *pBound, 
												  F32             fSphereRadiusSquared,
												  IFXVector3&     vSphereCentroid,
												  IFXVector3&     vContactPoint, 
												  IFXVector3      vContactNormal[2])
{
	IFXRESULT result = IFX_OK;

	if( pBound )
	{
		IFXMatrix4x4 mUnscaledMatrixA;
		IFXMatrix4x4 mWorldMatrixA;
		IFXMatrix4x4 mTransposeMatrixA;
		IFXVector3   vTranslateBtoA;
		IFXVector3   vTranslationA;
		IFXVector3   vScaleFactorA;
		IFXVector3   vNewCentroidA;
		IFXVector3   vNewMinA;
		IFXVector3   vNewMaxA;
		IFXVector3   vTmp;

		m_pBoundHierarchy->GetMatrixComponents(0, vTranslationA, vScaleFactorA, mUnscaledMatrixA);
		m_pBoundHierarchy->GetTransposeMatrix(0, mTransposeMatrixA);
		m_pBoundHierarchy->GetWorldMatrix(0, mWorldMatrixA);

		// Compute boxA's new centroid
		// Compute boxA's min/max points with the correct scale
		vNewMinA.Multiply(m_vMin, vScaleFactorA);
		vNewMaxA.Multiply(m_vMax, vScaleFactorA);
		mWorldMatrixA.TransformVector(m_vCentroid, vNewCentroidA);

		// Transform the sphere's centroid into the Box's space
		vTmp.Subtract(vSphereCentroid, vNewCentroidA);
		mTransposeMatrixA.TransformVector(vTmp, vTranslateBtoA);

		// Test box/sphere overlap
		IFXVector3 vMSContactPoint;
		F32        fDistance = 0;
		F32        fTmp;
		U32        i;

		for(i=0; i<3; i++)
		{
			if( vTranslateBtoA[i] < vNewMinA[i] )
			{
				// Sphere center component closer box min extent
				fTmp       = vTranslateBtoA[i] - vNewMinA[i];	
				fDistance += fTmp * fTmp;

				vMSContactPoint[i] = vNewMinA[i];
			}
			else if( vTranslateBtoA[i] > vNewMaxA[i] )
			{
				// Sphere center component closer to box max extent
				fTmp       = vTranslateBtoA[i] - vNewMaxA[i];	
				fDistance += fTmp * fTmp;

				vMSContactPoint[i] = vNewMaxA[i];
			}
			else
			{
				// Sphere center component in between min/max extents
				vMSContactPoint[i] = vTranslateBtoA[i];
			}
		}

		if( fDistance > fSphereRadiusSquared )
			result = IFX_FALSE;
		else
		{
			// Compute contactPoint - Take model space CP and transform into world space
			mUnscaledMatrixA.TransformVector(vMSContactPoint, vContactPoint);
			vContactPoint.Add(vNewCentroidA);

			vContactNormal[0].Subtract(vContactPoint, vSphereCentroid);
			vContactNormal[0].Normalize();
			vContactNormal[1].Set( -vContactNormal[0].X(),
								   -vContactNormal[0].Y(),
								   -vContactNormal[0].Z() );
			result = IFX_TRUE;
		}
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::IntersectRay
//
//  Test if the ray from pOrigin in pDirection intersects the AABB
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::IntersectRay(IFXVector3 vOrigin, 
											IFXVector3 vDirection,
											U32        uPickType)
{
	IFXRESULT result = IFX_TRUE;

	if( m_pBoundHierarchy )
	{
		IFXMatrix4x4 mUnscaledMatrix;
		IFXMatrix4x4 mWorldMatrix;
		IFXVector3   vScaleFactor;
		IFXVector3   vHalfWidth;
		IFXVector3   vHLDirection;
		//IFXVector3   vMatrixTranslation;
		IFXVector3   vAxis;
		IFXVector3   vTranslation;
		IFXVector3   vNewCentroid;
		F32          fTMin = -FLT_MAX;
		F32          fTMax =  FLT_MAX;
		F32          fAxisDirection;
		F32          e;
		F32          fT1, fT2;
		U32          i;

		m_pBoundHierarchy->GetMatrixComponents(0, vTranslation, vScaleFactor, mUnscaledMatrix);
		m_pBoundHierarchy->GetWorldMatrix(0, mWorldMatrix);

		mWorldMatrix.TransformVector(m_vCentroid, vNewCentroid);
		vNewCentroid.Subtract(vOrigin);
		
		// Add scale factors:
		vHalfWidth.X() = m_vHalfWidth.X() * vScaleFactor.X();
		vHalfWidth.Y() = m_vHalfWidth.Y() * vScaleFactor.Y();
		vHalfWidth.Z() = m_vHalfWidth.Z() * vScaleFactor.Z();
		
		for(i=0; i<3 && result == IFX_TRUE; i++)
		{
			vHLDirection.Set(0,0,0);
			vHLDirection[i] = 1.0f;

			mUnscaledMatrix.TransformVector(vHLDirection, vAxis);
			vAxis.Normalize();								

			fAxisDirection = vAxis.DotProduct(vDirection);
	
			if( fabsf(fAxisDirection) > IFX_EPSILON )
			{
				// Take inverse of fAxisDirection
				fAxisDirection = 1.0f / fAxisDirection;

				e = vAxis.DotProduct(vNewCentroid);

				fT1 = (e + vHalfWidth[i]) * fAxisDirection;
				fT2 = (e - vHalfWidth[i]) * fAxisDirection;

				if( fT1 > fT2 ) 
					SwapFloats(fT1, fT2);
				if( fT1 > fTMin )
					fTMin = fT1;
				if( fT2 < fTMax )
					fTMax = fT2;

				if( (fTMin > fTMax) || (fTMax < 0) )
					result = IFX_FALSE;
			}
		}
	}
	else 
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::IntersectRayTriangle
//
//  Lowest level of the hierarchy has been reached.  Extract the triangle from
//  the bounding volume and test whether it intersects with the ray.
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::IntersectRayTriangle(IFXVector3 vOrigin, 
													IFXVector3 vDirection,
													U32        uPickType)
{
	IFXRESULT result = IFX_OK;

	if( m_pBoundHierarchy && m_pFaceList )
	{
		IFXVector3* pVertexList = m_pBoundHierarchy->GetVertexList();

		if( pVertexList )
		{
			CIFXPrimitiveOverlap overlap;
			IFXMatrix4x4         mMatrix;
			//IFXVector3 		     vTranslation;
			IFXVector3 		     vScaleFactor;
			IFXVector3           vNextVert;
			IFXVector3           vTFVerts[3];
			IFXU32Face           face;
			F32                  fU, fV;    // Barycentric coordinates
			F32	                 fT;        // Distance to triangle intersection point

			m_pBoundHierarchy->GetWorldMatrix(0, mMatrix);
			m_pBoundHierarchy->GetScaleFactor(0, vScaleFactor);
			
			// Walk over the number of faces in leafNodeA
			U32 i;
			for(i=0; i<m_uNumFaces; i++)
			{
				m_pFaceList[i].GetFace(&face);

				// Get vertex 0 from the vertexList and orient it
				vNextVert = pVertexList[face.Vertex(0)];
				mMatrix.TransformVector(vNextVert, vTFVerts[0]);

				// Get vertex 1 from the vertexList and orient it
				vNextVert = pVertexList[face.Vertex(1)];
				mMatrix.TransformVector(vNextVert, vTFVerts[1]);

				// Get vertex 0 from the vertexList and orient it
				vNextVert = pVertexList[face.Vertex(2)];
				mMatrix.TransformVector(vNextVert, vTFVerts[2]);

				result = overlap.RayTriangle( vTFVerts, 
											  vOrigin, 
											  vDirection, 
											  uPickType, 
											  &fU, 
											  &fV, 
											  &fT );

				if( result == IFX_TRUE )
				{
					m_pFaceList[0].SetIntersect(IFX_TRUE);

					// Set the mesh and face ID's in the boundHierarhcy result array
					m_pBoundHierarchy->SetResult( m_pFaceList[i].GetMeshID(), 
												  m_pFaceList[i].GetFaceID(), 
												  vTFVerts, 
												  fU, 
												  fV, 
												  fT );
				}
			}
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else 
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::IntersectTriangle
//
//  Test for intersection of triangles at the leaf node of these axis 
//  aligned bounding boxes
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::IntersectTriangle(IFXBoundVolume *pBound)
{
	IFXRESULT result = IFX_OK;

	if( pBound )
	{
		CIFXBoundFace* pFaceListB   = NULL;
		IFXVector3*    pVertexListA = NULL;
		IFXVector3*    pVertexListB = NULL;

		// Cast pBound to CIFXAxisAlignedBBox
		CIFXAxisAlignedBBox* pBoxB = (CIFXAxisAlignedBBox *) pBound;

		// Get boundHierarchyB
		IFXBoundHierarchy* pBoundHierarchyB = pBoxB->GetBoundHierarchy();

		if( pBoundHierarchyB )
		{
			// Get vertex and faceList associated with boundHierarchyB
			pVertexListA = m_pBoundHierarchy->GetVertexList();

			// Get vertex and faceList associated with boundHierarchyB
			pVertexListB = pBoundHierarchyB->GetVertexList();
			pFaceListB   = pBoxB->GetFaceList();

			if( !pVertexListA || !m_pFaceList || !pVertexListB || !pFaceListB )
				result = IFX_E_NOT_INITIALIZED;
		}
		else
			result = IFX_E_NOT_INITIALIZED;

		if( IFXSUCCESS(result) )
		{
			CIFXPrimitiveOverlap overlap;
			IFXOverlapTriangle   triangleA;
			IFXOverlapTriangle*  pTriangleB;
			IFXMatrix4x4         mMatrixA;
			IFXMatrix4x4         mMatrixB;
			IFXVector3           vNextVert;
			IFXVector3           vIntersectPoint[2];
			IFXVector3           vIntersectNormal[2];
			IFXU32Face           faceA;
			IFXU32Face           faceB;
			U32                  uNumFacesB = pBoxB->GetNumFaces();
			U32                  i, j;

			m_pBoundHierarchy->GetWorldMatrix(0, mMatrixA);
			pBoundHierarchyB->GetWorldMatrix(1, mMatrixB);

			// Allocate some memory for faceList associated with
			// pBoundHierarchyB.  This will alleviate the need to
			// transform the vertices at each test.
			pTriangleB = new IFXOverlapTriangle [uNumFacesB];

			if( !pTriangleB )
				result = IFX_E_OUT_OF_MEMORY;

			// Walk over the number of faces in leafNodeA
			for(i=0; i<m_uNumFaces && IFXSUCCESS(result); i++)
			{
				m_pFaceList[i].GetFace(&faceA);

				vNextVert = pVertexListA[faceA.Vertex(0)];
				mMatrixA.TransformVector(vNextVert, triangleA.v0);

				vNextVert = pVertexListA[faceA.Vertex(1)];
				mMatrixA.TransformVector(vNextVert, triangleA.v1);

				vNextVert = pVertexListA[faceA.Vertex(2)];
				mMatrixA.TransformVector(vNextVert, triangleA.v2);

				// Walk over the number of faces in leafNodeB
				for(j=0; j<uNumFacesB && IFXSUCCESS(result); j++)
				{
					if( i == 0 )
					{
						pFaceListB[j].GetFace(&faceB);

						vNextVert = pVertexListB[faceB.Vertex(0)];
						mMatrixB.TransformVector(vNextVert, pTriangleB[j].v0);

						vNextVert = pVertexListB[faceB.Vertex(1)];
						mMatrixB.TransformVector(vNextVert, pTriangleB[j].v1);

						vNextVert = pVertexListB[faceB.Vertex(2)];
						mMatrixB.TransformVector(vNextVert, pTriangleB[j].v2);
					}

					result = overlap.TriangleTriangle( triangleA, 
													   pTriangleB[j], 
													   vIntersectPoint, 
													   vIntersectNormal );
					if( result == IFX_TRUE )
					{
						m_pFaceList[i].SetIntersect(IFX_TRUE);
						pFaceListB[j].SetIntersect(IFX_TRUE);

						result = m_pBoundHierarchy->SetResult( 0, 
															   m_pFaceList[i].GetMeshID(), 
															   m_pFaceList[i].GetFaceID(), 
								  							   vIntersectPoint[0], 
															   vIntersectNormal[0] );
						if( IFXSUCCESS(result) )
						{
							result = pBoundHierarchyB->SetResult( 1, 
																  pFaceListB[j].GetMeshID(), 
																  pFaceListB[j].GetFaceID(), 
																  vIntersectPoint[1], 
																  vIntersectNormal[1] );
						}
					}
				}
			}

			IFXDELETE_ARRAY(pTriangleB);
		}		
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::SetFaceList
//
//  Set the faceList
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::SetFaceList(CIFXBoundFace **pFaceList)
{
	IFXRESULT result = IFX_OK;
	if( pFaceList )	
		m_pFaceList = *pFaceList;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::SetNumFaces
//
//  Set the number of CIFXBoundFaces contained within the AABB
//-----------------------------------------------------------------------------

void CIFXAxisAlignedBBox::SetNumFaces(U32 uNumFaces)
{ 
	m_uNumFaces = uNumFaces; 
}


//-----------------------------------------------------------------------------
// CIFXAxisAlignedBBox::SplitBound
//
// Split the axis aligned bounding box into 2 parts.  Ideally the split will
// result in two equal halves leading to a perfectly balanced tree.  In reality
// however is rarely the case.
//-----------------------------------------------------------------------------

IFXRESULT CIFXAxisAlignedBBox::SplitBound(CIFXAxisAlignedBBox **ppLeftBound, 
										  CIFXAxisAlignedBBox **ppRightBound)
{
	IFXRESULT result = IFX_OK;

	if( ppLeftBound && ppRightBound )
	{
		CIFXBoundFace *pTmpLeftList = NULL;
		CIFXBoundFace *pTmpRightList = NULL;
		CIFXBoundUtil  boundUtil;
		U32            uNumLeftFaces;
		U32			   uNumRightFaces;
		U32            uMedian;
		U32            i;

		if( m_pBoundHierarchy ) 
		{
			switch( m_pBoundHierarchy->GetSplitType() )
			{
			case IFXBoundHierarchy::MIDPOINT:
				result = boundUtil.SplitFaceList_Midpoint( m_pFaceList, 
														   &pTmpLeftList, 
														   &pTmpRightList, 
														   m_uNumFaces, 
														   &uNumLeftFaces, 
														   &uNumRightFaces, 
														   &m_vMin, 
														   &m_vMax );
				break;
			case IFXBoundHierarchy::MEDIAN:
				result = boundUtil.SplitFaceList_Median(m_uNumFaces, &uMedian);

				if( IFXSUCCESS(result) )
				{
					uNumLeftFaces  = uMedian;
					uNumRightFaces = m_uNumFaces - uMedian; 
				}
				break;
			case IFXBoundHierarchy::SORTED_MEDIAN:
				result = boundUtil.SplitFaceList_SortedMedian( m_pFaceList, 
															   m_uNumFaces, 
															   &m_vMin, 
															   &m_vMax, 
															   &uMedian );
				if( IFXSUCCESS(result) )
				{
					uNumLeftFaces  = uMedian;
					uNumRightFaces = m_uNumFaces - uMedian; 
				}		
				break;
			default:
				return IFX_E_UNDEFINED;
			}
		}
		else 
			result = IFX_E_UNDEFINED;

		if( IFXSUCCESS(result) )
		{
			// Create new bounding volumes
			(*ppLeftBound)  = new CIFXAxisAlignedBBox;

			if( !ppLeftBound ) 
				result = IFX_E_OUT_OF_MEMORY;
			else 
				result = IFX_OK;
		}

		if( IFXSUCCESS(result) )
		{
			(*ppRightBound) = new CIFXAxisAlignedBBox;

			if( !ppRightBound ) 
				result = IFX_E_OUT_OF_MEMORY;
			else 
				result = IFX_OK;
		}

		// Setup LEFT bound
		CIFXBoundFace *pLeftFaces = NULL;
		if( IFXSUCCESS(result) )
		{
			pLeftFaces = new CIFXBoundFace[uNumLeftFaces];

			if( !pLeftFaces ) 
				result = IFX_E_OUT_OF_MEMORY;
			else
			{
				result = (*ppLeftBound)->Initialize(m_pBoundHierarchy);

				if( IFXSUCCESS(result) ) 
					(*ppLeftBound)->SetNumFaces(uNumLeftFaces);
			}
		}

		// Setup RIGHT bound
		CIFXBoundFace *pRightFaces = NULL;
		if( IFXSUCCESS(result) )
		{
			pRightFaces = new CIFXBoundFace[uNumRightFaces];

			if( !pRightFaces ) 
				result = IFX_E_OUT_OF_MEMORY;
			else
			{
				result = (*ppRightBound)->Initialize(m_pBoundHierarchy);

				if( IFXSUCCESS(result) )
					(*ppRightBound)->SetNumFaces(uNumRightFaces);
			}
		}

		if( IFXSUCCESS(result) )
		{
			if( m_pBoundHierarchy->GetSplitType() == IFXBoundHierarchy::MIDPOINT )
			{
				// Fill the left face list
				for(i=0; i<uNumLeftFaces; i++) 
					pLeftFaces[i] = pTmpLeftList[i];
				
				// Fill the right face list
				for(i=0; i<uNumRightFaces; i++)	
					pRightFaces[i] = pTmpRightList[i];

				// Delete TmpFaceLists -- NOTE: must use the array operators here!!!
				IFXDELETE_ARRAY(pTmpLeftList);
				IFXDELETE_ARRAY(pTmpRightList);
			}
			else
			{
				// Fill the left face list
				for(i=0; i<uNumLeftFaces; i++) 
					pLeftFaces[i] = m_pFaceList[i];

				// Fill the right face list
				U32 uCount = 0;
				for(i=uMedian; i<m_uNumFaces; i++) 
					pRightFaces[uCount++] = m_pFaceList[i];
			}

			// Set the face list for each new bound
			result = (*ppLeftBound)->SetFaceList(&pLeftFaces);

			if( IFXSUCCESS(result) )
				result = (*ppRightBound)->SetFaceList(&pRightFaces);

			if( IFXSUCCESS(result) )
			{
				// Delete current node's pFaceList -- NOTE: must use the array operator!!!
				if( m_pFaceList )
					IFXDELETE_ARRAY(m_pFaceList);
			}
		}

		// Do final check for allocated memory in case an error code is present
		if( IFXFAILURE(result) )
		{
			if( pTmpRightList ) 
				IFXDELETE_ARRAY(pTmpRightList);
			if( pTmpLeftList  ) 
				IFXDELETE_ARRAY(pTmpLeftList);

			if( pRightFaces ) 
				IFXDELETE_ARRAY(pRightFaces);
			if( pLeftFaces  ) 
				IFXDELETE_ARRAY(pLeftFaces);
		}
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}
