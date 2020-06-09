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

/**
	@file	CIFXBoundFace.h

			Header file for the class CIFXBoundFace.  Contains the declarations
			for and methods that manipulate a face (i.e. triangle) contained in
			the bounding volume.
*/

#ifndef __CIFXBOUNDFACE_H__
#define __CIFXBOUNDFACE_H__

#include "IFXFace.h"
#include "IFXVector3.h"

class CIFXBoundFace
{
public:
	
	CIFXBoundFace();
	~CIFXBoundFace();

	void      GetCentroid(IFXVector3& vCentroid);
	F32       GetCentroidComponent(U32 uIndex);
	IFXRESULT GetFace(IFXU32Face* pFace);
	U32       GetFaceID();
	BOOL      GetIntersect();
	U32       GetMeshID();
	void      SetCentroid(IFXVector3& vCentroid);
	IFXRESULT SetFace(IFXU32Face* pFace);
	void      SetFaceID(U32 uFaceID);
	void      SetIntersect(BOOL bFlag);
	void      SetMeshID(U32 uMeshID);

	CIFXBoundFace &operator=(CIFXBoundFace &operand);

private:

	IFXVector3 m_vCentroid;
    IFXU32Face m_face;         // IFXFace within the node
	BOOL       m_bIntersect;   // Intesection flag
	U32        m_uFaceID;      // ID to original mesh face
    U32        m_uMeshID;      // ID to origianl mesh
};


IFXINLINE CIFXBoundFace &CIFXBoundFace::operator=(CIFXBoundFace &operand)
{
	m_vCentroid      = operand.m_vCentroid;
	m_face = operand.m_face;
	m_bIntersect     = FALSE;
	m_uFaceID        = operand.m_uFaceID;
	m_uMeshID        = operand.m_uMeshID;

	return *this;
}

#endif
