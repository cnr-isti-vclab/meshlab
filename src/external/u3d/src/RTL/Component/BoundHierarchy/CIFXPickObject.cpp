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
//	CIFXPickObject.cpp
//
//	DESCRIPTION
//		
//		Source file for module CIFXPickObject.
//
//	NOTES
//      None.
//
//*****************************************************************************

//*****************************************************************************
//	Includes
//*****************************************************************************
#include "CIFXPickObject.h"


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
//	CIFXPickObject::CIFXPickObject
//
//  Constructor
//-----------------------------------------------------------------------------

CIFXPickObject::CIFXPickObject()
{
	// IFXUnknown
	m_uRefCount = 0;

	// CIFXPickObject
	m_pModel = NULL;
	m_bComputed = FALSE;
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::~CIFXPickObject
//
//  Destructor
//-----------------------------------------------------------------------------

CIFXPickObject::~CIFXPickObject()
{
	m_pModel = NULL;
}


IFXRESULT IFXAPI_CALLTYPE CIFXPickObject_Factory(IFXREFIID iid, void** ppv)
{
	IFXRESULT result = IFX_OK;

	if( ppv )	
	{
		// Create the CIFXBitStream component.
		CIFXPickObject *pComponent = new CIFXPickObject;

		if ( pComponent ) 
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( iid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		
		} 
		else 
			result = IFX_E_OUT_OF_MEMORY;
	} 
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown Interface
U32 CIFXPickObject::AddRef()
{
	
	return ++m_uRefCount;
}


U32 CIFXPickObject::Release()
{
	
	if( 1 == m_uRefCount )
	{
		delete this;
		return 0;
	}
	else
		return --m_uRefCount;
}


IFXRESULT CIFXPickObject::QueryInterface(IFXREFIID riid, void** ppv)
{
	IFXRESULT result = IFX_OK;

	if( ppv )
	{
		if ( riid == IID_IFXUnknown )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXPickObject )
			*ppv = (IFXPickObject*)this;
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS(result) )
			AddRef();
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::GetIntersectNormal
//
//  Return the intersection point.
//-----------------------------------------------------------------------------

void CIFXPickObject::GetIntersectNormal(IFXVector3& vIntersectNormal)
{
	vIntersectNormal = m_vIntersectNormal;
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::GetIntersectPoint
//
//  Return the intersection point.
//-----------------------------------------------------------------------------

void CIFXPickObject::GetIntersectPoint(IFXVector3& vIntersectPoint)
{
	if( !m_bComputed )
	{
		// Compute Intersection Point
		m_vIntersectPoint = m_vTriVerts[0].Scale(1.0f - m_fU - m_fV);
		m_vIntersectPoint.Add(m_vTriVerts[1].Scale(m_fU));
		m_vIntersectPoint.Add(m_vTriVerts[2].Scale(m_fV));

		m_bComputed = TRUE;
	}

	vIntersectPoint = m_vIntersectPoint;
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::GetVertices
//
//  Get the triangle vertices.
//-----------------------------------------------------------------------------

void CIFXPickObject::GetVertices( IFXVector3& v1, 
								  IFXVector3& v2, 
								  IFXVector3& v3 )
{
	v1 = m_vTriVerts[0];
	v2 = m_vTriVerts[1];
	v3 = m_vTriVerts[2];
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::SetIDs
//
//  Set the mesh and face ID's to allow indexing into the IFXMeshGroup.
//-----------------------------------------------------------------------------

void CIFXPickObject::SetIDs(U32 uMeshID, U32 uFaceID)
{
	m_uMeshID = uMeshID;
	m_uFaceID = uFaceID;
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::SetVertices
//
//  Set the triangle vertices.
//-----------------------------------------------------------------------------

void CIFXPickObject::SetVertices( IFXVector3& v1, 
								  IFXVector3& v2, 
								  IFXVector3& v3 )

{
	m_bComputed = FALSE;

	m_vTriVerts[0] = v1;
	m_vTriVerts[1] = v2;
	m_vTriVerts[2] = v3;

	v1.Subtract(m_vTriVerts[1], m_vTriVerts[0]);
	v2.Subtract(m_vTriVerts[2], m_vTriVerts[0]);

	m_vIntersectNormal.CrossProduct(v1, v2);
	m_vIntersectNormal.Normalize();
}


//-----------------------------------------------------------------------------
//	CIFXPickObject::SetUVTCoords
//
//  Set the barycentric coordinates (U,V) plus distance to the point
//  of intersection (Distance).
//-----------------------------------------------------------------------------

void CIFXPickObject::SetUVTCoords( F32 fU, 
								   F32 fV, 
								   F32 fDistance )
{
	m_bComputed = FALSE;

	m_fU = fU;
	m_fV = fV;
	m_fDistance = fDistance;
}
