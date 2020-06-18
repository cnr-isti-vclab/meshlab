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
	@file	IFXFatCornerIter.cpp

			This module defines the IFXFatCornerIter interface.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXFatCornerIter.h"
#include "IFXNeighborMesh.h"
#include "IFXMeshGroup.h"
#include "IFXMesh.h"

// Can only be initialized by IFXNeighborMesh
void IFXFatCornerIter::Initialize(IFXNeighborMesh* pNeighborMesh,
								  U32 mesh, U32 face, 
								  U32 corner,
								  IFXMeshGroup* pMeshGroup,
								  U32 textureLayer)
{
	IFXCornerIter::Initialize(pNeighborMesh, mesh, face, corner);
	m_pMeshGroup = pMeshGroup;
	m_textureLayer = textureLayer;

	LoadMesh();	// load iterators for mesh

	m_pCurrFace = m_faceIter.Index(GetFaceIndex());
	m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());
}


void IFXFatCornerIter::LoadMesh()
{
	IFXMesh* pMesh = 0;
	m_pMeshGroup->GetMesh(GetMeshIndex(), pMesh);
	IFXASSERT(pMesh);  // no external conditions would cause this to fail

	IFXVertexAttributes attrs = pMesh->GetAttributes();
	m_hasNormal = attrs.m_uData.m_bHasNormals;
	m_hasTexCoord = m_textureLayer < attrs.m_uData.m_uNumTexCoordLayers;

	pMesh->GetFaceIter(m_faceIter);

	// We assume that all useful meshes will have 
	// vertex positions.
	IFXASSERT(attrs.m_uData.m_bHasPositions);
	pMesh->GetPositionIter(m_positionIter);

	if (m_hasNormal)
		pMesh->GetNormalIter(m_normalIter);

	if (m_hasTexCoord)
		pMesh->GetTexCoordIter(m_texCoordIter, m_textureLayer);


	IFXRELEASE(pMesh);
}


