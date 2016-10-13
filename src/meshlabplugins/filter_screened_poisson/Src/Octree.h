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

#ifndef OCT_NODE_INCLUDED
#define OCT_NODE_INCLUDED

#define NEW_OCTNODE_CODE

#include "Allocator.h"
#include "BinaryNode.h"
#include "MarchingCubes.h"


#define DIMENSION 3

template< class NodeData >
class OctNode
{
private:
	static int UseAlloc;
	unsigned long long _depthAndOffset;

	const OctNode* __faceNeighbor( int dir , int off ) const;
	const OctNode* __edgeNeighbor( int o , const int i[2] , const int idx[2] ) const;
	OctNode* __faceNeighbor( int dir , int off , int forceChildren , void (*Initializer)( OctNode& ) );
	OctNode* __edgeNeighbor( int o , const int i[2] , const int idx[2] , int forceChildren , void (*Initializer)( OctNode& ) );
public:
	static const int DepthShift , OffsetShift , OffsetShift1 , OffsetShift2 , OffsetShift3;
	static const int DepthMask , OffsetMask;

	static Allocator< OctNode > NodeAllocator;
	static int UseAllocator( void );
	static void SetAllocator( int blockSize );

	OctNode* parent;
	OctNode* children;
	NodeData nodeData;

	OctNode( void (*Initializer)( OctNode& )=NULL );
	static OctNode* NewBrood( void (*initializer)( OctNode& )=NULL );
	static void ResetDepthAndOffset( OctNode* root , int d , int off[3] );
	~OctNode( void );
	int initChildren( void (*Initializer)( OctNode& )=NULL );

	void depthAndOffset( int& depth , int offset[DIMENSION] ) const; 
	void centerIndex( int index[DIMENSION] ) const;
	int depth( void ) const;
	static inline void DepthAndOffset( const long long& index , int& depth , int offset[DIMENSION] );
	template< class Real > static inline void CenterAndWidth( const long long& index , Point3D< Real >& center , Real& width );
	template< class Real > static inline void StartAndWidth( const long long& index , Point3D< Real >& start , Real& width );
	static inline int Depth( const long long& index );
	static inline void Index( int depth , const int offset[3] , short& d , short off[DIMENSION] );
	static inline unsigned long long Index( int depth , const int offset[3] );
	template< class Real > void centerAndWidth( Point3D<Real>& center , Real& width ) const;
	template< class Real > void startAndWidth( Point3D< Real >& start , Real& width ) const;
	template< class Real > bool isInside( Point3D< Real > p ) const;

	size_t leaves( void ) const;
	size_t maxDepthLeaves( int maxDepth ) const;
	size_t nodes( void ) const;
	int maxDepth( void ) const;

	const OctNode* root( void ) const;

	const OctNode* nextLeaf( const OctNode* currentLeaf=NULL ) const;
	OctNode* nextLeaf( OctNode* currentLeaf=NULL );
	const OctNode* nextNode( const OctNode* currentNode=NULL ) const;
	OctNode* nextNode( OctNode* currentNode=NULL );
	const OctNode* nextBranch( const OctNode* current ) const;
	OctNode* nextBranch( OctNode* current );
	const OctNode* prevBranch( const OctNode* current ) const;
	OctNode* prevBranch( OctNode* current );

	void setFullDepth( int maxDepth , void (*Initializer)( OctNode& )=NULL );

	void printLeaves( void ) const;
	void printRange( void ) const;

	template< class Real > static int CornerIndex( const Point3D<Real>& center , const Point3D<Real> &p );

	OctNode* faceNeighbor( int faceIndex , int forceChildren , void (*Initializer)( OctNode& )=NULL );
	const OctNode* faceNeighbor( int faceIndex ) const;
	OctNode* edgeNeighbor( int edgeIndex , int forceChildren , void (*Initializer)( OctNode& )=NULL );
	const OctNode* edgeNeighbor( int edgeIndex ) const;
	OctNode* cornerNeighbor( int cornerIndex , int forceChildren , void (*Initializer)( OctNode& )=NULL );
	const OctNode* cornerNeighbor( int cornerIndex ) const;

	int write( const char* fileName ) const;
	int write( FILE* fp ) const;
	int read( const char* fileName , void (*Initializer)( OctNode& )=NULL );
	int read( FILE* fp , void (*Initializer)( OctNode& )=NULL );

	template< unsigned int Width >
	struct Neighbors
	{
		OctNode* neighbors[Width][Width][Width];
		Neighbors( void );
		void clear( void );
	};
	template< unsigned int Width >
	struct ConstNeighbors
	{
		const OctNode* neighbors[Width][Width][Width];
		ConstNeighbors( void );
		void clear( void );
	};

	template< unsigned int LeftRadius , unsigned int RightRadius >
	class NeighborKey
	{
		int _depth;
	public:
		template< int Width > using Neighbors = typename OctNode::template Neighbors< Width >;
		static const int Width = LeftRadius + RightRadius + 1;
		Neighbors< Width >* neighbors;

		NeighborKey( void );
		NeighborKey( const NeighborKey& key );
		~NeighborKey( void );
		int depth( void ) const { return _depth; }

		void set( int depth );
		template< bool CreateNodes > typename OctNode< NodeData >::template Neighbors< LeftRadius+RightRadius+1 >& getNeighbors( OctNode* node , void (*Initializer)( OctNode& )=NULL );
		template< bool CreateNodes , unsigned int _LeftRadius , unsigned int _RightRadius > void getNeighbors( OctNode* node , Neighbors< _LeftRadius + _RightRadius + 1 >& neighbors , void (*Initializer)( OctNode& )=NULL );
		template< bool CreateNodes > bool getChildNeighbors( int cIdx , int d , Neighbors< Width >& childNeighbors , void (*Initializer)( OctNode& )=NULL ) const;
		template< bool CreateNodes , class Real > bool getChildNeighbors( Point3D< Real > p , int d , Neighbors< Width >& childNeighbors , void (*Initializer)( OctNode& )=NULL ) const;
		typename OctNode< NodeData >::template Neighbors< LeftRadius+RightRadius+1 >& getNeighbors( const OctNode* node ) { return getNeighbors< false >( (OctNode*)node , NULL ); }
		template< unsigned int _LeftRadius , unsigned int _RightRadius > void getNeighbors( const OctNode* node , Neighbors< _LeftRadius + _RightRadius + 1 >& neighbors ){ return getNeighbors< false , _LeftRadius , _RightRadius >( (OctNode*)node , NULL ); }
		bool getChildNeighbors( int cIdx , int d , Neighbors< Width >& childNeighbors ) const { return getChildNeighbors< false >( cIdx , d , childNeighbors , NULL ); }
		template< class Real > bool getChildNeighbors( Point3D< Real > p , int d , Neighbors< Width >& childNeighbors ) const { return getChildNeighbors< false , Real >( p , d , childNeighbors , NULL ); }
	};

	template< unsigned int LeftRadius , unsigned int RightRadius >
	class ConstNeighborKey
	{
		int _depth;
	public:
		template< int Width > using Neighbors = typename OctNode::template ConstNeighbors< Width >;
		static const int Width = LeftRadius + RightRadius + 1;
		ConstNeighbors< Width >* neighbors;

		ConstNeighborKey( void );
		ConstNeighborKey( const ConstNeighborKey& key );
		~ConstNeighborKey( void );
		int depth( void ) const { return _depth; }

		void set( int depth );
		typename OctNode< NodeData >::template ConstNeighbors< LeftRadius+RightRadius+1 >& getNeighbors( const OctNode* node );
		template< unsigned int _LeftRadius , unsigned int _RightRadius > void getNeighbors( const OctNode* node , ConstNeighbors< _LeftRadius + _RightRadius + 1 >& neighbors );
	};

	void centerIndex( int maxDepth , int index[DIMENSION] ) const;
	int width( int maxDepth ) const;
};


#include "Octree.inl"

#endif // OCT_NODE_INCLUDED
