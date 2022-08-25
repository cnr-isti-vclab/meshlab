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

#ifndef IFXTRANSFORM_H
#define IFXTRANSFORM_H


#include "IFXMatrix4x4.h"
#include "IFXQuaternion.h"

#define IFXROTATE_BY_QUAT TRUE  // stack rotate using quats vs. matrix
#define IFXXFORM_BY_QUAT  FALSE // vector transform using quats vs. matrix


enum IFXReadWrite {READONLY,WRITEONLY,READWRITE};

/**
	@file	IFXTransform.h

	@brief	3D Transform with JIT auto-conversions.

			Wraps most IFXMatrix4x4 and IFXQuaternion functions
			to do automatic JIT conversions and control validity flags.
			The lack of virtuals is intentional for speed.

			Uses modeling-optimized 3x4 versions of IFXMatrix4x4 operations.

			To get data from the OpenGL current matrix:
			glGetFloatv(GL_MODELVIEW_MATRIX,transform.GetMatrixData(WRITEONLY));

			To put data into the OpenGL current matrix:
			glLoadMatrixf(transform.GetMatrixData(READONLY));

			To use relative to the OpenGL current matrix:
			glMultMatrixf(transform.GetMatrixData(READONLY));

			The last two require being in GL_MODELVIEW mode like with:
			glMatrixMode(GL_MODELVIEW);

			@warning Scaling is a recent feature with minimal testing.
			@todo: check matrix inverse use of scaling
			@todo: check IFXROTATE_BY_QUAT of FALSE
*/


/** This class wraps most IFXMatrix4x4 and IFXQuaternion functions
to do automatic JIT conversions and control validity flags. */
class IFXTransform
{
public:
	/** Default construct calls Reset().*/
	IFXTransform(void)  { Reset(); }

	/** Resets state to an identity matrix.*/
	void      Reset(void)
	{
		m_matrixValid=TRUE;
		m_matinverseValid=FALSE;
		m_quaternionValid=FALSE;
		m_matrix.Reset();
		m_matrixInverse.Reset();
	}

	IFXTransform(const IFXTransform& rOperand)
	{
		Reset();
		operator=(rOperand);
	}

	IFXTransform(const IFXMatrix4x4& rOperand)
	{
		Reset();
		operator=(rOperand);
	}

	IFXTransform(const IFXQuaternion& rOperand)
	{
		Reset();
		operator=(rOperand);
	}

	IFXTransform& operator=(const IFXTransform&  rOperand);
	IFXTransform& operator=(const IFXMatrix4x4&  rOperand);
	IFXTransform& operator=(const IFXQuaternion& rOperand);

	// not a const function since JIT update
	/** Compares to an arbitrary matrix,
	doing JIT conversion as necessary. */
	BOOL	operator==(const IFXMatrix4x4& rOperand)
	{
		UpdateMatrix();
		return (m_matrix==rOperand);
	}

	/** Multiplies the transform by an arbitrary matrix.
	Internally, this is currently done with a
	3x4-simplified matrix-matrix multiply. */
	IFXTransform& Multiply(const IFXMatrix4x4& rOperand);

	/** Multiplies the transform by an arbitrary transform.
	Internally, this is currently done with a
	3x4-simplified matrix-matrix multiply. */
	IFXTransform& Multiply(IFXTransform& rOperand)
	{ 
		return Multiply(rOperand.MatrixConst()); 
	}

	/** Sets this transform to the interpolated point
	between two other transform using t between
	0.0f and 1.0f. Rotation interpolation is done
	using Quaternion SLERP.  Translation and Scale
	is interpolated linearly. */
	BOOL      Interpolate(F32 t, IFXTransform& rFrom, IFXTransform& rTo);

	/** Sets the transform to identity.*/
	void      MakeIdentity(void);

	/** Sets the transform based on a given translation,
	rotation, and scale. */
	void      MakeTRS(	const IFXVector3& rTranslation,
						const IFXQuaternion& rRotation,
						const IFXVector3& rScale);

	/** Decomposes this matrix into translation, rotation,
	and scale (non-rotational shear is discarded). */
	IFXRESULT   CalcTRS(IFXVector3* pTranslation,
						IFXQuaternion* pRotation,
						IFXVector3* pScale);

	/** Premultiplies a translation.*/
	void      Translate(F32 x, F32 y, F32 z);

	/** Premultiplies a translation (vector form).*/
	void      Translate(const IFXVector3& rVector);

	/** Premultiplies an x,y,z scale.*/
	void      Scale(F32 x, F32 y, F32 z);

	/** Premultiplies an x,y,z scale (vector form).*/
	void      Scale(const IFXVector3& rVector);

	/** Premultiplies a rotation.*/
	void      Rotate(const IFXQuaternion& rOperand);

	/** Rotates about a primary axis
	(IFX_X_AXIS, IFX_Y_AXIS, or IFX_Z_AXIS). */
	void      Rotate(F32 radians, IFXAxisID axis);

	/** Rotates about an arbitrary axis.*/
	void      Rotate(F32 radians, const IFXVector3& rAxis);

	/** Rotates about an arbitrary axis displaced
	to an arbitrary pivot point. */
	void      RotateAbout(	const IFXVector3& rPivot,
							F32 radians, const IFXVector3& rAxis);

	/** Transforms a vector through this object.*/
	void      TransformVector(	const IFXVector3& rSource,
								IFXVector3& rResult);

	/** Transforms a vector through this object
	(F32* version avoids a copy). */
	void      TransformVector(const F32* pSource, IFXVector3& rResult);

	/** Transforms a vector through the inverse.*/
	void      ReverseTransformVector(	const IFXVector3& rSource,
										IFXVector3& rResult);

	/** Transforms a vector through the inverse
	(F32* version avoids a copy). */
	void      ReverseTransformVector(	const F32* pSource,
										IFXVector3& rResult);

	/** Rotates vector by this transform without
	translating or scaling. */
	void      RotateVector(	const IFXVector3& rSource,
							IFXVector3& rResult);

	/** Rotates vector by this transform without
	translating or scaling
	(F32* version avoids a copy). */
	void      RotateVector(const F32* pSource, IFXVector3& rResult);

	/** Rotate vector by inverse of transform without
	translating or scaling. */
	void      ReverseRotateVector(	const F32* pSource,
									IFXVector3& rResult);

	/** Rotates vector by inverse of transform without
	translating or scaling
	(F32* version avoids a copy). */
	void      ReverseRotateVector(	const IFXVector3& rSource,
									IFXVector3& rResult);

	/** Returns the matrix,
	doing JIT conversion as necessary. */
	const IFXMatrix4x4& MatrixConst(void)
	{
		// read implied, so make sure it's valid
		UpdateMatrix();
		return m_matrix;
	}

	// For GetMatrixData() and Quaternion(),
	// note that WRITEONLY mode may be faster,
	// but doesn't make sure current data is valid.

	// WRITEONLY and READONLY are your intent
	// and can't be enforced such as with const.
	// The returned value should be used
	// immediately, then ignored.

	// for read-only, try GetMatrixDataConst()

	/** Returns the matrix in F32* form.  This is not a
	copy.  The mode must be READONLY, WRITEONLY,
	or READWRITE, depending on what you intend to
	do with it.  These are not enforce, but can
	cause errors if not followed correctly. */
	F32* GetMatrixData(IFXReadWrite mode);

	/** Returns the matrix in F32* form. */
	const F32* GetMatrixDataConst(void)
	{
		// read implied, so make sure it's valid
		UpdateMatrix();
		return m_matrix.RawConst();
	}

	/** Returns the inverse matrix,
	doing JIT conversion as necessary. */
	const IFXMatrix4x4& MatrixInverseConst(void)
	{
		// read implied, so make sure it's valid
		UpdateMatrixInverse();
		return m_matrixInverse;
	}

	/** Returns the inverse matrix in F32* form. */
	const F32* GetMatrixInverseDataConst(void)
	{
		// read implied, so make sure it's valid
		UpdateMatrixInverse();
		return m_matrixInverse.RawConst();
	}

	/** Returns the current rotation.  This is not a
	copy.  The mode must be READONLY, WRITEONLY,
	or READWRITE, depending on what you intend to
	do with it.  These are not enforce, but can
	cause errors if not followed correctly. */
	IFXQuaternion& Quaternion(IFXReadWrite mode);
	
	/** Returns the rotation.*/
	const IFXQuaternion& QuaternionConst(void)
	{
		// read implied, so make sure it's valid
		UpdateQuaternion();
		return m_quaternion;
	}

	/** Replaces the current scale.*/
	void      SetScale(const IFXVector3& rSet);

	/** Returns the scale for x,y,z.*/
	const IFXVector3& ScaleConst(void)
	{
		// read implied, so make sure it's valid
		UpdateQuaternion();
		return m_scale;
	}
	/** Resets scale to unity.*/
	void      RemoveScale(void)
	{ 
		SetScale(IFXVector3(1.0f,1.0f,1.0f)); 
	}

	/** Replaces the current translation.*/
	void      SetTranslation(const IFXVector3& rVector)
	{ 
		m_matrix.SetTranslation(rVector); 
	}

	/** Returns the translation.*/
	const IFXVector3& TranslationConst(void) const
	{
		return m_matrix.TranslationConst(); 
	}

	/** (debugging) Returns true if quaternion conversion
	is up to date. */
	BOOL      QuaternionIsValid(void) 
	{ 
		return m_quaternionValid; 
	}

	/** (debugging) Returns true if matrix conversion
	is up to date. */
	BOOL      MatrixIsValid(void)   
	{ 
		return m_matrixValid; 
	}

	/** (debugging) Returns true if either quaternion
	or matrix conversion.  Since the constructor
	calls Reset(), false would likely indicate an
	internal failure, deleted object, or bad cast. */
	BOOL      IsValid(void)
	{ 
		return m_matrixValid || m_quaternionValid; 
	}

#if FALSE
	// NOTE experimental
	void      LimitToEulerZ(void);
	F32       ExtractEulerZ(void);
#endif

	/** Forces JIT conversion to matrix if not
	already valid. */
	void      UpdateMatrix(void);

private:

	// Forces JIT conversion to inverse if not
	// already valid.
	void      UpdateMatrixInverse(void);

	// Forces JIT conversion to quaterion
	// and scale if not already valid.
	void      UpdateQuaternion(void);

	static  void	RotateVectorBy(	const IFXMatrix4x4& rMat,
									const F32* pSource, IFXVector3& rResult);

	void	RotateVectorByQuat(	const F32* pSource,
								IFXVector3& rResult);

	void	ReverseRotateVectorByQuat(	const F32* pSource,
										IFXVector3& rResult);

	IFXMatrix4x4  m_matrix;
	IFXMatrix4x4  m_matrixInverse;

	IFXQuaternion m_quaternion;
	IFXVector3    m_scale;    // valid when m_quaternionValid

	BOOL      m_matrixValid;
	BOOL      m_matinverseValid;
	BOOL      m_quaternionValid;
};

IFXINLINE IFXTransform& IFXTransform::operator=(const IFXMatrix4x4& rOperand)
{
	m_matrix=rOperand;

	m_matrixValid=TRUE;
	m_matinverseValid=FALSE;
	m_quaternionValid=FALSE;

	return *this;
}

IFXINLINE IFXTransform& IFXTransform::operator=(const IFXQuaternion& rOperand)
{
	m_quaternion=rOperand;
	m_scale.Set(1.0f,1.0f,1.0f);
	m_matrix.ResetNonRotation();

	m_matrixValid=FALSE;
	m_matinverseValid=FALSE;
	m_quaternionValid=TRUE;

	return *this;
}

IFXINLINE void IFXTransform::TransformVector(const IFXVector3& rSource,
											 IFXVector3& rResult)
{
	TransformVector(rSource.RawConst(),rResult);
}

IFXINLINE void IFXTransform::TransformVector(const F32* pSource,
											 IFXVector3& rResult)
{
	F32 *pV=rResult.Raw();
	const F32 *pMat=m_matrix.RawConst();

	RotateVector(pSource,rResult);

	pV[0]+=pMat[12];
	pV[1]+=pMat[13];
	pV[2]+=pMat[14];
}

IFXINLINE void IFXTransform::ReverseTransformVector(const IFXVector3& rSource,
													IFXVector3& rResult)
{
	ReverseTransformVector(rSource.RawConst(),rResult);
}

IFXINLINE void IFXTransform::ReverseTransformVector(const F32* pSource,
													IFXVector3& rResult)
{
	IFXVector3 temp;
	const F32 *pMat=m_matrix.RawConst();
	F32 *pT=temp.Raw();

	pT[0]=pSource[0]-pMat[12];
	pT[1]=pSource[1]-pMat[13];
	pT[2]=pSource[2]-pMat[14];

	ReverseRotateVector(temp,rResult);
}

IFXINLINE void IFXTransform::RotateVector(const F32* pSource,
										  IFXVector3& rResult)
{
#if IFXXFORM_BY_QUAT
	RotateVectorByQuat(pSource,rResult);
	rResult.Multiply(m_scale);
#else
	UpdateMatrix();
	RotateVectorBy(m_matrix,pSource,rResult);
#endif
}

IFXINLINE void IFXTransform::RotateVector(const IFXVector3& rSource,
										  IFXVector3& rResult)
{
#if IFXXFORM_BY_QUAT
	RotateVectorByQuat(rSource.RawConst(),rResult);
	rResult.Multiply(m_scale);
#else
	UpdateMatrix();
	RotateVectorBy(m_matrix,rSource.RawConst(),rResult);
#endif
}

IFXINLINE void IFXTransform::ReverseRotateVector(const F32* pSource,
												 IFXVector3& rResult)
{
#if IFXXFORM_BY_QUAT
	IFXVector3 temp(pSource);
	temp.Divide(m_scale);
	ReverseRotateVectorByQuat(temp,rResult);
#else
	UpdateMatrixInverse();
	RotateVectorBy(m_matrixInverse,pSource,rResult);
#endif
}

IFXINLINE void IFXTransform::ReverseRotateVector(const IFXVector3& rSource,
												 IFXVector3& rResult)
{
#if IFXXFORM_BY_QUAT
	IFXVector3 temp(rSource);
	temp.Divide(m_scale);
	ReverseRotateVectorByQuat(temp.RawConst(),rResult);
#else
	UpdateMatrixInverse();
	RotateVectorBy(m_matrixInverse,rSource.RawConst(),rResult);
#endif
}

IFXINLINE void IFXTransform::Translate(F32 x,F32 y,F32 z)
{
	IFXVector3 vector(x,y,z);
	Translate(vector);
}

IFXINLINE void IFXTransform::Translate(const IFXVector3& rVector)
{
#if IFXROTATE_BY_QUAT
	// Quaternion version
	IFXVector3 rotated;
	RotateVectorByQuat(rVector.RawConst(),rotated);
	rotated.Multiply(m_scale);
	m_matrix[12]+=rotated[0];
	m_matrix[13]+=rotated[1];
	m_matrix[14]+=rotated[2];
	m_matrixValid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	m_matrix.Translate3x4(rVector);
	m_quaternionValid=FALSE;
#endif
	m_matinverseValid=FALSE;
}

IFXINLINE void IFXTransform::Scale(F32 x,F32 y,F32 z)
{
	IFXVector3 vector(x,y,z);
	Scale(vector);
}
IFXINLINE void IFXTransform::Scale(const IFXVector3& rVector)
{
#if IFXROTATE_BY_QUAT
	// Quaternion version
	UpdateQuaternion();
	m_scale.Multiply(rVector);
	m_matrixValid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	m_matrix.Scale3x4(rVector);
	m_quaternionValid=FALSE;
#endif
	m_matinverseValid=FALSE;
}

IFXINLINE void IFXTransform::Rotate(const IFXQuaternion& rOperand)
{
#if IFXROTATE_BY_QUAT
	// Quaternion version
	UpdateQuaternion();
	m_quaternion.Rotate(rOperand);
	m_matrixValid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	IFXMatrix4x4 copy=m_matrix;
	m_matrix.Multiply(copy,IFXMatrix4x4(rOperand));
	m_quaternionValid=FALSE;
#endif
	m_matinverseValid=FALSE;
}

IFXINLINE void IFXTransform::Rotate(F32 radians, IFXAxisID axis)
{
#if IFXROTATE_BY_QUAT
	// Quaternion version
	UpdateQuaternion();
	m_quaternion.Rotate(radians,axis);
	m_matrixValid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	m_matrix.Rotate3x4(radians,axis);
	m_quaternionValid=FALSE;
#endif
	m_matinverseValid=FALSE;
}

IFXINLINE void IFXTransform::Rotate(F32 radians, const IFXVector3& rAxis)
{
	IFXQuaternion rotation(radians,rAxis);
	Rotate(rotation);
}

IFXINLINE void IFXTransform::RotateAbout(const IFXVector3& rPivot,
										 F32 radians, const IFXVector3& rAxis)
{
	const F32 *pV=rPivot.RawConst();
	IFXQuaternion rotation(radians,rAxis);

	Translate(-pV[0],-pV[1],-pV[2]);
	Rotate(rotation);
	Translate(rPivot);
}

// static
IFXINLINE void IFXTransform::RotateVectorBy(const IFXMatrix4x4& rMat,
											const F32* pSource, 
											IFXVector3& rResult)
{
	const F32 *pM=rMat.RawConst();

	// unrolled
	rResult[0]=pSource[0]*pM[0]+pSource[1]*pM[4]+pSource[2]*pM[8];
	rResult[1]=pSource[0]*pM[1]+pSource[1]*pM[5]+pSource[2]*pM[9];
	rResult[2]=pSource[0]*pM[2]+pSource[1]*pM[6]+pSource[2]*pM[10];
}

IFXINLINE void IFXTransform::RotateVectorByQuat(const F32* pSource,
												IFXVector3& rResult)
{
	// Note: auto-conversions between m_quat and vector

	UpdateQuaternion();
	m_quaternion.RotateVector(pSource,rResult.Raw());
	rResult.Multiply(m_scale);
}

#endif
