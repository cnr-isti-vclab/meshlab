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
	@file IFXSkin_p3.cpp
*/

#include "IFXSkin.h"
#include "IFXVertexMap.h"
#include "IFXCharacter.h"
#include "IFXMeshInterface.h"
#include "IFXVertexIndexer.h"

/**
	ComputeDeformedVerticesPackedSSE

	@note	`speed critical'

	@warning
	If there is any scaling applied to the matrices,
	you must activate the compile-time flag IFXM34_UNSCALE
	in IFXMatrix3x4.h.

	@todo: get updates from original
	@todo: investigate re-unifying functions
*/
#ifdef __ICL

void IFXSkin::ComputeDeformedVerticesPackedSSE(BOOL clod)
{
	TIME_PROGRESS( m_character, IFXBonesManager::TimeDeformation, true );

	IFXPackBoneWeight *pNextWeight;

	F32 accumulator[4];
	F32 normaccum[4];
	IFXVertexIndexer *indexer = m_outmesh->GetVertexIndexer();

	IFXASSERT( m_inmesh->GetNumberMeshes() == m_outmesh->GetNumberMeshes() );

	const IFXBoneCacheArray &cbonecachearray = m_bonecachearray;

	U32 m, n, meshid;
	const U32 meshes = (U32)m_outmesh->GetNumberMeshes();

	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_outmesh->ChooseMeshIndex(meshid);

		const I32 numverts = (clod) ?
			m_outmesh->GetNumberVertices(): m_outmesh->GetMaxNumberVertices();

		IFXPackWeights &packweights = m_packweightarray[meshid];
		packweights.RewindForRead();
		const U32 length = packweights.GetNumberVerticesConst();
		const IFXPackVertex *packvertex = NULL;
		const IFXPackBoneWeight *packweight = NULL;
		U32 weights;
		U32 length1 = length - 1;

		if(length)
			packvertex=packweights.NextPackVertexForRead();

		for( m = 0; m < length; m++ )
		{
			packweight = packweights.NextPackBoneWeightForRead();
			weights = packvertex->GetNumWeights();
			IFXASSERT(weights > 0);

			packweights.PrefetchNextPackVertex();
			pNextWeight = packweights.PrefetchNextPackWeight();

			cbonecachearray.GetElementConst( packweight->GetBoneIndex()).
				GetGlobalMatrixConst().
					TransformVertexAndNormal(
						packweight->GetBoneWeight(),
						packvertex->OffsetConst(),
						accumulator,
						packvertex->NormalOffsetConst(),
						normaccum);

			IFXASSERT( ((IFXVector3*)&normaccum)->CalcMagnitude() <= 1.1f );
			for( n = 1; n < weights; n++ )
			{
				packweight = packweights.NextPackBoneWeightForRead();

				cbonecachearray.GetElementConst( packweight->GetBoneIndex()).
					GetGlobalMatrixConst().
						CumulativeTransformVertexAndNormal(
							packweight->GetBoneWeight(),
							packvertex->OffsetConst(), 
							accumulator,
							packvertex->NormalOffsetConst(),
							normaccum );

				IFXASSERT(((IFXVector3*)&normaccum)->CalcMagnitude()<=1.1f);
			}

			indexer->SetVertexAndNormal( packvertex->GetVertexIndex(), accumulator, normaccum );

			if(m < length1)
			{
				packvertex=packweights.NextPackVertexForRead();
				if( packvertex->GetVertexIndex() >= numverts )
				{
					// move on to next mesh
					m = length; // faster than break
				}
			}
		}
	}


	TIME_PROGRESS( m_character, IFXBonesManager::TimeDeformation, FALSE );
}
#else // __ICL

void IFXSkin::ComputeDeformedVerticesPackedSSE(BOOL clod)
{
	ComputeDeformedVerticesPacked(clod);
}

#endif // __ICL
