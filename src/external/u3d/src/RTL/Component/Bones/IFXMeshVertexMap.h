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
	@file IFXMeshVertexMap.h
*/

#ifndef IFXMESHVERTEXMAP_H
#define IFXMESHVERTEXMAP_H

#include "IFXList.h"
#include "IFXArray.h"

class IFXMeshVertex
{
public:
	IFXMeshVertex(void)
	{
		m_mesh=0;
		m_vertex=0;
	};

	U32             &GetMeshIndex(void)     { return m_mesh; }
	U32             &GetVertexIndex(void)   { return m_vertex; }

	void            GetMeshVertex(U32* m, U32* v)
	{
		*m = m_mesh;
		*v = m_vertex;
	}
	void            SetMeshVertex(U32 m,U32 v)
	{
		m_mesh=m;
		m_vertex=v;
	}

	IFXMeshVertex   &operator=(const IFXMeshVertex &operand)
	{
		m_mesh=operand.m_mesh;
		m_vertex=operand.m_vertex;
		return *this;
	}

	bool            operator==(const IFXMeshVertex &operand) const
	{ return (m_mesh==operand.m_mesh &&
	m_vertex==operand.m_vertex); };

private:
	U32     m_mesh;
	U32     m_vertex;
};

class IFXMeshVertexList: public IFXList<IFXMeshVertex>
{
public:
	IFXMeshVertexList(void)
	{
		SetAutoDestruct(true);
	};
};



class IFXMeshVertexArray: public IFXArray<IFXMeshVertex>
{
};

class IFXMeshVertexMap: public IFXArray<IFXMeshVertexList>
{
};

#endif
