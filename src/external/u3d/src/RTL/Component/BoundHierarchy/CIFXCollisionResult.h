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
//	CIFXCollisionResult.h
//
//	DESCRIPTION
//
//		Header file for the class CIFXCollisionResult.
//
//	NOTES
//
//***************************************************************************

#ifndef __CIFXCOLLISIONRESULT_H__
#define __CIFXCOLLISIONRESULT_H__

#include "IFXVector3.h"

class CIFXCollisionResult
{
public:

	CIFXCollisionResult();
	~CIFXCollisionResult();

	void GetIntersectNormal(IFXVector3& vIntersectNormal);
	void GetIntersectPoint(IFXVector3& vIntersectPoint);
	void GetVertices(IFXVector3& vVertex1, IFXVector3& vVertex2, IFXVector3& vVertex3);

	void SetIntersectNormal(IFXVector3& vIntersectNormal);
	void SetIntersectPoint(IFXVector3& vIntersectPoint);
	void SetVertices(IFXVector3& vVertex1, IFXVector3& vVertex2, IFXVector3& vVertex3);

	void Initialize();
	void SetIDs(U32 uMeshID, U32 uFaceID);
	void SetUVTCoords(F32 fU, F32 fV, F32 fDistance);

	CIFXCollisionResult * FindFirstCollision();

	// Inlines
	CIFXCollisionResult * GetNext() { return m_pNext; }
	F32 GetUCoord()   { return m_fU;        }
	F32 GetVCoord()   { return m_fV;        }
	F32 GetDistance() { return m_fDistance; }

	U32 GetFaceID()   { return m_uFaceID;   }
	U32 GetMeshID()   { return m_uMeshID;   }

	void SetNext(CIFXCollisionResult *pNext) { m_pNext = pNext; }

private:

	IFXVector3 m_vIntersectPoint;
	IFXVector3 m_vIntersectNormal;
	IFXVector3 m_vVertex[3];

	U32 m_uMeshID;
	U32 m_uFaceID;
	F32 m_fU;
	F32 m_fV;
	F32 m_fDistance;

	CIFXCollisionResult *m_pNext;
};

#endif
