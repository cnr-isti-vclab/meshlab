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
#include <stdio.h>
#include "Matrix4x4.h"


void Matrix4x4::reset ()
{
	int i;
	int j;
	for ( i = 0; i < 4; i++)
		for ( j = 0; j < 4; j++)
			data[i][j] = 0;
}


void Matrix4x4::operator += (Matrix4x4 &m)
{
	int i;
	int j;
	for ( i = 0; i < 4; i++)
		for ( j = 0; j < 4; j++)
			data[i][j] += m.data[i][j];
}


void Matrix4x4::operator *= (float scalar)
{
	int i;
	int j;
	for ( i = 0; i < 4; i++)
		for ( j = 0; j < 4; j++)
			data[i][j] *= scalar;
}


//                        Matrix1x4                             //

Matrix1x4 result1x4;

// m1x4(temp) = m1x4 * m4x4
Matrix1x4 & Matrix1x4::operator * (Matrix4x4 &m)
{
	int i;
	for ( i = 0; i < 4; i++)
	{
		result1x4.data[i] = 0.0f;
		int j;
		for ( j = 0; j < 4; j++)
			result1x4.data[i] += data[j] * m.data[j][i];
	}

	return result1x4;
}


float Matrix1x4::operator * (Matrix4x1 &m)
{
	float sum = 0.0f;
	int i;
	for ( i = 0; i < 4; i++)
		sum += data[i] * m.data[i];

	return sum;
}
