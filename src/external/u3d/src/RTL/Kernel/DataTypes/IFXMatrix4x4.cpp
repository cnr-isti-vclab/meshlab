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

#include "IFXIPP.h"
#include "IFXMatrix4x4.h"
#include "IFXQuaternion.h"

const F32 ifxEpsilon=1e-06f;

/*
#define IFXM4_TRS_FAST  TRUE
IFXRESULT IFXMatrix4x4::CalcTRS(IFXVector3 *pTranslation,
IFXQuaternion *pRotation,
IFXVector3 *pScale) const
{
IFXRESULT iResult=IFX_OK;
IFXQuaternion rotation=*this;

//  M3 = R S       S = R^ M3

IFXMatrix4x4 inverse,mscale,matrix3x3=*this;
matrix3x3.ResetTranslation();

if(!IFXSUCCESS(iResult=inverse.Invert3x4(rotation)))
return iResult;

#if IFXM4_TRS_FAST
const F32 *m2=inverse.RawConst();
const F32 *m1=matrix3x3.RawConst();
mscale[ 0]=m1[ 0]*m2[ 0]+m1[ 1]*m2[ 4]+m1[ 2]*m2[ 8];
mscale[ 5]=m1[ 4]*m2[ 1]+m1[ 5]*m2[ 5]+m1[ 6]*m2[ 9];
mscale[10]=m1[ 8]*m2[ 2]+m1[ 9]*m2[ 6]+m1[10]*m2[10];

if(pTranslation)
CopyTranslation(*pTranslation);
#else
mscale.Multiply(inverse,matrix3x3);

//  M4 = T R S     T = M4 M3^

if(!IFXSUCCESS(iResult=inverse.Invert3x4(matrix3x3)))
return iResult;

IFXMatrix4x4 mtranslation;
mtranslation.Multiply(*this,inverse);

if(pTranslation)
mtranslation.CopyTranslation(*pTranslation);
#endif

if(pRotation)
*pRotation=rotation;
if(pScale)
pScale->Set(mscale[0],mscale[5],mscale[10]);

return IFX_OK;
}
*/


IFXRESULT IFXMatrix4x4::CalcTRS(IFXVector3 *pTranslation,
								IFXQuaternion *pRotation,
								IFXVector3 *pScale) const
{
	IFXVector3 translation,scale;
	IFXMatrix4x4 rotation;

	translation.Reset();
	rotation.MakeIdentity();
	scale.Set(1.0f,1.0f,1.0f);

	IFXRESULT iResult=Decompose(translation,rotation,scale);

	if(pTranslation)
		*pTranslation=translation;
	if(pRotation)
		*pRotation=rotation;
	if(pScale)
		*pScale=scale;

	return iResult;
}


//* stole this better version from IFXMatrix4x4::decompose()
IFXRESULT IFXMatrix4x4::Decompose(IFXVector3 &translation,
								  IFXMatrix4x4 &rotation,
								  IFXVector3 &scale) const
{
	const F32 det=CalcDeterminant();

	if(det==0.0f)
		return IFX_E_INVALID_RANGE;

	IFXVector3 rowx=&m_data[0];
	IFXVector3 rowy=&m_data[4];
	IFXVector3 rowz=&m_data[8];
	IFXVector3 mid1,mid2;
	translation=&m_data[12];

	// do gram-schmidt orthogonalization and while normalizing each vector,
	// extract the scale.

	IFXVector3 rotx=rowx;
	scale[0]=rotx.CalcMagnitude();
	rotx.Scale(1.0f/scale[0]);

	IFXVector3 roty=rowy;
	mid1=rotx;
	roty.Subtract(mid1.Scale(rowy.DotProduct(rotx)));
	scale[1]=roty.CalcMagnitude();
	roty.Scale(1.0f/scale[1]);

	IFXVector3 rotz=rowz;
	mid1=rotx;
	mid2=roty;
	rotz.Subtract(mid1.Scale(rowz.DotProduct(rotx)));
	rotz.Subtract(mid2.Scale(rowz.DotProduct(roty)));
	scale[2]=rotz.CalcMagnitude();
	rotz.Scale(1.0f/scale[2]);

	rotation.Set(rotx,roty,rotz,IFXVector3(0.0f,0.0f,0.0f));
	rotation[3] = 0;
	rotation[7] = 0;
	rotation[11] = 0;
	rotation[15] = 1;

	F32 rotDet = rotation.CalcDeterminant();

	if( fabsf(-1 - rotDet) < ifxEpsilon )
	{
		scale[2]= -scale[2];
		//    rotz.Negate();
		rotation[8]= -rotation[8];
		rotation[9]= -rotation[9];
		rotation[10]= -rotation[10];
	}

	return IFX_OK;
}



#if FALSE
/******************************************************************************
M=TSR -> ignoring T, S = M R^-1, where R is unscaled version of M

Notes:
determinant could determine how many axis scales were negative:
det<0: 0 or 2
det>0: 1 or 3
negative scales on two axes is the same as a rotation
******************************************************************************/
void IFXMatrix4x4::ExtractScale(IFXVector3 &scale,IFXQuaternion &rotation) const
{
	rotation= *this;

	IFXMatrix4x4 inverse;
	inverse.Reset();

	rotation.Invert();
	inverse=rotation;
	rotation.Invert();

	const F32 *m2=m_data;     // left
	const F32 *m1=inverse.m_data; // right

	F32 *result=scale.Raw();

	result[0]=m1[ 0]*m2[ 0]+m1[ 1]*m2[ 4]+m1[ 2]*m2[ 8];
	result[1]=m1[ 4]*m2[ 1]+m1[ 5]*m2[ 5]+m1[ 6]*m2[ 9];
	result[2]=m1[ 8]*m2[ 2]+m1[ 9]*m2[ 6]+m1[10]*m2[10];
}
#endif



/******************************************************************************
scale found by transforming unit axis vectors

******************************************************************************/
void IFXMatrix4x4::CalcAxisScale(IFXVector3 &scale) const
{
	const IFXVector3 unitx(1.0f,0.0f,0.0f);
	const IFXVector3 unity(0.0f,1.0f,0.0f);
	const IFXVector3 unitz(0.0f,0.0f,1.0f);
	IFXVector3 rotx,roty,rotz;
	F32 *result=scale.Raw();

	RotateVector(unitx,rotx);
	RotateVector(unity,roty);
	RotateVector(unitz,rotz);

	result[0]=rotx.CalcMagnitude();
	result[1]=roty.CalcMagnitude();
	result[2]=rotz.CalcMagnitude();
}


/******************************************************************************
IFXMatrix4x4 &IFXMatrix4x4::operator=(const IFXQuaternion &quat)

convert Quaternion to matrix

Adapted from: Game Developer, Feb 1998, p. 38

******************************************************************************/
IFXMatrix4x4 &IFXMatrix4x4::operator=(const IFXQuaternion &quat)
{
	const F32 *q=quat.RawConst();

	F32 x2=q[1]+q[1];
	F32 y2=q[2]+q[2];
	F32 z2=q[3]+q[3];
	F32 xx=q[1]*x2;
	F32 xy=q[1]*y2;
	F32 xz=q[1]*z2;
	F32 yy=q[2]*y2;
	F32 yz=q[2]*z2;
	F32 zz=q[3]*z2;
	F32 wx=q[0]*x2;
	F32 wy=q[0]*y2;
	F32 wz=q[0]*z2;

	// this is the transpose of what the reference said
	m_data[0]=1.0f-(yy+zz);
	m_data[4]=xy-wz;
	m_data[8]=xz+wy;

	m_data[1]=xy+wz;
	m_data[5]=1.0f-(xx+zz);
	m_data[9]=yz-wx;

	m_data[2]=xz-wy;
	m_data[6]=yz+wx;
	m_data[10]=1.0f-(xx+yy);

	/*  original
	m_data[0]=1.0f-(yy+zz);
	m_data[1]=xy-wz;
	m_data[2]=xz+wy;

	m_data[4]=xy+wz;
	m_data[5]=1.0f-(xx+zz);
	m_data[6]=yz-wx;

	m_data[8]=xz-wy;
	m_data[9]=yz+wx;
	m_data[10]=1.0f-(xx+yy);
	*/

	/*
	// translation, etc
	m_data[3]=0.0f;
	m_data[7]=0.0f;
	m_data[11]=0.0f;
	m_data[12]=0.0f;
	m_data[13]=0.0f;
	m_data[14]=0.0f;
	m_data[15]=1.0f;
	*/

	return *this;
}

void IFXMatrix4x4::RotateAxis(const IFXVector4& axis, const F32 angle)
{
	F32 cs = cosf(-angle);
	F32 sn = sinf(-angle);

	F32 len = sqrtf( axis.X()*axis.X() + axis.Y()*axis.Y() + axis.Z()*axis.Z() );
	F32 x = axis.X() / len;
	F32 y = axis.Y() / len;
	F32 z = axis.Z() / len;

	F32 omc = 1-cs;
	F32 x2 = x*x, y2 = y*y, z2 = z*z;
	F32 xy = x*y, xz = x*z, yz = y*z;
	F32 snx = sn*x, sny = sn*y, snz = sn*z;

	m_data[0]  = (F32)(1-omc*(y2+z2));
	m_data[1]  = (F32)(-snz+omc*xy);
	m_data[2]  = (F32)(sny+omc*xz);
	m_data[3]  = 0.0f;
	m_data[4]  = (F32)(snz+omc*xy);
	m_data[5]  = (F32)(1-omc*(x2+z2));
	m_data[6]  = (F32)(-snx+omc*yz);
	m_data[7]  = 0.0f;
	m_data[8]  = (F32)(-sny+omc*xz);
	m_data[9]  = (F32)(snx+omc*yz);
	m_data[10] = (F32)(1-omc*(x2+y2));
	m_data[11]  = 0.0f;
	m_data[12]  = 0.0f;
	m_data[13]  = 0.0f;
	m_data[14]  = 0.0f;
	m_data[15]  = 1.0f;
}

// Multiply

void IFXMatrix4x4_Multiply(F32* m_data, const F32* m1, const F32* m2)
{
	m_data[ 0]=m1[ 0]*m2[ 0]+m1[ 1]*m2[ 4]+m1[ 2]*m2[ 8]+m1[ 3]*m2[12];
	m_data[ 1]=m1[ 0]*m2[ 1]+m1[ 1]*m2[ 5]+m1[ 2]*m2[ 9]+m1[ 3]*m2[13];
	m_data[ 2]=m1[ 0]*m2[ 2]+m1[ 1]*m2[ 6]+m1[ 2]*m2[10]+m1[ 3]*m2[14];
	m_data[ 3]=m1[ 0]*m2[ 3]+m1[ 1]*m2[ 7]+m1[ 2]*m2[11]+m1[ 3]*m2[15];
	m_data[ 4]=m1[ 4]*m2[ 0]+m1[ 5]*m2[ 4]+m1[ 6]*m2[ 8]+m1[ 7]*m2[12];
	m_data[ 5]=m1[ 4]*m2[ 1]+m1[ 5]*m2[ 5]+m1[ 6]*m2[ 9]+m1[ 7]*m2[13];
	m_data[ 6]=m1[ 4]*m2[ 2]+m1[ 5]*m2[ 6]+m1[ 6]*m2[10]+m1[ 7]*m2[14];
	m_data[ 7]=m1[ 4]*m2[ 3]+m1[ 5]*m2[ 7]+m1[ 6]*m2[11]+m1[ 7]*m2[15];
	m_data[ 8]=m1[ 8]*m2[ 0]+m1[ 9]*m2[ 4]+m1[10]*m2[ 8]+m1[11]*m2[12];
	m_data[ 9]=m1[ 8]*m2[ 1]+m1[ 9]*m2[ 5]+m1[10]*m2[ 9]+m1[11]*m2[13];
	m_data[10]=m1[ 8]*m2[ 2]+m1[ 9]*m2[ 6]+m1[10]*m2[10]+m1[11]*m2[14];
	m_data[11]=m1[ 8]*m2[ 3]+m1[ 9]*m2[ 7]+m1[10]*m2[11]+m1[11]*m2[15];
	m_data[12]=m1[12]*m2[ 0]+m1[13]*m2[ 4]+m1[14]*m2[ 8]+m1[15]*m2[12];
	m_data[13]=m1[12]*m2[ 1]+m1[13]*m2[ 5]+m1[14]*m2[ 9]+m1[15]*m2[13];
	m_data[14]=m1[12]*m2[ 2]+m1[13]*m2[ 6]+m1[14]*m2[10]+m1[15]*m2[14];
	m_data[15]=m1[12]*m2[ 3]+m1[13]*m2[ 7]+m1[14]*m2[11]+m1[15]*m2[15];
}

pIFXMatrix4x4_Multiply _IFXMatrix4x4_Multiply = IFXMatrix4x4_Multiply;

IFXMatrix4x4 &IFXMatrix4x4::Multiply(const IFXMatrix4x4 &left, const IFXMatrix4x4 &right)
{
	IFXASSERT(this != &left);
	IFXASSERT(this != &right);
	_IFXMatrix4x4_Multiply(m_data, right.m_data, left.m_data);
	return *this;
}

// MakeIdentity

void IFXMatrix4x4_MakeIdentity(F32* m_data)
{
	m_data[0]=1.0;
	m_data[1]=0.0;
	m_data[2]=0.0;
	m_data[3]=0.0;

	m_data[4]=0.0;
	m_data[5]=1.0;
	m_data[6]=0.0;
	m_data[7]=0.0;

	m_data[8]=0.0;
	m_data[9]=0.0;
	m_data[10]=1.0;
	m_data[11]=0.0;

	m_data[12]=0.0;
	m_data[13]=0.0;
	m_data[14]=0.0;
	m_data[15]=1.0;
}

pIFXMatrix4x4_MakeIdentity _IFXMatrix4x4_MakeIdentity = IFXMatrix4x4_MakeIdentity;

IFXMatrix4x4 &IFXMatrix4x4::MakeIdentity(void)
{
	_IFXMatrix4x4_MakeIdentity(m_data);
	return *this;
}

// Scale

void IFXMatrix4x4_Scale(F32* m_data, const F32* v)
{
	m_data[0]*=v[0];
	m_data[1]*=v[0];
	m_data[2]*=v[0];
	m_data[3]*=v[0];

	m_data[4]*=v[1];
	m_data[5]*=v[1];
	m_data[6]*=v[1];
	m_data[7]*=v[1];

	m_data[8]*=v[2];
	m_data[9]*=v[2];
	m_data[10]*=v[2];
	m_data[11]*=v[2];
}

pIFXMatrix4x4_Scale _IFXMatrix4x4_Scale = IFXMatrix4x4_Scale;

IFXMatrix4x4 &IFXMatrix4x4::Scale(const IFXVector3 &vector)
{
	IFXMatrix4x4_Scale(m_data, vector.RawConst());
	return *this;
}

// Transpose

void IFXMatrix4x4_Transpose(F32* m_data)
{
	F32 fTmp;
#define IFX_MSWAP(a, b) fTmp = (a); (a) = (b); (b) = fTmp;

	IFX_MSWAP(m_data[1], m_data[4]);
	IFX_MSWAP(m_data[2], m_data[8]);
	IFX_MSWAP(m_data[3], m_data[12]);
	IFX_MSWAP(m_data[6], m_data[9]);
	IFX_MSWAP(m_data[7], m_data[13]);
	IFX_MSWAP(m_data[11], m_data[14]);

#undef IFX_MSWAP
}

pIFXMatrix4x4_Transpose _IFXMatrix4x4_Transpose = IFXMatrix4x4_Transpose;

void IFXMatrix4x4::Transpose()
{
	_IFXMatrix4x4_Transpose(m_data);
}

// CalcDeterminant3x3

F32 IFXMatrix4x4_CalcDeterminant3x3(const F32* src)
{
	F32 det;
	F32 d1, d2, d3;
	d1 = src[5]*src[10] - src[6]*src[9];
	d2 = src[1]*src[10] - src[2]*src[9];
	d3 = src[1]*src[6]  - src[2]*src[5];
	det = src[0]*d1 - src[4]*d2 + src[8]*d3;

	return det;
}

pIFXMatrix4x4_CalcDeterminant3x3 _IFXMatrix4x4_CalcDeterminant3x3 = IFXMatrix4x4_CalcDeterminant3x3;

F32 IFXMatrix4x4::CalcDeterminant3x3() const
{
	return _IFXMatrix4x4_CalcDeterminant3x3(m_data);
}

F32 CalcDeterminant3x3(F32 a1, F32 a2, F32 a3, F32 b1, F32 b2, F32 b3, F32 c1, F32 c2, F32 c3)
{
	return a1*(b2*c3-b3*c2)-b1*(a2*c3-a3*c2)+c1*(a2*b3-a3*b2);
}

F32 IFXMatrix4x4::CalcDeterminant3x3(F32 a1, F32 a2, F32 a3, F32 b1, F32 b2, F32 b3, F32 c1, F32 c2, F32 c3)
{
	return ::CalcDeterminant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

// CalcDeterminant

F32 IFXMatrix4x4_CalcDeterminant(const F32* m)
{
	return
		m[0]*CalcDeterminant3x3(m[5],m[6],m[7],m[9],m[10],m[11],m[13],m[14],m[15])
		-m[4]*CalcDeterminant3x3(m[1],m[2],m[3],m[9],m[10],m[11],m[13],m[14],m[15])
		+m[8]*CalcDeterminant3x3(m[1],m[2],m[3],m[5],m[6],m[7],m[13],m[14],m[15])
		-m[12]*CalcDeterminant3x3(m[1],m[2],m[3],m[5],m[6],m[7],m[9],m[10],m[11]);
}

pIFXMatrix4x4_CalcDeterminant _IFXMatrix4x4_CalcDeterminant = IFXMatrix4x4_CalcDeterminant;

F32 IFXMatrix4x4::CalcDeterminant() const
{
	return IFXMatrix4x4_CalcDeterminant(m_data);
}

// Invert3x4

IFXRESULT IFXMatrix4x4_Invert3x4(F32* dst, const F32* src)
{
	F32 det = IFXMatrix4x4_CalcDeterminant3x3(src);
	if(fabsf(det) > IFXNEARZERO)
	{
		det = 1.0f / det;

		dst[0]  =  det*(src[5]*src[10] - src[6]*src[9]);
		dst[1]  = -det*(src[1]*src[10] - src[2]*src[9]);
		dst[2]  =  det*(src[1]*src[6]  - src[2]*src[5]);
		dst[3]  = 0;
		dst[4]  = -det*(src[4]*src[10] - src[6]*src[8]);
		dst[5]  =  det*(src[0]*src[10] - src[2]*src[8]);
		dst[6]  = -det*(src[0]*src[6]  - src[2]*src[4]);
		dst[7]  = 0;
		dst[8]  =  det*(src[4]*src[9]  - src[5]*src[8]);
		dst[9]  = -det*(src[0]*src[9]  - src[1]*src[8]);
		dst[10] =  det*(src[0]*src[5]  - src[1]*src[4]);
		dst[11] = 0;
		dst[12] = -(src[12]*dst[0]+src[13]*dst[4]+src[14]*dst[8]);
		dst[13] = -(src[12]*dst[1]+src[13]*dst[5]+src[14]*dst[9]);
		dst[14] = -(src[12]*dst[2]+src[13]*dst[6]+src[14]*dst[10]);
		dst[15] = 1;
		return IFX_OK;
	}

	return IFX_E_INVALID_RANGE;
}

pIFXMatrix4x4_Invert3x4 _IFXMatrix4x4_Invert3x4 = IFXMatrix4x4_Invert3x4;

IFXRESULT IFXMatrix4x4::Invert3x4(const IFXMatrix4x4 &operand)
{
	IFXASSERT(this!=&operand);
	return IFXMatrix4x4_Invert3x4(m_data, operand.m_data);
}

// Invert

IFXRESULT IFXMatrix4x4_Invert(F32* a, const F32* m)
{
	const F32 det = IFXMatrix4x4_CalcDeterminant(m);

	//* If determinant not valid, can't compute inverse (return identity)
	if(fabsf(det)<ifxEpsilon)
	{
		IFXMatrix4x4_MakeIdentity(a);
		return IFX_E_INVALID_RANGE;
	}

	const F32 inv=1.0f/det;

	a[0]=   inv*CalcDeterminant3x3(m[5],m[6],m[7],m[9],m[10],m[11],m[13],m[14],m[15]);
	a[1]=  -inv*CalcDeterminant3x3(m[1],m[2],m[3],m[9],m[10],m[11],m[13],m[14],m[15]);
	a[2]=   inv*CalcDeterminant3x3(m[1],m[2],m[3],m[5],m[6],m[7],m[13],m[14],m[15]);
	a[3]= -inv*CalcDeterminant3x3(m[1],m[2],m[3],m[5],m[6],m[7],m[9],m[10],m[11]);

	a[4]=  -inv*CalcDeterminant3x3(m[4],m[6],m[7],m[8],m[10],m[11],m[12],m[14],m[15]);
	a[5]=   inv*CalcDeterminant3x3(m[0],m[2],m[3],m[8],m[10],m[11],m[12],m[14],m[15]);
	a[6]=  -inv*CalcDeterminant3x3(m[0],m[2],m[3],m[4],m[6],m[7],m[12],m[14],m[15]);
	a[7]=  inv*CalcDeterminant3x3(m[0],m[2],m[3],m[4],m[6],m[7],m[8],m[10],m[11]);

	a[8]=   inv*CalcDeterminant3x3(m[4],m[5],m[7],m[8],m[9],m[11],m[12],m[13],m[15]);
	a[9]=  -inv*CalcDeterminant3x3(m[0],m[1],m[3],m[8],m[9],m[11],m[12],m[13],m[15]);
	a[10]=  inv*CalcDeterminant3x3(m[0],m[1],m[3],m[4],m[5],m[7],m[12],m[13],m[15]);
	a[11]= -inv*CalcDeterminant3x3(m[0],m[1],m[3],m[4],m[5],m[7],m[8],m[9],m[11]);

	a[12]=  -inv*CalcDeterminant3x3(m[4],m[5],m[6],m[8],m[9],m[10],m[12],m[13],m[14]);
	a[13]=   inv*CalcDeterminant3x3(m[0],m[1],m[2],m[8],m[9],m[10],m[12],m[13],m[14]);
	a[14]= -inv*CalcDeterminant3x3(m[0],m[1],m[2],m[4],m[5],m[6],m[12],m[13],m[14]);
	a[15]=  inv*CalcDeterminant3x3(m[0],m[1],m[2],m[4],m[5],m[6],m[8],m[9],m[10]);

	return IFX_OK;
}

pIFXMatrix4x4_Invert _IFXMatrix4x4_Invert = IFXMatrix4x4_Invert;

IFXRESULT IFXMatrix4x4::Invert(const IFXMatrix4x4 &operand)
{
	return _IFXMatrix4x4_Invert(m_data, operand.m_data);
}

// Multiply3x4

void IFXMatrix4x4_Multiply3x4(F32* m_data, const F32* m1, const F32* m2)
{
	m_data[ 0]=m1[ 0]*m2[ 0]+m1[ 1]*m2[ 4]+m1[ 2]*m2[ 8];
	m_data[ 1]=m1[ 0]*m2[ 1]+m1[ 1]*m2[ 5]+m1[ 2]*m2[ 9];
	m_data[ 2]=m1[ 0]*m2[ 2]+m1[ 1]*m2[ 6]+m1[ 2]*m2[10];

	m_data[ 4]=m1[ 4]*m2[ 0]+m1[ 5]*m2[ 4]+m1[ 6]*m2[ 8];
	m_data[ 5]=m1[ 4]*m2[ 1]+m1[ 5]*m2[ 5]+m1[ 6]*m2[ 9];
	m_data[ 6]=m1[ 4]*m2[ 2]+m1[ 5]*m2[ 6]+m1[ 6]*m2[10];

	m_data[ 8]=m1[ 8]*m2[ 0]+m1[ 9]*m2[ 4]+m1[10]*m2[ 8];
	m_data[ 9]=m1[ 8]*m2[ 1]+m1[ 9]*m2[ 5]+m1[10]*m2[ 9];
	m_data[10]=m1[ 8]*m2[ 2]+m1[ 9]*m2[ 6]+m1[10]*m2[10];

	m_data[12]=m1[12]*m2[ 0]+m1[13]*m2[ 4]+m1[14]*m2[ 8]+m2[12];
	m_data[13]=m1[12]*m2[ 1]+m1[13]*m2[ 5]+m1[14]*m2[ 9]+m2[13];
	m_data[14]=m1[12]*m2[ 2]+m1[13]*m2[ 6]+m1[14]*m2[10]+m2[14];
}

pIFXMatrix4x4_Multiply3x4 _IFXMatrix4x4_Multiply3x4 = IFXMatrix4x4_Multiply3x4;

IFXMatrix4x4 &IFXMatrix4x4::Multiply3x4(const IFXMatrix4x4 &left, const IFXMatrix4x4 &right)
{
	IFXASSERT(this!=&left);
	IFXASSERT(this!=&right);
	_IFXMatrix4x4_Multiply3x4(m_data, right.m_data, left.m_data);
	return *this;
}
