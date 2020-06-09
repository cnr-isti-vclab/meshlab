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
//	CIFXPrimitiveOverlap.h
//
//	DESCRIPTION
//
//     Header file for class CIFXPrimitiveOverlap
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXPRIMITIVEOVERLAP_H__
#define __CIFXPRIMITIVEOVERLAP_H__

#include "IFXMatrix4x4.h"


struct IFXOverlapTriangle
{
	IFXVector3 v0, v1, v2;
};


class CIFXPrimitiveOverlap
{

public:

	CIFXPrimitiveOverlap();
	~CIFXPrimitiveOverlap();

	void      ClosestPointToBox( IFXVector3  vVertsBoxA[10], 
								 IFXVector3  vVertsBoxB[10],
								 IFXVector3& vClosestPoint,
								 IFXVector3& vContactNormal );

	void	  ComputeTriangleNormal( IFXVector3& vP1,
									 IFXVector3& vP2,
									 IFXVector3& vP3,
									 IFXVector3& vNormal );

	IFXRESULT Intersection2DLineSegment( IFXVector2* pOrigin1, 
										 IFXVector2* pDirection1,
									 	 IFXVector2* pOrigin2, 
										 IFXVector2* pDirection2,
										 IFXVector2* pIntersectionPoint );

	BOOL      Intersection3DLineSegment( IFXVector3& vP1, 
										 IFXVector3& vP2,
										 IFXVector3& vP3, 
										 IFXVector3& vP4,
										 IFXVector3& vApproachPoint,
										 F32&        fDistance );

	IFXRESULT ObbObb(IFXMatrix4x4& mRotationBtoA, 
					 IFXVector3&   vhalfWidthB,
					 IFXVector3&   vhalfWidthA);

	BOOL      PointInTriangle(IFXVector2 triangle[4]);

	BOOL      RaySphereIntersection(IFXVector4& vBoundingSphere,
									IFXVector3& vOrigin, 
									IFXVector3& vDirection);

	IFXRESULT RayTriangle(IFXVector3 vTriangle[3], 
						  IFXVector3 vOrigin, 
						  IFXVector3 vDirection,
						  U32        uPickType,
						  F32*       pfU, 
						  F32*       pfV, 
						  F32*       pfT);

	IFXRESULT ThreePlaneIntersection(IFXVector3* pP1, 
									 IFXVector3* pN1, 
									 IFXVector3* pP2, 
									 IFXVector3* pN2, 
									 IFXVector3* pP3, 
									 IFXVector3* pN3, 
								 	 IFXVector3* pIPoint);

	IFXRESULT TriangleTriangle( IFXOverlapTriangle  TriA, 
							    IFXOverlapTriangle  TriB,
					 		    IFXVector3          vIntersectPoint[2], 
							    IFXVector3          vIntersectNormal[2] );

	IFXRESULT TriangleTriangleERIT(IFXVector3  vTriA[3], 
								   IFXVector3  vTriB[3], 
				 				   IFXVector3* pvIntersectPoint,
								   IFXVector3* pvIntersectNormal, 
								   F32*        pfPenetrationDistance);

private:

	void ComputeBoxNormals( IFXVector3 vVertsBox[10], 
							IFXVector3 vNormal[6] );


	void ComputeVertexEdgeNormal( IFXVector3  vVerts[10],
								  IFXVector3  vNormalsBox[6],
								  IFXVector3& vPoint,
								  IFXVector3& vNewNormal,
								  F32&        fDistance );

	void ComputeVertexNormal( IFXVector3  vNormalsBox[6],
							  IFXVector3& vNewNormal,
							  U32         uVertex );

	// Test if a point is within a triangle
	BOOL ComputeIntervals(F32 fProjections[3], 
						  F32 fDotProduct[3], 
						  F32 fPosNeg[2], 
						  F32 fTmps[3], 
						  F32 fIsect[2]);

	IFXRESULT CoplanarTriangles(IFXVector3* pvNormal, 
								IFXVector3  vTriA[3], 
								IFXVector3 vTriB[3]);

	IFXRESULT ProjectedEdgeOverlap(IFXVector3* pvVA0, 
								   IFXVector3* pvVB0, 
								   IFXVector3* pvVB1, 
								   F32         fAx, 
								   F32         fAy, 
								   U32         uI0, 
								   U32         uI1);

	IFXRESULT PointInTri(IFXVector3* pvVA0, 
						 IFXVector3  vTriB[3], 
						 U32         uI0, 
						 U32         uI1);
};


#endif
