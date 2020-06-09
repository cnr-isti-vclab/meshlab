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
	@file IFXVertexWeights.h
*/

#ifndef IFXVERTEXWEIGHTS_H
#define IFXVERTEXWEIGHTS_H

#include "IFXArray.h"
#include "IFXVertexWeight.h"

/**
	This class is used to store bone weights assigned to vertices
*/
class IFXVertexWeights : public IFXArray<IFXVertexWeight>
{
public:
				IFXVertexWeights(void) {};

	void        CopyFrom(const IFXVertexWeights *other);

	void        SortOnMeshThenVertex(void);
	void        SortOnMeshThenBonesThenVertex(void);
	void        SortOnZ(void);

	I32         GetMeshStart(I32 meshid) const
				{
					IFXASSERT( m_meshStarts.GetNumberElements() >= 1 );

					I32 max = m_meshStarts.GetNumberElements() - 1;
					if (meshid > max)
						meshid = max;
					
					return m_meshStarts[meshid];
				};

	void        Dump(void);

private:
	void        CreateMeshTable(void);

	static  I32 CompareOnMeshThenVertex(const void *elem1, const void *elem2);
	static  I32 CompareOnMeshThenBonesThenVertex(const void *elem1, const void *elem2);
	static  I32 CompareOnZ(const void *elem1, const void *elem2);

	IFXArray<I32>   m_meshStarts;
};


/**
	sort primary on mesh id, secondary on vertex id

@return
	- <0,   elem1 < elem2  
	- 0,    elem1 == elem2 
	- >0,   elem1 > elem2
*/
IFXINLINE I32 IFXVertexWeights::CompareOnMeshThenVertex(const void *elem1, const void *elem2)
{
	IFXVertexWeight *weight1 = (IFXVertexWeight *)elem1;
	IFXVertexWeight *weight2 = (IFXVertexWeight *)elem2;

	// lowest mesh id first
	if(weight1->GetMeshIndex()<weight2->GetMeshIndex())
		return -1;
	if(weight1->GetMeshIndex()>weight2->GetMeshIndex())
		return 1;

	// lowest vertex id first
	if(weight1->GetVertexIndex()<weight2->GetVertexIndex())
		return -1;
	if(weight1->GetVertexIndex()>weight2->GetVertexIndex())
		return 1;

	// highest bone first
	if(weight1->GetBoneIndex()>weight2->GetBoneIndex())
		return -1;
	if(weight1->GetBoneIndex()<weight2->GetBoneIndex())
		return 1;

	return 0;
}


/**
	@note like mesh:vertex should all have the same packed boneid values
*/
IFXINLINE I32 IFXVertexWeights::CompareOnMeshThenBonesThenVertex(
	const void *elem1, const void *elem2)
{
	IFXVertexWeight *weight1= (IFXVertexWeight *)elem1;
	IFXVertexWeight *weight2= (IFXVertexWeight *)elem2;

	// lowest mesh id first
	if(weight1->GetMeshIndex() < weight2->GetMeshIndex())
		return -1;
	if(weight1->GetMeshIndex() > weight2->GetMeshIndex())
		return 1;

	// sort on boneid's pack in offsets
	const F32 *fbone1 = weight1->Offset().RawConst();
	const F32 *fbone2 = weight2->Offset().RawConst();
	I32 m;

	for(m = 0; m < 3; ++m)
	{
		if(fbone1[m] < fbone2[m])
			return -1;
		if(fbone1[m] > fbone2[m])
			return 1;
	}

	fbone1=weight1->NormalOffset().RawConst();
	fbone2=weight2->NormalOffset().RawConst();
	
	for(m = 0; m < 3; ++m)
	{
		if(fbone1[m] < fbone2[m])
			return -1;
		if(fbone1[m] > fbone2[m])
			return 1;
	}

	// lowest vertex id first
	if(weight1->GetVertexIndex()<weight2->GetVertexIndex())
		return -1;
	if(weight1->GetVertexIndex()>weight2->GetVertexIndex())
		return 1;

	// highest weight first
	if(weight1->GetBoneWeight()>weight2->GetBoneWeight())
		return -1;
	if(weight1->GetBoneWeight()<weight2->GetBoneWeight())
		return 1;

	return 0;
}

/**
	sort on Offset Z

@return
	- <0,   elem1 < elem2  
	- 0,    elem1 == elem2 
	- >0,   elem1 > elem2
*/
IFXINLINE I32 IFXVertexWeights::CompareOnZ(const void *elem1, const void *elem2)
{
	IFXVertexWeight *weight1 = (IFXVertexWeight *)elem1;
	IFXVertexWeight *weight2 = (IFXVertexWeight *)elem2;

	I32 m = 2; // z

	if(weight1->Offset()[m] < weight2->Offset()[m])
		return -1;
	if(weight1->Offset()[m] > weight2->Offset()[m])
		return 1;

	return 0;
}

#endif
