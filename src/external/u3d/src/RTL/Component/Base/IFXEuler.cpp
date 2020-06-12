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
	@file	IFXEuler.cpp

			This module defines the IFXEuler interface.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXEuler.h"
#include "IFXTransform.h"

IFXEuler& IFXEuler::operator=(const IFXQuaternion& rOperand)
{
#if TRUE
	IFXVector3 xaxis(1.0f,0.0f,0.0f);

	IFXTransform transform=rOperand;
	transform.Quaternion(READWRITE).ForcePositiveW();

	IFXQuaternion qinv,result;
	IFXVector3 rotated;
	transform.RotateVector(xaxis,rotated);

	IFXASSERT(result[0]!=0.0f);
	m_data[2]= IFXATAN2(rotated[1],rotated[0]);

	qinv.MakeRotation(-m_data[2],IFX_Z_AXIS);
	result.Multiply(qinv,transform.QuaternionConst());
	result.ForcePositiveW();

	transform=result;

	transform.RotateVector(xaxis,rotated);

	IFXASSERT(result[0]!=0.0f);
	m_data[1]= -IFXATAN2(rotated[2],rotated[0]);

	qinv.MakeRotation(-m_data[1],IFX_Y_AXIS);
	result.Multiply(qinv,transform.QuaternionConst());
	result.ForcePositiveW();

	if(result[1]< -1.0f)
		m_data[0]= -IFXPI;
	else if(result[1]> 1.0f)
		m_data[0]=IFXPI;
	else
	{
		IFXASSERT(result[1]>= -1.0f);
		IFXASSERT(result[1]<=1.0f);
		m_data[0]=2.0f*IFXASIN(result[1]);
	}
#else
	IFXVector3 zaxis(0.0f,0.0f,1.0f);

	IFXTransform transform=rOperand;
	transform.Quaternion(READWRITE).ForcePositiveW();

	IFXQuaternion qinv,result;
	IFXVector3 rotated;
	transform.RotateVector(zaxis,rotated);

	m_data[0]= -IFXATAN2(rotated[1],rotated[2]);

	qinv.MakeRotation(-m_data[0],IFX_X_AXIS);
	result.Multiply(qinv,transform.QuaternionConst());
	result.ForcePositiveW();

	transform=result;

	transform.RotateVector(zaxis,rotated);

	m_data[1]= IFXATAN2(rotated[0],rotated[2]);

	qinv.MakeRotation(-m_data[1],IFX_Y_AXIS);
	result.Multiply(qinv,transform.QuaternionConst());
	result.ForcePositiveW();

	m_data[2]=2.0f*IFXASIN(result[3]);
#endif

	return *this;
}

void IFXEuler::ConvertToQuaternion(IFXQuaternion& rOperand) const
{
	rOperand.MakeRotation(m_data[2],IFX_Z_AXIS);
	rOperand.Rotate(m_data[1],IFX_Y_AXIS);
	rOperand.Rotate(m_data[0],IFX_X_AXIS);
}
