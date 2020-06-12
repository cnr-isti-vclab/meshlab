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

#ifndef __IFXMatrix4x4_h__
#define __IFXMatrix4x4_h__

#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXVector4.h"
#include <memory.h>

enum IFXAxisID {IFX_X_AXIS,IFX_Y_AXIS,IFX_Z_AXIS};

class IFXQuaternion;

#define IFXNEARZERO (1e-36f) // allows scale of 1/3 the exponent

/**************************************************************************/
/**
\brief 4x4 Matrix with various 3D transform operations.

Priorities:
<UL>
<LI> speed, not elegance (many loops unrolled)
<LI> direct use in OpenGL
</UL>

Internally implemented as an array of 16 floats.

\verbatim
Column major (x,y) where
00  10  20  30
01  11  21  31
02  12  22  32
03  13  23  33
is stored as
0 4 8 12
1 5 9 13
2 6 10  14
3 7 11  15
representing
Rot Rot Rot Tx
Rot Rot Rot Ty
Rot Rot Rot Tz
0 0 0 1
\endverbatim

NOTE the bottom row was previously referred to as shear, which is incorrect.
The bottom row really just contributes to a fourth vector element and
has no shearing effect on the 3 primary vector elements.

The fourth row is commonly used for a distance element in a
perspective calculation.

NOTE Functions named with "3x4" treat the matrix as an affine 3x4 transform.
*/
/***************************************************************************/
/**This interface provides 4x4 Matrix with various 3D transform operations.*/
class IFXMatrix4x4
{
public:
	/** Construct without initializing.*/
	IFXMatrix4x4(void)
	{ /* no default clear */ }

	/** Construct using an array of at least 16 floats
	representing the 16 elements in the matrix. */
	IFXMatrix4x4(const F32 *pFloats) { operator=(pFloats); }

	/** Copy constructor.*/
	IFXMatrix4x4(const IFXMatrix4x4 &operand) { operator=(operand); }

	/** Construct using a quaternion rotation and zero
	translation. */
	IFXMatrix4x4(const IFXQuaternion &quat) { ResetNonRotation(); operator=(quat); }

	/** Construct using four Vector3 objects to fill
	in a 3x4 transform. */
	IFXMatrix4x4(const IFXVector3 &x,const IFXVector3 &y, const IFXVector3 &z,const IFXVector3 &t) { ResetNonRotation(); Set(x,y,z,t); }

	/** Populates the matrix using four Vector3 objects to
	fill in a 3x4 transform. */
	IFXMatrix4x4  &Set(const IFXVector3 &x,const IFXVector3 &y, const IFXVector3 &z,const IFXVector3 &t);

	/** Populates the matrix using an array of at least
	16 floats representing the 16 elements in the
	matrix. */
	IFXMatrix4x4  &operator=(const F32 *pFloats);

	/** Deep copy. */
	IFXMatrix4x4  &operator=(const IFXMatrix4x4 &operand);

	// not inline
	/** Populates the matrix using a quaternion rotation.
	The translation and bottom row are untouched,
	but scaling is eliminated. */
	IFXMatrix4x4  &operator=(const IFXQuaternion &quat);

	/** Copies a true 2-dimensional 4x4 float array.*/
	IFXMatrix4x4  &CopyMatrix(const F32 ppFloats[4][4]);

	/** Exact compare operation (zero tolerance).*/
	BOOL      operator==(const IFXMatrix4x4 &operand) const;

	/** Initializes to a reasonable state
	(simply calls MakeIdentity()). */
	void      Reset(void)         { MakeIdentity(); }

	/** Initializes to the identity transform.*/
	IFXMatrix4x4  &MakeIdentity(void);

	/** Computes the tensor of two Vector3's and put
	the result in this matrix. */
	IFXMatrix4x4  &MakeTensor(const IFXVector3 &left, const IFXVector3 &right);

	/** Removes all elements outside of the 3x3 rotation and
	scale, including the translation and bottom row. */
	void      ResetNonRotation(void) { ResetTranslation(); ResetBottom(); }

	/** Sets the transform translation to zero.*/
	void      ResetTranslation(void);

	/** Clears the elements outside the basic 3x4 transform.*/
	void      ResetBottom(void);

	/** Returns a pointer to the internal float array
	containing the 16 floats. This is not a copy.
	This data matches the OpenGL format. */
	F32       *Raw(void)          { return m_data; };

	/** Returns a const pointer to the internal float array.*/
	const F32       *RawConst(void) const   { return m_data; };

	/** Accesses an element of the internal float array.*/
	F32       &operator[](int n)      { return m_data[n]; };

	/** Sets the translation directly.*/
	void      SetTranslation(const IFXVector3 &vector)
	{
		const F32 *v=vector.RawConst();
		m_data[12]=v[0];
		m_data[13]=v[1];
		m_data[14]=v[2];
	}

	/** Return a Vector3 containing the translation.
	This is not a copy. */
	const IFXVector3    &TranslationConst(void) const { return (const IFXVector3&)m_data[12]; }


	/** Pre-multiplies a translation to the 3x4 transform.*/
	IFXMatrix4x4  &Translate3x4(const IFXVector3 &vector);

	/** Pre-multiplies a rotation to the 3x4 transform of
	a given angle in radians about a given
	enumerated axis: IFX_X_AXIS, IFX_Y_AXIS,
	or IFX_Z_AXIS. */
	IFXMatrix4x4  &Rotate3x4(F32 radians,IFXAxisID axis);

	/** Pre-multiplies a scale to the 3x4 transform,
	using an independent scalar for each axis. */
	IFXMatrix4x4  &Scale3x4(const IFXVector3 &vector);

	/** Pre-multiplies a scale to the 3x4 transform,
	using an independent scalar for each axis.
	This version is slower since it accounts
	for data in the bottom row. */
	IFXMatrix4x4  &Scale(const IFXVector3 &vector);
	BOOL            NegativelyScaled();

	/** Pre-multiplies an arbitrary matrix with optimizations
	assuming that both matrices are 3x4 transforms. */
	IFXMatrix4x4  &Multiply3x4(const IFXMatrix4x4 &left, const IFXMatrix4x4 &right);

	/** Pre-multiplies an arbitrary matrix with no
	assumptions. */
	IFXMatrix4x4  &Multiply(const IFXMatrix4x4 &left, const IFXMatrix4x4 &right);

	/** Inverts matrix in place assuming that it is
	an affine 3x4 transform. */
	IFXRESULT   Invert3x4(const IFXMatrix4x4 &operand);

	/** Inverts matrix in place with no assumptions.*/
	IFXRESULT   Invert(const IFXMatrix4x4 &operand);

	/** Calculates the determinant of the full 4x4 matrix.*/
	F32       CalcDeterminant(void) const;

	/** Calculates the determinant of the core affine 3x3
	matrix, excluding translation and the forth row. */
	F32       CalcDeterminant3x3(void) const;

	/** Calculates the determinant of an arbitrary 3x3
	matrix passed as 9 individual floats. */
	static  F32       CalcDeterminant3x3(F32 a1,F32 a2,F32 a3,F32 b1,F32 b2,F32 b3,F32 c1,F32 c2,F32 c3);

	/** Transforms a Vector3 through this matrix.*/
	void      TransformVector(const IFXVector3 &source, IFXVector3 &result) const;
	/** Rotates a Vector3 through this matrix, excluding
	translation, but including scale. */
	void      RotateVector(const IFXVector3 &source, IFXVector3 &result) const;

	/** Projects a Vector3 through this matrix, assuming
	it is manually populated with a 4x4 projection
	matrix.  This uses the forth row to produce
	scalar to divide into the Vector3 result to
	simulate perspective. */
	void      ProjectVector(const IFXVector3 &source, IFXVector3 &result) const;

	// not inline:
#if FALSE
	/** Scale signs are determined by quaternion conversion.*/
	void      ExtractScale(IFXVector3 &scale, IFXQuaternion &pResult) const;
#endif
	/** Computes the translation, rotation, and scale
	that can be multiplied together to produce
	this matrix. */
	// Scale signs are determined by quaternion conversion.
	IFXRESULT   Decompose(IFXVector3 &translation, IFXMatrix4x4 &rotation, IFXVector3 &scale) const;

	/** Calls Decompose() to compute components but only
	populates the pointers if the are not NULL. */
	IFXRESULT   CalcTRS(IFXVector3 *pTranslation, IFXQuaternion *pRotation,IFXVector3 *pScale) const;

	/** Computes scaling relative to unit axis vectors.
	This is faster than a full decomposition. */
	void      CalcAxisScale(IFXVector3 &scale) const;

	// round near-integer values
//    void      ApplyThreshold(F32 threshold);

	/** Transpose this matrix in place.*/
	void      Transpose();

	/** Applies a right-handed rotation in radians around a
	given axis through the origin. */
	void            RotateAxis(const IFXVector4& axis, const F32 angle);

	/** The IFXMatrix4x4 IFXVector4
	multiplication operator. */
	const           IFXVector4 operator*( IFXVector4& v4 ) const
	{
		IFXVector4 res;
		F32 x = v4.X();
		F32 y = v4.Y();
		F32 z = v4.Z();
		F32 h = v4.H();

		res[0] = m_data[0]*x + m_data[4]*y + m_data[8]*z + m_data[12]*h;
		res[1] = m_data[1]*x + m_data[5]*y + m_data[9]*z + m_data[13]*h;
		res[2] = m_data[2]*x + m_data[6]*y + m_data[10]*z + m_data[14]*h;
		res[3] = m_data[3]*x + m_data[7]*y + m_data[11]*z + m_data[15]*h;

		return res;
	}

protected:
	F32       m_data[16];
};


IFXINLINE void IFXMatrix4x4::ResetTranslation(void)
{
	m_data[12]=0.0f;
	m_data[13]=0.0f;
	m_data[14]=0.0f;
}

IFXINLINE void IFXMatrix4x4::ResetBottom(void)
{
	m_data[3]=0.0f;
	m_data[7]=0.0f;
	m_data[11]=0.0f;
	m_data[15]=1.0f;
}

IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::MakeTensor(const IFXVector3 &left,
												 const IFXVector3 &right)
{
	/// @todo: check equations and test results

	const F32 *v1=left.RawConst();
	const F32 *v2=right.RawConst();

	m_data[0]=  v1[0]*v2[0];
	m_data[1]=  v1[1]*v2[0];
	m_data[2]=  v1[2]*v2[0];
	m_data[4]=  v1[0]*v2[1];
	m_data[5]=  v1[1]*v2[1];
	m_data[6]=  v1[2]*v2[1];
	m_data[8]=  v1[0]*v2[2];
	m_data[9]=  v1[1]*v2[2];
	m_data[10]= v1[2]*v2[2];
	ResetNonRotation();
	return *this;
}

IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::operator=(const F32 *pFloats)
{
	memcpy(m_data, pFloats, 16*sizeof(F32));
	return *this;
}

IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::CopyMatrix(const F32 ppFloats[4][4])
{
	memcpy(m_data, ppFloats, 16*sizeof(F32));
	return *this;
}

IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::operator=(const IFXMatrix4x4 &operand)
{
	memcpy(m_data, operand.m_data, 16*sizeof(F32));
	return *this;
}


IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::Set(const IFXVector3 &x, const IFXVector3 &y, const IFXVector3 &z, const IFXVector3 &t)
{
	const F32 *v=x.RawConst();
	m_data[0 ]=v[0];
	m_data[1 ]=v[1];
	m_data[2 ]=v[2];

	v=y.RawConst();
	m_data[4 ]=v[0];
	m_data[5 ]=v[1];
	m_data[6 ]=v[2];

	v=z.RawConst();
	m_data[8 ]=v[0];
	m_data[9 ]=v[1];
	m_data[10]=v[2];

	v=t.RawConst();
	m_data[12]=v[0];
	m_data[13]=v[1];
	m_data[14]=v[2];

	return *this;
}


IFXINLINE BOOL IFXMatrix4x4::operator==(const IFXMatrix4x4 &operand) const
{
//	return (memcmp(m_data, operand.m_data, 16*sizeof(F32)) == 0);
	U32 i;
	for (i=0; i<16; i++)
		if (m_data[i] != operand.m_data[i])
			return false;
	return true;
}

IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::Translate3x4(const IFXVector3 &vector)
{
	IFXVector3 result;
	const F32 *v=result.RawConst();

	RotateVector(vector,result);    //* equivalent when you do the math

	//* assuming 3x4
	m_data[12]+=v[0];
	m_data[13]+=v[1];
	m_data[14]+=v[2];

	return *this;
}


IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::Rotate3x4(F32 radians,IFXAxisID axis)
{

	F32 sina=IFXSIN(radians);
	F32 cosa=IFXCOS(radians);
	F32 b,c;

	static const int index[3][2]={{4,8},{8,0},{0,4}};
#if TRUE

	const int &x1=index[axis][0];
	const int &x2=index[axis][1];

	int y;
	for(y=0;y<3;y++)    //* would loop to 4 if using true 4x4
	{
		b=m_data[y+x1];
		c=m_data[y+x2];

		m_data[y+x1]=c*sina+b*cosa;
		m_data[y+x2]=c*cosa-b*sina;
	}
#else
	//* unrolled
	U32 x1=index[axis][0];
	U32 x2=index[axis][1];

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;
#endif

	return *this;
}

IFXINLINE IFXMatrix4x4 &IFXMatrix4x4::Scale3x4(const IFXVector3 &vector)
{
	const F32 *v=vector.RawConst();

	m_data[0]*=v[0];
	m_data[1]*=v[0];
	m_data[2]*=v[0];

	m_data[4]*=v[1];
	m_data[5]*=v[1];
	m_data[6]*=v[1];

	m_data[8]*=v[2];
	m_data[9]*=v[2];
	m_data[10]*=v[2];

	return *this;
}

IFXINLINE BOOL IFXMatrix4x4::NegativelyScaled()
{
	/* Effectively returns (((c0 cross c1) dot c2) < 0) */
	return (((m_data[1]*m_data[6]-m_data[2]*m_data[5])*m_data[8]
	+(m_data[2]*m_data[4]-m_data[0]*m_data[6])*m_data[9]
	+(m_data[0]*m_data[5]-m_data[1]*m_data[4])*m_data[10]) < 0.0);
}

// NOTE limited 3x3+3 3-component multiplication, not 4x4 full multiplication
IFXINLINE void IFXMatrix4x4::TransformVector(const IFXVector3 &source,
											 IFXVector3 &result) const
{
	const F32 *in=source.RawConst();
	F32 *out=result.Raw();
	IFXASSERT(in!=out);

	// unrolled
	out[0]=in[0]*m_data[0]+in[1]*m_data[4]+in[2]*m_data[8]+m_data[12];
	out[1]=in[0]*m_data[1]+in[1]*m_data[5]+in[2]*m_data[9]+m_data[13];
	out[2]=in[0]*m_data[2]+in[1]*m_data[6]+in[2]*m_data[10]+m_data[14];
}

// NOTE limited 3x3+3 3-component multiplication, not 4x4 full multiplication
// Divides by transformed w
IFXINLINE void IFXMatrix4x4::ProjectVector(const IFXVector3 &source,
										   IFXVector3 &result) const
{
	const F32 *in=source.RawConst();
	F32 *out=result.Raw();
	F32 w;
	IFXASSERT(in!=out);

	// unrolled -
	w=1.0f/(in[0]*m_data[3]+in[1]*m_data[7]+in[2]*m_data[11]+m_data[15] );
	out[0]=in[0]*m_data[0]+in[1]*m_data[4]+in[2]*m_data[8]+m_data[12];
	out[1]=in[0]*m_data[1]+in[1]*m_data[5]+in[2]*m_data[9]+m_data[13];
	out[2]=in[0]*m_data[2]+in[1]*m_data[6]+in[2]*m_data[10]+m_data[14];

	out[0]*=w;
	out[1]*=w;
	out[2]*=w;

}

IFXINLINE void IFXMatrix4x4::RotateVector(const IFXVector3 &source,
										  IFXVector3 &result) const
{
	const F32 *in=source.RawConst();
	F32 *out=result.Raw();
	IFXASSERT(in!=out);

	// unrolled
	out[0]=in[0]*m_data[0]+in[1]*m_data[4]+in[2]*m_data[8];
	out[1]=in[0]*m_data[1]+in[1]*m_data[5]+in[2]*m_data[9];
	out[2]=in[0]*m_data[2]+in[1]*m_data[6]+in[2]*m_data[10];
}

/*
IFXINLINE void IFXMatrix4x4::ApplyThreshold(F32 threshold)
{
U32 m;
for(m=0;m<16;m++)
{
I32 ivalue=I32((m_data[m]<0.0f)? m_data[m]-0.5f: m_data[m]+0.5f);

if(fabs(m_data[m]-(F32)ivalue) < threshold)
m_data[m]=(F32)ivalue;
}
}
*/

#endif /* __IFXMatrix4x4_h__ */

// eof
