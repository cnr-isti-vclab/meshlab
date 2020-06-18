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
	@file IFXMatrix3x4.h
*/

#ifndef IFXMATRIX3X4_H
#define IFXMATRIX3X4_H

#include "IFXMatrix4x4.h"
#include "IFXQuaternion.h"

// if bones are scaled, this MUST be activated to get unit normals
#define IFXM34_UNSCALE  TRUE

/**
	shearless version of IFXMatrix4x4
*/
class IFXMatrix3x4
{
public:
	IFXMatrix3x4(void)
	{
		// no default clear
	};

	IFXMatrix3x4(const IFXMatrix4x4 &operand)
	{
		operator=(operand);
	};

	IFXMatrix3x4    &operator=(const IFXMatrix4x4 &operand);

	void            TransformVertexAndNormal(const F32 scale,
		const F32 *const vector_in,
		F32 *const vector_out,
		const F32 *const normal_in,
		F32 *const normal_out) const;
	void            CumulativeTransformVertexAndNormal(const F32 scale,
		const F32 *const vector_in,
		F32 *const vector_out,
		const F32 *const normal_in,
		F32 *const normal_out) const;

private:

	F32             m_data[12];
#if IFXM34_UNSCALE
	F32             m_norm[9];
#endif
};


IFXINLINE IFXMatrix3x4 &IFXMatrix3x4::operator=(const IFXMatrix4x4 &operand)
	{
	const F32 *mat=operand.RawConst();

	m_data[0]=mat[0];
	m_data[1]=mat[4];
	m_data[2]=mat[8];
	m_data[3]=mat[12];

	m_data[4]=mat[1];
	m_data[5]=mat[5];
	m_data[6]=mat[9];
	m_data[7]=mat[13];

	m_data[8]=mat[2];
	m_data[9]=mat[6];
	m_data[10]=mat[10];
	m_data[11]=mat[14];

#if IFXM34_UNSCALE
	//* Is this valid and is it the fastest way to remove scale?
	IFXQuaternion quat=operand;
	IFXMatrix4x4 result=quat;
	const F32 *mat2=result.RawConst();

	m_norm[0]=mat2[0];
	m_norm[1]=mat2[4];
	m_norm[2]=mat2[8];

	m_norm[3]=mat2[1];
	m_norm[4]=mat2[5];
	m_norm[5]=mat2[9];

	m_norm[6]=mat2[2];
	m_norm[7]=mat2[6];
	m_norm[8]=mat2[10];
#endif

	return *this;
	}

	//*                     `Center of the World'
	//* for bones animation, this is primary action of the core loop
// NOTE limited 3x3+3 non-shear multiplication, not 4x4 full multiplication
IFXINLINE void IFXMatrix3x4::TransformVertexAndNormal(F32 const scale,
					F32 const *const vi,F32 *const vout,
					F32 const *const ni,F32 *const nout) const
	{
	// unrolled
#if IFXM34_UNSCALE
	vout[0]=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]=scale*(ni[0]*m_norm[0]+ni[1]*m_norm[1]+ni[2]*m_norm[2]);
	vout[1]=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]=scale*(ni[0]*m_norm[3]+ni[1]*m_norm[4]+ni[2]*m_norm[5]);
	vout[2]=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+m_data[11]);
	nout[2]=scale*(ni[0]*m_norm[6]+ni[1]*m_norm[7]+ni[2]*m_norm[8]);
#else
	vout[0]=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]=scale*(ni[0]*m_data[0]+ni[1]*m_data[1]+ni[2]*m_data[2]);
	vout[1]=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]=scale*(ni[0]*m_data[4]+ni[1]*m_data[5]+ni[2]*m_data[6]);
	vout[2]=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+m_data[11]);
	nout[2]=scale*(ni[0]*m_data[8]+ni[1]*m_data[9]+ni[2]*m_data[10]);
#endif
	}
IFXINLINE void IFXMatrix3x4::CumulativeTransformVertexAndNormal(
					const F32 scale,
					const F32 *const vi,F32 *const vout,
					const F32 *const ni,F32 *const nout) const
	{
	// unrolled
#if IFXM34_UNSCALE
	vout[0]+=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]+=scale*(ni[0]*m_norm[0]+ni[1]*m_norm[1]+ni[2]*m_norm[2]);
	vout[1]+=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]+=scale*(ni[0]*m_norm[3]+ni[1]*m_norm[4]+ni[2]*m_norm[5]);
	vout[2]+=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+
																	m_data[11]);
	nout[2]+=scale*(ni[0]*m_norm[6]+ni[1]*m_norm[7]+ni[2]*m_norm[8]);
#else
	vout[0]+=scale*(vi[0]*m_data[0]+vi[1]*m_data[1]+vi[2]*m_data[2]+m_data[3]);
	nout[0]+=scale*(ni[0]*m_data[0]+ni[1]*m_data[1]+ni[2]*m_data[2]);
	vout[1]+=scale*(vi[0]*m_data[4]+vi[1]*m_data[5]+vi[2]*m_data[6]+m_data[7]);
	nout[1]+=scale*(ni[0]*m_data[4]+ni[1]*m_data[5]+ni[2]*m_data[6]);
	vout[2]+=scale*(vi[0]*m_data[8]+vi[1]*m_data[9]+vi[2]*m_data[10]+
																	m_data[11]);
	nout[2]+=scale*(ni[0]*m_data[8]+ni[1]*m_data[9]+ni[2]*m_data[10]);
#endif
	}


#endif
