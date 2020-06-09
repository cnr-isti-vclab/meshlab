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
#ifndef MATRIX4X4_DOT_H
#define MATRIX4X4_DOT_H

#include "Primitives.h"
#include <memory.h>


// Matrix4x4

#define SIZE4x4 64  // 16 x 4
class Matrix4x4
{
	friend class Matrix1x4;
private:
	float data[4][4]; // [row][col]
	void copy(Matrix4x4 &m);

public:
	Matrix4x4 () {};
	Matrix4x4 (float values[16])	{ memcpy(data, values, SIZE4x4); };
	Matrix4x4 (Matrix4x4 &m)		{ copy(m); };

	void reset();
	void set (float values[16])		{ memcpy(data, values, SIZE4x4); };

	void operator += (Matrix4x4 &m);			// m4x4 += m4x4
	void operator *= (float scalar);			// m4x4 *= float
};


inline void Matrix4x4::copy (Matrix4x4 &m)
{
	memcpy (data, m.data, SIZE4x4);
}


//  Matrix1x4

class Matrix4x1;

#define SIZE1x4 16
class Matrix1x4
{
private:
	float data[4];
	inline void copy (Matrix1x4 &m)  { memcpy (data, m.data, SIZE1x4); };

public:
	// Constructors / initializers:
	inline Matrix1x4 ()								{};
	inline Matrix1x4 (IV3D &v)						{ set(v); };
	inline Matrix1x4 (float values[4])				{ memcpy (data, values, SIZE1x4); };
	inline Matrix1x4 (float x, float y, float z)	{ data[0]=x; data[1]=y; data[2]=z; data[3]=1.0f; };
	inline Matrix1x4 (Matrix1x4 &m)					{ copy(m); };
	inline void set	(float x, float y, float z)		{ data[0]=x; data[1]=y; data[2]=z; data[3]=1.0f; };
	inline void set (IV3D &v)						{ data[0]=v.x; data[1]=v.y; data[2]=v.z; data[3]=1.0f; };

	// Operators:
	Matrix1x4 &	operator * (Matrix4x4 &m);					// m1x4 = m1x4 * m4x4
	float		operator * (Matrix4x1 &m);					// scalar = m1x4 * m4x1
	void		operator = (Matrix1x4 &m)	{ copy(m); };	// m1x4 = m1x4;	
};


// Matrix4x1

class Matrix4x1
{
	friend class Matrix1x4;

private:
	float data[4];

public:
	void set (float x, float y, float z) { data[0]=x; data[1]=y; data[2]=z; data[3]=1.0f; };
	void set (IV3D &v) { data[0]=v.x; data[1]=v.y; data[2]=v.z; data[3]=1.0f; };
};


#endif
