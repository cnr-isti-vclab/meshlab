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
// [COMMENTS]
// -- Throughout the code, should make a distinction between indices and offsets
// -- Make an instance of _evaluate that samples the finite-elements correctly (specifically, to handle the boundaries)
// -- Make functions like depthAndOffset parity dependent (ideally all "depth"s should be relative to the B-Slpline resolution
// -- Make all points relative to the unit-cube, regardless of degree parity
// -- It's possible that for odd degrees, the iso-surfacing will fail because the leaves in the SortedTreeNodes do not form a partition of space
// -- [MAYBE] Treat normal field as a sum of delta functions, rather than a smoothed signal (again, so that high degrees aren't forced to generate smooth reconstructions)
// -- [MAYBE] Make the degree of the B-Spline with which the normals are splatted independent of the degree of the FEM system. (This way, higher degree systems aren't forced to generate smoother normal fields.)
// -- [MAYBE] Remove the isValidFEM/isValidSpace functions since the octree supports all degrees/boundary types (up to the max degree for which finalizedBrooded... was called)

// [TODO]
// -- Currently, the implementation assumes that the boundary constraints are the same for vector fields and scalar fields
// -- Modify the setting of the flags so that only the subset of the broods that are needed 

#ifndef MULTI_GRID_OCTREE_DATA_INCLUDED
#define MULTI_GRID_OCTREE_DATA_INCLUDED

#define NEW_CODE
#define FAST_SET_UP				// If enabled, kernel density estimation is done aglomeratively

#define POINT_DATA_RES 0		// Specifies the resolution of the subgrid storing points with each voxel (0==1 but is faster)

#define DATA_DEGREE 1			// The order of the B-Spline used to splat in data for color interpolation
#define WEIGHT_DEGREE 2			// The order of the B-Spline used to splat in the weights for density estimation
#define NORMAL_DEGREE 2			// The order of the B-Spline used to splat int the normals for constructing the Laplacian constraints
//#define MAX_MEMORY_GB 15		// The maximum memory the application is allowed to use
#define MAX_MEMORY_GB 0

#include <unordered_map>
#ifdef _OPENMP
#include <omp.h>
#endif // _OPENMP
#include "BSplineData.h"
#include "PointStream.h"
#include "Geometry.h"
#include "Octree.h"
#include "SparseMatrix.h"

#ifndef _OPENMP
int omp_get_num_procs( void ){ return 1; }
int omp_get_thread_num( void ){ return 0; }
#endif // _OPENMP

#define DERIVATIVES( Degree ) ( ( Degree>1 ) ? 2 : ( Degree==1 ? 1 : 0 ) )

class TreeNodeData
{
public:
	enum
	{
		SPACE_FLAG = 1 ,
		FEM_FLAG = 2 ,
		GHOST_FLAG = 1<<7
	};
	int nodeIndex;
	char flags;

	void setGhostFlag( bool f ){ if( f ) flags |= GHOST_FLAG ; else flags &= ~GHOST_FLAG; }
	bool getGhostFlag( void ) const { return ( flags & GHOST_FLAG )!=0; }
	TreeNodeData( void );
	~TreeNodeData( void );
};

class VertexData
{
	typedef OctNode< TreeNodeData > TreeOctNode;
public:
	static const int VERTEX_COORDINATE_SHIFT = ( sizeof( long long ) * 8 ) / 3;
	static long long   EdgeIndex( const TreeOctNode* node , int eIndex , int maxDepth , int index[DIMENSION] );
	static long long   EdgeIndex( const TreeOctNode* node , int eIndex , int maxDepth );
	static long long   FaceIndex( const TreeOctNode* node , int fIndex , int maxDepth,int index[DIMENSION] );
	static long long   FaceIndex( const TreeOctNode* node , int fIndex , int maxDepth );
	static long long CornerIndex( const TreeOctNode* node , int cIndex , int maxDepth , int index[DIMENSION] );
	static long long CornerIndex( const TreeOctNode* node , int cIndex , int maxDepth );
	static long long CenterIndex( const TreeOctNode* node , int maxDepth , int index[DIMENSION] );
	static long long CenterIndex( const TreeOctNode* node , int maxDepth );
	static long long CornerIndex( int depth , const int offSet[DIMENSION] , int cIndex , int maxDepth , int index[DIMENSION] );
	static long long CenterIndex( int depth , const int offSet[DIMENSION] , int maxDepth , int index[DIMENSION] );
	static long long CornerIndexKey( const int index[DIMENSION] );
};

// This class stores the octree nodes, sorted by depth and then by z-slice.
// To support primal representations, the initializer takes a function that
// determines if a node should be included/indexed in the sorted list.
// [NOTE] Indexing of nodes is _GLOBAL_
class SortedTreeNodes
{
	typedef OctNode< TreeNodeData > TreeOctNode;
protected:
	Pointer( Pointer( int ) ) _sliceStart;
	int _levels;
public:
	Pointer( TreeOctNode* ) treeNodes;
	int begin( int depth ) const{ return _sliceStart[depth][0]; }
	int   end( int depth ) const{ return _sliceStart[depth][(size_t)1<<depth]; }
	int begin( int depth , int slice ) const{ return _sliceStart[depth][slice  ]  ; }
	int   end( int depth , int slice ) const{ if(depth<0||depth>=_levels||slice<0||slice>=(1<<depth)) printf( "uh oh\n" ) ; return _sliceStart[depth][slice+1]; }
	int size( void ) const { return _sliceStart[_levels-1][(size_t)1<<(_levels-1)]; }
	int size( int depth ) const { if(depth<0||depth>=_levels) printf( "uhoh\n" ); return _sliceStart[depth][(size_t)1<<depth] - _sliceStart[depth][0]; }
	int size( int depth , int slice ) const { return _sliceStart[depth][slice+1] - _sliceStart[depth][slice]; }
	int levels( void ) const { return _levels; }

	SortedTreeNodes( void );
	~SortedTreeNodes( void );
	void set( TreeOctNode& root , std::vector< int >* map );
	void set( TreeOctNode& root );

	template< int Indices >
	struct  _Indices
	{
		int idx[Indices];
		_Indices( void ){ memset( idx , -1 , sizeof( int ) * Indices ); }
		int& operator[] ( int i ) { return idx[i]; }
		const int& operator[] ( int i ) const { return idx[i]; }
	};
	typedef _Indices< Square::CORNERS > SquareCornerIndices;
	typedef _Indices< Square::EDGES > SquareEdgeIndices;
	typedef _Indices< Square::FACES > SquareFaceIndices;

	struct SliceTableData
	{
		Pointer( SquareCornerIndices ) cTable;
		Pointer( SquareEdgeIndices   ) eTable;
		Pointer( SquareFaceIndices   ) fTable;
		int cCount , eCount , fCount , nodeOffset , nodeCount;
		SliceTableData( void ){ fCount = eCount = cCount = 0 , cTable = NullPointer( SquareCornerIndices ) , eTable = NullPointer( SquareEdgeIndices ) , fTable = NullPointer( SquareFaceIndices ) , _cMap = _eMap = _fMap = NullPointer( int ); }
		~SliceTableData( void ){ clear(); }
#ifdef BRUNO_LEVY_FIX
		void clear( void ){ DeletePointer( cTable ) ; DeletePointer( eTable ) ; DeletePointer( fTable ) ; DeletePointer( _cMap ) ; DeletePointer( _eMap ) ; DeletePointer( _fMap ) ; fCount = eCount = cCount = 0; }
#else // !BRUNO_LEVY_FIX
		void clear( void ){ DeletePointer( cTable ) ; DeletePointer( eTable ) ; DeletePointer( fTable ) ; fCount = eCount = cCount = 0; }
#endif // BRUNO_LEVY_FIX
		SquareCornerIndices& cornerIndices( const TreeOctNode* node );
		SquareCornerIndices& cornerIndices( int idx );
		const SquareCornerIndices& cornerIndices( const TreeOctNode* node ) const;
		const SquareCornerIndices& cornerIndices( int idx ) const;
		SquareEdgeIndices& edgeIndices( const TreeOctNode* node );
		SquareEdgeIndices& edgeIndices( int idx );
		const SquareEdgeIndices& edgeIndices( const TreeOctNode* node ) const;
		const SquareEdgeIndices& edgeIndices( int idx ) const;
		SquareFaceIndices& faceIndices( const TreeOctNode* node );
		SquareFaceIndices& faceIndices( int idx );
		const SquareFaceIndices& faceIndices( const TreeOctNode* node ) const;
		const SquareFaceIndices& faceIndices( int idx ) const;
	protected:
		Pointer( int ) _cMap;
		Pointer( int ) _eMap;
		Pointer( int ) _fMap;
		friend class SortedTreeNodes;
	};
	struct XSliceTableData
	{
		Pointer( SquareCornerIndices ) eTable;
		Pointer( SquareEdgeIndices ) fTable;
		int fCount , eCount , nodeOffset , nodeCount;
		XSliceTableData( void ){ fCount = eCount = 0 , eTable = NullPointer( SquareCornerIndices ) , fTable = NullPointer( SquareEdgeIndices ) , _eMap = _fMap = NullPointer( int ); }
		~XSliceTableData( void ){ clear(); }
#ifdef BRUNO_LEVY_FIX
		void clear( void ) { DeletePointer( fTable ) ; DeletePointer( eTable ) ; DeletePointer( _eMap ) ; DeletePointer( _fMap ) ; fCount = eCount = 0; }
#else // !BRUNO_LEVY_FIX
		void clear( void ) { DeletePointer( fTable ) ; DeletePointer( eTable ) ; fCount = eCount = 0; }
#endif // BRUNO_LEVY_FIX
		SquareCornerIndices& edgeIndices( const TreeOctNode* node );
		SquareCornerIndices& edgeIndices( int idx );
		const SquareCornerIndices& edgeIndices( const TreeOctNode* node ) const;
		const SquareCornerIndices& edgeIndices( int idx ) const;
		SquareEdgeIndices& faceIndices( const TreeOctNode* node );
		SquareEdgeIndices& faceIndices( int idx );
		const SquareEdgeIndices& faceIndices( const TreeOctNode* node ) const;
		const SquareEdgeIndices& faceIndices( int idx ) const;
	protected:
		Pointer( int ) _eMap;
		Pointer( int ) _fMap;
		friend class SortedTreeNodes;
	};
	void setSliceTableData (  SliceTableData& sData , int depth , int offset , int threads ) const;
	void setXSliceTableData( XSliceTableData& sData , int depth , int offset , int threads ) const;
};

template< int Degree >
struct PointSupportKey : public OctNode< TreeNodeData >::NeighborKey< BSplineSupportSizes< Degree >::SupportEnd , -BSplineSupportSizes< Degree >::SupportStart >
{
	static const int LeftRadius  =  BSplineSupportSizes< Degree >::SupportEnd;
	static const int RightRadius = -BSplineSupportSizes< Degree >::SupportStart;
	static const int Size = LeftRadius + RightRadius + 1;
};
template< int Degree >
struct ConstPointSupportKey : public OctNode< TreeNodeData >::ConstNeighborKey< BSplineSupportSizes< Degree >::SupportEnd , -BSplineSupportSizes< Degree >::SupportStart >
{
	static const int LeftRadius  =  BSplineSupportSizes< Degree >::SupportEnd;
	static const int RightRadius = -BSplineSupportSizes< Degree >::SupportStart;
	static const int Size = LeftRadius + RightRadius + 1;
};

template< class Real , bool HasGradients >
struct SinglePointData
{
	Point3D< Real > position;
	Real weight;
	Real value , _value;
	SinglePointData  operator +  ( const SinglePointData& p ) const { return SinglePointData( position + p.position , value + p.value , weight + p.weight ); }
	SinglePointData& operator += ( const SinglePointData& p ){ position += p.position ; weight += p.weight , value += p.value ; return *this; }
	SinglePointData  operator *  ( Real s ) const { return SinglePointData( position*s , weight*s , value*s ); }
	SinglePointData& operator *= ( Real s ){ position *= s , weight *= s , value *= s ; return *this; }
	SinglePointData  operator /  ( Real s ) const { return SinglePointData( position/s , weight/s , value/s ); }
	SinglePointData& operator /= ( Real s ){ position /= s , weight /= s , value /= s ; return *this; }
	SinglePointData( void ) : position( Point3D< Real >() ) , weight(0) , value(0) , _value(0) { ; }
	SinglePointData( Point3D< Real > p , Real v , Real w ) { position = p , value = v , weight = w , _value = (Real)0; }
};
template< class Real >
struct SinglePointData< Real , true > : public SinglePointData< Real , false >
{
	using SinglePointData< Real , false >::position;
	using SinglePointData< Real , false >::weight;
	using SinglePointData< Real , false >::value;
	using SinglePointData< Real , false >::_value;
	Point3D< Real > gradient , _gradient;
	SinglePointData  operator +  ( const SinglePointData& p ) const { return SinglePointData( position + p.position , weight + p.weight , value + p.value , gradient + p.gradient ); }
	SinglePointData& operator += ( const SinglePointData& p ){ position += p.position , weight += p.weight , value += p.value , gradient += p.gradient ; return *this; }
	SinglePointData  operator *  ( Real s ) const { return SinglePointData( position*s , weight*s , value*s , gradient*s ); }
	SinglePointData& operator *= ( Real s ){ position *= s , weight *= s , value *= s , gradient *= s ; return *this; }
	SinglePointData  operator /  ( Real s ) const { return SinglePointData( position/s , weight/s , value/s , gradient/s ); }
	SinglePointData& operator /= ( Real s ){ position /= s , weight /= s , value /= s , gradient /= s ; return *this; }
	SinglePointData( void ) : SinglePointData< Real , false >() , gradient( Point3D< Real >() ) , _gradient( Point3D< Real >() ) { ; }
	SinglePointData( Point3D< Real > p , Real v , Point3D< Real > g , Real w ) : SinglePointData< Real , false >( p , v , w ) { gradient = g , _gradient = Point3D< Real >(); }
};

#if POINT_DATA_RES
template< class Real , bool HasGradients >
struct PointData
{
	static const int RES = POINT_DATA_RES;
	static const int SAMPLES = RES * RES * RES;

	SinglePointData< Real , HasGradients > points[SAMPLES];
	SinglePointData< Real , HasGradients >& operator[] ( int idx ) { return points[idx]; }
	const SinglePointData< Real , HasGradients >& operator[] ( int idx ) const { return points[idx]; }

	static void SetIndices( Point3D< Real > p , Point3D< Real > c , Real w , int x[3] )
	{
		for( int d=0 ; d<3 ; d++ ) x[d] = std::max< int >( 0 , std::min< int >( RES-1 , int( floor( ( p[d]-( c[d]-w/2 ) ) / w * RES ) ) ) );
	}

	void addPoint( SinglePointData< Real , HasGradients > p , Point3D< Real > center , Real width  )
	{
		int x[3];
		SetIndices( p.position , center , width , x );
		points[ x[0]+x[1]*RES+x[2]*RES*RES ] += p;
	}

	PointData  operator +  ( const PointData& p ) const { PointData _p ; for( int c=0 ; c<SAMPLES ;  c++ ) _p.points[c] = points[c] + _p.points[c] ; return _p; }
	PointData& operator += ( const PointData& p ){ for( int c=0 ; c<SAMPLES ; c++ ) points[c] += p.points[c] ; return *this; }
	PointData  operator *  ( Real s ) const { PointData _p ; for( int c=0 ; c<SAMPLES ;  c++ ) _p.points[c] = points[c] * s ; return _p; }
	PointData& operator *= ( Real s ){ for( int c=0 ; c<SAMPLES ; c++ ) points[c] *= s ; return *this; }
	PointData  operator /  ( Real s ) const { PointData _p ; for( int c=0 ; c<SAMPLES ;  c++ ) _p.points[c] = points[c] / s ; return _p; }
	PointData& operator /= ( Real s ){ for( int c=0 ; c<SAMPLES ; c++ ) points[c] /= s ; return *this; }
};
#else // !POINT_DATA_RES
template< class Real , bool HasGradients > using PointData = SinglePointData< Real , HasGradients >;
#endif // POINT_DATA_RES

template< class Data , int Degree >
struct SparseNodeData
{
	size_t size( void ) const { return _data.size(); }
	const Data& operator[] ( int idx ) const { return _data[idx]; }
	Data& operator[] ( int idx ) { return _data[idx]; }
	void reserve( size_t sz ){ if( sz>_indices.size() ) _indices.resize( sz , -1 ); }
	Data* operator()( const OctNode< TreeNodeData >* node ){ return ( node->nodeData.nodeIndex<0 || node->nodeData.nodeIndex>=(int)_indices.size() || _indices[ node->nodeData.nodeIndex ]<0 ) ? NULL : &_data[ _indices[ node->nodeData.nodeIndex ] ]; }
	const Data* operator()( const OctNode< TreeNodeData >* node ) const { return ( node->nodeData.nodeIndex<0 || node->nodeData.nodeIndex>=(int)_indices.size() || _indices[ node->nodeData.nodeIndex ]<0 ) ? NULL : &_data[ _indices[ node->nodeData.nodeIndex ] ]; }
	Data& operator[]( const OctNode< TreeNodeData >* node )
	{
		if( node->nodeData.nodeIndex>=(int)_indices.size() ) _indices.resize( node->nodeData.nodeIndex+1 , -1 );
		if( _indices[ node->nodeData.nodeIndex ]==-1 )
		{
			_indices[ node->nodeData.nodeIndex ] = (int)_data.size();
			_data.push_back( Data() );
		}
		return _data[ _indices[ node->nodeData.nodeIndex ] ];
	}
	void remapIndices( const std::vector< int >& map )
	{
		std::vector< int > temp = _indices;
		_indices.resize( map.size() );
		for( size_t i=0 ; i<map.size() ; i++ )
			if( map[i]<(int)temp.size() ) _indices[i] = temp[ map[i] ];
			else                          _indices[i] = -1;
	}
	template< class _Data , int _Degree > friend struct SparseNodeData;
	template< class _Data , int _Degree >
	void init( const SparseNodeData< _Data , _Degree >& snd ){ _indices = snd._indices , _data.resize( snd._data.size() ); }
	void remove( const OctNode< TreeNodeData >* node ){ if( node->nodeData.nodeIndex<(int)_indices.size() && node->nodeData.nodeIndex>=0 ) _indices[ node->nodeData.nodeIndex ] = -1; }
protected:
	std::vector< int > _indices;
	std::vector< Data > _data;
};
template< class Data , int Degree >
struct DenseNodeData
{
	DenseNodeData( void ){ _data = NullPointer( Data ) ; _sz = 0; }
	DenseNodeData( size_t sz ){ _sz = sz ; if( sz ) _data = NewPointer< Data >( sz ) ; else _data = NullPointer( Data ); }
	DenseNodeData( const DenseNodeData&  d ) : DenseNodeData() { _resize( d._sz ) ; if( _sz ) memcpy( _data , d._data , sizeof(Data) * _sz ); }
	DenseNodeData(       DenseNodeData&& d ){ _data = d._data , _sz = d._sz ; d._data = NullPointer( Data ) , d._sz = 0; }
	DenseNodeData& operator = ( const DenseNodeData&  d ){ _resize( d._sz ) ; if( _sz ) memcpy( _data , d._data , sizeof(Data) * _sz ) ; return *this; }
	DenseNodeData& operator = (       DenseNodeData&& d ){ size_t __sz = _sz ; Pointer( Data ) __data = _data ; _data = d._data , _sz = d._sz ; d._data = __data , d._sz = __sz ; return *this; }
	~DenseNodeData( void ){ DeletePointer( _data ) ; _sz = 0; }

	Data& operator[] ( int idx ) { return _data[idx]; }
	const Data& operator[] ( int idx ) const { return _data[idx]; }
	size_t size( void ) const { return _sz; }
	Data& operator[]( const OctNode< TreeNodeData >* node ) { return _data[ node->nodeData.nodeIndex ]; }
	Data* operator()( const OctNode< TreeNodeData >* node ) { return ( node==NULL || node->nodeData.nodeIndex>=(int)_sz ) ? NULL : &_data[ node->nodeData.nodeIndex ]; }
	const Data* operator()( const OctNode< TreeNodeData >* node ) const { return ( node==NULL || node->nodeData.nodeIndex>=(int)_sz ) ? NULL : &_data[ node->nodeData.nodeIndex ]; }
	int index( const OctNode< TreeNodeData >* node ) const { return ( !node || node->nodeData.nodeIndex<0 || node->nodeData.nodeIndex>=(int)this->_data.size() ) ? -1 : node->nodeData.nodeIndex; }
protected:
	size_t _sz;
	void _resize( size_t sz ){ DeletePointer( _data ) ; if( sz ) _data = NewPointer< Data >( sz ) ; else _data = NullPointer( Data ) ; _sz = sz; }
	Pointer( Data ) _data;
};

// This is may be necessary in case the memory usage is larger than what fits on the stack
template< class C , int N > struct Stencil
{
	Stencil( void ){ _values = NewPointer< C >( N * N * N ); }
	~Stencil( void ){ DeletePointer( _values ); }
	C& operator()( int i , int j , int k ){ return _values[ i*N*N + j*N + k ]; }
	const C& operator()( int i , int j , int k ) const { return _values[ i*N*N + j*N + k ]; }
protected:
	Pointer( C ) _values;
};

template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 >
class SystemCoefficients
{
	typedef typename BSplineIntegrationData< Degree1 , BType1 , Degree2 , BType2 >::FunctionIntegrator FunctionIntegrator;
	static const int OverlapSize  = BSplineOverlapSizes< Degree1 , Degree2 >::OverlapSize;
	static const int OverlapStart = BSplineOverlapSizes< Degree1 , Degree2 >::OverlapStart;
	static const int OverlapEnd   = BSplineOverlapSizes< Degree1 , Degree2 >::OverlapEnd;
public:
	typedef typename BSplineIntegrationData< Degree1 , BType1 , Degree2 , BType2 >::FunctionIntegrator::template      Integrator< DERIVATIVES( Degree1 ) , DERIVATIVES( Degree2 ) >      Integrator;
	typedef typename BSplineIntegrationData< Degree1 , BType1 , Degree2 , BType2 >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( Degree1 ) , DERIVATIVES( Degree2 ) > ChildIntegrator;

	// The FEMSystemFunctor is a class that takes an object of type Integrator/ChildIntegrator, as well as a pair of indices of octree nodes
	// and returns the corresponding system coefficient.
	template< class _FEMSystemFunctor > static void SetCentralSystemStencil ( const _FEMSystemFunctor& F , const      Integrator& integrator , Stencil< double , OverlapSize >& stencil           );
	template< class _FEMSystemFunctor > static void SetCentralSystemStencils( const _FEMSystemFunctor& F , const ChildIntegrator& integrator , Stencil< double , OverlapSize >  stencils[2][2][2] );
	template< bool Reverse , class _FEMSystemFunctor > static void SetCentralConstraintStencil ( const _FEMSystemFunctor& F , const      Integrator& integrator , Stencil<          double   , OverlapSize >& stencil           );
	template< bool Reverse , class _FEMSystemFunctor > static void SetCentralConstraintStencils( const _FEMSystemFunctor& F , const ChildIntegrator& integrator , Stencil<          double   , OverlapSize >  stencils[2][2][2] );
	template< bool Reverse , class _FEMSystemFunctor > static void SetCentralConstraintStencil ( const _FEMSystemFunctor& F , const      Integrator& integrator , Stencil< Point3D< double > , OverlapSize >& stencil           );
	template< bool Reverse , class _FEMSystemFunctor > static void SetCentralConstraintStencils( const _FEMSystemFunctor& F , const ChildIntegrator& integrator , Stencil< Point3D< double > , OverlapSize >  stencils[2][2][2] );
};

template< int FEMDegree , BoundaryType BType >
struct FEMSystemFunctor
{
	double massWeight , lapWeight , biLapWeight;
	FEMSystemFunctor( double mWeight=0 , double lWeight=0 , double bWeight=0 ) : massWeight( mWeight ) , lapWeight( lWeight ) , biLapWeight( bWeight ) { ; }
	double integrate( const typename SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::     Integrator& integrator , const int off1[] , const int off2[] ) const { return _integrate( integrator , off1 , off2 ); }
	double integrate( const typename SystemCoefficients< FEMDegree , BType , FEMDegree , BType >::ChildIntegrator& integrator , const int off1[] , const int off2[] ) const { return _integrate( integrator , off1 , off2 ); }
	bool vanishesOnConstants( void ) const { return massWeight==0; }
protected:
	template< class I > double _integrate( const I& integrator , const int off1[] , const int off2[] ) const;
};
template< int SFDegree , BoundaryType SFBType , int FEMDegree , BoundaryType FEMBType >
struct FEMSFConstraintFunctor
{
	double massWeight , lapWeight , biLapWeight;
	FEMSFConstraintFunctor( double mWeight=0 , double lWeight=0 , double bWeight=0 ) : massWeight( mWeight ) , lapWeight( lWeight ) , biLapWeight( bWeight ) { ; }
	template< bool Reverse >
	double integrate( const typename SystemCoefficients< Reverse ? FEMDegree : SFDegree , Reverse ? FEMBType : SFBType , Reverse ? SFDegree : FEMDegree , Reverse ? SFBType : FEMBType >::     Integrator& integrator , const int off1[] , const int off2[] ) const { return _integrate< Reverse >( integrator , off1 , off2 ); }
	template< bool Reverse >
	double integrate( const typename SystemCoefficients< Reverse ? FEMDegree : SFDegree , Reverse ? FEMBType : SFBType , Reverse ? SFDegree : FEMDegree , Reverse ? SFBType : FEMBType >::ChildIntegrator& integrator , const int off1[] , const int off2[] ) const { return _integrate< Reverse >( integrator , off1 , off2 ); }
protected:
	template< bool Reverse , class I > double _integrate( const I& integrator , const int off1[] , const int off[2] ) const;
};
template< int VFDegree , BoundaryType VFBType , int FEMDegree , BoundaryType FEMBType >
struct FEMVFConstraintFunctor
{
	double lapWeight , biLapWeight;
	FEMVFConstraintFunctor( double lWeight=0 , double bWeight=0 ) : lapWeight( lWeight ) , biLapWeight( bWeight ) { ; }
	template< bool Reverse >
	Point3D< double > integrate( const typename SystemCoefficients< Reverse ? FEMDegree : VFDegree , Reverse ? FEMBType : VFBType , Reverse ? VFDegree : FEMDegree , Reverse ? VFBType : FEMBType >::     Integrator& integrator , const int off1[] , const int off2[] ) const { return _integrate< Reverse >( integrator , off1 , off2 ); }
	template< bool Reverse >
	Point3D< double > integrate( const typename SystemCoefficients< Reverse ? FEMDegree : VFDegree , Reverse ? FEMBType : VFBType , Reverse ? VFDegree : FEMDegree , Reverse ? VFBType : FEMBType >::ChildIntegrator& integrator , const int off1[] , const int off2[] ) const { return _integrate< Reverse >( integrator , off1 , off2 ); }
protected:
	template< bool Reverse , class I > Point3D< double > _integrate( const I& integrator , const int off1[] , const int off[2] ) const;
};

inline void SetGhostFlag( OctNode< TreeNodeData >* node , bool flag ){ if( node && node->parent ) node->parent->nodeData.setGhostFlag( flag ); }
inline bool GetGhostFlag( const OctNode< TreeNodeData >* node ){ return node==NULL || node->parent==NULL || node->parent->nodeData.getGhostFlag( ); }
inline bool IsActiveNode( const OctNode< TreeNodeData >* node ){ return !GetGhostFlag( node ); }

template< class Real >
class Octree
{
	typedef OctNode< TreeNodeData > TreeOctNode;
	static int _NodeCount;
	static void _NodeInitializer( TreeOctNode& node ){ node.nodeData.nodeIndex = _NodeCount++; }
public:
#if 0
	struct LocalDepth
	{
		LocalDepth( int d=0 ) : _d(d) { ; }
		operator int&()       { return _d; }
		operator int () const { return _d; }
	protected:
		int _d;
	};
	struct LocalOffset
	{
		LocalOffset( const int* off=NULL ){ if( off ) memcpy( _off , off , sizeof(_off) ) ; else memset( _off , 0 , sizeof( _off ) ); }
		operator        int*()       { return _off; }
		operator const  int*() const { return _off; }
	protected:
		int _off[3];
	};
#else
	typedef int LocalDepth;
	typedef int LocalOffset[3];
#endif

	static void ResetNodeCount( void ){ _NodeCount = 0 ; }
	static int NodeCount( void ){ return _NodeCount; }
	template< int FEMDegree , BoundaryType BType > void functionIndex( const TreeOctNode* node , int idx[3] ) const;

	struct PointSample{ const TreeOctNode* node ; ProjectiveData< OrientedPoint3D< Real > , Real > sample; };

	typedef typename TreeOctNode::     NeighborKey< 1 , 1 >      AdjacenctNodeKey;
	typedef typename TreeOctNode::ConstNeighborKey< 1 , 1 > ConstAdjacenctNodeKey;

	template< int FEMDegree , BoundaryType BType > bool isValidFEMNode( const TreeOctNode* node ) const;
	bool isValidSpaceNode( const TreeOctNode* node ) const;
	TreeOctNode* leaf( Point3D< Real > p );
	const TreeOctNode* leaf( Point3D< Real > p ) const;

	template< bool HasGradients >
	struct InterpolationInfo
	{
		SparseNodeData< PointData< Real , HasGradients > , 0 > iData;
		Real valueWeight , gradientWeight;
		InterpolationInfo( const class Octree< Real >& tree , const std::vector< PointSample >& samples , Real pointValue , int adaptiveExponent , Real v , Real g ) : valueWeight(v) , gradientWeight(g)
		{ iData = tree._densifyInterpolationInfo< HasGradients >( samples , pointValue , adaptiveExponent ); }
		PointData< Real , HasGradients >* operator()( const OctNode< TreeNodeData >* node ){ return iData(node); }
		const PointData< Real , HasGradients >* operator()( const OctNode< TreeNodeData >* node ) const { return iData(node); }
	};

	template< int DensityDegree > struct DensityEstimator : public SparseNodeData< Real , DensityDegree >
	{
		DensityEstimator( int kernelDepth ) : _kernelDepth( kernelDepth ){ ; }
		int kernelDepth( void ) const { return _kernelDepth; }
	protected:
		int _kernelDepth;
	};
protected:
	bool _isValidSpaceNode( const TreeOctNode* node ) const { return !GetGhostFlag( node ) && ( node->nodeData.flags & TreeNodeData::SPACE_FLAG ); }
	bool _isValidFEMNode( const TreeOctNode* node ) const { return !GetGhostFlag( node ) && ( node->nodeData.flags & TreeNodeData::FEM_FLAG ); }

	TreeOctNode* _tree;
	TreeOctNode* _spaceRoot;
	SortedTreeNodes _sNodes;
	LocalDepth _fullDepth , _maxDepth;

	static bool _InBounds( Point3D< Real > p );

	int _depthOffset;
	int _localToGlobal( LocalDepth d ) const { return d + _depthOffset; }
	LocalDepth _localDepth( const TreeOctNode* node ) const { return node->depth() - _depthOffset; }
	LocalDepth _localMaxDepth( const TreeOctNode* tree ) const { return tree->maxDepth() - _depthOffset; }
	int _localInset( LocalDepth d ) const { return _depthOffset<=1 ? 0 : 1<<( d + _depthOffset - 1 ); }
	void _localDepthAndOffset( const TreeOctNode* node , LocalDepth& d , LocalOffset& off ) const
	{
		node->depthAndOffset( d , off ) ; d -= _depthOffset;
		int inset = _localInset( d );
		off[0] -= inset , off[1] -= inset , off[2] -= inset;
	}
	template< int FEMDegree , BoundaryType BType > static int _BSplineBegin( LocalDepth depth ){ return BSplineEvaluationData< FEMDegree , BType >::Begin( depth ); }
	template< int FEMDegree , BoundaryType BType > static int _BSplineEnd  ( LocalDepth depth ){ return BSplineEvaluationData< FEMDegree , BType >::End  ( depth ); }
	template< int FEMDegree , BoundaryType BType >
	bool _outOfBounds( const TreeOctNode* node ) const
	{
		if( !node ) return true;
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( node , d , off );
		return d<0 || BSplineEvaluationData< FEMDegree , BType >::OutOfBounds( d , off[0] ) || BSplineEvaluationData< FEMDegree , BType >::OutOfBounds( d , off[1] ) || BSplineEvaluationData< FEMDegree , BType >::OutOfBounds( d , off[2] );
	}
	int _sNodesBegin( LocalDepth d ) const { return _sNodes.begin( _localToGlobal( d ) ); }
	int _sNodesEnd  ( LocalDepth d ) const { return _sNodes.end  ( _localToGlobal( d ) ); }
	int _sNodesSize ( LocalDepth d ) const { return _sNodes.size ( _localToGlobal( d ) ); }
	int _sNodesBegin( LocalDepth d , int slice ) const { return _sNodes.begin( _localToGlobal( d ) , slice + _localInset( d ) ); }
	int _sNodesEnd  ( LocalDepth d , int slice ) const { return _sNodes.end  ( _localToGlobal( d ) , slice + _localInset( d ) ); }
	int _sNodesSize ( LocalDepth d , int slice ) const { return _sNodes.size ( _localToGlobal( d ) , slice + _localInset( d ) ); }

	template< int FEMDegree > static bool _IsInteriorlySupported( LocalDepth depth , const LocalOffset off )
	{
		if( depth>=0 )
		{
			int begin , end;
			BSplineSupportSizes< FEMDegree >::InteriorSupportedSpan( depth , begin , end );
			return ( off[0]>=begin && off[0]<end && off[1]>=begin && off[1]<end && off[2]>=begin && off[2]<end );
		}
		else return false;
	}
	template< int FEMDegree > bool _isInteriorlySupported( const TreeOctNode* node ) const
	{
		if( !node ) return false;
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( node , d , off );
		return _IsInteriorlySupported< FEMDegree >( d , off );
	}
	template< int FEMDegree1 , int FEMDegree2 > static bool _IsInteriorlyOverlapped( LocalDepth depth , const LocalOffset off )
	{
		if( depth>=0 )
		{
			int begin , end;
			BSplineIntegrationData< FEMDegree1 , BOUNDARY_NEUMANN , FEMDegree2 , BOUNDARY_NEUMANN >::InteriorOverlappedSpan( depth , begin , end );
			return ( off[0]>=begin && off[0]<end && off[1]>=begin && off[1]<end && off[2]>=begin && off[2]<end );
		}
		else return false;
	}
	template< int FEMDegree1 , int FEMDegree2 > bool _isInteriorlyOverlapped( const TreeOctNode* node ) const
	{
		if( !node ) return false;
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( node , d , off );
		return _IsInteriorlyOverlapped< FEMDegree1 , FEMDegree2 >( d , off );
	}
	void _startAndWidth( const TreeOctNode* node , Point3D< Real >& start , Real& width ) const
	{
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( node , d , off );
		if( d>=0 ) width = Real( 1.0 / (1<<  d ) );
		else       width = Real( 1.0 * (1<<(-d)) );
		for( int dd=0 ; dd<DIMENSION ; dd++ ) start[dd] = Real( off[dd] ) * width;
	}
	void _centerAndWidth( const TreeOctNode* node , Point3D< Real >& center , Real& width ) const
	{
		int d , off[3];
		_localDepthAndOffset( node , d , off );
		width = Real( 1.0 / (1<<d) );
		for( int dd=0 ; dd<DIMENSION ; dd++ ) center[dd] = Real( off[dd] + 0.5 ) * width;
	}
	int _childIndex( const TreeOctNode* node , Point3D< Real > p ) const
	{
		Point3D< Real > c ; Real w;
		_centerAndWidth( node , c , w );
		return ( p[0]<c[0] ? 0 : 1 ) | ( p[1]<c[1] ? 0 : 2 ) | ( p[2]<c[2] ? 0 : 4 );
	}

	template< int Degree , BoundaryType BType > void _setFullDepth( TreeOctNode* node , LocalDepth depth ) const;
	template< int Degree , BoundaryType BType > void _setFullDepth( LocalDepth depth );

	template< int LeftRadius , int RightRadius >
	static typename TreeOctNode::ConstNeighbors< LeftRadius + RightRadius + 1 >& _neighbors( TreeOctNode::ConstNeighborKey< LeftRadius , RightRadius >& key , const TreeOctNode* node ){ return key.neighbors[ node->depth() ]; }
	template< int LeftRadius , int RightRadius >
	static typename TreeOctNode::Neighbors< LeftRadius + RightRadius + 1 >& _neighbors( TreeOctNode::NeighborKey< LeftRadius , RightRadius >& key , const TreeOctNode* node ){ return key.neighbors[ node->depth() ]; }
	template< int LeftRadius , int RightRadius >
	static const typename TreeOctNode::template Neighbors< LeftRadius + RightRadius + 1 >& _neighbors( const typename TreeOctNode::template NeighborKey< LeftRadius , RightRadius >& key , const TreeOctNode* node ){ return key.neighbors[ node->depth() ]; }
	template< int LeftRadius , int RightRadius >
	static const typename TreeOctNode::template ConstNeighbors< LeftRadius + RightRadius + 1 >& _neighbors( const typename TreeOctNode::template ConstNeighborKey< LeftRadius , RightRadius >& key , const TreeOctNode* node ){ return key.neighbors[ node->depth() ]; }

public:
	LocalDepth depth( const TreeOctNode* node ) const { return _localDepth( node ); }
	void depthAndOffset( const TreeOctNode* node , LocalDepth& depth , LocalOffset& offset ) const { _localDepthAndOffset( node , depth , offset ); }

	int nodesBegin( LocalDepth d ) const { return _sNodes.begin( _localToGlobal( d ) ); }
	int nodesEnd  ( LocalDepth d ) const { return _sNodes.end  ( _localToGlobal( d ) ); }
	int nodesSize ( LocalDepth d ) const { return _sNodes.size ( _localToGlobal( d ) ); }
	int nodesBegin( LocalDepth d , int slice ) const { return _sNodes.begin( _localToGlobal( d ) , slice + _localInset( d ) ); }
	int nodesEnd  ( LocalDepth d , int slice ) const { return _sNodes.end  ( _localToGlobal( d ) , slice + _localInset( d ) ); }
	int nodesSize ( LocalDepth d , int slice ) const { return _sNodes.size ( _localToGlobal( d ) , slice + _localInset( d ) ); }
	const TreeOctNode* node( int idx ) const { return _sNodes.treeNodes[idx]; }
protected:

	////////////////////////////////////
	// System construction code       //
	// MultiGridOctreeData.System.inl //
	////////////////////////////////////
	template< int FEMDegree >
	void _setMultiColorIndices( int start , int end , std::vector< std::vector< int > >& indices ) const;
	struct _SolverStats
	{
		double evaluateTime , systemTime , solveTime;
		double bNorm2 , inRNorm2 , outRNorm2;
	};
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	int _solveSystemGS( const FEMSystemFunctor& F , const BSplineData< FEMDegree , BType >& bsData , InterpolationInfo< HasGradients >* interpolationInfo , LocalDepth depth , DenseNodeData< Real , FEMDegree >& solution , DenseNodeData< Real , FEMDegree >& constraints , DenseNodeData< Real , FEMDegree >& metSolutionConstraints , int iters , bool coarseToFine , _SolverStats& stats , bool computeNorms );
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	int _solveSystemCG( const FEMSystemFunctor& F , const BSplineData< FEMDegree , BType >& bsData , InterpolationInfo< HasGradients >* interpolationInfo , LocalDepth depth , DenseNodeData< Real , FEMDegree >& solution , DenseNodeData< Real , FEMDegree >& constraints , DenseNodeData< Real , FEMDegree >& metSolutionConstraints , int iters , bool coarseToFine , _SolverStats& stats , bool computeNorms , double accuracy );
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	int _setMatrixRow( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& neighbors , Pointer( MatrixEntry< Real > ) row , int offset , const typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& integrator , const Stencil< double , BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& stencil , const BSplineData< FEMDegree , BType >& bsData ) const;
	template< int FEMDegree , BoundaryType BType >
	int _getMatrixRowSize( const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& neighbors ) const;

	template< int FEMDegree1 , int FEMDegree2 > static void _SetParentOverlapBounds( const TreeOctNode* node , int& startX , int& endX , int& startY , int& endY , int& startZ , int& endZ );
	// Updates the constraints @(depth) based on the solution coefficients @(depth-1)

	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	void _updateConstraintsFromCoarser( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& neighbors , const typename TreeOctNode::Neighbors< BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& pNeighbors , TreeOctNode* node , DenseNodeData< Real , FEMDegree >& constraints , const DenseNodeData< Real , FEMDegree >& metSolution , const typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& childIntegrator , const Stencil< double , BSplineOverlapSizes< FEMDegree , FEMDegree >::OverlapSize >& stencil , const BSplineData< FEMDegree , BType >& bsData ) const;

	// evaluate the points @(depth) using coefficients @(depth-1)
	template< int FEMDegree , BoundaryType BType , bool HasGradients >
	void _setPointValuesFromCoarser( InterpolationInfo< HasGradients >& interpolationInfo , LocalDepth highDepth , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& upSampledCoefficients );

	// Updates the cumulative integral constraints @(depth-1) based on the change in solution coefficients @(depth)
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor >
	void _updateCumulativeIntegralConstraintsFromFiner( const FEMSystemFunctor& F , 
		const BSplineData< FEMDegree , BType >& bsData , LocalDepth highDepth , const DenseNodeData< Real , FEMDegree >& fineSolution , DenseNodeData< Real , FEMDegree >& cumulativeConstraints ) const;
	// Updates the cumulative interpolation constraints @(depth-1) based on the change in solution coefficient @(depth)
	template< int FEMDegree , BoundaryType BType , bool HasGradients >
	void _updateCumulativeInterpolationConstraintsFromFiner( const InterpolationInfo< HasGradients >& interpolationInfo ,
		const BSplineData< FEMDegree , BType >& bsData , LocalDepth highDepth , const DenseNodeData< Real , FEMDegree >& fineSolution , DenseNodeData< Real , FEMDegree >& cumulativeConstraints ) const;

	template< int FEMDegree , BoundaryType BType >
	Real _coarserFunctionValue( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& upSampledCoefficients ) const;
	template< int FEMDegree , BoundaryType BType >
	Point3D< Real > _coarserFunctionGradient( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& upSampledCoefficients ) const;
	template< int FEMDegree , BoundaryType BType >
	Real   _finerFunctionValue( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& coefficients ) const;
	template< int FEMDegree , BoundaryType BType >
	Point3D< Real >   _finerFunctionGradient( Point3D< Real > p , const PointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , const BSplineData< FEMDegree , BType >& bsData , const DenseNodeData< Real , FEMDegree >& coefficients ) const;
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	int _getSliceMatrixAndUpdateConstraints( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , SparseMatrix< Real >& matrix , DenseNodeData< Real , FEMDegree >& constraints , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& integrator , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& childIntegrator , const BSplineData< FEMDegree , BType >& bsData , LocalDepth depth , int slice , const DenseNodeData< Real , FEMDegree >& metSolution , bool coarseToFine );
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	int _getMatrixAndUpdateConstraints( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , SparseMatrix< Real >& matrix , DenseNodeData< Real , FEMDegree >& constraints , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template Integrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& integrator , typename BSplineIntegrationData< FEMDegree , BType , FEMDegree , BType >::FunctionIntegrator::template ChildIntegrator< DERIVATIVES( FEMDegree ) , DERIVATIVES( FEMDegree ) >& childIntegrator , const BSplineData< FEMDegree , BType >& bsData , LocalDepth depth , const DenseNodeData< Real , FEMDegree >& metSolution , bool coarseToFine );

	// Down samples constraints @(depth) to constraints @(depth-1)
	template< class C , int FEMDegree , BoundaryType BType > void _downSample( LocalDepth highDepth , DenseNodeData< C , FEMDegree >& constraints ) const;
	// Up samples coefficients @(depth-1) to coefficients @(depth)
	template< class C , int FEMDegree , BoundaryType BType > void _upSample( LocalDepth highDepth , DenseNodeData< C , FEMDegree >& coefficients ) const;
	template< class C , int FEMDegree , BoundaryType BType > static void _UpSample( LocalDepth highDepth , ConstPointer( C ) lowCoefficients , Pointer( C ) highCoefficients , int threads );
public:
	template< class C , int FEMDegree , BoundaryType BType > DenseNodeData< C , FEMDegree > coarseCoefficients( const  DenseNodeData< C , FEMDegree >& coefficients ) const;
	template< class C , int FEMDegree , BoundaryType BType > DenseNodeData< C , FEMDegree > coarseCoefficients( const SparseNodeData< C , FEMDegree >& coefficients ) const;
protected:

	/////////////////////////////////////////////
	// Code for splatting point-sample data    //
	// MultiGridOctreeData.WeightedSamples.inl //
	/////////////////////////////////////////////
	template< int WeightDegree >
	void _addWeightContribution( DensityEstimator< WeightDegree >& densityWeights , TreeOctNode* node , Point3D< Real > position , PointSupportKey< WeightDegree >& weightKey , Real weight=Real(1.0) );
	template< int WeightDegree , class PointSupportKey >
	Real _getSamplesPerNode( const DensityEstimator< WeightDegree >& densityWeights , const TreeOctNode* node , Point3D< Real > position , PointSupportKey& weightKey ) const;
	template< int WeightDegree , class PointSupportKey >
	void _getSampleDepthAndWeight( const DensityEstimator< WeightDegree >& densityWeights , const TreeOctNode* node , Point3D< Real > position , PointSupportKey& weightKey , Real& depth , Real& weight ) const;
	template< int WeightDegree , class PointSupportKey >
	void _getSampleDepthAndWeight( const DensityEstimator< WeightDegree >& densityWeights , Point3D< Real > position , PointSupportKey& weightKey , Real& depth , Real& weight ) const;
	template< bool CreateNodes ,                    int DataDegree , class V > void      _splatPointData( TreeOctNode* node ,                                           Point3D< Real > point , V v , SparseNodeData< V , DataDegree >& data ,                                              PointSupportKey< DataDegree >& dataKey                                                   );
	template< bool CreateNodes , int WeightDegree , int DataDegree , class V > Real      _splatPointData( const DensityEstimator< WeightDegree >& densityWeights , Point3D< Real > point , V v , SparseNodeData< V , DataDegree >& data , PointSupportKey< WeightDegree >& weightKey , PointSupportKey< DataDegree >& dataKey , LocalDepth minDepth , LocalDepth maxDepth , int dim=DIMENSION );
	template< bool CreateNodes , int WeightDegree , int DataDegree , class V > Real _multiSplatPointData( const DensityEstimator< WeightDegree >* densityWeights , TreeOctNode* node , Point3D< Real > point , V v , SparseNodeData< V , DataDegree >& data , PointSupportKey< WeightDegree >& weightKey , PointSupportKey< DataDegree >& dataKey , int dim=DIMENSION );
	template< class V , int DataDegree , BoundaryType BType , class Coefficients > V _evaluate( const Coefficients& coefficients , Point3D< Real > p , const BSplineData< DataDegree , BType >& bsData , const ConstPointSupportKey< DataDegree >& dataKey ) const;
public:
	template< class V , int DataDegree , BoundaryType BType > Pointer( V ) voxelEvaluate( const DenseNodeData< V , DataDegree >& coefficients , int& res , Real isoValue=0.f , LocalDepth depth=-1 , bool primal=false );

	template< int NormalDegree >
	struct HasNormalDataFunctor
	{
		const SparseNodeData< Point3D< Real > , NormalDegree >& normalInfo;
		HasNormalDataFunctor( const SparseNodeData< Point3D< Real > , NormalDegree >& ni ) : normalInfo( ni ){ ; }
		bool operator() ( const TreeOctNode* node ) const
		{
			const Point3D< Real >* n = normalInfo( node );
			if( n )
			{
				const Point3D< Real >& normal = *n;
				if( normal[0]!=0 || normal[1]!=0 || normal[2]!=0 ) return true;
			}
			if( node->children ) for( unsigned int c=0 ; c<Cube::CORNERS ; c++ ) if( (*this)( node->children + c ) ) return true;
			return false;
		}
	};
	struct TrivialHasDataFunctor{ bool operator() ( const TreeOctNode* node ) const{ return true; } };

	// [NOTE] The input/output for this method is pre-scaled by weight
	template< bool HasGradients > bool _setInterpolationInfoFromChildren( TreeOctNode* node , SparseNodeData< PointData< Real , HasGradients > , 0 >& iInfo ) const;
	template< bool HasGradients > SparseNodeData< PointData< Real , HasGradients > , 0 > _densifyInterpolationInfo( const std::vector< PointSample >& samples , Real pointValue , int adaptiveExponent ) const;

	template< int FEMDegree , BoundaryType BType > void _setValidityFlags( void );
	template< class HasDataFunctor > void _clipTree( const HasDataFunctor& f );

	template< int FEMDegree , BoundaryType BType > SparseNodeData<          Real   , 0 > leafValues   ( const DenseNodeData< Real , FEMDegree >& coefficients ) const;
	template< int FEMDegree , BoundaryType BType > SparseNodeData< Point3D< Real > , 0 > leafGradients( const DenseNodeData< Real , FEMDegree >& coefficients ) const;

	////////////////////////////////////
	// Evaluation Methods             //
	// MultiGridOctreeData.Evaluation //
	////////////////////////////////////
	static const int CHILDREN = Cube::CORNERS;
	template< int FEMDegree , BoundaryType BType >
	struct _Evaluator
	{
		typename BSplineEvaluationData< FEMDegree , BType >::Evaluator evaluator;
		typename BSplineEvaluationData< FEMDegree , BType >::ChildEvaluator childEvaluator;
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > cellStencil;
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > cellStencils  [CHILDREN];
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > edgeStencil             [Cube::EDGES  ];
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > edgeStencils  [CHILDREN][Cube::EDGES  ];
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > faceStencil             [Cube::FACES  ];
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > faceStencils  [CHILDREN][Cube::FACES  ];
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > cornerStencil           [Cube::CORNERS];
		Stencil< double , BSplineSupportSizes< FEMDegree >::SupportSize > cornerStencils[CHILDREN][Cube::CORNERS];

		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dCellStencil;
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dCellStencils  [CHILDREN];
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dEdgeStencil             [Cube::EDGES  ];
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dEdgeStencils  [CHILDREN][Cube::EDGES  ];
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dFaceStencil             [Cube::FACES  ];
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dFaceStencils  [CHILDREN][Cube::FACES  ];
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dCornerStencil           [Cube::CORNERS];
		Stencil< Point3D< double > , BSplineSupportSizes< FEMDegree >::SupportSize > dCornerStencils[CHILDREN][Cube::CORNERS];

		void set( LocalDepth depth );
		_Evaluator( void ){ _bsData = NULL; }
		~_Evaluator( void ){ if( _bsData ) delete _bsData , _bsData = NULL; }
	protected:
		BSplineData< FEMDegree , BType >* _bsData;
		friend Octree;
	};
	template< class V , int FEMDegree , BoundaryType BType >
	V _getCenterValue( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node ,                     const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const;
	template< class V , int FEMDegree , BoundaryType BType >
	V _getCornerValue( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int corner        , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const;
	template< class V , int FEMDegree , BoundaryType BType >
	V _getEdgeValue  ( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int edge          , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const;
	template< class V , int FEMDegree , BoundaryType BType >
	V _getValue      ( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , Point3D< Real > p , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator ) const;

	template< int FEMDegree , BoundaryType BType >
	std::pair< Real , Point3D< Real > > _getCenterValueAndGradient( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node ,                     const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const;
	template< int FEMDegree , BoundaryType BType >
	std::pair< Real , Point3D< Real > > _getCornerValueAndGradient( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int corner        , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const;
	template< int FEMDegree , BoundaryType BType >
	std::pair< Real , Point3D< Real > > _getEdgeValueAndGradient  ( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int edge          , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const;
	template< int FEMDegree , BoundaryType BType >
	std::pair< Real , Point3D< Real > > _getValueAndGradient      ( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , Point3D< Real > p , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator ) const;

public:
	template< int Degree , BoundaryType BType >
	class MultiThreadedEvaluator
	{
		const Octree* _tree;
		int _threads;
		std::vector< ConstPointSupportKey< Degree > > _neighborKeys;
		_Evaluator< Degree , BType > _evaluator;
		const DenseNodeData< Real , Degree >& _coefficients;
		DenseNodeData< Real , Degree > _coarseCoefficients;
	public:
		MultiThreadedEvaluator( const Octree* tree , const DenseNodeData< Real , Degree >& coefficients , int threads=1 );
		Real value( Point3D< Real > p , int thread=0 , const TreeOctNode* node=NULL );
		std::pair< Real , Point3D< Real > > valueAndGradient( Point3D< Real > , int thread=0 , const TreeOctNode* node=NULL );
	};

	////////////////////////////////////////
	// Iso-Surfacing Methods              //
	// MultiGridOctreeData.IsoSurface.inl //
	////////////////////////////////////////
protected:
	struct _IsoEdge
	{
		long long edges[2];
		_IsoEdge( void ){ edges[0] = edges[1] = 0; }
		_IsoEdge( long long v1 , long long v2 ){ edges[0] = v1 , edges[1] = v2; }
		long long& operator[]( int idx ){ return edges[idx]; }
		const long long& operator[]( int idx ) const { return edges[idx]; }
	};
	struct _FaceEdges
	{
		_IsoEdge edges[2];
		int count;
	};
	template< class Vertex >
	struct _SliceValues
	{
		typename SortedTreeNodes::SliceTableData sliceData;
		Pointer( Real ) cornerValues ; Pointer( Point3D< Real > ) cornerGradients ; Pointer( char ) cornerSet;
		Pointer( long long ) edgeKeys ; Pointer( char ) edgeSet;
		Pointer( _FaceEdges ) faceEdges ; Pointer( char ) faceSet;
		Pointer( char ) mcIndices;
		std::unordered_map< long long, std::vector< _IsoEdge > > faceEdgeMap;
		std::unordered_map< long long, std::pair< int, Vertex > > edgeVertexMap;
		std::unordered_map< long long, long long > vertexPairMap;

		_SliceValues( void );
		~_SliceValues( void );
		void reset( bool nonLinearFit );
	protected:
		int _oldCCount , _oldECount , _oldFCount , _oldNCount;
	};
	template< class Vertex >
	struct _XSliceValues
	{
		typename SortedTreeNodes::XSliceTableData xSliceData;
		Pointer( long long ) edgeKeys ; Pointer( char ) edgeSet;
		Pointer( _FaceEdges ) faceEdges ; Pointer( char ) faceSet;
		std::unordered_map< long long, std::vector< _IsoEdge > > faceEdgeMap;
		std::unordered_map< long long, std::pair< int, Vertex > > edgeVertexMap;
		std::unordered_map< long long, long long > vertexPairMap;

		_XSliceValues( void );
		~_XSliceValues( void );
		void reset( void );
	protected:
		int _oldECount , _oldFCount;
	};
	template< class Vertex >
	struct _SlabValues
	{
	protected:
		_XSliceValues< Vertex > _xSliceValues[2];
		_SliceValues< Vertex > _sliceValues[2];
	public:
		_SliceValues< Vertex >& sliceValues( int idx ){ return _sliceValues[idx&1]; }
		const _SliceValues< Vertex >& sliceValues( int idx ) const { return _sliceValues[idx&1]; }
		_XSliceValues< Vertex >& xSliceValues( int idx ){ return _xSliceValues[idx&1]; }
		const _XSliceValues< Vertex >& xSliceValues( int idx ) const { return _xSliceValues[idx&1]; }
	};
	template< class Vertex , int FEMDegree , BoundaryType BType >
	void _setSliceIsoCorners( const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , Real isoValue , LocalDepth depth , int slice ,         std::vector< _SlabValues< Vertex > >& sValues , const _Evaluator< FEMDegree , BType >& evaluator , int threads );
	template< class Vertex , int FEMDegree , BoundaryType BType >
	void _setSliceIsoCorners( const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , Real isoValue , LocalDepth depth , int slice , int z , std::vector< _SlabValues< Vertex > >& sValues , const _Evaluator< FEMDegree , BType >& evaluator , int threads );
	template< int WeightDegree , int ColorDegree , BoundaryType BType , class Vertex >
	void _setSliceIsoVertices( const BSplineData< ColorDegree , BType >* colorBSData , const DensityEstimator< WeightDegree >* densityWeights , const SparseNodeData< ProjectiveData< Point3D< Real > , Real > , ColorDegree >* colorData , Real isoValue , LocalDepth depth , int slice ,         int& vOffset , CoredMeshData< Vertex >& mesh , std::vector< _SlabValues< Vertex > >& sValues , int threads );
	template< int WeightDegree , int ColorDegree , BoundaryType BType , class Vertex >
	void _setSliceIsoVertices( const BSplineData< ColorDegree , BType >* colorBSData , const DensityEstimator< WeightDegree >* densityWeights , const SparseNodeData< ProjectiveData< Point3D< Real > , Real > , ColorDegree >* colorData , Real isoValue , LocalDepth depth , int slice , int z , int& vOffset , CoredMeshData< Vertex >& mesh , std::vector< _SlabValues< Vertex > >& sValues , int threads );
	template< int WeightDegree , int ColorDegree , BoundaryType BType , class Vertex >
	void _setXSliceIsoVertices( const BSplineData< ColorDegree , BType >* colorBSData , const DensityEstimator< WeightDegree >* densityWeights , const SparseNodeData< ProjectiveData< Point3D< Real > , Real > , ColorDegree >* colorData , Real isoValue , LocalDepth depth , int slab , int& vOffset , CoredMeshData< Vertex >& mesh , std::vector< _SlabValues< Vertex > >& sValues , int threads );
	template< class Vertex >
	void _setSliceIsoEdges( LocalDepth depth , int slice ,         std::vector< _SlabValues< Vertex > >& slabValues , int threads );
	template< class Vertex >
	void _setSliceIsoEdges( LocalDepth depth , int slice , int z , std::vector< _SlabValues< Vertex > >& slabValues , int threads );
	template< class Vertex >
	void _setXSliceIsoEdges( LocalDepth depth , int slice , std::vector< _SlabValues< Vertex > >& slabValues , int threads );
	template< class Vertex >
	void _copyFinerSliceIsoEdgeKeys( LocalDepth depth , int slice ,         std::vector< _SlabValues< Vertex > >& sValues , int threads );
	template< class Vertex >
	void _copyFinerSliceIsoEdgeKeys( LocalDepth depth , int slice , int z , std::vector< _SlabValues< Vertex > >& sValues , int threads );
	template< class Vertex >
	void _copyFinerXSliceIsoEdgeKeys( LocalDepth depth , int slab , std::vector< _SlabValues< Vertex > >& sValues , int threads );

	template< class Vertex >
	void _setIsoSurface( LocalDepth depth , int offset , const _SliceValues< Vertex >& bValues , const _SliceValues< Vertex >& fValues , const _XSliceValues< Vertex >& xValues , CoredMeshData< Vertex >& mesh , bool polygonMesh , bool addBarycenter , int& vOffset , int threads );

	template< class Vertex >
	static int _addIsoPolygons( CoredMeshData< Vertex >& mesh , std::vector< std::pair< int , Vertex > >& polygon , bool polygonMesh , bool addBarycenter , int& vOffset );

	template< int WeightDegree , int ColorDegree , BoundaryType BType , class Vertex >
	bool _getIsoVertex( const BSplineData< ColorDegree , BType >* colorBSData , const DensityEstimator< WeightDegree >* densityWeights , const SparseNodeData< ProjectiveData< Point3D< Real > , Real > , ColorDegree >* colorData , Real isoValue , ConstPointSupportKey< WeightDegree >& weightKey , ConstPointSupportKey< ColorDegree >& colorKey , const TreeOctNode* node , int edgeIndex , int z , const _SliceValues< Vertex >& sValues , Vertex& vertex );
	template< int WeightDegree , int ColorDegree , BoundaryType BType , class Vertex >
	bool _getIsoVertex( const BSplineData< ColorDegree , BType >* colorBSData , const DensityEstimator< WeightDegree >* densityWeights , const SparseNodeData< ProjectiveData< Point3D< Real > , Real > , ColorDegree >* colorData , Real isoValue , ConstPointSupportKey< WeightDegree >& weightKey , ConstPointSupportKey< ColorDegree >& colorKey , const TreeOctNode* node , int cornerIndex , const _SliceValues< Vertex >& bValues , const _SliceValues< Vertex >& fValues , Vertex& vertex );

	void _init( TreeOctNode* node , LocalDepth maxDepth , bool (*Refine)( LocalDepth d , LocalOffset off ) );

	double _maxMemoryUsage , _localMemoryUsage;
public:
	int threads;
	double maxMemoryUsage( void ) const { return _maxMemoryUsage; }
	double localMemoryUsage( void ) const { return _localMemoryUsage; }
	void resetLocalMemoryUsage( void ){ _localMemoryUsage = 0; }
	double memoryUsage( void );

	Octree( void );

	void init( LocalDepth maxDepth , bool (*Refine)( LocalDepth d , LocalOffset off ) );
	template< class Data >
	int init( OrientedPointStream< Real >& pointStream , LocalDepth maxDepth , bool useConfidence , std::vector< PointSample >& samples , std::vector< ProjectiveData< Data , Real > >* sampleData );
	template< int DensityDegree >
	typename Octree::template DensityEstimator< DensityDegree >* setDensityEstimator( const std::vector< PointSample >& samples , LocalDepth splatDepth , Real samplesPerNode );
	template< int NormalDegree , int DensityDegree >
	SparseNodeData< Point3D< Real > , NormalDegree > setNormalField( const std::vector< PointSample >& samples , const DensityEstimator< DensityDegree >& density , Real& pointWeightSum , bool forceNeumann );
	template< int DataDegree , bool CreateNodes , int DensityDegree , class Data >
	SparseNodeData< ProjectiveData< Data , Real > , DataDegree > setDataField( const std::vector< PointSample >& samples , std::vector< ProjectiveData< Data , Real > >& sampleData , const DensityEstimator< DensityDegree >* density );
	template< int MaxDegree , int FEMDegree , BoundaryType FEMBType , class HasDataFunctor > void inalizeForBroodedMultigrid( LocalDepth fullDepth , const HasDataFunctor& F , std::vector< int >* map=NULL );

	// Generate an empty set of constraints
	template< int FEMDegree > DenseNodeData< Real , FEMDegree > initDenseNodeData( void );

	// Add finite-elements constraints (derived from a sparse scalar field)
	template< int FEMDegree , BoundaryType FEMBType , int SFDegree , BoundaryType SFBType , class FEMSFConstraintFunctor > void addFEMConstraints( const FEMSFConstraintFunctor& F , const SparseNodeData< Real , SFDegree >& sfCoefficients , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth )
	{ return _addFEMConstraints< FEMDegree , FEMBType , SFDegree , SFBType , FEMSFConstraintFunctor , const SparseNodeData< Real   , SFDegree > , Real , double >( F , sfCoefficients , constraints , maxDepth ); }
	// Add finite-elements constraints (derived from a dense scalar field)
	template< int FEMDegree , BoundaryType FEMBType , int SFDegree , BoundaryType SFBType , class FEMSFConstraintFunctor > void addFEMConstraints( const FEMSFConstraintFunctor& F , const  DenseNodeData< Real , SFDegree >& sfCoefficients , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth )
	{ return _addFEMConstraints< FEMDegree , FEMBType , SFDegree , SFBType , FEMSFConstraintFunctor , const  DenseNodeData< Real   , SFDegree > , Real , double >( F , sfCoefficients , constraints , maxDepth ); }
	// Add finite-elements constraints (derived from a sparse vector field)
	template< int FEMDegree , BoundaryType FEMBType , int VFDegree , BoundaryType VFBType , class FEMVFConstraintFunctor > void addFEMConstraints( const FEMVFConstraintFunctor& F , const SparseNodeData< Point3D< Real > , VFDegree >& vfCoefficients , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth )
	{ return _addFEMConstraints< FEMDegree , FEMBType , VFDegree , VFBType , FEMVFConstraintFunctor , const SparseNodeData< Point3D< Real > , VFDegree > , Point3D< Real > , Point3D< double > >( F , vfCoefficients , constraints , maxDepth ); }
	// Add finite-elements constraints (derived from a dense vector field)
	template< int FEMDegree , BoundaryType FEMBType , int VFDegree , BoundaryType VFBType , class FEMVFConstraintFunctor > void addFEMConstraints( const FEMVFConstraintFunctor& F , const  DenseNodeData< Point3D< Real > , VFDegree >& vfCoefficients , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth )
	{ return _addFEMConstraints< FEMDegree , FEMBType , VFDegree , VFBType , FEMVFConstraintFunctor , const  DenseNodeData< Point3D< Real > , VFDegree > , Point3D< Real > , Point3D< double > >( F , vfCoefficients , constraints , maxDepth ); }
	// Add interpolation constraints
	template< int FEMDegree , BoundaryType FEMBType , bool HasGradients > void addInterpolationConstraints( const InterpolationInfo< HasGradients >& interpolationInfo , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth );

	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor > double dot( const DotFunctor& F , const SparseNodeData< Real , Degree1 >& coefficients1 , const SparseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , false >( F , (const InterpolationInfo< false >*)NULL , coefficients1 , coefficients2 ); }
	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor > double dot( const DotFunctor& F , const SparseNodeData< Real , Degree1 >& coefficients1 , const DenseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , false >( F , (const InterpolationInfo< false >*)NULL , coefficients1 , coefficients2 ); }
	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor > double dot( const DotFunctor& F , const DenseNodeData< Real , Degree1 >& coefficients1 , const SparseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , false >( F , (const InterpolationInfo< false >*)NULL , coefficients1 , coefficients2 ); }
	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor > double dot( const DotFunctor& F , const DenseNodeData< Real , Degree1 >& coefficients1 , const DenseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , false >( F , (const InterpolationInfo< false >*)NULL , coefficients1 , coefficients2 ); }

	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor , bool HasGradients > double dot( const DotFunctor& F , const InterpolationInfo< HasGradients >* iInfo , const SparseNodeData< Real , Degree1 >& coefficients1 , const SparseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , HasGradients >( F , iInfo , coefficients1 , coefficients2 ); }
	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor , bool HasGradients > double dot( const DotFunctor& F , const InterpolationInfo< HasGradients >* iInfo , const SparseNodeData< Real , Degree1 >& coefficients1 , const DenseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , HasGradients >( F , iInfo , coefficients1 , coefficients2 ); }
	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor , bool HasGradients > double dot( const DotFunctor& F , const InterpolationInfo< HasGradients >* iInfo , const DenseNodeData< Real , Degree1 >& coefficients1 , const SparseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , HasGradients >( F , iInfo , coefficients1 , coefficients2 ); }
	template< int Degree1 , BoundaryType BType1 , int Degree2 , BoundaryType BType2 , class DotFunctor , bool HasGradients > double dot( const DotFunctor& F , const InterpolationInfo< HasGradients >* iInfo , const DenseNodeData< Real , Degree1 >& coefficients1 , const DenseNodeData< Real , Degree2 >& coefficients2 ) const
	{ return _dot< Degree1 , BType1 , Degree2 , BType2 , DotFunctor , HasGradients >( F , iInfo , coefficients1 , coefficients2 ); }

	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	void setSystemMatrix( const FEMSystemFunctor& F , const InterpolationInfo< HasGradients >* interpolationInfo , LocalDepth depth , SparseMatrix< Real >& matrix ) const;

	// Solve the linear system
	struct SolverInfo
	{
		// How to solve
		LocalDepth cgDepth;
		int iters;
		double cgAccuracy , lowResIterMultiplier;
		// What to output
		bool verbose , showResidual;

		SolverInfo( void ) : cgDepth(0) , iters(1), cgAccuracy(0) , lowResIterMultiplier(0) , verbose(false) , showResidual(false) { ; }
	};
	template< int FEMDegree , BoundaryType BType , class FEMSystemFunctor , bool HasGradients >
	DenseNodeData< Real , FEMDegree > solveSystem( const FEMSystemFunctor& F , InterpolationInfo< HasGradients >* iData , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxSolveDepth , const SolverInfo& solverInfo );

	template< int FEMDegree , BoundaryType BType , int WeightDegree , int ColorDegree , class Vertex >
	void getMCIsoSurface( const DensityEstimator< WeightDegree >* densityWeights , const SparseNodeData< ProjectiveData< Point3D< Real > , Real > , ColorDegree >* colorData , const DenseNodeData< Real , FEMDegree >& solution , Real isoValue , CoredMeshData< Vertex >& mesh , bool nonLinearFit=true , bool addBarycenter=false , bool polygonMesh=false );


	const TreeOctNode& tree( void ) const{ return *_tree; }
	size_t leaves( void ) const { return _tree->leaves(); }
	size_t nodes( void ) const { int count = 0 ; for( const TreeOctNode* n=_tree->nextNode() ; n ; n=_tree->nextNode( n ) ) if( IsActiveNode( n ) ) count++ ; return count; }
	size_t ghostNodes( void ) const { int count = 0 ; for( const TreeOctNode* n=_tree->nextNode() ; n ; n=_tree->nextNode( n ) ) if( !IsActiveNode( n ) ) count++ ; return count; }
	inline size_t validSpaceNodes( void ) const { int count = 0 ; for( const TreeOctNode* n=_tree->nextNode() ; n ; n=_tree->nextNode( n ) ) if( isValidSpaceNode( n ) ) count++ ;  return count; }
	inline size_t validSpaceNodes( LocalDepth d ) const { int count = 0 ; for( const TreeOctNode* n=_tree->nextNode() ; n ; n=_tree->nextNode( n ) ) if( _localDepth(n)==d && isValidSpaceNode( n ) ) count++ ; return count; }
	template< int Degree , BoundaryType BType > size_t validFEMNodes( void ) const { int count = 0 ; for( const TreeOctNode* n=_tree->nextNode() ; n ; n=_tree->nextNode( n ) ) if( isValidFEMNode< Degree , BType >( n ) ) count++ ;  return count; }
	template< int Degree , BoundaryType BType > size_t validFEMNodes( LocalDepth d ) const { int count = 0 ; for( const TreeOctNode* n=_tree->nextNode() ; n ; n=_tree->nextNode( n ) ) if( _localDepth(n)==d && isValidFEMNode< Degree , BType >( n ) ) count++ ; return count; }
	LocalDepth depth( void ) const { return _localMaxDepth( _tree ); }
	void resetNodeIndices( void ){ _NodeCount = 0 ; for( TreeOctNode* node=_tree->nextNode() ; node ; node=_tree->nextNode( node ) ) _NodeInitializer( *node ) , node->nodeData.flags=0; }

protected:
	template< class D > static bool _IsZero( const D& d );
	template< class D > static Real _Dot( const D& d1 , const D& d2 );
	template< int FEMDegree , BoundaryType FEMBType , int CDegree , BoundaryType CBType , class FEMConstraintFunctor , class Coefficients , class D , class _D >
	void _addFEMConstraints( const FEMConstraintFunctor& F , const Coefficients& coefficients , DenseNodeData< Real , FEMDegree >& constraints , LocalDepth maxDepth );
	template< int FEMDegree1 , BoundaryType FEMBType1 , int FEMDegree2 , BoundaryType FEMBType2 , class DotFunctor , bool HasGradients , class Coefficients1 , class Coefficients2 >
	double _dot( const DotFunctor& F , const InterpolationInfo< HasGradients >* iInfo , const Coefficients1& coefficients1 , const Coefficients2& coefficients2 ) const;
};
template< class Real > int Octree< Real >::_NodeCount = 0;


template< class Real > void Reset( void ){ Octree< Real >::ResetNodeCount(); }


#include "MultiGridOctreeData.inl"
#include "MultiGridOctreeData.SortedTreeNodes.inl"
#include "MultiGridOctreeData.WeightedSamples.inl"
#include "MultiGridOctreeData.System.inl"
#include "MultiGridOctreeData.IsoSurface.inl"
#include "MultiGridOctreeData.Evaluation.inl"
#endif // MULTI_GRID_OCTREE_DATA_INCLUDED
