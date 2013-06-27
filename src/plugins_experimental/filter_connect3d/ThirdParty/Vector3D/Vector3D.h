// Title:   Vector3D.h
// Created: Thu Sep 25 14:35:05 2003
// Authors: Richard Keiser, Oliver Knoll, Mark Pauly, Matthias Zwicker
//
// Copyright (c) 2001, 2002, 2003 Computer Graphics Lab, ETH Zurich
//
// This file is part of the Pointshop3D system.
// See http://www.pointshop3d.com/ for more information.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
//
// Contact info@pointshop3d.com if any conditions of this
// licensing are not clear to you.
//

#ifndef __VECTOR3D_H_
#define __VECTOR3D_H_

#include <math.h>

//#include "DataTypesDLL.h"
#define DATATYPES_API 

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

#ifndef PI_180
#define PI_180 (float)M_PI/180.0f 
#endif

#define SWAP(x,y) \
	tmp = entry[x][y];	\
	entry[x][y] = entry[y][x];	\
	entry[y][x] = tmp;	


////////////////////////////////////////////////////////////////////////////
//
// 3D vector
//

class DATATYPES_API Vector3D {

public:

	/////////////////////////////////////////////////////////////////
	// constructors
	/////////////////////////////////////////////////////////////////	
	inline Vector3D (void)	{};

	/////////////////////////////////////////////////////////////////

	inline Vector3D (float p0, float p1, float p2)
	{ 
		p[0] = p0;
		p[1] = p1;
		p[2] = p2; 
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D (float* pp)
	{ 
		p[0] = pp[0];
		p[1] = pp[1];
		p[2] = pp[2]; 
	};

	/////////////////////////////////////////////////////////////////

	inline Vector3D (const Vector3D& v)
	{ 
		*this = v;
	};

	/////////////////////////////////////////////////////////////////
	// overloaded operators
	/////////////////////////////////////////////////////////////////
	
	inline float& operator[] (int Index)	
	{
		return (p[Index]);
	};


	/////////////////////////////////////////////////////////////////

	inline const float& operator[] (int Index) const	
	{
		return (p[Index]);
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator= (const Vector3D& P)
	{
		p[0] = P[0];
		p[1] = P[1];
		p[2] = P[2];
		return (*this);
	};

	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator+= (Vector3D P)
	{
		p[0] += P[0];
		p[1] += P[1];
		p[2] += P[2];
		return (*this);
	};

	
	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator-= (Vector3D P)
	{
		p[0] -= P[0];
		p[1] -= P[1];
		p[2] -= P[2];
		return (*this);
	};
		

	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator*= (Vector3D P)
	{
		p[0] *= P[0];
		p[1] *= P[1];
		p[2] *= P[2];
		return (*this);
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator*= (float s)
	{
		p[0] *= s;
		p[1] *= s;
		p[2] *= s;
		return (*this);
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator/= (Vector3D P)
	{
		p[0] /= P[0];
		p[1] /= P[1];
		p[2] /= P[2];
		return (*this);
	};

	/////////////////////////////////////////////////////////////////

	inline Vector3D& operator/= (float s)
	{
		p[0] /= s;
		p[1] /= s;
		p[2] /= s;
		return (*this);
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D operator+ (const Vector3D P) const
	{
		Vector3D res;
		res[0] = p[0] + P[0];
		res[1] = p[1] + P[1];
		res[2] = p[2] + P[2];
		return (res); 
	};

	
	/////////////////////////////////////////////////////////////////

	inline Vector3D operator- (const Vector3D P) const
	{
		Vector3D res;
		res[0] = p[0] - P[0];
		res[1] = p[1] - P[1];
		res[2] = p[2] - P[2];
		return (res); 
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D operator- () const
	{
		Vector3D res;
		res[0] = -p[0];
		res[1] = -p[1];
		res[2] = -p[2];
		return (res); 
	};

	/////////////////////////////////////////////////////////////////

	inline Vector3D operator* (const Vector3D P) const
	{
		Vector3D res;
		res[0] = p[0] * P[0];
		res[1] = p[1] * P[1];
		res[2] = p[2] * P[2];
		return (res); 
	};

	/////////////////////////////////////////////////////////////////

	inline Vector3D operator* (float s) const
	{
		Vector3D res;
		res[0] = p[0] * s;
		res[1] = p[1] * s;
		res[2] = p[2] * s;
		return (res); 
	};


	/////////////////////////////////////////////////////////////////

	inline Vector3D operator/ (const Vector3D P) const
	{
		Vector3D res;
		res[0] = p[0] / P[0];
		res[1] = p[1] / P[1];
		res[2] = p[2] / P[2];
		return (res); 
	};

	/////////////////////////////////////////////////////////////////

	inline Vector3D operator/ (float s) const
	{
		Vector3D res;
		float invS = 1.0f / s;
		res[0] = p[0] * invS;
		res[1] = p[1] * invS;
		res[2] = p[2] * invS;
		return (res); 
	};


	
	/////////////////////////////////////////////////////////////////
	
	inline bool operator == (const Vector3D &a) const
	{
		return(p[0] == a[0] && p[1] == a[1] && p[2] == a[2]);
	};
	
	
	/////////////////////////////////////////////////////////////////
	
	inline bool operator != (const Vector3D &a) const
	{
		return(p[0] != a[0] || p[1] != a[1] || p[2] != a[2]);
	};
	
	
	/////////////////////////////////////////////////////////////////
	
	inline bool operator < (const Vector3D &a) const
	{
		return(p[0] < a[0] && p[1] < a[1] && p[2] < a[2]);
	};
	
	
	/////////////////////////////////////////////////////////////////
	
	inline bool operator >= (const Vector3D &a) const
	{
		return(p[0] >= a[0] && p[1] >= a[1] && p[2] >= a[2]);
	};


	/////////////////////////////////////////////////////////////////
	inline float* source (void) { return p; };


	/////////////////////////////////////////////////////////////////
	
	inline void makeZero()
	{
		p[0] = p[1] = p[2] = 0;
	};

	/////////////////////////////////////////////////////////////////
	
	inline void makeNegative() {
		p[0] = -p[0];
		p[1] = -p[1];
		p[2] = -p[2];
	};

	/////////////////////////////////////////////////////////////////

	inline float getSquaredLength() const
	{
		return( p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
	};
	
	/////////////////////////////////////////////////////////////////

	inline float getLength() const
	{
		return (float)sqrt (getSquaredLength()); 
	};


	/////////////////////////////////////////////////////////////////
	// normalize and return length

	inline float normalize (void) 
	{
		float length = getLength();
		if (length == 0.0f)
			return 0;

		float rezLength = 1.0f / length;
		p[0] *= rezLength;
		p[1] *= rezLength;
		p[2] *= rezLength;
		return length;
	};

	/////////////////////////////////////////////////////////////////
	// vector between the two points

	inline void fromTo (Vector3D& P1, Vector3D& P2)
	{
		p[0] = P2[0] - P1[0];
		p[1] = P2[1] - P1[1];
		p[2] = P2[2] - P1[2];
	};

	/**
	 * returns two orthogonal vectors u and v
	 * attention: u and v are not normalized
	 */
	inline void getTwoOrthogonals (Vector3D &u, Vector3D &v) const
	{
		if (fabs(p[0]) < fabs(p[1])) {
			// stefan: hack to make sure u[0] is never 0 for ray intersection
/*
			if (fabs(p[0]) < fabs(p[2])) {
				// p[0] is minimum
				u = Vector3D (0, -p[2], p[1]);
			}
			else {
				// p[2] is mimimum
*/
			u = Vector3D (-p[1], p[0], 0);
//			}
			// stefan
		}
		else { 
			if (fabs(p[1]) < fabs(p[2])) {
				
				// p[1] is minimum
				u = Vector3D (p[2], 0, -p[0]);
			}
			else {
				// p[2] is mimimum
				u = Vector3D(-p[1], p[0], 0);
			}
		}
		v = crossProduct (*this, u);
	}

	/**
	 * returns two normalized orthogonal vectors u and v
	 */
	inline void getTwoNormalizedOrthogonals (Vector3D &u, Vector3D &v) const
	{
		if (fabs(p[0]) < fabs(p[1])) {
			if (fabs(p[0]) < fabs(p[2])) {
				// p[0] is minimum
				u = Vector3D (0, -p[2], p[1]);
			}
			else {
				// p[2] is mimimum
				u = Vector3D (-p[1], p[0], 0);
			}
		}
		else { 
			if (fabs(p[1]) < fabs(p[2])) {
				
				// p[1] is minimum
				u = Vector3D (p[2], 0, -p[0]);
			}
			else {
				// p[2] is mimimum
				u = Vector3D(-p[1], p[0], 0);
			}
		}
		v = crossProduct (*this, u);

		u.normalize();
		v.normalize();
	}
		

	static inline Vector3D crossProduct(const Vector3D &a, const Vector3D &b)
	{
		Vector3D result;
	
		result[0] = a[1] * b[2] - a[2] * b[1];
		result[1] = a[2] * b[0] - a[0] * b[2];
		result[2] = a[0] * b[1] - a[1] * b[0];

		return(result);
	}


	static inline float dotProduct(const Vector3D &a, const Vector3D &b)
	{
		return(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
	}
		
	
	static inline float squaredDistance (const Vector3D &v1, const Vector3D &v2)
	{
		return ((v1 - v2).getSquaredLength());
	}
	
	
	static inline float distance (const Vector3D &v1, const Vector3D &v2)
	{
		return ((v1 - v2).getLength());
	}
	
	
	
	
	/**
	 * converts cartesion to polar coordinates
	 * result:
	 * [0] = length
	 * [1] = angle with z-axis
	 * [2] = angle of projection into x,y, plane with x-axis
	 */	
	static inline Vector3D cartesianToPolar (Vector3D &v)
	{
		Vector3D polar;
		polar[0] = v.getLength();
		
		if (v[2] > 0.0f) {
			polar[1] = (float) atan (sqrt (v[0] * v[0] + v[1] * v[1]) / v[2]);
		}
		else if (v[2] < 0.0f) {
			polar[1] = (float) atan (sqrt (v[0] * v[0] + v[1] * v[1]) / v[2]) + M_PI;
		}
		else {
			polar[1] = M_PI * 0.5f;
		}
		
		
		if (v[0] > 0.0f) {
			polar[2] = (float) atan (v[1] / v[0]);
		}
		else if (v[0] < 0.0f) {
			polar[2] = (float) atan (v[1] / v[0]) + M_PI;
		}
		else if (v[1] > 0) {
			polar[2] = M_PI * 0.5f;
		}
		else {
			polar[2] = -M_PI * 0.5;
		}
		return polar;
	}
	
	
	
	/**
	 * converts polar to cartesion coordinates
	 * input:
	 * [0] = length
	 * [1] = angle with z-axis
	 * [2] = angle of projection into x,y, plane with x-axis
	 */	
	static inline Vector3D polarToCartesian (Vector3D &v)
	{
		Vector3D cart;
		cart[0] = v[0] * (float) sin (v[1]) * (float) cos (v[2]);
		cart[1] = v[0] * (float) sin (v[1]) * (float) sin (v[2]);
		cart[2] = v[0] * (float) cos (v[1]);
		return cart;
	}
	
	
	/////////////////////////////////////////////////////////////////
	
	static inline Vector3D projectOntoVector (Vector3D &v1, Vector3D &v2)
	{
		return v2 * dotProduct (v1, v2);
	}
	

	static inline Vector3D projectVectorIntoPlane (Vector3D &v1, Vector3D &normal)
	{
		return v1 - projectOntoVector (v1, normal);
	}


	static inline Vector3D projectPointOntoPlane (Vector3D &point, Vector3D &anchor, Vector3D &normal) 
        {
                        
		Vector3D temp = point - anchor;
		return point - projectOntoVector (temp, normal);
	}


protected:

	float p[3];

};


////////////////////////////////////////////////////////////////////////////

inline Vector3D operator * (float s, const Vector3D &P)
{
    return (P * s);
}





////////////////////////////////////////////////////////////////////////////
//
// used for 3d transformations
//

class DATATYPES_API Matrix
{
	
public:
	
	

	inline float getEntry (const unsigned int y, const unsigned int x) const
	{
		return entry[y][x];
	};

	inline void setEntry (const unsigned int y, const unsigned int x, float value)
	{
		entry[y][x] = value;
	};

	inline Vector3D getCol (const unsigned int y) const
	{
		return Vector3D(entry[y][0], entry[y][1], entry[y][2]);
	};

	inline void setCol (const unsigned int y, Vector3D value)
	{
		entry[y][0] = value[0];
		entry[y][1] = value[1];
		entry[y][2] = value[2];
	};

	inline void scaleMatrix (float x, float y, float z)
	{
		entry[0][0] = x;
		entry[1][1] = y;
		entry[2][2] = z;
		entry[0][1] = entry[0][2] = entry[1][0] = entry[1][2] = entry[2][0] = entry[2][1] = 0;
	};


	inline void copy (Matrix* other)
	{
		int i, j;
		
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j ++)
				entry[i][j] = other->entry[i][j];
	};
		

	inline Vector3D transformPoint (const Vector3D &from) const
	{
		int i,j;
		Vector3D To (0, 0, 0);
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
				To[i] += entry[i][j] * from[j];
		}
		return (To);
	};

	inline Matrix* multiply (Matrix *other) const
	{
		Matrix *New = new Matrix();
		
		int i, j, k;
		
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j ++)
			{
				New->entry[i][j] = 0;
				for (k = 0; k < 3; k++)
					New->entry[i][j] += entry[i][k] * other->entry[k][j];
			}
			return New;
	};
	
	inline void transpose() {
		float tmp;
		SWAP(0,1);
		SWAP(0,2);
		SWAP(1,2);
	}

	inline void scale (float scale)
	{		
		int i, j;
		
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j ++)
				entry[i][j] *= scale;
	};



	inline void computeInverse (void) {

		Matrix temp;
		temp.copy (this);
		this->computeInverse (&temp);
	};
		



	inline void computeInverse (Matrix* other)
	{
		
		//determinant
		float det = 1 / (	other->entry[0][0] * other->entry[1][1] * other->entry[2][2] -
			other->entry[0][0] * other->entry[1][2] * other->entry[2][1] -
			other->entry[1][0] * other->entry[0][1] * other->entry[2][2] +
			other->entry[1][0] * other->entry[0][2] * other->entry[2][1] +
			other->entry[2][0] * other->entry[0][1] * other->entry[1][2] -
			other->entry[2][0] * other->entry[0][2] * other->entry[1][1]);
		
		entry[0][0] = (other->entry[1][1] * other->entry[2][2] - other->entry[1][2] * other->entry[2][1]) * det;
		entry[0][1] = -(other->entry[0][1] * other->entry[2][2] - other->entry[0][2] * other->entry[2][1]) * det;
		entry[0][2] = (other->entry[0][1] * other->entry[1][2] - other->entry[0][2] * other->entry[1][1]) * det;
		entry[1][0] = -(other->entry[1][0] * other->entry[2][2] - other->entry[1][2] * other->entry[2][0]) * det;
		entry[1][1] = (other->entry[0][0] * other->entry[2][2] - other->entry[0][2] * other->entry[2][0]) * det;
		entry[1][2] = -(other->entry[0][0] * other->entry[1][2] - other->entry[0][2] * other->entry[1][0]) * det;
		entry[2][0] = (other->entry[1][0] * other->entry[2][1] - other->entry[1][1] * other->entry[2][0]) * det;
		entry[2][1] = -(other->entry[0][0] * other->entry[2][1] - other->entry[0][1] * other->entry[2][0]) * det;
		entry[2][2] = (other->entry[0][0] * other->entry[1][1] - other->entry[0][1] * other->entry[1][0]) * det;

	};

	inline Matrix getInverse () const
	{
		Matrix res;
		//determinant
		float det = 1 / (	entry[0][0] * entry[1][1] * entry[2][2] -
			entry[0][0] * entry[1][2] * entry[2][1] -
			entry[1][0] * entry[0][1] * entry[2][2] +
			entry[1][0] * entry[0][2] * entry[2][1] +
			entry[2][0] * entry[0][1] * entry[1][2] -
			entry[2][0] * entry[0][2] * entry[1][1]);
		
		res.setEntry(0,0, (entry[1][1] * entry[2][2] - entry[1][2] * entry[2][1]) * det);
		res.setEntry(0,1,  -(entry[0][1] * entry[2][2] - entry[0][2] * entry[2][1]) * det);
		res.setEntry(0,2, (entry[0][1] * entry[1][2] - entry[0][2] * entry[1][1]) * det);
		res.setEntry(1,0, -(entry[1][0] * entry[2][2] - entry[1][2] * entry[2][0]) * det);
		res.setEntry(1,1, (entry[0][0] * entry[2][2] - entry[0][2] * entry[2][0]) * det);
		res.setEntry(1,2, -(entry[0][0] * entry[1][2] - entry[0][2] * entry[1][0]) * det);
		res.setEntry(2,0, (entry[1][0] * entry[2][1] - entry[1][1] * entry[2][0]) * det);
		res.setEntry(2,1, -(entry[0][0] * entry[2][1] - entry[0][1] * entry[2][0]) * det);
		res.setEntry(2,2, (entry[0][0] * entry[1][1] - entry[0][1] * entry[1][0]) * det);

		return res;

	};


	static Matrix Identity() {
		Matrix res;
		res.scaleMatrix(1,1,1);
		return res;
	}



protected:

	float entry[3][3];
	

	
};

#endif  // __VECTOR3D_H_

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
