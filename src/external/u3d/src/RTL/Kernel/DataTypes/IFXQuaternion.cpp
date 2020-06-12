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
	@file	IFXQuaternion.cpp

			This module contains the IFXQuaternion class used for
			representing and performing 3D rotations.

	@note	Several functions in this module are modified versions of Nick 
			Bobic's (nick@wiplay.com) GLquat.c source code
			(http://www.gdmag.com/src/feb98.zip) that's associated with his
			"Rotating Objects Using Quaternions" Game Developer article
			(http://www.gamasutra.com/features/19980703/quaternions_01.htm)
			from February 1998.  For additional details about his source 
			code and the license associated with it refer to the 
			IFXQuaternion class description in IFXQuaternion.h.
*/

#include "IFXQuaternion.h"

void IFXQuaternion::ForcePositiveW(void)
{
	// negative W is the alternate twin, so X,Y,Z may need to be inverted
	if(m_data[0]<0)
	{
		m_data[0]= -m_data[0];
		m_data[1]= -m_data[1];
		m_data[2]= -m_data[2];
		m_data[3]= -m_data[3];
	}
}

void IFXQuaternion::MakeRotation(F32 radians,IFXAxisID axis)
{
	//  IFXASSERT(radians>= -(IFXPI+0.0001f));
	//  IFXASSERT(radians<=  (IFXPI+0.0001f));

	F32 r2=radians*0.5f;

	m_data[0]=IFXCOS(r2);
	m_data[1]=0.0f;
	m_data[2]=0.0f;
	m_data[3]=0.0f;
	m_data[1+axis]=IFXSIN(r2);
}


void IFXQuaternion::MakeRotation(F32 radians,const IFXVector3 &axis)
{
	//  IFXASSERT(radians>= -(IFXPI+0.0001f));
	//  IFXASSERT(radians<=  (IFXPI+0.0001f));

	const F32 *v=axis.RawConst();
	if(fabs(v[0])+fabs(v[1])+fabs(v[2])<IFXDELTA)
	{
		MakeIdentity();
		return;
	}

	F32 r2=radians*0.5f;
	IFXVector3 temp=axis;

	temp.Normalize();
	temp.Scale(IFXSIN(r2));

	operator=(temp);
	m_data[0]=IFXCOS(r2);
}


/**
	Create quaternion to represent a rotation between two vectors.

	@note	This function contains a modified version of Nick Bobic's
			(nick@wiplay.com) GLquat.c source code
			(http://www.gdmag.com/src/feb98.zip) that's associated with his
			"Rotating Objects Using Quaternions" Game Developer article
			(http://www.gamasutra.com/features/19980703/quaternions_01.htm)
			from February 1998.  Specifically, the gluQuatSetFromAx_EXT
			function.  For additional details about his source code and 
			the license associated with it refer to the class description 
			in IFXQuaternion.h.
*/
void IFXQuaternion::MakeRotation(const IFXVector3 &from,
								 const IFXVector3 &to)
{
	//* NOTE: assumes normalized input

	const F32 *v1=from.RawConst();
	const F32 *v2=to.RawConst();

	F32 tx,ty,tz,temp,dist;
	F32 cost,len,ss;

	// get dot product of two vectors
	cost=v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];

	// check if parallel
	if (cost > IFXALMOST1)
	{
		MakeIdentity();
		return;
	}
	else if (cost < -IFXALMOST1)
	{
		// check if opposite

		// check if we can use cross product of from vector with [1, 0, 0]
		tx=0.0f;
		ty=v1[0];
		tz= -v1[1];

		len=IFXSQRT(ty*ty + tz*tz);

		if (len < IFXSLERP_DELTA)
		{
			// no, we need cross product of from vector with [0, 1, 0]
			tx= -v1[2];
			ty=0.0f;
			tz=v1[0];
		}

		// normalize
		temp=tx*tx + ty*ty + tz*tz;

		dist=(F32)(1.0f/IFXSQRT(temp));

		tx*=dist;
		ty*=dist;
		tz*=dist;

		Set(0.0f,tx,ty,tz);

		return;
	}

	// else just cross two vectors

	tx=v1[1]*v2[2] - v1[2]*v2[1];
	ty=v1[2]*v2[0] - v1[0]*v2[2];
	tz=v1[0]*v2[1] - v1[1]*v2[0];

	temp=tx*tx + ty*ty + tz*tz;
	dist=1.0f/IFXSQRT(temp);

	tx*=dist;
	ty*=dist;
	tz*=dist;

	// use half-angle formula (sin^2 t=( 1 - cos(2t) )/2)

	ss=IFXSQRT(0.5f * (1.0f - cost));

	tx*=ss;
	ty*=ss;
	tz*=ss;

	// cos^2 t=( 1 + cos (2t) ) / 2
	// w part is cosine of half the rotation angle

	Set(IFXSQRT(0.5f * (1.0f + cost)),tx,ty,tz);
}


void IFXQuaternion::ComputeAngleAxis(F32 &radians,IFXVector3 &axis) const
{
	//  F32 len=m_data[1]*m_data[1] + m_data[2]*m_data[2] + m_data[3]*m_data[3];

	F32 len=m_data[1];
	len*=m_data[1];
	F32 len2=m_data[2];
	len2*=m_data[2];
	len+=len2;
	F32 len3=m_data[3];
	len3*=m_data[3];
	len+=len3;

	//  if(len<IFXSLERP_DELTA || m_data[0]>(1.0f-IFXSLERP_DELTA))
	if(len==0.0f)
	{
		axis.Set(0.0f,0.0f,1.0f);
		radians=0.0f;
		return;
	}

	// NOTE example code didn't sqrt (why?)

	F32 inv=1.0f/IFXSQRT(len);
	if(m_data[0]<0.0f)
		inv= -inv;

	axis[0]=m_data[1]*inv;
	axis[1]=m_data[2]*inv;
	axis[2]=m_data[3]*inv;

	F32 abs_w = (F32)fabs(m_data[0]);
	radians=2.0f*IFXACOS( abs_w > 1.0f ? 1.0f : abs_w );
}


/**
	Converts a matrix to a quaternion.

	@note	This function contains a modified version of Nick Bobic's
			(nick@wiplay.com) GLquat.c source code
			(http://www.gdmag.com/src/feb98.zip) that's associated with his
			"Rotating Objects Using Quaternions" Game Developer article
			(http://www.gamasutra.com/features/19980703/quaternions_01.htm)
			from February 1998.  Specifically, the gluMatToQuat_EXT function.
			For additional details about his source code and the license 
			associated with it refer to the class description in 
			IFXQuaternion.h.

	@todo	Is this still valid if the matrix is scaled?
*/
IFXQuaternion &IFXQuaternion::operator=(const IFXMatrix4x4 &matrix)
{
	const int nxt[3]={1,2,0};
	const F32 *m=matrix.RawConst();

	F32 tr,s;

	//    0,0  1,1  2,2
	tr=m[0]+m[5]+m[10];

	if(tr>0.0f)
	{
		// diagonal is positive
		s=IFXSQRT(tr+1.0f);
		m_data[0]=s/2.0f;     // w

		s=0.5f/s;
		m_data[1]=(m[6]-m[9])*s;  // x = (m[1][2]-m[2][1])*s;
		m_data[2]=(m[8]-m[2])*s;  // y = (m[2][0]-m[0][2])*s;
		m_data[3]=(m[1]-m[4])*s;  // z = (m[0][1]-m[1][0])*s;
	}
	else
	{
		// diagonal is negative
		int i=0;
		if(m[5]>m[0])     // m[1][1]>m[0][0]
			i=1;

		if(m[10]>m[i*5])    // m[2][2]>m[i][i]
			i=2;

		int j=nxt[i];
		int k=nxt[j];

		//    m[i][i] m[j][j] m[k][k]
		s=IFXSQRT((m[i*5]-(m[j*5]+m[k*5]))+1.0f);

		F32 q[4];

		q[i]=s*0.5f;

		if(s!=0.0f)
			s=0.5f/s;

		q[3]=(m[j*4+k]-m[k*4+j])*s;
		q[j]=(m[i*4+j]+m[j*4+i])*s;
		q[k]=(m[i*4+k]+m[k*4+i])*s;

		m_data[0]=q[3];
		m_data[1]=q[0];
		m_data[2]=q[1];
		m_data[3]=q[2];
	}

	Normalize();
	return *this;
}


/**
	Interpolates from one quaternion to another.

	@note	This function contains a modified version of Nick Bobic's
			(nick@wiplay.com) GLquat.c source code
			(http://www.gdmag.com/src/feb98.zip) that's associated with his
			"Rotating Objects Using Quaternions" Game Developer article
			(http://www.gamasutra.com/features/19980703/quaternions_01.htm)
			from February 1998.  Specifically, the gluQuatSlerp_EXT function.
			For additional details about his source code and the license 
			associated with it refer to the class description in 
			IFXQuaternion.h.
*/
void IFXQuaternion::Interpolate(F32 t,const IFXQuaternion &from,
								const IFXQuaternion &to)
{
	const F32 *q1=from.RawConst();
	const F32 *q2=to.RawConst();

	F32 to1[4];
	F32 omega, cosom, sinom;  // NOTE: example used doubles
	F32 scale0, scale1;     // NOTE: example used doubles

	// calc cosine
	cosom = q1[0]*q2[0]+
		q1[1]*q2[1]+
		q1[2]*q2[2]+
		q1[3]*q2[3];

	// adjust signs (if necessary)
	if(cosom < 0.0f)
	{
		cosom= -cosom;

		to1[0]= -q2[1];
		to1[1]= -q2[2];
		to1[2]= -q2[3];
		to1[3]= -q2[0];
	}
	else
	{
		to1[0]=q2[1];
		to1[1]=q2[2];
		to1[2]=q2[3];
		to1[3]=q2[0];
	}

	// calculate coefficients

#if IFXUSE_SLERP
	if ( (1.0f - cosom) > IFXSLERP_DELTA )
	{
		// standard case (slerp)
		omega = IFXACOS(cosom);
		sinom = IFXSIN(omega);
		scale0 = IFXSIN((1.0f - t) * omega) / sinom;
		scale1 = IFXSIN(t * omega) / sinom;
	}
	else
#endif
	{
		// "from" and "to" quaternions are very close
		//  ... so we can do a linear interpolation

		scale0 = 1.0f - t;
		scale1 = t;
	}

	// calculate final values
	m_data[1] = scale0 * q1[1] + scale1 * to1[0];
	m_data[2] = scale0 * q1[2] + scale1 * to1[1];
	m_data[3] = scale0 * q1[3] + scale1 * to1[2];
	m_data[0] = scale0 * q1[0] + scale1 * to1[3];
}


F32 IFXQuaternion::ExtractEulerZ(void)
{
	IFXVector3 xaxis(1.0f,0.0f,0.0f);

	ForcePositiveW();

	IFXQuaternion qinv,result;
	IFXVector3 rotated;
	RotateVector(xaxis,rotated);

	IFXASSERT(rotated[0]!=0.0f);
	F32 zangle=IFXATAN2(rotated[1],rotated[0]);

	qinv.MakeRotation(-zangle,IFX_Z_AXIS);
	result.Multiply(qinv,(*this));

	(*this)=result;
	return zangle;
}
