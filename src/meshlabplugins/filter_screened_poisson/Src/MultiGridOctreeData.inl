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
#ifdef FAST_SET_UP
#include <functional>
#endif // FAST_SET_UP

#include "MultiGridOctreeData.h"

#include <cmath>
#include "PointStream.h"
#include "MemoryUsage.h"

#define MEMORY_ALLOCATOR_BLOCK_SIZE 1<<12
//#define MEMORY_ALLOCATOR_BLOCK_SIZE 0

const double MATRIX_ENTRY_EPSILON = 0;
const double EPSILON              = 1e-6;
const double ROUND_EPS            = 1e-5;

//////////////////
// TreeNodeData //
//////////////////
TreeNodeData::TreeNodeData( void ){ flags = 0; }
TreeNodeData::~TreeNodeData( void ) { }


////////////
// Octree //
////////////
template< class Real >
double Octree< Real >::memoryUsage( void )
{
	double mem = double( MemoryInfo::Usage() ) / (1<<20);
	_maxMemoryUsage = std::max< double >( mem , _maxMemoryUsage );
	_localMemoryUsage = std::max< double >( mem , _localMemoryUsage );
	return mem;
}

template< class Real > Octree< Real >::Octree( void ) : _maxMemoryUsage(0) , _localMemoryUsage(0), threads(1)
{
	_tree = TreeOctNode::NewBrood( _NodeInitializer );
	_tree->initChildren( _NodeInitializer ) , _spaceRoot = _tree->children;
	_depthOffset = 1;
}

template< class Real >
template< int FEMDegree , BoundaryType BType >
void Octree< Real >::functionIndex( const TreeOctNode* node , int idx[3] ) const
{
	LocalDepth d ; LocalOffset off;
	_localDepthAndOffset( node , d , off );
	for( int dd=0 ; dd<DIMENSION ; dd++ ) idx[dd] = BSplineData< FEMDegree , BType >::FunctionIndex( d , off[dd] );
}

template< class Real >
OctNode< TreeNodeData >* Octree< Real >::leaf( Point3D< Real > p )
{
	if( !_InBounds( p ) ) return NULL;
	Point3D< Real > center = Point3D< Real >( Real(0.5) , Real(0.5) , Real(0.5) );
	Real width = Real(1.0);
	TreeOctNode* node = _spaceRoot;
	while( node->children )
	{
		int cIndex = TreeOctNode::CornerIndex( center , p );
		node = node->children + cIndex;
		width /= 2;
		if( cIndex&1 ) center[0] += width/2;
		else           center[0] -= width/2;
		if( cIndex&2 ) center[1] += width/2;
		else           center[1] -= width/2;
		if( cIndex&4 ) center[2] += width/2;
		else           center[2] -= width/2;
	}
	return node;
}
template< class Real >
const OctNode< TreeNodeData >* Octree< Real >::leaf( Point3D< Real > p ) const
{
	if( !_InBounds( p ) ) return NULL;
	Point3D< Real > center = Point3D< Real >( Real(0.5) , Real(0.5) , Real(0.5) );
	Real width = Real(1.0);
	TreeOctNode* node = _spaceRoot;
	while( node->children )
	{
		int cIndex = TreeOctNode::CornerIndex( center , p );
		node = node->children + cIndex;
		width /= 2;
		if( cIndex&1 ) center[0] += width/2;
		else           center[0] -= width/2;
		if( cIndex&2 ) center[1] += width/2;
		else           center[1] -= width/2;
		if( cIndex&4 ) center[2] += width/2;
		else           center[2] -= width/2;
	}
	return node;
}
template< class Real > bool Octree< Real >::_InBounds( Point3D< Real > p ){ return p[0]>=Real(0.) && p[0]<=Real(1.0) && p[1]>=Real(0.) && p[1]<=Real(1.0) && p[2]>=Real(0.) && p[2]<=Real(1.0); }
template< class Real >
template< int FEMDegree , BoundaryType BType >
bool Octree< Real >::isValidFEMNode( const TreeOctNode* node ) const
{
	if( GetGhostFlag( node ) ) return false;
	LocalDepth d ; LocalOffset off;
	_localDepthAndOffset( node , d , off );
	if( d<0 ) return false;
	return !BSplineEvaluationData< FEMDegree , BType >::OutOfBounds( d , off[0] ) && !BSplineEvaluationData< FEMDegree , BType >::OutOfBounds( d , off[1] ) && !BSplineEvaluationData< FEMDegree , BType >::OutOfBounds( d , off[2] );
}
template< class Real >
bool Octree< Real >::isValidSpaceNode( const TreeOctNode* node ) const
{
	if( !node ) return false;
	LocalDepth d ; LocalOffset off;
	_localDepthAndOffset( node , d , off );
	if( d<0 ) return false;
	int res = 1<<d;
	return off[0]>=0 && off[0]<res && off[1]>=0 && off[1]<res && off[2]>=0 && off[2]<res;
}
template< class Real >
template< int Degree , BoundaryType BType >
void Octree< Real >::_setFullDepth( TreeOctNode* node , LocalDepth depth ) const
{
	bool refine = false;
	LocalDepth d ; LocalOffset off;
	_localDepthAndOffset( node , d , off );
	if( d<depth ){
		if( d<0 ) {
			refine = true;
		}
		else if( BType==BOUNDARY_FREE && !_outOfBounds< Degree , BType >( node ) ) {
			refine = true;
		}
		else if( !BSplineSupportSizes< Degree >::OutOfBounds( d , off[0] ) && !BSplineSupportSizes< Degree >::OutOfBounds( d , off[1] ) && !BSplineSupportSizes< Degree >::OutOfBounds( d , off[2] ) ) {
			refine = true;
		}
	}
	if( refine ) {
		if( !node->children )
			node->initChildren( _NodeInitializer );
		for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
			_setFullDepth< Degree , BType >( node->children+c , depth );
	}
}
template< class Real >
template< int Degree , BoundaryType BType >
void Octree< Real >::_setFullDepth( LocalDepth depth )
{
	if( !_tree->children ) _tree->initChildren( _NodeInitializer );
	for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
		_setFullDepth< Degree , BType >( _tree->children+c , depth );
}

template< class Real , bool HasGradients >
struct _PointDataAccumulator_
{
#if POINT_DATA_RES
	static inline void _AddToPointData_( PointData< Real , HasGradients >& pData , Point3D< Real > position , Real value , Point3D< Real > gradient , Point3D< Real > center , Real width , Real weight );
#else // !POINT_DATA_RES
	static inline void _AddToPointData_( PointData< Real , HasGradients >& pData , Point3D< Real > position , Real value , Point3D< Real > gradient , Real weight );
#endif // POINT_DATA_RES
};
template< class Real >
struct _PointDataAccumulator_< Real , false >
{
#if POINT_DATA_RES
	static inline void _AddToPointData_( PointData< Real , false >& pData , Point3D< Real > position , Real value , Point3D< Real > , Point3D< Real > center , Real width , Real weight )
	{
		pData.addPoint( SinglePointData< Real , false >( position , value , weight ) , center , width );
	}
#else // !POINT_DATA_RES
	static inline void _AddToPointData_( PointData< Real , false >& pData , Point3D< Real > position , Real value , Point3D< Real > , Real weight )
	{
		pData.position += position , pData.value += value , pData.weight += weight;
	}
#endif // POINT_DATA_RES
};
template< class Real >
struct _PointDataAccumulator_< Real , true >
{
#if POINT_DATA_RES
	static inline void _AddToPointData_( PointData< Real , true >& pData , Point3D< Real > position , Real value , Point3D< Real > gradient , Point3D< Real > center , Real width , Real weight )
	{
		pData.addPoint( SinglePointData< Real , true >( position , value , gradient , weight ) , center , width );
	}
#else // !POINT_DATA_RES
	static inline void _AddToPointData_( PointData< Real , true >& pData , Point3D< Real > position , Real value , Point3D< Real > gradient , Real weight )
	{
		pData.position += position , pData.value += value , pData.gradient += gradient , pData.weight += weight;
	}
#endif // POINT_DATA_RES
};

template< class Real >
void Octree< Real >::_init( TreeOctNode* node , LocalDepth maxDepth , bool (*Refine)( LocalDepth , LocalOffset ) )
{
	if( _localDepth( node )<maxDepth )
	{
		LocalDepth d ; LocalOffset off;
		_localDepthAndOffset( node , d , off );
		if( Refine( d , off ) )
		{
			node->initChildren( _NodeInitializer );
			for( int c=0 ; c<Cube::CORNERS ; c++ ) _init( node->children + c , maxDepth , Refine );
		}
	}
}
template< class Real > void Octree< Real >::init( LocalDepth maxDepth , bool (*Refine)( LocalDepth , LocalOffset ) ){ _init( _spaceRoot , maxDepth , Refine ); }
template< class Real >
template< class Data >
int Octree< Real >::init( OrientedPointStream< Real >& pointStream , LocalDepth maxDepth , bool useConfidence , std::vector< PointSample >& samples , std::vector< ProjectiveData< Data , Real > >* sampleData )
{
	OrientedPointStreamWithData< Real , Data >& pointStreamWithData = ( OrientedPointStreamWithData< Real , Data >& )pointStream;

	// Add the point data
	int outOfBoundPoints = 0 , zeroLengthNormals = 0 , undefinedNormals = 0 , pointCount = 0;
	{
		std::vector< int > nodeToIndexMap;
		Point3D< Real > p , n;
		OrientedPoint3D< Real > _p;
		Data _d;
		while( ( sampleData ? pointStreamWithData.nextPoint( _p , _d ) : pointStream.nextPoint( _p ) ) )
		{
			p = Point3D< Real >(_p.p) , n = Point3D< Real >(_p.n);
			Real len = (Real)Length( n );
			if( !_InBounds(p) ){ outOfBoundPoints++ ; continue; }
			if( !len ){ zeroLengthNormals++ ; continue; }
			if( len!=len ){ undefinedNormals++ ; continue; }
			n /= len;
			Point3D< Real > center = Point3D< Real >( Real(0.5) , Real(0.5) , Real(0.5) );
			Real width = Real(1.0);
			TreeOctNode* temp = _spaceRoot;
			LocalDepth depth = _localDepth( temp );
			while( depth<maxDepth )
			{
				if( !temp->children ) temp->initChildren( _NodeInitializer );
				int cIndex = TreeOctNode::CornerIndex( center , p );
				temp = temp->children + cIndex;
				width /= 2;
				if( cIndex&1 ) center[0] += width/2;
				else           center[0] -= width/2;
				if( cIndex&2 ) center[1] += width/2;
				else           center[1] -= width/2;
				if( cIndex&4 ) center[2] += width/2;
				else           center[2] -= width/2;
				depth++;
			}
			Real weight = (Real)( useConfidence ? len : 1. );
			int nodeIndex = temp->nodeData.nodeIndex;
			if( (unsigned int)nodeIndex>=nodeToIndexMap.size() ) nodeToIndexMap.resize( nodeIndex+1 , -1 );
			int idx = nodeToIndexMap[ nodeIndex ];
			if( idx==-1 )
			{
				idx = (int)samples.size();
				nodeToIndexMap[ nodeIndex ] = idx;
				samples.resize( idx+1 ) , samples[idx].node = temp;
				if( sampleData ) sampleData->resize( idx+1 );
			}
			samples[idx].sample += ProjectiveData< OrientedPoint3D< Real > , Real >( OrientedPoint3D< Real >( p * weight , n * weight ) , weight );
			if( sampleData ) (*sampleData)[ idx ] += ProjectiveData< Data , Real >( _d * weight , weight );
			pointCount++;
		}
		pointStream.reset();
	}
	if( outOfBoundPoints  ) fprintf( stderr , "[WARNING] Found out-of-bound points: %d\n" , outOfBoundPoints );
	if( zeroLengthNormals ) fprintf( stderr , "[WARNING] Found zero-length normals: %d\n" , zeroLengthNormals );
	if( undefinedNormals  ) fprintf( stderr , "[WARNING] Found undefined normals: %d\n" , undefinedNormals );

	memoryUsage();
	return pointCount;
}
template< class Real >
template< int DensityDegree >
typename Octree< Real >::template DensityEstimator< DensityDegree >* Octree< Real >::setDensityEstimator( const std::vector< PointSample >& samples , LocalDepth splatDepth , Real samplesPerNode )
{
	LocalDepth maxDepth = _localMaxDepth( _tree );
	splatDepth = std::max< LocalDepth >( 0 , std::min< LocalDepth >( splatDepth , maxDepth ) );
	DensityEstimator< DensityDegree >* _density = new DensityEstimator< DensityDegree >( splatDepth );
	DensityEstimator< DensityDegree >& density = *_density;
	PointSupportKey< DensityDegree > densityKey;
	densityKey.set( _localToGlobal( splatDepth ) );

#ifdef FAST_SET_UP
	std::vector< int > sampleMap( NodeCount() , -1 );
#pragma omp parallel for num_threads( threads )
        for( int i=0 ; i<(int)samples.size() ; i++ )
		if( samples[i].sample.weight>0 )
			sampleMap[ samples[i].node->nodeData.nodeIndex ] = i;
	std::function< ProjectiveData< OrientedPoint3D< Real > , Real > ( TreeOctNode* ) > SetDensity = [&] ( TreeOctNode* node )
	{
		ProjectiveData< OrientedPoint3D< Real > , Real > sample;
		LocalDepth d = _localDepth( node );
		int idx = node->nodeData.nodeIndex;
		if( node->children )
			for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
			{
				ProjectiveData< OrientedPoint3D< Real > , Real > s = SetDensity( node->children + c );
				if( d<=splatDepth && s.weight>0 )
				{
					Point3D< Real > p = s.data.p / s.weight;
					Real w = s.weight / samplesPerNode;
					_addWeightContribution( density , node , p , densityKey , w );
				}
				sample += s;
			}
		else if( (unsigned int)idx<sampleMap.size() && sampleMap[idx]!=-1 )
		{
			sample = samples[ sampleMap[ idx ] ].sample;
			if( d<=splatDepth && sample.weight>0 )
			{
				Point3D< Real > p = sample.data.p / sample.weight;
				Real w = sample.weight / samplesPerNode;
				_addWeightContribution( density , node , p , densityKey , w );
			}
		}
		return sample;
	};
	SetDensity( _spaceRoot );
#else // !FAST_SET_UP
        for( int i=0 ; i<(int)samples.size() ; i++ )
	{
		const TreeOctNode* node = samples[i].node;
		const ProjectiveData< OrientedPoint3D< Real > , Real >& sample = samples[i].sample;
		if( sample.weight>0 )
		{
			Point3D< Real > p = sample.data.p / sample.weight;
			Real w = sample.weight / samplesPerNode;
			for( TreeOctNode* _node=(TreeOctNode*)node ; _node ; _node=_node->parent ) if( _localDepth( _node )<=splatDepth ) _addWeightContribution( density , _node , p , densityKey , w );
		}
	}
#endif // FAST_SET_UP

	memoryUsage();
	return _density;
}
template< class Real >
template< int NormalDegree , int DensityDegree >
SparseNodeData< Point3D< Real > , NormalDegree > Octree< Real >::setNormalField( const std::vector< PointSample >& samples , const DensityEstimator< DensityDegree >& density , Real& pointWeightSum , bool )
{
	LocalDepth maxDepth = _localMaxDepth( _tree );
	PointSupportKey< DensityDegree > densityKey;
	PointSupportKey< NormalDegree > normalKey;
	densityKey.set( _localToGlobal( maxDepth ) ) , normalKey.set( _localToGlobal( maxDepth ) );

	Real weightSum = 0;
	pointWeightSum = 0;
	SparseNodeData< Point3D< Real > , NormalDegree > normalField;
	for( unsigned int i=0 ; i<samples.size() ; i++ )
	{
		const ProjectiveData< OrientedPoint3D< Real > , Real >& sample = samples[i].sample;
		if( sample.weight>0 )
		{
			Point3D< Real > p = sample.data.p / sample.weight , n = sample.data.n;
			weightSum += sample.weight;
			if( !_InBounds(p) ){ fprintf( stderr , "[WARNING] Octree:setNormalField: Point sample is out of bounds\n" ) ; continue; }
			pointWeightSum += _splatPointData< true >( density , p , n , normalField , densityKey , normalKey , 0 , maxDepth , 3 );
		}
	}
	pointWeightSum /= weightSum;
	memoryUsage();

	return normalField;
}
template< class Real >
template< int DataDegree , bool CreateNodes , int DensityDegree , class Data >
SparseNodeData< ProjectiveData< Data , Real > , DataDegree > Octree< Real >::setDataField( const std::vector< PointSample >& samples , std::vector< ProjectiveData< Data , Real > >& sampleData , const DensityEstimator< DensityDegree >* density )
{
	LocalDepth maxDepth = _localMaxDepth( _tree );
	PointSupportKey< DensityDegree > densityKey;
	PointSupportKey< DataDegree > dataKey;
	densityKey.set( _localToGlobal( maxDepth ) ) , dataKey.set( _localToGlobal( maxDepth ) );

	SparseNodeData< ProjectiveData< Data , Real > , DataDegree > dataField;
	for( unsigned int i=0 ; i<samples.size() ; i++ )
	{
		const ProjectiveData< OrientedPoint3D< Real > , Real >& sample = samples[i].sample;
		const ProjectiveData< Data , Real >& data = sampleData[i];
		Point3D< Real > p = sample.weight==0 ? sample.data.p : sample.data.p / sample.weight;
		if( !_InBounds(p) ){ fprintf( stderr , "[WARNING] Point is out of bounds: %f %f %f <- %f %f %f [%f]\n" , p[0] , p[1] , p[2] , sample.data.p[0] , sample.data.p[1] , sample.data.p[2] , sample.weight ) ; continue; }
		_multiSplatPointData< CreateNodes >( density , (TreeOctNode*)samples[i].node , p , data , dataField , densityKey , dataKey , 2 );
	}
	memoryUsage();
	return dataField;
}
template< class Real >
template< int MaxDegree , int FEMDegree , BoundaryType FEMBType , class HasDataFunctor >
void Octree< Real >::inalizeForBroodedMultigrid( LocalDepth fullDepth , const HasDataFunctor& F , std::vector< int >* map )
{
	if( FEMDegree>MaxDegree ) fprintf( stderr , "[ERROR] MaxDegree must be at least as large as the FEM degree: %d <= %d\n" , FEMDegree , MaxDegree );
	while( _localInset( 0 ) + BSplineEvaluationData< MaxDegree , BOUNDARY_FREE >::Begin( 0 )<0 || _localInset( 0 ) + BSplineEvaluationData< MaxDegree , BOUNDARY_FREE >::End( 0 )>(1<<_depthOffset) )
	{
		//                       +-+-+-+-+-+-+-+-+
		//                       | | | | | | | | |
		//                       +-+-+-+-+-+-+-+-+
		//                       | | | | | | | | |
		//          +-+-+-+-+    +-+-+-+-+-+-+-+-+
		//          | | | | |    | | | | | | | | |
		// +-+-+    +-+-+-+-+    +-+-+-+-+-+-+-+-+
		// |*| |    | | | | |    | | | | | | | | |
		// +-o-+ -> +-+-o-+-+ -> +-+-+-+-o-+-+-+-+
		// | | |    | | |*| |    | | | | |*| | | |
		// +-+-+    +-+-+-+-+    +-+-+-+-+-+-+-+-+
		//          | | | | |    | | | | | | | | |
		//          +-+-+-+-+    +-+-+-+-+-+-+-+-+
		//                       | | | | | | | | |
		//                       +-+-+-+-+-+-+-+-+
		//                       | | | | | | | | |
		//                       +-+-+-+-+-+-+-+-+

		TreeOctNode* newSpaceRootParent = TreeOctNode::NewBrood( _NodeInitializer );
		TreeOctNode* oldSpaceRootParent = _spaceRoot->parent;
		int corner = _depthOffset<=1 ? Cube::CORNERS-1 : 0;
		newSpaceRootParent[corner].children = _spaceRoot;
		oldSpaceRootParent->children = newSpaceRootParent;
		for( unsigned int c=0 ; c<Cube::CORNERS ; c++ ){
			_spaceRoot[c].parent = newSpaceRootParent + corner;
			newSpaceRootParent[c].parent = oldSpaceRootParent;
		}
		_depthOffset++;
	}
	int d=0 , off[] = { 0 , 0 , 0 };
	TreeOctNode::ResetDepthAndOffset( _tree , d , off );
	_maxDepth = _localMaxDepth( _tree );

	// Make the low-resolution part of the tree be complete
	_fullDepth = std::max< LocalDepth >( 0 , std::min< LocalDepth >( _maxDepth , fullDepth ) );
	_setFullDepth< MaxDegree , BOUNDARY_FREE >( _fullDepth );
	// Clear all the flags and make everything that is not low-res a ghost node
	for( TreeOctNode* node=_tree->nextNode() ; node ; node=_tree->nextNode( node ) ) node->nodeData.flags = 0 , SetGhostFlag( node , _localDepth( node )>_fullDepth );

	// Set the ghost nodes for the high-res part of the tree
	_clipTree( F );

	const int OverlapRadius = -BSplineOverlapSizes< MaxDegree , MaxDegree >::OverlapStart;
	typename TreeOctNode::NeighborKey< OverlapRadius , OverlapRadius > neighborKey;
	neighborKey.set( _localToGlobal( _maxDepth-1 ) );

	for( LocalDepth d=_maxDepth-1 ; d>=0 ; d-- )
		for( TreeOctNode* node=_tree->nextNode() ; node ; node=_tree->nextNode( node ) ) if( _localDepth( node )==d && IsActiveNode( node->children ) )
		{
			neighborKey.template getNeighbors< true >( node , _NodeInitializer );
			for( int i=0 ; i<neighborKey.Width ; i++ ) for( int j=0 ; j<neighborKey.Width ; j++ ) for( int k=0 ; k<neighborKey.Width ; k++ ) SetGhostFlag( neighborKey.neighbors[ _localToGlobal(d) ].neighbors[i][j][k] , false );
		}

	_sNodes.set( *_tree , map );
	_setValidityFlags< FEMDegree , FEMBType >();
	for( TreeOctNode* node=_tree->nextNode() ; node ; node=_tree->nextNode( node ) ) if( !IsActiveNode( node ) ) node->nodeData.nodeIndex = -1;
	memoryUsage();
}


template< class Real >
template< int FEMDegree , BoundaryType BType >
void Octree< Real >::_setValidityFlags( void )
{
	for( int i=0 ; i<_sNodes.size() ; i++ )
	{
		const unsigned char MASK = ~( TreeNodeData::SPACE_FLAG | TreeNodeData::FEM_FLAG );
		_sNodes.treeNodes[i]->nodeData.flags &= MASK;
		if( isValidSpaceNode( _sNodes.treeNodes[i] ) ) _sNodes.treeNodes[i]->nodeData.flags |= TreeNodeData::SPACE_FLAG;
		if( isValidFEMNode< FEMDegree , BType >( _sNodes.treeNodes[i] ) ) _sNodes.treeNodes[i]->nodeData.flags |= TreeNodeData::FEM_FLAG;
	}
}

// Trim off the branches of the tree (finer than _fullDepth) that don't contain data
template< class Real >
template< class HasDataFunctor >
void Octree< Real >::_clipTree( const HasDataFunctor& f )
{
	// Because we are doing things in a brooded fashion, if any of the children has data then the whole brood is active
	for( TreeOctNode* temp=_tree->nextNode() ; temp ; temp=_tree->nextNode(temp) ) if( temp->children && _localDepth( temp )>=_fullDepth )
	{
		bool hasData = false;
		for( unsigned int c=0 ; c<Cube::CORNERS && !hasData ; c++ )
			hasData |= f( temp->children + c );
		for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
			SetGhostFlag( temp->children+c , !hasData );
	}
}

template< class Real >
template< bool HasGradients >
bool Octree< Real >::_setInterpolationInfoFromChildren( TreeOctNode* node , SparseNodeData< PointData< Real , HasGradients > , 0 >& interpolationInfo ) const
{
	if( IsActiveNode( node->children ) )
	{
		bool hasChildData = false;
		PointData< Real , HasGradients > pData;
#if POINT_DATA_RES
		Point3D< Real > center;
		Real width;
		_centerAndWidth( node , center , width );
		for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
			if( _setInterpolationInfoFromChildren( node->children + c , interpolationInfo ) )
			{
				const PointData< Real , HasGradients >& _pData = interpolationInfo[ node->children + c ];
				for( int cc=0 ; cc<PointData< Real , HasGradients >::SAMPLES ; cc++ )
				{
					int x[3];
					PointData< Real , HasGradients >::SetIndices( _pData[cc].position / _pData[cc].weight , center , width , x );
					pData[ x[0] + x[1]*PointData< Real , HasGradients >::RES + x[2]*PointData< Real , HasGradients >::RES*PointData< Real , HasGradients >::RES ] += _pData[cc];
				}
				hasChildData = true;
			}
#else // !POINT_DATA_RES
		for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
			if( _setInterpolationInfoFromChildren( node->children + c , interpolationInfo ) )
			{
				pData += interpolationInfo[ node->children + c ];
				hasChildData = true;
			}
#endif // POINT_DATA_RES
		if( hasChildData && IsActiveNode( node ) ) interpolationInfo[ node ] += pData;
		return hasChildData;
	}
	else return interpolationInfo( node )!=NULL;
}
template< class Real >
template< bool HasGradients >
SparseNodeData< PointData< Real , HasGradients > , 0 > Octree< Real >::_densifyInterpolationInfo( const std::vector< PointSample >& samples , Real pointValue , int adaptiveExponent ) const
{
	SparseNodeData< PointData< Real , HasGradients > , 0 > iInfo;
	for( unsigned int i=0 ; i<samples.size() ; i++ )
	{
		const TreeOctNode* node = samples[i].node;
		const ProjectiveData< OrientedPoint3D< Real > , Real >& pData = samples[i].sample;
		while( !IsActiveNode( node ) ) node = node->parent;
		if( pData.weight )
		{
#if POINT_DATA_RES
			Point3D< Real > center;
			Real width;
			_centerAndWidth( node , center , width );
			_PointDataAccumulator_< Real , HasGradients >::_AddToPointData_( iInfo[node] , pData.data.p , pointValue * pData.weight , pData.data.n , center , width , pData.weight );
#else // !POINT_DATA_RES
			_PointDataAccumulator_< Real , HasGradients >::_AddToPointData_( iInfo[node] , pData.data.p , pointValue * pData.weight , pData.data.n , pData.weight );
#endif // POINT_DATA_RES
		}
	}

	// Set the interior values
	_setInterpolationInfoFromChildren( _spaceRoot, iInfo );
#pragma omp parallel for
	for( int i=0 ; i<(int)iInfo.size() ; i++ )
#if POINT_DATA_RES
		for( int c=0 ; c<PointData< Real , HasGradients >::SAMPLES ; c++ )
		{
			Real w = iInfo[i][c].weight;
			iInfo[i][c] /= w ; iInfo[i][c].weight = w;
		}
#else // !POINT_DATA_RES
	{
		Real w = iInfo[i].weight;
		iInfo[i] /= w ; iInfo[i].weight = w;
	}
#endif // POINT_DATA_RES
	LocalDepth maxDepth = _localMaxDepth( _tree );

	// Set the average position and scale the weights
	for( const TreeOctNode* node=_tree->nextNode() ; node ; node=_tree->nextNode(node) ) if( IsActiveNode( node ) )
	{
		PointData< Real , HasGradients >* pData = iInfo( node );
		if( pData )
		{
			int e = _localDepth( node ) * adaptiveExponent - ( maxDepth ) * (adaptiveExponent-1);
#if POINT_DATA_RES
			for( int c=0 ; c<PointData< Real , HasGradients >::SAMPLES ; c++ ) if( (*pData)[c].weight )
			{
				if( e<0 ) (*pData)[c].weight /= Real( 1<<(-e) );
				else      (*pData)[c].weight *= Real( 1<<  e  );
			}
#else // !POINT_DATA_RES
			if( e<0 ) pData->weight /= Real( 1<<(-e) );
			else      pData->weight *= Real( 1<<  e  );
#endif // POINT_DATA_RES
		}
	}
	return iInfo;
}
////////////////
// VertexData //
////////////////
long long VertexData::CenterIndex( const TreeOctNode* node , int maxDepth )
{
	int idx[DIMENSION];
	return CenterIndex(node,maxDepth,idx);
}
long long VertexData::CenterIndex(const TreeOctNode* node,int maxDepth,int idx[DIMENSION])
{
	int d , o[3];
	node->depthAndOffset( d , o );
	for( int i=0 ; i<DIMENSION ; i++ ) idx[i] = BinaryNode::CornerIndex( maxDepth+1 , d+1 , o[i]<<1 , 1 );
	return (long long)(idx[0]) | (long long)(idx[1])<<VERTEX_COORDINATE_SHIFT | (long long)(idx[2])<<(2*VERTEX_COORDINATE_SHIFT);
}
long long VertexData::CenterIndex( int depth , const int offSet[DIMENSION] , int maxDepth , int idx[DIMENSION] )
{
	for(int i=0;i<DIMENSION;i++) idx[i]=BinaryNode::CornerIndex( maxDepth+1 , depth+1 , offSet[i]<<1 , 1 );
	return (long long)(idx[0]) | (long long)(idx[1])<<VERTEX_COORDINATE_SHIFT | (long long)(idx[2])<<(2*VERTEX_COORDINATE_SHIFT);
}
long long VertexData::CornerIndex(const TreeOctNode* node,int cIndex,int maxDepth)
{
	int idx[DIMENSION];
	return CornerIndex(node,cIndex,maxDepth,idx);
}
long long VertexData::CornerIndex( const TreeOctNode* node , int cIndex , int maxDepth , int idx[DIMENSION] )
{
	int x[DIMENSION];
	Cube::FactorCornerIndex( cIndex , x[0] , x[1] , x[2] );
	int d , o[3];
	node->depthAndOffset( d , o );
	for( int i=0 ; i<DIMENSION ; i++ ) idx[i] = BinaryNode::CornerIndex( maxDepth+1 , d , o[i] , x[i] );
	return CornerIndexKey( idx );
}
long long VertexData::CornerIndex( int depth , const int offSet[DIMENSION] , int cIndex , int maxDepth , int idx[DIMENSION] )
{
	int x[DIMENSION];
	Cube::FactorCornerIndex( cIndex , x[0] , x[1] , x[2] );
	for( int i=0 ; i<DIMENSION ; i++ ) idx[i] = BinaryNode::CornerIndex( maxDepth+1 , depth , offSet[i] , x[i] );
	return CornerIndexKey( idx );
}
long long VertexData::CornerIndexKey( const int idx[DIMENSION] )
{
	return (long long)(idx[0]) | (long long)(idx[1])<<VERTEX_COORDINATE_SHIFT | (long long)(idx[2])<<(2*VERTEX_COORDINATE_SHIFT);
}
long long VertexData::FaceIndex(const TreeOctNode* node,int fIndex,int maxDepth){
	int idx[DIMENSION];
	return FaceIndex(node,fIndex,maxDepth,idx);
}
long long VertexData::FaceIndex(const TreeOctNode* node,int fIndex,int maxDepth,int idx[DIMENSION])
{
	int dir,offset;
	Cube::FactorFaceIndex(fIndex,dir,offset);
	int d,o[3];
	node->depthAndOffset(d,o);
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode::CornerIndex(maxDepth+1,d+1,o[i]<<1,1);}
	idx[dir]=BinaryNode::CornerIndex(maxDepth+1,d,o[dir],offset);
	return (long long)(idx[0]) | (long long)(idx[1])<<VERTEX_COORDINATE_SHIFT | (long long)(idx[2])<<(2*VERTEX_COORDINATE_SHIFT);
}
long long VertexData::EdgeIndex( const TreeOctNode* node , int eIndex , int maxDepth ){ int idx[DIMENSION] ; return EdgeIndex( node , eIndex , maxDepth , idx ); }
long long VertexData::EdgeIndex( const TreeOctNode* node , int eIndex , int maxDepth , int idx[DIMENSION] )
{
	int o , i1 , i2;
	int d , off[3];
	node->depthAndOffset( d ,off );
	Cube::FactorEdgeIndex( eIndex , o , i1 , i2 );
	for( int i=0 ; i<DIMENSION ; i++ ) idx[i] = BinaryNode::CornerIndex( maxDepth+1 , d+1 , off[i]<<1 , 1 );
	switch(o)
	{
		case 0:
			idx[1] = BinaryNode::CornerIndex( maxDepth+1 , d , off[1] , i1 );
			idx[2] = BinaryNode::CornerIndex( maxDepth+1 , d , off[2] , i2 );
			break;
		case 1:
			idx[0] = BinaryNode::CornerIndex( maxDepth+1 , d , off[0] , i1 );
			idx[2] = BinaryNode::CornerIndex( maxDepth+1 , d , off[2] , i2 );
			break;
		case 2:
			idx[0] = BinaryNode::CornerIndex( maxDepth+1 , d , off[0] , i1 );
			idx[1] = BinaryNode::CornerIndex( maxDepth+1 , d , off[1] , i2 );
			break;
	};
	return (long long)(idx[0]) | (long long)(idx[1])<<VERTEX_COORDINATE_SHIFT | (long long)(idx[2])<<(2*VERTEX_COORDINATE_SHIFT);
}
