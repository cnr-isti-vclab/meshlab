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
	@file IFXVertexWeight.h
*/

#ifndef IFXVERTEXWEIGHT_H
#define IFXVERTEXWEIGHT_H

#include "IFXVector3.h"

/**
	This class describes single vertex bone weight
*/
class IFXVertexWeight
{
public:
				IFXVertexWeight(void)   { Reset(); };

	void        Reset(void)
				{
					m_boneid = 0;
					m_meshid = 0;
					m_vertexid = 0;
					m_weight = 0.0f;
					m_offset.Reset();
					m_normalOffset.Reset();
				};

	IFXVertexWeight &operator=(const IFXVertexWeight &operand)
				{
					CopyFrom(operand);
					return *this;
				};

	void        CopyFrom(const IFXVertexWeight &other)
				{
					m_boneid = other.m_boneid;
					m_meshid = other.m_meshid;
					m_vertexid = other.m_vertexid;
					m_weight = other.m_weight;
					m_offset = other.m_offset;
					m_normalOffset = other.m_normalOffset;
				};

	void        SetBoneIndex(I32 set)           { m_boneid = set; };
	I32         GetBoneIndex(void) const        { return m_boneid; };
	I32         &BoneIndex(void)                { return m_boneid; };

	void        SetMeshIndex(I32 set)           { m_meshid = set; };
	I32         GetMeshIndex(void) const        { return m_meshid; };
	I32         &MeshIndex(void)                { return m_meshid; };

	void        SetVertexIndex(I32 set)         { m_vertexid = set; };
	I32         GetVertexIndex(void) const      { return m_vertexid; };
	I32         &VertexIndex(void)              { return m_vertexid; };

	void        SetBoneWeight(F32 set)          { m_weight = set; };
	F32         GetBoneWeight(void) const       { return m_weight; };
	F32         &BoneWeight(void)               { return m_weight; };

	IFXVector3  &Offset(void)                   { return m_offset; };
	const   IFXVector3  &OffsetConst(void) const        { return m_offset; };

	IFXVector3  &NormalOffset(void)             { return m_normalOffset; };
	const   IFXVector3  &NormalOffsetConst(void) const
												{ return m_normalOffset; };

	IFXString   Out(bool verbose = false) const;

private:
	I32             m_meshid;
	I32             m_boneid;
	I32             m_vertexid;
	F32             m_weight;
	IFXVector3      m_offset;
	IFXVector3      m_normalOffset; ///< cache only one
};

#endif
