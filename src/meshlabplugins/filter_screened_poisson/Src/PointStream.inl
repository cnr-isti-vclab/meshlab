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
#include "Ply.h"

template< class Real >
ASCIIPointStream< Real >::ASCIIPointStream( const char* fileName )
{
	_fp = fopen( fileName , "r" );
	if( !_fp ) fprintf( stderr , "Failed to open file for reading: %s\n" , fileName ) , exit( 0 );
}
template< class Real >
ASCIIPointStream< Real >::~ASCIIPointStream( void )
{
	fclose( _fp );
	_fp = NULL;
}
template< class Real >
void ASCIIPointStream< Real >::reset( void ) { fseek( _fp , SEEK_SET , 0 ); }
template< class Real >
bool ASCIIPointStream< Real >::nextPoint( Point3D< Real >& p , Point3D< Real >& n )
{
	float c[2*DIMENSION];
	if( fscanf( _fp , " %f %f %f %f %f %f " , &c[0] , &c[1] , &c[2] , &c[3] , &c[4] , &c[5] )!=2*DIMENSION ) return false;
	p[0] = c[0] , p[1] = c[1] , p[2] = c[2];
	n[0] = c[3] , n[1] = c[4] , n[2] = c[5];
	return true;
}
template< class Real >
BinaryPointStream< Real >::BinaryPointStream( const char* fileName )
{
	_pointsInBuffer = _currentPointIndex = 0;
	_fp = fopen( fileName , "rb" );
	if( !_fp ) fprintf( stderr , "Failed to open file for reading: %s\n" , fileName ) , exit( 0 );
}
template< class Real >
BinaryPointStream< Real >::~BinaryPointStream( void )
{
	fclose( _fp );
	_fp = NULL;
}
template< class Real >
void BinaryPointStream< Real >::reset( void )
{
	fseek( _fp , SEEK_SET , 0 );
	_pointsInBuffer = _currentPointIndex = 0;
}
template< class Real >
bool BinaryPointStream< Real >::nextPoint( Point3D< Real >& p , Point3D< Real >& n )
{
	if( _currentPointIndex<_pointsInBuffer )
	{
		p[0] = _pointBuffer[ _currentPointIndex*6+0 ];
		p[1] = _pointBuffer[ _currentPointIndex*6+1 ];
		p[2] = _pointBuffer[ _currentPointIndex*6+2 ];
		n[0] = _pointBuffer[ _currentPointIndex*6+3 ];
		n[1] = _pointBuffer[ _currentPointIndex*6+4 ];
		n[2] = _pointBuffer[ _currentPointIndex*6+5 ];
		_currentPointIndex++;
		return true;
	}
	else
	{
		_currentPointIndex = 0;
		_pointsInBuffer = int( fread( _pointBuffer , sizeof( Real ) * 6 , POINT_BUFFER_SIZE , _fp ) );
		if( !_pointsInBuffer ) return false;
		else return nextPoint( p , n );
	}
}

template< class Real >
MemoryPointStream< Real >::MemoryPointStream(  void*pVec, size_t stride_p,  void*nVec, size_t stride_n, size_t pointNum)
{
  _pVec = (char *)pVec;
  _nVec = (char *)nVec;
  _pStride = stride_p;
  _nStride = stride_n;
  _curPos=0;
  _pointNum = pointNum;
}
template< class Real >
MemoryPointStream< Real >::~MemoryPointStream( void )
{
}
template< class Real >
void MemoryPointStream< Real >::reset( void )
{
  _curPos=0;
}
template< class Real >
bool MemoryPointStream< Real >::nextPoint( Point3D< Real >& p , Point3D< Real >& n )
{
  if(_curPos >= _pointNum) return false;
  Real *baseP = (Real *)(_pVec + _curPos*_pStride);
  p[0] = baseP[0];
  p[1] = baseP[1];
  p[2] = baseP[2];
  Real *baseN = (Real *)(_nVec + _curPos*_pStride);
  n[0] = baseN[0];
  n[1] = baseN[1];
  n[2] = baseN[2];
  _curPos++;
  return true;
}



template< class Real >
PLYPointStream< Real >::PLYPointStream( const char* fileName )
{
	_fileName = new char[ strlen( fileName )+1 ];
	strcpy( _fileName , fileName );
	_ply = NULL;
	reset();
}
template< class Real >
void PLYPointStream< Real >::reset( void )
{
	int fileType;
	float version;
	PlyProperty** plist;
	if( _ply ) _free();
	_ply = ply_open_for_reading( _fileName, &_nr_elems, &_elist, &fileType, &version );
	if( !_ply )
	{
		fprintf( stderr, "[ERROR] Failed to open ply file for reading: %s\n" , _fileName );
		exit( 0 );
	}
	bool foundVertices = false;
	for( int i=0 ; i<_nr_elems ; i++ )
	{
		int num_elems;
		int nr_props;
		char* elem_name = _elist[i];
		plist = ply_get_element_description( _ply , elem_name , &num_elems , &nr_props );
		if( !plist )
		{
			fprintf( stderr , "[ERROR] Failed to get element description: %s\n" , elem_name );
			exit( 0 );
		}	

		if( equal_strings( "vertex" , elem_name ) )
		{
			foundVertices = true;
			_pCount = num_elems , _pIdx = 0;
			for( int i=0 ; i<PlyOrientedVertex< Real >::Components ; i++ ) 
				if( !ply_get_property( _ply , elem_name , &(PlyOrientedVertex< Real >::Properties[i]) ) )
				{
					fprintf( stderr , "[ERROR] Failed to find property in ply file: %s\n" , PlyOrientedVertex< Real >::Properties[i].name );
					exit( 0 );
				}
		}
		for( int j=0 ; j<nr_props ; j++ )
		{
			free( plist[j]->name );
			free( plist[j] );
		}
		free( plist );
		if( foundVertices ) break;
	}
	if( !foundVertices )
	{
		fprintf( stderr , "[ERROR] Could not find vertices in ply file\n" );
		exit( 0 );
	}
}
template< class Real >
void PLYPointStream< Real >::_free( void )
{
	if( _ply ) ply_close( _ply ) , _ply = NULL;
	if( _elist )
	{
		for( int i=0 ; i<_nr_elems ; i++ ) free( _elist[i] );
		free( _elist );
	}
}
template< class Real >
PLYPointStream< Real >::~PLYPointStream( void )
{
	_free();
	if( _fileName ) delete[] _fileName , _fileName = NULL;
}
template< class Real >
bool PLYPointStream< Real >::nextPoint( Point3D< Real >& p , Point3D< Real >& n )
{
	if( _pIdx<_pCount )
	{
		PlyOrientedVertex< Real > op;
		ply_get_element( _ply, (void *)&op );
		p = op.point;
		n = op.normal;
		_pIdx++;
		return true;
	}
	else return false;
}
