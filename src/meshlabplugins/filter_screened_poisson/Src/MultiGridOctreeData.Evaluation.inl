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

template< class Real >
template< int FEMDegree , BoundaryType BType>
void Octree< Real >::_Evaluator< FEMDegree , BType >::set( LocalDepth depth )
{
	static const int  LeftPointSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;

	BSplineEvaluationData< FEMDegree , BType >::SetEvaluator( evaluator , depth );
	if( depth>0 ) BSplineEvaluationData< FEMDegree , BType >::SetChildEvaluator( childEvaluator , depth-1 );
	int center = ( 1<<depth )>>1;

	// First set the stencils for the current depth
	for( int x=-LeftPointSupportRadius ; x<=RightPointSupportRadius ; x++ ) for( int y=-LeftPointSupportRadius ; y<=RightPointSupportRadius ; y++ ) for( int z=-LeftPointSupportRadius ; z<=RightPointSupportRadius ; z++ )
	{
		int fIdx[] = { center+x , center+y , center+z };

		// The cell stencil
		{
			double vv[3] , dv[3];
			for( int dd=0 ; dd<DIMENSION ; dd++ )
			{
				vv[dd] = evaluator.centerValue( fIdx[dd] , center , false );
				dv[dd] = evaluator.centerValue( fIdx[dd] , center , true  );
			}
			cellStencil( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
			dCellStencil( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
		}

		//// The face stencil
		for( unsigned int f=0 ; f<Cube::FACES ; f++ )
		{
			int dir , off;
			Cube::FactorFaceIndex( f , dir , off );
			double vv[3] = {0,0,0} , dv[3]={0,0,0};
			switch( dir )
			{
			case 0:
				vv[0] = evaluator.cornerValue( fIdx[0] , center+off , false );
				vv[1] = evaluator.centerValue( fIdx[1] , center     , false );
				vv[2] = evaluator.centerValue( fIdx[2] , center     , false );
				dv[0] = evaluator.cornerValue( fIdx[0] , center+off , true  );
				dv[1] = evaluator.centerValue( fIdx[1] , center     , true  );
				dv[2] = evaluator.centerValue( fIdx[2] , center     , true  );
				break;
			case 1:
				vv[0] = evaluator.centerValue( fIdx[0] , center     , false );
				vv[1] = evaluator.cornerValue( fIdx[1] , center+off , false );
				vv[2] = evaluator.centerValue( fIdx[2] , center     , false );
				dv[0] = evaluator.centerValue( fIdx[0] , center     , true  );
				dv[1] = evaluator.cornerValue( fIdx[1] , center+off , true  );
				dv[2] = evaluator.centerValue( fIdx[2] , center     , true  );
				break;
			case 2:
				vv[0] = evaluator.centerValue( fIdx[0] , center     , false );
				vv[1] = evaluator.centerValue( fIdx[1] , center     , false );
				vv[2] = evaluator.cornerValue( fIdx[2] , center+off , false );
				dv[0] = evaluator.centerValue( fIdx[0] , center     , true  );
				dv[1] = evaluator.centerValue( fIdx[1] , center     , true  );
				dv[2] = evaluator.cornerValue( fIdx[2] , center+off , true  );
				break;
			}
			faceStencil[f]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
			dFaceStencil[f]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
		}

		//// The edge stencil
		for( unsigned int e=0 ; e<Cube::EDGES ; e++ )
		{
			int orientation , i1 , i2;
			Cube::FactorEdgeIndex( e , orientation , i1 , i2 );
			double vv[3] = {0,0,0}, dv[3] = {0,0,0};
			switch( orientation )
			{
			case 0:
				vv[0] = evaluator.centerValue( fIdx[0] , center    , false );
				vv[1] = evaluator.cornerValue( fIdx[1] , center+i1 , false );
				vv[2] = evaluator.cornerValue( fIdx[2] , center+i2 , false );
				dv[0] = evaluator.centerValue( fIdx[0] , center    , true  );
				dv[1] = evaluator.cornerValue( fIdx[1] , center+i1 , true  );
				dv[2] = evaluator.cornerValue( fIdx[2] , center+i2 , true  );
				break;
			case 1:
				vv[0] = evaluator.cornerValue( fIdx[0] , center+i1 , false );
				vv[1] = evaluator.centerValue( fIdx[1] , center    , false );
				vv[2] = evaluator.cornerValue( fIdx[2] , center+i2 , false );
				dv[0] = evaluator.cornerValue( fIdx[0] , center+i1 , true  );
				dv[1] = evaluator.centerValue( fIdx[1] , center    , true  );
				dv[2] = evaluator.cornerValue( fIdx[2] , center+i2 , true  );
				break;
			case 2:
				vv[0] = evaluator.cornerValue( fIdx[0] , center+i1 , false );
				vv[1] = evaluator.cornerValue( fIdx[1] , center+i2 , false );
				vv[2] = evaluator.centerValue( fIdx[2] , center    , false );
				dv[0] = evaluator.cornerValue( fIdx[0] , center+i1 , true  );
				dv[1] = evaluator.cornerValue( fIdx[1] , center+i2 , true  );
				dv[2] = evaluator.centerValue( fIdx[2] , center    , true  );
				break;
			}
			edgeStencil[e]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
			dEdgeStencil[e]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
		}

		//// The corner stencil
		for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
		{
			int cx , cy  ,cz;
			Cube::FactorCornerIndex( c , cx , cy , cz );
			double vv[3] , dv[3];
			vv[0] = evaluator.cornerValue( fIdx[0] , center+cx , false );
			vv[1] = evaluator.cornerValue( fIdx[1] , center+cy , false );
			vv[2] = evaluator.cornerValue( fIdx[2] , center+cz , false );
			dv[0] = evaluator.cornerValue( fIdx[0] , center+cx , true  );
			dv[1] = evaluator.cornerValue( fIdx[1] , center+cy , true  );
			dv[2] = evaluator.cornerValue( fIdx[2] , center+cz , true  );
			cornerStencil[c]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
			dCornerStencil[c]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
		}
	}

	// Now set the stencils for the parents
	for( int child=0 ; child<CHILDREN ; child++ )
	{
		int childX , childY , childZ;
		Cube::FactorCornerIndex( child , childX , childY , childZ );
		for( int x=-LeftPointSupportRadius ; x<=RightPointSupportRadius ; x++ ) for( int y=-LeftPointSupportRadius ; y<=RightPointSupportRadius ; y++ ) for( int z=-LeftPointSupportRadius ; z<=RightPointSupportRadius ; z++ )
		{
			int fIdx[] = { center/2+x , center/2+y , center/2+z };

			//// The cell stencil
			{
				double vv[3] , dv[3];
				vv[0] = childEvaluator.centerValue( fIdx[0] , center+childX , false );
				vv[1] = childEvaluator.centerValue( fIdx[1] , center+childY , false );
				vv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ , false );
				dv[0] = childEvaluator.centerValue( fIdx[0] , center+childX , true  );
				dv[1] = childEvaluator.centerValue( fIdx[1] , center+childY , true  );
				dv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ , true  );
				cellStencils[child]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
				dCellStencils[child]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
			}

			//// The face stencil
			for( unsigned int f=0 ; f<Cube::FACES ; f++ )
			{
				int dir , off;
				Cube::FactorFaceIndex( f , dir , off );
				double vv[3] = {0,0,0}, dv[3] = {0,0,0};
				switch( dir )
				{
				case 0:
					vv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+off , false );
					vv[1] = childEvaluator.centerValue( fIdx[1] , center+childY     , false );
					vv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ     , false );
					dv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+off , true  );
					dv[1] = childEvaluator.centerValue( fIdx[1] , center+childY     , true  );
					dv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ     , true  );
					break;
				case 1:
					vv[0] = childEvaluator.centerValue( fIdx[0] , center+childX     , false );
					vv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+off , false );
					vv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ     , false );
					dv[0] = childEvaluator.centerValue( fIdx[0] , center+childX     , true  );
					dv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+off , true  );
					dv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ     , true  );
					break;
				case 2:
					vv[0] = childEvaluator.centerValue( fIdx[0] , center+childX     , false );
					vv[1] = childEvaluator.centerValue( fIdx[1] , center+childY     , false );
					vv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+off , false );
					dv[0] = childEvaluator.centerValue( fIdx[0] , center+childX     , true  );
					dv[1] = childEvaluator.centerValue( fIdx[1] , center+childY     , true  );
					dv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+off , true  );
					break;
				}
				faceStencils[child][f]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
				dFaceStencils[child][f]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
			}

			//// The edge stencil
			for( unsigned int e=0 ; e<Cube::EDGES ; e++ )
			{
				int orientation , i1 , i2;
				Cube::FactorEdgeIndex( e , orientation , i1 , i2 );
				double vv[3] = {0,0,0} , dv[3]={0,0,0};
				switch( orientation )
				{
				case 0:
					vv[0] = childEvaluator.centerValue( fIdx[0] , center+childX    , false );
					vv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+i1 , false );
					vv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+i2 , false );
					dv[0] = childEvaluator.centerValue( fIdx[0] , center+childX    , true  );
					dv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+i1 , true  );
					dv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+i2 , true  );
					break;
				case 1:
					vv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+i1 , false );
					vv[1] = childEvaluator.centerValue( fIdx[1] , center+childY    , false );
					vv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+i2 , false );
					dv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+i1 , true  );
					dv[1] = childEvaluator.centerValue( fIdx[1] , center+childY    , true  );
					dv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+i2 , true  );
					break;
				case 2:
					vv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+i1 , false );
					vv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+i2 , false );
					vv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ    , false );
					dv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+i1 , true  );
					dv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+i2 , true  );
					dv[2] = childEvaluator.centerValue( fIdx[2] , center+childZ    , true  );
					break;
				}
				edgeStencils[child][e]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
				dEdgeStencils[child][e]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
			}

			//// The corner stencil
			for( unsigned int c=0 ; c<Cube::CORNERS ; c++ )
			{
				int cx , cy  ,cz;
				Cube::FactorCornerIndex( c , cx , cy , cz );
				double vv[3] , dv[3];
				vv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+cx , false );
				vv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+cy , false );
				vv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+cz , false );
				dv[0] = childEvaluator.cornerValue( fIdx[0] , center+childX+cx , true  );
				dv[1] = childEvaluator.cornerValue( fIdx[1] , center+childY+cy , true  );
				dv[2] = childEvaluator.cornerValue( fIdx[2] , center+childZ+cz , true  );
				cornerStencils[child][c]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = vv[0] * vv[1] * vv[2];
				dCornerStencils[child][c]( x+LeftPointSupportRadius , y+LeftPointSupportRadius , z+LeftPointSupportRadius ) = Point3D< double >( dv[0] * vv[1] * vv[2] , vv[0] * dv[1] * vv[2] , vv[0] * vv[1] * dv[2] );
			}
		}
	}
	if( _bsData ) delete _bsData;
	_bsData = new BSplineData< FEMDegree , BType >( depth );
}
template< class Real >
template< class V , int FEMDegree , BoundaryType BType >
V Octree< Real >::_getValue( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , Point3D< Real > p , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	//static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int  LeftPointSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;

	if( IsActiveNode( node->children ) ) fprintf( stderr , "[WARNING] getValue assumes leaf node\n" );
	V value(0);

	while( GetGhostFlag( node ) )
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );

		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* _n = neighbors.neighbors[i][j][k];

			if( _isValidFEMNode( _n ) )
			{
				int _pIdx[3];
				Point3D< Real > _s ; Real _w;
				_startAndWidth( _n , _s , _w );
				int _fIdx[3];
				functionIndex< FEMDegree , BType >( _n , _fIdx );
				for( int dd=0 ; dd<3 ; dd++ ) _pIdx[dd] = std::max< int >( 0 , std::min< int >( SupportSize-1 , LeftSupportRadius + (int)floor( ( p[dd]-_s[dd] ) / _w ) ) );
				value += 
					solution[ _n->nodeData.nodeIndex ] *
					(Real)
					(
						evaluator._bsData->baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) *
						evaluator._bsData->baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) *
						evaluator._bsData->baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
						);
			}
		}
		node = node->parent;
	}

	LocalDepth d = _localDepth( node );

	for( int dd=0 ; dd<3 ; dd++ )
		if     ( p[dd]==0 )
			p[dd] = (Real)(0.+1e-6);
		else if( p[dd]==1 )
			p[dd] = (Real)(1.-1e-6);
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );

		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* _n = neighbors.neighbors[i][j][k];
			if( _isValidFEMNode( _n ) )
			{
				int _pIdx[3];
				Point3D< Real > _s ; Real _w;
				_startAndWidth( _n , _s , _w );
				int _fIdx[3];
				functionIndex< FEMDegree , BType >( _n , _fIdx );
				for( int dd=0 ; dd<3 ; dd++ ) _pIdx[dd] = std::max< int >( 0 , std::min< int >( SupportSize-1 , LeftSupportRadius + (int)floor( ( p[dd]-_s[dd] ) / _w ) ) );
				value +=
				        solution[ _n->nodeData.nodeIndex ] *
				        (Real)
				        (
				            evaluator._bsData->baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) *
				        evaluator._bsData->baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) *
				        evaluator._bsData->baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
				        );
			}
		}
		if( d>0 )
		{
			const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
			for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
			{
				const TreeOctNode* _n = neighbors.neighbors[i][j][k];
				if( _isValidFEMNode( _n ) )
				{
					int _pIdx[3];
					Point3D< Real > _s ; Real _w;
					_startAndWidth( _n , _s , _w );
					int _fIdx[3];
					functionIndex< FEMDegree , BType >( _n , _fIdx );
					for( int dd=0 ; dd<3 ; dd++ ) _pIdx[dd] = std::max< int >( 0 , std::min< int >( SupportSize-1 , LeftSupportRadius + (int)floor( ( p[dd]-_s[dd] ) / _w ) ) );
					value +=
					        coarseSolution[ _n->nodeData.nodeIndex ] *
					        (Real)
					        (
					            evaluator._bsData->baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) *
					        evaluator._bsData->baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) *
					        evaluator._bsData->baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
					        );
				}
			}
		}
	}
	return value;
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
std::pair< Real , Point3D< Real > > Octree< Real >::_getValueAndGradient( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , Point3D< Real > p , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftSupportRadius = -BSplineSupportSizes< FEMDegree >::SupportStart;
	static const int RightSupportRadius =  BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int  LeftPointSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;

	if( IsActiveNode( node->children ) ) fprintf( stderr , "[WARNING] _getValueAndGradient assumes leaf node\n" );
	Real value(0);
	Point3D< Real > gradient;

	while( GetGhostFlag( node ) )
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );

		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* _n = neighbors.neighbors[i][j][k];

			if( _isValidFEMNode( _n ) )
			{
				int _pIdx[3];
				Point3D< Real > _s; Real _w;
				_startAndWidth( _n , _s , _w );
				int _fIdx[3];
				functionIndex< FEMDegree , BType >( _n , _fIdx );
				for( int dd=0 ; dd<3 ; dd++ ) _pIdx[dd] = std::max< int >( 0 , std::min< int >( SupportSize-1 , LeftSupportRadius + (int)floor( ( p[dd]-_s[dd] ) / _w ) ) );
				value += 
					solution[ _n->nodeData.nodeIndex ] *
					(Real)
					(
						evaluator._bsData->baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData->baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData->baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
					);
				gradient += 
					Point3D< Real >
					(
						evaluator._bsData->dBaseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData-> baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData-> baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] ) ,
						evaluator._bsData-> baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData->dBaseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData-> baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] ) ,
						evaluator._bsData-> baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData-> baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData->dBaseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
					) * solution[ _n->nodeData.nodeIndex ];
			}
		}
		node = node->parent;
	}


	LocalDepth d = _localDepth( node );

	for( int dd=0 ; dd<3 ; dd++ )
		if     ( p[dd]==0 )
			p[dd] = (Real)(0.+1e-6);
		else if( p[dd]==1 )
			p[dd] = (Real)(1.-1e-6);

	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );

		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* _n = neighbors.neighbors[i][j][k];

			if( _isValidFEMNode( _n ) )
			{
				int _pIdx[3];
				Point3D< Real > _s ; Real _w;
				_startAndWidth( _n , _s , _w );
				int _fIdx[3];
				functionIndex< FEMDegree , BType >( _n , _fIdx );
				for( int dd=0 ; dd<3 ; dd++ ) _pIdx[dd] = std::max< int >( 0 , std::min< int >( SupportSize-1 , LeftSupportRadius + (int)floor( ( p[dd]-_s[dd] ) / _w ) ) );
				value +=
				        solution[ _n->nodeData.nodeIndex ] *
				        (Real)
				        (
				            evaluator._bsData->baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData->baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData->baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
				        );
				gradient +=
				        Point3D< Real >
				        (
				            evaluator._bsData->dBaseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData-> baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData-> baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] ) ,
				        evaluator._bsData-> baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData->dBaseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData-> baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] ) ,
				        evaluator._bsData-> baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData-> baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData->dBaseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
				        ) * solution[ _n->nodeData.nodeIndex ];
			}
		}
		if( d>0 )
		{
			const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
			for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
			{
				const TreeOctNode* _n = neighbors.neighbors[i][j][k];

				if( _isValidFEMNode( _n ) )
				{
					int _pIdx[3];
					Point3D< Real > _s ; Real _w;
					_startAndWidth( _n , _s , _w );
					int _fIdx[3];
					functionIndex< FEMDegree , BType >( _n , _fIdx );
					for( int dd=0 ; dd<3 ; dd++ ) _pIdx[dd] = std::max< int >( 0 , std::min< int >( SupportSize-1 , LeftSupportRadius + (int)floor( ( p[dd]-_s[dd] ) / _w ) ) );
					value +=
					        coarseSolution[ _n->nodeData.nodeIndex ] *
					        (Real)
					        (
					            evaluator._bsData->baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData->baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData->baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
					        );
					gradient +=
					        Point3D< Real >
					        (
					            evaluator._bsData->dBaseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData-> baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData-> baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] ) ,
					        evaluator._bsData-> baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData->dBaseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData-> baseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] ) ,
					        evaluator._bsData-> baseBSplines[ _fIdx[0] ][ _pIdx[0] ]( p[0] ) * evaluator._bsData-> baseBSplines[ _fIdx[1] ][ _pIdx[1] ]( p[1] ) * evaluator._bsData->dBaseBSplines[ _fIdx[2] ][ _pIdx[2] ]( p[2] )
					        ) * coarseSolution[ _n->nodeData.nodeIndex ];
				}
			}
		}
	}
	return std::pair< Real , Point3D< Real > >( value , gradient );
}
template< class Real >
template< class V , int FEMDegree , BoundaryType BType >
V Octree< Real >::_getCenterValue( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const
{
	static const int SupportSize = BSplineEvaluationData< FEMDegree , BType >::SupportSize;
	static const int  LeftPointSupportRadius =   BSplineEvaluationData< FEMDegree , BType >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineEvaluationData< FEMDegree , BType >::SupportStart;

	if( IsActiveNode( node->children ) ) fprintf( stderr , "[WARNING] getCenterValue assumes leaf node\n" );
	V value(0);
	LocalDepth d = _localDepth( node );

	if( isInterior )
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );
		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* n = neighbors.neighbors[i][j][k];
			if( IsActiveNode( n ) ) value += solution[ n->nodeData.nodeIndex ] * Real( evaluator.cellStencil( i , j , k ) );
		}
		if( d>0 )
		{
			int _corner = int( node - node->parent->children );
			const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
			for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
			{
				const TreeOctNode* n = neighbors.neighbors[i][j][k];
				if( IsActiveNode( n ) ) value += coarseSolution[n->nodeData.nodeIndex] * Real( evaluator.cellStencils[_corner]( i , j , k ) );
			}
		}
	}
	else
	{
		LocalOffset cIdx;
		_localDepthAndOffset( node , d , cIdx );
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );

		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* n = neighbors.neighbors[i][j][k];

			if( _isValidFEMNode( n ) )
			{
				LocalDepth _d ; LocalOffset fIdx;
				_localDepthAndOffset( n , _d , fIdx );
				value +=
					solution[ n->nodeData.nodeIndex ] *
					Real(
						evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , false ) *
						evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , false ) *
						evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , false )
					);
			}
		}
		if( d>0 )
		{
			const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
			for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
			{
				const TreeOctNode* n = neighbors.neighbors[i][j][k];
				if( _isValidFEMNode( n ) )
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( n , _d , fIdx );
					value +=
						coarseSolution[ n->nodeData.nodeIndex ] *
						Real(
							evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , false ) *
							evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , false ) *
							evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , false )
						);
				}
			}
		}
	}
	return value;
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
std::pair< Real , Point3D< Real > > Octree< Real >::_getCenterValueAndGradient( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const
{
	static const int SupportSize = BSplineEvaluationData< FEMDegree , BType >::SupportSize;
	static const int  LeftPointSupportRadius =   BSplineEvaluationData< FEMDegree , BType >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineEvaluationData< FEMDegree , BType >::SupportStart;

	if( IsActiveNode( node->children ) ) fprintf( stderr , "[WARNING] getCenterValueAndGradient assumes leaf node\n" );
	Real value(0);
	Point3D< Real > gradient;
	LocalDepth d = _localDepth( node );

	if( isInterior )
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );
		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* n = neighbors.neighbors[i][j][k];
			if( IsActiveNode( n ) )
			{
				value    +=          Real  ( evaluator. cellStencil( i , j , k ) ) * solution[ n->nodeData.nodeIndex ];
				gradient += Point3D< Real >( evaluator.dCellStencil( i , j , k ) ) * solution[ n->nodeData.nodeIndex ];
			}
		}
		if( d>0 )
		{
			int _corner = int( node - node->parent->children );
			const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
			for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
			{
				const TreeOctNode* n = neighbors.neighbors[i][j][k];
				if( IsActiveNode( n ) )
				{
					value    +=          Real  ( evaluator. cellStencils[_corner]( i , j , k ) ) * coarseSolution[n->nodeData.nodeIndex];
					gradient += Point3D< Real >( evaluator.dCellStencils[_corner]( i , j , k ) ) * coarseSolution[n->nodeData.nodeIndex];
				}
			}
		}
	}
	else
	{
		LocalOffset cIdx;
		_localDepthAndOffset( node , d , cIdx );
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );

		for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
		{
			const TreeOctNode* n = neighbors.neighbors[i][j][k];

			if( _isValidFEMNode( n ) )
			{
				LocalDepth _d ; LocalOffset fIdx;
				_localDepthAndOffset( n , _d , fIdx );
				value +=
					Real
					(
						evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , false ) * evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , false ) * evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , false )
					) * solution[ n->nodeData.nodeIndex ];
				gradient += 
					Point3D< Real >
					(
						evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , true  ) * evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , false ) * evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , false ) ,
						evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , false ) * evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , true  ) * evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , false ) ,
						evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , false ) * evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , false ) * evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , true  )
						) * solution[ n->nodeData.nodeIndex ];
			}
		}
		if( d>0 )
		{
			const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
			for( int i=0 ; i<SupportSize ; i++ ) for( int j=0 ; j<SupportSize ; j++ ) for( int k=0 ; k<SupportSize ; k++ )
			{
				const TreeOctNode* n = neighbors.neighbors[i][j][k];
				if( _isValidFEMNode( n ) )
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( n , _d , fIdx );
					value +=
						Real
						(
							evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , false ) * evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , false ) * evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , false )
						) * coarseSolution[ n->nodeData.nodeIndex ];
					gradient +=
						Point3D< Real >
						(
							evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , true  ) * evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , false ) * evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , false ) ,
							evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , false ) * evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , true  ) * evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , false ) ,
							evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , false ) * evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , false ) * evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , true  )
						) * coarseSolution[ n->nodeData.nodeIndex ];
				}
			}
		}
	}
	return std::pair< Real , Point3D< Real > >( value , gradient );
}
template< class Real >
template< class V , int FEMDegree , BoundaryType BType >
V Octree< Real >::_getEdgeValue( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int edge , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const
{
	static const int SupportSize = BSplineEvaluationData< FEMDegree , BType >::SupportSize;
	static const int  LeftPointSupportRadius =  BSplineEvaluationData< FEMDegree , BType >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineEvaluationData< FEMDegree , BType >::SupportStart;
	V value(0);
	LocalDepth d ; LocalOffset cIdx;
	_localDepthAndOffset( node , d , cIdx );
	int startX = 0 , endX = SupportSize , startY = 0 , endY = SupportSize , startZ = 0 , endZ = SupportSize;
	int orientation , i1 , i2;
	Cube::FactorEdgeIndex( edge , orientation , i1 , i2 );
	switch( orientation )
	{
	case 0:
		cIdx[1] += i1 , cIdx[2] += i2;
		if( i1 ) startY++ ; else endY--;
		if( i2 ) startZ++ ; else endZ--;
		break;
	case 1:
		cIdx[0] += i1 , cIdx[2] += i2;
		if( i1 ) startX++ ; else endX--;
		if( i2 ) startZ++ ; else endZ--;
		break;
	case 2:
		cIdx[0] += i1 , cIdx[1] += i2;
		if( i1 ) startX++ ; else endX--;
		if( i2 ) startY++ ; else endY--;
		break;
	}

	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , d );
		for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
		{
			const TreeOctNode* _node = neighbors.neighbors[x][y][z];
			if( _isValidFEMNode( _node ) )
			{
				if( isInterior ) value += solution[ _node->nodeData.nodeIndex ] * evaluator.edgeStencil[edge]( x , y , z );
				else
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					switch( orientation )
					{
					case 0:
						value +=
							solution[ _node->nodeData.nodeIndex ] *
							Real(
								evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , false ) *
								evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , false ) *
								evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , false )
							);
						break;
					case 1:
						value +=
							solution[ _node->nodeData.nodeIndex ] *
							Real(
								evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , false ) *
								evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , false ) *
								evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , false )
							);
						break;
					case 2:
						value +=
							solution[ _node->nodeData.nodeIndex ] *
							Real(
								evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , false ) *
								evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , false ) *
								evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , false )
							);
						break;
					}
				}
			}
		}
	}
	if( d>0 )
	{
		int _corner = int( node - node->parent->children );
		int _cx , _cy , _cz;
		Cube::FactorCornerIndex( _corner , _cx , _cy , _cz );
		// If the corner/child indices don't match, then the sample position is in the interior of the
		// coarser cell and so the full support resolution should be used.
		switch( orientation )
		{
		case 0:
			if( _cy!=i1 ) startY = 0 , endY = SupportSize;
			if( _cz!=i2 ) startZ = 0 , endZ = SupportSize;
			break;
		case 1:
			if( _cx!=i1 ) startX = 0 , endX = SupportSize;
			if( _cz!=i2 ) startZ = 0 , endZ = SupportSize;
			break;
		case 2:
			if( _cx!=i1 ) startX = 0 , endX = SupportSize;
			if( _cy!=i2 ) startY = 0 , endY = SupportSize;
			break;
		}
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
		for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
		{
			const TreeOctNode* _node = neighbors.neighbors[x][y][z];
			if( _isValidFEMNode( _node ) )
			{
				if( isInterior ) value += coarseSolution[ _node->nodeData.nodeIndex ] * evaluator.edgeStencils[_corner][edge]( x , y , z );
				else
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					switch( orientation )
					{
					case 0:
						value +=
							coarseSolution[ _node->nodeData.nodeIndex ] *
							Real(
								evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , false ) *
								evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , false ) *
								evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , false )
							);
						break;
					case 1:
						value +=
							coarseSolution[ _node->nodeData.nodeIndex ] *
							Real(
								evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , false ) *
								evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , false ) *
								evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , false )
							);
						break;
					case 2:
						value +=
							coarseSolution[ _node->nodeData.nodeIndex ] *
							Real(
								evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , false ) *
								evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , false ) *
								evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , false )
							);
						break;
					}
				}
			}
		}
	}
	return Real( value );
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
std::pair< Real , Point3D< Real > > Octree< Real >::_getEdgeValueAndGradient( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int edge , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const
{
	static const int SupportSize = BSplineEvaluationData< FEMDegree , BType >::SupportSize;
	static const int  LeftPointSupportRadius =  BSplineEvaluationData< FEMDegree , BType >::SupportEnd;
	static const int RightPointSupportRadius = -BSplineEvaluationData< FEMDegree , BType >::SupportStart;
	double value = 0;
	Point3D< double > gradient;
	LocalDepth d ; LocalOffset cIdx;
	_localDepthAndOffset( node , d , cIdx );

	int startX = 0 , endX = SupportSize , startY = 0 , endY = SupportSize , startZ = 0 , endZ = SupportSize;
	int orientation , i1 , i2;
	Cube::FactorEdgeIndex( edge , orientation , i1 , i2 );
	switch( orientation )
	{
	case 0:
		cIdx[1] += i1 , cIdx[2] += i2;
		if( i1 ) startY++ ; else endY--;
		if( i2 ) startZ++ ; else endZ--;
		break;
	case 1:
		cIdx[0] += i1 , cIdx[2] += i2;
		if( i1 ) startX++ ; else endX--;
		if( i2 ) startZ++ ; else endZ--;
		break;
	case 2:
		cIdx[0] += i1 , cIdx[1] += i2;
		if( i1 ) startX++ ; else endX--;
		if( i2 ) startY++ ; else endY--;
		break;
	}
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );
		for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
		{
			const TreeOctNode* _node = neighbors.neighbors[x][y][z];
			if( _isValidFEMNode( _node ) )
			{
				if( isInterior )
				{
					value    += evaluator. edgeStencil[edge]( x , y , z ) * solution[ _node->nodeData.nodeIndex ];
					gradient += evaluator.dEdgeStencil[edge]( x , y , z ) * solution[ _node->nodeData.nodeIndex ];
				}
				else
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );

					double vv[3] , dv[3];
					switch( orientation )
					{
					case 0:
						vv[0] = evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , false );
						vv[1] = evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , false );
						vv[2] = evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , false );
						dv[0] = evaluator.evaluator.centerValue( fIdx[0] , cIdx[0] , true  );
						dv[1] = evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , true  );
						dv[2] = evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , true  );
						break;
					case 1:
						vv[0] = evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , false );
						vv[1] = evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , false );
						vv[2] = evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , false );
						dv[0] = evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , true  );
						dv[1] = evaluator.evaluator.centerValue( fIdx[1] , cIdx[1] , true  );
						dv[2] = evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , true  );
						break;
					case 2:
						vv[0] = evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , false );
						vv[1] = evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , false );
						vv[2] = evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , false );
						dv[0] = evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , true  );
						dv[1] = evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , true  );
						dv[2] = evaluator.evaluator.centerValue( fIdx[2] , cIdx[2] , true  );
						break;
					}
					value += solution[ _node->nodeData.nodeIndex ] * vv[0] * vv[1] * vv[2];
					gradient += Point3D< double >( dv[0]*vv[1]*vv[2] , vv[0]*dv[1]*vv[2] , vv[0]*vv[1]*dv[2] ) * solution[ _node->nodeData.nodeIndex ];
				}
			}
		}
	}
	if( d>0 )
	{
		int _corner = int( node - node->parent->children );
		int _cx , _cy , _cz;
		Cube::FactorCornerIndex( _corner , _cx , _cy , _cz );
		// If the corner/child indices don't match, then the sample position is in the interior of the
		// coarser cell and so the full support resolution should be used.
		switch( orientation )
		{
		case 0:
			if( _cy!=i1 ) startY = 0 , endY = SupportSize;
			if( _cz!=i2 ) startZ = 0 , endZ = SupportSize;
			break;
		case 1:
			if( _cx!=i1 ) startX = 0 , endX = SupportSize;
			if( _cz!=i2 ) startZ = 0 , endZ = SupportSize;
			break;
		case 2:
			if( _cx!=i1 ) startX = 0 , endX = SupportSize;
			if( _cy!=i2 ) startY = 0 , endY = SupportSize;
			break;
		}
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
		for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
		{
			const TreeOctNode* _node = neighbors.neighbors[x][y][z];
			if( _isValidFEMNode( _node ) )
			{
				if( isInterior )
				{
					value    += evaluator. edgeStencils[_corner][edge]( x , y , z ) * coarseSolution[ _node->nodeData.nodeIndex ];
					gradient += evaluator.dEdgeStencils[_corner][edge]( x , y , z ) * coarseSolution[ _node->nodeData.nodeIndex ];
				}
				else
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					double vv[3] , dv[3];
					switch( orientation )
					{
					case 0:
						vv[0] = evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , false );
						vv[1] = evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , false );
						vv[2] = evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , false );
						dv[0] = evaluator.childEvaluator.centerValue( fIdx[0] , cIdx[0] , true  );
						dv[1] = evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , true  );
						dv[2] = evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , true  );
						break;
					case 1:
						vv[0] = evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , false );
						vv[1] = evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , false );
						vv[2] = evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , false );
						dv[0] = evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , true  );
						dv[1] = evaluator.childEvaluator.centerValue( fIdx[1] , cIdx[1] , true  );
						dv[2] = evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , true  );
						break;
					case 2:
						vv[0] = evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , false );
						vv[1] = evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , false );
						vv[2] = evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , false );
						dv[0] = evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , true  );
						dv[1] = evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , true  );
						dv[2] = evaluator.childEvaluator.centerValue( fIdx[2] , cIdx[2] , true  );
						break;
					}
					value += coarseSolution[ _node->nodeData.nodeIndex ] * vv[0] * vv[1] * vv[2];
					gradient += Point3D< double >( dv[0]*vv[1]*vv[2] , vv[0]*dv[1]*vv[2] , vv[0]*vv[1]*dv[2] ) * coarseSolution[ _node->nodeData.nodeIndex ];
				}
			}
		}
	}
	return std::pair< Real , Point3D< Real > >( Real( value ) , Point3D< Real >( gradient ) );
}

template< class Real >
template< class V , int FEMDegree , BoundaryType BType >
V Octree< Real >::_getCornerValue( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int corner , const DenseNodeData< V , FEMDegree >& solution , const DenseNodeData< V , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftPointSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;

	V value(0);
	LocalDepth d ; LocalOffset cIdx;
	_localDepthAndOffset( node , d , cIdx );

	int cx , cy , cz;
	int startX = 0 , endX = SupportSize , startY = 0 , endY = SupportSize , startZ = 0 , endZ = SupportSize;
	Cube::FactorCornerIndex( corner , cx , cy , cz );
	cIdx[0] += cx , cIdx[1] += cy , cIdx[2] += cz;
	{
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );
		if( cx==0 ) endX--;
		else      startX++;
		if( cy==0 ) endY--;
		else      startY++;
		if( cz==0 ) endZ--;
		else      startZ++;
		if( isInterior )
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node=neighbors.neighbors[x][y][z];
				if( IsActiveNode( _node ) ) value += solution[ _node->nodeData.nodeIndex ] * Real( evaluator.cornerStencil[corner]( x , y , z ) );
			}
		else
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node = neighbors.neighbors[x][y][z];
				if( _isValidFEMNode( _node ) )
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					value +=
						solution[ _node->nodeData.nodeIndex ] *
						Real(
							evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , false ) *
							evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , false ) *
							evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , false )
						);
				}
			}
	}
	if( d>0 )
	{
		int _corner = int( node - node->parent->children );
		int _cx , _cy , _cz;
		Cube::FactorCornerIndex( _corner , _cx , _cy , _cz );
		// If the corner/child indices don't match, then the sample position is in the interior of the
		// coarser cell and so the full support resolution should be used.
		if( cx!=_cx ) startX = 0 , endX = SupportSize;
		if( cy!=_cy ) startY = 0 , endY = SupportSize;
		if( cz!=_cz ) startZ = 0 , endZ = SupportSize;
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
		if( isInterior )
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node=neighbors.neighbors[x][y][z];
				if( IsActiveNode( _node ) ) value += coarseSolution[ _node->nodeData.nodeIndex ] * Real( evaluator.cornerStencils[_corner][corner]( x , y , z ) );
			}
		else
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node = neighbors.neighbors[x][y][z];
				if( _isValidFEMNode( _node ) )
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					value +=
						coarseSolution[ _node->nodeData.nodeIndex ] *
						Real(
							evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , false ) *
							evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , false ) *
							evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , false )
						);
				}
			}
	}
	return Real( value );
}
template< class Real >
template< int FEMDegree , BoundaryType BType >
std::pair< Real , Point3D< Real > > Octree< Real >::_getCornerValueAndGradient( const ConstPointSupportKey< FEMDegree >& neighborKey , const TreeOctNode* node , int corner , const DenseNodeData< Real , FEMDegree >& solution , const DenseNodeData< Real , FEMDegree >& coarseSolution , const _Evaluator< FEMDegree , BType >& evaluator , bool isInterior ) const
{
	static const int SupportSize = BSplineSupportSizes< FEMDegree >::SupportSize;
	static const int  LeftPointSupportRadius =   BSplineSupportSizes< FEMDegree >::SupportEnd;
	static const int RightPointSupportRadius = - BSplineSupportSizes< FEMDegree >::SupportStart;

	double value = 0;
	Point3D< double > gradient;
	LocalDepth d ; LocalOffset cIdx;
	_localDepthAndOffset( node , d , cIdx );

	int cx , cy , cz;
	int startX = 0 , endX = SupportSize , startY = 0 , endY = SupportSize , startZ = 0 , endZ = SupportSize;
	Cube::FactorCornerIndex( corner , cx , cy , cz );
	cIdx[0] += cx , cIdx[1] += cy , cIdx[2] += cz;
	{
		if( cx==0 ) endX--;
		else      startX++;
		if( cy==0 ) endY--;
		else      startY++;
		if( cz==0 ) endZ--;
		else      startZ++;
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node );
		if( isInterior )
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node=neighbors.neighbors[x][y][z];
				if( IsActiveNode( _node ) ) value += solution[ _node->nodeData.nodeIndex ] * evaluator.cornerStencil[corner]( x , y , z ) , gradient += evaluator.dCornerStencil[corner]( x , y , z ) * solution[ _node->nodeData.nodeIndex ];
			}
		else
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node = neighbors.neighbors[x][y][z];
				if( _isValidFEMNode( _node ) )
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					double v [] = { evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , false ) , evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , false ) , evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , false ) };
					double dv[] = { evaluator.evaluator.cornerValue( fIdx[0] , cIdx[0] , true  ) , evaluator.evaluator.cornerValue( fIdx[1] , cIdx[1] , true  ) , evaluator.evaluator.cornerValue( fIdx[2] , cIdx[2] , true  ) };
					value += solution[ _node->nodeData.nodeIndex ] * v[0] * v[1] * v[2];
					gradient += Point3D< double >( dv[0]*v[1]*v[2] , v[0]*dv[1]*v[2] , v[0]*v[1]*dv[2] ) * solution[ _node->nodeData.nodeIndex ];
				}
			}
	}
	if( d>0 )
	{
		int _corner = int( node - node->parent->children );
		int _cx , _cy , _cz;
		Cube::FactorCornerIndex( _corner , _cx , _cy , _cz );
		if( cx!=_cx ) startX = 0 , endX = SupportSize;
		if( cy!=_cy ) startY = 0 , endY = SupportSize;
		if( cz!=_cz ) startZ = 0 , endZ = SupportSize;
		const typename TreeOctNode::ConstNeighbors< SupportSize >& neighbors = _neighbors< LeftPointSupportRadius , RightPointSupportRadius >( neighborKey , node->parent );
		if( isInterior )
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node=neighbors.neighbors[x][y][z];
				if( IsActiveNode( _node ) ) value += coarseSolution[ _node->nodeData.nodeIndex ] * evaluator.cornerStencils[_corner][corner]( x , y , z ) , gradient += evaluator.dCornerStencils[_corner][corner]( x , y , z ) * coarseSolution[ _node->nodeData.nodeIndex ];
			}
		else
			for( int x=startX ; x<endX ; x++ ) for( int y=startY ; y<endY ; y++ ) for( int z=startZ ; z<endZ ; z++ )
			{
				const TreeOctNode* _node = neighbors.neighbors[x][y][z];
				if( _isValidFEMNode( _node ) )
				{
					LocalDepth _d ; LocalOffset fIdx;
					_localDepthAndOffset( _node , _d , fIdx );
					double v [] = { evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , false ) , evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , false ) , evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , false ) };
					double dv[] = { evaluator.childEvaluator.cornerValue( fIdx[0] , cIdx[0] , true  ) , evaluator.childEvaluator.cornerValue( fIdx[1] , cIdx[1] , true  ) , evaluator.childEvaluator.cornerValue( fIdx[2] , cIdx[2] , true  ) };
					value += coarseSolution[ _node->nodeData.nodeIndex ] * v[0] * v[1] * v[2];
					gradient += Point3D< double >( dv[0]*v[1]*v[2] , v[0]*dv[1]*v[2] , v[0]*v[1]*dv[2] ) * coarseSolution[ _node->nodeData.nodeIndex ];
				}
			}
	}
	return std::pair< Real , Point3D< Real > >( Real( value ) , Point3D< Real >( gradient ) );
}
template< class Real >
template< int Degree , BoundaryType BType >
Octree< Real >::MultiThreadedEvaluator< Degree , BType >::MultiThreadedEvaluator( const Octree< Real >* tree , const DenseNodeData< Real , Degree >& coefficients , int threads ) : _tree( tree ), _coefficients( coefficients )
{
	_threads = std::max< int >( 1 , threads );
	_neighborKeys.resize( _threads );
	_coarseCoefficients = _tree->template coarseCoefficients< Real , Degree , BType >( _coefficients );
	_evaluator.set( _tree->_maxDepth );
	for( int t=0 ; t<_threads ; t++ ) _neighborKeys[t].set( tree->_localToGlobal( _tree->_maxDepth ) );
}
template< class Real >
template< int Degree , BoundaryType BType >
Real Octree< Real >::MultiThreadedEvaluator< Degree , BType >::value( Point3D< Real > p , int thread , const TreeOctNode* node )
{
	if( !node ) node = _tree->leaf( p );
	ConstPointSupportKey< Degree >& nKey = _neighborKeys[thread];
	nKey.getNeighbors( node );
	return _tree->template _getValue< Real , Degree >( nKey , node , p , _coefficients , _coarseCoefficients , _evaluator );
}
template< class Real >
template< int Degree , BoundaryType BType >
std::pair< Real , Point3D< Real > > Octree< Real >::MultiThreadedEvaluator< Degree , BType >::valueAndGradient( Point3D< Real > p , int thread , const TreeOctNode* node )
{
	if( !node ) node = _tree->leaf( p );
	ConstPointSupportKey< Degree >& nKey = _neighborKeys[thread];
	nKey.getNeighbors( node );
	return _tree->template _getValueAndGradient< Degree >( nKey , node , p , _coefficients , _coarseCoefficients , _evaluator );
}
