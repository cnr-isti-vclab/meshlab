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

#ifndef IFXVERTEXMAPGROUP_H
#define IFXVERTEXMAPGROUP_H

#include "IFXResult.h"
#include "IFXDataTypes.h"
#include "IFXVertexMap.h"


/**
	@file	IFXVertexMapGroup.h

	@brief	A collection of vertex maps, one per mesh.

			A vertex map hold a mapping for each vertex index of the original 
			mesh to one or more vertex indices in the IFXMesh.  Since there 
			is one map per IFXMesh, we hold a vertex map group for an entire 
			IFXMeshGroup.

	@note	there is no parallel between UpdatesGroup, MeshGroup, and
			VertexMapGroup -- the indices refer to meshes in different contexts
			"meshNumber" below refers to authored nodes, not IFXMeshes.
*/

/** This class handles a collection of vertex maps, one per mesh. */
class IFXVertexMapGroup
{
	public:
						IFXVertexMapGroup();
						~IFXVertexMapGroup();

		/** Allocates space for a given number of vertex maps, presumably the
		same as the number of meshes in the mesh group we are paired with. */
		IFXRESULT		AllocateGroup(U32 numMeshes);

		/// Gets the specific map in the group by index.
		IFXVertexMap*	GetMap(U32 meshNumber);

		/** Queries the number of maps in this group, presumably 
		the same number that was allocated. */
		U32				GetNumMaps();

	private:
		IFXVertexMap*	m_pMapArray;
		U32				m_arraySize;
};


IFXINLINE IFXVertexMap* IFXVertexMapGroup::GetMap(U32 meshNumber)
{
	IFXASSERT(meshNumber < m_arraySize);
	return &m_pMapArray[meshNumber];
}

IFXINLINE U32 IFXVertexMapGroup::GetNumMaps()
{
	return m_arraySize;
}

#endif
