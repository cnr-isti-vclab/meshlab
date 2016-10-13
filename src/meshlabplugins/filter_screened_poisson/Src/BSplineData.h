/*
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#ifndef BSPLINE_DATA_INCLUDED
#define BSPLINE_DATA_INCLUDED

#include "PPolynomial.h"
#include "Array.h"

template< int Degree >
struct BSplineElementCoefficients
{
	int coeffs[Degree+1];
	BSplineElementCoefficients( void ){ memset( coeffs , 0 , sizeof( int ) * ( Degree+1 ) ); }
	int& operator[]( int idx ){ return coeffs[idx]; }
	const int& operator[]( int idx ) const { return coeffs[idx]; }
};
template< int Degree >
struct BSplineElements : public std::vector< BSplineElementCoefficients< Degree > >
{
	static const int _off = (Degree+1)/2;
	void _addLeft ( int offset , int boundary );
	void _addRight( int offset , int boundary );
public:
	enum
	{
		NONE      =  0,
		DIRICHLET = -1,
		NEUMANN   =  1
	};
	// Coefficients are ordered as "/" "-" "\"
	int denominator;

	BSplineElements( void ) { denominator = 1; }
	BSplineElements( int res , int offset , int boundary=NONE , int inset=0 );

	void upSample( BSplineElements& high ) const;
	void differentiate( BSplineElements< Degree-1 >& d ) const;

	void print( FILE* fp=stdout ) const
	{
		for( int i=0 ; i<std::vector< BSplineElementCoefficients< Degree > >::size() ; i++ )
		{
			printf( "%d]" , i );
			for( int j=0 ; j<=Degree ; j++ ) printf( " %d" , (*this)[i][j] );
			printf( " (%d)\n" , denominator );
		}
	}
};

template< int Degree >
class BSplineData
{
	int _boundaryType;
	double _vvIntegrals[Degree+1][Degree+1];
	double _vdIntegrals[Degree+1][Degree  ];
	double _dvIntegrals[Degree  ][Degree+1];
	double _ddIntegrals[Degree  ][Degree  ];

public:
	struct Integrator
	{
		struct IntegralTables
		{
			double vv_ccIntegrals[2*Degree+1][2*Degree+1] , vv_cpIntegrals[(2*Degree+1)*2][2*Degree+1];
			double dv_ccIntegrals[2*Degree+1][2*Degree+1] , dv_cpIntegrals[(2*Degree+1)*2][2*Degree+1];
			double vd_ccIntegrals[2*Degree+1][2*Degree+1] , vd_cpIntegrals[(2*Degree+1)*2][2*Degree+1];
			double dd_ccIntegrals[2*Degree+1][2*Degree+1] , dd_cpIntegrals[(2*Degree+1)*2][2*Degree+1];
		};
		std::vector< IntegralTables > iTables;
		double dot( int depth , int off1 , int off2 , bool d1 , bool d2 , bool childParent=false ) const;
	};
	double dot( int depth1 , int off1 , int depth2 , int off2 , bool d1 , bool d2 , bool inset=false ) const;
	void setIntegrator( Integrator& integrator , bool inset , bool useDotRatios=false ) const;
	template< int Radius >
	struct CenterEvaluator
	{
		struct ValueTables
		{
			double vValues[2*Degree+1][ 3*(2*Radius+1) ];
			double dValues[2*Degree+1][ 3*(2*Radius+1) ];
		};
		std::vector< ValueTables > vTables;
		double value( int depth , int off1 , int off2 , bool d , bool childParent=false ) const;
	};
	template< int Radius >
	void setCenterEvaluator( CenterEvaluator< Radius >& evaluator , double smoothingRadius , double dSmoothingRadius, bool inset ) const;
	double value( int depth , int off , double smoothingRadius , double s , bool d , bool inset=false ) const;
	template< int Radius >
	struct CornerEvaluator
	{
		struct ValueTables
		{
			double vValues[2*Degree+1][4*Radius+3];
			double dValues[2*Degree+1][4*Radius+3];
		};
		std::vector< ValueTables > vTables;
		double value( int depth , int off1 , int c1 , int off2 , bool d , bool childParent=false ) const;
	};
	template< int Radius >
	void setCornerEvaluator( CornerEvaluator< Radius >& evaluator , double smoothingRadius , double dSmoothingRadius, bool inset ) const;

	struct BSplineComponents
	{
		Polynomial< Degree > polys[Degree+1];
		Polynomial< Degree >& operator[] ( int idx ) { return polys[idx]; }
		const Polynomial< Degree >& operator[] ( int idx ) const { return polys[idx]; }
		void printnl( void ) const  { for( int d=0 ; d<=Degree ; d++ ) polys[d].printnl(); }
		BSplineComponents scale( double s ) const { BSplineComponents b ; for( int d=0 ; d<=Degree ; d++ ) b[d] = polys[d].scale(s) ; return b; }
		BSplineComponents shift( double s ) const { BSplineComponents b ; for( int d=0 ; d<=Degree ; d++ ) b[d] = polys[d].shift(s) ; return b; }
	};

	int depth;
	size_t functionCount , sampleCount;
	PPolynomial< Degree   >  baseFunction ,  leftBaseFunction ,  rightBaseFunction ,  leftRightBaseFunction;
	PPolynomial< Degree-1 > dBaseFunction , dLeftBaseFunction , dRightBaseFunction , dLeftRightBaseFunction;
	BSplineComponents baseBSpline , leftBSpline , rightBSpline , leftRightBSpline;
	Pointer( PPolynomial< Degree > ) baseFunctions;
	Pointer( BSplineComponents ) baseBSplines;

	BSplineData( void );

	const static int  VV_DOT_FLAG = 1;
	const static int  DV_DOT_FLAG = 2;
	const static int  DD_DOT_FLAG = 4;
	const static int   VALUE_FLAG = 1;
	const static int D_VALUE_FLAG = 2;
	template< class Real >
	struct DotTables
	{
		size_t functionCount;
		Pointer( Real ) vvDotTable;
		Pointer( Real ) dvDotTable;
		Pointer( Real ) ddDotTable;

		DotTables( void );
		~DotTables( void );

		inline size_t Index( int i1 , int i2 ) const;
		static inline size_t SymmetricIndex( int i1 , int i2 );
		static inline int SymmetricIndex( int i1 , int i2 , size_t& index );
	};
	template< class Real >
	struct ValueTables
	{
		size_t functionCount , sampleCount;
		Pointer( Real ) valueTable;
		Pointer( Real ) dValueTable;

		ValueTables( void );
		~ValueTables( void );

		inline size_t Index( int i1 , int i2 ) const;
		void setSampleSpan( int idx , int& start , int& end , double smooth=0 ) const;
	};
	void set( int maxDepth , int boundaryType=BSplineElements< Degree >::NONE );
	template< class Real >
	typename BSplineData< Degree >::template DotTables< Real > getDotTables( int flags , bool useDotRatios=true , bool inset=false ) const;
	template< class Real >
	typename BSplineData< Degree >::template ValueTables< Real > getValueTables( int flags , double valueSmooth=0 , double normalSmooth=0 ) const;
};

template< int Degree1 , int Degree2 > void SetBSplineElementIntegrals( double integrals[Degree1+1][Degree2+1] );

#include "BSplineData.inl"
#endif // BSPLINE_DATA_INCLUDED