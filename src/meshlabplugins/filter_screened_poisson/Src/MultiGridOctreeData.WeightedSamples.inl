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

// evaluate the result of splatting along a plane and then evaluating at a point on the plane.
template< int Degree > double GetScaleValue( void )
{
	double centerValues[Degree+1];
	Polynomial< Degree >::BSplineComponentValues( 0.5 , centerValues );
	double scaleValue = 0;
	for( int i=0 ; i<=Degree ; i++ ) scaleValue += centerValues[i] * centerValues[i];
	return 1./ scaleValue;
}
template< class Real >
template< int WeightDegree >
void Octree< Real >::_addWeightContribution( DensityEstimator< WeightDegree >& densityWeights , TreeOctNode* node , Point3D< Real > position , PointSupportKey< WeightDegree >& weightKey , Real weight )
{
	static const double ScaleValue = GetScaleValue< WeightDegree >();
	double dx[ DIMENSION ][ PointSupportKey< WeightDegree >::Size ];
	typename TreeOctNode::Neighbors< PointSupportKey< WeightDegree >::Size >& neighbors = weightKey.template getNeighbors< true >( node , _NodeInitializer );
	densityWeights.reserve( NodeCount() );
	Point3D< Real > start;
	Real w;
	_startAndWidth( node , start , w );
	for( int dim=0 ; dim<DIMENSION ; dim++ ) Polynomial< WeightDegree >::BSplineComponentValues( ( position[dim]-start[dim] ) / w , dx[dim] );

	weight *= (Real)ScaleValue;

	for( int i=0 ; i<PointSupportKey< WeightDegree >::Size ; i++ ) for( int j=0 ; j<PointSupportKey< WeightDegree >::Size ; j++ )
	{
		double dxdy = dx[0][i] * dx[1][j] * weight;
		TreeOctNode** _neighbors = neighbors.neighbors[i][j];
		for( int k=0 ; k<PointSupportKey< WeightDegree >::Size ; k++ ) if( _neighbors[k] ) densityWeights[ _neighbors[k] ] += Real( dxdy * dx[2][k] );
	}
}

template< class Real >
template< int WeightDegree , class PointSupportKey >
Real Octree< Real >::_getSamplesPerNode( const DensityEstimator< WeightDegree >& densityWeights , const TreeOctNode* node , Point3D< Real > position , PointSupportKey& weightKey ) const
{
	Real weight = 0;
	double dx[ DIMENSION ][ PointSupportKey::Size ];
	const typename PointSupportKey::template Neighbors< PointSupportKey::Size >& neighbors = weightKey.getNeighbors( node );

	Point3D< Real > start;
	Real w;
	_startAndWidth( node , start , w );

	for( int dim=0 ; dim<DIMENSION ; dim++ ) Polynomial< WeightDegree >::BSplineComponentValues( ( position[dim]-start[dim] ) / w , dx[dim] );

	for( int i=0 ; i<PointSupportKey::Size ; i++ ) for( int j=0 ; j<PointSupportKey::Size ; j++ )
	{
		double dxdy = dx[0][i] * dx[1][j];
		for( int k=0 ; k<PointSupportKey::Size ; k++ ) if( neighbors.neighbors[i][j][k] )
		{
			const Real* w = densityWeights( neighbors.neighbors[i][j][k] );
			if( w ) weight += Real( dxdy * dx[2][k] * (*w) );
		}
	}
	return weight;
}
template< class Real >
template< int WeightDegree , class PointSupportKey >
void Octree< Real >::_getSampleDepthAndWeight( const DensityEstimator< WeightDegree >& densityWeights , const TreeOctNode* node , Point3D< Real > position , PointSupportKey& weightKey , Real& depth , Real& weight ) const
{
	const TreeOctNode* temp = node;
	while( _localDepth( temp )>densityWeights.kernelDepth() ) temp = temp->parent;
	weight = _getSamplesPerNode( densityWeights , temp , position , weightKey );
	if( weight>=(Real)1. ) depth = Real( _localDepth( temp ) + log( weight ) / log(double(1<<(DIMENSION-1))) );
	else
	{
		Real oldWeight , newWeight;
		oldWeight = newWeight = weight;
		while( newWeight<(Real)1. && temp->parent )
		{
			temp=temp->parent;
			oldWeight = newWeight;
			newWeight = _getSamplesPerNode( densityWeights , temp , position , weightKey );
		}
		depth = Real( _localDepth( temp ) + log( newWeight ) / log( newWeight / oldWeight ) );
	}
	weight = Real( pow( double(1<<(DIMENSION-1)) , -double(depth) ) );
}
template< class Real >
template< int WeightDegree , class PointSupportKey >
void Octree< Real >::_getSampleDepthAndWeight( const DensityEstimator< WeightDegree >& densityWeights , Point3D< Real > position , PointSupportKey& weightKey , Real& depth , Real& weight ) const
{
	TreeOctNode* temp;
	Point3D< Real > myCenter( (Real)0.5 , (Real)0.5 , (Real)0.5 );
	Real myWidth = Real( 1. );

	// Get the finest node with depth less than or equal to the splat depth that contains the point
	temp = _spaceRoot;
	while( _localDepth( temp )<densityWeights.kernelDepth() )
	{
		if( !IsActiveNode( temp->children ) ) break;// fprintf( stderr , "[ERROR] Octree::GetSampleDepthAndWeight\n" ) , exit( 0 );
		int cIndex = TreeOctNode::CornerIndex( myCenter , position );
		temp = temp->children + cIndex;
		myWidth /= 2;
		if( cIndex&1 ) myCenter[0] += myWidth/2;
		else		   myCenter[0] -= myWidth/2;
		if( cIndex&2 ) myCenter[1] += myWidth/2;
		else		   myCenter[1] -= myWidth/2;
		if( cIndex&4 ) myCenter[2] += myWidth/2;
		else		   myCenter[2] -= myWidth/2;
	}
	return _getSampleDepthAndWeight( densityWeights , temp , position , weightKey , depth , weight );
}

template< class Real >
template< bool CreateNodes , int DataDegree , class V >
void Octree< Real >::_splatPointData( TreeOctNode* node , Point3D< Real > position , V v , SparseNodeData< V , DataDegree >& dataInfo , PointSupportKey< DataDegree >& dataKey )
{
	double dx[ DIMENSION ][ PointSupportKey< DataDegree >::Size ];
	typename TreeOctNode::Neighbors< PointSupportKey< DataDegree >::Size >& neighbors = dataKey.template getNeighbors< CreateNodes >( node , _NodeInitializer );
	Point3D< Real > start;
	Real w;
	_startAndWidth( node , start , w );

	for( int dd=0 ; dd<DIMENSION ; dd++ ) Polynomial< DataDegree >::BSplineComponentValues( ( position[dd]-start[dd] ) / w , dx[dd] );

	for( int i=0 ; i<PointSupportKey< DataDegree >::Size ; i++ ) for( int j=0 ; j<PointSupportKey< DataDegree >::Size ; j++ )
	{
		double dxdy = dx[0][i] * dx[1][j];
		for( int k=0 ; k<PointSupportKey< DataDegree >::Size ; k++ )
			if( IsActiveNode( neighbors.neighbors[i][j][k] ) )
			{
				TreeOctNode* _node = neighbors.neighbors[i][j][k];

				double dxdydz = dxdy * dx[2][k];
				dataInfo[ _node ] += v * (Real)dxdydz;
			}
	}
}
template< class Real >
template< bool CreateNodes , int WeightDegree , int DataDegree , class V >
Real Octree< Real >::_splatPointData( const DensityEstimator< WeightDegree >& densityWeights , Point3D< Real > position , V v , SparseNodeData< V , DataDegree >& dataInfo , PointSupportKey< WeightDegree >& weightKey , PointSupportKey< DataDegree >& dataKey , LocalDepth minDepth , LocalDepth maxDepth , int dim )
{
	double dx;
	V _v;
	TreeOctNode* temp;
	double width;
	Point3D< Real > myCenter( (Real)0.5 , (Real)0.5 , (Real)0.5 );
	Real myWidth = (Real)1.;

	temp = _spaceRoot;
	while( _localDepth( temp )<densityWeights.kernelDepth() )
	{
		if( !IsActiveNode( temp->children ) ) break;
		int cIndex = TreeOctNode::CornerIndex( myCenter , position );
		temp = temp->children + cIndex;
		myWidth /= 2;
		if( cIndex&1 ) myCenter[0] += myWidth/2;
		else		   myCenter[0] -= myWidth/2;
		if( cIndex&2 ) myCenter[1] += myWidth/2;
		else 	  	   myCenter[1] -= myWidth/2;
		if( cIndex&4 ) myCenter[2] += myWidth/2;
		else 		   myCenter[2] -= myWidth/2;
	}
	Real weight , depth;
	_getSampleDepthAndWeight( densityWeights , temp , position , weightKey , depth , weight );

	if( depth<minDepth ) depth = Real(minDepth);
	if( depth>maxDepth ) depth = Real(maxDepth);
	int topDepth = int(ceil(depth));

	dx = 1.0-(topDepth-depth);
	if     ( topDepth<=minDepth ) topDepth = minDepth , dx = 1;
	else if( topDepth> maxDepth ) topDepth = maxDepth , dx = 1;

	while( _localDepth( temp )>topDepth ) temp=temp->parent;
	while( _localDepth( temp )<topDepth )
	{
		if( !temp->children ) temp->initChildren( _NodeInitializer );
		int cIndex = TreeOctNode::CornerIndex( myCenter , position );
		temp = &temp->children[cIndex];
		myWidth/=2;
		if( cIndex&1 ) myCenter[0] += myWidth/2;
		else		   myCenter[0] -= myWidth/2;
		if( cIndex&2 ) myCenter[1] += myWidth/2;
		else		   myCenter[1] -= myWidth/2;
		if( cIndex&4 ) myCenter[2] += myWidth/2;
		else		   myCenter[2] -= myWidth/2;
	}
	width = 1.0 / ( 1<<_localDepth( temp ) );
	_v = v * weight / Real( pow( width , dim ) ) * Real( dx );
	_splatPointData< CreateNodes >( temp , position , _v , dataInfo , dataKey );
	if( fabs(1.0-dx) > EPSILON )
	{
		dx = Real(1.0-dx);
		temp = temp->parent;
		width = 1.0 / ( 1<<_localDepth( temp ) );

		_v = v * weight / Real( pow( width , dim ) ) * Real( dx );
		_splatPointData< CreateNodes >( temp , position , _v , dataInfo , dataKey );
	}
	return weight;
}
template< class Real >
template< bool CreateNodes , int WeightDegree , int DataDegree , class V >
Real Octree< Real >::_multiSplatPointData( const DensityEstimator< WeightDegree >* densityWeights , TreeOctNode* node , Point3D< Real > position , V v , SparseNodeData< V , DataDegree >& dataInfo , PointSupportKey< WeightDegree >& weightKey , PointSupportKey< DataDegree >& dataKey , int dim )
{
	Real _depth , weight;
	if( densityWeights ) _getSampleDepthAndWeight( *densityWeights , position , weightKey , _depth , weight );
	else weight = (Real)1.;
	V _v = v * weight;

	double dx[ DIMENSION ][ PointSupportKey< DataDegree >::Size ];
	dataKey.template getNeighbors< CreateNodes >( node , _NodeInitializer );

	for( TreeOctNode* _node=node ; _localDepth( _node )>=0 ; _node=_node->parent )
	{
		V __v = _v * (Real)pow( 1<<_localDepth( _node ) , dim );
		Point3D< Real > start;
		Real w;
		_startAndWidth( _node , start , w );
		for( int dd=0 ; dd<DIMENSION ; dd++ ) Polynomial< DataDegree >::BSplineComponentValues( ( position[dd]-start[dd] ) / w , dx[dd] );
		typename TreeOctNode::Neighbors< PointSupportKey< DataDegree >::Size >& neighbors = dataKey.neighbors[ _localToGlobal( _localDepth( _node ) ) ];
		for( int i=0 ; i<PointSupportKey< DataDegree >::Size ; i++ ) for( int j=0 ; j<PointSupportKey< DataDegree >::Size ; j++ )
		{
			double dxdy = dx[0][i] * dx[1][j];
			for( int k=0 ; k<PointSupportKey< DataDegree >::Size ; k++ )
				if( IsActiveNode( neighbors.neighbors[i][j][k] ) )
				{
					TreeOctNode* _node = neighbors.neighbors[i][j][k];
					double dxdydz = dxdy * dx[2][k];
					dataInfo[ _node ] += __v * (Real)dxdydz;
				}
		}
	}
	return weight;
}

template< class Real >
template< class V , int DataDegree , BoundaryType BType , class Coefficients >
V Octree< Real >::_evaluate( const Coefficients& coefficients , Point3D< Real > p , const BSplineData< DataDegree , BType >& bsData , const ConstPointSupportKey< DataDegree >& dataKey ) const
{
	V value = V(0);

	for( int d=_localToGlobal( 0 ) ; d<=dataKey.depth() ; d++ )
	{
		double dx[ DIMENSION ][ PointSupportKey< DataDegree >::Size ];
		memset( dx , 0 , sizeof( double ) * DIMENSION * PointSupportKey< DataDegree >::Size );
		{
			const TreeOctNode* n = dataKey.neighbors[d].neighbors[ PointSupportKey< DataDegree >::LeftRadius ][ PointSupportKey< DataDegree >::LeftRadius ][ PointSupportKey< DataDegree >::LeftRadius ];
			if( !n ) fprintf( stderr , "[ERROR] Point is not centered on a node\n" ) , exit( 0 );
			int fIdx[3];
			functionIndex< DataDegree , BType >( n , fIdx );
			int fStart , fEnd;
			BSplineData< DataDegree , BType >::FunctionSpan( _localDepth( n ) , fStart , fEnd );
			for( int dd=0 ; dd<DIMENSION ; dd++ ) for( int i=-PointSupportKey< DataDegree >::LeftRadius ; i<=PointSupportKey< DataDegree >::RightRadius ; i++ )
				if( fIdx[dd]+i>=fStart && fIdx[dd]+i<fEnd ) dx[dd][i] = bsData.baseBSplines[ fIdx[dd]+i ][ -i+PointSupportKey< DataDegree >::RightRadius ]( p[dd] );
		}
		for( int i=0 ; i<PointSupportKey< DataDegree >::Size ; i++ ) for( int j=0 ; j<PointSupportKey< DataDegree >::Size ; j++ ) for( int k=0 ; k<PointSupportKey< DataDegree >::Size ; k++ )
		{
			const TreeOctNode* n = dataKey.neighbors[d].neighbors[i][j][k];
			if( isValidFEMNode< DataDegree , BType >( n ) )
			{
				const V* v = coefficients( n );
				if( v ) value += (*v) * (Real) ( dx[0][i] * dx[1][j] * dx[2][k] );
			}
		}
	}

	return value;
}

template< class Real >
template< class V , int DataDegree , BoundaryType BType >
Pointer( V ) Octree< Real >::voxelEvaluate( const DenseNodeData< V , DataDegree >& coefficients , int& res , Real isoValue , LocalDepth depth , bool primal )
{
	int begin , end , dim;
	if( depth<=0 || depth>_maxDepth ) depth = _maxDepth;

	// Initialize the coefficients at the coarsest level
	Pointer( V ) _coefficients = NullPointer( V );
	{
		LocalDepth d = 0;
		begin = _BSplineBegin< DataDegree , BType >( d ) , end = _BSplineEnd< DataDegree , BType >( d ) , dim = end - begin;
		_coefficients = NewPointer< V >( dim * dim * dim );
		memset( _coefficients , 0 , sizeof( V ) * dim  * dim * dim );
#pragma omp parallel for num_threads( threads )
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( !_outOfBounds< DataDegree , BType >( _sNodes.treeNodes[i] ) )
		{
			LocalDepth _d ; LocalOffset _off;
			_localDepthAndOffset( _sNodes.treeNodes[i] , _d , _off );
			_off[0] -= begin , _off[1] -= begin , _off[2] -= begin;
			_coefficients[ _off[0] + _off[1]*dim + _off[2]*dim*dim ] = coefficients[i];
		}
	}

	// Up-sample and add in the existing coefficients
	for( LocalDepth d=1 ; d<=depth ; d++ )
	{
		begin = _BSplineBegin< DataDegree , BType >( d ) , end = _BSplineEnd< DataDegree , BType >( d ) , dim = end - begin;
		Pointer( V ) __coefficients = NewPointer< V >( dim * dim *dim );
		memset( __coefficients , 0 , sizeof( V ) * dim  * dim * dim );
#pragma omp parallel for num_threads( threads )
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( !_outOfBounds< DataDegree , BType >( _sNodes.treeNodes[i] ) )
		{
			LocalDepth _d ; LocalOffset _off;
			_localDepthAndOffset( _sNodes.treeNodes[i] , _d , _off );
			_off[0] -= begin , _off[1] -= begin , _off[2] -= begin;
			__coefficients[ _off[0] + _off[1]*dim + _off[2]*dim*dim ] = coefficients[i];
		}
		_UpSample< V , DataDegree , BType >( d , ( ConstPointer(V) )_coefficients , __coefficients , threads );
		DeletePointer( _coefficients );
		_coefficients = __coefficients;
	}

	res = 1<<depth;
	if( primal ) res++;
	Pointer( V ) values = NewPointer< V >( res*res*res );
	memset( values , 0 , sizeof(V)*res*res*res );

	if( primal )
	{
		// evaluate at the cell corners
		typename BSplineEvaluationData< DataDegree , BType >::CornerEvaluator::Evaluator evaluator;
		BSplineEvaluationData< DataDegree , BType >::SetCornerEvaluator( evaluator , depth );
#pragma omp parallel for num_threads( threads )
		for( int k=0 ; k<res ; k++ ) for( int j=0 ; j<res ; j++ ) for( int i=0 ; i<res ; i++ )
		{
			V value = values[ i + j*res + k*res*res ];
			for( int kk=-BSplineSupportSizes< DataDegree >::CornerEnd ; kk<=-BSplineSupportSizes< DataDegree >::CornerStart ; kk++ ) if( k+kk>=begin && k+kk<end )
				for( int jj=-BSplineSupportSizes< DataDegree >::CornerEnd ; jj<=-BSplineSupportSizes< DataDegree >::CornerStart ; jj++ ) if( j+jj>=begin && j+jj<end )
				{
					double weight = evaluator.value( k+kk , k , false ) * evaluator.value( j+jj , j , false );
					int idx = (j+jj-begin)*dim + (k+kk-begin)*dim*dim;
					for( int ii=-BSplineSupportSizes< DataDegree >::CornerEnd ; ii<=-BSplineSupportSizes< DataDegree >::CornerStart ; ii++ ) if( i+ii>=begin && i+ii<end )
						value += _coefficients[ i + ii - begin + idx ] * Real( weight * evaluator.value( i + ii , i , false ) );
				}
			values[ i + j*res + k*res*res ] = value;
		}
	}
	else
	{
		// evaluate at the cell centers
		typename BSplineEvaluationData< DataDegree , BType >::CenterEvaluator::Evaluator evaluator;
		BSplineEvaluationData< DataDegree , BType >::SetCenterEvaluator( evaluator , depth );
#pragma omp parallel for num_threads( threads )
		for( int k=0 ; k<res ; k++ ) for( int j=0 ; j<res ; j++ ) for( int i=0 ; i<res ; i++ )
		{
			V& value = values[ i + j*res + k*res*res ];
			for( int kk=-BSplineSupportSizes< DataDegree >::SupportEnd ; kk<=-BSplineSupportSizes< DataDegree >::SupportStart ; kk++ ) if( k+kk>=begin && k+kk<end )
				for( int jj=-BSplineSupportSizes< DataDegree >::SupportEnd ; jj<=-BSplineSupportSizes< DataDegree >::SupportStart ; jj++ ) if( j+jj>=begin && j+jj<end )
				{
					double weight = evaluator.value( k+kk , k , false ) * evaluator.value( j+jj , j , false );
					int idx = (j+jj-begin)*dim + (k+kk-begin)*dim*dim;
					for( int ii=-BSplineSupportSizes< DataDegree >::SupportEnd ; ii<=-BSplineSupportSizes< DataDegree >::SupportStart ; ii++ ) if( i+ii>=begin && i+ii<end )
						value += _coefficients[ i + ii - begin + idx ] * Real( weight * evaluator.value( i+ii , i , false ) );
				}
		}
	}
	memoryUsage();
	DeletePointer( _coefficients );
	for( int i=0 ; i<res*res*res ; i++ ) values[i] -= isoValue;

	return values;
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
SparseNodeData< Real , 0 > Octree< Real >::leafValues( const DenseNodeData< Real , FEMDegree >& coefficients ) const
{
	SparseNodeData< Real , 0 > values;
	DenseNodeData< Real , FEMDegree > _coefficients( _sNodesEnd(_maxDepth-1) );
	memset( &_coefficients[0] , 0 , sizeof(Real)*_sNodesEnd(_maxDepth-1) );
	for( int i=_sNodes.begin( _localToGlobal( 0 ) ) ; i<_sNodesEnd(_maxDepth-1) ; i++ ) _coefficients[i] = coefficients[i];
	for( LocalDepth d=1 ; d<_maxDepth ; d++ ) _upSample( d , _coefficients );
	for( LocalDepth d=_maxDepth ; d>=0 ; d-- )
	{
		_Evaluator< FEMDegree , BType > evaluator;
		evaluator.set( d );
		std::vector< ConstPointSupportKey< FEMDegree > > neighborKeys( std::max< int >( 1 , threads ) );
		for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d ) );
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( _isValidSpaceNode( _sNodes.treeNodes[i] ) )
		{
			ConstPointSupportKey< FEMDegree >& neighborKey = neighborKeys[ omp_get_thread_num() ];
			TreeOctNode* node = _sNodes.treeNodes[i];
			if( !IsActiveNode( node->children ) )
			{
				neighborKey.getNeighbors( node );
				bool isInterior = _IsInteriorlySupported< FEMDegree >( node->parent );
				values[ node ] = _getCenterValue( neighborKey , node , coefficients , _coefficients , evaluator , isInterior );
			}
		}
	}
	return values;
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
SparseNodeData< Point3D< Real > , 0 > Octree< Real >::leafGradients( const DenseNodeData< Real , FEMDegree >& coefficients ) const
{
	SparseNodeData< Point3D< Real > , 0 > gradients;
	DenseNodeData< Real , FEMDegree > _coefficients( _sNodesEnd(_maxDepth-1 ) );
	memset( &_coefficients[0] , 0 , sizeof(Real)*_sNodesEnd(_maxDepth-1) );
	for( int i=_sNodesBegin(0) ; i<_sNodesEnd(_maxDepth-1) ; i++ ) _coefficients[i] = coefficients[i];
	for( LocalDepth d=1 ; d<_maxDepth ; d++ ) _upSample( d , _coefficients );
	for( LocalDepth d=_maxDepth ; d>=0 ; d-- )
	{
		_Evaluator< FEMDegree , BType > evaluator;
		evaluator.set( d );
		std::vector< ConstPointSupportKey< FEMDegree > > neighborKeys( std::max< int >( 1 , threads ) );
		for( size_t i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( _localToGlobal( d ) );
		for( int i=_sNodesBegin(d) ; i<_sNodesEnd(d) ; i++ ) if( _isValidSpaceNode( _sNodes.treeNodes[i] ) )
		{
			ConstPointSupportKey< FEMDegree >& neighborKey = neighborKeys[ omp_get_thread_num() ];
			TreeOctNode* node = _sNodes.treeNodes[i];
			if( !IsActiveNode( node->children ) )
			{
				neighborKey.getNeighbors( node );
				bool isInterior = _IsInteriorlySupported< FEMDegree >( node->parent );
				gradients[ node ] = _getCenterValueAndGradient( neighborKey , node , coefficients , _coefficients , evaluator , isInterior ).second;
			}
		}
	}
	return gradients;
}
