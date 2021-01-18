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
	@file	IFXCoincidentVertexMap.h

			Builds a map of vertices that are coincident in space (also called
			the geometric vertices).  One method performs floating point
			comparisons of the vertex positions and the other uses a supplied 
			standard vertex map (IFXVertexMapGroup).
			
	@note	The IFXVertexMapGroup method is much faster because it uses the 
			vertex map created from the process of material partitioning the 
			original mesh vertices.  If the original mesh has duplicate 
			vertices (vertices with different indices but same position) then 
			the neighbor mesh will have boundries on the edges with those 
			vertices.  If this is not desired, it is recommended that the user 
			"weld" the vertices in 3D Studio MAX before exporting it.

			The method using vertex position comparisons has O(n) performance
			at best (widely distributed points), and O(n^2) worst case (all 
			vertices are the same position).  Models tend to have distributed 
			vertex positions.
*/

#ifndef IFXCOINCEDENTVERTEXMAP_H
#define IFXCOINCEDENTVERTEXMAP_H

#include "IFXVertexMapGroup.h"

/** This class is used to build a map of vertices that are coincident 
in space (also called the geometric vertices). One method performs floating 
point comparisons of the vertex positions and the other uses a supplied 
standard vertex map (IFXVertexMapGroup). */
class IFXCoincidentVertexMap
{
public:
	IFXCoincidentVertexMap();
	~IFXCoincidentVertexMap();
		
	/** Builds a coincident vertex map using vertex positions 
	(float hash table). 
	
	Build a coincident vertex map by comparing floating point positions
	of all the vertices in the mesh group.*/
	IFXRESULT Initialize(IFXMeshGroup* pMeshGroup);

	/** Builds a coincident vertex map using a vertex map. */
	IFXRESULT Initialize(IFXMeshGroup* pMeshGroup, 
						 IFXVertexMapGroup* pVertexMapGroup);

	/** Gets the number of geometric vertices. */
	U32 GetNumVertexID();

	/** Converts the IFXMesh vertex index into a geomtric vertex ID. */
	U32 Convert(U32 meshIndex, U32 vertexIndex);

private:
	/// number of pointers to pCoincidentVertexID
	U32		m_numMeshes;	
	/// vertex map. Vertices with same position have same ID
	U32**	m_ppCoincidentVertexID;	
	/// number of geometric vertices
	U32		m_numVertexID;
};

//
//  inlines
//

IFXINLINE U32 IFXCoincidentVertexMap::GetNumVertexID()
{
	return m_numVertexID;
}


IFXINLINE U32 IFXCoincidentVertexMap::Convert(U32 meshIndex, U32 vertexIndex)
{
	return m_ppCoincidentVertexID[meshIndex][vertexIndex];
}

#endif
