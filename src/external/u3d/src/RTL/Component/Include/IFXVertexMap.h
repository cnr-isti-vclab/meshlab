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

#ifndef IFXVERTEXMAP_H
#define IFXVERTEXMAP_H

#include "IFXResult.h"
#include "IFXDebug.h"
#include "IFXDataTypes.h"

/**
	@file	IFXVertexMap.h

			The conversion of a model to an IFXMeshGroup of IFXMeshes may 
			duplicate vertices to handle the multiple normals or texture 
			coordinates that may occur at a particular original vertex.  
			The VertexMap maps an original vertex index to the IFXMesh 
			index for IFXMeshGroup and the IFXMesh vertex index.
*/

/** This class manages the mapping of an original vertex index to 
the IFXMesh index for the IFXMeshGroup and the IFXMesh vertex indexes. */
class IFXVertexMap
{
	public:
		IFXVertexMap();
		~IFXVertexMap();

		/// Returns the number of original vertices.
		U32 GetNumMapEntries(void);

		/** Returns the number of copies of a particular 
		original vertex there are in the IFXMesh. */
		U32 GetNumVertexCopies(U32 originalVertexIndex);

		/** Specifies the original vertex index and the copy number.  
		Returns the	corresponding IFXMesh index for IFXMeshGroup 
		and the IFXMesh vertex index.*/
		IFXRESULT GetVertexCopy(U32 originalVertexIndex,
								U32 copyIndex,
								U32* pIndexIFXMesh,
								U32* pIndexIFXVertex);

		/// This is used by the AuthorMeshMap concatenate.
		IFXRESULT Concatenate(U32* pMapArray, U32 arraySize);

		/** This is used by MeshMap concatenate. It returns a new map
		that combines pVMap with this map.*/
		IFXRESULT ConcatenateVertexMap(IFXVertexMap* pVMap, 
									   IFXVertexMap** ppResultMap);
		
		/** Builds the map.*/
		IFXRESULT AllocateMap(U32 numOrigVertices);

		IFXRESULT AddVertex(U32 origVertexIndex, U32 indexIFXMesh, 
							U32 indexIFXVertex);

	private:
		/** number of elements in pCounts and
		number of pointers MeshVert lists in ppCopies.*/
		U32 m_mapSize;		

		struct MeshVert {
			U32 mesh;		///< IFXMesh index for IFXMeshGroup
			U32 vert;		///< IFXMesh vertex index
		};

		U32*		m_pCounts;
		U32*		m_pBlockSizes;
		MeshVert**	m_ppCopies;
};


//
// inlines
//

/** Returns how many copies of a particular imesh vertex there are 
in the IFX mesh. */
IFXINLINE U32 IFXVertexMap::GetNumVertexCopies(U32 meshVertexIndex)
{ 
	U32 result = 0;

	IFXASSERT(meshVertexIndex < m_mapSize);

	if (meshVertexIndex < m_mapSize)
	{
		result = m_pCounts[meshVertexIndex];
	}

	return result;
}

/** Gives you the index into the IFXMeshGroup and position in the IFXMesh of a
particular copy of an imesh vertex. */
IFXINLINE IFXRESULT IFXVertexMap::GetVertexCopy(U32  meshVertexIndex,
												U32  copyIndex,
												U32* pIndexIFXMesh,
												U32* pIndexIFXVertex)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(pIndexIFXMesh);
	IFXASSERT(pIndexIFXVertex);

	if ( (NULL != pIndexIFXMesh) && (NULL != pIndexIFXVertex) )
	{
		IFXASSERT(meshVertexIndex < m_mapSize);
		IFXASSERT(copyIndex < m_pCounts[meshVertexIndex]);

		if ( (meshVertexIndex < m_mapSize) && (copyIndex < m_pCounts[meshVertexIndex]) )
		{
			MeshVert& mapEntry = m_ppCopies[meshVertexIndex][copyIndex];
			*pIndexIFXMesh   = mapEntry.mesh;
			*pIndexIFXVertex = mapEntry.vert;
		}
		else
		{
			result = IFX_E_INVALID_RANGE;
		}
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}

#endif
