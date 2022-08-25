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
	@file	IFXCornerIter.cpp

			This module defines the IFXCornerIter class.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXCornerIter.h"
#include "IFXNeighborMesh.h"

void IFXCornerIter::Initialize(IFXNeighborMesh* pNeighborMesh,
							   U32 mesh, U32 face, U32 corner)
{
	IFXASSERT(pNeighborMesh);

	if (NULL != pNeighborMesh)
	{
		m_pNeighborMesh = pNeighborMesh;
		m_mesh = mesh;
		m_face = face;
		m_corner = corner;

		m_pNeighborFaces = m_pNeighborMesh->GetNeighborFaceArray(m_mesh);
	}
}

void IFXCornerIter::LoadMesh()
{
	// Get new pointer to array of neighbor faces
	m_pNeighborFaces = m_pNeighborMesh->GetNeighborFaceArray(m_mesh);
}
