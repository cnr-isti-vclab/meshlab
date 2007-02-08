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

#ifndef __VCGLIB_QUADRIC5
#define __VCGLIB_QUADRIC5

#include "algebra5.h"
#include <vcg/math/quadric.h>

using namespace vcg::math;

template<typename  Scalar>
class Quadric5
{
public:
    typedef Scalar ScalarType;
	typedef  CMeshO::VertexType::FaceType FaceType;
	
	// the real quadric
	ScalarType a[15];
	ScalarType b[5];
	ScalarType c;
	
	inline Quadric5() { c = -1;}

	// Necessari se si utilizza stl microsoft
	// inline bool operator <  ( const Quadric & q ) const { return false; }
	// inline bool operator == ( const Quadric & q ) const { return true; }

	bool IsValid() const { return (c>=0); }
	void SetInvalid() { c = -1.0; }

	void Zero()																// Azzera le quadriche
	{
		a[0] = 0;
		a[1] = 0;
		a[2] = 0;
		a[3] = 0;
		a[4] = 0;
		a[5] = 0;
		a[6] = 0;
		a[7] = 0;
		a[8] = 0;
		a[9] = 0;
		a[10] = 0;
		a[11] = 0;
		a[12] = 0;
		a[13] = 0;
		a[14] = 0;

		b[0] = 0;
		b[1] = 0;
		b[2] = 0;
		b[3] = 0;
		b[4] = 0;

		c    = 0;
	}

	// computes the real quadric and the geometric quadric using the face
	// The geometric quadric is added to the parameter qgeo
	void byFace(FaceType &f, math::Quadric<double> &q1, math::Quadric<double> &q2, math::Quadric<double> &q3)
	{
		// computes the geometrical quadric
		byFace(f,true);

		q1.a[0] += a[0];
		q1.a[1] += a[1];
		q1.a[2] += a[2];
		q1.a[3] += a[5];
		q1.a[4] += a[6];

		q1.a[5] += a[9];

		q1.b[0] += b[0];
		q1.b[1] += b[1];
		q1.b[2] += b[2];

		q1.c += c;

		q2.a[0] += a[0];
		q2.a[1] += a[1];
		q2.a[2] += a[2];
		q2.a[3] += a[5];
		q2.a[4] += a[6];

		q2.a[5] += a[9];

		q2.b[0] += b[0];
		q2.b[1] += b[1];
		q2.b[2] += b[2];

		q2.c += c;

		q3.a[0] += a[0];
		q3.a[1] += a[1];
		q3.a[2] += a[2];
		q3.a[3] += a[5];
		q3.a[4] += a[6];

		q3.a[5] += a[9];

		q3.b[0] += b[0];
		q3.b[1] += b[1];
		q3.b[2] += b[2];

		q3.c += c;


		// computes the real quadric
		byFace(f,false);
	}
	
	// Computes the geometrical quadric if onlygeo == true and the real quadric if onlygeo == false
	void byFace(FaceType &fi, bool onlygeo)
	{
		ScalarType pe1;
		ScalarType pe2;
		ScalarType tmpmat[5][5];  
		ScalarType tmpsymmat[15];  
		ScalarType tmpvec[5];  
		ScalarType p[5]; 
		ScalarType q[5];
		ScalarType r[5];
		ScalarType A[5][5];  
		ScalarType e1[5];
		ScalarType e2[5];
		ScalarType diffe[5];

		CVertexO &v1 = (*fi.V(0));
		CVertexO &v2 = (*fi.V(1));
		CVertexO &v3 = (*fi.V(2));

		// computes p
		p[0] = v1.P().X();
		p[1] = v1.P().Y();
		p[2] = v1.P().Z();

		if(!onlygeo)
		{
			vcg::TCoord2<float>& t = fi.WT(0);
			p[3] = t.u();
			p[4] = t.v();
		}
		else
		{
			p[3] = 0;
			p[4] = 0;
		}

		//  computes q
		q[0] = v2.P().X();
		q[1] = v2.P().Y();
		q[2] = v2.P().Z();

		if(!onlygeo)
		{
			vcg::TCoord2<float>& t = fi.WT(1);
			q[3] = t.u();
			q[4] = t.v();
		}
		else
		{
			q[3] = 0;
			q[4] = 0;
		}


		//  computes r
		r[0] = v3.P().X();
		r[1] = v3.P().Y();
		r[2] = v3.P().Z();
		if(!onlygeo)
		{
			vcg::TCoord2<float>& t = fi.WT(2);
			r[3] = t.u();
			r[4] = t.v();
		}
		else
		{
			r[3] = 0;
			r[4] = 0;
		}


		//  computes e1
		sub_vec5(q,p,e1);
		normalize_vec5(e1);
		
		//  computes e2
		sub_vec5(r,p,diffe);
		outproduct5(e1,diffe,tmpmat);
		prod_matvec5(tmpmat,e1,tmpvec);
		sub_vec5(diffe,tmpvec,e2);
		normalize_vec5(e2);


		// computes A
		a[0] = 1;
		a[1] = 0;
		a[2] = 0;
		a[3] = 0;
		a[4] = 0;
		a[5] = 1;
		a[6] = 0;
		a[7] = 0;
		a[8] = 0;
		a[9] = 1;
		a[10] = 0;
		a[11] = 0;
		a[12] = 1;
		a[13] = 0;
		a[14] = 1;

		symprod_vvt5(tmpsymmat,e1);
		sub_symmat5(a,tmpsymmat);
		symprod_vvt5(tmpsymmat,e2);
		sub_symmat5(a,tmpsymmat);


		pe1 = inproduct5(p,e1);
		pe2 = inproduct5(p,e2);
		
		//  computes b

		tmpvec[0] = pe1*e1[0] + pe2*e2[0]; 
		tmpvec[1] = pe1*e1[1] + pe2*e2[1]; 
		tmpvec[2] = pe1*e1[2] + pe2*e2[2]; 
		tmpvec[3] = pe1*e1[3] + pe2*e2[3]; 
		tmpvec[4] = pe1*e1[4] + pe2*e2[4];

		sub_vec5(tmpvec,p,b);

		//  computes c
		c = inproduct5(p,p)-pe1*pe1-pe2*pe2;

		
		
	}

	bool Gauss55( ScalarType x[], ScalarType C[5][5+1] )
	{
		const ScalarType keps = (ScalarType)1e-6;
		int i,j,k;

		ScalarType eps;					// Determina valore cond.
			eps = math::Abs(C[0][0]);
		for(i=1;i<5;++i)
		{
			ScalarType t = math::Abs(C[i][i]);
			if( eps<t ) eps = t;
		}
		eps *= keps;

		for (i = 0; i < 5 - 1; ++i)    		// Ciclo di riduzione
		{
			int ma = i;				// Ricerca massimo pivot
			ScalarType vma = math::Abs( C[i][i] );
			for (k = i + 1; k < 5; k++)
			{
				ScalarType t = math::Abs( C[k][i] );
				if (t > vma)
				{
					vma = t;
					ma  = k;
				}
			}
			if (vma<eps)
				return false;        			// Matrice singolare
			if(i!=ma)				// Swap del massimo pivot
				for(k=0;k<=5;k++)
				{
					ScalarType t = C[i][k];
					C[i][k] = C[ma][k];
					C[ma][k] = t;
				}

			for (k = i + 1; k < 5; k++)        	//  Riduzione
			{
				ScalarType s;
				s = C[k][i] / C[i][i];
				for (j = i+1; j <= 5; j++)
					C[k][j] -= C[i][j] * s;
				C[k][i] = 0.0;
			}
		}

			// Controllo finale singolarita'
		if( math::Abs(C[5-1][5- 1])<eps)
			return false;

		for (i=5-1; i>=0; i--)			// Sostituzione
		{
			ScalarType t;
			for (t = 0.0, j = i + 1; j < 5; j++)
				t += C[i][j] * x[j];
			x[i] = (C[i][5] - t) / C[i][i];
		}

		return true;
	}

	
	// computes the minimum of the quadric, imposing the geometrical constraint (geo[3] and geo[4] are obviosly ignored)
	bool MinimumWithGeoContraints(ScalarType x[5],ScalarType geo[5])
	{	
		x[0] = geo[0];
		x[1] = geo[1];
		x[2] = geo[2];

		ScalarType C3 = -(b[3]+geo[0]*a[3]+geo[1]*a[7]+geo[2]*a[10]);
		ScalarType C4 = -(b[4]+geo[0]*a[4]+geo[1]*a[8]+geo[2]*a[11]);

		if(a[12] != 0)
		{
			double tmp = (a[14]-a[13]*a[13]/a[12]);

			if(tmp == 0)
				return false;

			x[4] = (C4 - a[13]*C3/a[12])/ tmp;
			x[3] = (C3 - a[13]*x[4])/a[12];
		}
		else
		{
			if(a[13] == 0)
				return false;

			x[4] = C3/a[13];
			x[3] = (C4 - a[14]*x[4])/a[13];
		}

		return true;
	}

	// computes the minimum of the quadric
	bool Minimum(ScalarType x[5])
	{	
			ScalarType C[5][6];

			C[0][0] = a[0];
			C[0][1] = a[1];
			C[0][2] = a[2];
			C[0][3] = a[3];
			C[0][4] = a[4];
			C[1][0] = a[1];
			C[1][1] = a[5];
			C[1][2] = a[6];
			C[1][3] = a[7];
			C[1][4] = a[8];
			C[2][0] = a[2];
			C[2][1] = a[6];
			C[2][2] = a[9];
			C[2][3] = a[10];
			C[2][4] = a[11];
			C[3][0] = a[3];
			C[3][1] = a[7];
			C[3][2] = a[10];
			C[3][3] = a[12];
			C[3][4] = a[13];
			C[4][0] = a[4];
			C[4][1] = a[8];
			C[4][2] = a[11];
			C[4][3] = a[13];
			C[4][4] = a[14];

			C[0][5]=-b[0];
			C[1][5]=-b[1];
			C[2][5]=-b[2];
			C[3][5]=-b[3];
			C[4][5]=-b[4];
			
			return Gauss55(&(x[0]),C);
	}

	void operator = ( const Quadric5<double> & q )			// Assegna una quadrica
	{
		//assert( IsValid() );
		assert( q.IsValid() );

		a[0] = q.a[0];
		a[1] = q.a[1];
		a[2] = q.a[2];
		a[3] = q.a[3];
		a[4] = q.a[4];
		a[5] = q.a[5];
		a[6] = q.a[6];
		a[7] = q.a[7];
		a[8] = q.a[8];
		a[9] = q.a[9];
		a[10] = q.a[10];
		a[11] = q.a[11];
		a[12] = q.a[12];
		a[13] = q.a[13];
		a[14] = q.a[14];

		b[0] = q.b[0];
		b[1] = q.b[1];
		b[2] = q.b[2];
		b[3] = q.b[3];
		b[4] = q.b[4];

		c    = q.c;
	}

	// sums the geometrical and the real quadrics
	void operator += ( const Quadric5<double> & q )			
	{
		//assert( IsValid() );
		assert( q.IsValid() );

		a[0] += q.a[0];
		a[1] += q.a[1];
		a[2] += q.a[2];
		a[3] += q.a[3];
		a[4] += q.a[4];
		a[5] += q.a[5];
		a[6] += q.a[6];
		a[7] += q.a[7];
		a[8] += q.a[8];
		a[9] += q.a[9];
		a[10] += q.a[10];
		a[11] += q.a[11];
		a[12] += q.a[12];
		a[13] += q.a[13];
		a[14] += q.a[14];

		b[0] += q.b[0];
		b[1] += q.b[1];
		b[2] += q.b[2];
		b[3] += q.b[3];
		b[4] += q.b[4];

		c    += q.c;

	}

/*
it sums the real quadric of the class with a quadric obtained by the geometrical quadric of the vertex.
This quadric is obtained extending to five dimensions the geometrical quadric and simulating that it has been
obtained by sums of 5-dimension quadrics which were computed using vertexes and faces with always the same values 
in the fourth and fifth dimensions (respectly the function parameter u and the function parameter v).
this allows to simulate the inexistant continuity in vertexes with multiple texture coords
however this continuity is still inexistant, so even if the algorithm makes a good collapse with this expedient,it obviously
computes bad the priority......this should be adjusted with the extra weight user parameter through.....

*/
	void inline Sum3 (const math::Quadric<double> & q3, float u, float v)
  {
		assert( q3.IsValid() );

		a[0] += q3.a[0];
		a[1] += q3.a[1];
		a[2] += q3.a[2];

		a[5] += q3.a[3];
		a[6] += q3.a[4];

		a[9] += q3.a[5];
	
		a[12] += 1;
		a[14] += 1;

		b[0] += q3.b[0];
		b[1] += q3.b[1];
		b[2] += q3.b[2];

		b[3] -= u;
		b[4] -= v;

		c    += q3.c + u*u + v*v;

	}

  // returns the quadric value in v
  	ScalarType Apply(ScalarType v[5])
	{

		assert( IsValid() );

		ScalarType tmpmat[5][5];  
		ScalarType tmpvec[5];  

		tmpmat[0][0] = a[0]; 
		tmpmat[0][1] = tmpmat[1][0] = a[1]; 
		tmpmat[0][2] = tmpmat[2][0] = a[2]; 
		tmpmat[0][3] = tmpmat[3][0] = a[3]; 
		tmpmat[0][4] = tmpmat[4][0] = a[4]; 
		
		tmpmat[1][1] = a[5]; 
		tmpmat[1][2] = tmpmat[2][1] = a[6]; 
		tmpmat[1][3] = tmpmat[3][1] = a[7]; 
		tmpmat[1][4] = tmpmat[4][1] = a[8]; 

		tmpmat[2][2] = a[9]; 
		tmpmat[2][3] = tmpmat[3][2] = a[10]; 
		tmpmat[2][4] = tmpmat[4][2] = a[11]; 

		tmpmat[3][3] = a[12]; 
		tmpmat[3][4] = tmpmat[4][3] = a[13]; 

		tmpmat[4][4] = a[14];

		prod_matvec5(tmpmat,v,tmpvec);

		return  inproduct5(v,tmpvec) + 2*inproduct5(b,v) + c;

	}
};


#endif
