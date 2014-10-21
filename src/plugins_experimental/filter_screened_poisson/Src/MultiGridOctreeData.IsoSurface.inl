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

#include "Octree.h"
#include "time.h"
#include "MemoryUsage.h"
#include "MAT.h"



template< class Real , int Degree >
template< class Vertex >
Octree< Real , Degree >::SliceValues< Vertex >::SliceValues( void )
{
	_oldCCount = _oldECount = _oldFCount = _oldNCount = 0;
	cornerValues = NullPointer< Real >() ; cornerNormals = NullPointer< Point3D< Real > >() ; cornerSet = NullPointer< char >();
	edgeKeys = NullPointer< long long >() ; edgeSet = NullPointer< char >();
	faceEdges = NullPointer< FaceEdges >() ; faceSet = NullPointer< char >();
	mcIndices = NullPointer< char >();
}
template< class Real , int Degree >
template< class Vertex >
Octree< Real , Degree >::SliceValues< Vertex >::~SliceValues( void )
{
	_oldCCount = _oldECount = _oldFCount = _oldNCount = 0;
	FreePointer( cornerValues ) ; FreePointer( cornerNormals ) ; FreePointer( cornerSet );
	FreePointer( edgeKeys ) ; FreePointer( edgeSet );
	FreePointer( faceEdges ) ; FreePointer( faceSet );
	FreePointer( mcIndices );
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SliceValues< Vertex >::reset( bool nonLinearFit )
{
	faceEdgeMap.clear() , edgeVertexMap.clear() , vertexPairMap.clear();

	if( _oldNCount<sliceData.nodeCount )
	{
		_oldNCount = sliceData.nodeCount;
		FreePointer( mcIndices );
		if( sliceData.nodeCount>0 ) mcIndices = AllocPointer< char >( _oldNCount );
	}
	if( _oldCCount<sliceData.cCount )
	{
		_oldCCount = sliceData.cCount;
		FreePointer( cornerValues ) ; FreePointer( cornerNormals ) ; FreePointer( cornerSet );
		if( sliceData.cCount>0 )
		{
			cornerValues = AllocPointer< Real >( _oldCCount );
			if( nonLinearFit ) cornerNormals = AllocPointer< Point3D< Real > >( _oldCCount );
			cornerSet = AllocPointer< char >( _oldCCount );
		}
	}
	if( _oldECount<sliceData.eCount )
	{
		_oldECount = sliceData.eCount;
		FreePointer( edgeKeys ) ; FreePointer( edgeSet );
		edgeKeys = AllocPointer< long long >( _oldECount );
		edgeSet = AllocPointer< char >( _oldECount );
	}
	if( _oldFCount<sliceData.fCount )
	{
		_oldFCount = sliceData.fCount;
		FreePointer( faceEdges ) ; FreePointer( faceSet );
		faceEdges = AllocPointer< FaceEdges >( _oldFCount );
		faceSet = AllocPointer< char >( _oldFCount );
	}
	
	if( sliceData.cCount>0 ) memset( cornerSet , 0 , sizeof( char ) * sliceData.cCount );
	if( sliceData.eCount>0 ) memset(   edgeSet , 0 , sizeof( char ) * sliceData.eCount );
	if( sliceData.fCount>0 ) memset(   faceSet , 0 , sizeof( char ) * sliceData.fCount );
}
template< class Real , int Degree >
template< class Vertex >
Octree< Real , Degree >::XSliceValues< Vertex >::XSliceValues( void )
{
	_oldECount = _oldFCount = 0;
	edgeKeys = NullPointer< long long >() ; edgeSet = NullPointer< char >();
	faceEdges = NullPointer< FaceEdges >() ; faceSet = NullPointer< char >();
}
template< class Real , int Degree >
template< class Vertex >
Octree< Real , Degree >::XSliceValues< Vertex >::~XSliceValues( void )
{
	_oldECount = _oldFCount = 0;
	FreePointer( edgeKeys ) ; FreePointer( edgeSet );
	FreePointer( faceEdges ) ; FreePointer( faceSet );
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::XSliceValues< Vertex >::reset( void )
{
	faceEdgeMap.clear() , edgeVertexMap.clear() , vertexPairMap.clear();

	if( _oldECount<xSliceData.eCount )
	{
		_oldECount = xSliceData.eCount;
		FreePointer( edgeKeys ) ; FreePointer( edgeSet );
		edgeKeys = AllocPointer< long long >( _oldECount );
		edgeSet = AllocPointer< char >( _oldECount );
	}
	if( _oldFCount<xSliceData.fCount )
	{
		_oldFCount = xSliceData.fCount;
		FreePointer( faceEdges ) ; FreePointer( faceSet );
		faceEdges = AllocPointer< FaceEdges >( _oldFCount );
		faceSet = AllocPointer< char >( _oldFCount );
	}
	if( xSliceData.eCount>0 ) memset( edgeSet , 0 , sizeof( char ) * xSliceData.eCount );
	if( xSliceData.fCount>0 ) memset( faceSet , 0 , sizeof( char ) * xSliceData.fCount );
}

template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::GetMCIsoSurface( ConstPointer( Real ) kernelDensityWeights , ConstPointer( Real ) solution , Real isoValue , CoredMeshData< Vertex >& mesh , bool nonLinearFit , bool addBarycenter , bool polygonMesh )
{
	typename BSplineData< Degree >::template CornerEvaluator< 2 > evaluator;
	_fData.setCornerEvaluator( evaluator , 0 , 0 , _boundaryType==0 );

	int maxDepth = tree.maxDepth();

	std::vector< Real > coarseSolution( _sNodes.nodeCount[maxDepth] , 0 );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodes.nodeCount[_minDepth] ; i<_sNodes.nodeCount[maxDepth] ; i++ ) coarseSolution[i] = solution[i];
	for( int d=_minDepth ; d<maxDepth ; d++ ) UpSample( d , _sNodes , ( ConstPointer( Real ) )GetPointer( coarseSolution ) + _sNodes.nodeCount[d-1] , GetPointer( coarseSolution ) + _sNodes.nodeCount[d] );
	MemoryUsage();

	typename TreeOctNode::ConstNeighborKey3 nKey;
	nKey.set( maxDepth );
	std::vector< CornerValueStencil > vStencils( maxDepth+1 );
	std::vector< CornerNormalStencil > nStencils( maxDepth+1 );
	for( int d=_minDepth ; d<=maxDepth ; d++ )
	{
		SetCornerEvaluationStencil ( evaluator , d , vStencils[d].stencil  );
		SetCornerEvaluationStencils( evaluator , d , vStencils[d].stencils );
		SetCornerNormalEvaluationStencil ( evaluator , d , nStencils[d].stencil  );
		SetCornerNormalEvaluationStencils( evaluator , d , nStencils[d].stencils );
	}
	int vertexOffset = 0;
	std::vector< SlabValues< Vertex > > slabValues( maxDepth+1 );

	// Initialize the back slice
	for( int d=maxDepth ; d>=_minDepth ; d-- )
	{
		_sNodes.setSliceTableData (  slabValues[d].sliceValues(0).sliceData  , d , 0 , threads );
		_sNodes.setSliceTableData (  slabValues[d].sliceValues(1).sliceData  , d , 1 , threads );
		_sNodes.setXSliceTableData( slabValues[d].xSliceValues(0).xSliceData , d , 0 , threads );
		slabValues[d].sliceValues (0).reset( nonLinearFit );
		slabValues[d].sliceValues (1).reset( nonLinearFit );
		slabValues[d].xSliceValues(0).reset( );
	}
	for( int d=maxDepth ; d>=_minDepth ; d-- )
	{
		// Copy edges from finer
		if( d<maxDepth ) CopyFinerSliceIsoEdgeKeys( d , 0 , slabValues , threads );
		SetSliceIsoCorners( solution , GetPointer( coarseSolution ) , isoValue , d , 0 , slabValues , evaluator , vStencils[d].stencil , vStencils[d].stencils , nStencils[d].stencil , nStencils[d].stencils , threads );
		SetSliceIsoVertices( kernelDensityWeights , isoValue , d , 0 , vertexOffset , mesh , slabValues , threads );
		SetSliceIsoEdges( d , 0 , slabValues , threads );
	}
	// Iterate over the slices at the finest level
	for( int slice=0 ; slice<(1<<maxDepth) ; slice++ )
	{
		// Process at all depths that that contain this slice
		for( int d=maxDepth , o=slice+1 ; d>=_minDepth ; d-- , o>>=1 )
		{
			// Copy edges from finer (required to ensure we correctly track edge cancellations)
			if( d<maxDepth )
			{
				CopyFinerSliceIsoEdgeKeys( d , o , slabValues , threads );
				CopyFinerXSliceIsoEdgeKeys( d , o-1 , slabValues , threads );
			}

			// Set the slice values/vertices
			SetSliceIsoCorners( solution , GetPointer( coarseSolution ) , isoValue , d , o , slabValues , evaluator , vStencils[d].stencil , vStencils[d].stencils , nStencils[d].stencil , nStencils[d].stencils , threads );
			SetSliceIsoVertices( kernelDensityWeights , isoValue , d , o , vertexOffset , mesh , slabValues , threads );
			SetSliceIsoEdges( d , o , slabValues , threads );

			// Set the cross-slice edges
			SetXSliceIsoVertices( kernelDensityWeights , isoValue , d , o-1 , vertexOffset , mesh , slabValues , threads );
			SetXSliceIsoEdges( d , o-1 , slabValues , threads );

			// Add the triangles
			SetIsoSurface( d , o-1 , slabValues[d].sliceValues(o-1) , slabValues[d].sliceValues(o) , slabValues[d].xSliceValues(o-1) , mesh , polygonMesh , addBarycenter , vertexOffset , threads );

			if( o&1 ) break;
		}
		for( int d=maxDepth , o=slice+1 ; d>=_minDepth ; d-- , o>>=1 )
		{
			// Initialize for the next pass
			if( o<(1<<d) )
			{
				_sNodes.setSliceTableData( slabValues[d].sliceValues(o+1).sliceData , d , o+1 , threads );
				_sNodes.setXSliceTableData( slabValues[d].xSliceValues(o).xSliceData , d , o , threads );
				slabValues[d].sliceValues(o+1).reset( nonLinearFit );
				slabValues[d].xSliceValues(o).reset();
			}
			if( o&1 ) break;
		}
	}
	MemoryUsage();
}

template< class Real , int Degree >
Real Octree< Real , Degree >::GetIsoValue( ConstPointer( Real ) solution , const std::vector< Real >& centerWeights )
{
	Real isoValue=0 , weightSum=0;
	int maxDepth = tree.maxDepth();

	typename BSplineData< Degree >::template CenterEvaluator< 1 > evaluator;
	_fData.setCenterEvaluator( evaluator , 0 , 0 , _boundaryType==0 );
	std::vector< CenterValueStencil > vStencils( maxDepth+1 );
	for( int d=_minDepth ; d<=maxDepth ; d++ )
	{
		SetCenterEvaluationStencil ( evaluator , d , vStencils[d].stencil  );
		SetCenterEvaluationStencils( evaluator , d , vStencils[d].stencils );
	}
	std::vector< Real > metSolution( _sNodes.nodeCount[maxDepth] , 0 );
	std::vector< Real > centerValues( _sNodes.nodeCount[maxDepth+1] );
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodes.nodeCount[_minDepth] ; i<_sNodes.nodeCount[maxDepth] ; i++ ) metSolution[i] = solution[i];
	for( int d=_minDepth ; d<maxDepth ; d++ ) UpSample( d , _sNodes , ( ConstPointer( Real ) )GetPointer( metSolution ) + _sNodes.nodeCount[d-1] , GetPointer( metSolution ) + _sNodes.nodeCount[d] );
	for( int d=maxDepth ; d>=_minDepth ; d-- )
	{
		typename TreeOctNode::ConstNeighborKey3 nKey;
		nKey.set( d );
#pragma omp parallel for num_threads( threads ) reduction( + : isoValue , weightSum ) firstprivate( nKey )
		for( int i=_sNodes.nodeCount[d] ; i<_sNodes.nodeCount[d+1] ; i++ )
		{
			TreeOctNode* node = _sNodes.treeNodes[i];
			Real value = Real(0);
			if( node->children )
			{
				for( int c=0 ; c<Cube::CORNERS ; c++ ) value += centerValues[ node->children[c].nodeData.nodeIndex ];
				value /= Cube::CORNERS;
			}
			else
			{
				nKey.getNeighbors( node );
				int c=0 , x , y , z;
				if( node->parent ) c = int( node - node->parent->children );
				Cube::FactorCornerIndex( c , x , y , z );

				int d , off[3];
				node->depthAndOffset( d , off );
				int o = _boundaryType==0 ? (1<<(d-2)) : 0;
				int mn = 2+o , mx = (1<<d)-2-o;
				bool isInterior = ( off[0]>=mn && off[0]<mx && off[1]>=mn && off[1]<mx && off[2]>=mn && off[2]<mx );

				value = getCenterValue( nKey , node , solution , GetPointer( metSolution ) , evaluator , vStencils[d].stencil , vStencils[d].stencils[c] , isInterior );
			}
			centerValues[i] = value;
			Real w = centerWeights[ node->nodeData.nodeIndex ];
			if( w!=0 ) isoValue += value * w , weightSum += w;
		}
	}
	if( _boundaryType==-1 ) return isoValue/weightSum - Real(0.5);
	else                    return isoValue/weightSum;
}

template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetSliceIsoCorners( ConstPointer( Real ) solution , ConstPointer( Real ) coarseSolution , Real isoValue , int depth , int slice , std::vector< SlabValues< Vertex > >& slabValues , const typename BSplineData< Degree >::template CornerEvaluator< 2 >& evaluator , const Stencil< double , 3 > vStencil[8] , const Stencil< double , 3 > vStencils[8][8] , const Stencil< Point3D< double > , 3 > nStencil[8] , const Stencil< Point3D< double > , 3 > nStencils[8][8] , int threads )
{
	if( slice>0          ) SetSliceIsoCorners( solution , coarseSolution , isoValue , depth , slice , 1 , slabValues , evaluator , vStencil , vStencils , nStencil , nStencils , threads );
	if( slice<(1<<depth) ) SetSliceIsoCorners( solution , coarseSolution , isoValue , depth , slice , 0 , slabValues , evaluator , vStencil , vStencils , nStencil , nStencils , threads );
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetSliceIsoCorners( ConstPointer( Real ) solution , ConstPointer( Real ) coarseSolution , Real isoValue , int depth , int slice , int z , std::vector< SlabValues< Vertex > >& slabValues , const typename BSplineData< Degree >::template CornerEvaluator< 2 >& evaluator , const Stencil< double , 3 > vStencil[8] , const Stencil< double , 3 > vStencils[8][8] , const Stencil< Point3D< double > , 3 > nStencil[8] , const Stencil< Point3D< double > , 3 > nStencils[8][8] , int threads )
{
	typename Octree< Real , Degree >::template SliceValues< Vertex >& sValues = slabValues[depth].sliceValues( slice );
	Real squareValues[ Square::CORNERS ];
	typename TreeOctNode::ConstNeighborKey3 nKey;
	nKey.set( depth );
	{
#pragma omp parallel for num_threads( threads ) firstprivate( nKey , squareValues )
		for( int i=_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slice-z] ; i<_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slice-z+1] ; i++ )
		{
			TreeOctNode* leaf = _sNodes.treeNodes[i];
			if( !leaf->children )
			{
				const typename SortedTreeNodes::SquareCornerIndices& cIndices = sValues.sliceData.cornerIndices( leaf );

				bool isInterior;
				{
					int d , off[3];
					leaf->depthAndOffset( d , off );
					int o = _boundaryType==0 ? (1<<(d-2)) : 0;
					int mn = 2+o , mx = (1<<d)-2-o;
					isInterior = ( off[0]>=mn && off[0]<mx && off[1]>=mn && off[1]<mx && off[2]>=mn && off[2]<mx );
				}
				nKey.getNeighbors( leaf );

				for( int x=0 ; x<2 ; x++ ) for( int y=0 ; y<2 ; y++ )
				{
					int cc = Cube::CornerIndex( x , y , z );
					int fc = Square::CornerIndex( x , y );
					int vIndex = cIndices[fc];
					if( !sValues.cornerSet[vIndex] )
					{
						if( sValues.cornerNormals )
						{
							std::pair< Real , Point3D< Real > > p = getCornerValueAndNormal( nKey , leaf , cc , solution , coarseSolution , evaluator , vStencil[cc] , vStencils[cc] , nStencil[cc] , nStencils[cc] , isInterior );
							sValues.cornerValues[vIndex] = p.first , sValues.cornerNormals[vIndex] = p.second;
						}
						else sValues.cornerValues[vIndex] = getCornerValue( nKey , leaf , cc , solution , coarseSolution , evaluator , vStencil[cc] , vStencils[cc] , isInterior );
						sValues.cornerSet[vIndex] = 1;
					}
					squareValues[fc] = sValues.cornerValues[ vIndex ];
					TreeOctNode* node = leaf;
					int _depth = depth , _slice = slice;
					while( node->parent && (node-node->parent->children)==cc )
					{
						node = node->parent , _depth-- , _slice >>= 1;
						typename Octree< Real , Degree >::template SliceValues< Vertex >& _sValues = slabValues[_depth].sliceValues( _slice );
						const typename SortedTreeNodes::SquareCornerIndices& _cIndices = _sValues.sliceData.cornerIndices( node );
						int _vIndex = _cIndices[fc];
						_sValues.cornerValues[_vIndex] = sValues.cornerValues[vIndex];
						if( _sValues.cornerNormals ) _sValues.cornerNormals[_vIndex] = sValues.cornerNormals[vIndex];
						_sValues.cornerSet[_vIndex] = 1;
					}
				}
				sValues.mcIndices[ i - sValues.sliceData.nodeOffset ] = MarchingSquares::GetIndex( squareValues , isoValue );
			}
		}
	}
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetSliceIsoVertices( ConstPointer( Real ) kernelDensityWeight , Real isoValue , int depth , int slice , int& vOffset , CoredMeshData< Vertex >& mesh , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	if( slice>0          ) SetSliceIsoVertices( kernelDensityWeight , isoValue , depth , slice , 1 , vOffset , mesh , slabValues , threads );
	if( slice<(1<<depth) ) SetSliceIsoVertices( kernelDensityWeight , isoValue , depth , slice , 0 , vOffset , mesh , slabValues , threads );
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetSliceIsoVertices( ConstPointer( Real ) kernelDensityWeight , Real isoValue , int depth , int slice , int z , int& vOffset , CoredMeshData< Vertex >& mesh , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	typename Octree< Real , Degree >::template SliceValues< Vertex >& sValues = slabValues[depth].sliceValues( slice );
	typename TreeOctNode::ConstNeighborKey3 nKey;
	nKey.set( depth );
#pragma omp parallel for num_threads( threads ) firstprivate( nKey )
	for( int i=_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slice-z] ; i<_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slice-z+1] ; i++ )
	{
		TreeOctNode* leaf = _sNodes.treeNodes[i];
		if( !leaf->children )
		{
			int idx = i - sValues.sliceData.nodeOffset;
			const typename SortedTreeNodes::SquareEdgeIndices& eIndices = sValues.sliceData.edgeIndices( leaf );
			if( MarchingSquares::HasRoots( sValues.mcIndices[idx] ) )
			{
				nKey.getNeighbors( leaf );
				for( int e=0 ; e<Square::EDGES ; e++ )
					if( MarchingSquares::HasEdgeRoots( sValues.mcIndices[idx] , e ) )
					{
						int vIndex = eIndices[e];
						if( !sValues.edgeSet[vIndex] )
						{
							Vertex vertex;
							int o , y;
							Square::FactorEdgeIndex( e , o , y );
							long long key = VertexData::EdgeIndex( leaf , Cube::EdgeIndex( o , y , z ) , _sNodes.maxDepth );
							GetIsoVertex( kernelDensityWeight , isoValue , nKey , leaf , e , z , sValues , vertex );
							vertex.point = vertex.point * _scale + _center;
							bool stillOwner = false;
							std::pair< int , Vertex > hashed_vertex;
#pragma omp critical (add_point_access)
							{
								if( !sValues.edgeSet[vIndex] )
								{
									mesh.addOutOfCorePoint( vertex );
									sValues.edgeSet[ vIndex ] = 1;
									sValues.edgeKeys[ vIndex ] = key;
									sValues.edgeVertexMap[key] = hashed_vertex = std::pair< int , Vertex >( vOffset , vertex );
									vOffset++;
									stillOwner = true;
								}
							}
							if( stillOwner )
							{
								// We only need to pass the iso-vertex down if the edge it lies on is adjacent to a coarser leaf
								bool isNeeded;
								switch( o )
								{
								case 0: isNeeded = ( nKey.neighbors[depth].neighbors[1][2*y][1]==NULL || nKey.neighbors[depth].neighbors[1][2*y][2*z]==NULL || nKey.neighbors[depth].neighbors[1][1][2*z]==NULL ) ; break;
								case 1: isNeeded = ( nKey.neighbors[depth].neighbors[2*y][1][1]==NULL || nKey.neighbors[depth].neighbors[2*y][1][2*z]==NULL || nKey.neighbors[depth].neighbors[1][1][2*z]==NULL ) ; break;
								}
								if( isNeeded )
								{
									int f[2];
									Cube::FacesAdjacentToEdge( Cube::EdgeIndex( o , y , z ) , f[0] , f[1] );
									for( int k=0 ; k<2 ; k++ )
									{
										TreeOctNode* node = leaf;
										int _depth = depth , _slice = slice;
										bool _isNeeded = isNeeded;
										while( _isNeeded = node->parent && Cube::IsFaceCorner( (int)(node-node->parent->children) , f[k] ) )
										{
											node = node->parent , _depth-- , _slice >>= 1;
											typename Octree< Real , Degree >::template SliceValues< Vertex >& _sValues = slabValues[_depth].sliceValues( _slice );
#pragma omp critical (add_coarser_point_access)
											_sValues.edgeVertexMap[key] = hashed_vertex;
											switch( o )
											{
												case 0: _isNeeded = ( nKey.neighbors[_depth].neighbors[1][2*y][1]==NULL || nKey.neighbors[_depth].neighbors[1][2*y][2*z]==NULL || nKey.neighbors[_depth].neighbors[1][1][2*z]==NULL ) ; break;
												case 1: _isNeeded = ( nKey.neighbors[_depth].neighbors[2*y][1][1]==NULL || nKey.neighbors[_depth].neighbors[2*y][1][2*z]==NULL || nKey.neighbors[_depth].neighbors[1][1][2*z]==NULL ) ; break;
											}
										}
									}
								}
							}
						}
					}
			}
		}
	}
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetXSliceIsoVertices( ConstPointer( Real ) kernelDensityWeight , Real isoValue , int depth , int slab , int& vOffset , CoredMeshData< Vertex >& mesh , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	typename Octree< Real , Degree >::template  SliceValues< Vertex >& bValues = slabValues[depth].sliceValues ( slab   );
	typename Octree< Real , Degree >::template  SliceValues< Vertex >& fValues = slabValues[depth].sliceValues ( slab+1 );
	typename Octree< Real , Degree >::template XSliceValues< Vertex >& xValues = slabValues[depth].xSliceValues( slab   );
	typename TreeOctNode::ConstNeighborKey3 nKey;
	nKey.set( depth );

#pragma omp parallel for num_threads( threads ) firstprivate( nKey )
	for( int i=_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slab] ; i<_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slab+1] ; i++ )
	{
		TreeOctNode* leaf = _sNodes.treeNodes[i];
		if( !leaf->children )
		{
			unsigned char mcIndex = ( bValues.mcIndices[ i - bValues.sliceData.nodeOffset ] ) | ( fValues.mcIndices[ i - fValues.sliceData.nodeOffset ] )<<4;
			const typename SortedTreeNodes::SquareCornerIndices& eIndices = xValues.xSliceData.edgeIndices( leaf );
			if( MarchingCubes::HasRoots( mcIndex ) )
			{
				nKey.getNeighbors( leaf );
				for( int x=0 ; x<2 ; x++ ) for( int y=0 ; y<2 ; y++ )
				{
					int c = Square::CornerIndex( x , y );
					int e = Cube::EdgeIndex( 2 , x , y );
					if( MarchingCubes::HasEdgeRoots( mcIndex , e ) )
					{
						int vIndex = eIndices[c];
						if( !xValues.edgeSet[vIndex] )
						{
							Vertex vertex;
							long long key = VertexData::EdgeIndex( leaf , e , _sNodes.maxDepth );
							GetIsoVertex( kernelDensityWeight , isoValue , nKey , leaf , c , bValues , fValues , vertex );
							vertex.point = vertex.point * _scale + _center;
							bool stillOwner = false;
							std::pair< int , Vertex > hashed_vertex;
#pragma omp critical (add_x_point_access)
							{
								if( !xValues.edgeSet[vIndex] )
								{
									mesh.addOutOfCorePoint( vertex );
									xValues.edgeSet[ vIndex ] = 1;
									xValues.edgeKeys[ vIndex ] = key;
									xValues.edgeVertexMap[key] = hashed_vertex = std::pair< int , Vertex >( vOffset , vertex );
									stillOwner = true;
									vOffset++;
								}
							}
							if( stillOwner )
							{
								// We only need to pass the iso-vertex down if the edge it lies on is adjacent to a coarser leaf
								bool isNeeded = ( nKey.neighbors[depth].neighbors[2*x][1][1]==NULL || nKey.neighbors[depth].neighbors[2*x][2*y][1]==NULL || nKey.neighbors[depth].neighbors[1][2*y][1]==NULL );
								if( isNeeded )
								{
									int f[2];
									Cube::FacesAdjacentToEdge( e , f[0] , f[1] );
									for( int k=0 ; k<2 ; k++ )
									{
										TreeOctNode* node = leaf;
										int _depth = depth , _slab = slab;
										bool _isNeeded = isNeeded;
										while( _isNeeded && node->parent && Cube::IsFaceCorner( (int)(node-node->parent->children) , f[k] ) )
										{
											node = node->parent , _depth-- , _slab >>= 1;
											typename Octree< Real , Degree >::template XSliceValues< Vertex >& _xValues = slabValues[_depth].xSliceValues( _slab );
#pragma omp critical (add_x_coarser_point_access)
											_xValues.edgeVertexMap[key] = hashed_vertex;
											_isNeeded = ( nKey.neighbors[_depth].neighbors[2*x][1][1]==NULL || nKey.neighbors[_depth].neighbors[2*x][2*y][1]==NULL || nKey.neighbors[_depth].neighbors[1][2*y][1]==NULL );
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::CopyFinerSliceIsoEdgeKeys( int depth , int slice , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	if( slice>0          ) CopyFinerSliceIsoEdgeKeys( depth , slice , 1 , slabValues , threads );
	if( slice<(1<<depth) ) CopyFinerSliceIsoEdgeKeys( depth , slice , 0 , slabValues , threads );
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::CopyFinerSliceIsoEdgeKeys( int depth , int slice , int z , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	SliceValues< Vertex >& pSliceValues = slabValues[depth  ].sliceValues(slice   );
	SliceValues< Vertex >& cSliceValues = slabValues[depth+1].sliceValues(slice<<1);
	typename SortedTreeNodes::SliceTableData& pSliceData = pSliceValues.sliceData;
	typename SortedTreeNodes::SliceTableData& cSliceData = cSliceValues.sliceData;
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodes.nodeCount[depth] + _sNodes.sliceOffsets[depth][slice-z] ; i<_sNodes.nodeCount[depth] + _sNodes.sliceOffsets[depth][slice-z+1] ; i++ )
		if( _sNodes.treeNodes[i]->children )
		{
			typename SortedTreeNodes::SquareEdgeIndices& pIndices = pSliceData.edgeIndices( i );
			// Copy the edges that overlap the coarser edges
			for( int orientation=0 ; orientation<2 ; orientation++ ) for( int y=0 ; y<2 ; y++ )
			{
				int fe = Square::EdgeIndex( orientation , y );
				int pIndex = pIndices[fe];
				if( !pSliceValues.edgeSet[ pIndex ] )
				{
					int ce = Cube::EdgeIndex( orientation , y , z );
					int c1 , c2;
					switch( orientation )
					{
					case 0: c1 = Cube::CornerIndex( 0 , y , z ) , c2 = Cube::CornerIndex( 1 , y , z ) ; break;
					case 1: c1 = Cube::CornerIndex( y , 0 , z ) , c2 = Cube::CornerIndex( y , 1 , z ) ; break;
					}
					int cIndex1 = cSliceData.edgeIndices( _sNodes.treeNodes[i]->children + c1 )[fe];
					int cIndex2 = cSliceData.edgeIndices( _sNodes.treeNodes[i]->children + c2 )[fe];
					if( cSliceValues.edgeSet[cIndex1] != cSliceValues.edgeSet[cIndex2] )
					{
						long long key;
						if( cSliceValues.edgeSet[cIndex1] ) key = cSliceValues.edgeKeys[cIndex1];
						else                                key = cSliceValues.edgeKeys[cIndex2];
						std::pair< int , Vertex > vPair = cSliceValues.edgeVertexMap[key];
#pragma omp critical ( copy_finer_edge_keys )
						pSliceValues.edgeVertexMap[key] = vPair;
						pSliceValues.edgeKeys[pIndex] = key;
						pSliceValues.edgeSet[pIndex] = 1;
					}
					else if( cSliceValues.edgeSet[cIndex1] && cSliceValues.edgeSet[cIndex2] )
					{
						long long key1 = cSliceValues.edgeKeys[cIndex1] , key2 = cSliceValues.edgeKeys[cIndex2];
#pragma omp critical ( set_edge_pairs )
						pSliceValues.vertexPairMap[ key1 ] = key2 ,	pSliceValues.vertexPairMap[ key2 ] = key1;

						const TreeOctNode* node = _sNodes.treeNodes[i];
						int _depth = depth , _slice = slice;
						while( node->parent && Cube::IsEdgeCorner( (int)( node - node->parent->children ) , ce ) )
						{
							node = node->parent , _depth-- , _slice >>= 1;
							SliceValues< Vertex >& _pSliceValues = slabValues[_depth].sliceValues(_slice);
#pragma omp critical ( set_edge_pairs )
							_pSliceValues.vertexPairMap[ key1 ] = key2 , _pSliceValues.vertexPairMap[ key2 ] = key1;
						}
					}
				}
			}
		}
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::CopyFinerXSliceIsoEdgeKeys( int depth , int slab , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	XSliceValues< Vertex >& pSliceValues  = slabValues[depth  ].xSliceValues(slab);
	XSliceValues< Vertex >& cSliceValues0 = slabValues[depth+1].xSliceValues( (slab<<1)|0 );
	XSliceValues< Vertex >& cSliceValues1 = slabValues[depth+1].xSliceValues( (slab<<1)|1 );
	typename SortedTreeNodes::XSliceTableData& pSliceData  = pSliceValues.xSliceData;
	typename SortedTreeNodes::XSliceTableData& cSliceData0 = cSliceValues0.xSliceData;
	typename SortedTreeNodes::XSliceTableData& cSliceData1 = cSliceValues1.xSliceData;
#pragma omp parallel for num_threads( threads )
	for( int i=_sNodes.nodeCount[depth] + _sNodes.sliceOffsets[depth][slab] ; i<_sNodes.nodeCount[depth] + _sNodes.sliceOffsets[depth][slab+1] ; i++ )
		if( _sNodes.treeNodes[i]->children )
		{
			typename SortedTreeNodes::SquareCornerIndices& pIndices = pSliceData.edgeIndices( i );
			for( int x=0 ; x<2 ; x++ ) for( int y=0 ; y<2 ; y++ )
			{
				int fc = Square::CornerIndex( x , y );
				int pIndex = pIndices[fc];
				if( !pSliceValues.edgeSet[pIndex] )
				{
					int c0 = Cube::CornerIndex( x , y , 0 ) , c1 = Cube::CornerIndex( x , y , 1 );
					int cIndex0 = cSliceData0.edgeIndices( _sNodes.treeNodes[i]->children + c0 )[fc];
					int cIndex1 = cSliceData1.edgeIndices( _sNodes.treeNodes[i]->children + c1 )[fc];
					if( cSliceValues0.edgeSet[cIndex0] != cSliceValues1.edgeSet[cIndex1] )
					{
						long long key;
						std::pair< int , Vertex > vPair;
						if( cSliceValues0.edgeSet[cIndex0] ) key = cSliceValues0.edgeKeys[cIndex0] , vPair = cSliceValues0.edgeVertexMap[key];
						else                                 key = cSliceValues1.edgeKeys[cIndex1] , vPair = cSliceValues1.edgeVertexMap[key];
#pragma omp critical ( copy_finer_x_edge_keys )
						pSliceValues.edgeVertexMap[key] = vPair;
						pSliceValues.edgeKeys[ pIndex ] = key;
						pSliceValues.edgeSet[ pIndex ] = 1;
					}
					else if( cSliceValues0.edgeSet[cIndex0] && cSliceValues1.edgeSet[cIndex1] )
					{
						long long key0 = cSliceValues0.edgeKeys[cIndex0] , key1 = cSliceValues1.edgeKeys[cIndex1];
#pragma omp critical ( set_x_edge_pairs )
						pSliceValues.vertexPairMap[ key0 ] = key1 , pSliceValues.vertexPairMap[ key1 ] = key0;
						const TreeOctNode* node = _sNodes.treeNodes[i];
						int _depth = depth , _slab = slab , ce = Cube::CornerIndex( 2 , x , y );
						while( node->parent && Cube::IsEdgeCorner( (int)( node - node->parent->children ) , ce ) )
						{
							node = node->parent , _depth-- , _slab>>= 1;
							SliceValues< Vertex >& _pSliceValues = slabValues[_depth].sliceValues(_slab);
#pragma omp critical ( set_x_edge_pairs )
							_pSliceValues.vertexPairMap[ key0 ] = key1 , _pSliceValues.vertexPairMap[ key1 ] = key0;
						}
					}
				}
			}
		}
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetSliceIsoEdges( int depth , int slice , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	if( slice>0          ) SetSliceIsoEdges( depth , slice , 1 , slabValues , threads );
	if( slice<(1<<depth) ) SetSliceIsoEdges( depth , slice , 0 , slabValues , threads );
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetSliceIsoEdges( int depth , int slice , int z , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	typename Octree< Real , Degree >::template SliceValues< Vertex >& sValues = slabValues[depth].sliceValues( slice );
	int isoEdges[ 2 * MarchingSquares::MAX_EDGES ];
	typename TreeOctNode::ConstNeighborKey3 nKey;
	nKey.set( depth );
#pragma omp parallel for num_threads( threads ) firstprivate( nKey , isoEdges )
	for( int i=_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slice-z] ; i<_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slice-z+1] ; i++ )
	{
		TreeOctNode* leaf = _sNodes.treeNodes[i];
		if( !leaf->children )
		{
			int idx = i - sValues.sliceData.nodeOffset;
			const typename SortedTreeNodes::SquareEdgeIndices& eIndices = sValues.sliceData.edgeIndices( leaf );
			const typename SortedTreeNodes::SquareFaceIndices& fIndices = sValues.sliceData.faceIndices( leaf );
			unsigned char mcIndex = sValues.mcIndices[idx];
			if( !sValues.faceSet[ fIndices[0] ] )
			{
				nKey.getNeighbors( leaf );
				if( !nKey.neighbors[depth].neighbors[1][1][2*z] || !nKey.neighbors[depth].neighbors[1][1][2*z]->children )
				{
					FaceEdges fe;
					fe.count = MarchingSquares::AddEdgeIndices( mcIndex , isoEdges );
					for( int j=0 ; j<fe.count ; j++ ) for( int k=0 ; k<2 ; k++ )
					{
						if( !sValues.edgeSet[ eIndices[ isoEdges[2*j+k] ] ] ) fprintf( stderr , "[ERROR] Edge not set 1: %d / %d\n" , slice , 1<<depth ) , exit( 0 );
						fe.edges[j][k] = sValues.edgeKeys[ eIndices[ isoEdges[2*j+k] ] ];
					}
					sValues.faceSet[ fIndices[0] ] = 1;
					sValues.faceEdges[ fIndices[0] ] = fe;

					TreeOctNode* node = leaf;
					int _depth = depth , _slice = slice , f = Cube::FaceIndex( 2 , z );
					std::vector< IsoEdge > edges;
					edges.resize( fe.count );
					for( int j=0 ; j<fe.count ; j++ ) edges[j] = fe.edges[j];
					while( node->parent && Cube::IsFaceCorner( (int)(node-node->parent->children) , f ) )
					{
						node = node->parent , _depth-- , _slice >>= 1;
						if( nKey.neighbors[_depth].neighbors[1][1][2*z] && nKey.neighbors[_depth].neighbors[1][1][2*z]->children ) break;
						long long key = VertexData::FaceIndex( node , f , _sNodes.maxDepth );
#pragma omp critical( add_iso_edge_access )
						{
							typename Octree< Real , Degree >::template SliceValues< Vertex >& _sValues = slabValues[_depth].sliceValues( _slice );
							typename hash_map< long long , std::vector< IsoEdge > >::iterator iter = _sValues.faceEdgeMap.find(key);
							if( iter==_sValues.faceEdgeMap.end() ) _sValues.faceEdgeMap[key] = edges;
							else for( int j=0 ; j<fe.count ; j++ ) iter->second.push_back( fe.edges[j] );
						}
					}
				}
			}
		}
	}
}
template< class Real , int Degree >
template< class Vertex >
void Octree< Real , Degree >::SetXSliceIsoEdges( int depth , int slab , std::vector< SlabValues< Vertex > >& slabValues , int threads )
{
	typename Octree< Real , Degree >::template  SliceValues< Vertex >& bValues = slabValues[depth].sliceValues ( slab   );
	typename Octree< Real , Degree >::template  SliceValues< Vertex >& fValues = slabValues[depth].sliceValues ( slab+1 );
	typename Octree< Real , Degree >::template XSliceValues< Vertex >& xValues = slabValues[depth].xSliceValues( slab   );
	int isoEdges[ 2 * MarchingSquares::MAX_EDGES ];
	typename TreeOctNode::ConstNeighborKey3 nKey;
	nKey.set( depth );
#pragma omp parallel for num_threads( threads ) firstprivate( nKey , isoEdges )
	for( int i=_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slab] ; i<_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][slab+1] ; i++ )
	{
		TreeOctNode* leaf = _sNodes.treeNodes[i];
		if( !leaf->children )
		{
			const typename SortedTreeNodes::SquareCornerIndices& cIndices = xValues.xSliceData.edgeIndices( leaf );
			const typename SortedTreeNodes::SquareEdgeIndices& eIndices = xValues.xSliceData.faceIndices( leaf );
			unsigned char mcIndex = ( bValues.mcIndices[ i - bValues.sliceData.nodeOffset ] ) | ( fValues.mcIndices[ i - fValues.sliceData.nodeOffset ]<<4 );
			{
				nKey.getNeighbors( leaf );
				for( int o=0 ; o<2 ; o++ ) for( int x=0 ; x<2 ; x++ )
				{
					int e = Square::EdgeIndex( o , x );
					int f = Cube::FaceIndex( 1-o , x );
					unsigned char _mcIndex = MarchingCubes::GetFaceIndex( mcIndex , f );
					int xx = o==1 ? 2*x : 1 , yy = o==0 ? 2*x : 1 , zz = 1;
					if(	!xValues.faceSet[ eIndices[e] ] && ( !nKey.neighbors[depth].neighbors[xx][yy][zz] || !nKey.neighbors[depth].neighbors[xx][yy][zz]->children ) )
					{
						FaceEdges fe;
						fe.count = MarchingSquares::AddEdgeIndices( _mcIndex , isoEdges );
						for( int j=0 ; j<fe.count ; j++ ) for( int k=0 ; k<2 ; k++ )
						{
							int _o , _x;
							Square::FactorEdgeIndex( isoEdges[2*j+k] , _o , _x );
							if( _o==1 ) // Cross-edge
							{
								int idx = o==0 ? cIndices[ Square::CornerIndex(_x,x) ] : cIndices[ Square::CornerIndex(x,_x) ];
								if( !xValues.edgeSet[ idx ] ) fprintf( stderr , "[ERROR] Edge not set 3: %d / %d\n" , slab , 1<<depth ) , exit( 0 );
								fe.edges[j][k] = xValues.edgeKeys[ idx ];
							}
							else
							{
								const typename Octree< Real , Degree >::template SliceValues< Vertex >& sValues = (_x==0) ? bValues : fValues;
								int idx = sValues.sliceData.edgeIndices(i)[ Square::EdgeIndex(o,x) ];
								if( !sValues.edgeSet[ idx ] ) fprintf( stderr , "[ERROR] Edge not set 5: %d / %d\n" , slab , 1<<depth ) , exit( 0 );
								fe.edges[j][k] = sValues.edgeKeys[ idx ];
							}
						}
						xValues.faceSet[ eIndices[e] ] = 1;
						xValues.faceEdges[ eIndices[e] ] = fe;

						TreeOctNode* node = leaf;
						int _depth = depth , _slab = slab;
						std::vector< IsoEdge > edges;
						edges.resize( fe.count );
						for( int j=0 ; j<fe.count ; j++ ) edges[j] = fe.edges[j];
						while( node->parent && Cube::IsFaceCorner( (int)(node-node->parent->children) , f ) )
						{
							node = node->parent , _depth-- , _slab >>= 1;
							if( nKey.neighbors[_depth].neighbors[xx][yy][zz] && nKey.neighbors[_depth].neighbors[xx][yy][zz]->children ) break;
							long long key = VertexData::FaceIndex( node , f , _sNodes.maxDepth );
#pragma omp critical( add_x_iso_edge_access )
							{
								typename Octree< Real , Degree >::template XSliceValues< Vertex >& _xValues = slabValues[_depth].xSliceValues( _slab );
								typename hash_map< long long , std::vector< IsoEdge > >::iterator iter = _xValues.faceEdgeMap.find(key);
								if( iter==_xValues.faceEdgeMap.end() ) _xValues.faceEdgeMap[key] = edges;
								else for( int j=0 ; j<fe.count ; j++ ) iter->second.push_back( fe.edges[j] );
							}
						}
					}
				}
			}
		}
	}
}
template< class Real , int Degree >
template< class Vertex >
int Octree< Real , Degree >::SetIsoSurface( int depth , int offset , const SliceValues< Vertex >& bValues , const SliceValues< Vertex >& fValues , const XSliceValues< Vertex >& xValues , CoredMeshData< Vertex >& mesh , bool polygonMesh , bool addBarycenter , int& vOffset , int threads )
{
	typename TreeOctNode::ConstNeighborKey3 nKey;
	std::vector< std::pair< int , Vertex > > polygon;
	std::vector< IsoEdge > edges;
	nKey.set( depth );
#pragma omp parallel for num_threads( threads ) firstprivate( nKey , edges )
	for( int i=_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][offset] ; i<_sNodes.nodeCount[depth]+_sNodes.sliceOffsets[depth][offset+1] ; i++ )
	{
		TreeOctNode* leaf = _sNodes.treeNodes[i];
		if( !leaf->children )
		{
			edges.clear();
			unsigned char mcIndex = ( bValues.mcIndices[ i - bValues.sliceData.nodeOffset ] ) | ( fValues.mcIndices[ i - fValues.sliceData.nodeOffset ]<<4 );
			// [WARNING] Just because the node looks empty doesn't mean it doesn't get eges from finer neighbors
			{
				// Gather the edges from the faces (with the correct orientation)
				for( int f=0 ; f<Cube::FACES ; f++ )
				{
					int d , o;
					Cube::FactorFaceIndex( f , d , o );
					int flip = d==1 ? 1 : 0; // To account for the fact that the section in y flips the orientation
					if( o ) flip = 1-flip;
					flip = 1-flip; // To get the right orientation
					if( d==2 )
					{
						const SliceValues< Vertex >& sValues = (o==0) ? bValues : fValues;
						int fIdx = sValues.sliceData.faceIndices(i)[0];
						if( sValues.faceSet[fIdx] )
						{
							const FaceEdges& fe = sValues.faceEdges[ fIdx ];
							for( int j=0 ; j<fe.count ; j++ ) edges.push_back( IsoEdge( fe.edges[j][flip] , fe.edges[j][1-flip] ) );
						}
						else
						{
							long long key = VertexData::FaceIndex( leaf , f , _sNodes.maxDepth );
							typename hash_map< long long , std::vector< IsoEdge > >::const_iterator iter = sValues.faceEdgeMap.find( key );
							if( iter!=sValues.faceEdgeMap.end() )
							{
								const std::vector< IsoEdge >& _edges = iter->second;
								for( int j=0 ; j<_edges.size() ; j++ ) edges.push_back( IsoEdge( _edges[j][flip] , _edges[j][1-flip] ) );
							}
							else fprintf( stderr , "[ERROR] Invalid faces: %d  %d %d\n" , i , d , o ) , exit( 0 );
						}
					}
					else
					{
						int fIdx = xValues.xSliceData.faceIndices(i)[ Square::EdgeIndex( 1-d , o ) ];
						if( xValues.faceSet[fIdx] )
						{
							const FaceEdges& fe = xValues.faceEdges[ fIdx ];
							for( int j=0 ; j<fe.count ; j++ ) edges.push_back( IsoEdge( fe.edges[j][flip] , fe.edges[j][1-flip] ) );
						}
						else
						{
							long long key = VertexData::FaceIndex( leaf , f , _sNodes.maxDepth );
							typename hash_map< long long , std::vector< IsoEdge > >::const_iterator iter = xValues.faceEdgeMap.find( key );
							if( iter!=xValues.faceEdgeMap.end() )
							{
								const std::vector< IsoEdge >& _edges = iter->second;
								for( int j=0 ; j<_edges.size() ; j++ ) edges.push_back( IsoEdge( _edges[j][flip] , _edges[j][1-flip] ) );
							}
							else fprintf( stderr , "[ERROR] Invalid faces: %d  %d %d\n" , i , d , o ) , exit( 0 );
						}
					}
				}
				// Get the edge loops
				std::vector< std::vector< long long  > > loops;
				while( edges.size() )
				{
					loops.resize( loops.size()+1 );
					IsoEdge edge = edges.back();
					edges.pop_back();
					long long start = edge[0] , current = edge[1];
					while( current!=start )
					{
						int idx;
						for( idx=0 ; idx<edges.size() ; idx++ ) if( edges[idx][0]==current ) break;
						if( idx==edges.size() )
						{
							typename hash_map< long long , long long >::const_iterator iter;
							if     ( (iter=bValues.vertexPairMap.find(current))!=bValues.vertexPairMap.end() ) loops.back().push_back( current ) , current = iter->second;
							else if( (iter=fValues.vertexPairMap.find(current))!=fValues.vertexPairMap.end() ) loops.back().push_back( current ) , current = iter->second;
							else if( (iter=xValues.vertexPairMap.find(current))!=xValues.vertexPairMap.end() ) loops.back().push_back( current ) , current = iter->second;
							else fprintf( stderr , "[ERROR] Failed to close loop @ depth %d / %d (%d): %lld\n" , depth , _sNodes.maxDepth-1 , i , current ) , exit( 0 );
						}
						else
						{
							loops.back().push_back( current );
							current = edges[idx][1];
							edges[idx] = edges.back() , edges.pop_back();
						}
					}
					loops.back().push_back( start );
				}
				// Add the loops to the mesh
				for( int j=0 ; j<loops.size() ; j++ )
				{
					std::vector< std::pair< int , Vertex > > polygon( loops[j].size() );
					for( int k=0 ; k<loops[j].size() ; k++ )
					{
						long long key = loops[j][k];
						typename hash_map< long long , std::pair< int , Vertex > >::const_iterator iter;
						if     ( ( iter=bValues.edgeVertexMap.find( key ) )!=bValues.edgeVertexMap.end() ) polygon[k] = iter->second;
						else if( ( iter=fValues.edgeVertexMap.find( key ) )!=fValues.edgeVertexMap.end() ) polygon[k] = iter->second;
						else if( ( iter=xValues.edgeVertexMap.find( key ) )!=xValues.edgeVertexMap.end() ) polygon[k] = iter->second;
						else fprintf( stderr , "[ERROR] Couldn't find vertex in edge map\n" ) , exit( 0 );
					}
					AddIsoPolygons( mesh , polygon , polygonMesh , addBarycenter , vOffset );
				}
			}
		}
	}
}
template< class Real > void SetIsoVertexValue(      PlyVertex< float >& vertex , Real value ){ ; }
template< class Real > void SetIsoVertexValue( PlyValueVertex< float >& vertex , Real value ){ vertex.value = float(value); }
template< class Real , int Degree >
template< class Vertex >
bool Octree< Real , Degree >::GetIsoVertex( ConstPointer( Real ) kernelDensityWeights , Real isoValue , typename TreeOctNode::ConstNeighborKey3& neighborKey3 , const TreeOctNode* node , int edgeIndex , int z , const SliceValues< Vertex >& sValues , Vertex& vertex )
{
	Point3D< Real > position;
	int c0 , c1;
	Square::EdgeCorners( edgeIndex , c0 , c1 );

	Real x0 , x1;
	Point3D< Real > n0 , n1;
	const typename SortedTreeNodes::SquareCornerIndices& idx = sValues.sliceData.cornerIndices( node );
	x0 = sValues.cornerValues[idx[c0]] , x1 = sValues.cornerValues[idx[c1]];
	if( sValues.cornerNormals ) n0 = sValues.cornerNormals[idx[c0]] , n1 = sValues.cornerNormals[idx[c1]];

	int o , y;
	Square::FactorEdgeIndex( edgeIndex , o , y );

	Point3D< Real > c;
	Real center , width;
	node->centerAndWidth( c , width );
	center = c[o];
	for( int i=0 ; i<DIMENSION ; i++ ) n0[i] *= width , n1[i] *= width;

	switch(o)
	{
	case 0:
		position[1] = c[1] - width/2 + width*y;
		position[2] = c[2] - width/2 + width*z;
		break;
	case 1:
		position[0] = c[0] - width/2 + width*y;
		position[2] = c[2] - width/2 + width*z;
		break;
	}

	double dx0 = n0[o] , dx1 = n1[o];
	{
		double dAverage = ( dx0 + dx1 ) / 2.;
		dx0 = dx0 * (1.-_normalSmooth) + dAverage * _normalSmooth;
		dx1 = dx1 * (1.-_normalSmooth) + dAverage * _normalSmooth;
	}

	// The scaling will turn the Hermite Spline into a quadratic
	double scl = (x1-x0) / ( (dx1+dx0 ) / 2 );
	dx0 *= scl , dx1 *= scl;

	// Hermite Spline
	Polynomial< 2 > P;
	P.coefficients[0] = x0;
	P.coefficients[1] = dx0;
	P.coefficients[2] = 3*(x1-x0)-dx1-2*dx0;

	Real averageRoot=0;
	double roots[2];
	int rCount = 0 , rootCount = P.getSolutions( isoValue , roots , EPSILON );
	for( int i=0 ; i<rootCount ; i++ ) if( roots[i]>=0 && roots[i]<=1 ) averageRoot += Real( roots[i] ) , rCount++;
	if( rCount && sValues.cornerNormals ) averageRoot /= rCount;
	else					              averageRoot  = Real( ( x0-isoValue ) / ( x0-x1 ) );
	if( averageRoot<0 || averageRoot>1 )
	{
		fprintf( stderr , "[WARNING] Bad average root: %f\n" , averageRoot );
		fprintf( stderr , "\t(%f %f) , (%f %f) (%f)\n" , x0 , x1 , dx0 , dx1 , isoValue );
		if( averageRoot<0 ) averageRoot = 0;
		if( averageRoot>1 ) averageRoot = 1;
	}
	position[o] = Real( center - width/2 + width*averageRoot );
	vertex.point = position;
	if( kernelDensityWeights )
	{
		Real depth , weight;
		const TreeOctNode* temp = node;
		while( temp->depth()>_splatDepth ) temp=temp->parent;
		GetSampleDepthAndWeight( kernelDensityWeights , temp , position , neighborKey3 , _samplesPerNode , depth , weight );
		SetIsoVertexValue( vertex , depth );
	}
	return true;
}
template< class Real , int Degree >
template< class Vertex >
bool Octree< Real , Degree >::GetIsoVertex( ConstPointer( Real ) kernelDensityWeights , Real isoValue , typename TreeOctNode::ConstNeighborKey3& neighborKey3 , const TreeOctNode* node , int cornerIndex , const SliceValues< Vertex >& bValues , const SliceValues< Vertex >& fValues , Vertex& vertex )
{
	Point3D< Real > position;

	Real x0 , x1;
	Point3D< Real > n0 , n1;
	const typename SortedTreeNodes::SquareCornerIndices& idx0 = bValues.sliceData.cornerIndices( node );
	const typename SortedTreeNodes::SquareCornerIndices& idx1 = fValues.sliceData.cornerIndices( node );
	x0 = bValues.cornerValues[ idx0[cornerIndex] ] , x1 = fValues.cornerValues[ idx1[cornerIndex] ];
	if( bValues.cornerNormals || fValues.cornerNormals ) n0 = bValues.cornerNormals[ idx0[cornerIndex] ] , n1 = fValues.cornerNormals[ idx1[cornerIndex] ];

	int x , y;
	Square::FactorCornerIndex( cornerIndex , x , y );

	Point3D< Real > c;
	Real center , width;
	node->centerAndWidth( c , width );
	center = c[2];
	for( int i=0 ; i<DIMENSION ; i++ ) n0[i] *= width , n1[i] *= width;

	position[0] = c[0] - width/2 + width*x;
	position[1] = c[1] - width/2 + width*y;

	double dx0 = n0[2] , dx1 = n1[2];
	{
		double dAverage = ( dx0 + dx1 ) / 2.;
		dx0 = dx0 * (1.-_normalSmooth) + dAverage * _normalSmooth;
		dx1 = dx1 * (1.-_normalSmooth) + dAverage * _normalSmooth;
	}

	// The scaling will turn the Hermite Spline into a quadratic
	double scl = (x1-x0) / ( (dx1+dx0 ) / 2 );
	dx0 *= scl , dx1 *= scl;

	// Hermite Spline
	Polynomial< 2 > P;
	P.coefficients[0] = x0;
	P.coefficients[1] = dx0;
	P.coefficients[2] = 3*(x1-x0)-dx1-2*dx0;

	Real averageRoot=0;
	double roots[2];
	int rCount = 0 , rootCount = P.getSolutions( isoValue , roots , EPSILON );
	for( int i=0 ; i<rootCount ; i++ ) if( roots[i]>=0 && roots[i]<=1 ) averageRoot += Real( roots[i] ) , rCount++;
	if( rCount && bValues.cornerNormals && fValues.cornerNormals ) averageRoot /= rCount;
	else					                                       averageRoot  = Real( ( x0-isoValue ) / ( x0-x1 ) );
	if( averageRoot<0 || averageRoot>1 )
	{
		fprintf( stderr , "[WARNING] Bad average root: %f\n" , averageRoot );
		fprintf( stderr , "\t(%f %f) , (%f %f) (%f)\n" , x0 , x1 , dx0 , dx1 , isoValue );
		if( averageRoot<0 ) averageRoot = 0;
		if( averageRoot>1 ) averageRoot = 1;
	}
	position[2] = Real( center - width/2 + width*averageRoot );
	vertex.point = position;
	if( kernelDensityWeights )
	{
		Real depth , weight;
		const TreeOctNode* temp = node;
		while( temp->depth()>_splatDepth ) temp=temp->parent;
		GetSampleDepthAndWeight( kernelDensityWeights , temp , position , neighborKey3 , _samplesPerNode , depth , weight );
		SetIsoVertexValue( vertex , depth );
	}
	return true;
}

template< class Real , int Degree >
template< class Vertex >
int Octree< Real , Degree >::AddIsoPolygons( CoredMeshData< Vertex >& mesh , std::vector< std::pair< int , Vertex > >& polygon , bool polygonMesh , bool addBarycenter , int& vOffset )
{
	if( polygonMesh )
	{
		std::vector< int > vertices( polygon.size() );
		for( int i=0 ; i<(int)polygon.size() ; i++ ) vertices[i] = polygon[polygon.size()-1-i].first;
		mesh.addPolygon_s( vertices );
		return 1;
	}
	if( polygon.size()>3 )
	{
		bool isCoplanar = false;
		std::vector< int > triangle( 3 );

		if( addBarycenter )
			for( int i=0 ; i<(int)polygon.size() ; i++ )
				for( int j=0 ; j<i ; j++ )
					if( (i+1)%polygon.size()!=j && (j+1)%polygon.size()!=i )
					{
						Vertex v1 = polygon[i].second , v2 = polygon[j].second;
						for( int k=0 ; k<3 ; k++ ) if( v1.point[k]==v2.point[k] ) isCoplanar = true;
					}
		if( isCoplanar )
		{
			Vertex c;
			c *= 0;
			for( int i=0 ; i<(int)polygon.size() ; i++ ) c += polygon[i].second;
			c /= Real( polygon.size() );
			int cIdx;
#pragma omp critical (add_barycenter_point_access)
			{
				cIdx = mesh.addOutOfCorePoint( c );
				vOffset++;
			}
			for( int i=0 ; i<(int)polygon.size() ; i++ )
			{
				triangle[0] = polygon[ i                  ].first;
				triangle[1] = cIdx;
				triangle[2] = polygon[(i+1)%polygon.size()].first;
				mesh.addPolygon_s( triangle );
			}
			return (int)polygon.size();
		}
		else
		{
			MinimalAreaTriangulation< Real > MAT;
			std::vector< Point3D< Real > > vertices;
			std::vector< TriangleIndex > triangles;
			vertices.resize( polygon.size() );
			// Add the points
			for( int i=0 ; i<(int)polygon.size() ; i++ ) vertices[i] = polygon[i].second.point;
			MAT.GetTriangulation( vertices , triangles );
			for( int i=0 ; i<(int)triangles.size() ; i++ )
			{
				for( int j=0 ; j<3 ; j++ ) triangle[2-j] = polygon[ triangles[i].idx[j] ].first;
				mesh.addPolygon_s( triangle );
			}
		}
	}
	else if( polygon.size()==3 )
	{
		std::vector< int > vertices( 3 );
		for( int i=0 ; i<3 ; i++ ) vertices[2-i] = polygon[i].first;
		mesh.addPolygon_s( vertices );
	}
	return (int)polygon.size()-2;
}
