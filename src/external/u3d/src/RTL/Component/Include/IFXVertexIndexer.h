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
	@file IFXVertexIndexer.h 
*/

#ifndef IFXVERTEXINDEXER_H
#define IFXVERTEXINDEXER_H

#include "IFXIterators.h"

/**
	Index-optimized mesh iterator.

	This class defines an accessor to IFXMesh and is derived from IFXVertexIter 
	with special attention placed on random access by vertex index.

	Only accesses first texture layer.
*/
class IFXVertexIndexer : public IFXVertexIter
{
public:
	/// Sets current vertex index and adjusts state.
	void MoveToIndex(U32 index)
	{
		U32 i;
		for(i = 0; i <= IFX_MESH_TC0; i++)
			m_ppIndexedData[i] = m_ppData[i] + index * m_puStride[i];
	}

	/// Gets the current vertex position.
	IFXVector3	*GetPosition(void)
	{
		return (IFXVector3*)(m_ppIndexedData[IFX_MESH_POSITION]);
	}

	/// Sets the current vertex position.
	void	SetPosition(const float *data)
	{
		*((IFXVector3*)(m_ppIndexedData[IFX_MESH_POSITION]))
														=data;
	}

	/// Gets the current vertex normal.
	IFXVector3	*GetNormal(void)
	{
		return (IFXVector3*)(m_ppIndexedData[IFX_MESH_NORMAL]);
	}
	
	/// Sets the current vertex normal.
	void	SetNormal(const float *data)
	{
		*((IFXVector3*)(m_ppIndexedData[IFX_MESH_NORMAL])) = data;
	}

	/// Gets the current vertex texture coordinate.
	IFXVector2	*GetTexCoord(void)
	{
		return (IFXVector2*)(m_ppIndexedData[IFX_MESH_TC0]);
	}

	/// Sets the current vertex texture coordinate.
	void		SetTexCoord(const float *data)
	{
		*((IFXVector2*)(m_ppIndexedData[IFX_MESH_TC0])) = data;
	}

	/** Sets the position and normal of an indexed vertex
		in a single call (convenience). */
	void		SetVertexAndNormal(long index,const float vert[3],
													const float normal[3])
	{
		MoveToIndex(index);
		SetPosition(vert);
		SetNormal(normal);
	}

private:
	U8*	m_ppIndexedData[IFX_MESH_TC0+1];
};

#endif
