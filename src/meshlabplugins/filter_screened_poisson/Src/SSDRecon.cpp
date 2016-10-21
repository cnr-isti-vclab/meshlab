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

#undef FAST_COMPILE
#undef ARRAY_DEBUG
#define BRUNO_LEVY_FIX
#define FOR_RELEASE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <Windows.h>
#include <Psapi.h>
#endif // _WIN32 || _WIN64
#include "MyTime.h"
#include "MarchingCubes.h"
#include "Octree.h"
#include "SparseMatrix.h"
#include "CmdLineParser.h"
#include "PPolynomial.h"
#include "Ply.h"
#include "MemoryUsage.h"
#ifdef _OPENMP
#include "omp.h"
#endif // _OPENMP
void DumpOutput( const char* format , ... );
void DumpOutput2( std::vector< char* >& comments , const char* format , ... );
#include "MultiGridOctreeData.h"

#define DEFAULT_FULL_DEPTH 5

#define XSTR(x) STR(x)
#define STR(x) #x
#if DEFAULT_FULL_DEPTH
#pragma message ( "[WARNING] Setting default full depth to " XSTR(DEFAULT_FULL_DEPTH) )
#endif // DEFAULT_FULL_DEPTH

#include <stdarg.h>
char* outputFile=NULL;
int echoStdout=0;
void DumpOutput( const char* format , ... )
{
	if( outputFile )
	{
		FILE* fp = fopen( outputFile , "a" );
		va_list args;
		va_start( args , format );
		vfprintf( fp , format , args );
		fclose( fp );
		va_end( args );
	}
	if( echoStdout )
	{
		va_list args;
		va_start( args , format );
		vprintf( format , args );
		va_end( args );
	}
}
void DumpOutput2( std::vector< char* >& comments  , const char* format , ... )
{
	if( outputFile )
	{
		FILE* fp = fopen( outputFile , "a" );
		va_list args;
		va_start( args , format );
		vfprintf( fp , format , args );
		fclose( fp );
		va_end( args );
	}
	if( echoStdout )
	{
		va_list args;
		va_start( args , format );
		vprintf( format , args );
		va_end( args );
	}
	comments.push_back( new char[1024] );
	char* str = comments.back();
	va_list args;
	va_start( args , format );
	vsprintf( str , format , args );
	va_end( args );
	if( str[strlen(str)-1]=='\n' ) str[strlen(str)-1] = 0;
}


cmdLineString
	In( "in" ) ,
	Out( "out" ) ,
	VoxelGrid( "voxel" ) ,
	XForm( "xForm" );

cmdLineReadable
#if defined( _WIN32 ) || defined( _WIN64 )
	Performance( "performance" ) ,
#endif // _WIN32 || _WIN64
	ShowResidual( "showResidual" ) ,
	NoComments( "noComments" ) ,
	PolygonMesh( "polygonMesh" ) ,
	Confidence( "confidence" ) ,
	NormalWeights( "nWeights" ) ,
	NonManifold( "nonManifold" ) ,
	ASCII( "ascii" ) ,
	Density( "density" ) ,
	NonLinearFit( "nonLinearFit" ) ,
	PrimalVoxel( "primalVoxel" ) ,
#ifndef FAST_COMPILE
	FreeBoundary( "freeBoundary" ) ,
	Double( "double" ) ,
#endif // !FAST_COMPILE
	Verbose( "verbose" );

cmdLineInt
#ifndef FAST_COMPILE
	Degree( "degree" , 2 ) ,
#endif // !FAST_COMPILE
	Depth( "depth" , 8 ) ,
	CGDepth( "cgDepth" , 0 ) ,
	KernelDepth( "kernelDepth" ) ,
	AdaptiveExponent( "adaptiveExp" , 1 ) ,
	Iters( "iters" , 8 ) ,
	VoxelDepth( "voxelDepth" , -1 ) ,
	FullDepth( "fullDepth" , DEFAULT_FULL_DEPTH ) ,
	MaxSolveDepth( "maxSolveDepth" ) ,
	Threads( "threads" , omp_get_num_procs() );

cmdLineFloat
	Color( "color" , 16.f ) ,
	SamplesPerNode( "samplesPerNode" , 1.5f ) ,
	Scale( "scale" , 1.1f ) ,
	CGSolverAccuracy( "cgAccuracy" , 1e-3f ) ,
	LowResIterMultiplier( "iterMultiplier" , 1.5f ) , 
	ValueWeight   (    "valueWeight" , 4e-0f ) , 
	GradientWeight( "gradientWeight" , 1e-3f ) ,
	BiLapWeight   (    "biLapWeight" , 1e-5f );


cmdLineReadable* params[] =
{
#ifndef FAST_COMPILE
	&Degree , &Double , &FreeBoundary ,
#endif // !FAST_COMPILE
	&In , &Depth , &Out , &XForm ,
	&Scale , &Verbose , &CGSolverAccuracy , &NoComments , &LowResIterMultiplier ,
	&KernelDepth , &SamplesPerNode , &Confidence , &NormalWeights , &NonManifold , &PolygonMesh , &ASCII , &ShowResidual , &VoxelDepth ,
	&BiLapWeight ,
	&ValueWeight , &GradientWeight , &VoxelGrid , &Threads , &MaxSolveDepth ,
	&AdaptiveExponent ,
	&Density ,
	&FullDepth ,
	&CGDepth , &Iters ,
	&Color ,
	&NonLinearFit ,
	&PrimalVoxel ,
#if defined( _WIN32 ) || defined( _WIN64 )
	&Performance ,
#endif // _WIN32 || _WIN64
};


void ShowUsage( char* ex )
{
	printf( "Usage: %s\n" , ex );
	printf( "\t --%s <input points>\n" , In.name );

	printf( "\t[--%s <ouput triangle mesh>]\n" , Out.name );

	printf( "\t[--%s <ouput voxel grid>]\n" , VoxelGrid.name );

#ifndef FAST_COMPILE
	printf( "\t[--%s <b-spline degree>=%d]\n" , Degree.name , Degree.value );

#ifndef FOR_RELEASE
	printf( "\t[--%s]\n" , FreeBoundary.name );
#endif // !FOR_RELEASE
#endif // !FAST_COMPILE

	printf( "\t[--%s <maximum reconstruction depth>=%d]\n" , Depth.name , Depth.value );

	printf( "\t[--%s <scale factor>=%f]\n" , Scale.name , Scale.value );

	printf( "\t[--%s <minimum number of samples per node>=%f]\n" , SamplesPerNode.name, SamplesPerNode.value );

	printf( "\t[--%s <zero-crossing weight>=%.3e]\n" , ValueWeight.name , ValueWeight.value );

	printf( "\t[--%s <gradient weight>=%.3e]\n" , GradientWeight.name , GradientWeight.value );

	printf( "\t[--%s <bi-laplacian weight>=%.3e]\n" , BiLapWeight.name , BiLapWeight.value );

	printf( "\t[--%s]\n" , Confidence.name );

	printf( "\t[--%s]\n" , NormalWeights.name );

#ifndef FOR_RELEASE
	printf( "\t[--%s <adaptive weighting exponent>=%d]\n", AdaptiveExponent.name , AdaptiveExponent.value );
#endif // !FOR_RELEASE

	printf( "\t[--%s <iterations>=%d]\n" , Iters.name , Iters.value );

#ifndef FOR_RELEASE
	printf( "\t[--%s <low-resolution iteration multiplier>=%f]\n" , LowResIterMultiplier.name , LowResIterMultiplier.value );
#endif // FOR_RELEASE

	printf( "\t[--%s <conjugate-gradients depth>=%d]\n" , CGDepth.name , CGDepth.value );

#ifndef FOR_RELEASE
	printf( "\t[--%s <conjugate-gradients solver accuracy>=%g]\n" , CGSolverAccuracy.name , CGSolverAccuracy.value );
#endif // !FOR_RELEASE

	printf( "\t[--%s <full depth>=%d]\n" , FullDepth.name , FullDepth.value );

	printf( "\t[--%s <depth at which to extract the voxel grid>=<%s>]\n" , VoxelDepth.name , Depth.name );

	printf( "\t[--%s]\n" , PrimalVoxel.name );

	printf( "\t[--%s <pull factor>]\n" , Color.name );

	printf( "\t[--%s]\n" , Density.name );

	printf( "\t[--%s]\n" , NonLinearFit.name );

	printf( "\t[--%s]\n" , PolygonMesh.name);

#ifndef FOR_RELEASE
	printf( "\t[--%s]\n" , NonManifold.name );
#endif // !FOR_RELEASE

#ifdef _OPENMP
	printf( "\t[--%s <num threads>=%d]\n" , Threads.name , Threads.value );
#endif // _OPENMP

	printf( "\t[--%s]\n" , Verbose.name );

#ifndef FOR_RELEASE
#if defined( _WIN32 ) || defined( _WIN64 )
	printf( "\t[--%s]\n" , Performance.name );
#endif // _WIN32 || _WIN64

#endif // !FOR_RELEASE
#ifndef FOR_RELEASE
	printf( "\t[--%s]\n" , ASCII.name );
	
	printf( "\t[--%s]\n" , NoComments.name );
#endif // !FOR_RELEASE
	
#ifndef FAST_COMPILE
	printf( "\t[--%s]\n" , Double.name );
#endif // !FAST_COMPILE
}

template< class Real >
struct ColorInfo
{
	static Point3D< Real > ReadASCII( FILE* fp )
	{
		Point3D< unsigned char > c;
		if( fscanf( fp , " %c %c %c " , &c[0] , &c[1] , &c[2] )!=3 ) fprintf( stderr , "[ERROR] Failed to read color\n" ) , exit( 0 );
		return Point3D< Real >( (Real)c[0] , (Real)c[1] , (Real)c[2] );
	};
	static bool ValidPlyProperties( const bool* props ){ return ( props[0] || props[3] ) && ( props[1] || props[4] ) && ( props[2] || props[5] ); }
	const static PlyProperty PlyProperties[];
};
template<>
const PlyProperty ColorInfo< float >::PlyProperties[] =
{
	{ "r"     , PLY_UCHAR , PLY_FLOAT , int( offsetof( Point3D< float > , coords[0] ) ) , 0 , 0 , 0 , 0 } ,
	{ "g"     , PLY_UCHAR , PLY_FLOAT , int( offsetof( Point3D< float > , coords[1] ) ) , 0 , 0 , 0 , 0 } ,
	{ "b"     , PLY_UCHAR , PLY_FLOAT , int( offsetof( Point3D< float > , coords[2] ) ) , 0 , 0 , 0 , 0 } ,
	{ "red"   , PLY_UCHAR , PLY_FLOAT , int( offsetof( Point3D< float > , coords[0] ) ) , 0 , 0 , 0 , 0 } , 
	{ "green" , PLY_UCHAR , PLY_FLOAT , int( offsetof( Point3D< float > , coords[1] ) ) , 0 , 0 , 0 , 0 } ,
	{ "blue"  , PLY_UCHAR , PLY_FLOAT , int( offsetof( Point3D< float > , coords[2] ) ) , 0 , 0 , 0 , 0 }
};
template<>
const PlyProperty ColorInfo< double >::PlyProperties[] =
{
	{ "r"     , PLY_UCHAR , PLY_DOUBLE , int( offsetof( Point3D< double > , coords[0] ) ) , 0 , 0 , 0 , 0 } ,
	{ "g"     , PLY_UCHAR , PLY_DOUBLE , int( offsetof( Point3D< double > , coords[1] ) ) , 0 , 0 , 0 , 0 } ,
	{ "b"     , PLY_UCHAR , PLY_DOUBLE , int( offsetof( Point3D< double > , coords[2] ) ) , 0 , 0 , 0 , 0 } ,
	{ "red"   , PLY_UCHAR , PLY_DOUBLE , int( offsetof( Point3D< double > , coords[0] ) ) , 0 , 0 , 0 , 0 } , 
	{ "green" , PLY_UCHAR , PLY_DOUBLE , int( offsetof( Point3D< double > , coords[1] ) ) , 0 , 0 , 0 , 0 } ,
	{ "blue"  , PLY_UCHAR , PLY_DOUBLE , int( offsetof( Point3D< double > , coords[2] ) ) , 0 , 0 , 0 , 0 }
};

bool ValidPlyColorProperties( const bool* props ){ return ( props[0] || props[3] ) && ( props[1] || props[4] ) && ( props[2] || props[5] ); }

double Weight( double v , double start , double end )
{
	v = ( v - start ) / ( end - start );
	if     ( v<0 ) return 1.;
	else if( v>1 ) return 0.;
	else
	{
		// P(x) = a x^3 + b x^2 + c x + d
		//		P (0) = 1 , P (1) = 0 , P'(0) = 0 , P'(1) = 0
		// =>	d = 1 , a + b + c + d = 0 , c = 0 , 3a + 2b + c = 0
		// =>	c = 0 , d = 1 , a + b = -1 , 3a + 2b = 0
		// =>	a = 2 , b = -3 , c = 0 , d = 1
		// =>	P(x) = 2 x^3 - 3 x^2 + 1
		return 2. * v * v * v - 3. * v * v + 1.;
	}
}

#if defined( _WIN32 ) || defined( _WIN64 )
double PeakMemoryUsageMB( void )
{
	HANDLE h = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	return GetProcessMemoryInfo( h , &pmc , sizeof(pmc) ) ? ( (double)pmc.PeakWorkingSetSize )/(1<<20) : 0;
}
#endif // _WIN32 || _WIN64


template< class Real >
struct OctreeProfiler
{
	Octree< Real >& tree;
	double t;

	OctreeProfiler( Octree< Real >& t ) : tree(t) { ; }
	void start( void ){ t = Time() , tree.resetLocalMemoryUsage(); }
	void print( const char* header ) const
	{
		tree.memoryUsage();
#if defined( _WIN32 ) || defined( _WIN64 )
		if( header ) printf( "%s %9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" , header , Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() , PeakMemoryUsageMB() );
		else         printf(    "%9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" ,          Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() , PeakMemoryUsageMB() );
#else // !_WIN32 && !_WIN64
		if( header ) printf( "%s %9.1f (s), %9.1f (MB) / %9.1f (MB)\n" , header , Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() );
		else         printf(    "%9.1f (s), %9.1f (MB) / %9.1f (MB)\n" ,          Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() );
#endif // _WIN32 || _WIN64
	}
	void dumpOutput( const char* header ) const
	{
		tree.memoryUsage();
#if defined( _WIN32 ) || defined( _WIN64 )
		if( header ) DumpOutput( "%s %9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" , header , Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() , PeakMemoryUsageMB() );
		else         DumpOutput(    "%9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" ,          Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() , PeakMemoryUsageMB() );
#else // !_WIN32 && !_WIN64
		if( header ) DumpOutput( "%s %9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" , header , Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() );
		else         DumpOutput(    "%9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" ,          Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() );
#endif // _WIN32 || _WIN64
	}
	void dumpOutput2( std::vector< char* >& comments , const char* header ) const
	{
		tree.memoryUsage();
#if defined( _WIN32 ) || defined( _WIN64 )
		if( header ) DumpOutput2( comments , "%s %9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" , header , Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() , PeakMemoryUsageMB() );
		else         DumpOutput2( comments ,    "%9.1f (s), %9.1f (MB) / %9.1f (MB) / %9.1f (MB)\n" ,          Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() , PeakMemoryUsageMB() );
#else // !_WIN32 && !_WIN64
		if( header ) DumpOutput2( comments , "%s %9.1f (s), %9.1f (MB) / %9.1f (MB)\n" , header , Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() );
		else         DumpOutput2( comments ,    "%9.1f (s), %9.1f (MB) / %9.1f (MB)\n" ,          Time()-t , tree.localMemoryUsage() , tree.maxMemoryUsage() );
#endif // _WIN32 || _WIN64
	}
};

template< class Real >
XForm4x4< Real > GetPointXForm( OrientedPointStream< Real >& stream , Real scaleFactor )
{
	Point3D< Real > min , max;
	stream.boundingBox( min , max );
	Point3D< Real > center = ( max + min ) / 2;
	Real scale = std::max< Real >( max[0]-min[0] , std::max< Real >( max[1]-min[1] , max[2]-min[2] ) );
	scale *= scaleFactor;
	for( int i=0 ; i<3 ; i++ ) center[i] -= scale/2;
	XForm4x4< Real > tXForm = XForm4x4< Real >::Identity() , sXForm = XForm4x4< Real >::Identity();
	for( int i=0 ; i<3 ; i++ ) sXForm(i,i) = (Real)(1./scale ) , tXForm(3,i) = -center[i];
	return sXForm * tXForm;
}

template< class Real , int Degree , BoundaryType BType , class Vertex >
int _Execute( int argc , char* argv[] )
{
	typedef typename Octree< Real >::template InterpolationInfo< true > InterpolationInfo;
	typedef OrientedPointStream< Real > PointStream;
	typedef OrientedPointStreamWithData< Real , Point3D< Real > > PointStreamWithData;
	typedef TransformedOrientedPointStream< Real > XPointStream;
	typedef TransformedOrientedPointStreamWithData< Real , Point3D< Real > > XPointStreamWithData;
	Reset< Real >();
	int paramNum = sizeof(params)/sizeof(cmdLineReadable*);
	std::vector< char* > comments;

	if( Verbose.set ) echoStdout=1;

	XForm4x4< Real > xForm , iXForm;
	if( XForm.set )
	{
		FILE* fp = fopen( XForm.value , "r" );
		if( !fp )
		{
			fprintf( stderr , "[WARNING] Could not read x-form from: %s\n" , XForm.value );
			xForm = XForm4x4< Real >::Identity();
		}
		else
		{
			for( int i=0 ; i<4 ; i++ ) for( int j=0 ; j<4 ; j++ )
			{
				float f;
				if( fscanf( fp , " %f " , &f )!=1 ) fprintf( stderr , "[ERROR] Execute: Failed to read xform\n" ) , exit( 0 );
				xForm(i,j) = (Real)f;
			}
			fclose( fp );
		}
	}
	else xForm = XForm4x4< Real >::Identity();

	DumpOutput2( comments , "Running SSD Reconstruction (Version 9.0)\n" );
	char str[1024];
	for( int i=0 ; i<paramNum ; i++ )
		if( params[i]->set )
		{
			params[i]->writeValue( str );
			if( strlen( str ) ) DumpOutput2( comments , "\t--%s %s\n" , params[i]->name , str );
			else                DumpOutput2( comments , "\t--%s\n" , params[i]->name );
		}

	double startTime = Time();
	Real isoValue = 0;

	Octree< Real > tree;
	OctreeProfiler< Real > profiler( tree );
	tree.threads = Threads.value;
	if( !In.set )
	{
		ShowUsage( argv[0] );
		return 0;
	}
	if( !MaxSolveDepth.set ) MaxSolveDepth.value = Depth.value;
	
	OctNode< TreeNodeData >::SetAllocator( MEMORY_ALLOCATOR_BLOCK_SIZE );

	int kernelDepth = KernelDepth.set ? KernelDepth.value : Depth.value-2;
	if( kernelDepth>Depth.value )
	{
		fprintf( stderr,"[WARNING] %s can't be greater than %s: %d <= %d\n" , KernelDepth.name , Depth.name , KernelDepth.value , Depth.value );
		kernelDepth = Depth.value;
	}

	int pointCount;

	Real pointWeightSum;
	std::vector< typename Octree< Real >::PointSample >* samples = new std::vector< typename Octree< Real >::PointSample >();
	std::vector< ProjectiveData< Point3D< Real > , Real > >* sampleData = NULL;
	SparseNodeData< Real , WEIGHT_DEGREE >* density = NULL;
	SparseNodeData< Point3D< Real > , NORMAL_DEGREE >* normalInfo = NULL;
	Real targetValue = (Real)0.;

	// Read in the samples (and color data)
	{
		profiler.start();
		PointStream* pointStream;
		char* ext = GetFileExtension( In.value );
		if( Color.set && Color.value>0 )
		{
			sampleData = new std::vector< ProjectiveData< Point3D< Real > , Real > >();
			if     ( !strcasecmp( ext , "bnpts" ) ) pointStream = new BinaryOrientedPointStreamWithData< Real , Point3D< Real > , float , Point3D< unsigned char > >( In.value );
			else if( !strcasecmp( ext , "ply"   ) ) pointStream = new    PLYOrientedPointStreamWithData< Real , Point3D< Real > >( In.value , ColorInfo< Real >::PlyProperties , 6 , ColorInfo< Real >::ValidPlyProperties );
			else                                    pointStream = new  ASCIIOrientedPointStreamWithData< Real , Point3D< Real > >( In.value , ColorInfo< Real >::ReadASCII );
		}
		else
		{
			if     ( !strcasecmp( ext , "bnpts" ) ) pointStream = new BinaryOrientedPointStream< Real , float >( In.value );
			else if( !strcasecmp( ext , "ply"   ) ) pointStream = new    PLYOrientedPointStream< Real >( In.value );
			else                                    pointStream = new  ASCIIOrientedPointStream< Real >( In.value );
		}
		delete[] ext;
		XPointStream _pointStream( xForm , *pointStream );
		xForm = GetPointXForm( _pointStream , (Real)Scale.value ) * xForm;
		if( sampleData )
		{
			XPointStreamWithData _pointStream( xForm , ( PointStreamWithData& )*pointStream );
			pointCount = tree.template init< Point3D< Real > >( _pointStream , Depth.value , Confidence.set , *samples , sampleData );
		}
		else
		{
			XPointStream _pointStream( xForm , *pointStream );
			pointCount = tree.template init< Point3D< Real > >( _pointStream , Depth.value , Confidence.set , *samples , sampleData );
		}
		iXForm = xForm.inverse();
		delete pointStream;
#pragma omp parallel for num_threads( Threads.value )
		for( int i=0 ; i<(int)samples->size() ; i++ ) (*samples)[i].sample.data.n *= (Real)-1;

		DumpOutput( "Input Points / Samples: %d / %d\n" , pointCount , samples->size() );
		profiler.dumpOutput2( comments , "# Read input into tree:" );
	}

	DenseNodeData< Real , Degree > solution;
	// Solve
	{
		DenseNodeData< Real , Degree > constraints;
		InterpolationInfo* iInfo = NULL;
		int solveDepth = MaxSolveDepth.value;

		tree.resetNodeIndices();

		// Get the kernel density estimator
		{
			profiler.start();
			density = new SparseNodeData< Real , WEIGHT_DEGREE >();
			*density = tree.template setDensityEstimator< WEIGHT_DEGREE >( *samples , kernelDepth , SamplesPerNode.value );
			profiler.dumpOutput2( comments , "#   Got kernel density:" );
		}

		// Transform the Hermite samples into a vector field
		{
			profiler.start();
			normalInfo = new SparseNodeData< Point3D< Real > , NORMAL_DEGREE >();
			*normalInfo = tree.template setNormalField< NORMAL_DEGREE >( *samples , *density , pointWeightSum , BType==BOUNDARY_NEUMANN );
			profiler.dumpOutput2( comments , "#     Got normal field:" );
		}

		if( !Density.set ) delete density , density = NULL;

		// Trim the tree and prepare for multigrid
		{
			profiler.start();
			std::vector< int > indexMap;

			constexpr int MAX_DEGREE = NORMAL_DEGREE > Degree ? NORMAL_DEGREE : Degree;
			tree.template inalizeForBroodedMultigrid< MAX_DEGREE , Degree , BType >( FullDepth.value , typename Octree< Real >::template HasNormalDataFunctor< NORMAL_DEGREE >( *normalInfo ) , &indexMap );

			if( normalInfo ) normalInfo->remapIndices( indexMap );
			if( density ) density->remapIndices( indexMap );
			profiler.dumpOutput2( comments , "#       Finalized tree:" );
		}

		// Free up the normal info
		if( normalInfo ) delete normalInfo , normalInfo = NULL;

		// Add the interpolation constraints
		if( ValueWeight.value>0 || GradientWeight.value>0 )
		{
			profiler.start();
			iInfo = new InterpolationInfo( tree , *samples , targetValue , AdaptiveExponent.value , (Real)ValueWeight.value * pointWeightSum , (Real)GradientWeight.value * pointWeightSum );
			constraints = tree.template initDenseNodeData< Degree >( );
			tree.template addInterpolationConstraints< Degree , BType >( *iInfo , constraints , solveDepth );
			profiler.dumpOutput2( comments , "#Set point constraints:" );
		}

		DumpOutput( "Leaf Nodes / Active Nodes / Ghost Nodes: %d / %d / %d\n" , (int)tree.leaves() , (int)tree.nodes() , (int)tree.ghostNodes() );
		DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage())/(1<<20) );

		// Solve the linear system
		{
			profiler.start();
			typename Octree< Real >::SolverInfo solverInfo;
			solverInfo.cgDepth = CGDepth.value , solverInfo.iters = Iters.value , solverInfo.cgAccuracy = CGSolverAccuracy.value , solverInfo.verbose = Verbose.set , solverInfo.showResidual = ShowResidual.set , solverInfo.lowResIterMultiplier = std::max< double >( 1. , LowResIterMultiplier.value );
			solution = tree.template solveSystem< Degree , BType >( FEMSystemFunctor< Degree , BType >( 0 , 0 , BiLapWeight.value ) , iInfo , constraints , solveDepth , solverInfo );
			profiler.dumpOutput2( comments , "# Linear system solved:" );
			DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage() )/(1<<20) );
			if( iInfo ) delete iInfo , iInfo = NULL;
		}
	}

	CoredFileMeshData< Vertex > mesh;

	{
		profiler.start();
		double valueSum = 0 , weightSum = 0;
		typename Octree< Real >::template MultiThreadedEvaluator< Degree , BType > evaluator( &tree , solution , Threads.value );
#pragma omp parallel for num_threads( Threads.value ) reduction( + : valueSum , weightSum )
		for( int j=0 ; j<samples->size() ; j++ )
		{
			ProjectiveData< OrientedPoint3D< Real > , Real >& sample = (*samples)[j].sample;
			Real w = sample.weight;
			if( w>0 ) weightSum += w , valueSum += evaluator.value( sample.data.p / sample.weight , omp_get_thread_num() , (*samples)[j].node ) * w;
		}
		isoValue = (Real)( valueSum / weightSum );
		if( !( Color.set && Color.value>0 ) && samples ) delete samples , samples = NULL;
		profiler.dumpOutput( "Got average:" );
		DumpOutput( "Iso-Value: %e\n" , isoValue );
	}

	if( VoxelGrid.set )
	{
		profiler.start();
		FILE* fp = fopen( VoxelGrid.value , "wb" );
		if( !fp ) fprintf( stderr , "Failed to open voxel file for writing: %s\n" , VoxelGrid.value );
		else
		{
			int res = 0;
			Pointer( Real ) values = tree.template voxelEvaluate< Real , Degree , BType >( solution , res , isoValue , VoxelDepth.value , PrimalVoxel.set );
			fwrite( &res , sizeof(int) , 1 , fp );
			if( sizeof(Real)==sizeof(float) ) fwrite( values , sizeof(float) , res*res*res , fp );
			else
			{
				float *fValues = new float[res*res*res];
				for( int i=0 ; i<res*res*res ; i++ ) fValues[i] = float( values[i] );
				fwrite( fValues , sizeof(float) , res*res*res , fp );
				delete[] fValues;
			}
			fclose( fp );
			DeletePointer( values );
		}
		profiler.dumpOutput( "Got voxel grid:" );
	}

	if( Out.set )
	{
		profiler.start();
		SparseNodeData< ProjectiveData< Point3D< Real > , Real > , DATA_DEGREE >* colorData = NULL;
		if( sampleData )
		{
			colorData = new SparseNodeData< ProjectiveData< Point3D< Real > , Real > , DATA_DEGREE >();
			*colorData = tree.template setDataField< DATA_DEGREE , false >( *samples , *sampleData , (SparseNodeData< Real , WEIGHT_DEGREE >*)NULL );
			delete sampleData , sampleData = NULL;
			for( const OctNode< TreeNodeData >* n = tree.tree().nextNode() ; n ; n=tree.tree().nextNode( n ) )
			{
				ProjectiveData< Point3D< Real > , Real >* clr = (*colorData)( n );
				if( clr ) (*clr) *= (Real)pow( Color.value , tree.depth( n ) );
			}
		}
		tree.template getMCIsoSurface< Degree , BType , WEIGHT_DEGREE , DATA_DEGREE >( density , colorData , solution , isoValue , mesh , NonLinearFit.set , !NonManifold.set , PolygonMesh.set );
		DumpOutput( "Vertices / Polygons: %d / %d\n" , mesh.outOfCorePointCount()+mesh.inCorePoints.size() , mesh.polygonCount() );
		if( PolygonMesh.set ) profiler.dumpOutput2( comments , "#         Got polygons:" );
		else                  profiler.dumpOutput2( comments , "#        Got triangles:" );

		if( colorData ) delete colorData , colorData = NULL;

		if( NoComments.set )
		{
			if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , NULL , 0 , iXForm );
			else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , NULL , 0 , iXForm );
		}
		else
		{
			if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , &comments[0] , (int)comments.size() , iXForm );
			else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , &comments[0] , (int)comments.size() , iXForm );
		}
	}
	if( density ) delete density , density = NULL;
	DumpOutput2( comments , "#          Total Solve: %9.1f (s), %9.1f (MB)\n" , Time()-startTime , tree.maxMemoryUsage() );

	return 1;
}

#if defined( _WIN32 ) || defined( _WIN64 )
inline double to_seconds( const FILETIME& ft )
{
	const double low_to_sec=100e-9; // 100 nanoseconds
	const double high_to_sec=low_to_sec*4294967296.0;
	return ft.dwLowDateTime*low_to_sec+ft.dwHighDateTime*high_to_sec;
}
#endif // _WIN32 || _WIN64

#ifndef FAST_COMPILE
template< class Real , class Vertex >
int Execute( int argc , char* argv[] )
{
	if( FreeBoundary.set )
		switch( Degree.value )
		{
		case 2: return _Execute< Real , 2 , BOUNDARY_FREE , Vertex >( argc , argv );
		case 3: return _Execute< Real , 3 , BOUNDARY_FREE , Vertex >( argc , argv );
		case 4: return _Execute< Real , 4 , BOUNDARY_FREE , Vertex >( argc , argv );
		default: fprintf( stderr , "[ERROR] Only B-Splines of degree 2 - 4 are supported" ) ; return EXIT_FAILURE;
		}
	else
		switch( Degree.value )
		{
		case 2: return _Execute< Real , 2 , BOUNDARY_NEUMANN , Vertex >( argc , argv );
		case 3: return _Execute< Real , 3 , BOUNDARY_NEUMANN , Vertex >( argc , argv );
		case 4: return _Execute< Real , 4 , BOUNDARY_NEUMANN , Vertex >( argc , argv );
		default: fprintf( stderr , "[ERROR] Only B-Splines of degree 2 - 4 are supported" ) ; return EXIT_FAILURE;
		}
}
#endif // !FAST_COMPILE
int main( int argc , char* argv[] )
{
#if defined(WIN32) && defined(MAX_MEMORY_GB)
	if( MAX_MEMORY_GB>0 )
	{
		SIZE_T peakMemory = 1;
		peakMemory <<= 30;
		peakMemory *= MAX_MEMORY_GB;
		printf( "Limiting memory usage to %.2f GB\n" , float( peakMemory>>30 ) );
		HANDLE h = CreateJobObject( NULL , NULL );
		AssignProcessToJobObject( h , GetCurrentProcess() );

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_JOB_MEMORY;
		jeli.JobMemoryLimit = peakMemory;
		if( !SetInformationJobObject( h , JobObjectExtendedLimitInformation , &jeli , sizeof( jeli ) ) )
			fprintf( stderr , "Failed to set memory limit\n" );
	}
#endif // defined(WIN32) && defined(MAX_MEMORY_GB)
	double t = Time();

	cmdLineParse( argc-1 , &argv[1] , sizeof(params)/sizeof(cmdLineReadable*) , params , 1 );
	if( GradientWeight.value<=0 ) fprintf( stderr , "[ERROR] Gradient weight must be positive: %g>=0\n" , GradientWeight.value ) , exit( 0 );
	if( BiLapWeight.value<=0 ) fprintf( stderr , "[ERROR] Bi-Laplacian weight must be positive: %g>=0\n" , BiLapWeight.value ) , exit( 0 );
#ifdef FAST_COMPILE
	static const int Degree = 2;
	static const BoundaryType BType = BOUNDARY_NEUMANN;
	fprintf( stderr , "[WARNING] Compiling for degree-%d, boundary-%s, single-precision _only_\n" , Degree , BoundaryNames[ BType ] );
	if( Density.set )
		if( Color.set && Color.value>0 ) _Execute< float , Degree , BType , PlyColorAndValueVertex< float > >( argc , argv );
		else                             _Execute< float , Degree , BType , PlyValueVertex< float > >( argc , argv );
	else
		if( Color.set && Color.value>0 ) _Execute< float , Degree , BType , PlyColorVertex< float > >( argc , argv );
		else                             _Execute< float , Degree , BType , PlyVertex< float > >( argc , argv );
#else // !FAST_COMPILE
	if( Density.set )
		if( Color.set && Color.value>0 )
			if( Double.set ) Execute< double , PlyColorAndValueVertex< float > >( argc , argv );
			else             Execute< float  , PlyColorAndValueVertex< float > >( argc , argv );
		else
			if( Double.set ) Execute< double , PlyValueVertex< float > >( argc , argv );
			else             Execute< float  , PlyValueVertex< float > >( argc , argv );
	else
		if( Color.set && Color.value>0 )
			if( Double.set ) Execute< double , PlyColorVertex< float > >( argc , argv );
			else             Execute< float  , PlyColorVertex< float > >( argc , argv );
		else
			if( Double.set ) Execute< double , PlyVertex< float > >( argc , argv );
			else             Execute< float  , PlyVertex< float > >( argc , argv );
#endif // FAST_COMPILE
#if defined( _WIN32 ) || defined( _WIN64 )
	if( Performance.set )
	{
		HANDLE cur_thread=GetCurrentThread();
		FILETIME tcreat, texit, tkernel, tuser;
		if( GetThreadTimes( cur_thread , &tcreat , &texit , &tkernel , &tuser ) )
			printf( "Time (Wall/User/Kernel): %.2f / %.2f / %.2f\n" , Time()-t , to_seconds( tuser ) , to_seconds( tkernel ) );
		else printf( "Time: %.2f\n" , Time()-t );
		HANDLE h = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		if( GetProcessMemoryInfo( h , &pmc , sizeof(pmc) ) ) printf( "Peak Memory (MB): %d\n" , (int)(pmc.PeakWorkingSetSize>>20) );
	}
#endif // _WIN32 || _WIN64
	return EXIT_SUCCESS;
}
