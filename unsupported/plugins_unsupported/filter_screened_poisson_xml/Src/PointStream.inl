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


///////////////////////////////
// MemoryOrientedPointStream //
///////////////////////////////
template< class Real >
MemoryOrientedPointStream< Real >::MemoryOrientedPointStream( size_t pointCount , const OrientedPoint3D< Real >* points ){ _points = points , _pointCount = pointCount , _current = 0; }
template< class Real >
MemoryOrientedPointStream< Real >::~MemoryOrientedPointStream( void ){ ; }
template< class Real >
void MemoryOrientedPointStream< Real >::reset( void ) { _current=0; }
template< class Real >
bool MemoryOrientedPointStream< Real >::nextPoint( OrientedPoint3D< Real >& p )
{
	if( _current>=_pointCount ) return false;
	p = _points[_current];
	_current++;
	return true;
}

//////////////////////////////
// ASCIIOrientedPointStream //
//////////////////////////////
template< class Real >
ASCIIOrientedPointStream< Real >::ASCIIOrientedPointStream( const char* fileName )
{
	_fp = fopen( fileName , "r" );
	if( !_fp ) fprintf( stderr , "Failed to open file for reading: %s\n" , fileName ) , exit( 0 );
}
template< class Real >
ASCIIOrientedPointStream< Real >::~ASCIIOrientedPointStream( void )
{
	fclose( _fp );
	_fp = NULL;
}
template< class Real >
void ASCIIOrientedPointStream< Real >::reset( void ) { fseek( _fp , SEEK_SET , 0 ); }
template< class Real >
bool ASCIIOrientedPointStream< Real >::nextPoint( OrientedPoint3D< Real >& p )
{
	float c[2*3];
	if( fscanf( _fp , " %f %f %f %f %f %f " , &c[0] , &c[1] , &c[2] , &c[3] , &c[4] , &c[5] )!=2*3 ) return false;
	p.p[0] = c[0] , p.p[1] = c[1] , p.p[2] = c[2];
	p.n[0] = c[3] , p.n[1] = c[4] , p.n[2] = c[5];
	return true;
}

///////////////////////////////
// BinaryOrientedPointStream //
///////////////////////////////
template< class Real , class RealOnDisk >
BinaryOrientedPointStream< Real , RealOnDisk >::BinaryOrientedPointStream( const char* fileName )
{
	_pointsInBuffer = _currentPointIndex = 0;
	_fp = fopen( fileName , "rb" );
	if( !_fp ) fprintf( stderr , "Failed to open file for reading: %s\n" , fileName ) , exit( 0 );
}
template< class Real , class RealOnDisk >
BinaryOrientedPointStream< Real , RealOnDisk >::~BinaryOrientedPointStream( void )
{
	fclose( _fp );
	_fp = NULL;
}
template< class Real , class RealOnDisk >
void BinaryOrientedPointStream< Real , RealOnDisk >::reset( void )
{
	fseek( _fp , SEEK_SET , 0 );
	_pointsInBuffer = _currentPointIndex = 0;
}
template< class Real , class RealOnDisk >
bool BinaryOrientedPointStream< Real , RealOnDisk >::nextPoint( OrientedPoint3D< Real >& p )
{
	if( _currentPointIndex<_pointsInBuffer )
	{
		p = OrientedPoint3D< Real >( _pointBuffer[ _currentPointIndex ] );
		_currentPointIndex++;
		return true;
	}
	else
	{
		_currentPointIndex = 0;
		_pointsInBuffer = int( fread( _pointBuffer , sizeof( OrientedPoint3D< RealOnDisk > ) , POINT_BUFFER_SIZE , _fp ) );
		if( !_pointsInBuffer ) return false;
		else return nextPoint( p );
	}
}

///////////////////////////////////////
// MemoryOrientedPointStreamWithData //
///////////////////////////////////////
template< class Real , class Data >
MemoryOrientedPointStreamWithData< Real , Data >::MemoryOrientedPointStreamWithData( size_t pointCount , const std::pair< OrientedPoint3D< Real > , Data >* points ){ _points = points , _pointCount = pointCount , _current = 0; }
template< class Real , class Data >
MemoryOrientedPointStreamWithData< Real , Data >::~MemoryOrientedPointStreamWithData( void ){ ; }
template< class Real , class Data >
void MemoryOrientedPointStreamWithData< Real , Data >::reset( void ) { _current=0; }
template< class Real , class Data >
bool MemoryOrientedPointStreamWithData< Real , Data >::nextPoint( OrientedPoint3D< Real >& p , Data& d )
{
	if( _current>=_pointCount ) return false;
	p = _points[_current].first;
	d = _points[_current].second;
	_current++;
	return true;
}

//////////////////////////////////////
// ASCIIOrientedPointStreamWithData //
//////////////////////////////////////
template< class Real , class Data >
ASCIIOrientedPointStreamWithData< Real , Data >::ASCIIOrientedPointStreamWithData( const char* fileName , Data (*readData)( FILE* ) ) : _readData( readData )
{
	_fp = fopen( fileName , "r" );
	if( !_fp ) fprintf( stderr , "Failed to open file for reading: %s\n" , fileName ) , exit( 0 );
}
template< class Real , class Data >
ASCIIOrientedPointStreamWithData< Real , Data >::~ASCIIOrientedPointStreamWithData( void )
{
	fclose( _fp );
	_fp = NULL;
}
template< class Real , class Data >
void ASCIIOrientedPointStreamWithData< Real , Data >::reset( void ) { fseek( _fp , SEEK_SET , 0 ); }
template< class Real , class Data >
bool ASCIIOrientedPointStreamWithData< Real , Data >::nextPoint( OrientedPoint3D< Real >& p , Data& d )
{
	float c[2*3];
	if( fscanf( _fp , " %f %f %f %f %f %f " , &c[0] , &c[1] , &c[2] , &c[3] , &c[4] , &c[5] )!=2*3 ) return false;
	p.p[0] = c[0] , p.p[1] = c[1] , p.p[2] = c[2];
	p.n[0] = c[3] , p.n[1] = c[4] , p.n[2] = c[5];
	d = _readData( _fp );
	return true;
}

///////////////////////////////////////
// BinaryOrientedPointStreamWithData //
///////////////////////////////////////
template< class Real , class Data , class RealOnDisk , class DataOnDisk >
BinaryOrientedPointStreamWithData< Real , Data , RealOnDisk , DataOnDisk >::BinaryOrientedPointStreamWithData( const char* fileName )
{
	_pointsInBuffer = _currentPointIndex = 0;
	_fp = fopen( fileName , "rb" );
	if( !_fp ) fprintf( stderr , "Failed to open file for reading: %s\n" , fileName ) , exit( 0 );
}
template< class Real , class Data , class RealOnDisk , class DataOnDisk >
BinaryOrientedPointStreamWithData< Real , Data , RealOnDisk , DataOnDisk >::~BinaryOrientedPointStreamWithData( void )
{
	fclose( _fp );
	_fp = NULL;
}
template< class Real , class Data , class RealOnDisk , class DataOnDisk >
void BinaryOrientedPointStreamWithData< Real , Data , RealOnDisk , DataOnDisk >::reset( void )
{
	fseek( _fp , SEEK_SET , 0 );
	_pointsInBuffer = _currentPointIndex = 0;
}
template< class Real , class Data , class RealOnDisk , class DataOnDisk >
bool BinaryOrientedPointStreamWithData< Real , Data , RealOnDisk , DataOnDisk >::nextPoint( OrientedPoint3D< Real >& p , Data& d )
{
	if( _currentPointIndex<_pointsInBuffer )
	{
		p = OrientedPoint3D< Real >( _pointBuffer[ _currentPointIndex ].first );
		d = Data( _pointBuffer[ _currentPointIndex ].second );
		_currentPointIndex++;
		return true;
	}
	else
	{
		_currentPointIndex = 0;
		_pointsInBuffer = int( fread( _pointBuffer , sizeof( std::pair< OrientedPoint3D< RealOnDisk > , DataOnDisk > ) , POINT_BUFFER_SIZE , _fp ) );
		if( !_pointsInBuffer ) return false;
		else return nextPoint( p , d );
	}
}
