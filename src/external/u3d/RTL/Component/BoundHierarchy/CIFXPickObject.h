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
//	CIFXPickObject.h
//
//	DESCRIPTION
//
//		Header file for the class CIFXPickObject.
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXPickObject_H__
#define __CIFXPickObject_H__

#include "IFXPickObject.h"

class CIFXPickObject : public IFXPickObject
{
	CIFXPickObject ();
	virtual ~CIFXPickObject ();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXPickObject_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown Interface
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);

	// IFXPickObject
	CIFXPickObject& operator=( const CIFXPickObject& pickObj );

	void  IFXAPI 	GetIntersectNormal(IFXVector3& vIntersectNormal);
	
	void  IFXAPI 	GetIntersectPoint(IFXVector3& vIntersectPoint);

	IFXModel* IFXAPI GetModel();
	U32  IFXAPI 	GetInstance();

	void IFXAPI 	GetVertices(IFXVector3& v1,
								IFXVector3& v2, 
								IFXVector3& v3 );

	void IFXAPI 	SetIDs(U32 uMeshID, U32 uFaceID);

	void IFXAPI 	SetModel(IFXModel* pModel);
	void IFXAPI 	SetInstance(U32 Instance);

	void IFXAPI 	SetUVTCoords(F32 fU, F32 fV, F32 fDistance);

	void IFXAPI 	 SetVertices(	IFXVector3& v1, 
									IFXVector3& v2, 
									IFXVector3& v3 );

	F32  IFXAPI 	GetDistance() { return m_fDistance; }
	F32  IFXAPI 	GetUCoord()   { return m_fU; }
	F32  IFXAPI 	GetVCoord()   { return m_fV; }

	U32  IFXAPI 	GetMeshID()   { return m_uMeshID; }
	U32  IFXAPI 	GetFaceID()   { return m_uFaceID; }

private:

	// IFXUnknown Interface
	U32 m_uRefCount;

	IFXVector3 m_vIntersectPoint;
	IFXVector3 m_vIntersectNormal;
	IFXVector3 m_vTriVerts[3];

	IFXModel*  m_pModel;
	U32		m_Instance;

	BOOL       m_bComputed;
	U32        m_uMeshID;
	U32        m_uFaceID;
	F32        m_fU;
	F32        m_fV;
	F32        m_fDistance;
};


IFXINLINE CIFXPickObject& CIFXPickObject::operator=( const CIFXPickObject& pickObj )
{
	m_pModel           = pickObj.m_pModel;

	m_vIntersectPoint  = pickObj.m_vIntersectPoint;
	m_vIntersectNormal = pickObj.m_vIntersectNormal;
	m_vTriVerts[0]     = pickObj.m_vTriVerts[0];
	m_vTriVerts[1]     = pickObj.m_vTriVerts[1];
	m_vTriVerts[2]     = pickObj.m_vTriVerts[2];

	m_uMeshID          = pickObj.m_uMeshID;
	m_uFaceID          = pickObj.m_uFaceID;

	m_fU               = pickObj.m_fU;	
	m_fV               = pickObj.m_fV;
	m_fDistance        = pickObj.m_fDistance;

	return *this;
}


	// Inlines
IFXINLINE IFXModel* CIFXPickObject::GetModel()
{ 
	return m_pModel;
}


IFXINLINE void CIFXPickObject::SetModel(IFXModel* pModel) 
{ 
	m_pModel = pModel; 
}

///Addons to support Multiple parents
IFXINLINE U32 CIFXPickObject::GetInstance()
{ 
	return m_Instance;
}


IFXINLINE void CIFXPickObject::SetInstance(U32 Instance) 
{ 
	m_Instance = Instance; 
}

#endif
