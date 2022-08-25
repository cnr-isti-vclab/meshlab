//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file IFXBoneWeightsModifier.h

	Declaration of IFXBoneWeightsModifier interface.
	The IFXBoneWeightsModifier interface is used to get and set bone 
	weights information. The bone weights information is used for bones
	based character animation.
*/
#ifndef __IFXBONEWEIGHTSMODIFIER_H__
#define __IFXBONEWEIGHTSMODIFIER_H__

#include "IFXUnknown.h"
#include "IFXBones.h"
#include "IFXModifier.h"

// {EE447627-5449-4ed4-8484-1283AA9EC79A}
IFXDEFINE_GUID(IID_IFXBoneWeightsModifier, 
0xee447627, 0x5449, 0x4ed4, 0x84, 0x84, 0x12, 0x83, 0xaa, 0x9e, 0xc7, 0x9a);

class IFXBoneWeightsModifier : virtual public IFXModifier
{
public:
	
	/** Gets the total number of vertices with weights and the sum total of
		weights for all the vertices.  This needs to set for every mesh
		in the model. This is required for space allocation before
		setting any weight data. */
virtual	IFXRESULT IFXAPI GetTotalBoneWeightCount(U32 meshIndex,
												 U32 *pTotalWeightCount) =0;

/** Gets the number of weights for a particular vertex on a
	specified mesh.  This allows for preallocation before
	GetBoneWeights(). */
virtual	IFXRESULT IFXAPI GetBoneWeightCount(U32 vertexIndex,
											U32 meshIndex,
											U32 *pWeightCount)	=0;

/** Gets the number of weights for a particular vertex on a
	specified mesh.  The pointers are arrays of length uWeightCount
	and are assumed to be preallocated. */
virtual	IFXRESULT IFXAPI GetBoneWeights(U32 vertexIndex,U32 meshIndex,
										U32 weightCount,U32 *pBoneIDs,
										F32 *pBoneWeights) =0;

/** Sets the total number of vertices with weights and the sum total of
	weights for all the vertices of author mesh.  This is used only for 
	temporal storing of data during decoding. This is required for space 
	allocation before setting any weight data. */
virtual	IFXRESULT IFXAPI SetTotalBoneWeightCountForAuthorMesh(
	U32 totalVertexCount)=0;

/** Sets the weights for a particular vertex on an author mesh.
    This is used only for temporal storing of data during decoding.

    @note The sum of all the bone weights at the position should be 1.0 
    (see section 9.7.4.6.3 of ECMA-363 2nd edition) */
virtual IFXRESULT IFXAPI SetBoneWeightsForAuthorMesh(U32 vertexIndex, 
													 U32 weightCount,
													 U32 *pBoneIDs,
													 F32 *pBoneWeights) =0;
};

#endif
