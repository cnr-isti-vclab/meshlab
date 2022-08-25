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
//  CIFXPrimitiveOverlap.cpp
//
//  DESCRIPTION
//    A series of primitive - primative intersection tests

//  NOTES
//
//*****************************************************************************
//*****************************************************************************
//  Includes
//*****************************************************************************
#include <float.h>
#include "CIFXPrimitiveOverlap.h"

//*****************************************************************************
//  Defines
//*****************************************************************************
#define IFX_EPSILON               1e-6f
#define IFX_NEG_EPSILON          -1e-4f
#define IFX_DISTANCE_EPSILON      0.1f
#define IFX_DISTANCE_EPSILON2     0.15f
#define IFX_DISTANCE_EPSILON_FACE 0.05f
#define IFX_DEFAULT               666

#define ISECT(VV0, VV1, VV2, D0, D1, D2, isect0, isect1)    \
	isect0 = VV0 + (VV1 - VV0) * D0 / (D0 - D1);  \
	isect1 = VV0 + (VV2 - VV0) * D0 / (D0 - D2);

//*****************************************************************************
//  Constants
//*****************************************************************************

//*****************************************************************************
//  Enumerations
//*****************************************************************************

//*****************************************************************************
//  Global data
//*****************************************************************************

//*****************************************************************************
//  Local data
//*****************************************************************************

//*****************************************************************************
//  Classes, structures and types
//*****************************************************************************

//-----------------------------------------------------------------------------
//  CIFXPrimitiveOverlap::CIFXPrimitiveOverlap
//
//  Constructor
//-----------------------------------------------------------------------------

CIFXPrimitiveOverlap::CIFXPrimitiveOverlap()
{
}


//-----------------------------------------------------------------------------
//  CIFXPrimitiveOverlap::~CIFXPrimitiveOverlap
//
//  Destructor
//-----------------------------------------------------------------------------

CIFXPrimitiveOverlap::~CIFXPrimitiveOverlap()
{
}


//-----------------------------------------------------------------------------
// CIFXPrimitiveOverlap::ClosestPointToBox
//
// Compute the closest vertex of boxB to the boxA.
//-----------------------------------------------------------------------------

void CIFXPrimitiveOverlap::ClosestPointToBox( IFXVector3  vVertsBoxA[10],
											 IFXVector3  vVertsBoxB[10],
											 IFXVector3& vClosestPoint,
											 IFXVector3& vContactNormal )
{
	IFXVector3 vNormalA[6];
	IFXVector3 vNormalB[6];
	IFXVector3 vTmp;
	F32        fShortestDistance = FLT_MAX;
	F32        fNextDistance;
	U32        uCount = 1;
	U32        i, j;

	vClosestPoint.Reset();
	vContactNormal.Reset();

	// Compute the normals for each box
	ComputeBoxNormals(vVertsBoxA, vNormalA);
	ComputeBoxNormals(vVertsBoxB, vNormalB);

	// Compute the approximate point of contact
	//    a) Distance of each vertexA to boxB
	//    b) Distance of each vertexB to boxA

	for(i=0; i<6; i++)
	{
		for(j=0; j<9; j++)
		{
			if( j == 4 )
				continue;

			if( i < 3 )
				vTmp.Subtract(vVertsBoxA[j], vVertsBoxB[1]);
			else
				vTmp.Subtract(vVertsBoxA[j], vVertsBoxB[8]);

			fNextDistance = vTmp.DotProduct(vNormalB[i]);

			if( fNextDistance > IFX_EPSILON )
			{
				if( fabsf(fNextDistance - fShortestDistance) < IFX_DISTANCE_EPSILON2 )
				{
					vClosestPoint.Add(vVertsBoxA[j]);

					uCount ++;

				}
				else if( fNextDistance < fShortestDistance )
				{
					vClosestPoint = vVertsBoxA[j];
					fShortestDistance = fNextDistance;

					uCount = 1;
				}
			}
		}
	}

	for(i=0; i<6; i++)
	{
		for(j=0; j<9; j++)
		{
			if( j == 4 )
				continue;

			if( i < 3 )
				vTmp.Subtract(vVertsBoxB[j], vVertsBoxA[1]);
			else
				vTmp.Subtract(vVertsBoxB[j], vVertsBoxA[8]);

			fNextDistance = vTmp.DotProduct(vNormalA[i]);

			if( fNextDistance > IFX_EPSILON )
			{
				if( fabsf(fNextDistance - fShortestDistance) < IFX_DISTANCE_EPSILON2 )
				{
					vClosestPoint.Add(vVertsBoxB[j]);

					uCount ++;

				}
				else if( fNextDistance < fShortestDistance )
				{
					vClosestPoint = vVertsBoxB[j];
					fShortestDistance = fNextDistance;

					uCount = 1;
				}
			}
		}
	}

	// Average vClosestPoint
	vClosestPoint.Scale(1.0f / (F32)(uCount));

	//
	// Compute the collision normal - Find the shortest distance for
	// each of the cases below
	//
	// 1. Compute a vertex normal
	// 2. Compute an edge normal
	// 3. Compute a face normal


	IFXVector3 vNewNormal[6];
	F32        fDistance[] = { FLT_MAX, FLT_MAX, FLT_MAX,
		FLT_MAX, FLT_MAX, FLT_MAX };
	U32        uIndex = 0;

	// 1. Compute the distance and normal vector for the vertex the
	//    the point of contact is closest to.

	for(i=0; i<9; i++)
	{
		if( i == 4 )
			continue;

		fNextDistance = vClosestPoint.CalcDistanceFrom(vVertsBoxA[i]);
		if( fNextDistance < fDistance[0] )
		{
			fDistance[0] = fNextDistance;
			uIndex = i;
		}
	}

	ComputeVertexNormal( vNormalA, vNewNormal[0], uIndex );
	vNewNormal[0].Negate();

	for(i=0; i<9; i++)
	{
		if( i == 4 )
			continue;

		fNextDistance = vClosestPoint.CalcDistanceFrom(vVertsBoxB[i]);
		if( fNextDistance < fDistance[1] )
		{
			fDistance[1] = fNextDistance;
			uIndex = i;
		}
	}

	ComputeVertexNormal( vNormalB, vNewNormal[1], uIndex );


	// 2. Compute the distance and normal vector for the edge the
	//    the point of contact is closest to.

	ComputeVertexEdgeNormal( vVertsBoxA,
		vNormalA,
		vClosestPoint,
		vNewNormal[2],
		fDistance[2] );
	vNewNormal[2].Negate();

	ComputeVertexEdgeNormal( vVertsBoxB,
		vNormalB,
		vClosestPoint,
		vNewNormal[3],
		fDistance[3] );

	// 3. Compute the distance and normal vector for the face the
	//    the point of contact is closest to.

	for(i=0; i<6; i++)
	{
		if( i < 3 )
			vTmp.Subtract(vClosestPoint, vVertsBoxA[1]);
		else
			vTmp.Subtract(vClosestPoint, vVertsBoxA[8]);

		fNextDistance = fabsf(vTmp.DotProduct(vNormalA[i]));

		if( fNextDistance < IFX_DISTANCE_EPSILON_FACE )
		{
			if( fNextDistance < fDistance[4] )
			{
				fDistance[4]  = fNextDistance;
				vNewNormal[4] = vNormalA[i];
				vNewNormal[4].Negate();
			}
		}
	}

	for(i=0; i<6; i++)
	{
		if( i < 3 )
			vTmp.Subtract(vClosestPoint, vVertsBoxB[1]);
		else
			vTmp.Subtract(vClosestPoint, vVertsBoxB[8]);

		fNextDistance = fabsf(vTmp.DotProduct(vNormalB[i]));

		if( fNextDistance < IFX_DISTANCE_EPSILON_FACE )
		{
			if( fNextDistance < fDistance[5] )
			{
				fDistance[5]  = fNextDistance;
				vNewNormal[5] = vNormalB[i];
			}
		}
	}

	uIndex = 0;
	for(i=1; i<6; i++)
	{
		if( fDistance[i] < fDistance[uIndex] )
			uIndex = i;
	}

	// Check if the contanct point is very close to
	// a vertex as well as the plane.  If so then
	// use vertex normal.

	if( uIndex == 4 || uIndex == 5 )
	{
		if( fabsf(fDistance[2] - fDistance[uIndex]) < IFX_DISTANCE_EPSILON )
			uIndex = 2;
		if( fabsf(fDistance[3] - fDistance[uIndex]) < IFX_DISTANCE_EPSILON )
			uIndex = 3;
	}

	if( uIndex == 0 || uIndex == 1 )
	{
		if( fabsf(fDistance[4] - fDistance[uIndex]) < IFX_DISTANCE_EPSILON )
			uIndex = 4;
		if( fabsf(fDistance[5] - fDistance[uIndex]) < IFX_DISTANCE_EPSILON )
			uIndex = 5;
	}

	vContactNormal = vNewNormal[uIndex];
	vContactNormal.Normalize();
}


//-----------------------------------------------------------------------------
// CIFXPrimitiveOverlap::ComputeTriangleNormal
//
// Compute the normal of the the passed in triangle.
//-----------------------------------------------------------------------------

void CIFXPrimitiveOverlap::ComputeTriangleNormal( IFXVector3& vP1,
												 IFXVector3& vP2,
												 IFXVector3& vP3,
												 IFXVector3& vNormal)
{
	IFXVector3 vE1;
	IFXVector3 vE2;

	vE1.Subtract(vP1, vP2);
	vE2.Subtract(vP3, vP2);

	vNormal.CrossProduct(vE2, vE1);
	vNormal.Normalize();
}


//-----------------------------------------------------------------------------
// CIFXPrimitiveOverlap::LineIntersection2D
//
// Compute the intersection of 2 line segments in 2-space.  If a valid
// intersection exists, pass back the point of intersection.
//-----------------------------------------------------------------------------

IFXRESULT CIFXPrimitiveOverlap::Intersection2DLineSegment(IFXVector2 *pOrigin1, IFXVector2 *pDirection1,
														  IFXVector2 *pOrigin2, IFXVector2 *pDirection2,
														  IFXVector2 *pIntersectionPoint)
{
	IFXRESULT result = IFX_TRUE;

	if( pOrigin1 && pDirection1 && pOrigin1 && pDirection1 && pIntersectionPoint )
	{
		IFXVector2 direction1;
		IFXVector2 perpDot1;

		direction1.Subtract((*pDirection1), (*pOrigin1));
		perpDot1[0] = -direction1[1];
		perpDot1[1] =  direction1[1];
		F32 fLength1 = direction1.CalcMagnitude();
		direction1.Normalize();

		IFXVector2 direction2;
		IFXVector2 perpDot2;

		direction2.Subtract((*pDirection2), (*pOrigin2));
		perpDot2[0] = -direction2[1];
		perpDot2[1] =  direction2[1];
		F32 fLength2 = direction2.CalcMagnitude();
		direction2.Normalize();

		F32 s, t;

		// Compute scale factors s,t:
		F32 denominator1 = direction1.DotProduct(perpDot2);
		F32 denominator2 = direction2.DotProduct(perpDot1);

		if( (fabsf(denominator1) > IFX_EPSILON) && (fabsf(denominator2) > IFX_EPSILON) )
		{
			IFXVector2 originDiff;

			// Compute s
			originDiff.Subtract((*pOrigin2), (*pOrigin1));
			s = originDiff.DotProduct(perpDot2) / denominator1;

			// Compute t
			originDiff.Subtract((*pOrigin1), (*pOrigin2));
			t = originDiff.DotProduct(perpDot1) / denominator2;

			if( s >= 0.0f && s <= fLength1 && t >= 0.0f && t <= fLength2 )
				(*pIntersectionPoint) = (*pOrigin1).Add(direction1.Scale(s));
		}
		else result = IFX_FALSE;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//-----------------------------------------------------------------------------
// CIFXPrimitiveOverlap::LineIntersection3D
//
// Compute the intersection of 2 line segments in 3-space.  If a valid
// intersection exists, pass back the point of intersection.
//
//   Calculate the line segment PaPb that is the shortest route between
//   two lines SaEa and SbEb. Calculate also the values of mua and mub where
//      Pa = P1 + a (P2 - P1)
//      Pb = P3 + b (P3 - P4)
//-----------------------------------------------------------------------------

BOOL CIFXPrimitiveOverlap::Intersection3DLineSegment( IFXVector3& vP1,
													 IFXVector3& vP2,
													 IFXVector3& vP3,
													 IFXVector3& vP4,
													 IFXVector3& vApproachPoint,
													 F32&        fDistance )
{
	IFXVector3 vD43;
	vD43.Subtract(vP4, vP3);

	if( fabsf(vD43[0]) < IFX_EPSILON &&
		fabsf(vD43[1]) < IFX_EPSILON &&
		fabsf(vD43[2]) < IFX_EPSILON )
	{
		return FALSE;
	}

	IFXVector3 vD21;
	vD21.Subtract(vP2, vP1);

	if( fabsf(vD21[0]) < IFX_EPSILON &&
		fabsf(vD21[1]) < IFX_EPSILON &&
		fabsf(vD21[2]) < IFX_EPSILON )
	{
		return FALSE;
	}

	IFXVector3 vD13;
	vD13.Subtract(vP1, vP3);

	F32 fDot1        = vD43.DotProduct(vD21);
	F32 fDot2        = vD43.DotProduct(vD43);
	F32 fDot3        = vD21.DotProduct(vD21);
	F32 fDenominator = fDot3 * fDot2 - fDot1 * fDot1;

	if( fabsf(fDenominator) < IFX_EPSILON )
		return FALSE;

	F32 fDot4      = vD13.DotProduct(vD43);
	F32 fDot5      = vD13.DotProduct(vD21);
	F32 fNumerator = fDot4 * fDot1 - fDot5 * fDot2;

	F32 fParamA = fNumerator / fDenominator;
	F32 fParamB = ((fDot4 + fDot1) * fParamA) / fDot2;

	IFXVector3 vPa, vPb;

	vPa[0] = vP1[0] + fParamA * vD21[0];
	vPa[1] = vP1[1] + fParamA * vD21[1];
	vPa[2] = vP1[2] + fParamA * vD21[2];

	vPb[0] = vP3[0] + fParamB * vD43[0];
	vPb[1] = vP3[1] + fParamB * vD43[1];
	vPb[2] = vP3[2] + fParamB * vD43[2];


	// Average the center of the line segment to get the
	// point of closest approach between the lines
	vApproachPoint[0] = (vPa[0] + vPb[0]) * 0.5f;
	vApproachPoint[1] = (vPa[1] + vPb[1]) * 0.5f;
	vApproachPoint[2] = (vPa[2] + vPb[2]) * 0.5f;

	// Compute the distance between vPa and vPb
	fDistance = vPa.CalcDistanceFrom(vPb);

	return TRUE;
}


//-----------------------------------------------------------------------------
//  CIFXPrimitiveOverlap::ObbObb
//
//  Test for overlap between two oriented bounding boxes.
//
//  The overlap test is based on Stefan Gottschalk's algorithm as presented in
//  "OBBTree: A Hierarchical Structure for Rapid Interference Detection,"
//  Computer Graphics (SIGGRAPH Proceedings 1996).
//
//-----------------------------------------------------------------------------

IFXRESULT CIFXPrimitiveOverlap::ObbObb(IFXMatrix4x4& mRotationBtoA,
									   IFXVector3&   vHalfWidthB,
									   IFXVector3&   vHalfWidthA)
{
	IFXMatrix4x4 MatrixABS;
	IFXVector3   vTranslateBtoA;
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

	vTranslateBtoA.Set(mRotationBtoA[12], mRotationBtoA[13], mRotationBtoA[14]);

	// Test 3 edges from ObbA
	fLeftSide  = vTranslateBtoA[0];
	fRightSide = vHalfWidthA[0] + (vHalfWidthB[0] * MatrixABS[0])
		+ (vHalfWidthB[1] * MatrixABS[1])
		+ (vHalfWidthB[2] * MatrixABS[2]);
	if( fLeftSide > fRightSide )
	{
		//m_bMarked = FALSE;
		//pBoxB->SetMarked(FALSE);

		return IFX_FALSE;
	}

	fLeftSide  = vTranslateBtoA[1];
	fRightSide = vHalfWidthA[1] + (vHalfWidthB[0] * MatrixABS[4])
		+ (vHalfWidthB[1] * MatrixABS[5])
		+ (vHalfWidthB[2] * MatrixABS[6]);
	if( fLeftSide > fRightSide )
	{
		//m_bMarked = FALSE;
		//pBoxB->SetMarked(FALSE);

		return IFX_FALSE;
	}

	fLeftSide  = vTranslateBtoA[2];
	fRightSide = vHalfWidthA[2] + (vHalfWidthB[0] * MatrixABS[8])
		+ (vHalfWidthB[1] * MatrixABS[9])
		+ (vHalfWidthB[2] * MatrixABS[10]);
	if( fLeftSide > fRightSide )
	{
		//m_bMarked = FALSE;
		//pBoxB->SetMarked(FALSE);

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
		//m_bMarked = FALSE;
		//pBoxB->SetMarked(FALSE);

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
		//m_bMarked = FALSE;
		//pBoxB->SetMarked(FALSE);

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
		//m_bMarked = FALSE;
		//pBoxB->SetMarked(FALSE);

		return IFX_FALSE;
	}

	return IFX_TRUE;
}


//-----------------------------------------------------------------------------
//  CIFXPrimitiveOverlap::RaySphereIntersection
//
//  Test for intersection of a direction ray with a bounding sphere.
//-----------------------------------------------------------------------------

BOOL CIFXPrimitiveOverlap::RaySphereIntersection(IFXVector4& vBoundingSphere,
												 IFXVector3& vOrigin,
												 IFXVector3& vDirection)
{
	IFXVector3 vOriginToSphere;
	F32         fRadiusSquared;
	F32         fProjDir;
	F32         fOriginToSphere2;
	F32         fSquaredDistance;

	// Compute the squared radius of the bounding sphere
	fRadiusSquared = vBoundingSphere[3] * vBoundingSphere[3];

	// Compute the vector from the ray origin to the center of the sphere
	vOriginToSphere[0] = vBoundingSphere[0] - vOrigin[0];
	vOriginToSphere[1] = vBoundingSphere[1] - vOrigin[1];
	vOriginToSphere[2] = vBoundingSphere[2] - vOrigin[2];

	// Projet computed vector onto ray direction
	fProjDir = vOriginToSphere.DotProduct(vDirection);

	fOriginToSphere2 = vOriginToSphere.DotProduct(vOriginToSphere);

	if( fProjDir < 0 && fOriginToSphere2 > fRadiusSquared )
		return FALSE;

	// Test squaredDistance against squaredRadius
	fSquaredDistance = fOriginToSphere2 - (fProjDir * fProjDir);

	if( fSquaredDistance > fRadiusSquared )
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
//  CIFXPrimitiveOverlap::RayTriangle
//
//  Test for intersection of a ray with pOrigin/pDirection and a triangle.
//  If an intersection occurs, the barycentric coordinates and the distance from
//  the origin to the intersection point are returned.
//
//  The algorithm is based on the technique presented by Thomas Moeller and
//  Ben Trumbore in "Fast, Minimum Storage Ray-Triangle Intersection,"
//  Journal of Graphics Tools, volume 2. number 1.
//-----------------------------------------------------------------------------

IFXRESULT CIFXPrimitiveOverlap::RayTriangle(IFXVector3 vTriangle[3],
											IFXVector3 vOrigin,
											IFXVector3 vDirection,
											U32         uPickType,
											F32*        pfU,
											F32*        pfV,
											F32*        pfT)
{
	IFXRESULT result = IFX_TRUE;

	if( pfU && pfV && pfT )
	{
		// Do some initialization in case there is no intersection
		*pfU = *pfV = *pfT = 0.0f;

		IFXVector3 vEdge1, vEdge2;
		IFXVector3 vp;

		vEdge1.Subtract(vTriangle[1], vTriangle[0]);
		vEdge2.Subtract(vTriangle[2], vTriangle[0]);

		// Compute the determinant
		vp.CrossProduct(vDirection, vEdge2);

		F32 fDet = vEdge1.DotProduct(vp);

		switch( uPickType )
		{
		case 0:
			// Picking turned off
			return IFX_FALSE;
		case 1:
			{
				// If fDet is < 0 the triangle is back facing
				if( fDet < IFX_EPSILON )
					return IFX_FALSE;

				// Compute the distance from vertex 0 to ray origin
				IFXVector3 vs;
				vs.Subtract(vOrigin, vTriangle[0]);

				// Compute U
				*pfU = vs.DotProduct(vp);

				if( *pfU < IFX_NEG_EPSILON || *pfU > fDet )
					return IFX_FALSE;

				// Compute V
				IFXVector3 vq;
				vq.CrossProduct(vs, vEdge1);

				*pfV = vDirection.DotProduct(vq);

				if( *pfV < IFX_NEG_EPSILON || ((*pfU)+(*pfV)) > fDet )
					return IFX_FALSE;

				// Compute the distance to the point of intersection
				*pfT = vEdge2.DotProduct(vq);

				if( (*pfT) < IFX_EPSILON )
					return IFX_FALSE;

				F32 fInverseDet = 1.0f / fDet;

				// Scale the parameters by the inverseDeterminant
				*pfT *= fInverseDet;
				*pfU *= fInverseDet;
				*pfV *= fInverseDet;

				break;
			}

		case 2:
		case 3:
			{
				if( fDet > -IFX_EPSILON && fDet < IFX_EPSILON )
					return IFX_FALSE;

				F32 fInverseDet = 1.0f / fDet;

				// Compute the distance from vertex 0 to ray origin
				IFXVector3 vs;
				vs.Subtract(vOrigin, vTriangle[0]);

				// Compute pfU and test bounds
				*pfU = vs.DotProduct(vp) * fInverseDet;

				if( *pfU < IFX_NEG_EPSILON || *pfU > 1.0f )
					return IFX_FALSE;

				// Compute pfU and test bounds
				IFXVector3 vq;
				vq.CrossProduct(vs, vEdge1);

				*pfV = vDirection.DotProduct(vq) * fInverseDet;

				if( *pfV < IFX_NEG_EPSILON || ((*pfU)+(*pfV)) > 1.0f )
					return IFX_FALSE;

				// Compute the distance to the point of intersection
				*pfT = vEdge2.DotProduct(vq) * fInverseDet;

				// Make sure we have a positive distance
				if( (*pfT) < IFX_EPSILON )
					return IFX_FALSE;

				// The value of the determinant indicates if the triangle is
				// FRONT or BACK facing.
				if( uPickType == 2 && fDet >= IFX_EPSILON )
					return IFX_FALSE;

				break;
			}
		default:
			IFXASSERT(0);
			break;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXPrimitiveOverlap::ThreePlaneIntersection
//
// Compute the intersection of 3 planes in 3-space.
//-----------------------------------------------------------------------------

IFXRESULT CIFXPrimitiveOverlap::ThreePlaneIntersection(IFXVector3 *pP1, IFXVector3 *pN1,
													   IFXVector3 *pP2, IFXVector3 *pN2,
													   IFXVector3 *pP3, IFXVector3 *pN3,
													   IFXVector3 *pIPoint)
{
	IFXRESULT result = IFX_OK;

	if( pP1 && pN1 && pP2 && pN2 && pP3 && pN3 && pIPoint )
	{
		F32 fDeterminant = pN1->X() * (pN2->Y() * pN3->Z() - pN3->Y() * pN2->Z()) -
			pN1->Y() * (pN2->X() * pN3->Z() - pN3->X() * pN2->Z()) +
			pN1->Z() * (pN2->X() * pN3->Y() - pN3->X() * pN2->Y());

		if( fabsf(fDeterminant) > IFX_EPSILON )
		{
			IFXVector3 n12Cross, n23Cross, n31Cross;
			F32 pN1Dot, pN2Dot, pN3Dot;

			n12Cross.CrossProduct(*pN1, *pN2);
			n23Cross.CrossProduct(*pN2, *pN3);
			n31Cross.CrossProduct(*pN3, *pN1);

			pN1Dot = pP1->DotProduct(*pN1);
			pN2Dot = pP2->DotProduct(*pN2);
			pN3Dot = pP3->DotProduct(*pN3);

			n23Cross.Scale(pN1Dot);
			n31Cross.Scale(pN2Dot);
			n12Cross.Scale(pN3Dot);

			pIPoint->Add(n23Cross, n31Cross);
			pIPoint->Add(n12Cross);
			pIPoint->Scale(1.0f / fDeterminant);
		}
		else
		{
			pIPoint = NULL;
			result = IFX_E_UNDEFINED;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//  CIFXPrimitiveOverlap::TriangleTriangle
//
//  Test for intersection between 2 triangles.
//
//  The code is based on Thomas Moeller's technique described in
//  the book "Real-Time Rendering".
//
//-----------------------------------------------------------------------------

IFXRESULT CIFXPrimitiveOverlap::TriangleTriangle( IFXOverlapTriangle  TriA,
												 IFXOverlapTriangle  TriB,
												 IFXVector3          vIntersectPoint[2],
												 IFXVector3          vIntersectNormal[2] )
{
	///  @todo: I have found an addendum published by
	///  Moeller which now implements the code without divisions!
	IFXVector3 Edge1, Edge2, vTriANormal;

	// Compute the plane equation for triA
	Edge1.Subtract(TriA.v1, TriA.v0);
	Edge2.Subtract(TriA.v2, TriA.v0);

	vTriANormal.CrossProduct(Edge1, Edge2);

	F32 d1 = - vTriANormal.DotProduct(TriA.v0);

	// Substitute vTriB into plane equation to compute distances to plane
	F32 du[3];
	du[0] = vTriANormal.DotProduct(TriB.v0) + d1;
	du[1] = vTriANormal.DotProduct(TriB.v1) + d1;
	du[2] = vTriANormal.DotProduct(TriB.v2) + d1;

	if( fabsf(du[0]) < IFX_EPSILON ) du[0] = 0;
	if( fabsf(du[1]) < IFX_EPSILON ) du[1] = 0;
	if( fabsf(du[2]) < IFX_EPSILON ) du[2] = 0;

	F32 du0du[2];
	du0du[0] = du[0] * du[1];
	du0du[1] = du[0] * du[2];

	// If same sign on all of them and not equal 0: NO INTERSECTION
	if( du0du[0] > 0 && du0du[1] > 0 )
		return IFX_FALSE;

	IFXVector3 vTriBNormal;
	F32         dv[3];
	F32         dv0dv[2];
	F32         d2;

	// Compute the plane equation for vTriB
	Edge1.Subtract(TriB.v1, TriB.v0);
	Edge2.Subtract(TriB.v2, TriB.v0);

	vTriBNormal.CrossProduct(Edge1, Edge2);

	d2 = - vTriBNormal.DotProduct(TriB.v0);

	// Substitute vTriA into plane equation to compute distances
	// to the plane

	dv[0] = vTriBNormal.DotProduct(TriA.v0) + d2;
	dv[1] = vTriBNormal.DotProduct(TriA.v1) + d2;
	dv[2] = vTriBNormal.DotProduct(TriA.v2) + d2;

	if( fabsf(dv[0]) < IFX_EPSILON ) dv[0] = 0.0f;
	if( fabsf(dv[1]) < IFX_EPSILON ) dv[1] = 0.0f;
	if( fabsf(dv[2]) < IFX_EPSILON ) dv[2] = 0.0f;

	dv0dv[0] = dv[0] * dv[1];
	dv0dv[1] = dv[0] * dv[2];

	// If same sign on all of them and not equal 0: NO INTERSECTION
	if( dv0dv[0] > 0 && dv0dv[1] > 0 )
		return IFX_FALSE;

	IFXVector3 lineDirection;

	// Compute direction of the line intersecting the two planes
	lineDirection.CrossProduct(vTriANormal, vTriBNormal);

	// Compute index to the largest component of D
	F32 max[3];
	max[0] = fabsf(lineDirection.X());
	max[1] = fabsf(lineDirection.Y());
	max[2] = fabsf(lineDirection.Z());

	U32 uIndex = 0;
	if( max[1] > max[uIndex] ) uIndex = 1;
	if( max[2] > max[uIndex] ) uIndex = 2;

	// Projection onto line L
	F32 vp[3];
	vp[0] = TriA.v0[uIndex];
	vp[1] = TriA.v1[uIndex];
	vp[2] = TriA.v2[uIndex];

	F32 up[3];
	up[0] = TriB.v0[uIndex];
	up[1] = TriB.v1[uIndex];
	up[2] = TriB.v2[uIndex];

	F32 tmpA[3], x[2];
	F32 tmpB[3], y[2];

	ComputeIntervals(vp, dv, dv0dv, tmpA, x);
	ComputeIntervals(up, du, du0du, tmpB, y);

	F32 fIntervalA[2], fIntervalB[2];
	fIntervalA[0] = x[0];
	fIntervalA[1] = x[1];
	fIntervalB[0] = y[0];
	fIntervalB[1] = y[1];

	if( fIntervalA[0] > fIntervalA[1] )
	{
		F32 tmp = fIntervalA[0];
		fIntervalA[0] = fIntervalA[1];
		fIntervalA[1] = tmp;
	}

	if( fIntervalB[0] > fIntervalB[1] )
	{
		F32 tmp = fIntervalB[0];
		fIntervalB[0] = fIntervalB[1];
		fIntervalB[1] = tmp;
	}

	if( fIntervalA[1] < fIntervalB[0] || fIntervalB[1] < fIntervalA[0] )
		return IFX_FALSE;

	vIntersectPoint[0] = TriA.v0;
	vIntersectPoint[0].Add(TriA.v1);
	vIntersectPoint[0].Add(TriA.v2);

	vIntersectPoint[1] = TriB.v0;
	vIntersectPoint[1].Add(TriB.v1);
	vIntersectPoint[1].Add(TriB.v2);

	vIntersectNormal[0] = vTriANormal;
	vIntersectNormal[1] = vTriBNormal;

	return IFX_TRUE;
}


//-----------------------------------------------------------------------------
// CIFXPrimitiveOverlap::TriangleTriangleERIT
//
// Test for intersection between 2 triangles.
//
// Notes:
//
// The code is based on ERIT technique described in
// the book "Real-Time Rendering" pp. 317-318.
//-----------------------------------------------------------------------------

IFXRESULT CIFXPrimitiveOverlap::TriangleTriangleERIT(IFXVector3 vTriA[3], IFXVector3 vTriB[3],
													 IFXVector3 *pvIntersectPoint,
													 IFXVector3 *pvIntersectNormal,
													 F32 *pfPenetrationDistance)
{
	IFXRESULT result = IFX_TRUE;

	if( pvIntersectPoint && pvIntersectNormal && pfPenetrationDistance )
	{
		IFXVector3 vVector1, vVector2, vTriBNormal;

		// Compute the plane equation for vTriB
		vVector1.Subtract(vTriB[1], vTriB[0]);
		vVector2.Subtract(vTriB[2], vTriB[0]);

		vTriBNormal.CrossProduct(vVector1, vVector2);
		vTriBNormal.Normalize();

		F32 d1 = - vTriBNormal.DotProduct(vTriB[0]);

		// Substitute vTriB into plane equation to compute distances to plane
		F32 du[3];
		F32 fduABS[3];

		du[0] = vTriBNormal.DotProduct(vTriA[0]) + d1;
		du[1] = vTriBNormal.DotProduct(vTriA[1]) + d1;
		du[2] = vTriBNormal.DotProduct(vTriA[2]) + d1;


		if( (fduABS[0] = fabsf(du[0])) < IFX_EPSILON ) du[0] = 0.0f;
		if( (fduABS[1] = fabsf(du[1])) < IFX_EPSILON ) du[1] = 0.0f;
		if( (fduABS[2] = fabsf(du[2])) < IFX_EPSILON ) du[2] = 0.0f;

		F32 du0du[2];
		du0du[0] = du[0] * du[1];
		du0du[1] = du[0] * du[2];

		// If same sign on all of them and not equal 0: NO INTERSECTION
		if( du0du[0] > 0.0f && du0du[1] > 0.0f ) result = IFX_FALSE;

		if( result == IFX_TRUE )
		{
			U32 uSameSide[2];
			U32 uDiffSide;

			// Find which vertex has opposite sign
			if( (du[0] > 0.0f && du[1] > 0.0f) || (du[0] < 0.0f && du[1] < 0.0f) )
			{
				uSameSide[0] = 0; uSameSide[1] = 1; uDiffSide = 2;
			}
			else if( (du[0] > 0.0f && du[2] > 0.0f) || (du[0] < 0.0f && du[2] < 0.0f) )
			{
				uSameSide[0] = 0; uSameSide[1] = 2; uDiffSide = 1;
			}
			else
			{
				uSameSide[0] = 1; uSameSide[1] = 2; uDiffSide = 0;
			}

			// Compute the points of possible intersection: p,q
			IFXVector3 iPointP;

			iPointP.Subtract(vTriA[uSameSide[0]], vTriA[uDiffSide]);
			iPointP.Scale((du[uDiffSide] / (du[uDiffSide] - du[uSameSide[0]])));
			iPointP.Add(vTriA[uDiffSide]);

			IFXVector3 iPointQ;
			iPointQ.Subtract(vTriA[uSameSide[1]], vTriA[uDiffSide]);
			iPointQ.Scale((du[uDiffSide] / (du[uDiffSide] - du[uSameSide[1]])));
			iPointQ.Add(vTriA[uDiffSide]);

			// Find the component that will maximize projected area
			U32 uIndex = 0;
			U32 uOther[] = {1,2};
			if( fabsf(vTriBNormal[1]) > fabsf(vTriBNormal[uIndex]) ) { uIndex = 1; uOther[0] = 0; uOther[1] = 2; }
			if( fabsf(vTriBNormal[2]) > fabsf(vTriBNormal[uIndex]) ) { uIndex = 2; uOther[0] = 0; uOther[1] = 1; }

			// Project iPointP on the coordinate plane
			IFXVector2 iPointPProj;
			iPointPProj[0] = iPointP[uOther[0]];
			iPointPProj[1] = iPointP[uOther[1]];

			// Project vTriB on the coordinate plane and translate to origin
			IFXVector2 vTriBProj[4];
			vTriBProj[0][0] = vTriB[0][uOther[0]] - iPointPProj[0];
			vTriBProj[0][1] = vTriB[0][uOther[1]] - iPointPProj[1];
			vTriBProj[1][0] = vTriB[1][uOther[0]] - iPointPProj[0];
			vTriBProj[1][1] = vTriB[1][uOther[1]] - iPointPProj[1];
			vTriBProj[2][0] = vTriB[2][uOther[0]] - iPointPProj[0];
			vTriBProj[2][1] = vTriB[2][uOther[1]] - iPointPProj[1];
			vTriBProj[3]    = vTriBProj[0];

			// Test if point P is inside the triangle
			BOOL pInside = PointInTriangle(vTriBProj);

			// Project iPointQ on the coordinate plane
			IFXVector2 iPointQProj;
			iPointQProj[0] = iPointQ[uOther[0]];
			iPointQProj[1] = iPointQ[uOther[1]];

			// Project vTriB on the coordinate plane and translate to origin
			vTriBProj[0][0] = vTriB[0][uOther[0]] - iPointQProj[0];
			vTriBProj[0][1] = vTriB[0][uOther[1]] - iPointQProj[1];
			vTriBProj[1][0] = vTriB[1][uOther[0]] - iPointQProj[0];
			vTriBProj[1][1] = vTriB[1][uOther[1]] - iPointQProj[1];
			vTriBProj[2][0] = vTriB[2][uOther[0]] - iPointQProj[0];
			vTriBProj[2][1] = vTriB[2][uOther[1]] - iPointQProj[1];
			vTriBProj[3]    = vTriBProj[0];

			// Test if point Q is inside the triangle
			BOOL qInside = PointInTriangle(vTriBProj);

			// Must check four states at this point:
			//
			// 1. P in  -- Q in
			// 2. P in  -- Q out
			// 3. P out -- Q in
			// 4. P out -- Q out

			F32 fDistance;
			(*pfPenetrationDistance) = 0;
			(*pvIntersectNormal) = vTriBNormal;

			if( pInside && qInside )
			{
				IFXVector3 vTmp;

				vTmp.Subtract(vTriA[0], vTriB[1]);
				fDistance = vTriBNormal.DotProduct(vTmp);
				if( fDistance < (*pfPenetrationDistance) )
				{
					(*pfPenetrationDistance) = fDistance;
					(*pvIntersectPoint) = vTriA[0];
				}

				vTmp.Subtract(vTriA[1], vTriB[1]);
				fDistance = vTriBNormal.DotProduct(vTmp);
				if( fDistance < (*pfPenetrationDistance) )
				{
					(*pfPenetrationDistance) = fDistance;
					(*pvIntersectPoint) = vTriA[1];
				}

				vTmp.Subtract(vTriA[2], vTriB[1]);
				fDistance = vTriBNormal.DotProduct(vTmp);
				if( fDistance < (*pfPenetrationDistance) )
				{
					(*pfPenetrationDistance) = fDistance;
					(*pvIntersectPoint) = vTriA[2];
				}

				(*pfPenetrationDistance) = fabsf(*pfPenetrationDistance);

				result = IFX_TRUE;
			}
			else if( pInside && !qInside )
			{
				// P inside, Q outside triangle: Need to find the intersection point
				// of the triangle edge the line segment from P->Q crosses

				U32 i;
				F32 fDistance;
				for(i=0; i<3; i++)
				{
					result = Intersection3DLineSegment(iPointP, iPointQ, vTriB[i],
						vTriB[(i+1)%3], *pvIntersectPoint,
						fDistance);
					if( result == IFX_TRUE )
					{
						(*pvIntersectPoint) = iPointP;

						// Compute the penetration distance from intersected
						// triangle edge to iPointP
						IFXVector3 vLineSegment;
						vLineSegment.Subtract(iPointP, (*pvIntersectPoint));
						(*pfPenetrationDistance) = vLineSegment.CalcMagnitude();

						break;
					}
				}
			}
			else if( !pInside && qInside )
			{
				// Q inside, P outside triangle: Need to find the intersection point
				// of the triangle edge the line segment from Q->P crosses

				F32 fDistance;
				U32 i;
				for(i=0; i<3; i++)
				{
					result = Intersection3DLineSegment(iPointQ, iPointP, vTriB[i],
						vTriB[(i+1)%3], *pvIntersectPoint,
						fDistance);
					if( result == IFX_TRUE )
					{
						(*pvIntersectPoint) = iPointQ;

						// Compute the penetration distance from intersected
						// triangle edge to iPointP
						IFXVector3 vLineSegment;
						vLineSegment.Subtract(iPointQ, (*pvIntersectPoint));
						(*pfPenetrationDistance) = vLineSegment.CalcMagnitude();

						break;
					}
				}
			}
			else
			{
				// Both P,Q are outside the triangle. Need to test each line segment
				// of vTriBProj against linesegment: iPointPProj *-------* iPointQProj
				// If an intersection is found then we have to find where the line
				// croses the edges in 3-space.

				IFXVector3 iP1, iP2, iP3;
				U32 uCount = 0;
				F32 fDistance;

				if( IFX_TRUE == Intersection3DLineSegment(iPointP, iPointQ, vTriB[0], vTriB[1], iP1, fDistance) )
					uCount ++;
				if( IFX_TRUE == Intersection3DLineSegment(iPointP, iPointQ, vTriB[1], vTriB[2], iP2, fDistance) )
					uCount ++;
				if( IFX_TRUE == Intersection3DLineSegment(iPointP, iPointQ, vTriB[2], vTriB[0], iP3, fDistance) )
					uCount ++;

				if( uCount < 2 ) result = IFX_FALSE;
				else
				{
					IFXVector3 vTmp;

					vTmp.Subtract(vTriA[0], vTriB[1]);
					fDistance = vTriBNormal.DotProduct(vTmp);
					if( fDistance < (*pfPenetrationDistance) )
					{
						(*pfPenetrationDistance) = fDistance;
						(*pvIntersectPoint) = vTriA[0];
					}

					vTmp.Subtract(vTriA[1], vTriB[1]);
					fDistance = vTriBNormal.DotProduct(vTmp);
					if( fDistance < (*pfPenetrationDistance) )
					{
						(*pfPenetrationDistance) = fDistance;
						(*pvIntersectPoint) = vTriA[1];
					}

					vTmp.Subtract(vTriA[2], vTriB[1]);
					fDistance = vTriBNormal.DotProduct(vTmp);
					if( fDistance < (*pfPenetrationDistance) )
					{
						(*pfPenetrationDistance) = fDistance;
						(*pvIntersectPoint) = vTriA[2];
					}

					(*pfPenetrationDistance) = fabsf(*pfPenetrationDistance);

					result = IFX_TRUE;
				}
			}
		}
	}
	else
		result = IFX_E_UNDEFINED;

	return result;
}


//
// Private Methods
//

void CIFXPrimitiveOverlap::ComputeBoxNormals( IFXVector3 vVertsBox[10],
											 IFXVector3 vNormal[6] )
{
	IFXVector3 vE1, vE2;

	// Top
	vE1.Subtract(vVertsBox[0], vVertsBox[1]);
	vE2.Subtract(vVertsBox[2], vVertsBox[1]);
	vNormal[0].CrossProduct(vE2, vE1);
	vNormal[0].Normalize();

	// Front
	vE1.Subtract(vVertsBox[0], vVertsBox[1]);
	vE2.Subtract(vVertsBox[6], vVertsBox[1]);
	vNormal[1].CrossProduct(vE1, vE2);
	vNormal[1].Normalize();

	// Right
	vE1.Subtract(vVertsBox[1], vVertsBox[2]);
	vE2.Subtract(vVertsBox[7], vVertsBox[2]);
	vNormal[2].CrossProduct(vE1, vE2);
	vNormal[2].Normalize();

	// Back
	vE1.Subtract(vVertsBox[2], vVertsBox[3]);
	vE2.Subtract(vVertsBox[8], vVertsBox[3]);
	vNormal[3].CrossProduct(vE1, vE2);
	vNormal[3].Normalize();

	// Left
	vE1.Subtract(vVertsBox[3], vVertsBox[0]);
	vE2.Subtract(vVertsBox[5], vVertsBox[0]);
	vNormal[4].CrossProduct(vE1, vE2);
	vNormal[4].Normalize();

	// Bottom
	vE1.Subtract(vVertsBox[7], vVertsBox[6]);
	vE2.Subtract(vVertsBox[5], vVertsBox[6]);
	vNormal[5].CrossProduct(vE2, vE1);
	vNormal[5].Normalize();
}


BOOL CIFXPrimitiveOverlap::ComputeIntervals(F32 fProjVert[3], F32 fDist[3],
											F32 fPosNeg[2], F32 fTmps[3],
											F32 fIsect[2])
{
	if( fPosNeg[0] > 0.0f )
	{
		ISECT(fProjVert[2], fProjVert[0], fProjVert[1], fDist[2], fDist[0],
			fDist[1], fIsect[0], fIsect[1]);
	}
	else if( fPosNeg[1] > 0.0f )
	{
		ISECT(fProjVert[1], fProjVert[0], fProjVert[2], fDist[1], fDist[0],
			fDist[2], fIsect[0], fIsect[1]);
	}
	else if( ((fDist[1] * fDist[2]) > 0.0f) || fDist[0] != 0.0f )
	{
		ISECT(fProjVert[0], fProjVert[1], fProjVert[2], fDist[0], fDist[1],
			fDist[2], fIsect[0], fIsect[1]);
	}
	else if( fDist[1] != 0.0f )
	{
		ISECT(fProjVert[1], fProjVert[0], fProjVert[2], fDist[1], fDist[0],
			fDist[2], fIsect[0], fIsect[1]);
	}
	else if( fDist[2] != 0.0f )
	{
		ISECT(fProjVert[2], fProjVert[0], fProjVert[1], fDist[2], fDist[0],
			fDist[1], fIsect[0], fIsect[1]);
	}

	return TRUE;
}


void CIFXPrimitiveOverlap::ComputeVertexEdgeNormal( IFXVector3  vVerts[10],
												   IFXVector3  vNormalsBox[6],
												   IFXVector3& vPoint,
												   IFXVector3& vNewNormal,
												   F32&        fDistance )
{
	// Compute the distance of vClosestPoint to edge edge of each box

	IFXVector3 v, w;
	BOOL       bVertex = FALSE;
	BOOL       bEdgeType = TRUE;
	F32        fShortestDistance = FLT_MAX;
	F32        fNewDistance;
	F32        c1, c2;
	U32        uVertex = IFX_DEFAULT;
	U32        uEdge[3] = {0, 0, 0};
	U32        uOffset = 0;
	U32        uStart = 0, uEnd = 0;
	U32        uVertexFinal = 0;
	U32        i, j;

	for(i=0; i<3; i++)
	{
		if( i == 0 )
		{
			uStart  = 0;
			uEnd    = 4;
			uOffset = 1;
		}
		else if( i == 1 )
		{
			uStart  = 0;
			uEnd    = 5;
			uOffset = 5;
		}
		else
		{
			uStart  = 5;
			uEnd    = 9;
			uOffset = 1;
		}

		for(j=uStart;j<uEnd; j++)
		{
			// Top
			v.Subtract(vVerts[j+uOffset], vVerts[j]);
			w.Subtract(vPoint, vVerts[j]);

			c1 = v.DotProduct(w);

			if( c1 <= 0.0f )
			{
				fNewDistance = vPoint.CalcDistanceFrom(vVerts[j]);
				uVertex = j;
				bVertex = TRUE;
			}
			else
			{
				c2 = v.DotProduct(v);

				if( c2 <= c1 )
				{
					fNewDistance = vPoint.CalcDistanceFrom(vVerts[j+uOffset]);
					uVertex = j+uOffset;
					bVertex = TRUE;
				}
				else
				{
					F32 b = c1 / c2;

					IFXVector3 vPb = vVerts[j];
					vPb.Add(v.Scale(b));

					fNewDistance = vPoint.CalcDistanceFrom(vPb);
					bVertex = FALSE;
				}
			}

			if( fNewDistance < fShortestDistance )
			{
				fShortestDistance = fNewDistance;
				uEdge[0] = i;
				uEdge[1] = j;
				uEdge[2] = j+uOffset;

				if( !bVertex )
					bEdgeType = TRUE;
				else
				{
					uVertexFinal = uVertex;
					bEdgeType = FALSE;
				}
			}
		}
	}

	if( bEdgeType )
	{
		if( uEdge[0] == 0 )
		{
			if( uEdge[1] == 0 || uEdge[2] == 1 )
				vNewNormal.Add(vNormalsBox[0], vNormalsBox[1]);
			else if( uEdge[1] == 1 || uEdge[2] == 2 )
				vNewNormal.Add(vNormalsBox[0], vNormalsBox[2]);
			else if( uEdge[1] == 2 || uEdge[2] == 3 )
				vNewNormal.Add(vNormalsBox[0], vNormalsBox[3]);
			else if( uEdge[1] == 3 || uEdge[2] == 4 )
				vNewNormal.Add(vNormalsBox[0], vNormalsBox[4]);
			else
				IFXASSERT(0);
		}
		else if( uEdge[0] == 1 )
		{
			if( uEdge[1] == 0 || uEdge[2] == 5 )
				vNewNormal.Add(vNormalsBox[4], vNormalsBox[1]);
			else if( uEdge[1] == 1 || uEdge[2] == 6 )
				vNewNormal.Add(vNormalsBox[1], vNormalsBox[2]);
			else if( uEdge[1] == 2 || uEdge[2] == 7 )
				vNewNormal.Add(vNormalsBox[2], vNormalsBox[3]);
			else if( uEdge[1] == 3 || uEdge[2] == 8 )
				vNewNormal.Add(vNormalsBox[3], vNormalsBox[4]);
			else
				IFXASSERT(0);
		}
		else
		{
			if( uEdge[1] == 5 || uEdge[2] == 6 )
				vNewNormal.Add(vNormalsBox[5], vNormalsBox[1]);
			else if( uEdge[1] == 6 || uEdge[2] == 7 )
				vNewNormal.Add(vNormalsBox[5], vNormalsBox[2]);
			else if( uEdge[1] == 7 || uEdge[2] == 8 )
				vNewNormal.Add(vNormalsBox[5], vNormalsBox[3]);
			else if( uEdge[1] == 8 || uEdge[2] == 9 )
				vNewNormal.Add(vNormalsBox[5], vNormalsBox[4]);
			else
				IFXASSERT(0);
		}
	}
	else
	{
		ComputeVertexNormal( vNormalsBox, vNewNormal, uVertexFinal );
	}

	fDistance = fShortestDistance;
}


void CIFXPrimitiveOverlap::ComputeVertexNormal( IFXVector3  vNormalsBox[6],
											   IFXVector3& vNewNormal,
											   U32         uVertex )
{
	switch( uVertex )
	{
	case 0:
	case 4:
		vNewNormal.Add(vNormalsBox[0], vNormalsBox[1]);
		vNewNormal.Add(vNormalsBox[4]);
		break;
	case 1:
		vNewNormal.Add(vNormalsBox[0], vNormalsBox[1]);
		vNewNormal.Add(vNormalsBox[2]);
		break;
	case 2:
		vNewNormal.Add(vNormalsBox[0], vNormalsBox[2]);
		vNewNormal.Add(vNormalsBox[3]);
		break;
	case 3:
		vNewNormal.Add(vNormalsBox[0], vNormalsBox[4]);
		vNewNormal.Add(vNormalsBox[4]);
		break;
	case 5:
	case 9:
		vNewNormal.Add(vNormalsBox[5], vNormalsBox[1]);
		vNewNormal.Add(vNormalsBox[4]);
		break;
	case 6:
		vNewNormal.Add(vNormalsBox[5], vNormalsBox[1]);
		vNewNormal.Add(vNormalsBox[4]);
		break;
	case 7:
		vNewNormal.Add(vNormalsBox[5], vNormalsBox[2]);
		vNewNormal.Add(vNormalsBox[3]);
		break;
	case 8:
		vNewNormal.Add(vNormalsBox[5], vNormalsBox[3]);
		vNewNormal.Add(vNormalsBox[4]);
		break;
	default:
		IFXASSERT(0);
	}
}



IFXRESULT CIFXPrimitiveOverlap::CoplanarTriangles(IFXVector3 *pvNormal, IFXVector3 vTriA[3],
												  IFXVector3 vTriB[3])
{
	IFXRESULT result = IFX_OK;

	if( pvNormal )
	{
		// Find largest normal Component
		U32 uIndex = 0,
			uI0    = 1,
			uI1    = 2;

		if( (*pvNormal)[1] > (*pvNormal)[uIndex] )
		{
			uIndex = 1;
			uI0    = 0,
				uI1    = 2;
		}
		if( (*pvNormal)[2] > (*pvNormal)[uIndex] )
		{
			uI0    = 0,
				uI1    = 1;
		}

		// Test all edges of vTriA against the edges of vTriB
		F32 fAx = vTriA[1][uI0] - vTriA[0][uI0];
		F32 fAy = vTriA[1][uI1] - vTriA[0][uI1];

		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[0], &vTriB[0], &vTriB[1], fAx, fAy, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[0], &vTriB[1], &vTriB[2], fAx, fAy, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[0], &vTriB[2], &vTriB[0], fAx, fAy, uI0, uI1) ) return IFX_TRUE;

		fAx = vTriA[2][uI0] - vTriA[1][uI0];
		fAy = vTriA[2][uI1] - vTriA[1][uI1];

		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[1], &vTriB[0], &vTriB[1], fAx, fAy, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[1], &vTriB[1], &vTriB[2], fAx, fAy, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[1], &vTriB[2], &vTriB[0], fAx, fAy, uI0, uI1) ) return IFX_TRUE;

		fAx = vTriA[0][uI0] - vTriA[2][uI0];
		fAy = vTriA[0][uI1] - vTriA[2][uI1];

		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[2], &vTriB[0], &vTriB[1], fAx, fAy, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[2], &vTriB[1], &vTriB[2], fAx, fAy, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == ProjectedEdgeOverlap(&vTriA[2], &vTriB[2], &vTriB[0], fAx, fAy, uI0, uI1) ) return IFX_TRUE;

		if( IFX_TRUE == PointInTri(&vTriA[0], vTriB, uI0, uI1) ) return IFX_TRUE;
		if( IFX_TRUE == PointInTri(&vTriB[0], vTriA, uI0, uI1) ) return IFX_TRUE;

		result = IFX_FALSE;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXPrimitiveOverlap::ProjectedEdgeOverlap(IFXVector3 *pvVA0, IFXVector3 *pvVB0,
													 IFXVector3 *pvVB1, F32 fAx, F32 fAy,
													 U32 uI0, U32 uI1)
{
	IFXRESULT result = IFX_FALSE;

	if( pvVA0 && pvVB0 && pvVB1 )
	{
		F32 fBx = (*pvVB0)[uI0] - (*pvVB1)[uI0];
		F32 fBy = (*pvVB0)[uI1] - (*pvVB1)[uI1];
		F32 fCx = (*pvVA0)[uI0] - (*pvVB0)[uI0];
		F32 fCy = (*pvVA0)[uI1] - (*pvVB0)[uI1];

		F32 fF = fAy * fBx - fAx * fBy,
			fD = fBy * fCx - fBx * fCy;

		if( (fF > 0.0f && fD >= 0.0f && fD <= fF) ||
			(fF < 0.0f && fD <= 0.0f && fD >= fF) )
		{
			F32 fE = fAx * fCy - fAy * fCx;

			if( fF > 0.0f )
			{
				if( fE >= 0.0f && fE <= fF ) result = IFX_TRUE;
			}
			else
			{
				if( fE <= 0.0f && fE >= fF ) result = IFX_TRUE;
			}
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXPrimitiveOverlap::PointInTri(IFXVector3 *pvVA0, IFXVector3 vTriB[3], U32 uI0, U32 uI1)
{
	IFXRESULT result = IFX_OK;

	if( pvVA0 )
	{
		F32 fA, fB, fC, fD0, fD1, fD2;

		fA  = vTriB[1][uI1]       - vTriB[0][uI1];
		fB  = -(vTriB[1][uI0]     - vTriB[0][uI0]);
		fC  = -fA * vTriB[0][uI0] - fB * vTriB[0][uI1];
		fD0 = fA * (*pvVA0)[uI0]  + fB * (*pvVA0)[uI1] + fC;

		fA  = vTriB[2][uI1]       - vTriB[1][uI1];
		fB  = -(vTriB[2][uI0]     - vTriB[1][uI0]);
		fC  = -fA * vTriB[1][uI0] - fB * vTriB[1][uI1];
		fD1 = fA * (*pvVA0)[uI0]  + fB * (*pvVA0)[uI1] + fC;

		fA  = vTriB[0][uI1]       - vTriB[2][uI1];
		fB  = -(vTriB[0][uI0]     - vTriB[2][uI0]);
		fC  = -fA * vTriB[2][uI0] - fB * vTriB[2][uI1];
		fD2 = fA * (*pvVA0)[uI0]  + fB * (*pvVA0)[uI1] + fC;

		if( (fD0 * fD1) > 0.0 && (fD0 * fD2) > 0.0f ) result = IFX_TRUE;
		else result = IFX_FALSE;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


BOOL CIFXPrimitiveOverlap::PointInTriangle(IFXVector2 triangle[4])
{
	BOOL inside = FALSE;

	U32 k;
	for( k=0; k<4; k++)
	{
		if( ((triangle[k].Y() >= 0) && (triangle[k+1].Y() <  0)) ||
			((triangle[k].Y() <  0) && (triangle[k+1].Y() >= 0)) )
		{
			if( (triangle[k].X() >= 0) && (triangle[k+1].X() >= 0) )
				inside = !inside;

			else if( ((triangle[k].X() >= 0) && (triangle[k+1].X() < 0)) ||
				((triangle[k].X() < 0)  && (triangle[k+1].X() >= 0)) )
			{
				F32 tmp_val = (triangle[k+1].X() - triangle[k].X()) /
					(triangle[k+1].Y() - triangle[k].Y());
				if( (triangle[k].X() - (triangle[k].Y() * tmp_val)) > 0.0001f )
					inside = !inside;
			}
		}
	}

	return inside;
}
