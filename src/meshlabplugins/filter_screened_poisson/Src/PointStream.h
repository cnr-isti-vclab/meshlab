/*
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
Copyright (c) 2016, Bruno Levy
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

#ifndef POINT_STREAM_INCLUDED
#define POINT_STREAM_INCLUDED

// [Bruno Levy 2016]: using a replacement class for
// Ply vertices, so that we
// do not need importing all the Ply I/O code.
// Adapted from Ply.h

#include "PlyVertexMini.h"

template< class Real >
class OrientedPointStream
{
public:
	virtual ~OrientedPointStream( void ){}
	virtual void reset( void ) = 0;
	virtual bool nextPoint( OrientedPoint3D< Real >& p ) = 0;
	virtual int nextPoints( OrientedPoint3D< Real >* p , int count )
	{
		int c=0;
		for( int i=0 ; i<count ; i++ , c++ ) if( !nextPoint( p[i] ) ) break;
		return c;
	}
	void boundingBox( Point3D< Real >& min , Point3D< Real >& max )
	{
		bool first = true;
		OrientedPoint3D< Real > p;
		while( nextPoint( p ) )
		{
			for( int i=0 ; i<3 ; i++ )
			{
				if( first || p.p[i]<min[i] ) min[i] = p.p[i];
				if( first || p.p[i]>max[i] ) max[i] = p.p[i];
			}
			first = false;
		}
		reset();
	}
};

template< class Real , class Data >
class OrientedPointStreamWithData : public OrientedPointStream< Real >
{
public:
	virtual ~OrientedPointStreamWithData( void ){}
	virtual void reset( void ) = 0;
	virtual bool nextPoint( OrientedPoint3D< Real >& p , Data& d ) = 0;

	virtual bool nextPoint( OrientedPoint3D< Real >& p ){ Data d ; return nextPoint( p , d ); }
	virtual int nextPoints( OrientedPoint3D< Real >* p , Data* d , int count )
	{
		int c=0;
		for( int i=0 ; i<count ; i++ , c++ ) if( !nextPoint( p[i] , d[i] ) ) break;
		return c;
	}
	virtual int nextPoints( OrientedPoint3D< Real >* p , int count ){ return OrientedPointStream< Real >::nextPoints( p , count ); }
};

template< class Real >
class TransformedOrientedPointStream : public OrientedPointStream< Real >
{
	XForm4x4< Real > _xForm;
	XForm3x3< Real > _normalXForm;
	OrientedPointStream< Real >& _stream;
public:
	TransformedOrientedPointStream( XForm4x4< Real > xForm , OrientedPointStream< Real >& stream ) : _xForm(xForm) , _stream(stream)
	{
		for( int i=0 ; i<3 ; i++ ) for( int j=0 ; j<3 ; j++ ) _normalXForm(i,j) = _xForm(i,j);
		_normalXForm = _normalXForm.transpose().inverse();
	};
	virtual void reset( void ){ _stream.reset(); }
	virtual bool nextPoint( OrientedPoint3D< Real >& p )
	{
		bool ret = _stream.nextPoint( p );
		p.p = _xForm * p.p , p.n = _normalXForm * p.n;
		return ret;
	}
};

template< class Real , class Data >
class TransformedOrientedPointStreamWithData : public OrientedPointStreamWithData< Real , Data >
{
	XForm4x4< Real > _xForm;
	XForm3x3< Real > _normalXForm;
	OrientedPointStreamWithData< Real , Data >& _stream;
public:
	TransformedOrientedPointStreamWithData( XForm4x4< Real > xForm , OrientedPointStreamWithData< Real , Data >& stream ) : _xForm(xForm) , _stream(stream)
	{
		for( int i=0 ; i<3 ; i++ ) for( int j=0 ; j<3 ; j++ ) _normalXForm(i,j) = _xForm(i,j);
		_normalXForm = _normalXForm.transpose().inverse();
	};
	virtual void reset( void ){ _stream.reset(); }
	virtual bool nextPoint( OrientedPoint3D< Real >& p , Data& d )
	{
		bool ret = _stream.nextPoint( p , d );
		p.p = _xForm * p.p , p.n = _normalXForm * p.n;
		return ret;
	}
};

template< class Real >
class MemoryOrientedPointStream : public OrientedPointStream< Real >
{
	const OrientedPoint3D< Real >* _points;
	size_t _pointCount;
	size_t _current;
public:
	MemoryOrientedPointStream( size_t pointCount , const OrientedPoint3D< Real >* points );
	~MemoryOrientedPointStream( void );
	void reset( void );
	bool nextPoint( OrientedPoint3D< Real >& p );
};

template< class Real , class Data >
class MemoryOrientedPointStreamWithData : public OrientedPointStreamWithData< Real , Data >
{
	const std::pair< OrientedPoint3D< Real > , Data >* _points;
	size_t _pointCount;
	size_t _current;
public:
	MemoryOrientedPointStreamWithData( size_t pointCount , const std::pair< OrientedPoint3D< Real > , Data >* points );
	~MemoryOrientedPointStreamWithData( void );
	void reset( void );
	bool nextPoint( OrientedPoint3D< Real >& p , Data& d );
};

template< class Real >
class ASCIIOrientedPointStream : public OrientedPointStream< Real >
{
	FILE* _fp;
public:
	ASCIIOrientedPointStream( const char* fileName );
	~ASCIIOrientedPointStream( void );
	void reset( void );
	bool nextPoint( OrientedPoint3D< Real >& p );
};

template< class Real , class Data >
class ASCIIOrientedPointStreamWithData : public OrientedPointStreamWithData< Real , Data >
{
	FILE* _fp;
	Data (*_readData)( FILE* );
public:
	ASCIIOrientedPointStreamWithData( const char* fileName , Data (*readData)( FILE* ) );
	~ASCIIOrientedPointStreamWithData( void );
	void reset( void );
	bool nextPoint( OrientedPoint3D< Real >& p , Data& d );
};

template< class Real , class RealOnDisk=Real >
class BinaryOrientedPointStream : public OrientedPointStream< Real >
{
	FILE* _fp;
	static const int POINT_BUFFER_SIZE=1024;
	OrientedPoint3D< RealOnDisk > _pointBuffer[ POINT_BUFFER_SIZE ];
	int _pointsInBuffer , _currentPointIndex;
public:
	BinaryOrientedPointStream( const char* filename );
	~BinaryOrientedPointStream( void );
	void reset( void );
	bool nextPoint( OrientedPoint3D< Real >& p );
};

template< class Real , class Data , class RealOnDisk=Real , class DataOnDisk=Data >
class BinaryOrientedPointStreamWithData : public OrientedPointStreamWithData< Real , Data >
{
	FILE* _fp;
	static const int POINT_BUFFER_SIZE=1024;
	std::pair< OrientedPoint3D< RealOnDisk > , DataOnDisk > _pointBuffer[ POINT_BUFFER_SIZE ];
	int _pointsInBuffer , _currentPointIndex;
public:
	BinaryOrientedPointStreamWithData( const char* filename );
	~BinaryOrientedPointStreamWithData( void );
	void reset( void );
	bool nextPoint( OrientedPoint3D< Real >& p , Data& d );
};

#include "PointStream.inl"
#endif // POINT_STREAM_INCLUDED
