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
	@file IFXVertexWeights.cpp
*/

#include <stdlib.h> // qsort support
#include "IFXVertexWeights.h"

void IFXVertexWeights::CopyFrom(const IFXVertexWeights *other)
{
	I32 m = 0, weights = 0;

	IFXASSERT(NULL != other);

	Clear();

	weights = other->GetNumberElements();
	ResizeToAtLeast(weights);

	for(m = 0; m < weights; ++m)
		GetElement(m).CopyFrom(other->GetElementConst(m));
}

void IFXVertexWeights::SortOnMeshThenVertex(void)
{
	qsort(Collect(), GetNumberElements(), sizeof(IFXVertexWeight),
						&IFXVertexWeights::CompareOnMeshThenVertex);

	CreateMeshTable();
//  Dump();
}

void IFXVertexWeights::SortOnMeshThenBonesThenVertex(void)
{
	qsort(Collect(),GetNumberElements(),sizeof(IFXVertexWeight),
						&IFXVertexWeights::CompareOnMeshThenBonesThenVertex);

	CreateMeshTable();
//  Dump();
}

void IFXVertexWeights::SortOnZ(void)
{
	qsort(Collect(), GetNumberElements(), sizeof(IFXVertexWeight), &IFXVertexWeights::CompareOnZ);
}

void IFXVertexWeights::Dump(void)
{
	I32 m = 0, length = GetNumberElements();

	for(m = 0; m < length; ++m)
	{
	   // IFXVertexWeight &vertexweight=GetElement(m);
		IFXTRACE_GENERIC(L"%d: %ls\n",m,GetElement(m).Out().Raw());
	}
}

/**
	@note must Sort() first
*/
void IFXVertexWeights::CreateMeshTable(void)
{
	I32 meshes = 0;

	m_meshStarts.Clear();

	// mark where each mesh starts in the list of weights
	I32 n, m, length = GetNumberElements();

	for(m = 0; m < length; ++m)
	{
		IFXVertexWeight &rVertexWeight = GetElement(m);
		I32 meshid = rVertexWeight.GetMeshIndex();

		if(meshes < meshid+1)
		{
			m_meshStarts.ResizeToAtLeast(meshid+1);

			for(n = meshes; n < meshid + 1; ++n)
				m_meshStarts[n]=0;

			m_meshStarts[meshid] = m;
			meshes = meshid + 1;
		}
	}

	// add an extra entry for the end
	++meshes;
	m_meshStarts.ResizeToAtLeast(meshes);
	m_meshStarts[meshes-1]=m;

	// point meshes without weights to the next mesh
	for(m = meshes-1; m > 0; --m)
	{
		if(0 == m_meshStarts[m])
			m_meshStarts[m]=m_meshStarts[m+1];

		//      IFXTRACE_GENERIC(L"mesh %d at %d\n",m,m_meshStarts[m]);
	}
}
