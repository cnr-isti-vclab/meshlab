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
//	CIFXCollisionResult.cpp
//
//	DESCRIPTION
//		
//		Souce file for module CIFXCollisionResult.
//
//	NOTES
//      None.
//
//*****************************************************************************

//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXResultAllocator.h"

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
//	CIFXCollisionResult::CIFXCollisionResult
//
//  Constructor
//-----------------------------------------------------------------------------

CIFXCollisionResult::CIFXCollisionResult()
{
	m_fU        = -1.0f;
	m_fV        = -1.0f;
	m_fDistance = -1.0f;
	m_pNext     = NULL;

	m_vIntersectNormal.Set(0,0,1);
	m_vIntersectPoint.Set(0,0,0);
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::~CIFXCollisionResult
//
//  Destructor
//-----------------------------------------------------------------------------

CIFXCollisionResult::~CIFXCollisionResult()
{
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::FindFirstCollision
//
//  Return a pointer to the first CollisionResult.
//-----------------------------------------------------------------------------

CIFXCollisionResult * CIFXCollisionResult::FindFirstCollision()
{
	CIFXCollisionResult* pResult  = GetNext();
	CIFXCollisionResult* pClosest = this;

	F32 fNextDistance;
	F32 fDistance = m_fDistance;

	while( pResult )
	{
		fNextDistance = pResult->GetDistance();
		
		if( fNextDistance < fDistance )
		{
			fDistance = fNextDistance;
			pClosest  = pResult;
		}

		pResult = pResult->GetNext();
	}

	return pClosest;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::GetIntersectNormal
//
//  Return the intersection point.
//-----------------------------------------------------------------------------
void CIFXCollisionResult::GetIntersectNormal(IFXVector3& vIntersectNormal)
{
	vIntersectNormal = m_vIntersectNormal;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::GetIntersectPoint
//
//  Return the intersection point.
//-----------------------------------------------------------------------------
void CIFXCollisionResult::GetIntersectPoint(IFXVector3& vIntersectPoint)
{
	vIntersectPoint = m_vIntersectPoint;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::GetVertex
//
//  Return the indexed vertex - (Picking only)
//-----------------------------------------------------------------------------

void CIFXCollisionResult::GetVertices( IFXVector3& vVertex1, 
									   IFXVector3& vVertex2, 
									   IFXVector3& vVertex3 )

{
	vVertex1 = m_vVertex[0];
	vVertex2 = m_vVertex[1];
	vVertex3 = m_vVertex[2];
}


void CIFXCollisionResult::Initialize()
{
	m_fU = m_fV = m_fDistance = -1.0f;
	m_pNext = NULL;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::SetIDs
//
//  Set the mesh and face ID's to allow indexing into the IFXMeshGroup.
//-----------------------------------------------------------------------------

void CIFXCollisionResult::SetIDs(U32 uMeshID, U32 uFaceID)
{
	m_uMeshID = uMeshID;
	m_uFaceID = uFaceID;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::SetIntersectNormal
//
//  Set the normal at the intersection point.
//-----------------------------------------------------------------------------

void CIFXCollisionResult::SetIntersectNormal(IFXVector3& vIntersectNormal)
{
	m_vIntersectNormal = vIntersectNormal;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::SetIntersectPoint
//
//  Set the intersection point.
//-----------------------------------------------------------------------------

void CIFXCollisionResult::SetIntersectPoint(IFXVector3& vIntersectPoint)
{
	m_vIntersectPoint = vIntersectPoint;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::SetUVTCoords
//
//  Set the barycentric coordinates (U,V) plus distance to the point
//  of intersection (Distance).
//-----------------------------------------------------------------------------

void CIFXCollisionResult::SetUVTCoords(F32 fU, F32 fV, F32 fDistance)
{
	m_fU = fU;
	m_fV = fV;
	m_fDistance = fDistance;
}


//-----------------------------------------------------------------------------
//	CIFXCollisionResult::SetVertices
//
//  Set the vertices of the intersected triangle
//-----------------------------------------------------------------------------

void CIFXCollisionResult::SetVertices( IFXVector3& vVertex1, 
									   IFXVector3& vVertex2, 
									   IFXVector3& vVertex3 )
{
	m_vVertex[0] = vVertex1;
	m_vVertex[1] = vVertex2;
	m_vVertex[2] = vVertex3;
}
