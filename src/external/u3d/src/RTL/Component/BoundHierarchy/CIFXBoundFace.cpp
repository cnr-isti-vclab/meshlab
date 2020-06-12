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
//	CIFXBoundFace.cpp
//
//	DESCRIPTION
//	
//		Source file for CIFXBoundFace class.  Contains definitions for the
//		methods that manipulate a bounding face.	
//
//	NOTES
//
//*****************************************************************************
//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXBoundFace.h"

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

CIFXBoundFace::CIFXBoundFace()
{
}

CIFXBoundFace::~CIFXBoundFace()
{
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::GetCentroid
//
//  Return a pointer to the face centroid
//-----------------------------------------------------------------------------

void CIFXBoundFace::GetCentroid(IFXVector3& vCentroid)
{ 
	vCentroid = m_vCentroid;
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::GetCentroidComponent
//
//  Return the {x,y,z} component of the centroid
//-----------------------------------------------------------------------------

F32 CIFXBoundFace::GetCentroidComponent(U32 uIndex)
{ 
	IFXASSERT(uIndex <= 2);

	return m_vCentroid[uIndex];
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::SetCentroid
//
//  Set the face centroid
//-----------------------------------------------------------------------------

void CIFXBoundFace::SetCentroid(IFXVector3& vCentroid)
{ 
	m_vCentroid = vCentroid;
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::GetFace
//
//  Return a pointer to the IFXU32Face
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundFace::GetFace(IFXU32Face *pFace)
{ 
	IFXRESULT result = IFX_OK;

	if( pFace )
		*pFace = m_face;
	else 
		result = IFX_E_INVALID_POINTER;
	return result;
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::SetFace
//
//  Set the IFXU32Face
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundFace::SetFace(IFXU32Face *pFace)
{ 
	IFXRESULT result = IFX_OK;

	if( pFace )
		m_face = *pFace;
	else 
		result = IFX_E_INVALID_POINTER;
	return result;
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::GetMeshID
//
//  Return the MeshID from which this face comes
//-----------------------------------------------------------------------------

U32 CIFXBoundFace::GetMeshID()
{
	return m_uMeshID;
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::SetMeshID
//
//  Set the MeshID from which this face comes
//-----------------------------------------------------------------------------

void CIFXBoundFace::SetMeshID(U32 uMeshID) 
{ 
	m_uMeshID = uMeshID; 
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::GetFaceID
//
//  Return the FaceID from which this face comes
//-----------------------------------------------------------------------------

U32 CIFXBoundFace::GetFaceID()
{ 
	return m_uFaceID; 
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::SetFaceID
//
//  Set the FaceID from which this face comes - indexes into the faceList
//-----------------------------------------------------------------------------

void CIFXBoundFace::SetFaceID(U32 uFaceID) 
{ 
	m_uFaceID = uFaceID; 
}


//-----------------------------------------------------------------------------
//	CIFXBoundFace::GetIntersect
//
//  Return TRUE if the face is currently intersecting another face; else FALSE
//-----------------------------------------------------------------------------

BOOL CIFXBoundFace::GetIntersect()
{ 
	return m_bIntersect; 
}

	
//-----------------------------------------------------------------------------
//	CIFXBoundFace::SetIntersect
//
//  Set the face intersection flag
//-----------------------------------------------------------------------------

void CIFXBoundFace::SetIntersect(BOOL bFlag)
{ 
	m_bIntersect = bFlag; 
}
