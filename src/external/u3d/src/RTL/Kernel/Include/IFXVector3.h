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

#ifndef __IFXVector3_h__
#define __IFXVector3_h__

#include "IFXDebug.h"
#include "IFXDataTypes.h"
#include "IFXString.h"
#include "IFXVector2.h"

/**
	This class handles vectors of three floats.

	@note	There is no default initialization. The vector is undefined unless set.
*/
class IFXVector3
{
public:
	/// Construct without initializing.
	IFXVector3(void)        { }

	/// Construct using 3 explicit values.
	IFXVector3(F32 x,F32 y,F32 z) { Set(x,y,z); }

	/** Construct using an array of at least 3 floats
	representing the 3 components in the vector. */
	IFXVector3(const float *floats) { operator=(floats); }

	/// Copies constructor.
	IFXVector3(const IFXVector3 &operand)
	{ operator=(operand); }

	/** Constructs using a 2 component vector.
	The third component is set to zero. */
	IFXVector3(const IFXVector2 &operand)
	{ operator=(operand); }

	/// Sets all three components to zero.
	void      Reset(void);

	/// Explicitly sets the three components in the vector.
	void      Set(F32 x,F32 y,F32 z);

	/** Populates the vector using an array of at least 3
	floats representing the 3 components in the
	vector. */
	IFXVector3    &operator=(const F32 *floats);

	/// Deep copy.
	IFXVector3    &operator=(const IFXVector3 &operand);

	/** Populates this vector using a 2 component vector.
	The third component is set to zero. */
	IFXVector3    &operator=(const IFXVector2 &operand);

	/// Exact compare operation (zero tolerance).
	BOOL      operator==(const IFXVector3 &operand) const;

	/** Returns TRUE if each component in the operand is
	with the given margin from the value of the
	corresponding component in this vector. */
	BOOL      IsApproximately(const IFXVector3 &operand,
		F32 margin) const;

	/** Returns a pointer to the internal float array
	containing the 3 floats.  This is not a copy. */
	F32       *Raw(void)            { return m_value; };

	/// Returns a const pointer to the internal float array.
	const F32       *RawConst(void) const     { return m_value; };

	/// Accesses a component of the internal float array.
	F32       &operator[](U32 n);

	/// Accesses a component as const.
	F32       operator[](U32 n) const;

	/// Accesses a component of the internal float array.
	F32       &Value(U32 index);

	/// Accesses a component as const.
	F32       Value(U32 index) const;

	/// Accesses the first component of the vector.
	F32       &X(void)            { return m_value[0]; }

	/// Accesses the second component of the vector.
	F32       &Y(void)            { return m_value[1]; }

	/// Accesses the third component of the vector.
	F32       &Z(void)            { return m_value[2]; }

	/// Accesses the first component of the vector as const.
	F32       XC(void)  const       { return m_value[0]; }

	/// Accesses the second component of the vector as const.
	F32       YC(void)  const       { return m_value[1]; }

	/// Accesses the third component of the vector as const.
	F32       ZC(void)  const       { return m_value[2]; }

	/// Returns true if any component is non-zero.
	BOOL      IsNonZero(void) const;

	/// Returns true if any component is zero.
	BOOL      HasZeroComponent(void) const;

	/// Adds the operand into this vector.
	IFXVector3    &Add(const IFXVector3 &operand);

	/// Populates this vector with the sum of the operands.
	IFXVector3    &Add(const IFXVector3 &left,const IFXVector3 &right);

	/// Scales the operand and add into this vector.
	IFXVector3    &AddScaled(const IFXVector3 &operand,F32 scalar);

	/// Subtracts the operand out of this vector.
	IFXVector3    &Subtract(const IFXVector3 &operand);

	/** Populates this vector with the difference of the
	operands. */
	IFXVector3    &Subtract(const IFXVector3 &left,
		const IFXVector3 &right);

	/** Multiplies each component in the operand into
	the corresponding component of this vector. */
	IFXVector3    &Multiply(const IFXVector3 &operand);

	/** Populates components of this vector with the product
	of the corresponding components of the operands. */
	IFXVector3    &Multiply(const IFXVector3 &left,
		const IFXVector3 &right);

	/** Divides each component in the vector with the
	corresponding component in the operand.
	Attempted division by zero results in
	no operation (on a per-component basis). */
	IFXVector3    &Divide(const IFXVector3 &operand);

	/** Divides each component in the left vector with the
	corresponding component in the right vector and
	store the result in this vector.
	Attempted division by zero results in
	no operation (on a per-component basis). */
	IFXVector3    &Divide(const IFXVector3 &left,const IFXVector3 &right);

	/** Multiplies each components of this vector by the
	given scalar. */
	IFXVector3    &Scale(F32 scalar);

	/** Sets the length of the vector
	(normalize, then scale). */
	IFXVector3    &Rescale(F32 magnitude);

	/// Multiplies all components by -1 (additive inverse).
	IFXVector3    &Negate(void);

	/** Divides each component into 1 and put the result
	back into the component (multiplicative inverse). */
	IFXVector3    &Reciprocate(void);

	/// Alias for Negate().
	IFXVector3    &Invert(void)         { return Negate(); };

	/** Returns the dot product of this vector with the
	operand.  If you expect to take the acos() of
	result to find an angle, both vectors must
	be  normalized before calling this method.
	Consider using CalcCosAngleFrom() if the vectors
	are not normalized. */
	F32       DotProduct(const IFXVector3 &operand) const;

	/** Returns the dot product of the two operands.
	If you expect to take the acos() of
	result to find an angle, both vectors must
	be  normalized before calling this method.
	Consider using CalcCosAngleFrom() if the vectors
	are not normalized. */
	static  F32       DotProduct(const IFXVector3 &left,
		const IFXVector3 &right);

	/** Populates this vector with the cross product of
	the two operands.  The result will be normalized
	if the operands were normalized. */
	IFXVector3    &CrossProduct(const IFXVector3 &left,
		const IFXVector3 &right);

	/// @todo: consider having Normalize just call SafeNormalize

	/// Scales the vector to a length of 1.
	IFXVector3    &Normalize(void);

	/** Scales the vector to a length of 1, but protect
	against divide by zero. */
	IFXVector3    &SafeNormalize(void);

	/// Returns the length of the vector.
	F32       CalcMagnitude(void) const;

	/** Returns the square of the length of the vector.
	This is faster than computing the actual length. */
	F32       CalcMagnitudeSquared(void) const;

	/** Returns the distance between this vector and
	the operand, presuming they both represent
	positions. */
	F32       CalcDistanceFrom(const IFXVector3 &operand) const;

	/** Returns the square of the distance between this
	vector and the operand.  This is faster than
	computing the actual distance. */
	F32       CalcDistanceSquaredFrom(const IFXVector3 &operand)const;

	/** Computes the angle between this vector and the
	operand.  They do not have to be normalized.
	DotProduct() is faster if they are normalized. */
	F32       CalcCosAngleFrom(const IFXVector3 &operand)const;
	/** Computes the square of the angle between this vector
	and the operand.  This is faster than computing
	the actual angle. */
	F32       CalcCosAngleSquaredFrom(const IFXVector3 &operand)const;

	/** Populates this vector by the interpolated value
	from the first vector to the second vector by
	the fraction t, where t=0 results in 'from' and
	t=1 results in 'to'. */
	void      Interpolate(F32 t,const IFXVector3 &from,
		const IFXVector3 &to);

	/** Generates a string representing the contents of
	this vector. */
	IFXString   Out(BOOL verbose=false) const;

	// not IFXINLINE:

private:
	F32       m_value[3];
};

IFXINLINE void IFXVector3::Reset(void)
{
	m_value[0]=0.0f;
	m_value[1]=0.0f;
	m_value[2]=0.0f;
}

IFXINLINE void IFXVector3::Set(F32 x,F32 y,F32 z)
{
	m_value[0]=x;
	m_value[1]=y;
	m_value[2]=z;
}

IFXINLINE F32 &IFXVector3::operator[](U32 n)
{
	IFXASSERT(n<3);
	return m_value[n];
}
IFXINLINE F32 IFXVector3::operator[](U32 n) const
{
	IFXASSERT(n<3);
	return m_value[n];
}

IFXINLINE F32 &IFXVector3::Value(U32 index)
{
	IFXASSERT(index<3);
	return m_value[index];
}
IFXINLINE F32 IFXVector3::Value(U32 index) const
{
	IFXASSERT(index<3);
	return m_value[index];
}

IFXINLINE BOOL IFXVector3::IsNonZero(void) const
{
	return (m_value[0]!=0.0f || m_value[1]!=0.0f || m_value[2]!=0.0f);
}

IFXINLINE BOOL IFXVector3::HasZeroComponent(void) const
{
	return (m_value[0]==0.0f || m_value[1]==0.0f || m_value[2]==0.0f);
}

IFXINLINE IFXVector3 &IFXVector3::operator=(const F32 *floats)
{
	// 3 floats assumed

	m_value[0]=floats[0];
	m_value[1]=floats[1];
	m_value[2]=floats[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::operator=(const IFXVector3 &operand)
{
	m_value[0]=operand.m_value[0];
	m_value[1]=operand.m_value[1];
	m_value[2]=operand.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::operator=(const IFXVector2 &operand)
{
	m_value[0]=operand.RawConst()[0];
	m_value[1]=operand.RawConst()[1];
	m_value[2]=0.0f;

	return *this;
}

/**
	@todo Watch out when comparing floating point values:
         0x80000000 is negative zero (-1.0 * 2**-127), and
         0x00000000 is positive zero (1.0 * 2**-127)
	This method will return false when comparing negative zero with positive zero
*/
IFXINLINE BOOL IFXVector3::operator==(const IFXVector3 &operand) const
{
	// Unrolled to reduce branch count to 1.
	if( m_value[0] != operand.m_value[0]
	|| m_value[1] != operand.m_value[1]
	|| m_value[2] != operand.m_value[2] )
		return false;

	return true;
}

IFXINLINE BOOL IFXVector3::IsApproximately(
	IFXVector3 const &operand,
	F32 margin) const
{
	BOOL res = FALSE;

	if( fabs(m_value[0]-operand.m_value[0])<margin &&
		fabs(m_value[1]-operand.m_value[1])<margin &&
		fabs(m_value[2]-operand.m_value[2])<margin )
	{
		res = TRUE;
	}
	else
	{
		res = FALSE;
	}

	return res;
}

IFXINLINE IFXVector3 &IFXVector3::Add(const IFXVector3 &operand)
{
	m_value[0]+=operand.m_value[0];
	m_value[1]+=operand.m_value[1];
	m_value[2]+=operand.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Add(const IFXVector3 &left,
									  const IFXVector3 &right)
{
	m_value[0]=left.m_value[0]+right.m_value[0];
	m_value[1]=left.m_value[1]+right.m_value[1];
	m_value[2]=left.m_value[2]+right.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::AddScaled(const IFXVector3 &operand,
											F32 scalar)
{
	m_value[0]+=operand.m_value[0]*scalar;
	m_value[1]+=operand.m_value[1]*scalar;
	m_value[2]+=operand.m_value[2]*scalar;

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Subtract(const IFXVector3 &operand)
{
	m_value[0]-=operand.m_value[0];
	m_value[1]-=operand.m_value[1];
	m_value[2]-=operand.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Subtract(const IFXVector3 &left,
										   const IFXVector3 &right)
{
	m_value[0]=left.m_value[0]-right.m_value[0];
	m_value[1]=left.m_value[1]-right.m_value[1];
	m_value[2]=left.m_value[2]-right.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Multiply(const IFXVector3 &operand)
{
	m_value[0]*=operand.m_value[0];
	m_value[1]*=operand.m_value[1];
	m_value[2]*=operand.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Multiply(const IFXVector3 &left,
										   const IFXVector3 &right)
{
	m_value[0]=left.m_value[0]*right.m_value[0];
	m_value[1]=left.m_value[1]*right.m_value[1];
	m_value[2]=left.m_value[2]*right.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Divide(const IFXVector3 &operand)
{
	IFXASSERT(!operand.HasZeroComponent());

	if(operand.m_value[0]!=0.0f)
		m_value[0]/=operand.m_value[0];
	if(operand.m_value[1]!=0.0f)
		m_value[1]/=operand.m_value[1];
	if(operand.m_value[2]!=0.0f)
		m_value[2]/=operand.m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Divide(const IFXVector3 &left,
										 const IFXVector3 &right)
{
	IFXASSERT(!right.HasZeroComponent());

	if(right.m_value[0]!=0.0f)
		m_value[0]=left.m_value[0]/right.m_value[0];
	if(right.m_value[1]!=0.0f)
		m_value[1]=left.m_value[1]/right.m_value[1];
	if(right.m_value[2]!=0.0f)
		m_value[2]=left.m_value[2]/right.m_value[2];

	return *this;
}


IFXINLINE IFXVector3 &IFXVector3::CrossProduct(const IFXVector3 &left,
											   const IFXVector3 &right)
{
	IFXASSERT(this!=&left);
	IFXASSERT(this!=&right);
	m_value[0]= left.m_value[1]*right.m_value[2]-
		left.m_value[2]*right.m_value[1];
	m_value[1]= left.m_value[2]*right.m_value[0]-
		left.m_value[0]*right.m_value[2];
	m_value[2]= left.m_value[0]*right.m_value[1]-
		left.m_value[1]*right.m_value[0];

	return *this;
}

IFXINLINE F32 IFXVector3::DotProduct(const IFXVector3 &left,
									 const IFXVector3 &right)
{
	return  left.m_value[0]*right.m_value[0]+
		left.m_value[1]*right.m_value[1]+
		left.m_value[2]*right.m_value[2];
}

IFXINLINE F32 IFXVector3::DotProduct(const IFXVector3 &operand) const
{
	return  m_value[0]*operand.m_value[0]+
		m_value[1]*operand.m_value[1]+
		m_value[2]*operand.m_value[2];
}

IFXINLINE IFXVector3 &IFXVector3::Scale(F32 scalar)
{
	m_value[0]*=scalar;
	m_value[1]*=scalar;
	m_value[2]*=scalar;

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Negate(void)
{
	m_value[0]= -m_value[0];
	m_value[1]= -m_value[1];
	m_value[2]= -m_value[2];

	return *this;
}

IFXINLINE IFXVector3 &IFXVector3::Reciprocate(void)
{
	m_value[0]= (m_value[0]==0.0f)? 0.0f: 1.0f/m_value[0];
	m_value[1]= (m_value[1]==0.0f)? 0.0f: 1.0f/m_value[1];
	m_value[2]= (m_value[2]==0.0f)? 0.0f: 1.0f/m_value[2];

	return *this;
}

IFXINLINE F32 IFXVector3::CalcMagnitude(void) const
{
#if FALSE
	return ifxFastMagnitude(m_value);
#else
	return sqrtf(m_value[0]*m_value[0]+
		m_value[1]*m_value[1]+
		m_value[2]*m_value[2]);
#endif
}

IFXINLINE F32 IFXVector3::CalcMagnitudeSquared(void) const
{
	return m_value[0]*m_value[0]+m_value[1]*m_value[1]+m_value[2]*m_value[2];
}

IFXINLINE F32 IFXVector3::CalcDistanceFrom(
	const IFXVector3 &operand) const
{
	return sqrtf(CalcDistanceSquaredFrom(operand));
}

IFXINLINE F32 IFXVector3::CalcDistanceSquaredFrom(
	const IFXVector3 &operand) const
{
	F32 part=m_value[0]-operand.m_value[0];
	F32 sum=part*part;

	part=m_value[1]-operand.m_value[1];
	sum+=part*part;

	part=m_value[2]-operand.m_value[2];
	return sum+part*part;
}

IFXINLINE F32 IFXVector3::CalcCosAngleFrom(const IFXVector3 &operand) const
{
	return sqrtf(CalcCosAngleSquaredFrom(operand));
}

IFXINLINE F32 IFXVector3::CalcCosAngleSquaredFrom(
	const IFXVector3 &operand) const
{
	IFXASSERT(IsNonZero());
	IFXASSERT(operand.IsNonZero());

	F32 dot=DotProduct(operand);
	return ( (dot*dot) /
		(CalcMagnitudeSquared()*operand.CalcMagnitudeSquared()) );
}

IFXINLINE IFXVector3 &IFXVector3::Normalize(void)
{
	IFXASSERT(IsNonZero());
	return Scale(1.0f/CalcMagnitude());
}

IFXINLINE IFXVector3 &IFXVector3::SafeNormalize(void)
{
	F32 fMag = CalcMagnitude();
	if(fMag)
		return Scale(1.0f / fMag);
	else
		return (*this);
}

IFXINLINE IFXVector3 &IFXVector3::Rescale(F32 magnitude)
{
	IFXASSERT(IsNonZero());
	return Scale(magnitude/CalcMagnitude());
}

#endif
