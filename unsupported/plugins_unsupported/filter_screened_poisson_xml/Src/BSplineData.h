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

#define NEW_BSPLINE_CODE

#include "BinaryNode.h"
#include "PPolynomial.h"
#include "Array.h"

enum BoundaryType
{
	BOUNDARY_FREE ,
	BOUNDARY_DIRICHLET ,
	BOUNDARY_NEUMANN ,
	BOUNDARY_COUNT
};
const char* BoundaryNames[] = { "free" , "Dirichlet" , "Neumann" };
template< BoundaryType BType > inline bool HasPartitionOfUnity( void ){ return BType!=BOUNDARY_DIRICHLET; }

// This class represents a function that is a linear combination of B-spline elements.
// The coeff member indicating how much of each element is present.
// [WARNING] The ordering of B-spline elements is in the opposite order from that returned by Polynomial::BSplineComponent
template< int Degree >
struct BSplineElementCoefficients
{
	int coeffs[Degree+1];
	BSplineElementCoefficients( void ){ memset( coeffs , 0 , sizeof( coeffs ) ); }
	int& operator[]( int idx ){ return coeffs[idx]; }
	const int& operator[]( int idx ) const { return coeffs[idx]; }
};

// This class represents a function on the the interval, partitioned into "res" blocks.
// On each block, the function is a degree-Degree polynomial, represented by the coefficients
// in the associated BSplineElementCoefficients.
// [NOTE] This representation of a function is agnostic to the type of boundary conditions (though the constructor is not).
template< int Degree >
struct BSplineElements : public std::vector< BSplineElementCoefficients< Degree > >
{
	static const bool _Primal = (Degree&1)==1;
	static const int _Off = (Degree+1)/2;
	static int _ReflectLeft ( int offset , int res );
	static int _ReflectRight( int offset , int res );
	static int _RotateLeft  ( int offset , int res );
	static int _RotateRight ( int offset , int res );
	template< bool Left > void _addPeriodic( int offset , bool negate );
public:
	// Coefficients are ordered as "/" "-" "\"
	// [WARNING] This is the opposite of the order in Polynomial::BSplineComponent
	int denominator;

	BSplineElements( void ) { denominator = 1; }
	BSplineElements( int res , int offset , BoundaryType bType );

	void upSample( BSplineElements& high ) const;
	template< unsigned int D >
	void differentiate( BSplineElements< Degree-D >& d ) const;

	void print( FILE* fp=stdout ) const
	{
		for( int i=0 ; i<std::vector< BSplineElementCoefficients< Degree > >::size() ; i++ )
		{
			printf( "%d]" , i );
			for( int j=0 ; j<=Degree ; j++ ) printf( " %d" , (*this)[i][j] );
			printf( " (%d)\n" , denominator );
		}
	}
	Polynomial< Degree > polynomial( int idx ) const
	{
		int res = (int)std::vector< BSplineElementCoefficients< Degree > >::size();
		Polynomial< Degree > P;
		if( idx>=0 && idx<res ) for( int d=0 ; d<=Degree ; d++ ) P += Polynomial< Degree >::BSplineComponent( Degree-d ).scale( 1./res ).shift( (idx+0.)/res ) * ( (*this)[idx][d] );
		return P / denominator;
	}
	PPolynomial< Degree > pPolynomial( void ) const
	{
		int res = (int)std::vector< BSplineElementCoefficients< Degree > >::size();
		PPolynomial< Degree > P;
		P.polyCount = res + 1;
		P.polys = AllocPointer< StartingPolynomial< Degree > >( P.polyCount );
		for( int i=0 ; i<P.polyCount ; i++ ) P.polys[i].start = (i+0.) / res , P.polys[i].p = polynomial(i);
		for( int i=res ; i>=1 ; i-- ) P.polys[i].p -= P.polys[i-1].p;
		return P.compress(0);
	}
};
template< int Degree , int DDegree > struct Differentiator                   { static void Differentiate( const BSplineElements< Degree >& bse , BSplineElements< DDegree >& dbse ); };
template< int Degree >               struct Differentiator< Degree , Degree >{ static void Differentiate( const BSplineElements< Degree >& bse , BSplineElements<  Degree >& dbse ); };
#define BSPLINE_SET_BOUNDS( name , s , e ) \
	static const int name ## Start = (s); \
	static const int name ## End   = (e); \
	static const int name ## Size  = (e)-(s)+1

// Assumes that x is non-negative
#define _FLOOR_OF_HALF( x ) (   (x)    >>1 )
#define  _CEIL_OF_HALF( x ) ( ( (x)+1 )>>1 )
// Done with the assumption
#define FLOOR_OF_HALF( x ) ( (x)<0 ? -  _CEIL_OF_HALF( -(x) ) : _FLOOR_OF_HALF( x ) )
#define  CEIL_OF_HALF( x ) ( (x)<0 ? - _FLOOR_OF_HALF( -(x) ) :  _CEIL_OF_HALF( x ) )
#define SMALLEST_INTEGER_LARGER_THAN_HALF( x ) (  CEIL_OF_HALF( (x)+1 ) )
#define LARGEST_INTEGER_SMALLER_THAN_HALF( x ) ( FLOOR_OF_HALF( (x)-1 ) )
#define SMALLEST_INTEGER_LARGER_THAN_OR_EQUAL_TO_HALF( x ) (  CEIL_OF_HALF( x ) )
#define LARGEST_INTEGER_SMALLER_THAN_OR_EQUAL_TO_HALF( x ) ( FLOOR_OF_HALF( x ) )

template< int Degree >
struct BSplineSupportSizes
{
	inline static int Nodes( int depth ){ return ( 1<<depth ) + ( Degree&1 ); }
	inline static bool OutOfBounds( int depth , int offset ){ return offset>=0 || offset<Nodes(depth); }
	// An index is interiorly supported if its support is in the range [0,1<<depth)
	inline static void InteriorSupportedSpan( int depth , int& begin , int& end ){ begin = -SupportStart , end = (1<<depth)-SupportEnd; }

	// If the degree is even, we use a dual basis and functions are centered at the center of the interval
	// It the degree is odd, we use a primal basis and functions are centered at the left end of the interval
	// The function at index I is supported in:
	//	Support( I ) = [ I - (Degree+1-Inset)/2 , I + (Degree+1+Inset)/2 ]
	// [NOTE] The value of ( Degree + 1 +/- Inset ) is always even
	static const int Inset = (Degree&1) ? 0 : 1;
	BSPLINE_SET_BOUNDS(      Support , -( (Degree+1)/2 ) , Degree/2           );
	BSPLINE_SET_BOUNDS( ChildSupport ,    2*SupportStart , 2*(SupportEnd+1)-1 );
	BSPLINE_SET_BOUNDS(       Corner ,    SupportStart+1 , SupportEnd         );
	BSPLINE_SET_BOUNDS(  ChildCorner ,  2*SupportStart+1 , 2*SupportEnd + 1   );

	// Setting I=0, we are looking for the smallest/largest integers J such that:
	//		Support( 0 ) CONTAINS Support( J )
	// <=>	[-(Degree+1-Inset) , (Degree+1+Inset) ] CONTAINS [ J-(Degree+1-Inset)/2 , J+(Degree+1+Inset)/2 ]
	// Which is the same as the smallest/largest integers J such that:
	//		J - (Degree+1-Inset)/2 >= -(Degree+1-Inset)	| J + (Degree+1+Inset)/2 <= (Degree+1+Inset)
	// <=>	J >= -(Degree+1-Inset)/2					| J <= (Degree+1+Inset)/2
	BSPLINE_SET_BOUNDS( UpSample , - ( Degree + 1 - Inset ) / 2 , ( Degree + 1 + Inset ) /2 );

	// Setting I=0/1, we are looking for the smallest/largest integers J such that:
	//		Support( J ) CONTAINS Support( 0/1 )
	// <=>	[ 2*J - (Degree+1-Inset) , 2*J + (Degree+1+Inset) ] CONTAINS [ 0/1 - (Degree+1-Inset)/2 , 0/1 + (Degree+1+Inset)/2 ]
	// Which is the same as the smallest/largest integers J such that:
	//		2*J + (Degree+1+Inset) >= 0/1 + (Degree+1+Inset)/2	| 2*J - (Degree+1-Inset) <= 0/1 - (Degree+1-Inset)/2
	// <=>	2*J >= 0/1 - (Degree+1+Inset)/2						| 2*J <= 0/1 + (Degree+1-Inset)/2
	BSPLINE_SET_BOUNDS( DownSample0 , SMALLEST_INTEGER_LARGER_THAN_OR_EQUAL_TO_HALF( 0 - ( Degree + 1 + Inset ) / 2 ) , LARGEST_INTEGER_SMALLER_THAN_OR_EQUAL_TO_HALF( 0 + ( Degree + 1 - Inset ) / 2 ) );
	BSPLINE_SET_BOUNDS( DownSample1 , SMALLEST_INTEGER_LARGER_THAN_OR_EQUAL_TO_HALF( 1 - ( Degree + 1 + Inset ) / 2 ) , LARGEST_INTEGER_SMALLER_THAN_OR_EQUAL_TO_HALF( 1 + ( Degree + 1 - Inset ) / 2 ) );
	static const int DownSampleStart[] , DownSampleEnd[] , DownSampleSize[];
};
template< int Degree > const int BSplineSupportSizes< Degree >::DownSampleStart[] = { DownSample0Start , DownSample1Start };
template< int Degree > const int BSplineSupportSizes< Degree >::DownSampleEnd  [] = { DownSample0End   , DownSample1End   };
template< int Degree > const int BSplineSupportSizes< Degree >::DownSampleSize [] = { DownSample0Size  , DownSample1Size  };


// Given a B-Spline of degree Degree1 at position i, this gives the offsets of the B-splines of degree Degree2 that just overlap with it.
template< int Degree1 , int Degree2 >
struct BSplineOverlapSizes
{
	typedef BSplineSupportSizes< Degree1 > EData1;
	typedef BSplineSupportSizes< Degree2 > EData2;
	BSPLINE_SET_BOUNDS(             Overlap , EData1::     SupportStart - EData2::SupportEnd , EData1::     SupportEnd - EData2::SupportStart );
	BSPLINE_SET_BOUNDS(        ChildOverlap , EData1::ChildSupportStart - EData2::SupportEnd , EData1::ChildSupportEnd - EData2::SupportStart );
	BSPLINE_SET_BOUNDS(      OverlapSupport ,      OverlapStart + EData2::SupportStart ,      OverlapEnd + EData2::SupportEnd );
	BSPLINE_SET_BOUNDS( ChildOverlapSupport , ChildOverlapStart + EData2::SupportStart , ChildOverlapEnd + EData2::SupportEnd );

	// Setting I=0/1, we are looking for the smallest/largest integers J such that:
	//		Support( 2*J ) * 2 INTERSECTION Support( 0/1 ) NON-EMPTY
	// <=>	[ 2*J - (Degree2+1-Inset2) , 2*J + (Degree2+1+Inset2) ] INTERSECTION [ 0/1 - (Degree1+1-Inset1)/2 , 0/1 + (Degree1+1+Inset1)/2 ] NON-EMPTY
	// Which is the same as the smallest/largest integers J such that:
	//		0/1 - (Degree1+1-Inset1)/2 < 2*J + (Degree2+1+Inset2)			| 0/1 + (Degree1+1+Inset1)/2 > 2*J - (Degree2+1-Inset2)	
	// <=>	2*J > 0/1 - ( 2*Degree2 + Degree1 + 3 + 2*Inset2 - Inset1 ) / 2	| 2*J < 0/1 + ( 2*Degree2 + Degree1 + 3 - 2*Inset2 + Inset1 ) / 2
	BSPLINE_SET_BOUNDS( ParentOverlap0 , SMALLEST_INTEGER_LARGER_THAN_HALF( 0 - ( 2*Degree2 + Degree1 + 3 + 2*EData2::Inset - EData1::Inset ) / 2 ) , LARGEST_INTEGER_SMALLER_THAN_HALF( 0 + ( 2*Degree2 + Degree1 + 3 - 2*EData2::Inset + EData1::Inset ) / 2 ) );
	BSPLINE_SET_BOUNDS( ParentOverlap1 , SMALLEST_INTEGER_LARGER_THAN_HALF( 1 - ( 2*Degree2 + Degree1 + 3 + 2*EData2::Inset - EData1::Inset ) / 2 ) , LARGEST_INTEGER_SMALLER_THAN_HALF( 1 + ( 2*Degree2 + Degree1 + 3 - 2*EData2::Inset + EData1::Inset ) / 2 ) );
	static const int ParentOverlapStart[] , ParentOverlapEnd[] , ParentOverlapSize[];
};
template< int Degree1 , int Degree2 > const int BSplineOverlapSizes< Degree1 , Degree2 >::ParentOverlapStart[] = { ParentOverlap0Start , ParentOverlap1Start };
template< int Degree1 , int Degree2 > const int BSplineOverlapSizes< Degree1 , Degree2 >::ParentOverlapEnd  [] = { ParentOverlap0End   , ParentOverlap1End   };
template< int Degree1 , int Degree2 > const int BSplineOverlapSizes< Degree1 , Degree2 >::ParentOverlapSize [] = { ParentOverlap0Size  , ParentOverlap1Size  };

template< int Degree , BoundaryType BType >
class BSplineEvaluationData
{
public:
	static const int Pad = (BType==BOUNDARY_FREE ) ? BSplineSupportSizes< Degree >::SupportEnd : ( (Degree&1) && BType==BOUNDARY_DIRICHLET ) ? -1 : 0;
	inline static int Begin( int depth ){ return -Pad; }
	inline static int End  ( int depth ){ return (1<<depth) + (Degree&1) + Pad; }
	inline static bool OutOfBounds( int depth , int offset ){ return offset<Begin(depth) || offset>=End(depth); }

	static const int OffsetStart = -BSplineSupportSizes< Degree >::SupportStart , OffsetStop = BSplineSupportSizes< Degree >::SupportEnd + ( Degree&1 ) , IndexSize = OffsetStart + OffsetStop + 1 + 2 * Pad;
	static int OffsetToIndex( int depth , int offset )
	{
		int dim = BSplineSupportSizes< Degree >::Nodes( depth );
		if     ( offset<OffsetStart )     return Pad + offset;
		else if( offset>=dim-OffsetStop ) return Pad + OffsetStart + 1 + offset - ( dim-OffsetStop );
		else                              return Pad + OffsetStart;
	}
	static inline int IndexToOffset( int depth , int idx ){ return ( idx-Pad<=OffsetStart ? idx - Pad : ( BSplineSupportSizes< Degree >::Nodes(depth) + Pad - IndexSize + idx ) ); }

	BSplineEvaluationData( void );

	// [NOTE] The offset represents the node position, not the index of the function
	static double Value( int depth , int off , double s , bool derivative );

	// Note that this struct stores the components in left-to-right order
	struct BSplineComponents
	{
	protected:
		Polynomial< Degree > _polys[Degree+1];
	public:
		BSplineComponents( void ){ ; }
		BSplineComponents( int depth , int offset );
		const Polynomial< Degree >& operator[] ( int idx ) const { return _polys[idx]; }
		BSplineComponents derivative( void ) const;
		void printnl( void ) const { for( int d=0 ; d<=Degree ; d++ ) printf( "[%d] " , d ) , _polys[d].printnl(); }
	};
	struct BSplineUpSamplingCoefficients
	{
	protected:
		int _coefficients[ BSplineSupportSizes< Degree >::UpSampleSize ];
	public:
		BSplineUpSamplingCoefficients( void ){ ; }
		BSplineUpSamplingCoefficients( int depth , int offset );
		double operator[] ( int idx ){ return (double)_coefficients[idx] / (1<<Degree); }
	};

	struct CenterEvaluator
	{
		struct Evaluator
		{
		protected:
			friend BSplineEvaluationData;
			int _depth;
			double _ccValues[2][IndexSize][BSplineSupportSizes< Degree >::SupportSize];
		public:
#ifdef BRUNO_LEVY_FIX
			Evaluator( void ){ _depth = 0 ; memset( _ccValues , 0 , sizeof(_ccValues) ); }
#endif // BRUNO_LEVY_FIX
			double value( int fIdx , int cIdx , bool d ) const;
			int depth( void ) const { return _depth; }
		};
		struct ChildEvaluator
		{
		protected:
			friend BSplineEvaluationData;
			int _parentDepth;
			double _pcValues[2][IndexSize][BSplineSupportSizes< Degree >::ChildSupportSize];
		public:
#ifdef BRUNO_LEVY_FIX
			ChildEvaluator( void ){ _parentDepth = 0 ; memset( _pcValues , 0 , sizeof(_pcValues) ); }
#endif // BRUNO_LEVY_FIX
			double value( int fIdx , int cIdx , bool d ) const;
			int parentDepth( void ) const { return _parentDepth; }
			int childDepth( void ) const { return _parentDepth+1; }
		};
	};
	static void SetCenterEvaluator( typename CenterEvaluator::Evaluator& evaluator , int depth );
	static void SetChildCenterEvaluator( typename CenterEvaluator::ChildEvaluator& evaluator , int parentDepth );

	struct CornerEvaluator
	{
		struct Evaluator
		{
		protected:
			friend BSplineEvaluationData;
			int _depth;
			double _ccValues[2][IndexSize][BSplineSupportSizes< Degree >::CornerSize];
		public:
#ifdef BRUNO_LEVY_FIX
			Evaluator( void ){ _depth = 0 ; memset( _ccValues , 0 , sizeof( _ccValues ) ); }
#endif // BRUNO_LEVY_FIX
			double value( int fIdx , int cIdx , bool d ) const;
			int depth( void ) const { return _depth; }
		};
		struct ChildEvaluator
		{
		protected:
			friend BSplineEvaluationData;
			int _parentDepth;
			double _pcValues[2][IndexSize][BSplineSupportSizes< Degree >::ChildCornerSize];
		public:
#ifdef BRUNO_LEVY_FIX
			ChildEvaluator( void ){ _parentDepth = 0 ; memset( _pcValues , 0 , sizeof( _pcValues ) ); }
#endif // BRUNO_LEVY_FIX
			double value( int fIdx , int cIdx , bool d ) const;
			int parentDepth( void ) const { return _parentDepth; }
			int childDepth( void ) const { return _parentDepth+1; }
		};
	};
	static void SetCornerEvaluator( typename CornerEvaluator::Evaluator& evaluator , int depth );
	static void SetChildCornerEvaluator( typename CornerEvaluator::ChildEvaluator& evaluator , int parentDepth );

	struct Evaluator
	{
		typename CenterEvaluator::Evaluator centerEvaluator;
		typename CornerEvaluator::Evaluator cornerEvaluator;
		double centerValue( int fIdx , int cIdx , bool d ) const { return centerEvaluator.value( fIdx , cIdx , d ); }
		double cornerValue( int fIdx , int cIdx , bool d ) const { return cornerEvaluator.value( fIdx , cIdx , d ); }
	};
	static void SetEvaluator( Evaluator& evaluator , int depth ){ SetCenterEvaluator( evaluator.centerEvaluator , depth ) , SetCornerEvaluator( evaluator.cornerEvaluator , depth ); }

	struct ChildEvaluator
	{
		typename CenterEvaluator::ChildEvaluator centerEvaluator;
		typename CornerEvaluator::ChildEvaluator cornerEvaluator;
		double centerValue( int fIdx , int cIdx , bool d ) const { return centerEvaluator.value( fIdx , cIdx , d ); }
		double cornerValue( int fIdx , int cIdx , bool d ) const { return cornerEvaluator.value( fIdx , cIdx , d ); }
	};
	static void SetChildEvaluator( ChildEvaluator& evaluator , int depth ){ SetChildCenterEvaluator( evaluator.centerEvaluator , depth ) , SetChildCornerEvaluator( evaluator.cornerEvaluator , depth ); }

	struct UpSampleEvaluator
	{
	protected:
		friend BSplineEvaluationData;
		int _lowDepth;
		double _pcValues[IndexSize][BSplineSupportSizes< Degree >::UpSampleSize];
	public:
#ifdef BRUNO_LEVY_FIX
		UpSampleEvaluator( void ){ _lowDepth = 0 ; memset( _pcValues , 0 , sizeof( _pcValues ) ); }
#endif // BRUNO_LEVY_FIX
		double value( int pIdx , int cIdx ) const;
		int lowDepth( void ) const { return _lowDepth; }
	};
	static void SetUpSampleEvaluator( UpSampleEvaluator& evaluator , int lowDepth );
};

template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
class BSplineIntegrationData
{
public:
	static const int OffsetStart = - BSplineOverlapSizes< Degree1 , Degree2 >::OverlapSupportStart , OffsetStop = BSplineOverlapSizes< Degree1 , Degree2 >::OverlapSupportEnd + ( Degree1&1 ) , IndexSize = OffsetStart + OffsetStop + 1 + 2 * BSplineEvaluationData< Degree1 , BType1 >::Pad;
	static int OffsetToIndex( int depth , int offset )
	{
		int dim = BSplineSupportSizes< Degree1 >::Nodes( depth );
		if     ( offset<OffsetStart )     return BSplineEvaluationData< Degree1 , BType1 >::Pad + offset;
		else if( offset>=dim-OffsetStop ) return BSplineEvaluationData< Degree1 , BType1 >::Pad + OffsetStart + 1 + offset - ( dim-OffsetStop );
		else                              return BSplineEvaluationData< Degree1 , BType1 >::Pad + OffsetStart;
	}
	static inline int IndexToOffset( int depth , int idx ){ return ( idx-BSplineEvaluationData< Degree1 , BType1 >::Pad<=OffsetStart ? idx-BSplineEvaluationData< Degree1 , BType1 >::Pad : ( BSplineSupportSizes< Degree1 >::Nodes(depth) + BSplineEvaluationData< Degree1 , BType1 >::Pad - IndexSize + idx ) ); }

	template< unsigned int D1 , unsigned int D2 > static double Dot( int depth1 , int off1 , int depth2 , int off2 );
	// An index is interiorly overlapped if the support of its overlapping neighbors is in the range [0,1<<depth)
	inline static void InteriorOverlappedSpan( int depth , int& begin , int& end ){ begin = -BSplineOverlapSizes< Degree1 , Degree2 >::OverlapStart-BSplineSupportSizes< Degree2 >::SupportStart , end = (1<<depth)-BSplineOverlapSizes< Degree1 , Degree2 >::OverlapEnd-BSplineSupportSizes< Degree2 >::SupportEnd; }

	struct FunctionIntegrator
	{
		template< unsigned int D1 , unsigned int D2 >
		struct Integrator
		{
		protected:
			friend BSplineIntegrationData;
			int _depth;
			double _ccIntegrals[D1+1][D2+1][IndexSize][BSplineOverlapSizes< Degree1 , Degree2 >::OverlapSize];
		public:
#ifdef BRUNO_LEVY_FIX
			Integrator( void ){ _depth = 0 ; memset(_ccIntegrals, 0, sizeof(_ccIntegrals)); }
#endif // BRUNO_LEVY_FIX
			double dot( int fIdx1 , int fidx2 , int d1 , int d2 ) const;
			int depth( void ) const { return _depth; }
		};
		template< unsigned int D1 , unsigned int D2 >
		struct ChildIntegrator
		{
		protected:
			friend BSplineIntegrationData;
			int _parentDepth;
			double _pcIntegrals[D1+1][D2+1][IndexSize][BSplineOverlapSizes< Degree1 , Degree2 >::ChildOverlapSize];
		public:
#ifdef BRUNO_LEVY_FIX
			ChildIntegrator( void ){ _parentDepth = 0 ; memset( _pcIntegrals , 0 , sizeof( _pcIntegrals ) ); }
#endif // BRUNO_LEVY_FIX
			double dot( int fIdx1 , int fidx2 , int d1 , int d2 ) const;
			int parentDepth( void ) const { return _parentDepth; }
			int childDepth( void ) const { return _parentDepth+1; }
		};
	};
	// D1 and D2 indicate the number of derivatives that should be taken
	template< unsigned int D1 , unsigned int D2 >
	static void SetIntegrator( typename FunctionIntegrator::template Integrator< D1 , D2 >& integrator , int depth );
	template< unsigned int D1 , unsigned int D2 >
	static void SetChildIntegrator( typename FunctionIntegrator::template ChildIntegrator< D1 , D2 >& integrator , int parentDepth );

protected:
	// _D1 and _D2 indicate the total number of derivatives the integrator will be storing
	template< unsigned int D1 , unsigned int D2 , unsigned int _D1 , unsigned int _D2 >
	struct _IntegratorSetter
	{
		static void Set( typename FunctionIntegrator::template      Integrator< _D1 , _D2 >& integrator , int depth );
		static void Set( typename FunctionIntegrator::template ChildIntegrator< _D1 , _D2 >& integrator , int depth );
	};

	template< unsigned int D1 , unsigned int D2 , unsigned int _D1 , unsigned int _D2 , class Integrator >
	struct IntegratorSetter
	{
		static void Set2D( Integrator& integrator , int depth );
		static void Set1D( Integrator& integrator , int depth );
	};
	template< unsigned int D1 , unsigned int _D1 , unsigned int _D2 , class Integrator >
	struct IntegratorSetter< D1 , 0 , _D1 , _D2 , Integrator >
	{
		static void Set2D( Integrator& integrator , int  depth );
		static void Set1D( Integrator& integrator , int  depth );
	};
	template< unsigned int D2 , unsigned int _D1 , unsigned int _D2 , class Integrator >
	struct IntegratorSetter< 0 , D2 , _D1 , _D2 , Integrator >
	{
		static void Set2D( Integrator& integrator , int  depth );
		static void Set1D( Integrator& integrator , int  depth );
	};
	template< unsigned int _D1 , unsigned int _D2 , class Integrator >
	struct IntegratorSetter< 0 , 0 , _D1 , _D2 , Integrator >
	{
		static void Set2D( Integrator& integrator , int  depth );
		static void Set1D( Integrator& integrator , int  depth );
	};
};
#undef BSPLINE_SET_BOUNDS
#undef _FLOOR_OF_HALF
#undef  _CEIL_OF_HALF
#undef FLOOR_OF_HALF
#undef  CEIL_OF_HALF
#undef SMALLEST_INTEGER_LARGER_THAN_HALF
#undef LARGEST_INTEGER_SMALLER_THAN_HALF
#undef SMALLEST_INTEGER_LARGER_THAN_OR_EQUAL_TO_HALF
#undef LARGEST_INTEGER_SMALLER_THAN_OR_EQUAL_TO_HALF

template< int Degree , BoundaryType BType >
struct BSplineData
{
	inline static int TotalFunctionCount( int depth ){ return depth<0 ? 0 : (1<<(depth+1)) - 1 + (depth+1) * ( (Degree&1) + 2 * BSplineEvaluationData< Degree , BType >::Pad ); }
	inline static int FunctionIndex( int depth , int offset ){ return TotalFunctionCount( depth-1 ) + offset + BSplineEvaluationData< Degree , BType >::Pad; }
	inline static void FactorFunctionIndex( int idx , int& depth , int& offset )
	{
		int dim;
		depth = 0;
		while( idx>=( dim = BSplineEvaluationData< Degree , BType >::End( depth ) - BSplineEvaluationData< Degree , BType >::Begin( depth ) ) ) idx -= dim , depth++;
		offset = idx - BSplineEvaluationData< Degree , BType >::Pad;
	}
	inline static void FunctionSpan( int depth , int& fStart , int& fEnd ){ fStart = TotalFunctionCount( depth-1 ) , fEnd = TotalFunctionCount( depth ); }
	inline static int RemapOffset( int depth , int idx , bool& reflect );

	size_t functionCount;
	Pointer( typename BSplineEvaluationData< Degree , BType >::BSplineComponents )  baseBSplines;
	Pointer( typename BSplineEvaluationData< Degree , BType >::BSplineComponents ) dBaseBSplines;

	BSplineData( int maxDepth );
	~BSplineData( void );
};

template< int Degree1 , int Degree2 > void SetBSplineElementIntegrals( double integrals[Degree1+1][Degree2+1] );


#include "BSplineData.inl"
#endif // BSPLINE_DATA_INCLUDED