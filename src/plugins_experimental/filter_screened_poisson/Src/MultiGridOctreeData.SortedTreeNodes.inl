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

/////////////////////
// SortedTreeNodes //
/////////////////////
SortedTreeNodes::SortedTreeNodes( void )
{
	nodeCount = NULL;
	treeNodes = NullPointer< TreeOctNode* >();
	maxDepth = 0;
	sliceOffsets = NullPointer< Pointer( int ) >();
}
SortedTreeNodes::~SortedTreeNodes( void )
{
	if( nodeCount ) delete[] nodeCount , nodeCount = NULL;
	if( treeNodes ) DeletePointer( treeNodes );
	if( sliceOffsets )
	{
		for( int d=0 ; d<maxDepth ; d++ ) FreePointer( sliceOffsets[d] );
		FreePointer( sliceOffsets );
	}
}
void SortedTreeNodes::set( TreeOctNode& root , std::vector< int >* map )
{
	if( nodeCount ) delete[] nodeCount;
	if( treeNodes ) DeletePointer( treeNodes );
	if( sliceOffsets )
	{
		for( int d=0 ; d<maxDepth ; d++ ) FreePointer( sliceOffsets[d] );
		FreePointer( sliceOffsets );
	}
	maxDepth = root.maxDepth()+1;
	nodeCount = new int[ maxDepth+1 ];
	treeNodes = NewPointer< TreeOctNode* >( root.nodes() );

	int startDepth = 0;
	nodeCount[0] = 0 , nodeCount[1] = 1;
	treeNodes[0] = &root;
	for( int d=startDepth+1 ; d<maxDepth ; d++ )
	{
		nodeCount[d+1] = nodeCount[d];
		for( int i=nodeCount[d-1] ; i<nodeCount[d] ; i++ )
		{
			TreeOctNode* temp = treeNodes[i];
			if( temp->children ) for( int c=0 ; c<8 ; c++ ) treeNodes[ nodeCount[d+1]++ ] = temp->children + c;
		}
	}
	_sortByZCoordinate();
	if( map )
	{
		map->resize( nodeCount[maxDepth] );
		for( int i=0 ; i<nodeCount[maxDepth] ; i++ ) (*map)[i] = treeNodes[i]->nodeData.nodeIndex;
	}
	for( int i=0 ; i<nodeCount[maxDepth] ; i++ ) treeNodes[i]->nodeData.nodeIndex = i;
}
int SortedTreeNodes::Slices( int depth ){ return 1<<depth; }
std::pair< int , int > SortedTreeNodes::sliceSpan( int depth , int off , int d ) const
{
	int dd = d-depth;
	return std::pair< int , int >( nodeCount[d] + sliceOffsets[d][off<<dd] , nodeCount[d] + sliceOffsets[d][(off+1)<<dd] );
}
void SortedTreeNodes::_sortByZCoordinate( void )
{
	sliceOffsets = AllocPointer< Pointer( int ) >( maxDepth );
	for( int d=0 ; d<maxDepth ;  d++ )
	{
		int slices = Slices( d );
		sliceOffsets[d] = AllocPointer< int >( slices+1 );
		memset( sliceOffsets[d] , 0 , sizeof(int)*(slices+1) );
		for( int i=nodeCount[d] ; i<nodeCount[d+1] ; i++ )
		{
			int _d , _off[3];
			treeNodes[i]->depthAndOffset( _d , _off );
			sliceOffsets[d][ _off[2] ]++;
		}
		for( int i=1 ; i<slices ; i++ ) sliceOffsets[d][i] += sliceOffsets[d][i-1];
		for( int i=slices ; i>=1 ; i-- ) sliceOffsets[d][i] = sliceOffsets[d][i-1];
		sliceOffsets[d][0] = 0;
	}
	for( TreeOctNode* node=treeNodes[0]->nextNode() ; node ; node=treeNodes[0]->nextNode( node ) )
	{
		int d , off[3];
		node->depthAndOffset( d , off );
		treeNodes[ nodeCount[d] + sliceOffsets[d][ off[2] ] ] = node;
		sliceOffsets[d][ off[2] ]++;
	}
	for( int d=0 ; d<maxDepth ; d++ )
	{
		for( int i=Slices(d) ; i>=1 ; i-- ) sliceOffsets[d][i] = sliceOffsets[d][i-1];
		sliceOffsets[d][0] = 0;
	}
}
SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::SliceTableData::cornerIndices( const TreeOctNode* node ) { return cTable[ node->nodeData.nodeIndex - nodeOffset ]; }
SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::SliceTableData::cornerIndices( int idx ) { return cTable[ idx - nodeOffset ]; }
const SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::SliceTableData::cornerIndices( const TreeOctNode* node ) const { return cTable[ node->nodeData.nodeIndex - nodeOffset ]; }
const SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::SliceTableData::cornerIndices( int idx ) const { return cTable[ idx - nodeOffset ]; }
SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::SliceTableData::edgeIndices( const TreeOctNode* node ) { return eTable[ node->nodeData.nodeIndex - nodeOffset ]; }
SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::SliceTableData::edgeIndices( int idx ) { return eTable[ idx - nodeOffset ]; }
const SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::SliceTableData::edgeIndices( const TreeOctNode* node ) const { return eTable[ node->nodeData.nodeIndex - nodeOffset ]; }
const SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::SliceTableData::edgeIndices( int idx ) const { return eTable[ idx - nodeOffset ]; }
SortedTreeNodes::SquareFaceIndices& SortedTreeNodes::SliceTableData::faceIndices( const TreeOctNode* node ) { return fTable[ node->nodeData.nodeIndex - nodeOffset ]; }
SortedTreeNodes::SquareFaceIndices& SortedTreeNodes::SliceTableData::faceIndices( int idx ) { return fTable[ idx - nodeOffset ]; }
const SortedTreeNodes::SquareFaceIndices& SortedTreeNodes::SliceTableData::faceIndices( const TreeOctNode* node ) const { return fTable[ node->nodeData.nodeIndex - nodeOffset ]; }
const SortedTreeNodes::SquareFaceIndices& SortedTreeNodes::SliceTableData::faceIndices( int idx ) const { return fTable[ idx - nodeOffset ]; }
SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::XSliceTableData::edgeIndices( const TreeOctNode* node ) { return eTable[ node->nodeData.nodeIndex - nodeOffset ]; }
SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::XSliceTableData::edgeIndices( int idx ) { return eTable[ idx - nodeOffset ]; }
const SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::XSliceTableData::edgeIndices( const TreeOctNode* node ) const { return eTable[ node->nodeData.nodeIndex - nodeOffset ]; }
const SortedTreeNodes::SquareCornerIndices& SortedTreeNodes::XSliceTableData::edgeIndices( int idx ) const { return eTable[ idx - nodeOffset ]; }
SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::XSliceTableData::faceIndices( const TreeOctNode* node ) { return fTable[ node->nodeData.nodeIndex - nodeOffset ]; }
SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::XSliceTableData::faceIndices( int idx ) { return fTable[ idx - nodeOffset ]; }
const SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::XSliceTableData::faceIndices( const TreeOctNode* node ) const { return fTable[ node->nodeData.nodeIndex - nodeOffset ]; }
const SortedTreeNodes::SquareEdgeIndices& SortedTreeNodes::XSliceTableData::faceIndices( int idx ) const { return fTable[ idx - nodeOffset ]; }


void SortedTreeNodes::setSliceTableData( SliceTableData& sData , int depth , int offset , int threads ) const
{
	if( offset<0 || offset>(1<<depth) ) return;
	if( threads<=0 ) threads = 1;
	// The vector of per-depth node spans
	std::pair< int , int > span( nodeCount[depth] + sliceOffsets[depth][ std::max<int>(0,offset-1) ] , nodeCount[depth] + sliceOffsets[depth][ std::min<int>(1<<depth,offset+1) ] );
	sData.nodeOffset = span.first;
	sData.nodeCount = span.second - span.first;

	sData._cMap.clear() , sData._eMap.clear() , sData._fMap.clear();
	sData._cMap.resize( sData.nodeCount * Square::CORNERS , 0 ) , sData._eMap.resize( sData.nodeCount * Square::EDGES , 0 ) , sData._fMap.resize( sData.nodeCount * Square::FACES , 0 );
	sData.cTable.resize( sData.nodeCount ) , sData.eTable.resize( sData.nodeCount ) , sData.fTable.resize( sData.nodeCount );
	std::vector< TreeOctNode::ConstNeighborKey3 > neighborKeys( std::max< int >( 1 , threads ) );
	for( int i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( depth );
#pragma omp parallel for num_threads( threads )
	for( int i=span.first ; i<span.second ; i++ )
	{
		TreeOctNode::ConstNeighborKey3& neighborKey = neighborKeys[ omp_get_thread_num() ];
		TreeOctNode* node = treeNodes[i];
		const TreeOctNode::ConstNeighbors3& neighbors = neighborKey.getNeighbors( node );
		int d , off[3];
		node->depthAndOffset( d , off );
		int z;
		if     ( off[2]==offset-1 ) z = 1;
		else if( off[2]==offset   ) z = 0;
		else fprintf( stderr , "[ERROR] Node out of bounds: %d %d\n" , offset , off[2] ) , exit( 0 );
		// Process the corners
		for( int x=0 ; x<2 ; x++ ) for( int y=0 ; y<2 ; y++ )
		{
			int c = Cube::CornerIndex( x , y , z );
			int fc = Square::CornerIndex( x , y );
			bool cornerOwner = true;
			int ac = Cube::AntipodalCornerIndex(c); // The index of the node relative to the corner
			for( int cc=0 ; cc<Cube::CORNERS ; cc++ ) // Iterate over the corner's cells
			{
				int xx , yy , zz;
				Cube::FactorCornerIndex( cc , xx , yy , zz );
				xx += x , yy += y , zz += z;
				if( neighbors.neighbors[xx][yy][zz] && cc<ac ){ cornerOwner = false ; break; }
			}
			if( cornerOwner )
			{
				int myCount = (i - sData.nodeOffset) * Square::CORNERS + fc;
				sData._cMap[ myCount ] = 1;
				for( int cc=0 ; cc<Cube::CORNERS ; cc++ )
				{
					int xx , yy , zz;
					Cube::FactorCornerIndex( cc , xx , yy , zz );
					int ac = Square::CornerIndex( 1-xx , 1-yy );
					xx += x , yy += y , zz += z;
					if( neighbors.neighbors[xx][yy][zz] ) sData.cornerIndices( neighbors.neighbors[xx][yy][zz] )[ac] = myCount;
				}
			}
		}
		// Process the edges
		for( int o=0 ; o<2 ; o++ ) for( int y=0 ; y<2 ; y++ )
		{
			int fe = Square::EdgeIndex( o , y );
			bool edgeOwner = true;

			int ac = Square::AntipodalCornerIndex( Square::CornerIndex( y , z ) );
			for( int cc=0 ; cc<Square::CORNERS ; cc++ )
			{
				int ii , jj , xx , yy , zz;
				Square::FactorCornerIndex( cc , ii , jj );
				ii += y , jj += z;
				switch( o )
				{
				case 0: yy = ii , zz = jj , xx = 1 ; break;
				case 1: xx = ii , zz = jj , yy = 1 ; break;
				}
				if( neighbors.neighbors[xx][yy][zz] && cc<ac ){ edgeOwner = false ; break; }
			}
			if( edgeOwner )
			{
				int myCount = ( i - sData.nodeOffset ) * Square::EDGES + fe;
				sData._eMap[ myCount ] = 1;
				// Set all edge indices
				for( int cc=0 ; cc<Square::CORNERS ; cc++ )
				{
					int ii , jj , aii , ajj , xx , yy , zz;
					Square::FactorCornerIndex( cc , ii , jj );
					Square::FactorCornerIndex( Square::AntipodalCornerIndex( cc ) , aii , ajj );
					ii += y , jj += z;
					switch( o )
					{
					case 0: yy = ii , zz = jj , xx = 1 ; break;
					case 1: xx = ii , zz = jj , yy = 1 ; break;
					}
					if( neighbors.neighbors[xx][yy][zz] ) sData.edgeIndices( neighbors.neighbors[xx][yy][zz] )[ Square::EdgeIndex( o , aii ) ] = myCount;
				}
			}
		}
		// Process the Faces
		{
			bool faceOwner = !( neighbors.neighbors[1][1][2*z] && !z );
			if( faceOwner )
			{
				int myCount = ( i - sData.nodeOffset ) * Square::FACES;
				sData._fMap[ myCount ] = 1;
				// Set the face indices
				sData.faceIndices( node )[0] = myCount;
				if( neighbors.neighbors[1][1][2*z] ) sData.faceIndices( neighbors.neighbors[1][1][2*z] )[0] = myCount;
			}
		}
	}
	int cCount = 0 , eCount = 0 , fCount = 0;

	for( int i=0 ; i<sData._cMap.size() ; i++ ) if( sData._cMap[i] ) sData._cMap[i] = cCount++;
	for( int i=0 ; i<sData._eMap.size() ; i++ ) if( sData._eMap[i] ) sData._eMap[i] = eCount++;
	for( int i=0 ; i<sData._fMap.size() ; i++ ) if( sData._fMap[i] ) sData._fMap[i] = fCount++;
#pragma omp parallel for num_threads( threads )
	for( int i=0 ; i<sData.nodeCount ; i++ )
	{
		for( int j=0 ; j<Square::CORNERS ; j++ ) sData.cTable[i][j] = sData._cMap[ sData.cTable[i][j] ];
		for( int j=0 ; j<Square::EDGES   ; j++ ) sData.eTable[i][j] = sData._eMap[ sData.eTable[i][j] ];
		for( int j=0 ; j<Square::FACES   ; j++ ) sData.fTable[i][j] = sData._fMap[ sData.fTable[i][j] ];
	}

	sData.cCount = cCount , sData.eCount = eCount , sData.fCount = fCount;
}
void SortedTreeNodes::setXSliceTableData( XSliceTableData& sData , int depth , int offset , int threads ) const
{
	if( offset<0 || offset>=(1<<depth) ) return;
	if( threads<=0 ) threads = 1;
	// The vector of per-depth node spans
	std::pair< int , int > span( nodeCount[depth] + sliceOffsets[depth][offset] , nodeCount[depth] + sliceOffsets[depth][offset+1] );
	sData.nodeOffset = span.first;
	sData.nodeCount = span.second - span.first;

	sData._eMap.clear() , sData._fMap.clear();
	sData._eMap.resize( sData.nodeCount * Square::CORNERS , 0 ) , sData._fMap.resize( sData.nodeCount * Square::EDGES , 0 );
	sData.eTable.resize( sData.nodeCount ) , sData.fTable.resize( sData.nodeCount );
	std::vector< TreeOctNode::ConstNeighborKey3 > neighborKeys( std::max< int >( 1 , threads ) );
	for( int i=0 ; i<neighborKeys.size() ; i++ ) neighborKeys[i].set( depth );
#pragma omp parallel for num_threads( threads )
	for( int i=span.first ; i<span.second ; i++ )
	{
		TreeOctNode::ConstNeighborKey3& neighborKey = neighborKeys[ omp_get_thread_num() ];
		TreeOctNode* node = treeNodes[i];
		const TreeOctNode::ConstNeighbors3& neighbors = neighborKey.getNeighbors( node );
		int d , off[3];
		node->depthAndOffset( d , off );
		// Process the edges
		int o=2;
		for( int x=0 ; x<2 ; x++ ) for( int y=0 ; y<2 ; y++ )
		{
			int fc = Square::CornerIndex( x , y );
			bool edgeOwner = true;

			int ac = Square::AntipodalCornerIndex( Square::CornerIndex( x , y ) );
			for( int cc=0 ; cc<Square::CORNERS ; cc++ )
			{
				int ii , jj , xx , yy , zz;
				Square::FactorCornerIndex( cc , ii , jj );
				ii += x , jj += y;
				xx = ii , yy = jj , zz = 1;
				if( neighbors.neighbors[xx][yy][zz] && cc<ac ){ edgeOwner = false ; break; }
			}
			if( edgeOwner )
			{
				int myCount = ( i - sData.nodeOffset ) * Square::CORNERS + fc;
				sData._eMap[ myCount ] = 1;

				// Set all edge indices
				for( int cc=0 ; cc<Square::CORNERS ; cc++ )
				{
					int ii , jj , aii , ajj , xx , yy , zz;
					Square::FactorCornerIndex( cc , ii , jj );
					Square::FactorCornerIndex( Square::AntipodalCornerIndex( cc ) , aii , ajj );
					ii += x , jj += y;
					xx = ii , yy = jj , zz = 1;
					if( neighbors.neighbors[xx][yy][zz] ) sData.edgeIndices( neighbors.neighbors[xx][yy][zz] )[ Square::CornerIndex( aii , ajj ) ] = myCount;
				}
			}
		}
		// Process the faces
		for( int o=0 ; o<2 ; o++ ) for( int y=0 ; y<2 ; y++ )
		{
			bool faceOwner;
			if( o==0 ) faceOwner = !( neighbors.neighbors[1][2*y][1] && !y );
			else       faceOwner = !( neighbors.neighbors[2*y][1][1] && !y );
			if( faceOwner )
			{
				int fe = Square::EdgeIndex( o , y );
				int ae = Square::EdgeIndex( o , 1-y );
				int myCount = ( i - sData.nodeOffset ) * Square::EDGES + fe;
				sData._fMap[ myCount ] = 1;
				// Set the face indices
				sData.faceIndices( node )[fe] = myCount;
				if( o==0 && neighbors.neighbors[1][2*y][1] ) sData.faceIndices( neighbors.neighbors[1][2*y][1] )[ae] = myCount;
				if( o==1 && neighbors.neighbors[2*y][1][1] ) sData.faceIndices( neighbors.neighbors[2*y][1][1] )[ae] = myCount;
			}
		}
	}
	int eCount = 0 , fCount = 0;

	for( int i=0 ; i<sData._eMap.size() ; i++ ) if( sData._eMap[i] ) sData._eMap[i] = eCount++;
	for( int i=0 ; i<sData._fMap.size() ; i++ ) if( sData._fMap[i] ) sData._fMap[i] = fCount++;
#pragma omp parallel for num_threads( threads )
	for( int i=0 ; i<sData.nodeCount ; i++ )
	{
		for( int j=0 ; j<Square::CORNERS ; j++ ) sData.eTable[i][j] = sData._eMap[ sData.eTable[i][j] ];
		for( int j=0 ; j<Square::EDGES   ; j++ ) sData.fTable[i][j] = sData._fMap[ sData.fTable[i][j] ];
	}

	sData.eCount = eCount , sData.fCount = fCount;
}
