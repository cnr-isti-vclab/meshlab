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
	@file	IFXTransform.cpp

			This module defines the IFXTransform class.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXTransform.h"

IFXTransform& IFXTransform::operator=(const IFXTransform& rOperand)
{
	if (this != &rOperand)
	{
		if(rOperand.m_matrixValid)
		{
			m_matrix=rOperand.m_matrix;
			m_matrixValid=TRUE;
		}
		else
		{
			const F32 *pMat=rOperand.m_matrix.RawConst();

			// always copy translation data
			m_matrix.ResetBottom();
			m_matrix[12]=pMat[12];
			m_matrix[13]=pMat[13];
			m_matrix[14]=pMat[14];
			m_matrixValid=FALSE;
		}

		if(rOperand.m_quaternionValid)
		{
			m_quaternion=rOperand.m_quaternion;
			m_scale=rOperand.m_scale;
			m_quaternionValid=TRUE;
		}
		else
			m_quaternionValid=FALSE;

		m_matinverseValid=FALSE;
	}

	return *this;
}


IFXTransform& IFXTransform::Multiply(const IFXMatrix4x4& rOperand)
{
	UpdateMatrix();
	IFXMatrix4x4 temp=m_matrix;
	m_matrix.Multiply3x4(temp,rOperand);

	m_matrixValid=TRUE;
	m_matinverseValid=FALSE;
	m_quaternionValid=FALSE;

	return *this;
}


void IFXTransform::UpdateMatrix(void)
{
	if(!m_matrixValid)
	{
		IFXVector3 translation= &m_matrix[12];
		IFXMatrix4x4 rotation=m_quaternion;

		IFXASSERT(m_quaternionValid);
		m_matrix.MakeIdentity();
		m_matrix.Translate3x4(translation);
		IFXMatrix4x4 copy=m_matrix;
		m_matrix.Multiply3x4(copy,rotation);
		m_matrix.Scale3x4(m_scale);

		m_matrixValid=TRUE;
	}
}


void IFXTransform::UpdateMatrixInverse(void)
{
	if(!m_matinverseValid)
	{
		UpdateQuaternion();

		if(m_quaternionValid)
		{
			// temporary inversion
			m_quaternion.Invert();
			m_scale.Reciprocate();

			m_matrixInverse=m_quaternion;
			m_matrixInverse.Scale3x4(m_scale);

			m_quaternion.Invert();
			m_scale.Reciprocate();

			m_matrixInverse.ResetBottom();
			IFXVector3 translation,result;
			translation[0]= -m_matrix[12];
			translation[1]= -m_matrix[13];
			translation[2]= -m_matrix[14];
			ReverseRotateVectorByQuat(translation.RawConst(),result);
			m_matrixInverse[12]=result[0];
			m_matrixInverse[13]=result[1];
			m_matrixInverse[14]=result[2];

			m_matinverseValid=TRUE;
		}
	}
}


void IFXTransform::UpdateQuaternion(void)
{
	if(!m_quaternionValid)
	{
		IFXASSERT(m_matrixValid);
		
		if (m_matrixValid)
		{
			// also computes scale
			m_matrix.CalcTRS(NULL,&m_quaternion,&m_scale);

			m_matinverseValid=FALSE;
			m_quaternionValid=TRUE;
		}
	}
}


BOOL IFXTransform::Interpolate(F32 t,IFXTransform& rFrom,IFXTransform& rTo)
{
	IFXASSERT(this!=&rFrom);
	IFXASSERT(this!=&rTo);
	// not const args, since JIT m_quat update

	rFrom.UpdateQuaternion();
	rTo.UpdateQuaternion();

	if( !rFrom.QuaternionIsValid() || !rTo.QuaternionIsValid() )
		return false;

	IFXVector3 translation;

	translation.Interpolate(t,IFXVector3(&rFrom.m_matrix[12]),
		IFXVector3(&rTo.m_matrix[12]));
	m_scale.Interpolate(t,rFrom.m_scale,rTo.m_scale);
	m_quaternion.Interpolate(t,rFrom.QuaternionConst(),
		rTo.QuaternionConst());

	SetTranslation(translation);

	m_matrixValid=FALSE;
	m_matinverseValid=FALSE;
	m_quaternionValid=TRUE;

	return true;
}


void IFXTransform::MakeIdentity(void)
{
	m_matrix.ResetNonRotation();
	m_matrix.MakeIdentity();
	m_matrixInverse.ResetNonRotation();
	m_matrixInverse.MakeIdentity();
	m_quaternion.MakeIdentity();
	m_scale.Set(1.0f,1.0f,1.0f);

	m_matrixValid=TRUE;
	m_matinverseValid=TRUE;
	m_quaternionValid=TRUE;
}


IFXRESULT IFXTransform::CalcTRS(IFXVector3* pTranslation,
								IFXQuaternion* pRotation,IFXVector3* pScale)
{
	/// @todo: UpdateQuaternion() should return a value (invert can fail)
	UpdateQuaternion();

	if(pTranslation)
		*pTranslation=&m_matrix[12];
	if(pRotation)
		*pRotation=m_quaternion;
	if(pScale)
		*pScale=m_scale;

	return IFX_OK;
}


void IFXTransform::MakeTRS(const IFXVector3& rTranslation,
						   const IFXQuaternion& rRotation,
						   const IFXVector3& rScale)
{
	SetTranslation(rTranslation);
	m_quaternion=rRotation;
	m_scale=rScale;

	m_matrixValid=FALSE;
	m_matinverseValid=FALSE;
	m_quaternionValid=TRUE;
}


#if FALSE
void IFXTransform::LimitToEulerZ(void)
{
	UpdateQuaternion();
	F32 zangle=m_quaternion.ExtractEulerZ();
	m_quaternion.MakeRotation(zangle,IFX_Z_AXIS);

	m_matrixValid=FALSE;
	m_matinverseValid=FALSE;
}

F32 IFXTransform::ExtractEulerZ(void)
{
	UpdateQuaternion();
	F32 zangle=m_quaternion.ExtractEulerZ();

	m_matrixValid=FALSE;
	m_matinverseValid=FALSE;

	return zangle;
}
#endif


void IFXTransform::ReverseRotateVectorByQuat(const F32* pSource,
											 IFXVector3& rResult)
{
	// Note: auto-conversions between m_quat and vector

	UpdateQuaternion();

	if(m_quaternion.Raw()[0] < 1.0f)
	{

		IFXVector3 temp(pSource);
		temp.Divide(m_scale);

		//* temporary inversion
		m_quaternion.Invert();
		m_quaternion.RotateVector(temp.Raw(),rResult.Raw());
		m_quaternion.Invert();
	}
	else
	{
		IFXVector3 temp(pSource);
		rResult = temp;
	}
}


F32* IFXTransform::GetMatrixData(IFXReadWrite mode)
{
	// but don't waste the time if writing
	if(mode!=WRITEONLY)
		UpdateMatrix();

	if(mode!=READONLY)
	{
		m_matrixValid=TRUE;
		m_matinverseValid=FALSE;
		m_quaternionValid=FALSE;
	}

	return m_matrix.Raw();
}


IFXQuaternion& IFXTransform::Quaternion(IFXReadWrite mode)
{
	// don't waste the time if not reading
	if(mode!=WRITEONLY)
		UpdateQuaternion();

	if(mode!=READONLY)
	{
		m_matrixValid=FALSE;
		m_matinverseValid=FALSE;
		m_quaternionValid=TRUE;

		// todo: What should we do about the scale and pretranslation?
	}

	return m_quaternion;
}


void IFXTransform::SetScale(const IFXVector3& rSet)
{
	UpdateQuaternion();

	m_matrixValid=FALSE;
	m_matinverseValid=FALSE;
	m_quaternionValid=TRUE;

	m_scale=rSet;
}
