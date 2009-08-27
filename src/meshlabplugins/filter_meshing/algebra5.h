/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
  History
$Log$
Revision 1.1  2007/02/08 13:39:59  pirosu
Added Quadric Simplification(with textures) Filter


****************************************************************************/
#ifndef __VCGLIB_ALGEBRA5_H
#define __VCGLIB_ALGEBRA5_H

#include<math.h>

namespace vcg {
	namespace math {

		typedef double ScalarType;

		// r = a-b
		void inline sub_vec5(const ScalarType a[5], const ScalarType b[5], ScalarType r[5])
		{
			r[0] = a[0] - b[0];
			r[1] = a[1] - b[1];
			r[2] = a[2] - b[2];
			r[3] = a[3] - b[3];
			r[4] = a[4] - b[4];
		}

		// returns the in-product a*b
		ScalarType inline inproduct5(const ScalarType a[5], const ScalarType b[5])
		{
			return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]+a[4]*b[4];
		}

		// r = out-product of a*b
		void inline outproduct5(const ScalarType a[5], const ScalarType b[5], ScalarType r[5][5])
		{
			for(int i = 0; i < 5; i++)
				for(int j = 0; j < 5; j++)
					r[i][j] = a[i]*b[j];
		}

		// r = m*v
		void inline prod_matvec5(ScalarType m[5][5], ScalarType v[5], ScalarType r[5])
		{
			r[0] = inproduct5(m[0],v);	
			r[1] = inproduct5(m[1],v);	
			r[2] = inproduct5(m[2],v);	
			r[3] = inproduct5(m[3],v);	
			r[4] = inproduct5(m[4],v);	
		}

		// r = (v transposed)*m
		void inline prod_vecmat5(ScalarType v[5],ScalarType m[5][5], ScalarType r[5])
		{
			for(int i = 0; i < 5; i++)
				for(int j = 0; j < 5; j++)
					r[j] = v[j]*m[j][i];
		}

		void inline normalize_vec5(ScalarType v[5])
		{
			ScalarType norma = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]+v[4]*v[4]);

			v[0]/=norma;
			v[1]/=norma;
			v[2]/=norma;
			v[3]/=norma;
			v[4]/=norma;
		}

		void inline normalize_vec3(ScalarType v[3])
		{
			ScalarType norma = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

			v[0]/=norma;
			v[1]/=norma;
			v[2]/=norma;

		}

		// dest -= m

		void inline sub_mat5(ScalarType dest[5][5],ScalarType m[5][5])
		{
			for(int i = 0; i < 5; i++)
				for(int j = 0; j < 5; j++)
					dest[i][j] -= m[i][j];
		}

		/* computes the symmetric matrix v*v */
		void inline symprod_vvt5(ScalarType dest[15],ScalarType v[5])
		{
			dest[0] = v[0]*v[0];
			dest[1] = v[0]*v[1];
			dest[2] = v[0]*v[2];
			dest[3] = v[0]*v[3];
			dest[4] = v[0]*v[4];
			dest[5] = v[1]*v[1];
			dest[6] = v[1]*v[2];
			dest[7] = v[1]*v[3];
			dest[8] = v[1]*v[4];
			dest[9] = v[2]*v[2];
			dest[10] = v[2]*v[3];
			dest[11] = v[2]*v[4];
			dest[12] = v[3]*v[3];
			dest[13] = v[3]*v[4];
			dest[14] = v[4]*v[4];

		}

		/* subtracts symmetric matrix */
		void inline sub_symmat5(ScalarType dest[15],ScalarType m[15])
		{
			for(int i = 0; i < 15; i++)
				dest[i] -= m[i];
		}

	}
}
#endif