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

#include "MultiGridOctreeData.h"

template< class Real , int Degree , bool HasGradients >
struct _ConstraintCalculator_
{
	static inline Real _CalculateConstraint_( const PointData< Real , HasGradients >& p , const Polynomial< Degree >& px , const Polynomial< Degree >& py , const Polynomial< Degree >& pz , const Polynomial< Degree >& dpx , const Polynomial< Degree >& dpy , const Polynomial< Degree >& dpz , Real valueWeight , Real gradientWeight );
	static inline Real _CalculateConstraint_( const PointData< Real , HasGradients >& p , const Polynomial< Degree >& px , const Polynomial< Degree >& py , const Polynomial< Degree >& pz , const Polynomial< Degree >& dpx , const Polynomial< Degree >& dpy , const Polynomial< Degree >& dpz );
#if POINT_DATA_RES
	static inline void _CalculateCoarser_( int c , PointData< Real , HasGradients >& p , Real value , Point3D< Real > gradient , Real valueWeight , Real gradientWeight );
#else // !POINT_DATA_RES
	static inline void _CalculateCoarser_( PointData< Real , HasGradients >& p , Real value , Point3D< Real > gradient , Real valueWeight , Real gradientWeight );
#endif // POINT_DATA_RES

};
template< class Real , int Degree >
struct _ConstraintCalculator_< Real , Degree , false >
{
	static inline Real _CalculateConstraint_( const PointData< Real , false >& p , const Polynomial< Degree >& px , const Polynomial< Degree >& py , const Polynomial< Degree >& pz , const Polynomial< Degree >& , const Polynomial< Degree >& , const Polynomial< Degree >& , Real valueWeight , Real )
	{
#if POINT_DATA_RES
		Real constraint = 0;
		for( int c=0 ; c<PointData< Real , false >::SAMPLES ; c++ ) if( p[c].weight ) 
		{
			const Point3D< Real > q = p[c].position;
			constraint += (Real)( px( q[0] ) * py( q[1] ) * pz( q[2] ) * p[c].weight * p[c].value );
		}
		return constraint * valueWeight;
#else // !POINT_DATA_RES
		const Point3D< Real > q = p.position;
		return (Real)( px( q[0] ) * py( q[1] ) * pz( q[2] ) * p.weight * p.value ) * valueWeight;
#endif // POINT_DATA_RES
	}
	static inline Real _CalculateConstraint_( const PointData< Real , false >& p , const Polynomial< Degree >& px , const Polynomial< Degree >& py , const Polynomial< Degree >& pz , const Polynomial< Degree >& , const Polynomial< Degree >& , const Polynomial< Degree >& )
	{
#if POINT_DATA_RES
		Real constraint = 0;
		for( int c=0 ; c<PointData< Real , false >::SAMPLES ; c++ ) if( p[c].weight ) 
		{
			const Point3D< Real > q = p[c].position;
			constraint += (Real)( px( q[0] ) * py( q[1] ) * pz( q[2] ) * p[c]._value );
		}
		return constraint;
#else // !POINT_DATA_RES
		const Point3D< Real > q = p.position;
		return (Real)( px( q[0] ) * py( q[1] ) * pz( q[2] ) * p._value );
#endif // POINT_DATA_RES
	}
#if POINT_DATA_RES
	static inline void _CalculateCoarser_( int c , PointData< Real , false >& p , Real value , Point3D< Real > , Real valueWeight , Real )
	{
		p[c]._value = value * valueWeight * p[c].weight;
	}
#else // !POINT_DATA_RES
	static inline void _CalculateCoarser_( PointData< Real , false >& p , Real value , Point3D< Real > , Real valueWeight , Real )
	{
		p._value = value * valueWeight * p.weight;
	}
#endif // POINT_DATA_RES
};
template< class Real , int Degree >
struct _ConstraintCalculator_< Real , Degree , true >
{
	static inline Real _CalculateConstraint_( const PointData< Real , true >& p , const Polynomial< Degree >& px , const Polynomial< Degree >& py , const Polynomial< Degree >& pz , const Polynomial< Degree >& dpx , const Polynomial< Degree >& dpy , const Polynomial< Degree >& dpz , Real valueWeight , Real gradientWeight )
	{
#if POINT_DATA_RES
		Real constraint = 0;
		for( int c=0 ; c<PointData< Real , true >::SAMPLES ; c++ ) if( p[c].weight ) 
		{
			const Point3D< Real > q = p[c].position;
			double _px = px( q[0] ) , _py = py( q[1] ) , _pz = pz( q[2] );
			constraint +=
				(
					(Real)( _px * _py * _pz * p[c].value ) * valueWeight +
					Point3D< Real >::Dot( Point3D< Real >( dpx( q[0] ) * _py * _pz , _px * dpy( q[1] ) * _pz , _px * _py * dpz( q[2] ) ) , p[c].gradient ) * gradientWeight
				) * p[c].weight;
		}
		return constraint;
#else // !POINT_DATA_RES
		const Point3D< Real > q = p.position;
		double _px = px( q[0] ) , _py = py( q[1] ) , _pz = pz( q[2] );
		return
			(
			(Real)( _px * _py * _pz * p.value ) * valueWeight +
				Point3D< Real >::Dot( Point3D< Real >( dpx( q[0] ) * _py * _pz , _px * dpy( q[1] ) * _pz , _px * _py * dpz( q[2] ) ) , p.gradient ) * gradientWeight
			) * p.weight;
#endif // POINT_DATA_RES
	}
	static inline Real _CalculateConstraint_( const PointData< Real , true >& p , const Polynomial< Degree >& px , const Polynomial< Degree >& py , const Polynomial< Degree >& pz , const Polynomial< Degree >& dpx , const Polynomial< Degree >& dpy , const Polynomial< Degree >& dpz )
	{
#if POINT_DATA_RES
		Real constraint = 0;
		for( int c=0 ; c<PointData< Real , true >::SAMPLES ; c++ ) if( p[c].weight ) 
		{
			const Point3D< Real > q = p[c].position;
			double _px = px( q[0] ) , _py = py( q[1] ) , _pz = pz( q[2] );
			constraint +=
				(Real)( _px * _py * _pz * p[c]._value ) +
				Point3D< Real >::Dot( Point3D< Real >( dpx( q[0] ) * _py * _pz , _px * dpy( q[1] ) * _pz , _px * _py * dpz( q[2] ) ) , p[c]._gradient );
		}
		return constraint;
#else // !POINT_DATA_RES
		const Point3D< Real > q = p.position;
		double _px = px( q[0] ) , _py = py( q[1] ) , _pz = pz( q[2] );
		return
		(Real)( _px * _py * _pz * p._value ) +
			Point3D< Real >::Dot( Point3D< Real >( dpx( q[0] ) * _py * _pz , _px * dpy( q[1] ) * _pz , _px * _py * dpz( q[2] ) ) , p._gradient );
#endif // POINT_DATA_RES
	}
#if POINT_DATA_RES
	static inline void _CalculateCoarser_( int c , PointData< Real , true >& p , Real value , Point3D< Real > gradient , Real valueWeight , Real gradientWeight ){ p[c]._value = value * valueWeight * p[c].weight ; p[c]._gradient = gradient * gradientWeight * p[c].weight; }
#else // !POINT_DATA_RES
	static inline void _CalculateCoarser_( PointData< Real , true >& p , Real value , Point3D< Real > gradient , Real valueWeight , Real gradientWeight ){ p._value = value * valueWeight * p.weight ; p._gradient = gradient * gradientWeight * p.weight; }
#endif // POINT_DATA_RES
};

template< >
template< class I >
double FEMSystemFunctor< 0 , BOUNDARY_FREE >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 );
	return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight;
#undef D_DOT
}
template< >
template< class I >
double FEMSystemFunctor< 0 , BOUNDARY_NEUMANN >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 );
	return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight;
#undef D_DOT
}
template< >
template< class I >
double FEMSystemFunctor< 0 , BOUNDARY_DIRICHLET >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 );
	return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight;
#undef D_DOT
}
template< >
template< class I >
double FEMSystemFunctor< 1 , BOUNDARY_FREE >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 ) , d11[] = D_DOT( 1 , 1 );
	return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight 
		+
		(
			d11[0] * d00[1] * d00[2] +
			d11[1] * d00[2] * d00[0] +
			d11[2] * d00[0] * d00[1]
			) * lapWeight;
#undef D_DOT
}
template< >
template< class I >
double FEMSystemFunctor< 1 , BOUNDARY_NEUMANN >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 ) , d11[] = D_DOT( 1 , 1 );
	return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight 
		+
		(
			d11[0] * d00[1] * d00[2] +
			d11[1] * d00[2] * d00[0] +
			d11[2] * d00[0] * d00[1]
			) * lapWeight;
#undef D_DOT
}
template< >
template< class I >
double FEMSystemFunctor< 1 , BOUNDARY_DIRICHLET >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 ) , d11[] = D_DOT( 1 , 1 );
	return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight 
		+
		(
			d11[0] * d00[1] * d00[2] +
			d11[1] * d00[2] * d00[0] +
			d11[2] * d00[0] * d00[1]
			) * lapWeight;
#undef D_DOT
}

template< int FEMDegree , BoundaryType BType >
template< class I >
double FEMSystemFunctor< FEMDegree , BType >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , D1 , D2 ) , integrator.dot( off1[1] , off2[1] , D1 , D2 ) , integrator.dot( off1[2] , off2[2] , D1 , D2 ) }
	double d00[] = D_DOT( 0 , 0 ) , d02[] = D_DOT( 0 , 2 ) , d20[] = D_DOT( 2 , 0 ) , d22[] = D_DOT( 2 , 2 ) , d11[] = D_DOT( 1 , 1 );
	return
		(
		d00[0] * d00[1] * d00[2]
		) * massWeight 
		+
		(
		d11[0] * d00[1] * d00[2] +
		d11[1] * d00[2] * d00[0] +
		d11[2] * d00[0] * d00[1]
		) * lapWeight
		+
		(
		d22[0] * d00[1] * d00[2] +							// Unmixed
		d22[1] * d00[2] * d00[0] +							// Unmixed
		d22[2] * d00[0] * d00[1] +							// Unmixed
		d00[0] * ( d02[1] * d20[2] + d20[1] * d02[2] ) +	//   Mixed
		d00[1] * ( d02[2] * d20[0] + d20[2] * d02[0] ) +	//   Mixed
		d00[2] * ( d02[0] * d20[1] + d20[0] * d02[1] )		//   Mixed
		) * biLapWeight;
#undef D_DOT
}
template< int SFDegree , BoundaryType SFBType , int FEMDegree , BoundaryType FEMBType >
template< bool Reverse , class I >
double FEMSFConstraintFunctor< SFDegree , SFBType , FEMDegree , FEMBType >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , Reverse ? D2 : D1 , Reverse ? D1 : D2 ) , integrator.dot( off1[1] , off2[1] , Reverse ? D2 : D1 , Reverse ? D1 : D2 ) , integrator.dot( off1[2] , off2[2] , Reverse ? D2 : D1 , Reverse ? D1 : D2 ) }
	double d00[] = D_DOT( 0 , 0 ) , d02[] = D_DOT( 0 , 2 ) , d20[] = D_DOT( 2 , 0 ) , d22[] = D_DOT( 2 , 2 ) , d11[] = D_DOT( 1 , 1 );
	if( SFDegree==0 || FEMDegree==0 )
		return d00[0] * d00[1] * d00[2] * massWeight;
	else if( SFDegree<=1 || FEMDegree<=1 ) 
		return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight 
		+
		(
			d11[0] * d00[1] * d00[2] +
			d11[1] * d00[2] * d00[0] +
			d11[2] * d00[0] * d00[1]
			) * lapWeight;
	else
		return
		(
			d00[0] * d00[1] * d00[2]
			) * massWeight 
		+
		(
			d11[0] * d00[1] * d00[2] +
			d11[1] * d00[2] * d00[0] +
			d11[2] * d00[0] * d00[1]
			) * lapWeight
		+
		(
			d22[0] * d00[1] * d00[2] +							// Unmixed
			d22[1] * d00[2] * d00[0] +							// Unmixed
			d22[2] * d00[0] * d00[1] +							// Unmixed
			d00[0] * ( d02[1] * d20[2] + d20[1] * d02[2] ) +	//   Mixed
			d00[1] * ( d02[2] * d20[0] + d20[2] * d02[0] ) +	//   Mixed
			d00[2] * ( d02[0] * d20[1] + d20[0] * d02[1] )		//   Mixed
			) * biLapWeight;
#undef D_DOT
}
template< int VFDegree , BoundaryType VFBType , int FEMDegree , BoundaryType FEMBType >
template< bool Reverse , class I >
Point3D< double > FEMVFConstraintFunctor< VFDegree , VFBType , FEMDegree , FEMBType >::_integrate( const I& integrator , const int off1[] , const int off2[] ) const
{
#define D_DOT( D1 , D2 ) { integrator.dot( off1[0] , off2[0] , Reverse ? D2 : D1 , Reverse ? D1 : D2 ) , integrator.dot( off1[1] , off2[1] , Reverse ? D2 : D1 , Reverse ? D1 : D2 ) , integrator.dot( off1[2] , off2[2] , Reverse ? D2 : D1 , Reverse ? D1 : D2 ) }
	if( FEMDegree==0 ) fprintf( stderr , "[ERROR] FEMDegree does not support differentiation: %d\n" , FEMDegree  ) , exit( 0 );
	if( VFDegree==0 || FEMDegree==1 )
	{
		double d00[] = D_DOT( 0 , 0 ) , d01[] = D_DOT( 0 , 1 );
		return
			Point3D< double >
			(
				d01[0] * d00[1] * d00[2] ,
				d01[1] * d00[2] * d00[0] ,
				d01[2] * d00[0] * d00[1]
			) * lapWeight;
	}
	else
	{
		double d00[] = D_DOT( 0 , 0 ) , d10[] = D_DOT( 1 , 0 ) , d01[] = D_DOT( 0 , 1 ) , d02[] = D_DOT( 0 , 2 ) , d12[] = D_DOT( 1 , 2 );
		return
			Point3D< double >
			(
				d01[0] * d00[1] * d00[2] ,
				d01[1] * d00[2] * d00[0] ,
				d01[2] * d00[0] * d00[1]
			) * lapWeight
			+
			Point3D< double >
			(
				d12[0] * d00[1] * d00[2] + d10[0] * ( d00[1] * d02[2] + d02[1] * d00[2] ) , 
				d12[1] * d00[2] * d00[0] + d10[1] * ( d00[2] * d02[0] + d02[2] * d00[0] ) , 
				d12[2] * d00[0] * d00[1] + d10[2] * ( d00[0] * d02[1] + d02[0] * d00[1] )
			) * biLapWeight;
	}
#undef D_DOT
}

template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
template< bool Reverse , class _FEMSystemFunctor >
void SystemCoefficients< Degree1 , BType1 , Degree2 , BType2 >::SetCentralConstraintStencil( const _FEMSystemFunctor& F , const Integrator& integrator , Stencil< double , OverlapSize >& stencil  )
{
	int center = ( 1<<integrator.depth() )>>1;
	int offset[] = { center , center , center };
	for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
	{
		int _offset[] = { x+center-OverlapEnd , y+center-OverlapEnd , z+center-OverlapEnd };
		stencil( x , y , z ) = F.template integrate< Reverse >( integrator , _offset , offset );
	}
}
template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
template< bool Reverse , class _FEMSystemFunctor >
void SystemCoefficients< Degree1 , BType1 , Degree2 , BType2 >::SetCentralConstraintStencils( const _FEMSystemFunctor& F , const ChildIntegrator& integrator , Stencil< double , OverlapSize > stencils[2][2][2] )
{
	int center = ( 1<<integrator.childDepth() )>>1;
	// [NOTE] We want the center to be at the first node of the brood
	// Which is not the case when childDepth is 1.
	center = ( center>>1 )<<1;
	for( int i=0 ; i<2 ; i++ ) for( int j=0 ; j<2 ; j++ ) for( int k=0 ; k<2 ; k++ )
	{
		int offset[] = { center+i , center+j , center+k };
		for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
		{
			int _offset[] = { x+center/2-OverlapEnd , y+center/2-OverlapEnd , z+center/2-OverlapEnd };
			stencils[i][j][k]( x , y , z ) = F.template integrate< Reverse >( integrator , _offset , offset );
		}
	}
}
template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
template< bool Reverse , class _FEMSystemFunctor >
void SystemCoefficients< Degree1 , BType1 , Degree2 , BType2 >::SetCentralConstraintStencil( const _FEMSystemFunctor& F , const Integrator& integrator , Stencil< Point3D< double > , OverlapSize >& stencil  )
{
	int center = ( 1<<integrator.depth() )>>1;
	int offset[] = { center , center , center };
	for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
	{
		int _offset[] = { x+center-OverlapEnd , y+center-OverlapEnd , z+center-OverlapEnd };
		stencil( x , y , z ) = F.template integrate< Reverse >( integrator , _offset , offset );
	}
}
template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
template< bool Reverse , class _FEMSystemFunctor >
void SystemCoefficients< Degree1 , BType1 , Degree2 , BType2 >::SetCentralConstraintStencils( const _FEMSystemFunctor& F , const ChildIntegrator& integrator , Stencil< Point3D< double > , OverlapSize > stencils[2][2][2] )
{
	int center = ( 1<<integrator.childDepth() )>>1;
	// [NOTE] We want the center to be at the first node of the brood
	// Which is not the case when childDepth is 1.
	center = ( center>>1 )<<1;
	for( int i=0 ; i<2 ; i++ ) for( int j=0 ; j<2 ; j++ ) for( int k=0 ; k<2 ; k++ )
	{
		int offset[] = { center+i , center+j , center+k };
		for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
		{
			int _offset[] = { x+center/2-OverlapEnd , y+center/2-OverlapEnd , z+center/2-OverlapEnd };
			stencils[i][j][k]( x , y , z ) = F.template integrate< Reverse >( integrator , _offset , offset );
		}
	}
}
template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
template< class _FEMSystemFunctor >
void SystemCoefficients< Degree1 , BType1 , Degree2 , BType2 >::SetCentralSystemStencil( const _FEMSystemFunctor& F , const Integrator& integrator , Stencil< double , OverlapSize >& stencil )
{
	int center = ( 1<<integrator.depth() )>>1;
	int offset[] = { center , center , center };
	for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
	{
		int _offset[] = { x+center-OverlapEnd , y+center-OverlapEnd , z+center-OverlapEnd };
		stencil( x , y , z ) = F.integrate( integrator , _offset , offset );
	}
}
template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
template< class _FEMSystemFunctor >
void SystemCoefficients< Degree1 , BType1 , Degree2 , BType2 >::SetCentralSystemStencils( const _FEMSystemFunctor& F , const ChildIntegrator& integrator , Stencil< double , OverlapSize > stencils[2][2][2] )
{
	int center = ( 1<<integrator.childDepth() )>>1;
	// [NOTE] We want the center to be at the first node of the brood
	// Which is not the case when childDepth is 1.
	center = ( center>>1 )<<1;
	for( int i=0 ; i<2 ; i++ ) for( int j=0 ; j<2 ; j++ ) for( int k=0 ; k<2 ; k++ )
	{
		int offset[] = { center+i , center+j , center+k };
		for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
		{
			int _offset[] = { x+center/2-OverlapEnd , y+center/2-OverlapEnd , z+center/2-OverlapEnd };
			stencils[i][j][k]( x , y , z ) = F.integrate( integrator , _offset , offset );
		}
	}
}

template< class Real >
template< int FEMDegree >
void Octree< Real >::_setMultiColorIndices( int start , int end , std::vector< std::vector< int > >& indices ) const
{
	static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;

	const int modulus = OverlapRadius+1;
	indices.resize( modulus*modulus*modulus );
	int count[modulus*modulus*modulus];
	memset( count , 0 , sizeof(int)*modulus*modulus*modulus );
#pragma omp parallel for num_threads( threads )
	for( int i=start ; i<end ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		// [NOTE] We have to use the global offset so that it's positive
		int d , off[3];
		_sNodes.treeNodes[i]->depthAndOffset( d , off );
		int idx = (modulus*modulus) * ( off[2]%modulus ) + modulus * ( off[1]%modulus ) + ( off[0]%modulus );
#pragma omp atomic
		count[idx]++;
	}

	for( int i=0 ; i<modulus*modulus*modulus ; i++ ) indices[i].reserve( count[i] ) , count[i]=0;
	for( int i=start ; i<end ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		int d , off[3];
		_sNodes.treeNodes[i]->depthAndOffset( d , off );
		int idx = (modulus*modulus) * ( off[2]%modulus ) + modulus * ( off[1]%modulus ) + ( off[0]%modulus );
		indices[idx].push_back( i - start );
	}
}

template< class Real >
template< class C , int FEMDegree , BoundaryType BType >
void Octree< Real >::_downSample( LocalDepth highDepth , DenseNodeData< C , FEMDegree >& constraints ) const
{
	typedef typename TreeOctNode::NeighborKey< -BSplineSupportSizes< FEMDegree >::UpSampleStart , BSplineSupportSizes< FEMDegree >::UpSampleEnd > UpSampleKey;

	LocalDepth lowDepth = highDepth-1;
	if( lowDepth<0 ) return;

	typename BSplineEvaluationData< FEMDegree , BType >::UpSampleEvaluator upSampleEvaluator;
	BSplineEvaluationData< FEMDegree , BType >::SetUpSampleEvaluator( upSampleEvaluator , lowDepth );
	std::vector< UpSampleKey > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( lowDepth ) );

	Stencil< double , BSplineSupportSizes< FEMDegree >::UpSampleSize > upSampleStencil;
	int lowCenter = ( 1<<lowDepth )>>1;
	for( int i=0 ; i<BSplineSupportSizes< FEMDegree >::UpSampleSize ; i++ ) for( int j=0 ; j<BSplineSupportSizes< FEMDegree >::UpSampleSize ; j++ ) for( int k=0 ; k<BSplineSupportSizes< FEMDegree >::UpSampleSize ; k++ )
		upSampleStencil( i , j , k ) =
		upSampleEvaluator.value( lowCenter , 2*lowCenter + i + BSplineSupportSizes< FEMDegree >::UpSampleStart ) *
		upSampleEvaluator.value( lowCenter , 2*lowCenter + j + BSplineSupportSizes< FEMDegree >::UpSampleStart ) *
		upSampleEvaluator.value( lowCenter , 2*lowCenter + k + BSplineSupportSizes< FEMDegree >::UpSampleStart );

	// Iterate over all (valid) parent nodes
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(lowDepth) ; i<_sNodesEnd(lowDepth) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		TreeOctNode* pNode = _sNodes.treeNodes[i];

		UpSampleKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( pNode , d , off );

		neighborKey.template getNeighbors< false >( pNode );

		// Get the child neighbors
		typename TreeOctNode::Neighbors< BSplineSupportSizes< FEMDegree >::UpSampleSize > neighbors;
		neighborKey.template getChildNeighbors< false >( 0 , _localToGlobal( d ) , neighbors );

		C& coarseConstraint = constraints[i];

		// Want to make sure test if contained children are interior.
		// This is more conservative because we are test that overlapping children are interior
		bool isInterior = _isInteriorlyOverlapped< FEMDegree , FEMDegree >( pNode );
		if( isInterior )
		{
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::UpSampleSize ; ii++ ) for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::UpSampleSize ; jj++ ) for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::UpSampleSize ; kk++ )
			{
				const TreeOctNode* cNode = neighbors.neighbors[ii][jj][kk];
				if( IsActiveNode( cNode ) ) coarseConstraint += (C)( constraints[ cNode->nodeData.nodeIndex ] * upSampleStencil( ii , jj , kk ) );
			}
		}
		else
		{
			double upSampleValues[3][ BSplineSupportSizes< FEMDegree >::UpSampleSize ];
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::UpSampleSize ; ii++ )
			{
				upSampleValues[0][ii] = upSampleEvaluator.value( off[0] , 2*off[0] + ii + BSplineSupportSizes< FEMDegree >::UpSampleStart );
				upSampleValues[1][ii] = upSampleEvaluator.value( off[1] , 2*off[1] + ii + BSplineSupportSizes< FEMDegree >::UpSampleStart );
				upSampleValues[2][ii] = upSampleEvaluator.value( off[2] , 2*off[2] + ii + BSplineSupportSizes< FEMDegree >::UpSampleStart );
			}
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::UpSampleSize ; ii++ ) for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::UpSampleSize ; jj++ )
			{
				double dxy = upSampleValues[0][ii] * upSampleValues[1][jj];
				for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::UpSampleSize ; kk++ )
				{
					const TreeOctNode* cNode = neighbors.neighbors[ii][jj][kk];
					if( _isValidFEMNode( cNode ) ) coarseConstraint += (C)( constraints[ cNode->nodeData.nodeIndex ] * dxy * upSampleValues[2][kk] );
				}
			}
		}
	}
}
template< class Real >
template< class C , int FEMDegree , BoundaryType BType >
void Octree< Real >::_upSample( LocalDepth highDepth , DenseNodeData< C , FEMDegree >& coefficients ) const
{
	static const int  LeftDownSampleRadius = -( ( BSplineSupportSizes< FEMDegree >::DownSample0Start < BSplineSupportSizes< FEMDegree >::DownSample1Start ) ? BSplineSupportSizes< FEMDegree >::DownSample0Start : BSplineSupportSizes< FEMDegree >::DownSample1Start );
	static const int RightDownSampleRadius =  ( ( BSplineSupportSizes< FEMDegree >::DownSample0End   > BSplineSupportSizes< FEMDegree >::DownSample1End   ) ? BSplineSupportSizes< FEMDegree >::DownSample0End   : BSplineSupportSizes< FEMDegree >::DownSample1End   );
	typedef TreeOctNode::NeighborKey< LeftDownSampleRadius , RightDownSampleRadius > DownSampleKey;

	LocalDepth lowDepth = highDepth-1;
	if( lowDepth<0 ) return;

	typename BSplineEvaluationData< FEMDegree , BType >::UpSampleEvaluator upSampleEvaluator;
	BSplineEvaluationData< FEMDegree , BType >::SetUpSampleEvaluator( upSampleEvaluator , lowDepth );
	std::vector< DownSampleKey > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( lowDepth ) );
	
	static const int DownSampleSize = BSplineSupportSizes< FEMDegree >::DownSample0Size > BSplineSupportSizes< FEMDegree >::DownSample1Size ? BSplineSupportSizes< FEMDegree >::DownSample0Size : BSplineSupportSizes< FEMDegree >::DownSample1Size;
	Stencil< double , DownSampleSize > downSampleStencils[ Cube::CORNERS ];
	int lowCenter = ( 1<<lowDepth )>>1;
	for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
	{
		int cx , cy , cz;
		Cube::FactorCornerIndex( c , cx , cy , cz );
		for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ )
			for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; jj++ )
				for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; kk++ )
					downSampleStencils[c]( ii , jj , kk ) = 
					upSampleEvaluator.value( lowCenter + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] , 2*lowCenter + cx ) *
					upSampleEvaluator.value( lowCenter + jj + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] , 2*lowCenter + cy ) *
					upSampleEvaluator.value( lowCenter + kk + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] , 2*lowCenter + cz ) ;
	}

	// For Dirichlet constraints, can't get to all children from parents because boundary nodes are invalid
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(highDepth) ; i<_sNodesEnd(highDepth) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		TreeOctNode *cNode = _sNodes.treeNodes[i] , *pNode = cNode->parent;
		int c = (int)( cNode-pNode->children );

		DownSampleKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( pNode , d , off );
		typename TreeOctNode::Neighbors< LeftDownSampleRadius + RightDownSampleRadius + 1 >& neighbors = neighborKey.template getNeighbors< false >( pNode );

		// Want to make sure test if contained children are interior.
		// This is more conservative because we are test that overlapping children are interior
		bool isInterior = _isInteriorlyOverlapped< FEMDegree , FEMDegree >( pNode );

		C& fineCoefficient = coefficients[ cNode->nodeData.nodeIndex ];

		int cx , cy , cz;
		Cube::FactorCornerIndex( c , cx , cy , cz );

		if( isInterior )
		{
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ ) for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; jj++ )
			{
				int _ii = ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] + LeftDownSampleRadius;
				int _jj = jj + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] + LeftDownSampleRadius;
				for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; kk++ )
				{
					int _kk = kk + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] + LeftDownSampleRadius;
					const TreeOctNode* _pNode = neighbors.neighbors[_ii][_jj][_kk];
					if( _pNode ) fineCoefficient += (C)( coefficients[ _pNode->nodeData.nodeIndex ] * downSampleStencils[c]( ii , jj , kk ) );
				}
			}
		}
		else
		{
			double downSampleValues[3][ BSplineSupportSizes< FEMDegree >::DownSample0Size > BSplineSupportSizes< FEMDegree >::DownSample1Size ? BSplineSupportSizes< FEMDegree >::DownSample0Size : BSplineSupportSizes< FEMDegree >::DownSample1Size ];
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ ) downSampleValues[0][ii] = upSampleEvaluator.value( off[0] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] , 2*off[0] + cx );
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; ii++ ) downSampleValues[1][ii] = upSampleEvaluator.value( off[1] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] , 2*off[1] + cy );
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; ii++ ) downSampleValues[2][ii] = upSampleEvaluator.value( off[2] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] , 2*off[2] + cz );

			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ ) for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; jj++ )
			{
				double dxy = downSampleValues[0][ii] * downSampleValues[1][jj];
				int _ii = ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] + LeftDownSampleRadius;
				int _jj = jj + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] + LeftDownSampleRadius;
				for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; kk++ )
				{
					int _kk = kk + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] + LeftDownSampleRadius;
					const TreeOctNode* _pNode = neighbors.neighbors[_ii][_jj][_kk];
					if( _isValidFEMNode( _pNode ) ) fineCoefficient += (C)( coefficients[ _pNode->nodeData.nodeIndex ] * dxy * downSampleValues[2][kk] );
				}
			}
		}
	}
}

template< class Real >
template< class C , int FEMDegree , BoundaryType BType >
void Octree< Real >::_UpSample( LocalDepth highDepth , ConstPointer( C ) lowCoefficients , Pointer( C ) highCoefficients , int threads )
{
	static const int  LeftDownSampleRadius = -( ( BSplineSupportSizes< FEMDegree >::DownSample0Start < BSplineSupportSizes< FEMDegree >::DownSample1Start ) ? BSplineSupportSizes< FEMDegree >::DownSample0Start : BSplineSupportSizes< FEMDegree >::DownSample1Start );
	static const int RightDownSampleRadius =  ( ( BSplineSupportSizes< FEMDegree >::DownSample0End   > BSplineSupportSizes< FEMDegree >::DownSample1End   ) ? BSplineSupportSizes< FEMDegree >::DownSample0End   : BSplineSupportSizes< FEMDegree >::DownSample1End   );
	typedef TreeOctNode::NeighborKey< LeftDownSampleRadius , RightDownSampleRadius > DownSampleKey;

	LocalDepth lowDepth = highDepth - 1;
	if( lowDepth<0 ) return;

	typename BSplineEvaluationData< FEMDegree , BType >::UpSampleEvaluator upSampleEvaluator;
	BSplineEvaluationData< FEMDegree , BType >::SetUpSampleEvaluator( upSampleEvaluator , lowDepth );
	std::vector< DownSampleKey > neighborKeys( std::max< int >( 1 , threads ) );

	static const int DownSampleSize = BSplineSupportSizes< FEMDegree >::DownSample0Size > BSplineSupportSizes< FEMDegree >::DownSample1Size ? BSplineSupportSizes< FEMDegree >::DownSample0Size : BSplineSupportSizes< FEMDegree >::DownSample1Size;
	Stencil< double , DownSampleSize > downSampleStencils[ Cube::CORNERS ];
	int lowCenter = ( 1<<lowDepth )>>1;
	for( int c=0 ; c<Cube::CORNERS ; c++ )
	{
		int cx , cy , cz;
		Cube::FactorCornerIndex( c , cx , cy , cz );
		static const int DownSampleSize = BSplineSupportSizes< FEMDegree >::DownSample0Size > BSplineSupportSizes< FEMDegree >::DownSample1Size ? BSplineSupportSizes< FEMDegree >::DownSample0Size : BSplineSupportSizes< FEMDegree >::DownSample1Size;
		for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ )
			for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; jj++ )
				for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; kk++ )
					downSampleStencils[c]( ii , jj , kk ) = 
					upSampleEvaluator.value( lowCenter + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] , 2*lowCenter + cx ) *
					upSampleEvaluator.value( lowCenter + jj + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] , 2*lowCenter + cy ) *
					upSampleEvaluator.value( lowCenter + kk + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] , 2*lowCenter + cz ) ;
	}
	int  lowBegin = _BSplineBegin< FEMDegree , BType >(  lowDepth ) ,  lowEnd = _BSplineEnd< FEMDegree , BType >(  lowDepth );
	int highBegin = _BSplineBegin< FEMDegree , BType >( highDepth ) , highEnd = _BSplineEnd< FEMDegree , BType >( highDepth );
	int lowDim = lowEnd - lowBegin , highDim = highEnd - highBegin;
	// Iterate over all child nodes. (This is required since there can be child nodes whose parent is inactive.)
#pragma omp parallel for num_threads( threads )
	for( int k=0 ; k<highDim ; k++ ) for( int j=0 ; j<highDim ; j++ ) for( int i=0 ; i<highDim ; i++ )
	{
		DownSampleKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
		LocalOffset off , _off;
		off[0] = i + highBegin , off[1] = j + highBegin , off[2] = k + highBegin;
		int highIdx = i + j * highDim  + k * highDim * highDim;
		_off[0] = off[0]>>1 , _off[1] = off[1]>>1 , _off[2] = off[2]>>1;

		// Want to make sure test if contained children are interior.
		// This is more conservative because we are test that overlapping children are interior
		bool isInterior = _IsInteriorlyOverlapped< FEMDegree , FEMDegree >( lowDepth , _off );
		int cx = off[0]&1 , cy = off[1]&1 , cz = off[2]&1;
		int c = Cube::CornerIndex( cx , cy , cz );

		C& highCoefficient = highCoefficients[ highIdx ];

		if( isInterior )
		{
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ ) for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; jj++ )
			{
				int _i = _off[0] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] - lowBegin;
				int _j = _off[1] + jj + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] - lowBegin;
				for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; kk++ )
				{
					int _k = _off[2] + kk + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] - lowBegin;
					highCoefficient += (C)( lowCoefficients[ _i + _j*lowDim  + _k*lowDim*lowDim ] * downSampleStencils[c]( ii , jj , kk ) );
				}
			}
		}
		else
		{
			double downSampleValues[3][ BSplineSupportSizes< FEMDegree >::DownSample0Size > BSplineSupportSizes< FEMDegree >::DownSample1Size ? BSplineSupportSizes< FEMDegree >::DownSample0Size : BSplineSupportSizes< FEMDegree >::DownSample1Size ];

			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ ) downSampleValues[0][ii] = upSampleEvaluator.value( _off[0] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] , off[0] );
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; ii++ ) downSampleValues[1][ii] = upSampleEvaluator.value( _off[1] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] , off[1] );
			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; ii++ ) downSampleValues[2][ii] = upSampleEvaluator.value( _off[2] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] , off[2] );

			for( int ii=0 ; ii<BSplineSupportSizes< FEMDegree >::DownSampleSize[cx] ; ii++ ) for( int jj=0 ; jj<BSplineSupportSizes< FEMDegree >::DownSampleSize[cy] ; jj++ )
			{
				double dxy = downSampleValues[0][ii] * downSampleValues[1][jj];
				int _i = _off[0] + ii + BSplineSupportSizes< FEMDegree >::DownSampleStart[cx] - lowBegin;
				int _j = _off[1] + jj + BSplineSupportSizes< FEMDegree >::DownSampleStart[cy] - lowBegin;
				if( _i>=0 && _i<lowDim && _j>=0 && _j<lowDim )
					for( int kk=0 ; kk<BSplineSupportSizes< FEMDegree >::DownSampleSize[cz] ; kk++ )
					{
						int _k = _off[2] + kk + BSplineSupportSizes< FEMDegree >::DownSampleStart[cz] - lowBegin;
						if( _k>=0 && _k<lowDim ) highCoefficient += (C)( lowCoefficients[ _i + _j*lowDim  + _k*lowDim*lowDim ] * dxy * downSampleValues[2][kk] );
					}
			}
		}
	}
}

template< class Real >
template< class C , int FEMDegree , BoundaryType BType >
DenseNodeData< C , FEMDegree > Octree< Real >::coarseCoefficients( const DenseNodeData< C , FEMDegree >& coefficients ) const
{
	DenseNodeData< Real , FEMDegree > coarseCoefficients( _sNodesEnd(_maxDepth-1) );
	memset( &coarseCoefficients[0] , 0 , sizeof(Real)*_sNodesEnd(_maxDepth-1) );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(0) ; i<_sNodesEnd(_maxDepth-1) ; i++ ) coarseCoefficients[i] = coefficients[i];
	for( LocalDepth d=1 ; d<_maxDepth ; d++ ) _upSample< C , FEMDegree , BType >( d , coarseCoefficients );
	return coarseCoefficients;
}
template< class Real >
template< class C , int FEMDegree , BoundaryType BType >
DenseNodeData< C , FEMDegree > Octree< Real >::coarseCoefficients( const SparseNodeData< C , FEMDegree >& coefficients ) const
{
	DenseNodeData< Real , FEMDegree > coarseCoefficients( _sNodesEnd(_maxDepth-1) );
	memset( &coarseCoefficients[0] , 0 , sizeof(Real)*_sNodesEnd(_maxDepth-1) );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(0) ; i<_sNodesEnd(_maxDepth-1) ; i++ )
	{
		const C* c = coefficients( _sNodes.treeNodes[i] );
		if( c ) coarseCoefficients[i] = *c;
	}
	for( LocalDepth d=1 ; d<_maxDepth ; d++ ) _upSample< C , FEMDegree , BType >( d , coarseCoefficients );
	return coarseCoefficients;
}

template< class Real >
template< int FEMDegree , BoundaryType BType >
Real Octree< Real >::_coarserFunctionValue( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* pointNode , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& upSampledCoefficients ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;
	//static const int RightSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int  LeftPointSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;

	double pointValue = 0;
	LocalDepth depth = _localDepth( pointNode );
	if( depth<0 ) return (Real)0.;

	// Iterate over all basis functions that overlap the point at the coarser resolution
	{
		const typename TreeOctNode::Neighbors< SupportSize >& neighbors = neighborKey.neighbors[ _localToGlobal( depth-1 ) ];
		LocalDepth _d ; LocalOffset _off;
		_localDepthAndOffset( pointNode->parent , _d , _off );
		int fStart , fEnd;
		BSplineData< FEMDegree , BType >::FunctionSpan( _d , fStart , fEnd );

		double pointValues[ DIMENSION ][SupportSize];
		memset( pointValues , 0 , sizeof(double) * DIMENSION * SupportSize );

		for( int dd=0 ; dd<DIMENSION ; dd++ ) for( int i=-LeftPointSupportRadius ; i<=RightPointSupportRadius ; i++ )
		{
			int fIdx = BSplineData< FEMDegree , BType >::FunctionIndex( _d , _off[dd]+i );
			if( fIdx>=fStart && fIdx<fEnd ) pointValues[dd][i+LeftPointSupportRadius] = bsData.baseBSplines[ fIdx ][LeftSupportRadius-i]( p[dd] );
		}

		for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			double xyValue = pointValues[0][j] * pointValues[1][k];
			double _pointValue = 0;
			for( int l=0 ; l<SupportSize ; l++ )
			{
				const TreeOctNode* _node = neighbors.neighbors[j][k][l];
				if( _isValidFEMNode( _node ) ) _pointValue += pointValues[2][l] * double( upSampledCoefficients[_node->nodeData.nodeIndex] );
			}
			pointValue += _pointValue * xyValue;
		}
	}
	return Real( pointValue );
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
Point3D< Real > Octree< Real >::_coarserFunctionGradient( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* pointNode , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& upSampledCoefficients ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;
	//static const int RightSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int  LeftPointSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;

	Point3D< double > pointGradient;
	LocalDepth depth = _localDepth( pointNode );
	if( depth<=0 ) return Real(0.);

	// Iterate over all basis functions that overlap the point at the coarser resolution
	{
		const typename TreeOctNode::Neighbors< SupportSize >& neighbors = neighborKey.neighbors[ _localToGlobal( depth-1 ) ];
		LocalDepth _d ; LocalOffset _off;
		_localDepthAndOffset( pointNode->parent , _d , _off );
		int fStart , fEnd;
		BSplineData< FEMDegree , BType >::FunctionSpan( _d , fStart , fEnd );

		double _pointValues[ DIMENSION ][SupportSize] , dPointValues[ DIMENSION ][SupportSize];
		memset( _pointValues , 0 , sizeof(double) * DIMENSION * SupportSize );
		memset( dPointValues , 0 , sizeof(double) * DIMENSION * SupportSize );

		for( int dd=0 ; dd<DIMENSION ; dd++ ) for( int i=-LeftPointSupportRadius ; i<=RightPointSupportRadius ; i++ )
		{
			int fIdx = BSplineData< FEMDegree , BType >::FunctionIndex( _d , _off[dd]+i );
			if( fIdx>=fStart && fIdx<fEnd )
			{
				_pointValues[dd][i+LeftPointSupportRadius] = bsData.baseBSplines[ fIdx ][LeftSupportRadius-i]( p[dd] );
				dPointValues[dd][i+LeftPointSupportRadius] = bsData.dBaseBSplines[ fIdx ][LeftSupportRadius-i]( p[dd] );
			}
		}

		for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			double _x_yValue = _pointValues[0][j] * _pointValues[1][k];
			double dx_yValue = dPointValues[0][j] * _pointValues[1][k];
			double _xdyValue = _pointValues[0][j] * dPointValues[1][k];
			double __pointValue = 0 , _dPointValue = 0;
			for( int l=0 ; l<SupportSize ; l++ )
			{
				const TreeOctNode* _node = neighbors.neighbors[j][k][l];
				if( _isValidFEMNode( _node ) )
				{
					__pointValue += _pointValues[2][l] * double( upSampledCoefficients[_node->nodeData.nodeIndex] );
					_dPointValue += dPointValues[2][l] * double( upSampledCoefficients[_node->nodeData.nodeIndex] );
				}
			}

			pointGradient += Point3D< double >( __pointValue * dx_yValue , __pointValue * _xdyValue , _dPointValue * _x_yValue );
		}
	}
	return Point3D< Real >( pointGradient );
}

template< class Real >
template< int FEMDegree , BoundaryType BType >
Real Octree< Real >::_finerFunctionValue( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* pointNode , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& finerCoefficients ) const
{
	typename TreeOctNode::Neighbors< BSplineSupportSizes< FEMDegree >::SupportSize > childNeighbors;
	static const int  LeftPointSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	static const int  LeftSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	//static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;

	double pointValue = 0;
	LocalDepth depth = _localDepth( pointNode );
	neighborKey.template getChildNeighbors< false >( _childIndex( pointNode , p ) , _localToGlobal( depth ) , childNeighbors );
	for( int j=-LeftPointSupportRadius ; j<=RightPointSupportRadius ; j++ )
		for( int k=-LeftPointSupportRadius ; k<=RightPointSupportRadius ; k++ )
			for( int l=-LeftPointSupportRadius ; l<=RightPointSupportRadius ; l++ )
			{
				const TreeOctNode* _node = childNeighbors.neighbors[j+LeftPointSupportRadius][k+LeftPointSupportRadius][l+LeftPointSupportRadius];
				if( _isValidFEMNode( _node ) )
				{
					int fIdx[3];
					functionIndex< FEMDegree , BType >( _node , fIdx );
					pointValue += 
						bsData.baseBSplines[ fIdx[0] ][LeftSupportRadius-j]( p[0] ) *
						bsData.baseBSplines[ fIdx[1] ][LeftSupportRadius-k]( p[1] ) *
						bsData.baseBSplines[ fIdx[2] ][LeftSupportRadius-l]( p[2] ) *
						double( finerCoefficients[ _node->nodeData.nodeIndex ] );
				}
			}
	return Real( pointValue );
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
Point3D< Real > Octree< Real >::_finerFunctionGradient( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* pointNode , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& finerCoefficients ) const
{
	typename TreeOctNode::Neighbors< BSplineSupportSizes< FEMDegree >::SupportSize > childNeighbors;
	static const int  LeftPointSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	static const int  LeftSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	//static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;

	Point3D< double > pointGradient = 0;
	LocalDepth depth = _localDepth( pointNode );
	neighborKey.template getChildNeighbors< false >( _childIndex( pointNode , p ) , _localToGlobal( depth ) , childNeighbors );
	for( int j=-LeftPointSupportRadius ; j<=RightPointSupportRadius ; j++ )
		for( int k=-LeftPointSupportRadius ; k<=RightPointSupportRadius ; k++ )
			for( int l=-LeftPointSupportRadius ; l<=RightPointSupportRadius ; l++ )
			{
				const TreeOctNode* _node = childNeighbors.neighbors[j+LeftPointSupportRadius][k+LeftPointSupportRadius][l+LeftPointSupportRadius];
				if( _isValidFEMNode( _node ) )
				{
					int fIdx[3];
					functionIndex< FEMDegree , BType >( _node , fIdx );
					double  x = bsData. baseBSplines[ fIdx[0] ][LeftSupportRadius-j]( p[0] ) ,  y = bsData. baseBSplines[ fIdx[1] ][LeftSupportRadius-k]( p[1] ) ,  z = bsData. baseBSplines[ fIdx[2] ][LeftSupportRadius-l]( p[2] );
					double dx = bsData.dBaseBSplines[ fIdx[0] ][LeftSupportRadius-j]( p[0] ) , dy = bsData.dBaseBSplines[ fIdx[1] ][LeftSupportRadius-k]( p[1] ) , dz = bsData.dBaseBSplines[ fIdx[2] ][LeftSupportRadius-l]( p[2] );
					pointGradient += Point3D< double >( dx * y * z , x * dy * z , x * y * dz ) * (double)( finerCoefficients[ _node->nodeData.nodeIndex ] );
				}
			}
	return Point3D< Real >( pointGradient );
}

template< class Real >
template< int FEMDegree , BoundaryType BType , bool HasGradients >
void Octree< Real >::_setPointValuesFromCoarser( InterpolationInfo< HasGradients >& interpolationInfo , LocalDepth highDepth , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& upSampledCoefficients )
{
	LocalDepth lowDepth = highDepth-1;
	if( lowDepth<0 ) return;
	std::vector< PointSupportKey< FEMDegree > > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( lowDepth ) );

#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(highDepth) ; i<_sNodesEnd(highDepth) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		PointSupportKey< FEMDegree >& neighborKey = neighborKeys[ omp_get_thread_num() ];
		PointData< Real , HasGradients >* pData = interpolationInfo( _sNodes.treeNodes[i] );
		if( pData )
		{
			neighborKey.template getNeighbors< false >( _sNodes.treeNodes[i]->parent );
#if POINT_DATA_RES
			for( int c=0 ; c<PointData< Real , HasGradients >::SAMPLES ; c++ ) if( (*pData)[c].weight )
				_ConstraintCalculator_< Real , FEMDegree , HasGradients >::_CalculateCoarser_
				(
					c , *pData ,
					_coarserFunctionValue( (*pData)[c].position , neighborKey , _sNodes.treeNodes[i] , bsData , upSampledCoefficients ) ,
					HasGradients ? _coarserFunctionGradient( (*pData)[c].position , neighborKey , _sNodes.treeNodes[i] , bsData , upSampledCoefficients ) : Point3D< Real >() ,
					interpolationInfo.valueWeight , interpolationInfo.gradientWeight 
				);
#else // !POINT_DATA_RES
			_ConstraintCalculator_< Real , FEMDegree , HasGradients >::_CalculateCoarser_
			(
				*pData ,
				_coarserFunctionValue( pData->position , neighborKey , _sNodes.treeNodes[i] , bsData , upSampledCoefficients ) ,
				HasGradients ? _coarserFunctionGradient( pData->position , neighborKey , _sNodes.treeNodes[i] , bsData , upSampledCoefficients ) : Point3D< Real >() ,
				interpolationInfo.valueWeight , interpolationInfo.gradientWeight 
			);
#endif // POINT_DATA_RES
		}
	}
}

template< class Real >
template< int FEMDegree , BoundaryType BType , bool HasGradients >
void Octree< Real >::_updateCumulativeInterpolationConstraintsFromFiner( const InterpolationInfo< HasGradients >& interpolationInfo , const BSplineData< FEMDegree , BType >& bsData , LocalDepth highDepth , const DenseNodeData< Real , FEMDegree >& finerCoefficients , DenseNodeData< Real , FEMDegree >& coarserConstraints ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftPointSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	static const int  LeftSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	//static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;

	// Note: We can't iterate over the finer point nodes as the point weights might be
	// scaled incorrectly, due to the adaptive exponent. So instead, we will iterate
	// over the coarser nodes and evaluate the finer solution at the associated points.
	LocalDepth  lowDepth = highDepth-1;
	if( lowDepth<0 ) return;
	//size_t start = _sNodesBegin(lowDepth) , end = _sNodesEnd(lowDepth);
	std::vector< PointSupportKey< FEMDegree > > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( lowDepth ) );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(lowDepth) ; i<_sNodesEnd(lowDepth) ; i++ ) if( _isValidSpaceNode( _sNodes.treeNodes[i] ) )
	{
		PointSupportKey< FEMDegree >& neighborKey = neighborKeys[ omp_get_thread_num() ];
		const PointData< Real , HasGradients >* pData = interpolationInfo( _sNodes.treeNodes[i] );
		if( pData )
		{
			typename TreeOctNode::Neighbors< SupportSize >& neighbors = neighborKey.template getNeighbors< false >( _sNodes.treeNodes[i] );
			// evaluate the solution @( depth ) at the current point @( depth-1 )
#if POINT_DATA_RES
			for( int c=0 ; c<PointData< Real , HasGradients >::SAMPLES ; c++ ) if( (*pData)[c].weight )
#endif // POINT_DATA_RES
			{
#if POINT_DATA_RES
				Real finerPointDValue = _finerFunctionValue( (*pData)[c].position , neighborKey , _sNodes.treeNodes[i] , bsData , finerCoefficients ) * interpolationInfo.valueWeight * (*pData)[c].weight;
				Point3D< Real > finerPointDGradient = HasGradients ? _finerFunctionGradient( (*pData)[c].position , neighborKey , _sNodes.treeNodes[i] , bsData , finerCoefficients ) * interpolationInfo.gradientWeight * (*pData)[c].weight : Point3D< Real >();
				Point3D< Real > p = (*pData)[c].position;
#else // !POINT_DATA_RES
				Real finerPointDValue = _finerFunctionValue( pData->position , neighborKey , _sNodes.treeNodes[i] , bsData , finerCoefficients ) * interpolationInfo.valueWeight * pData->weight;
				Point3D< Real > finerPointDGradient = HasGradients ? _finerFunctionGradient( pData->position , neighborKey , _sNodes.treeNodes[i] , bsData , finerCoefficients ) * interpolationInfo.gradientWeight * pData->weight : Point3D< Real >();
				Point3D< Real > p = pData->position;
#endif // POINT_DATA_RES
				// Update constraints for all nodes @( depth-1 ) that overlap the point
				int idx[3];
				functionIndex< FEMDegree , BType >( _sNodes.treeNodes[i] , idx );
				for( int x=-LeftPointSupportRadius ; x<=RightPointSupportRadius ; x++ ) for( int y=-LeftPointSupportRadius ; y<=RightPointSupportRadius ; y++ ) for( int z=-LeftPointSupportRadius ; z<=RightPointSupportRadius ; z++ )
				{
					const TreeOctNode* _node = neighbors.neighbors[x+LeftPointSupportRadius][y+LeftPointSupportRadius][z+LeftPointSupportRadius];
					if( _isValidFEMNode( _node ) )
					{
						double px = bsData.baseBSplines[idx[0]+x][LeftSupportRadius-x]( p[0] ) , py = bsData.baseBSplines[idx[1]+y][LeftSupportRadius-y]( p[1] ) , pz = bsData.baseBSplines[idx[2]+z][LeftSupportRadius-z]( p[2] );
#pragma omp atomic
						coarserConstraints[ _node->nodeData.nodeIndex ] += (Real)( px * py * pz * finerPointDValue );
						if( HasGradients )
						{
							double dpx = bsData.dBaseBSplines[idx[0]+x][LeftSupportRadius-x]( p[0] ) , dpy = bsData.dBaseBSplines[idx[1]+y][LeftSupportRadius-y]( p[1] ) , dpz = bsData.dBaseBSplines[idx[2]+z][LeftSupportRadius-z]( p[2] );
#pragma omp atomic
							coarserConstraints[ _node->nodeData.nodeIndex ] += Point3D< Real >::Dot( finerPointDGradient , Point3D< Real >( dpx * py * pz , px * dpy * pz , px * py * dpz ) );
						}
					}
				}
			}
		}
	}
}

template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
int Octree< Real >::_setMatrixRow( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& neighbors , Pointer( MatrixEntry< Real > ) row , int offset , const typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& integrator , const Stencil< double , BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& stencil , const BSplineData< FEMDegree , BType >& bsData ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;
	static const int OverlapSize   =   BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;
	static const int LeftSupportRadius  = -BSplineSupportSizes< FEMDegree >::SupportStart;
	static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int LeftPointSupportRadius  = BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;

	bool hasYZPoints[SupportSize] , hasZPoints[SupportSize][SupportSize];
	Real diagonal = 0;
	// Given a node:
	// -- for each node in its support:
	// ---- if the supporting node contains a point:
	// ------ evaluate the x, y, and z B-splines of the nodes supporting the point
	// splineValues \in [-LeftSupportRadius,RightSupportRadius] x [-LeftSupportRadius,RightSupportRadius] x [-LeftSupportRadius,RightSupportRadius] x [0,Dimension) x [-LeftPointSupportRadius,RightPointSupportRadius]
#if POINT_DATA_RES
	Real _splineValues[PointData< Real , HasGradients >::SAMPLES][SupportSize][SupportSize][SupportSize][DIMENSION][SupportSize];
	Real wSplineValues[PointData< Real , HasGradients >::SAMPLES][SupportSize][SupportSize][SupportSize][DIMENSION][SupportSize];
	Real dSplineValues[PointData< Real , HasGradients >::SAMPLES][SupportSize][SupportSize][SupportSize][DIMENSION][SupportSize];
	memset( _splineValues , 0 , sizeof( Real ) * PointData< Real , HasGradients >::SAMPLES * SupportSize * SupportSize * SupportSize * DIMENSION *SupportSize );
	memset( wSplineValues , 0 , sizeof( Real ) * PointData< Real , HasGradients >::SAMPLES * SupportSize * SupportSize * SupportSize * DIMENSION *SupportSize );
	memset( dSplineValues , 0 , sizeof( Real ) * PointData< Real , HasGradients >::SAMPLES * SupportSize * SupportSize * SupportSize * DIMENSION *SupportSize );
#else // !POINT_DATA_RES
	Real _splineValues[SupportSize][SupportSize][SupportSize][DIMENSION][SupportSize];
	Real wSplineValues[SupportSize][SupportSize][SupportSize][DIMENSION][SupportSize];
	Real dSplineValues[SupportSize][SupportSize][SupportSize][DIMENSION][SupportSize];
	memset( _splineValues , 0 , sizeof( Real ) * SupportSize * SupportSize * SupportSize * DIMENSION *SupportSize );
	memset( wSplineValues , 0 , sizeof( Real ) * SupportSize * SupportSize * SupportSize * DIMENSION *SupportSize );
	memset( dSplineValues , 0 , sizeof( Real ) * SupportSize * SupportSize * SupportSize * DIMENSION *SupportSize );
#endif // NEW_POINT_DATA

	int count = 0;
	const TreeOctNode* node = neighbors.neighbors[OverlapRadius][OverlapRadius][OverlapRadius];
	LocalDepth d ; LocalOffset off;
	_localDepthAndOffset( node , d , off );
	int fStart , fEnd;
	BSplineData< FEMDegree , BType >::FunctionSpan( d , fStart , fEnd );
	bool isInterior = _isInteriorlyOverlapped< FEMDegree , FEMDegree >( node );

	if( interpolationInfo )
	{
		// Iterate over all neighboring nodes that may have a constraining point
		// -- For each one, compute the values of the spline functions supported on the point
		for( int j=0 ; j<SupportSize ; j++ )
		{
			hasYZPoints[j] = false;
			for( int k=0 ; k<SupportSize ; k++ ) hasZPoints[j][k] = false;
		}
		for( int j=-LeftSupportRadius , jj=0 ; j<=RightSupportRadius ; j++ , jj++ )
			for( int k=-LeftSupportRadius , kk=0 ; k<=RightSupportRadius ; k++ , kk++ )
				for( int l=-LeftSupportRadius , ll=0 ; l<=RightSupportRadius ; l++ , ll++ )
				{
					const TreeOctNode* _node = neighbors.neighbors[OverlapRadius+j][OverlapRadius+k][OverlapRadius+l];
					if( _isValidSpaceNode( _node ) && (*interpolationInfo)( _node ) )
					{
						int pOff[] = { off[0]+j , off[1]+k , off[2]+l };
						hasYZPoints[jj] = hasZPoints[jj][kk] = true;
						const PointData< Real , HasGradients >& pData = *( (*interpolationInfo)( _node ) );

#if POINT_DATA_RES
						for( int c=0 ; c<PointData< Real , HasGradients >::SAMPLES ; c++ ) if( pData[c].weight )
#endif // POINT_DATA_RES
						{
#if POINT_DATA_RES
							Real (*__splineValues)[SupportSize] = _splineValues[c][jj][kk][ll];
							Real (*_wSplineValues)[SupportSize] = wSplineValues[c][jj][kk][ll];
							Real (*_dSplineValues)[SupportSize] = dSplineValues[c][jj][kk][ll];
							Real weight = pData[c].weight;
							Point3D< Real > p = pData[c].position;
#else // !POINT_DATA_RES
							Real (*__splineValues)[SupportSize] = _splineValues[jj][kk][ll];
							Real (*_wSplineValues)[SupportSize] = wSplineValues[jj][kk][ll];
							Real (*_dSplineValues)[SupportSize] = dSplineValues[jj][kk][ll];
							Real weight = pData.weight;
							Point3D< Real > p = pData.position;
#endif // POINT_DATA_RES

							// evaluate the point p at all the nodes whose functions have it in their support
							for( int s=-LeftPointSupportRadius ; s<=RightPointSupportRadius ; s++ ) for( int dd=0 ; dd<DIMENSION ; dd++ )
							{
								int fIdx = BSplineData< FEMDegree , BType >::FunctionIndex( d , pOff[dd]+s );
								if( fIdx>=fStart && fIdx<fEnd )
								{
									_wSplineValues[dd][ s+LeftPointSupportRadius ] = __splineValues[dd][ s+LeftPointSupportRadius ] = Real( bsData.baseBSplines[ fIdx ][ -s+LeftSupportRadius ]( p[dd] ) );
									if( HasGradients ) _dSplineValues[dd][ s+LeftPointSupportRadius ] = Real( bsData.dBaseBSplines[ fIdx ][ -s+LeftSupportRadius ]( p[dd] ) );
								}
							}
							// The value of the function of the node that we started with
							Real value = __splineValues[0][-j+LeftPointSupportRadius] * __splineValues[1][-k+LeftPointSupportRadius] * __splineValues[2][-l+LeftPointSupportRadius];
							Real weightedValue = value * interpolationInfo->valueWeight * weight;
							Point3D< Real > weightedGradient;
							if( HasGradients )
							{
								Point3D< Real > gradient
									(
									_dSplineValues[0][-j+LeftPointSupportRadius] * __splineValues[1][-k+LeftPointSupportRadius] * __splineValues[2][-l+LeftPointSupportRadius] ,
									__splineValues[0][-j+LeftPointSupportRadius] * _dSplineValues[1][-k+LeftPointSupportRadius] * __splineValues[2][-l+LeftPointSupportRadius] ,
									__splineValues[0][-j+LeftPointSupportRadius] * __splineValues[1][-k+LeftPointSupportRadius] * _dSplineValues[2][-l+LeftPointSupportRadius]
									);
								weightedGradient = gradient * interpolationInfo->gradientWeight * weight;
								diagonal += value * weightedValue + Point3D< Real >::Dot( gradient , weightedGradient );
							}
							else diagonal += value * weightedValue;

							// Pre-multiply the x-coordinate values so that when we evaluate at one of the neighboring basis functions
							// we get the product of the values of the center base function and the base function of the neighboring node
							if( HasGradients ) for( int s=0 ; s<SupportSize ; s++ ) _wSplineValues[0][s] *= weightedValue , _dSplineValues[0][s] *= weightedGradient[0] , _dSplineValues[1][s] *= weightedGradient[1] , _dSplineValues[2][s] *= weightedGradient[2];
							else               for( int s=0 ; s<SupportSize ; s++ ) _wSplineValues[0][s] *= weightedValue;
						}
					}
				}
	}

	Real pointValues[OverlapSize][OverlapSize][OverlapSize];
	if( interpolationInfo )
	{
		memset( pointValues , 0 , sizeof(Real) * OverlapSize * OverlapSize * OverlapSize );
		// Iterate over all supported neighbors that could have a point constraint	
		for( int i=-LeftSupportRadius ; i<=RightSupportRadius ; i++ ) if( hasYZPoints[i+LeftSupportRadius] )
			for( int j=-LeftSupportRadius ; j<=RightSupportRadius ; j++ ) if( hasZPoints[i+LeftSupportRadius][j+LeftSupportRadius] )
				for( int k=-LeftSupportRadius ; k<=RightSupportRadius ; k++ )
				{
					const TreeOctNode* _node = neighbors.neighbors[i+OverlapRadius][j+OverlapRadius][k+OverlapRadius];
					if( _isValidSpaceNode( _node ) && (*interpolationInfo)( _node ) )
					{
#if POINT_DATA_RES
						const PointData< Real , HasGradients >& pData = *( (*interpolationInfo)( _node ) );
						for( int c=0 ; c<PointData< Real , HasGradients >::SAMPLES ; c++ ) if( pData[c].weight )
#endif // POINT_DATA_RES
						{
#if POINT_DATA_RES
							Real (*__splineValues)[SupportSize] = _splineValues[c][i+LeftSupportRadius][j+LeftSupportRadius][k+LeftSupportRadius];
							Real (*_wSplineValues)[SupportSize] = wSplineValues[c][i+LeftSupportRadius][j+LeftSupportRadius][k+LeftSupportRadius];
							Real (*_dSplineValues)[SupportSize] = dSplineValues[c][i+LeftSupportRadius][j+LeftSupportRadius][k+LeftSupportRadius];
#else // !POINT_DATA_RES
							Real (*__splineValues)[SupportSize] = _splineValues[i+LeftSupportRadius][j+LeftSupportRadius][k+LeftSupportRadius];
							Real (*_wSplineValues)[SupportSize] = wSplineValues[i+LeftSupportRadius][j+LeftSupportRadius][k+LeftSupportRadius];
							Real (*_dSplineValues)[SupportSize] = dSplineValues[i+LeftSupportRadius][j+LeftSupportRadius][k+LeftSupportRadius];
#endif // POINT_DATA_RES
							// Iterate over all neighbors whose support contains the point and accumulate the mutual integral
							for( int ii=-LeftPointSupportRadius ; ii<=RightPointSupportRadius ; ii++ )
								for( int jj=-LeftPointSupportRadius ; jj<=RightPointSupportRadius ; jj++ )
									if( HasGradients )
									{
										Real partialW_SplineValue = _wSplineValues[0][ii+LeftPointSupportRadius ] * __splineValues[1][jj+LeftPointSupportRadius ];
										Real partial__SplineValue = __splineValues[0][ii+LeftPointSupportRadius ] * __splineValues[1][jj+LeftPointSupportRadius ];
										Real partialD0SplineValue = _dSplineValues[0][ii+LeftPointSupportRadius ] * __splineValues[1][jj+LeftPointSupportRadius ];
										Real partialD1SplineValue = __splineValues[0][ii+LeftPointSupportRadius ] * _dSplineValues[1][jj+LeftPointSupportRadius ];
										Real* _pointValues = pointValues[i+ii+OverlapRadius][j+jj+OverlapRadius] + k + OverlapRadius;
										Real* ___splineValues = __splineValues[2] + LeftPointSupportRadius;
										Real* __dSplineValues = _dSplineValues[2] + LeftPointSupportRadius;
										TreeOctNode* const * _neighbors = neighbors.neighbors[i+ii+OverlapRadius][j+jj+OverlapRadius] + k + OverlapRadius;
										for( int kk=-LeftPointSupportRadius ; kk<=RightPointSupportRadius ; kk++ ) if( _isValidFEMNode( _neighbors[kk] ) )
											_pointValues[kk] +=
												partialW_SplineValue * ___splineValues[kk] + partialD0SplineValue * ___splineValues[kk] + partialD1SplineValue * ___splineValues[kk] + partial__SplineValue * __dSplineValues[kk];
									}
									else
									{
										Real partialWSplineValue = _wSplineValues[0][ii+LeftPointSupportRadius ] * __splineValues[1][jj+LeftPointSupportRadius ];
										Real* _pointValues = pointValues[i+ii+OverlapRadius][j+jj+OverlapRadius] + k + OverlapRadius;
										Real* ___splineValues = __splineValues[2] + LeftPointSupportRadius;
										TreeOctNode* const * _neighbors = neighbors.neighbors[i+ii+OverlapRadius][j+jj+OverlapRadius] + k + OverlapRadius;
										for( int kk=-LeftPointSupportRadius ; kk<=RightPointSupportRadius ; kk++ ) if( _isValidFEMNode( _neighbors[kk] ) )
											_pointValues[kk] += partialWSplineValue * ___splineValues[kk];
									}
						}
					}
				}
	}
	pointValues[OverlapRadius][OverlapRadius][OverlapRadius] = diagonal;
	int nodeIndex = neighbors.neighbors[OverlapRadius][OverlapRadius][OverlapRadius]->nodeData.nodeIndex;
	if( isInterior ) // General case, so try to make fast
	{
		const TreeOctNode* const * _nodes = &neighbors.neighbors[0][0][0];
		const double* _stencil = &stencil( 0 , 0 , 0 );
		Real* _values = &pointValues[0][0][0];
		const static int CenterIndex = OverlapSize*OverlapSize*OverlapRadius + OverlapSize*OverlapRadius + OverlapRadius;
		if( interpolationInfo ) for( int i=0 ; i<OverlapSize*OverlapSize*OverlapSize ; i++ ) _values[i] = Real( _stencil[i] + _values[i] );
		else                    for( int i=0 ; i<OverlapSize*OverlapSize*OverlapSize ; i++ ) _values[i] = Real( _stencil[i] );

		row[count++] = MatrixEntry< Real >( nodeIndex-offset , _values[CenterIndex] );
		for( int i=0 ; i<OverlapSize*OverlapSize*OverlapSize ; i++ ) if( i!=CenterIndex && _isValidFEMNode( _nodes[i] ) )
			row[count++] = MatrixEntry< Real >( _nodes[i]->nodeData.nodeIndex-offset , _values[i] );
	}
	else
	{
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( node , d , off );
		Real temp = (Real)F.integrate( integrator , off , off );
		if( interpolationInfo ) temp += pointValues[OverlapRadius][OverlapRadius][OverlapRadius];
		row[count++] = MatrixEntry< Real >( nodeIndex-offset , temp );
		for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
			if( (x!=OverlapRadius || y!=OverlapRadius || z!=OverlapRadius) && _isValidFEMNode( neighbors.neighbors[x][y][z] ) )
			{
				const TreeOctNode* _node = neighbors.neighbors[x][y][z];
				LocalDepth _d ; LocalOffset _off;
				_localDepthAndOffset( _node , _d , _off );
				Real temp = (Real)F.integrate( integrator , _off , off );
				if( interpolationInfo ) temp += pointValues[x][y][z];
				row[count++] = MatrixEntry< Real >( _node->nodeData.nodeIndex-offset , temp );
			}
	}
	return count;
}

template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
int Octree< Real >::_getMatrixAndUpdateConstraints( const FEMSystemFunctor& F , const InterpolationInfo<  HasGradients >* interpolationInfo , SparseMatrix< Real >& matrix , DenseNodeData< Real , FEMDegree >& constraints , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& integrator , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& childIntegrator , const BSplineData< FEMDegree , BType >& bsData , LocalDepth depth , const DenseNodeData< Real , FEMDegree >& metSolution , bool coarseToFine )
{
	static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;
	static const int OverlapSize   =   BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;

	size_t start = _sNodesBegin(depth) , end = _sNodesEnd(depth) , range = end-start;
	Stencil< double , OverlapSize > stencil , stencils[2][2][2];
	SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::SetCentralSystemStencil ( F ,      integrator , stencil  );
	SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::SetCentralSystemStencils( F , childIntegrator , stencils );
	matrix.Resize( (int)range );
	std::vector< AdjacenctNodeKey > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( depth ) );
#pragma omp parallel for num_threads( threads )
	for( int i=0 ; i<(int)range ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i+start] ) )
	{
		AdjacenctNodeKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
		TreeOctNode* node = _sNodes.treeNodes[i+start];
		// Get the matrix row size
		typename TreeOctNode::Neighbors< OverlapSize > neighbors;
		neighborKey.template getNeighbors< false , OverlapRadius , OverlapRadius >( node , neighbors );
		int count = _getMatrixRowSize< FEMDegree , BType >( neighbors );
		// Allocate memory for the row
		matrix.SetRowSize( i , count );

		// Set the row entries
		matrix.rowSizes[i] = _setMatrixRow( F , interpolationInfo , neighbors , matrix[i] , (int)start , integrator , stencil , bsData );
		if( coarseToFine && depth>0 )
		{
			// Offset the constraints using the solution from lower resolutions.
			int x , y , z;
			Cube::FactorCornerIndex( (int)( node - node->parent->children ) , x , y , z );
			typename TreeOctNode::Neighbors< OverlapSize > pNeighbors;
			neighborKey.template getNeighbors< false , OverlapRadius , OverlapRadius >( node->parent , pNeighbors );
			_updateConstraintsFromCoarser( F , interpolationInfo , neighbors , pNeighbors , node , constraints , metSolution , childIntegrator , stencils[x][y][z] , bsData );
		}
	}
	memoryUsage();
	return 1;
}

template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
int Octree< Real >::_getSliceMatrixAndUpdateConstraints( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , SparseMatrix< Real >& matrix , DenseNodeData< Real , FEMDegree >& constraints , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& integrator , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& childIntegrator , const BSplineData< FEMDegree , BType >& bsData , LocalDepth depth , int slice , const DenseNodeData< Real , FEMDegree >& metSolution , bool coarseToFine )
{
	static const int OverlapSize   =  BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;
	static const int OverlapRadius = -BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;

	int nStart = _sNodesBegin( depth , slice ) , nEnd = _sNodesEnd( depth , slice );
	size_t range = nEnd - nStart;
	Stencil< double , OverlapSize > stencil , stencils[2][2][2];
	SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::SetCentralSystemStencil ( F ,      integrator , stencil  );
	SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::SetCentralSystemStencils( F , childIntegrator , stencils );

	matrix.Resize( (int)range );
	std::vector< AdjacenctNodeKey > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( depth ) );
#pragma omp parallel for num_threads( threads )
	for( int i=0 ; i<(int)range ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i+nStart] ) )
	{
		AdjacenctNodeKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
		TreeOctNode* node = _sNodes.treeNodes[i+nStart];
		// Get the matrix row size
		typename TreeOctNode::Neighbors< OverlapSize > neighbors;
		neighborKey.template getNeighbors< false , OverlapRadius , OverlapRadius >( node , neighbors );
		int count = _getMatrixRowSize< FEMDegree , BType >( neighbors );

		// Allocate memory for the row
		matrix.SetRowSize( i , count );

		// Set the row entries
		matrix.rowSizes[i] = _setMatrixRow( F , interpolationInfo , neighbors , matrix[i] , _sNodesBegin( depth , slice ) , integrator , stencil , bsData );

		if( coarseToFine && depth>0 )
		{
			// Offset the constraints using the solution from lower resolutions.
			int x , y , z;
			Cube::FactorCornerIndex( (int)( node - node->parent->children ) , x , y , z );
			typename TreeOctNode::Neighbors< OverlapSize > pNeighbors;
			neighborKey.template getNeighbors< false, OverlapRadius , OverlapRadius >( node->parent , pNeighbors );
			_updateConstraintsFromCoarser( F , interpolationInfo , neighbors , pNeighbors , node , constraints , metSolution , childIntegrator , stencils[x][y][z] , bsData );
		}
	}
#if !defined( _WIN32 ) && !defined( _WIN64 )
//#pragma message( "[WARNING] I'm not sure how expensive this system call is on non-Windows system. (You may want to comment this out.)" )
#endif // !_WIN32 && !_WIN64
	memoryUsage();
	return 1;
}

#ifndef MOD
#define MOD( a , b ) ( (a)>0 ? (a) % (b) : ( (b) - ( -(a) % (b) ) ) % (b) )
#endif // MOD
template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
int Octree< Real >::_solveSystemGS( const FEMSystemFunctor& F , const BSplineData< FEMDegree , BType >& bsData , InterpolationInfo< HasGradients >* interpolationInfo , LocalDepth depth , DenseNodeData< Real , FEMDegree >& solution , DenseNodeData< Real , FEMDegree >& constraints , DenseNodeData< Real , FEMDegree >& metSolutionConstraints , int iters , bool coarseToFine , _SolverStats& stats , bool computeNorms )
{
	const int OverlapRadius = -BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;
	typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template      Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >      integrator;
	typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) > childIntegrator;
	BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::SetIntegrator( integrator , depth );
	if( depth>0 ) BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::SetChildIntegrator( childIntegrator , depth-1 );

	DenseNodeData< Real , FEMDegree >& metSolution    = metSolutionConstraints;	// This stores the up-sampled solution up to depth-2
	DenseNodeData< Real , FEMDegree >& metConstraints = metSolutionConstraints; // This stores the down-sampled constraints up to depth

	int sliceBegin = _BSplineBegin< FEMDegree , BType >( depth ) , sliceEnd = _BSplineEnd< FEMDegree , BType >( depth );
	double&   systemTime = stats.  systemTime;
	double&    solveTime = stats.   solveTime;
	double& evaluateTime = stats.evaluateTime;
	systemTime = solveTime = evaluateTime = 0.;

	if( coarseToFine )
	{
		if( depth>0 )
		{
			// Up-sample the cumulative change in solution @(depth-2) into the cumulative change in solution @(depth-1)
			if( depth-2>=0 ) _upSample< Real , FEMDegree , BType >( depth-1 , metSolution );
			// Add in the change in solution @(depth-1)
#pragma omp parallel for num_threads( threads )
			for( int i=_sNodesBegin(depth-1) ; i<_sNodesEnd(depth-1) ; i++ ) metSolution[i] += solution[i];
			// evaluate the points @(depth) using the cumulative change in solution @(depth-1)
			if( interpolationInfo )
			{
				evaluateTime = Time();
				_setPointValuesFromCoarser( *interpolationInfo , depth , bsData , metSolution );
				evaluateTime = Time() - evaluateTime;
			}
		}
	}
	else if( depth<_maxDepth ) for( int i=_sNodesBegin(depth) ; i<_sNodesEnd(depth) ; i++ ) constraints[i] -= metConstraints[i];
	double bNorm = 0 , inRNorm = 0 , outRNorm = 0;
	if( depth>=0 )
	{
		// Add padding space if we are computing residuals
		int frontOffset = computeNorms ? OverlapRadius : 0 , backOffset = computeNorms ? OverlapRadius : 0;
		// Set the number of in-memory slices required for a temporally blocked solver
		int solveSlices = std::max< int >( 0 , std::min< int >( OverlapRadius*iters - (OverlapRadius-1) , sliceEnd-sliceBegin ) ) , matrixSlices = std::max< int >( 1 , std::min< int >( solveSlices+frontOffset+backOffset , sliceEnd-sliceBegin ) );
		// The list of matrices for each in-memory slices
		std::vector< SparseMatrix< Real > > _M( matrixSlices );
		// The list of multi-colored indices  for each in-memory slice
		std::vector< std::vector< std::vector< int > > > __mcIndices( solveSlices );

		int dir = coarseToFine ? -1 : 1 , start = coarseToFine ? sliceEnd-1 : sliceBegin , end = coarseToFine ? sliceBegin-1 : sliceEnd;
		for( int frontSlice=start-frontOffset*dir , backSlice = frontSlice-OverlapRadius*(iters-1)*dir ; backSlice!=end+backOffset*dir ; frontSlice+=dir , backSlice+=dir )
		{
			double t;
			if( frontSlice+frontOffset*dir>=sliceBegin && frontSlice+frontOffset*dir<sliceEnd )
			{
				int s = frontSlice+frontOffset*dir , _s = MOD( s , matrixSlices );
				t = Time();
				// Compute the system matrix
				_getSliceMatrixAndUpdateConstraints( F , interpolationInfo , _M[_s] , constraints , integrator , childIntegrator , bsData , depth , s , metSolution , coarseToFine );
				systemTime += Time()-t;
				// Compute residuals
				if( computeNorms )
				{
					ConstPointer( Real ) B = GetPointer( &constraints[0] + _sNodesBegin( depth ) , _sNodesSize( depth ) ) + ( _sNodesBegin( depth , s ) - _sNodesBegin( depth ) );
					Pointer( Real ) X = GetPointer( &solution[0] + _sNodesBegin( depth ) , _sNodesSize( depth ) ) + ( _sNodesBegin( depth , s ) - _sNodesBegin( depth ) );
#pragma omp parallel for num_threads( threads ) reduction( + : bNorm , inRNorm )
					for( int j=0 ; j<_M[_s].rows ; j++ )
					{
						Real temp = Real(0);
						ConstPointer( MatrixEntry< Real > ) start = _M[_s][j];
						ConstPointer( MatrixEntry< Real > ) end = start + _M[_s].rowSizes[j];
						ConstPointer( MatrixEntry< Real > ) e;
						for( e=start ; e!=end ; e++ ) temp += X[ e->N ] * e->Value;
						bNorm += B[j]*B[j];
						inRNorm += (temp-B[j]) * (temp-B[j]);
					}
				}
			}
			t = Time();
			// Compute the multicolor indices
			if( iters && frontSlice>=sliceBegin && frontSlice<sliceEnd )
			{
				int s = frontSlice , __s = MOD( s , solveSlices );
				for( int i=0 ; i<int( __mcIndices[__s].size() ) ; i++ ) __mcIndices[__s][i].clear();
				_setMultiColorIndices< FEMDegree >( _sNodesBegin( depth , s ) , _sNodesEnd( depth , s ) , __mcIndices[__s] );
			}
			// Advance through the in-memory slices, taking an appropriately sized stride
			for( int slice=frontSlice ; slice*dir>=backSlice*dir ; slice-=OverlapRadius*dir )
				if( slice>=sliceBegin && slice<sliceEnd )
				{
					int s = slice , _s = MOD( s , matrixSlices ) , __s = MOD( s , solveSlices );
					// Do the GS solver
					ConstPointer( Real ) B = GetPointer( &constraints[0] + _sNodesBegin( depth)  , _sNodesSize( depth ) ) + ( _sNodesBegin( depth , s ) - _sNodesBegin( depth ) );
					Pointer( Real ) X = GetPointer( &solution[0] + _sNodesBegin( depth ) , _sNodesSize( depth ) ) + ( _sNodesBegin( depth , s ) - _sNodesBegin( depth ) );
					SparseMatrix< Real >::SolveGS( __mcIndices[__s] , _M[_s] , B , X , !coarseToFine , threads );
				}
			solveTime += Time() - t;
			// Compute residuals
			if( computeNorms && backSlice-backOffset*dir>=sliceBegin && backSlice-backOffset*dir<sliceEnd )
			{
				int s = backSlice-backOffset*dir , _s = MOD( s , matrixSlices );
				ConstPointer( Real ) B = GetPointer( &constraints[0] + _sNodesBegin( depth ) , _sNodesSize( depth ) ) + ( _sNodesBegin( depth , s ) - _sNodesBegin( depth ) );
				Pointer( Real ) X = GetPointer( &solution[0] + _sNodesBegin( depth ) , _sNodesSize( depth ) ) + ( _sNodesBegin( depth , s ) - _sNodesBegin( depth ) );
#pragma omp parallel for num_threads( threads ) reduction( + : outRNorm )
				for( int j=0 ; j<_M[_s].rows ; j++ )
				{
					Real temp = Real(0);
					ConstPointer( MatrixEntry< Real > ) start = _M[_s][j];
					ConstPointer( MatrixEntry< Real > ) end = start + _M[_s].rowSizes[j];
					ConstPointer( MatrixEntry< Real > ) e;
					for( e=start ; e!=end ; e++ ) temp += X[ e->N ] * e->Value;
					outRNorm += (temp-B[j]) * (temp-B[j]);
				}
			}
		}
	}
	if( computeNorms ) stats.bNorm2 = bNorm , stats.inRNorm2 = inRNorm , stats.outRNorm2 = outRNorm;

	if( !coarseToFine && depth>0 )
	{
		// Explicitly compute the restriction of the met solution onto the coarser nodes
		// and down-sample the previous accumulation
		{
			_updateCumulativeIntegralConstraintsFromFiner( F , bsData , depth , solution , metConstraints );
			if( interpolationInfo ) _updateCumulativeInterpolationConstraintsFromFiner( *interpolationInfo , bsData , depth , solution , metConstraints );
			if( depth<_maxDepth ) _downSample< Real , FEMDegree , BType >( depth , metConstraints );
		}
	}
	memoryUsage();

	return iters;
}
#undef MOD

template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
int Octree< Real >::_solveSystemCG( const FEMSystemFunctor& F , const BSplineData< FEMDegree , BType >& bsData , InterpolationInfo< HasGradients >* interpolationInfo , LocalDepth depth , DenseNodeData< Real , FEMDegree >& solution , DenseNodeData< Real , FEMDegree >& constraints , DenseNodeData< Real , FEMDegree >& metSolutionConstraints , int iters , bool coarseToFine , _SolverStats& stats , bool computeNorms , double accuracy )
{
	typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template      Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >      integrator;
	typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) > childIntegrator;
	BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::SetIntegrator( integrator , depth );
	if( depth>0 ) BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::SetChildIntegrator( childIntegrator , depth-1 );

	DenseNodeData< Real , FEMDegree >& metSolution    = metSolutionConstraints;	// This stores the up-sampled solution up to depth-2
	DenseNodeData< Real , FEMDegree >& metConstraints = metSolutionConstraints; // This stores the down-sampled constraints up to depth

	int iter = 0;
	Pointer( Real ) X = GetPointer( &   solution[0] + _sNodesBegin(depth) , _sNodesSize(depth) );
	Pointer( Real ) B = GetPointer( &constraints[0] + _sNodesBegin(depth) , _sNodesSize(depth) );
	SparseMatrix< Real > M;
	double&   systemTime = stats.  systemTime;
	double&    solveTime = stats.   solveTime;
	double& evaluateTime = stats.evaluateTime;
	systemTime = solveTime = evaluateTime = 0.;

	if( coarseToFine )
	{
		if( depth>0 )
		{
			// Up-sample the cumulative change in solution @(depth-2) into the cumulative change in solution @(depth-1)
			if( depth-2>=0 ) _upSample< Real , FEMDegree , BType >( depth-1 , metSolution );
			// Add in the change in solution @(depth-1)
#pragma omp parallel for num_threads( threads )
			for( int i=_sNodesBegin(depth-1) ; i<_sNodesEnd(depth-1) ; i++ ) metSolution[i] += solution[i];
			// evaluate the points @(depth) using the cumulative change in solution @(depth-1)
			if( interpolationInfo )
			{
				evaluateTime = Time();
				_setPointValuesFromCoarser( *interpolationInfo , depth , bsData , metSolution );
				evaluateTime = Time() - evaluateTime;
			}
		}
	}
	else if( depth<_maxDepth ) for( int i=_sNodesBegin(depth) ; i<_sNodesEnd(depth) ; i++ ) constraints[i] -= metConstraints[i];

	// Get the system matrix (and adjust the right-hand-side based on the coarser solution if prolonging)
	systemTime = Time();
	_getMatrixAndUpdateConstraints( F , interpolationInfo , M , constraints , integrator , childIntegrator , bsData , depth , metSolution , coarseToFine );
	systemTime = Time()-systemTime;

	solveTime = Time();
	// Solve the linear system
	accuracy = Real( accuracy / 100000 ) * M.rows;
	int dim = _BSplineEnd< FEMDegree , BType >( depth ) - _BSplineBegin< FEMDegree , BType >( depth );
	int nonZeroRows = 0;
	for( int i=0 ; i<M.rows ; i++ ) if( M.rowSizes[i] ) nonZeroRows++;
	bool addDCTerm = ( nonZeroRows==dim*dim*dim && ( !interpolationInfo || !interpolationInfo->valueWeight ) && HasPartitionOfUnity< BType >() && F.vanishesOnConstants() );
	double bNorm = 0 , inRNorm = 0 , outRNorm = 0;
	if( computeNorms )
	{
#pragma omp parallel for num_threads( threads ) reduction( + : bNorm , inRNorm )
		for( int j=0 ; j<M.rows ; j++ )
		{
			Real temp = Real(0);
			ConstPointer( MatrixEntry< Real > ) start = M[j];
			ConstPointer( MatrixEntry< Real > ) end = start + M.rowSizes[j];
			ConstPointer( MatrixEntry< Real > ) e;
			for( e=start ; e!=end ; e++ ) temp += X[ e->N ] * e->Value;
			bNorm += B[j] * B[j];
			inRNorm += ( temp-B[j] ) * ( temp-B[j] );
		}
	}

	iters = std::min< int >( nonZeroRows , iters );
	if( iters ) iter += SparseMatrix< Real >::SolveCG( M , ( ConstPointer( Real ) )B , iters , X , Real( accuracy ) , 0 , addDCTerm , false , threads );

	solveTime = Time()-solveTime;
	if( computeNorms )
	{
#pragma omp parallel for num_threads( threads ) reduction( + : outRNorm )
		for( int j=0 ; j<M.rows ; j++ )
		{
			Real temp = Real(0);
			ConstPointer( MatrixEntry< Real > ) start = M[j];
			ConstPointer( MatrixEntry< Real > ) end = start + M.rowSizes[j];
			ConstPointer( MatrixEntry< Real > ) e;
			for( e=start ; e!=end ; e++ ) temp += X[ e->N ] * e->Value;
			outRNorm += ( temp-B[j] ) * ( temp-B[j] );
		}
		stats.bNorm2 = bNorm , stats.inRNorm2 = inRNorm , stats.outRNorm2 = outRNorm;
	}

	// Copy the old solution into the buffer, write in the new solution, compute the change, and update the met solution
	if( !coarseToFine && depth>0 )
	{
		// Explicitly compute the restriction of the met solution onto the coarser nodes
		// and down-sample the previous accumulation
		{
			_updateCumulativeIntegralConstraintsFromFiner( F , bsData , depth , solution , metConstraints );
			if( interpolationInfo ) _updateCumulativeInterpolationConstraintsFromFiner( *interpolationInfo , bsData , depth , solution , metConstraints );
			if( depth>_maxDepth ) _downSample< Real , FEMDegree , BType >( depth , metConstraints );
		}
	}
	memoryUsage();
	return iter;
}

template< class Real >
template< int FEMDegree , BoundaryType BType >
int Octree< Real >::_getMatrixRowSize( const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& neighbors ) const
{
	static const int OverlapSize   =   BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;
	//static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;

	int count = 0;
	//int nodeIndex = neighbors.neighbors[OverlapRadius][OverlapRadius][OverlapRadius]->nodeData.nodeIndex;
	const TreeOctNode* const * _nodes = &neighbors.neighbors[0][0][0];
	for( int i=0 ; i<OverlapSize*OverlapSize*OverlapSize ; i++ ) if( _isValidFEMNode( _nodes[i] ) ) count++;
	return count;
}


template< class Real >
template< int FEMDegree1 , int FEMDegree2 >
void Octree< Real >::_SetParentOverlapBounds( const TreeOctNode* node , int& startX , int& endX , int& startY , int& endY , int& startZ , int& endZ )
{
	const int OverlapStart = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapStart;

	if( node->parent )
	{
		int x , y , z , c = (int)( node - node->parent->children );
		Cube::FactorCornerIndex( c , x , y , z );
		startX = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::ParentOverlapStart[x]-OverlapStart , endX = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::ParentOverlapEnd[x]-OverlapStart+1;
		startY = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::ParentOverlapStart[y]-OverlapStart , endY = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::ParentOverlapEnd[y]-OverlapStart+1;
		startZ = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::ParentOverlapStart[z]-OverlapStart , endZ = BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::ParentOverlapEnd[z]-OverlapStart+1;
	}
}

// It is assumed that at this point, the evaluationg of the current depth's points, using the coarser resolution solution
// has already happened
template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
void Octree< Real >::_updateConstraintsFromCoarser( const FEMSystemFunctor& F , const InterpolationInfo<  HasGradients >* interpolationInfo , const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& neighbors , const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& pNeighbors , TreeOctNode* node , DenseNodeData< Real , FEMDegree >& constraints , const DenseNodeData< Real , FEMDegree >& metSolution , const typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& childIntegrator , const Stencil< double , BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& lapStencil , const BSplineData< FEMDegree , BType >& bsData ) const
{
	static const int LeftSupportRadius  = -BSplineSupportSizes< FEMDegree >::SupportStart;
	static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;

	if( _localDepth( node )<=0 ) return;
	// This is a conservative estimate as we only need to make sure that the parent nodes don't overlap the child (not the parent itself)
	bool isInterior = _isInteriorlyOverlapped< FEMDegree , FEMDegree >( node->parent );
	LocalDepth d ; LocalOffset off;
	_localDepthAndOffset( node , d , off );

	// Offset the constraints using the solution from lower resolutions.
	int startX , endX , startY , endY , startZ , endZ;
	_SetParentOverlapBounds< FEMDegree , FEMDegree >( node , startX , endX , startY , endY , startZ , endZ );

	for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
		if( _isValidFEMNode( pNeighbors.neighbors[x][y][z] ) )
		{
			const TreeOctNode* _node = pNeighbors.neighbors[x][y][z];
			Real _solution = metSolution[ _node->nodeData.nodeIndex ];
			{
				if( isInterior ) constraints[ node->nodeData.nodeIndex ] -= Real( lapStencil( x , y , z ) * _solution );
				else
				{
					LocalDepth _d ; LocalOffset _off;
					_localDepthAndOffset( _node , _d , _off );
					constraints[ node->nodeData.nodeIndex ] -= (Real)F.integrate( childIntegrator , _off , off ) * _solution;
				}
			}
		}

	if( interpolationInfo )
	{
		double constraint = 0;
		int fIdx[3];
		functionIndex< FEMDegree , BType >( node , fIdx );
		// evaluate the current node's basis function at adjacent points
		for( int x=-LeftSupportRadius ; x<=RightSupportRadius ; x++ ) for( int y=-LeftSupportRadius ; y<=RightSupportRadius ; y++ ) for( int z=-LeftSupportRadius ; z<=RightSupportRadius ; z++ )
		{
			const TreeOctNode* _node = neighbors.neighbors[x+OverlapRadius][y+OverlapRadius][z+OverlapRadius];
			if( _isValidSpaceNode( _node ) && (*interpolationInfo)( _node ) )
			{
				const PointData< Real , HasGradients >& pData = *( (*interpolationInfo)( _node ) );
				constraint += _ConstraintCalculator_< Real , FEMDegree , HasGradients >::_CalculateConstraint_
					(
						pData ,
						bsData. baseBSplines[ fIdx[0] ][x+LeftSupportRadius] ,
						bsData. baseBSplines[ fIdx[1] ][y+LeftSupportRadius] ,
						bsData. baseBSplines[ fIdx[2] ][z+LeftSupportRadius] ,
						bsData.dBaseBSplines[ fIdx[0] ][x+LeftSupportRadius] ,
						bsData.dBaseBSplines[ fIdx[1] ][y+LeftSupportRadius] ,
						bsData.dBaseBSplines[ fIdx[2] ][z+LeftSupportRadius]
					);
			}
		}
		constraints[ node->nodeData.nodeIndex ] -= Real( constraint );
	}
}

// Given the solution @( depth ) add to the met constraints @( depth-1 )
template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor >
void Octree< Real >::_updateCumulativeIntegralConstraintsFromFiner( const FEMSystemFunctor& F , const BSplineData< FEMDegree , BType >& , LocalDepth highDepth , const DenseNodeData< Real , FEMDegree >& fineSolution , DenseNodeData< Real , FEMDegree >& coarseConstraints ) const
{
	typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) > childIntegrator;
	BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::SetChildIntegrator( childIntegrator , highDepth-1 );

	static const int OverlapSize   =   BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;
	static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;
	typedef typename TreeOctNode::NeighborKey< -BSplineSupportSizes< FEMDegree >::SupportStart , BSplineSupportSizes< FEMDegree >::SupportEnd >SupportKey;

	if( highDepth<=0 ) return;
	// Get the stencil describing the Laplacian relating coefficients @(depth) with coefficients @(depth-1)
	Stencil< double , OverlapSize > stencils[2][2][2];
	SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::SetCentralSystemStencils( F , childIntegrator , stencils );
	//size_t start = _sNodesBegin( highDepth) , end = _sNodesEnd(highDepth) ;
	//int lStart = _sNodesBegin(highDepth-1);

	// Iterate over the nodes @( depth )
	std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( highDepth )-1 );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(highDepth) ; i<_sNodesEnd(highDepth) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
		TreeOctNode* node = _sNodes.treeNodes[i];

		// Offset the coarser constraints using the solution from the current resolutions.
		int x , y , z , c;
		c = int( node - node->parent->children );
		Cube::FactorCornerIndex( c , x , y , z );
		{
			typename TreeOctNode::Neighbors< OverlapSize > pNeighbors;
			neighborKey.template getNeighbors< false , OverlapRadius , OverlapRadius >( node->parent , pNeighbors );
			const Stencil< double , OverlapSize >& stencil = stencils[x][y][z];

			bool isInterior = _isInteriorlyOverlapped< FEMDegree , FEMDegree >( node->parent );
			LocalDepth d ; LocalOffset off;
			_localDepthAndOffset( node , d , off );

			// Offset the constraints using the solution from finer resolutions.
			int startX , endX , startY , endY , startZ , endZ;
			_SetParentOverlapBounds< FEMDegree , FEMDegree >( node , startX , endX , startY  , endY , startZ , endZ );

			Real solution = fineSolution[ node->nodeData.nodeIndex ];
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
				if( _isValidFEMNode( pNeighbors.neighbors[x][y][z] ) )
				{
					const TreeOctNode* _node = pNeighbors.neighbors[x][y][z];
					if( isInterior )
#pragma omp atomic
						coarseConstraints[ _node->nodeData.nodeIndex ] += Real( stencil( x , y , z ) * solution );
					else
					{
						LocalDepth _d ; LocalOffset _off;
						_localDepthAndOffset( _node , _d , _off );
#pragma omp atomic
						coarseConstraints[ _node->nodeData.nodeIndex ] += Real( F.integrate( childIntegrator , _off , off ) * solution );
					}
				}
		}
	}
}

template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
void Octree< Real >::setSystemMatrix( const FEMSystemFunctor& F , const InterpolationInfo<  HasGradients >* interpolationInfo , LocalDepth depth , SparseMatrix< Real >& matrix ) const
{
	if( depth<0 || depth>_maxDepth ) fprintf( stderr , "[ERROR] System depth out of bounds: %d <= %d <= %d\n" , 0 , depth , _maxDepth ) , exit( 0 );
	typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) > integrator;
	BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::SetIntegrator( integrator , depth );
	BSplineData< FEMDegree , BType > bsData( depth );

	static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;
	static const int OverlapSize   =   BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;

	Stencil< double , OverlapSize > stencil;
	SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::SetCentralSystemStencil ( F , integrator , stencil );

	matrix.Resize( _sNodesSize(depth) );
	std::vector< AdjacenctNodeKey > neighborKeys( std::max< int >( 1 , threads ) );
	for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( depth ) );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodesBegin(depth) ; i<_sNodesEnd( depth ) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
	{
		int ii = i - _sNodesBegin(depth);
		AdjacenctNodeKey& neighborKey = neighborKeys[ omp_get_thread_num() ];

		typename TreeOctNode::Neighbors< OverlapSize > neighbors;
		neighborKey.template getNeighbors< false , OverlapRadius , OverlapRadius >( _sNodes.treeNodes[i] , neighbors );

		matrix.SetRowSize( ii , _getMatrixRowSize< FEMDegree , BType >( neighbors ) );
		matrix.rowSizes[ii] = _setMatrixRow( F , interpolationInfo , neighbors , matrix[ii] , _sNodesBegin(depth) , integrator , stencil , bsData );
	}
}

template< class Real >
template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
DenseNodeData< Real , FEMDegree > Octree< Real >::solveSystem( const FEMSystemFunctor& F , InterpolationInfo< HasGradients >* interpolationInfo , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxSolveDepth , const typename Octree< Real >::SolverInfo& solverInfo )
{
	BSplineData< FEMDegree , BType > bsData( maxSolveDepth );

	maxSolveDepth = std::min< LocalDepth >( maxSolveDepth , _maxDepth );
	const int _iters = std::max< int >( 0 , solverInfo.iters );

	DenseNodeData< Real , FEMDegree > solution( _sNodesEnd( _maxDepth ) );
	memset( &solution[0] , 0 , sizeof(Real) * _sNodesEnd( _maxDepth ) );

	DenseNodeData< Real , FEMDegree > metSolution( _sNodesEnd( _maxDepth-1 ) );
	memset( &metSolution[0] , 0 , sizeof(Real)*_sNodesEnd( _maxDepth-1 ) );
	for( LocalDepth d=0 ; d<=maxSolveDepth ; d++ )
	{
		int iters = (int)ceil( _iters * pow( solverInfo.lowResIterMultiplier , maxSolveDepth-d ) );
		_SolverStats sStats;
		if( !d ) _solveSystemCG( F , bsData , interpolationInfo , d , solution , constraints , metSolution , _sNodesSize(d) , true , sStats , solverInfo.showResidual , 0 );
		else
		{
			if( d>solverInfo.cgDepth ) _solveSystemGS( F , bsData , interpolationInfo , d , solution , constraints , metSolution , iters , true , sStats , solverInfo.showResidual );
			else                       _solveSystemCG( F , bsData , interpolationInfo , d , solution , constraints , metSolution , iters , true , sStats , solverInfo.showResidual , solverInfo.cgAccuracy );
		}
		int femNodes = 0;
        #pragma omp parallel for reduction( + : femNodes )
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) ) femNodes++;
		if( solverInfo.verbose )
		{
			if( maxSolveDepth<10 ) printf( "Depth[%d/%d]:\t" , d , maxSolveDepth );
			else                   printf( "Depth[%2d/%d]:\t" , d , maxSolveDepth );
			printf( "Evaluated / Got / Solved in: %6.3f / %6.3f / %6.3f\t(%.3f MB)\tNodes: %d\n" , sStats.evaluateTime , sStats.systemTime , sStats.solveTime , _localMemoryUsage , femNodes );
		}
		if( solverInfo.showResidual && iters )
		{
			for( LocalDepth dd=0 ; dd<d ; dd++ ) printf( "  " );
			printf( "%s: %.4e -> %.4e -> %.4e (%.2e) [%d]\n" , d<=solverInfo.cgDepth ? "CG" : "GS" , sqrt( sStats.bNorm2 ) , sqrt( sStats.inRNorm2 ) , sqrt( sStats.outRNorm2 ) , sqrt( sStats.outRNorm2  / sStats.bNorm2 ) , iters );
		}
	}
	memoryUsage();
	return solution;
}

template< class Real >
template< int FEMDegree >
DenseNodeData< Real , FEMDegree > Octree< Real >::initDenseNodeData( void )
{
	DenseNodeData< Real , FEMDegree > constraints( _sNodes.size() );
	memset( &constraints[0] , 0 , sizeof(Real)*_sNodes.size() );
	return constraints;
}
template< > template< > float  Octree< float  >::_Dot( const float & r1 , const float & r2 ){ return r1*r2; }
template< > template< > double Octree< double >::_Dot( const double& r1 , const double& r2 ){ return r1*r2; }
template< > template< > float  Octree< float  >::_Dot( const Point3D< float  >& p1 , const Point3D< float  >& p2 ){ return Point3D< float  >::Dot( p1 , p2 ); }
template< > template< > double Octree< double >::_Dot( const Point3D< double >& p1 , const Point3D< double >& p2 ){ return Point3D< double >::Dot( p1 , p2 ); }
template< > template< > bool Octree< float  >::_IsZero( const float & r ){ return r==0; }
template< > template< > bool Octree< double >::_IsZero( const double& r ){ return r==0; }
template< > template< > bool Octree< float  >::_IsZero( const Point3D< float  >& p ){ return p[0]==0 && p[1]==0 && p[2]==0; }
template< > template< > bool Octree< double >::_IsZero( const Point3D< double >& p ){ return p[0]==0 && p[1]==0 && p[2]==0; }
template< class Real >
template< int FEMDegree , BoundaryType FEMBType , int CDegree , BoundaryType CBType , class FEMConstraintFunctor , class Coefficients , class D , class _D >
void Octree< Real >::_addFEMConstraints( const FEMConstraintFunctor& F , const Coefficients& coefficients , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth )
{
	typedef typename TreeOctNode::NeighborKey< -BSplineSupportSizes< FEMDegree >::SupportStart , BSplineSupportSizes< FEMDegree >::SupportEnd > SupportKey;
	const int      CFEMOverlapSize   =  BSplineOverlapSizes< CDegree , FEMDegree >::OverlapSize;
	const int  LeftCFEMOverlapRadius = -BSplineOverlapSizes< CDegree , FEMDegree >::OverlapStart;
	const int RightCFEMOverlapRadius =  BSplineOverlapSizes< CDegree , FEMDegree >::OverlapEnd;
	const int  LeftFEMCOverlapRadius = -BSplineOverlapSizes< FEMDegree , CDegree >::OverlapStart;
	const int RightFEMCOverlapRadius =  BSplineOverlapSizes< FEMDegree , CDegree >::OverlapEnd;

	// To set the constraints, we iterate over the
	// splatted normals and compute the dot-product of the
	// divergence of the normal field with all the basis functions.
	// Within the same depth: set directly as a gather
	// Coarser depths 
	maxDepth = std::min< LocalDepth >( maxDepth , _maxDepth );
	DenseNodeData< Real , FEMDegree >* __constraints = new DenseNodeData< Real , FEMDegree >( _sNodesEnd(maxDepth-1) );
	DenseNodeData< Real , FEMDegree >& _constraints = *__constraints;
	memset( &_constraints[0] , 0 , sizeof(Real)*( _sNodesEnd(maxDepth-1) ) );
	memoryUsage();

	for( LocalDepth d=maxDepth ; d>=0 ; d-- )
	{
		Stencil< _D , CFEMOverlapSize > stencil , stencils[2][2][2];
		typename SystemCoefficients< CDegree , CBType , FEMDegree , FEMBType >::     Integrator      integrator;
		typename SystemCoefficients< FEMDegree , FEMBType , CDegree , CBType >::ChildIntegrator childIntegrator;
		BSplineIntegrationData< CDegree , CBType , FEMDegree , FEMBType >::SetIntegrator( integrator , d );
		if( d>0 ) BSplineIntegrationData< FEMDegree , FEMBType , CDegree , CBType >::SetChildIntegrator( childIntegrator , d-1 );
		SystemCoefficients< CDegree , CBType , FEMDegree , FEMBType >::template SetCentralConstraintStencil < false >( F,      integrator , stencil  );
		SystemCoefficients< FEMDegree , FEMBType , CDegree , CBType >::template SetCentralConstraintStencils< true  >( F, childIntegrator , stencils );

		std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
		for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d ) );

        #pragma omp parallel for num_threads( threads )
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ )
		{
			SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
			TreeOctNode* node = _sNodes.treeNodes[i];
			int startX=0 , endX=CFEMOverlapSize , startY=0 , endY=CFEMOverlapSize , startZ=0 , endZ=CFEMOverlapSize;
			typename TreeOctNode::Neighbors< CFEMOverlapSize > neighbors;
			neighborKey.template getNeighbors< false , LeftFEMCOverlapRadius , RightFEMCOverlapRadius >( node , neighbors );
			bool isInterior = _isInteriorlyOverlapped< FEMDegree , CDegree >( node ) , isInterior2 = _isInteriorlyOverlapped< CDegree , FEMDegree >( node->parent );

			LocalDepth d ; LocalOffset off;
			_localDepthAndOffset( node , d , off );
			// Set constraints from current depth
			// Gather the constraints from the vector-field at _node into the constraint stored with node
			if( _isValidFEMNode( node ) )
			{
				for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
				{
					const TreeOctNode* _node = neighbors.neighbors[x][y][z];
					if( isValidFEMNode< CDegree , CBType >( _node ) )
					{
						const D* d = coefficients( _node );
						if( d ) {
							if( isInterior ) { constraints[i] += _Dot( (D)stencil( x , y , z ) , *d ); }
							else
							{
								LocalDepth _d ; LocalOffset _off;
								_localDepthAndOffset( _node , _d , _off );
								constraints[i] += _Dot( *d , (D)F.template integrate< false >( integrator , _off , off ) );
							}
						}
					}
				}
				_SetParentOverlapBounds< CDegree , FEMDegree >( node , startX , endX , startY , endY , startZ , endZ );
			}
			if( !isValidFEMNode< CDegree , CBType >( node ) ) continue;
			const D* _data = coefficients( node );
			if( !_data ) continue;
			const D& data = *_data;
			if( _IsZero( data ) ) continue;

			// Set the _constraints for the parents
			if( d>0 )
			{
				int cx , cy , cz;
				Cube::FactorCornerIndex( (int)( node - node->parent->children ) , cx , cy ,cz );
				const Stencil< _D , CFEMOverlapSize >& _stencil = stencils[cx][cy][cz];

				neighborKey.template getNeighbors< false , LeftCFEMOverlapRadius , RightCFEMOverlapRadius >( node->parent , neighbors );

				for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
				{
					TreeOctNode* _node = neighbors.neighbors[x][y][z];
					if( _node && ( isInterior2 || _isValidFEMNode( _node ) ) )
					{
						TreeOctNode* _node = neighbors.neighbors[x][y][z];
						Real c;
						if( isInterior2 ) c = _Dot( (D)_stencil( x , y , z ) , data );
						else
						{
							LocalDepth _d ; LocalOffset _off;
							_localDepthAndOffset( _node , _d , _off );
							c = _Dot( data , (D)F.template integrate< true >( childIntegrator , _off , off ) );
						}
#pragma omp atomic
						_constraints[ _node->nodeData.nodeIndex ] += c;
					}
				}
			}
		}
		memoryUsage();
	}

	// Fine-to-coarse down-sampling of constraints
	for( LocalDepth d=maxDepth-1 ; d>0 ; d-- ) _downSample< Real , FEMDegree , FEMBType >( d , _constraints );

	// Add the accumulated constraints from all finer depths
#pragma omp parallel for num_threads( threads )
	for( int i=0 ; i<_sNodesEnd(maxDepth-1) ; i++ ) constraints[i] += _constraints[i];

	delete __constraints;

	DenseNodeData< D , CDegree > _coefficients( _sNodesEnd(maxDepth-1) );
	memset( &_coefficients[0] , 0 , sizeof(D) * _sNodesEnd(maxDepth-1) );
	for( LocalDepth d=maxDepth-1 ; d>=0 ; d-- )
	{
#pragma omp parallel for num_threads( threads )
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( isValidFEMNode< CDegree , CBType >( _sNodes.treeNodes[i] ) )
		{
			const D* d = coefficients( _sNodes.treeNodes[i] );
			if( d )	_coefficients[i] += *d;
		}
	}

	// Coarse-to-fine up-sampling of coefficients
	for( LocalDepth d=1 ; d<maxDepth ; d++ ) _upSample< D , CDegree , CBType >( d , _coefficients );

	// Compute the contribution from all coarser depths
	for( LocalDepth d=1 ; d<=maxDepth ; d++ )
	{
		//size_t start = _sNodesBegin( d ) , end = _sNodesEnd( d );
		Stencil< _D , CFEMOverlapSize > stencils[2][2][2];
		typename SystemCoefficients< CDegree , CBType , FEMDegree , FEMBType >::ChildIntegrator childIntegrator;
		BSplineIntegrationData< CDegree , CBType , FEMDegree , FEMBType >::SetChildIntegrator( childIntegrator , d-1 );
		SystemCoefficients< CDegree , CBType , FEMDegree , FEMBType >::template SetCentralConstraintStencils< false >( F , childIntegrator , stencils );
		std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
		for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d-1 ) );
        #pragma omp parallel for num_threads( threads )
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
		{
			SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
			TreeOctNode* node = _sNodes.treeNodes[i];
			int startX , endX , startY , endY , startZ , endZ;
			_SetParentOverlapBounds< FEMDegree , CDegree >( node , startX , endX , startY , endY , startZ , endZ );
			typename TreeOctNode::Neighbors< CFEMOverlapSize > pNeighbors;
			neighborKey.template getNeighbors< false , LeftFEMCOverlapRadius , RightFEMCOverlapRadius >( node->parent , pNeighbors );

			bool isInterior = _isInteriorlyOverlapped< FEMDegree , CDegree >( node->parent );
			int cx , cy , cz;
			if( d>0 )
			{
				int c = int( node - node->parent->children );
				Cube::FactorCornerIndex( c , cx , cy , cz );
			}
			else cx = cy = cz = 0;
			Stencil< _D , CFEMOverlapSize >& _stencil = stencils[cx][cy][cz];

			Real constraint = Real(0);
			LocalDepth d ; LocalOffset off;
			_localDepthAndOffset( node , d , off );
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				TreeOctNode* _node = pNeighbors.neighbors[x][y][z];
				if( isValidFEMNode< CDegree , CBType >( _node ) )
				{
					if( isInterior ) constraint += _Dot( _coefficients[ _node->nodeData.nodeIndex ] , (D)_stencil( x , y , z ) );
					else
					{
						LocalDepth _d ; LocalOffset _off;
						_localDepthAndOffset ( _node , _d , _off );
						constraint += _Dot( _coefficients[ _node->nodeData.nodeIndex ] , (D)F.template integrate< false >( childIntegrator , _off , off ) );
					}
				}
			}
			constraints[i] += constraint;
		}
	}
	memoryUsage();
}

template< class Real >
template< int FEMDegree , BoundaryType BType , bool HasGradients >
void Octree< Real >::addInterpolationConstraints( const InterpolationInfo< HasGradients >& interpolationInfo , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth )
{
	typedef typename TreeOctNode::NeighborKey< -BSplineSupportSizes< FEMDegree >::SupportStart , BSplineSupportSizes< FEMDegree >::SupportEnd > SupportKey;
	maxDepth = std::min< LocalDepth >( maxDepth , _maxDepth );
	{
		static const int OverlapSize = BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize;
		static const int LeftSupportRadius  = -BSplineSupportSizes< FEMDegree >::SupportStart;
		static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
		static const int OverlapRadius = - BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapStart;
		BSplineData< FEMDegree , BType > bsData( _maxDepth );
		for( int d=0 ; d<=maxDepth ; d++ )
		{
			std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
			for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( maxDepth ) );

#pragma omp parallel for num_threads( threads )
			for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( _isValidFEMNode( _sNodes.treeNodes[i] ) )
			{
				TreeOctNode* node = _sNodes.treeNodes[i];
				SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
				typename TreeOctNode::Neighbors< OverlapSize > neighbors;
				neighborKey.template getNeighbors< false , OverlapRadius , OverlapRadius >( node , neighbors );

				double constraint = 0;
				int fIdx[3];
				functionIndex< FEMDegree , BType >( node , fIdx );
				// evaluate the current node's basis function at adjacent points
				for( int x=-LeftSupportRadius ; x<=RightSupportRadius ; x++ ) for( int y=-LeftSupportRadius ; y<=RightSupportRadius ; y++ ) for( int z=-LeftSupportRadius ; z<=RightSupportRadius ; z++ )
				{
					const TreeOctNode* _node = neighbors.neighbors[x+OverlapRadius][y+OverlapRadius][z+OverlapRadius];
					if( _isValidSpaceNode( _node ) && interpolationInfo( _node ) )
					{
						const PointData< Real , HasGradients >& pData = *( interpolationInfo( _node ) );
						constraint += _ConstraintCalculator_< Real , FEMDegree , HasGradients >::_CalculateConstraint_
							(
								pData ,
								bsData. baseBSplines[ fIdx[0] ][x+LeftSupportRadius] ,
								bsData. baseBSplines[ fIdx[1] ][y+LeftSupportRadius] ,
								bsData. baseBSplines[ fIdx[2] ][z+LeftSupportRadius] ,
								bsData.dBaseBSplines[ fIdx[0] ][x+LeftSupportRadius] ,
								bsData.dBaseBSplines[ fIdx[1] ][y+LeftSupportRadius] ,
								bsData.dBaseBSplines[ fIdx[2] ][z+LeftSupportRadius] ,
								interpolationInfo.valueWeight , interpolationInfo.gradientWeight
							);
					}
				}
				constraints[ node->nodeData.nodeIndex ] += (Real)constraint;
			}
		}
		memoryUsage();
	}
}
template< class Real >
template< int FEMDegree1 , BoundaryType FEMBType1 , int FEMDegree2 , BoundaryType FEMBType2 , class DotFunctor , bool HasGradients , class Coefficients1 , class Coefficients2 >
double Octree< Real >::_dot( const DotFunctor& F , const InterpolationInfo< HasGradients >* iInfo , const Coefficients1& coefficients1 , const Coefficients2& coefficients2 ) const
{
	double dot = 0;

	// Calculate the contribution from @(depth,depth)
	{
		typedef typename TreeOctNode::ConstNeighborKey< -BSplineSupportSizes< FEMDegree1 >::SupportStart , BSplineSupportSizes< FEMDegree1 >::SupportEnd > SupportKey;
		const int      OverlapSize   =  BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapSize;
		const int  LeftOverlapRadius = -BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapStart;
		const int RightOverlapRadius =  BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapEnd;

		for( LocalDepth d=0 ; d<=_maxDepth ; d++ )
		{
			Stencil< double , OverlapSize > stencil;
			typename SystemCoefficients< FEMDegree1 , FEMBType1 , FEMDegree2 , FEMBType2 >::Integrator integrator;
			BSplineIntegrationData< FEMDegree1 , FEMBType1 , FEMDegree2 , FEMBType2 >::SetIntegrator( integrator , d );
			SystemCoefficients< FEMDegree1 , FEMBType1 , FEMDegree2 , FEMBType2 >::template SetCentralConstraintStencil< false , DotFunctor >( F , integrator , stencil );

			std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
			for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d ) );

#pragma omp parallel for num_threads( threads ) reduction( + : dot )
			for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ )
			{
				const TreeOctNode* node = _sNodes.treeNodes[i];
				const Real* _data1;
				if( isValidFEMNode< FEMDegree1 , FEMBType1 >( node ) && ( _data1=coefficients1(node) ) )
				{
					SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
					typename TreeOctNode::ConstNeighbors< OverlapSize > neighbors;
					neighborKey.template getNeighbors< LeftOverlapRadius , RightOverlapRadius >( node , neighbors );
					bool isInterior = _isInteriorlyOverlapped< FEMDegree1 , FEMDegree2 >( node );

					LocalDepth d ; LocalOffset off;
					_localDepthAndOffset( node , d , off );

					for( int x=0 ; x<OverlapSize ; x++ ) for( int y=0 ; y<OverlapSize ; y++ ) for( int z=0 ; z<OverlapSize ; z++ )
					{
						const TreeOctNode* _node = neighbors.neighbors[x][y][z];
						const Real* _data2;
						if( isValidFEMNode< FEMDegree2 , FEMBType2 >( _node ) && ( _data2=coefficients2( _node ) ) ) {
							if( isInterior ) { dot += (*_data1) * (*_data2 ) * stencil( x , y , z ); }
							else
							{
								LocalDepth _d ; LocalOffset _off;
								_localDepthAndOffset( _node , _d , _off );
								dot += (*_data1) * (*_data2) * F.template integrate< false >( integrator , off , _off );
							}
						}
					}
				}
			}
		}
	}
	// Calculate the contribution from @(<depth,depth)
	{
		typedef typename TreeOctNode::ConstNeighborKey< -BSplineSupportSizes< FEMDegree1 >::SupportStart , BSplineSupportSizes< FEMDegree1 >::SupportEnd > SupportKey;
		const int      OverlapSize   =  BSplineOverlapSizes< FEMDegree2 , FEMDegree1 >::OverlapSize;
		const int  LeftOverlapRadius = -BSplineOverlapSizes< FEMDegree2 , FEMDegree1 >::OverlapStart;
		const int RightOverlapRadius =  BSplineOverlapSizes< FEMDegree2 , FEMDegree1 >::OverlapEnd;

		DenseNodeData< Real , FEMDegree1 > cumulative1( _sNodesEnd( _maxDepth-1 ) );
		if( _maxDepth>0 ) memset( &cumulative1[0] , 0 , sizeof(Real) * _sNodesEnd( _maxDepth-1 ) );

		for( LocalDepth d=1 ; d<=_maxDepth ; d++ )
		{
			// Update the cumulative coefficients with the coefficients @(depth-1)
#pragma omp parallel for
			for( int i=_sNodesBegin(d-1) ; i<_sNodesEnd(d-1) ; i++ )
			{
				const Real* _data1 = coefficients1( _sNodes.treeNodes[i] );
				if( _data1 ) cumulative1[i] += *_data1;
			}

			Stencil< double , OverlapSize > stencils[2][2][2];
			typename SystemCoefficients< FEMDegree1 , FEMBType1 , FEMDegree2 , FEMBType2 >::ChildIntegrator childIntegrator;
			BSplineIntegrationData< FEMDegree1 , FEMBType1 , FEMDegree2 , FEMBType2 >::SetChildIntegrator( childIntegrator , d-1 );
			SystemCoefficients< FEMDegree1 , FEMBType1 , FEMDegree2 , FEMBType2 >::template SetCentralConstraintStencils< false >( F, childIntegrator , stencils );

			std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
			for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d-1 ) );

#pragma omp parallel for num_threads( threads ) reduction( + : dot )
			for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ )
			{
				const TreeOctNode* node = _sNodes.treeNodes[i];
				const Real* _data2;
				if( isValidFEMNode< FEMDegree2 , FEMBType2 >( node ) && ( _data2=coefficients2( node ) ) )
				{
					SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
					bool isInterior = _isInteriorlyOverlapped< FEMDegree1 , FEMDegree2 >( node->parent );

					LocalDepth d ; LocalOffset off;
					_localDepthAndOffset( node , d , off );

					int cx , cy , cz;
					Cube::FactorCornerIndex( (int)( node - node->parent->children ) , cx , cy ,cz );
					const Stencil< double , OverlapSize >& _stencil = stencils[cx][cy][cz];
					typename TreeOctNode::ConstNeighbors< OverlapSize > neighbors;
					neighborKey.template getNeighbors< LeftOverlapRadius , RightOverlapRadius >( node->parent , neighbors );

					int startX , endX , startY , endY , startZ , endZ;
					_SetParentOverlapBounds< FEMDegree2 , FEMDegree1 >( node , startX , endX , startY , endY , startZ , endZ );
					for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
					{
						const TreeOctNode* _node = neighbors.neighbors[x][y][z];
						const Real* _data1;
						if( isValidFEMNode< FEMDegree1 , FEMBType1 >( _node ) && ( _data1=cumulative1(_node) ) )
						{
							if( isInterior ) dot += (*_data1) * (*_data2) * _stencil( x , y , z );
							else
							{
								LocalDepth _d ; LocalOffset _off;
								_localDepthAndOffset( _node , _d , _off );
								dot += (*_data1) * (*_data2) * F.template integrate< false >( childIntegrator , _off , off );
							}
						}
					}
				}
			}
			// Up sample the cumulative coefficients for the next level
			if( d<_maxDepth ) _upSample< Real , FEMDegree1 , FEMBType1 >( d , cumulative1 );
		}
	}

	// Calculate the contribution from @(>depth,depth)
	{
		typedef typename TreeOctNode::ConstNeighborKey< -BSplineSupportSizes< FEMDegree2 >::SupportStart , BSplineSupportSizes< FEMDegree2 >::SupportEnd > SupportKey;
		const int      OverlapSize   =  BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapSize;
		const int  LeftOverlapRadius = -BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapStart;
		const int RightOverlapRadius =  BSplineOverlapSizes< FEMDegree1 , FEMDegree2 >::OverlapEnd;

		DenseNodeData< Real , FEMDegree2 > cumulative2( _sNodesEnd( _maxDepth-1 ) );
		if( _maxDepth>0 ) memset( &cumulative2[0] , 0 , sizeof(Real) * _sNodesEnd( _maxDepth-1 ) );

		for( LocalDepth d=_maxDepth ; d>0 ; d-- )
		{
			Stencil< double , OverlapSize > stencils[2][2][2];
			typename SystemCoefficients< FEMDegree2 , FEMBType2 , FEMDegree1 , FEMBType1 >::ChildIntegrator childIntegrator;
			BSplineIntegrationData< FEMDegree2 , FEMBType2 , FEMDegree1 , FEMBType1 >::SetChildIntegrator( childIntegrator , d-1 );
			SystemCoefficients< FEMDegree2 , FEMBType2 , FEMDegree1 , FEMBType1 >::template SetCentralConstraintStencils< true >( F , childIntegrator , stencils );

			std::vector< SupportKey > neighborKeys( std::max< int >( 1 , threads ) );
			for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d-1 ) );

			// Update the cumulative constraints @(depth-1) from @(depth)
#pragma omp parallel for num_threads( threads )
			for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ )
			{
				const TreeOctNode* node = _sNodes.treeNodes[i];
				const Real* _data1;
				if( isValidFEMNode< FEMDegree1 , FEMBType1 >( node ) && ( _data1=coefficients1( node ) ) )
				{
					SupportKey& neighborKey = neighborKeys[ omp_get_thread_num() ];
					bool isInterior = _isInteriorlyOverlapped< FEMDegree2 , FEMDegree1 >( node->parent );

					LocalDepth d ; LocalOffset off;
					_localDepthAndOffset( node , d , off );

					int cx , cy , cz;
					Cube::FactorCornerIndex( (int)( node - node->parent->children ) , cx , cy ,cz );
					const Stencil< double , OverlapSize >& _stencil = stencils[cx][cy][cz];
					typename TreeOctNode::ConstNeighbors< OverlapSize > neighbors;
					neighborKey.template getNeighbors< LeftOverlapRadius , RightOverlapRadius >( node->parent , neighbors );

					int startX , endX , startY , endY , startZ , endZ;
					_SetParentOverlapBounds< FEMDegree1 , FEMDegree2 >( node , startX , endX , startY , endY , startZ , endZ );

					for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
					{
						const TreeOctNode* _node = neighbors.neighbors[x][y][z];
						if( isValidFEMNode< FEMDegree2 , FEMBType2 >( _node ) )
						{
							Real _dot;
							if( isInterior ) _dot = (*_data1) * _stencil( x , y , z );
							else
							{
								LocalDepth _d ; LocalOffset _off;
								_localDepthAndOffset( _node , _d , _off );
								_dot = (*_data1) * F.template integrate< true >( childIntegrator , _off , off );
							}
#pragma omp atomic
							cumulative2[ _node->nodeData.nodeIndex ] += _dot;
						}
					}
				}
			}
			// Update the dot-product using the cumulative constraints @(depth-1)
#pragma omp parallel for num_threads( threads ) reduction( + : dot )
			for( int i=_sNodesBegin(d-1) ; i<_sNodesEnd(d-1) ; i++ )
			{
				const TreeOctNode* node = _sNodes.treeNodes[i];
				const Real* _data2;
				if( isValidFEMNode< FEMDegree2 , FEMBType2 >( node ) && ( _data2=coefficients2( node ) ) ) dot += cumulative2[ node->nodeData.nodeIndex ] * (*_data2);
			}

			// Down-sample the cumulative constraints from @(depth-1) to @(depth-2) for the next pass
			if( d-1>0 ) _downSample< Real , FEMDegree2 , FEMBType2 >( d-1 , cumulative2 );
		}
	}

	if( iInfo )
	{
		MultiThreadedEvaluator< FEMDegree1 , FEMBType1 > mt1( this , coefficients1 , threads );
		MultiThreadedEvaluator< FEMDegree2 , FEMBType2 > mt2( this , coefficients2 , threads );

#pragma omp parallel for num_threads( threads ) reduction( + : dot )
		for( int i=_sNodesBegin(0) ; i<_sNodesEnd(_maxDepth) ; i++ )
		{
			if( _isValidSpaceNode( _sNodes.treeNodes[i] ) && !_isValidSpaceNode( _sNodes.treeNodes[i]->children ) && (*iInfo)( _sNodes.treeNodes[i] ) )
			{

				const PointData< Real , HasGradients >& pData = *( (*iInfo)( _sNodes.treeNodes[i] ) );
#if POINT_DATA_RES
				for( int c=0 ; c<PointData< Real , false >::SAMPLES ; c++ ) if( pData[c].weight ) 
				{
					Point3D< Real > p = pData[c].position;
					Real w = pData[c].weight;
					if( HasGradients )
					{
						std::pair< Real , Point3D< Real > > v1 = mt1.valueAndGradient( p , omp_get_thread_num() );
						std::pair< Real , Point3D< Real > > v2 = mt2.valueAndGradient( p , omp_get_thread_num() );
						dot += v1.first * v2.first * w * iInfo->valueWeight + Point3D< Real >::Dot( v1.second , v2.second ) * w * iInfo->gradientWeight;
					}
					else dot += mt1.value( p , omp_get_thread_num() ) * mt2.value( p , omp_get_thread_num() ) * w * iInfo->valueWeight;
				}
#else // !POINT_DATA_RES
				Point3D< Real > p = pData.position;
				Real w = pData.weight;
				if( HasGradients )
				{
					std::pair< Real , Point3D< Real > > v1 = mt1.valueAndGradient( p , omp_get_thread_num() );
					std::pair< Real , Point3D< Real > > v2 = mt2.valueAndGradient( p , omp_get_thread_num() );
					dot += v1.first * v2.first * w * iInfo->valueWeight + Point3D< Real >::Dot( v1.second , v2.second ) * w * iInfo->gradientWeight;
				}
				else dot += mt1.value( p , omp_get_thread_num() ) * mt2.value( p , omp_get_thread_num() ) * w * iInfo->valueWeight;
#endif // POINT_DATA_RES
			}
		}
	}

	return dot;
}
