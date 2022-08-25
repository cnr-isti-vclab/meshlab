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
	@file	IFXQuaternion.h

			This module contains the IFXQuaternion class used for 
			representing and performing 3D rotations.

	@note	Several functions in this module are modified versions of Nick 
			Bobic's (nick@wiplay.com) GLquat.c source code
			(http://www.gdmag.com/src/feb98.zip) that's associated with his
			"Rotating Objects Using Quaternions" Game Developer article
			(http://www.gamasutra.com/features/19980703/quaternions_01.htm)
			from February 1998.  For additional details about his source 
			code and the license associated with it refer to the 
			IFXQuaternion class description.
*/

#ifndef __IFXQuaternion_h__
#define __IFXQuaternion_h__

#include "IFXMatrix4x4.h"
#include "IFXVector4.h"
#include "IFXDebug.h"

#define IFXUSE_SLERP	TRUE		// use non-linear interpolation for quats
#define IFXSLERP_DELTA	(1e-6f)		// margin under which use linear always
#define IFXDELTA		(1e-4f)		// for equivalence comparisons
#define IFXALMOST1		(0.999999f)	// for trivial dot products


/**
	Four-dimensional complex number used to represent 3D rotations.

	Formated as <w,x,y,z> with right-handed logic.  Most operations rely
	on the quaternions being normalized.  Deviation from unit length can
	cause substantial differences from the expected results.

	@note	The non-trivial quaternion operations draw heavily from Nick 
			Bobic's (nick@wiplay.com) GLquat.c source code 
			(http://www.gdmag.com/src/feb98.zip) and his "Rotating Objects 
			Using Quaternions" Game Developer article 
			(http://www.gamasutra.com/features/19980703/quaternions_01.htm) 
			from February 1998.  Some functions containing a modified 
			version of his source code include operator=, Interpolate and 
			MakeRotation.

	@note	Nick Bobic has provided the following license terms to his 
			quaternion source code (http://www.gdmag.com/src/feb98.zip):
			@verbatim
			-------------------------------------------------------------
			This software is provided 'as-is', without any express or
			implied warranty.  In no event will Nick Bobic be held
			liable for any damages arising from the use of this software.

			Permission is granted to anyone to use this software for any
			purpose, including commercial applications, and to alter it
			and redistribute it freely, subject to the following
			restrictions:

			1. The origin of the software must not be misrepresented;
			you must not claim that you wrote the original software.
			If you use this software in a product, an acknowledgment
			in the product documentation would be appreciated but is
			not required.
			2. Altered source versions must be plainly marked as such,
			and must not be misrepresented as being the original
			software.
			3. This notice may not be removed or altered from any source
			distribution.
			-------------------------------------------------------------
			@endverbatim
*/
class IFXQuaternion
{
	public:
						/** Construct without initializing.*/
						IFXQuaternion(void)									
												{ m_data[0]=0; m_data[1]=0;
												  m_data[2]=0; m_data[3]=0; }

						/** Copies constructor.*/
						IFXQuaternion(const IFXQuaternion &operand)
												{ operator=(operand); }

						/** Construct using a rotation of a specified angle
							in radians about a given arbitrary axis. */
						IFXQuaternion(F32 radians,const IFXVector3 &axis)
												{ MakeRotation(radians,axis); }

						/** Construct with an explicit <w,x,y,z>.*/
						IFXQuaternion(F32 w,F32 x,F32 y,F32 z)
												{ Set(w,x,y,z); }

						/** Construct using an array of at least 4 floats.*/
						IFXQuaternion(const F32 *F32s)
												{ operator=(F32s); }

						/** Construct using a vector of 3 floats.
							The result is <0,x,y,z>. */
						IFXQuaternion(const IFXVector3 &operand)
												{ operator=(operand); }

						/** Construct by directly copying a Vector4.*/
						IFXQuaternion(const IFXVector4 &operand)
												{ operator=(operand); }

						/** Construct using the computed rotation of a matrix.*/
						IFXQuaternion(const IFXMatrix4x4 &matrix)
												{ operator=(matrix); }

						/** Deep copy.*/
		IFXQuaternion	&operator=(const IFXQuaternion &operand);

						/** Populates using an array of 4 floats.*/
		IFXQuaternion	&operator=(const F32 *F32s);

						/** Populate using an vector of 3 floats.
							The result is <0,x,y,z>. */
		IFXQuaternion	&operator=(const IFXVector3 &operand);

						/** Copies the elements of a Vector4.*/
		IFXQuaternion	&operator=(const IFXVector4 &operand);

						// not inline
						/** Populates using the computed rotation of a matrix.*/
		IFXQuaternion	&operator=(const IFXMatrix4x4 &operand);

						/** Exact compare operation (zero tolerance).*/
		BOOL			operator==(const IFXQuaternion &operand) const;

						/** Populates using an explicit <w,x,y,z>.*/
		void			Set(F32 w,F32 x,F32 y,F32 z);

						/** Sets to identity, <1,0,0,0>.*/
		void			MakeIdentity(void);

						/** Sets to a rotation of a specified angle in radians
							about a given enumerated axis: IFX_X_AXIS,
							IFX_Y_AXIS, or IFX_Z_AXIS. */
		void			MakeRotation(F32 radians,IFXAxisID axis);

						/** Sets to a rotation of a specified angle in radians
							about a given arbitrary axis. */
		void			MakeRotation(F32 radians,const IFXVector3 &axis);

						/** Sets as the rotation required to move the first
							Vector3 to the second.  The vectors must be
							pre-normalized for valid results. */
		void			MakeRotation(const IFXVector3 &from,
												const IFXVector3 &to);

						/** Compares to another quaternion using a margin
							of tolerance. */
		BOOL			IsApproximately(const IFXQuaternion &operand,
													F32 margin) const;

						/** Returns quaternion of the value <1,0,0,0>.*/
static
const	IFXQuaternion	&Identity(void);

						/** Returns a pointer to the internal float array
                            containing the 4 floats.  This is not a copy. */
		F32				*Raw(void)					{ return m_data; }

						/** Returns a const pointer to the internal float array.*/
const	F32				*RawConst(void) const		{ return m_data; }

						/** Accesses an element of the internal float array.*/
		F32				&operator[](const int x)	{ return m_data[x]; }

						/** Computes an angle of rotation about an axis
							that is equivalent to this quaternion.
							Both arguments are outputs. */
		void			ComputeAngleAxis(F32 &radians,IFXVector3 &axis) const;

						/** Inverts the quaternion in place.*/
		IFXQuaternion	&Invert(void);

						/** Sets the quaternion to the inverse of the argument.*/
		IFXQuaternion	&Invert(const IFXQuaternion &operand);

						/** Calculates the square of the length of the
							quaternion.  Take the sqrt() of this value to
							get the length. */
		F32				CalcMagnitudeSquared(void);

						/** Returns TRUE if the magnitude is very close to one,
							by some internally defined margin. */
		BOOL			IsValid(void);

						/** Normalizes the quaternion.  Most operations will
							maintain unit length of the quaternion, but
							repeated operations can result in drift. */
		void			Normalize(void);

						/** Multiplies two quaternion arguments and puts the
							result in this quaternion. */
		IFXQuaternion	&Multiply(const IFXQuaternion &left,
												const IFXQuaternion &right);

						/** Multiplies this quaternion by another quaternion
							in place. */
		void			Multiply(const IFXQuaternion &operand)
													{ Rotate(operand); };

						/** Rotates this quaternion by another quaternion
							in place. This is equivalent to a Multiply. */
		void			Rotate(const IFXQuaternion &operand);

						/** Rotates by a specified angle in radians
							about a given enumerated axis: IFX_X_AXIS,
							IFX_Y_AXIS, or IFX_Z_AXIS. */
		void			Rotate(F32 radians,IFXAxisID axis);

						/** Rotate a Vector3 using this quaternion.*/
		void			RotateVector(const IFXVector3 &input,
														IFXVector3 &output);

						/** Rotates an implied using this quaternion.
							You must supply two arrays with room for 3 floats.
							The first must contain valid input valids.
							The second will be overwritten with the output. */
		void			RotateVector(const F32 *input,F32 *output);

						/** Scales the implied angle of rotation in the
							quaternion. */
		void			ScaleAngle(F32 scale);

						/** Every quaternion has a single equivalent quaternion
							with the signs of all four elements reversed.
							This forces the quaternion to choose the one
							with the positive w element. */
		void			ForcePositiveW(void);

	// not inline:
						/** Populates this quaternion with the interpolated
							value between the given quaternion arguments
							for a t between zero and one, where t=0 results
							in an exact copy of the 'from' argument. */
		void			Interpolate(F32 t,const IFXQuaternion &from,
											const IFXQuaternion &to);

						/** Computes and remove the z component of the
							equivalent IFXEuler form. */
		F32				ExtractEulerZ(void);

	private:
		F32				m_data[4];
};

IFXINLINE void IFXQuaternion::Set(F32 w,F32 x,F32 y,F32 z)
{
	m_data[0]=w;
	m_data[1]=x;
	m_data[2]=y;
	m_data[3]=z;
}

IFXINLINE void IFXQuaternion::MakeIdentity(void)
{
	m_data[0]=1.0f;
	m_data[1]=0.0f;
	m_data[2]=0.0f;
	m_data[3]=0.0f;
}

// static
IFXINLINE const IFXQuaternion &IFXQuaternion::Identity(void)
{
	static const IFXQuaternion identity(1.0f,0.0f,0.0f,0.0f);
	return identity;
}

IFXINLINE BOOL IFXQuaternion::IsApproximately(const IFXQuaternion &operand,
													F32 margin) const
{
	if( fabs(m_data[0]-operand.m_data[0])<margin &&
		fabs(m_data[1]-operand.m_data[1])<margin &&
		fabs(m_data[2]-operand.m_data[2])<margin &&
		fabs(m_data[3]-operand.m_data[3])<margin )
			return true;

	return false;
}


IFXINLINE IFXQuaternion &IFXQuaternion::operator=(const IFXQuaternion &operand)
{
	m_data[0]=operand.m_data[0];
	m_data[1]=operand.m_data[1];
	m_data[2]=operand.m_data[2];
	m_data[3]=operand.m_data[3];

	return *this;
}

IFXINLINE IFXQuaternion &IFXQuaternion::operator=(const F32 *F32s)
{
	m_data[0]=F32s[0];
	m_data[1]=F32s[1];
	m_data[2]=F32s[2];
	m_data[3]=F32s[3];

	return *this;
}

IFXINLINE IFXQuaternion &IFXQuaternion::operator=(const IFXVector3 &operand)
{
	const F32 *v=operand.RawConst();

	m_data[0]=0.0f;
	m_data[1]=v[0];
	m_data[2]=v[1];
	m_data[3]=v[2];

	return *this;
}

IFXINLINE IFXQuaternion &IFXQuaternion::operator=(const IFXVector4 &operand)
{
	const F32 *v=operand.RawConst();

	m_data[0]=v[0];
	m_data[1]=v[1];
	m_data[2]=v[2];
	m_data[3]=v[3];

	return *this;
}

IFXINLINE BOOL IFXQuaternion::operator==(const IFXQuaternion &operand) const
{
	return (	(m_data[0]-operand.m_data[0]<IFXDELTA)	&&
				(m_data[1]-operand.m_data[1]<IFXDELTA)	&&
				(m_data[2]-operand.m_data[2]<IFXDELTA)	&&
				(m_data[3]-operand.m_data[3]<IFXDELTA)	);
}

IFXINLINE void IFXQuaternion::Rotate(const IFXQuaternion &operand)
{
	IFXQuaternion temp=*this;		// copy

	Multiply(temp,operand);
}

IFXINLINE void IFXQuaternion::Rotate(F32 radians,IFXAxisID axis)
{
	IFXQuaternion rot;
	rot.MakeRotation(radians,axis);

	Rotate(rot);
}

IFXINLINE void IFXQuaternion::ScaleAngle(F32 scale)
{
	F32 angle;
	IFXVector3 axis;

	ComputeAngleAxis(angle,axis);
	MakeRotation(angle*scale,axis);
}

IFXINLINE IFXQuaternion &IFXQuaternion::Invert(const IFXQuaternion &operand)
{
	IFXASSERT(this!=&operand);
	const F32 *q=operand.RawConst();

#if FALSE
	F32 norm,invNorm;

	norm=q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
	invNorm= 1.0f/norm;

	m_data[0]=  q[0]*invNorm;
	m_data[1]= -q[1]*invNorm;
	m_data[2]= -q[2]*invNorm;
	m_data[3]= -q[3]*invNorm;
#else
	m_data[0]=  q[0];
	m_data[1]= -q[1];
	m_data[2]= -q[2];
	m_data[3]= -q[3];
#endif
	return *this;
}

IFXINLINE IFXQuaternion &IFXQuaternion::Invert(void)
{
	m_data[1]= -m_data[1];
	m_data[2]= -m_data[2];
	m_data[3]= -m_data[3];
	return *this;
}

IFXINLINE F32 IFXQuaternion::CalcMagnitudeSquared(void)
{
	return m_data[0]*m_data[0]+m_data[1]*m_data[1]+
			m_data[2]*m_data[2]+m_data[3]*m_data[3];
}

IFXINLINE BOOL IFXQuaternion::IsValid(void)
{
	F32 mag2=CalcMagnitudeSquared();
	return (mag2>0.99f && mag2<1.01f);
}

IFXINLINE void IFXQuaternion::Normalize(void)
{
	F32 invDist,square=CalcMagnitudeSquared();
	if(square==0.0f)
	{
		MakeIdentity();
		return;
	}

	invDist= 1.0f/IFXSQRT(square);

	m_data[0]*=invDist;
	m_data[1]*=invDist;
	m_data[2]*=invDist;
	m_data[3]*=invDist;
}

// vector-specific version assuming w=0 (in and out) for speed
// this: 24mult,17add  quat_mult*2: 32mult,24add	matrix3x3_mult:9mult,6add
IFXINLINE void IFXQuaternion::RotateVector(
								const IFXVector3 &input,IFXVector3 &output)
{
	RotateVector(input.RawConst(),output.Raw());
}
IFXINLINE void IFXQuaternion::RotateVector(const F32 *in,F32 *out)
{
	F32 mid[4];

	mid[0]= -m_data[1]*in[0]-m_data[2]*in[1]-m_data[3]*in[2];
	mid[1]=  m_data[0]*in[0]+m_data[2]*in[2]-m_data[3]*in[1];
	mid[2]=  m_data[0]*in[1]+m_data[3]*in[0]-m_data[1]*in[2];
	mid[3]=  m_data[0]*in[2]+m_data[1]*in[1]-m_data[2]*in[0];

#if FALSE
	IFXQuaternion inverse;
	inverse.Invert(*this);

	const F32 *inv=inverse.RawConst();

	out[0]=mid[0]*inv[1]+mid[1]*inv[0]+mid[2]*inv[3]-mid[3]*inv[2];
	out[1]=mid[0]*inv[2]+mid[2]*inv[0]+mid[3]*inv[1]-mid[1]*inv[3];
	out[2]=mid[0]*inv[3]+mid[3]*inv[0]+mid[1]*inv[2]-mid[2]*inv[1];
#else
	out[0]=-mid[0]*m_data[1]+mid[1]*m_data[0]-mid[2]*m_data[3]+mid[3]*m_data[2];
	out[1]=-mid[0]*m_data[2]+mid[2]*m_data[0]-mid[3]*m_data[1]+mid[1]*m_data[3];
	out[2]=-mid[0]*m_data[3]+mid[3]*m_data[0]-mid[1]*m_data[2]+mid[2]*m_data[1];
#endif
}

IFXINLINE IFXQuaternion &IFXQuaternion::Multiply(const IFXQuaternion &left,
												const IFXQuaternion &right)
{
	IFXASSERT(this!=&left);
	IFXASSERT(this!=&right);
	const F32 *a=left.RawConst();
	const F32 *b=right.RawConst();

#if TRUE
	// this: 16mult,12add  matrix3x3:27mult,18add
	m_data[1]=a[0]*b[1]+a[1]*b[0]+a[2]*b[3]-a[3]*b[2];
	m_data[2]=a[0]*b[2]+a[2]*b[0]+a[3]*b[1]-a[1]*b[3];
	m_data[3]=a[0]*b[3]+a[3]*b[0]+a[1]*b[2]-a[2]*b[1];
	m_data[0]=a[0]*b[0]-a[1]*b[1]-a[2]*b[2]-a[3]*b[3];
#else
	// WARNING untested
	// this: 12mult,35add  matrix3x3:27mult,18add
	F32 A=(a[0]+a[1])*(b[0]+b[1]);
	F32 B=(a[3]-a[2])*(b[2]-b[3]);
	F32 C=(a[1]-a[0])*(b[2]-b[3]);
	F32 D=(a[2]+a[3])*(b[1]-b[0]);
	F32 E=(a[1]+a[3])*(b[1]+b[2]);
	F32 F=(a[1]-a[3])*(b[1]-b[2]);
	F32 G=(a[0]+a[2])*(b[0]-b[3]);
	F32 H=(a[0]-a[2])*(b[0]+b[3]);

	m_data[0]=B+(-E-F+G+H)*0.5f;
	m_data[1]=A-(E+F+G+H)*0.5f;
	m_data[2]= -C+(E-F+G-H)*0.5f;
	m_data[3]= -D+(E-F-G+H)*0.5f;
#endif


	//* NOTE we use this mult for vector xforms, so DON'T NORMALIZE
//	Normalize();

	return *this;
}


#endif


