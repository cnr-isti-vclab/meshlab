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

#ifndef IFXVector4_h
#define IFXVector4_h

#include "IFXDebug.h"
#include "IFXDataTypes.h"
#include "IFXVector3.h"


/**************************************************************************//**
\brief Vector of 4 Floats

There is no	default	initialization.	 The vector	is undefined unless	set.

IFXVector4 porting notes:
<UL>
<LI>IFXVector4 used	an SG_EPSILON of 0.0001f for comparisons.
IFXVector4 doesn't,	but	provides NearlyEquals()	with a margin
argument.
<LI>IFXVector4::operator=()	didn't copy	to 4th element,
IFXVector4 does.
</UL>
*//***************************************************************************/

/**This class handles vectors of four floats.*/
class IFXVector4
{
public:
	/// Construct without initializing.
	IFXVector4(void)          { }

	/** Construct using 3 explicit values.  The fourth is set
	to 1. */
	IFXVector4(F32 r,F32 g,F32 b)   { Set(r,g,b); }

	/// Construct using 4 explicit values.
	IFXVector4(F32 r,F32 g,F32 b,F32 a) { Set(r,g,b,a); }

	/** Construct using an array of at least 4 floats
	representing the 4 components in the vector. */
	IFXVector4(const F32 *floats)   { operator=(floats); }

	/// Copies constructor.
	IFXVector4(const IFXVector4 &operand) {operator=(operand);}

	/** Populates using a 3 component vector.
	The fourth component is set to zero. */
	IFXVector4(const IFXVector3 &operand) {operator=(operand);}

	/** Constructs the vector using an array of at least 3
	floats representing the 3 components in the vector. */
	IFXVector4  &operator=(const F32 *floats);

	/// Deep copy.
	IFXVector4  &operator=(const IFXVector4 &operand);

	/** Populates this vector using a 3 component vector.
	The fourth component is set to zero. */
	IFXVector4  &operator=(const IFXVector3 &operand);

	/// Casts operator to an IFXVector3
	operator IFXVector3&(void) { return *(IFXVector3*)this; }

	/// Exact compare operation (zero tolerance).
	BOOL    operator==(const IFXVector4& operand) const;

	/// Exact compare operation (zero tolerance).
	BOOL    operator!=(const IFXVector4& operand) const;

	/** Returns TRUE if each component in the operand is
	with the given margin from the value of the
	corresponding component in this vector. */
	BOOL    NearlyEquals(const IFXVector4& operand,F32 margin) const;

	/// Resets vector to <0,0,0,1>
	void    Reset(void);

	/// Sets all 4 RGBA or XYZH components (4th defaults to 1.0)
	void    Set(F32 r, F32 g, F32 b, F32 a=1.0f);

	/// Sets the first 3 components explicitly (4th is unchanged).
	void    Set3(F32 r,F32 g,F32 b);

	/** Returns a pointer to the internal float array
	containing the 4 floats.  This is not a copy. */
	F32     *Raw(void)              { return m_value; }

	/// Returns a const pointer to the internal float array.
	const F32     *RawConst(void) const       { return m_value; }

	/// Accesses a component of the internal float array.
	F32     &operator[](U32 n);

	/// Accesses a component as const.
	F32     operator[](U32 n) const;

	/// Accesses element using an index in the range 0 to 3
	F32&    Value(U32 index);

	/// Accesses an indexed element as const
	F32     Value(U32 index) const;
	/// Accesses the Red component (1st)
	F32&    R(void)               { return m_value[0]; }
	/// Accesses the Green component (2nd)
	F32&    G(void)               { return m_value[1]; }
	/// Accesses the Blue component (3rd)
	F32&    B(void)               { return m_value[2]; }
	/// Accesses the Alpha component (4th)
	F32&    A(void)               { return m_value[3]; }

	/// Accesses the X component (1st)
	F32&    X(void)               { return m_value[0]; }
	/// Accesses the Y component (2nd)
	F32&    Y(void)               { return m_value[1]; }
	/// Accesses the Z component (3rd)
	F32&    Z(void)               { return m_value[2]; }
	/// Accesses the H component (4th)
	F32&    H(void)               { return m_value[3]; }

	/// Accesses the U component (1st)
	F32&    U(void)               { return m_value[0]; }
	/// Accesses the V component (2nd)
	F32&    V(void)               { return m_value[1]; }
	/// Accesses the W component (3rd)
	F32&    W(void)               { return m_value[2]; }
	/// Accesses the T component (4th)
	F32&    T(void)               { return m_value[3]; }

	/// Accesses the Red component (1st) as const
	F32     R(void) const           { return m_value[0]; }
	/// Accesses the Green component (2nd) as const
	F32     G(void) const           { return m_value[1]; }
	/// Accesses the Blue component (3rd) as const
	F32     B(void) const           { return m_value[2]; }
	/// Accesses the Alpha component (4th) as const
	F32     A(void) const           { return m_value[3]; }

	/// Accesses the X component (1st) as const
	F32     X(void) const           { return m_value[0]; }
	/// Accesses the Y component (2nd) as const
	F32     Y(void) const           { return m_value[1]; }
	/// Accesses the Z component (3rd) as const
	F32     Z(void) const           { return m_value[2]; }
	/// Accesses the H component (4th) as const
	F32     H(void) const           { return m_value[3]; }

	/// Accesses the U component (1st) as const
	F32     U(void) const           { return m_value[0]; }
	/// Accesses the V component (2nd) as const
	F32     V(void) const           { return m_value[1]; }
	/// Accesses the W component (3rd) as const
	F32     W(void) const           { return m_value[2]; }
	/// Accesses the T component (4th) as const
	F32     T(void) const           { return m_value[3]; }

	/// Accesses the Radius component (4th)
	F32&    Radius(void)            { return m_value[3]; }

	/// Accesses the Radius component (4th) as const
	F32     Radius(void) const          { return m_value[3]; }

	/// Adds the operand into this vector.
	IFXVector4  &Add(const IFXVector4& right);

	/// Populates this vector with the sum of the operands.
	IFXVector4  &Add(const IFXVector4& left, const IFXVector4& right);

	/// Subtracts the operand out of this vector.
	IFXVector4  &Subtract(const IFXVector4& right);

	/** Populates this vector with the difference of the
	operands. */
	IFXVector4  &Subtract(const IFXVector4& left, const IFXVector4& right);

	/** Multiplies each component in the operand into
	the corresponding component of this vector. */
	IFXVector4  &Multiply(const IFXVector4& right);

	/** Populates components of this vector with the product
	of the corresponding components of the operands. */
	IFXVector4  &Multiply(const IFXVector4& left, const IFXVector4& right);

	/** Multiplies each component of this vector by the
	given scalar. */
	IFXVector4  &Scale(F32 scalar);

	/** Multiplies first 3 components of this vector by the
	given scalar. */
	IFXVector4  &Scale3(const F32 fScale);

	/** Sets each component of this vector to the minimum of
	that component's current value and the corresponding
	component of the operand. */
	IFXVector4  &Min(const IFXVector4& operand);

	/** Sets each component of this vector to the minimum of
	that component's current value and the corresponding
	component of the given float array. */
	IFXVector4  &Min(const F32* floats);

	/** Sets each component of this vector to the maximum of
	that component's current value and the corresponding
	component of the operand. */
	IFXVector4  &Max(const IFXVector4& operand);

	/** Sets each component of this vector to the maximum of
	that component's current value and the corresponding
	component of the given float array. */
	IFXVector4  &Max(const F32* floats);

	/** Returns the dot product of this vector's first 3
	components with the first 3 components of the operand.
	If you expect to take the acos() of result to find an
	angle, both vectors must be  normalized before calling
	this method. */
	F32     DotProduct3(const IFXVector4 &operand) const;

	/** Populates this vector with the cross product of
	the first 3 components of the two operands.  The result
	will be normalized if the operands were normalized. */
	IFXVector4  &CrossProduct3(const IFXVector4 &left,
		const IFXVector4 &right);

	/** Returns the length of the 3-vector implied by the first
	3 components. */
	F32     CalcMagnitude3(void) const;

	/** Returns the square length of the 3-vector implied by the
	first 3 components.  This is faster than computing the
	actual length. */
	F32     CalcMagnitudeSquared3(void) const;

	/// Scales the first 3 components to a length of 1.
	IFXVector4  &Normalize3(void);

	F32     CalcDistanceFrom(const IFXVector4 &operand) const;

	/** Treating the forth element as radius, this function
	sets this "sphere" as the collective bounds of this
	current sphere and the given operand's sphere. */
	void    IncorporateSphere(const IFXVector4 &bound);

	/** Treating the forth element as radius, this function
	sets this "sphere" as the collective bounds of this
	current sphere and the given operand's sphere. */
	void    IncorporateSphere(const IFXVector4 *pBound);

	/** Gets the RGBA	representation of the vector as	a single
	integer. This is useful for setting colors in D3D.
	This assumes that colors as held in	the	floating point vector
	are in the range from 0.0 - 1.0.
	Colors are clamped to 0.0 -	1.0	range during conversion	
	to U32.	 The vector	is not changed by the clamping.*/
    U32     GetRGBA() const;

	/** Gets the BGRA	representation of the vector as	a single
	integer. This is useful for setting colors in D3D.
	This assumes that colors as held in	the	floating point vector
	are in the range from 0.0 - 1.0.
	Colors are clamped to 0.0 -	1.0	range during conversion	
	to U32.	 The vector	is not changed by the clamping.*/
    U32     GetBGRA() const;

	/** Performs RGBA<->BGRA switch */
	void    BGR();

	/** Generates a string representing the contents of
	this vector. */
	IFXString Out(BOOL verbose=false) const;

private:
	F32 m_value[4];
};

IFXINLINE IFXVector4 &IFXVector4::operator=(const F32 *floats)
{
	// 4 floats assumed
	m_value[0]=floats[0];
	m_value[1]=floats[1];
	m_value[2]=floats[2];
	m_value[3]=floats[3];
	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::operator=(const IFXVector3 &operand)
{
	const F32 *v=operand.RawConst();
	m_value[0]=v[0];
	m_value[1]=v[1];
	m_value[2]=v[2];
	m_value[3]=1.0f;
	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::operator=(const IFXVector4 &operand)
{
	m_value[0]=operand.m_value[0];
	m_value[1]=operand.m_value[1];
	m_value[2]=operand.m_value[2];
	m_value[3]=operand.m_value[3];
	return *this;
}

IFXINLINE BOOL IFXVector4::operator==(const IFXVector4 &operand) const
{
	// Unrolled to reduce branch count to 1.
	if( m_value[0] != operand.m_value[0]
	|| m_value[1] != operand.m_value[1]
	|| m_value[2] != operand.m_value[2]
	|| m_value[3] != operand.m_value[3] )
		return false;

	return true;

	/* IFXVector4 version:
	return ((fabs(x - v.x) < SG_EPSILON) &&
	(fabs(y - v.y) < SG_EPSILON) &&
	(fabs(z - v.z) < SG_EPSILON));
	*/
}

IFXINLINE BOOL IFXVector4::operator!=(const IFXVector4 &operand) const
{
	return !(*this==operand);
}

IFXINLINE BOOL IFXVector4::NearlyEquals(const IFXVector4& operand,
										F32 margin) const
{
	return (fabs(m_value[0]-operand.m_value[0])<margin &&
		fabs(m_value[1]-operand.m_value[1])<margin &&
		fabs(m_value[2]-operand.m_value[2])<margin);
}

IFXINLINE void IFXVector4::Reset(void)
{
	m_value[0] = 0.0f;
	m_value[1] = 0.0f;
	m_value[2] = 0.0f;
	m_value[3] = 1.0f;
}

IFXINLINE void IFXVector4::Set(F32 r,F32 g,F32 b,F32 a)
{
	m_value[0] = r;
	m_value[1] = g;
	m_value[2] = b;
	m_value[3] = a;
}

IFXINLINE void IFXVector4::Set3(F32 r,F32 g,F32 b)
{
	m_value[0] = r;
	m_value[1] = g;
	m_value[2] = b;
}

IFXINLINE F32& IFXVector4::Value(U32 index)
{
	IFXASSERT(index < 4);
	return m_value[index];
}

IFXINLINE IFXVector4 &IFXVector4::Multiply(const IFXVector4& right)
{
	m_value[0] *= right.m_value[0];
	m_value[1] *= right.m_value[1];
	m_value[2] *= right.m_value[2];
	m_value[3] *= right.m_value[3];

	return *this;
}

IFXINLINE F32 &IFXVector4::operator[](U32 n)
{
	IFXASSERT(n<4);
	return m_value[n];
}

IFXINLINE F32 IFXVector4::operator[](U32 n) const
{
	IFXASSERT(n<4);
	return m_value[n];
}

IFXINLINE IFXVector4 &IFXVector4::Multiply(const IFXVector4& left,
										   const IFXVector4& right)
{
	m_value[0] = left.m_value[0] * right.m_value[0];
	m_value[1] = left.m_value[1] * right.m_value[1];
	m_value[2] = left.m_value[2] * right.m_value[2];
	m_value[3] = left.m_value[3] * right.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Add(const IFXVector4& right)
{
	m_value[0] += right.m_value[0];
	m_value[1] += right.m_value[1];
	m_value[2] += right.m_value[2];
	m_value[3] += right.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Add(const IFXVector4& left,
									  const IFXVector4& right)
{
	m_value[0] = left.m_value[0] + right.m_value[0];
	m_value[1] = left.m_value[1] + right.m_value[1];
	m_value[2] = left.m_value[2] + right.m_value[2];
	m_value[3] = left.m_value[3] + right.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Min(const IFXVector4& operand)
{
	if(operand.m_value[0] < m_value[0]) m_value[0] = operand.m_value[0];
	if(operand.m_value[1] < m_value[1]) m_value[1] = operand.m_value[1];
	if(operand.m_value[2] < m_value[2]) m_value[2] = operand.m_value[2];
	if(operand.m_value[3] < m_value[3]) m_value[3] = operand.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Min(const F32* floats)
{
	if(floats[0] < m_value[0]) m_value[0] = floats[0];
	if(floats[1] < m_value[1]) m_value[1] = floats[1];
	if(floats[2] < m_value[2]) m_value[2] = floats[2];
	if(floats[3] < m_value[3]) m_value[3] = floats[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Max(const IFXVector4& operand)
{
	if(operand.m_value[0] > m_value[0]) m_value[0] = operand.m_value[0];
	if(operand.m_value[1] > m_value[1]) m_value[1] = operand.m_value[1];
	if(operand.m_value[2] > m_value[2]) m_value[2] = operand.m_value[2];
	if(operand.m_value[3] > m_value[3]) m_value[3] = operand.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Max(const F32* floats)
{
	if(floats[0] > m_value[0]) m_value[0] = floats[0];
	if(floats[1] > m_value[1]) m_value[1] = floats[1];
	if(floats[2] > m_value[2]) m_value[2] = floats[2];
	if(floats[3] > m_value[3]) m_value[3] = floats[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Subtract(const IFXVector4& right)
{
	m_value[0] -= right.m_value[0];
	m_value[1] -= right.m_value[1];
	m_value[2] -= right.m_value[2];
	m_value[3] -= right.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Subtract(const IFXVector4& left,
										   const IFXVector4& right)
{
	m_value[0] = left.m_value[0] - right.m_value[0];
	m_value[1] = left.m_value[1] - right.m_value[1];
	m_value[2] = left.m_value[2] - right.m_value[2];
	m_value[3] = left.m_value[3] - right.m_value[3];

	return *this;
}

IFXINLINE IFXVector4 &IFXVector4::Scale3(const F32 fScale)
{
	m_value[0] *= fScale;
	m_value[1] *= fScale;
	m_value[2] *= fScale;

	return *this;
}

IFXINLINE F32 IFXVector4::DotProduct3(const IFXVector4 &operand) const
{
	IFXVector3 left=RawConst(),right=operand.RawConst();
	return left.DotProduct(right);
}

IFXINLINE F32 IFXVector4::CalcDistanceFrom(const IFXVector4 &operand) const
{
	IFXVector3 left=RawConst(),right=operand.RawConst();
	return left.CalcDistanceFrom(right);
}


IFXINLINE IFXVector4 &IFXVector4::CrossProduct3(const IFXVector4 &left,
												const IFXVector4 &right)
{
	IFXVector3 result,left3=left.RawConst(),right3=right.RawConst();
	result.CrossProduct(left3,right3);
	operator=(result.RawConst());
	return *this;
}

IFXINLINE F32 IFXVector4::CalcMagnitude3(void) const
{
	IFXVector3 vector=m_value;
	return vector.CalcMagnitude();
}

IFXINLINE F32 IFXVector4::CalcMagnitudeSquared3(void) const
{
	IFXVector3 vector=m_value;
	return vector.CalcMagnitudeSquared();
}

IFXINLINE IFXVector4 &IFXVector4::Normalize3(void)
{
	IFXVector3 vector=m_value;
	if(vector.IsNonZero())
		return Scale3(1.0f/vector.CalcMagnitude());

	m_value[0]=0.0f;
	m_value[1]=0.0f;
	m_value[2]=0.0f;
	return *this;
}

IFXINLINE F32 IFXVector4::Value(U32 index) const
{
	IFXASSERT(index < 4);
	return m_value[index];
}

IFXINLINE U32 IFXVector4::GetRGBA() const
{
	F32 v0 = m_value[0];
	F32 v1 = m_value[1];
	F32 v2 = m_value[2];
	F32 v3 = m_value[3];

	v0 = IFXMAX( v0, 0.0f );
	v0 = IFXMIN( v0, 1.0f );
	v1 = IFXMAX( v1, 0.0f );
	v1 = IFXMIN( v1, 1.0f );
	v2 = IFXMAX( v2, 0.0f );
	v2 = IFXMIN( v2, 1.0f );
	v3 = IFXMAX( v3, 0.0f );
	v3 = IFXMIN( v3, 1.0f );
	
	return SET_ENDIAN32(((U32)(v3*255) << 24) | ((U32)(v2*255) << 16) |
						((U32)(v1*255) <<  8) | ((U32)(v0*255) ) );
}

IFXINLINE U32 IFXVector4::GetBGRA() const
{
	F32 v0 = m_value[0];
	F32 v1 = m_value[1];
	F32 v2 = m_value[2];
	F32 v3 = m_value[3];

	v0 = IFXMAX( v0, 0.0f );
	v0 = IFXMIN( v0, 1.0f );
	v1 = IFXMAX( v1, 0.0f );
	v1 = IFXMIN( v1, 1.0f );
	v2 = IFXMAX( v2, 0.0f );
	v2 = IFXMIN( v2, 1.0f );
	v3 = IFXMAX( v3, 0.0f );
	v3 = IFXMIN( v3, 1.0f );
	
	return SET_ENDIAN32(((U32)(v3*255) << 24) | ((U32)(v0*255) << 16) |
						((U32)(v1*255) <<  8) | ((U32)(v2*255) ) );
}

IFXINLINE void IFXVector4::BGR()
{
	F32 temp = m_value[0];
	m_value[0] = m_value[2];
	m_value[2] = temp;
}

IFXINLINE IFXVector4 &IFXVector4::Scale(F32 scalar)
{
	m_value[0]*=scalar;
	m_value[1]*=scalar;
	m_value[2]*=scalar;
	m_value[3]*=scalar;

	return *this;
}

#endif
