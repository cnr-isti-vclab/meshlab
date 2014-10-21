/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "Src/MarchingCubes.h"
#include "Src/Octree.h"
#include "Src/SparseMatrix.h"
#include "Src/CmdLineParser.h"
#include "Src/PPolynomial.h"
#include "Src/Ply.h"
void DumpOutput( const char* format , ... );
void DumpOutput2( char* str , const char* format , ... );

#include "Src/MultiGridOctreeData.h"

#include "filter_screened_poisson.h"
#include <QtScript>
void DumpOutput( const char* format , ... )
{
  char buf[4096];
  va_list marker;
  va_start( marker, format );

  vsprintf(buf,format,marker);
  va_end( marker );

  qDebug(buf);
 }
void DumpOutput2( char* str , const char* format , ... )
{
  char buf[4096];
  va_list marker;
  va_start( marker, format );

  vsprintf(buf,format,marker);
  va_end( marker );
  qDebug(buf);
}

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterScreenedPoissonPlugin::FilterScreenedPoissonPlugin()
{
}
template <class Real>
class PoissonParam
{
public:
  int MaxDepthVal;
  int MaxSolveDepthVal;
  int KernelDepthVal;
  int MinDepthVal;
  int FullDepthVal;
  Real SamplesPerNodeVal;
  Real ScaleVal;
  bool ConfidenceFlag;
  bool NormalWeightsFlag;
  Real PointWeightVal;
  int AdaptiveExponentVal;
  int BoundaryTypeVal;
  bool CompleteFlag;
bool NonManifoldFlag;
  bool ShowResidualFlag;
  int CGDepthVal;
  int ItersVal;
  Real CSSolverAccuracyVal;

  PoissonParam()
  {
    MaxDepthVal=8;
    MaxSolveDepthVal=-1;
    KernelDepthVal=-1;
    MinDepthVal=0;
    FullDepthVal=5;
    SamplesPerNodeVal =1.0f;
    ScaleVal=1.1f;
    ConfidenceFlag=false;
    NormalWeightsFlag=false;
    PointWeightVal = 4.0f;
    AdaptiveExponentVal=1;
    BoundaryTypeVal=1;
    CompleteFlag=false;
    NonManifoldFlag=false;
    ShowResidualFlag=false;
    CGDepthVal=0;
    ItersVal=8;
    CSSolverAccuracyVal=1e-3f;
  }
};

template< class Real , int Degree >
int Execute(CMeshO &m, CMeshO &pm, PoissonParam<Real> &pp)
{

  XForm4x4< Real > xForm=XForm4x4< Real >::Identity();
  int ThreadVal=1;
  // All the following parameters defaulf values are taken from the original command line PoissonRecon.cpp

  size_t stride = ((char *)&(m.vert[1].P()[0])) - ((char *)&(m.vert[0].P()[0]));
  MemoryPointStream<Real> myPointStream( &(m.vert[0].P()[0]),stride,&(m.vert[0].N()[0]),stride,m.vert.size());

    DumpOutput( "Running Screened Poisson Reconstruction (Version 6.11)\n" );

    double t;
    double tt=Time();
    Real isoValue = 0;

    Octree< Real , Degree > tree;
    tree.threads = ThreadVal;
    if( pp.MaxSolveDepthVal<0 ) pp.MaxSolveDepthVal = pp.MaxDepthVal;

    OctNode< TreeNodeData >::SetAllocator( MEMORY_ALLOCATOR_BLOCK_SIZE );

//    t=Time();
//    int kernelDepth = KernelDepth.set ?  KernelDepth.value : Depth.value-2;
    if(pp.KernelDepthVal<0) pp.KernelDepthVal =pp.MaxDepthVal-2;
    if( pp.KernelDepthVal>pp.MaxDepthVal )
    {
//        fprintf( stderr,"[ERROR] %s can't be greater than %s: %d <= %d\n" , KernelDepth.name , Depth.name , KernelDepth.value , Depth.value );
        return EXIT_FAILURE;
    }

    double maxMemoryUsage;
    t=Time();
     tree.maxMemoryUsage=0;
    typename Octree< Real , Degree >::PointInfo* pointInfo = new typename Octree< Real , Degree >::PointInfo();
    typename Octree< Real , Degree >::NormalInfo* normalInfo = new typename Octree< Real , Degree >::NormalInfo();
    std::vector< Real >* kernelDensityWeights = new std::vector< Real >();
    std::vector< Real >* centerWeights = new std::vector< Real >();
//    int SetTree( char* fileName , int minDepth , int maxDepth , int fullDepth , int splatDepth , Real samplesPerNode ,
//		Real scaleFactor , bool useConfidence , bool useNormalWeight , Real constraintWeight , int adaptiveExponent ,
//		PointInfo& pointInfo , NormalInfo& normalInfo , std::vector< Real >& kernelDensityWeights , std::vector< Real >& centerWeights ,
//		int boundaryType=BSplineElements< Degree >::NONE , XForm4x4< Real > xForm=XForm4x4< Real >::Identity , bool makeComplete=false );


//    int pointCount = tree.template SetTree< float >( In.value , MinDepth.value , Depth.value , FullDepth.value , kernelDepth , Real(SamplesPerNode.value) , Scale.value , Confidence.set , NormalWeights.set , PointWeight.value , AdaptiveExponent.value , *pointInfo , *normalInfo , *kernelDensityWeights , *centerWeights , BoundaryType.value , xForm , Complete.set );
      int pointCount = tree.template SetTree< Scalarm >(  0, pp.MinDepthVal , pp.MaxDepthVal , pp.FullDepthVal , pp.KernelDepthVal , pp.SamplesPerNodeVal ,
                                                        pp.ScaleVal , pp.ConfidenceFlag , pp.NormalWeightsFlag , pp.PointWeightVal , pp.AdaptiveExponentVal ,
                                                        *pointInfo , *normalInfo , *kernelDensityWeights , *centerWeights ,
                                                        &myPointStream, pp.BoundaryTypeVal , xForm , pp.CompleteFlag );
//    if( !Density.set ) delete kernelDensityWeights , kernelDensityWeights = NULL;

    DumpOutput("#             Tree set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
    DumpOutput( "Input Points: %d\n" , pointCount );
    DumpOutput( "Leaves/Nodes: %d/%d\n" , tree.tree.leaves() , tree.tree.nodes() );
    DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage() )/(1<<20) );

    maxMemoryUsage = tree.maxMemoryUsage;
    t=Time() , tree.maxMemoryUsage=0;
    Pointer( Real ) constraints = tree.SetLaplacianConstraints( *normalInfo );
    delete normalInfo;
    DumpOutput("#      Constraints set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
    DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage())/(1<<20) );
    maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

    t=Time() , tree.maxMemoryUsage=0;
    Pointer( Real ) solution = tree.SolveSystem( *pointInfo , constraints , pp.ShowResidualFlag , pp.ItersVal , pp.MaxSolveDepthVal , pp.CGDepthVal , pp.CSSolverAccuracyVal );
    delete pointInfo;
    FreePointer( constraints );
    DumpOutput( "# Linear system solved in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
    DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage() )/(1<<20) );
    maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

    CoredFileMeshData< PlyValueVertex< float > > mesh;

    tree.maxMemoryUsage=0;
    t=Time();
    isoValue = tree.GetIsoValue( solution , *centerWeights );
    delete centerWeights;
    DumpOutput( "Got average in: %f\n" , Time()-t );
    DumpOutput( "Iso-Value: %e\n" , isoValue );

//    if( VoxelGrid.set )
//    {
//        double t = Time();
//        FILE* fp = fopen( VoxelGrid.value , "wb" );
//        if( !fp ) fprintf( stderr , "Failed to open voxel file for writing: %s\n" , VoxelGrid.value );
//        else
//        {
//            int res;
//            Pointer( Real ) values = tree.Evaluate( solution , res , isoValue , VoxelDepth.value );
//            fwrite( &res , sizeof(int) , 1 , fp );
//            if( sizeof(Real)==sizeof(float) ) fwrite( values , sizeof(float) , res*res*res , fp );
//            else
//            {
//                float *fValues = new float[res*res*res];
//                for( int i=0 ; i<res*res*res ; i++ ) fValues[i] = float( values[i] );
//                fwrite( fValues , sizeof(float) , res*res*res , fp );
//                delete[] fValues;
//            }
//            fclose( fp );
//            DeletePointer( values );
//        }
//        DumpOutput( "Got voxel grid in: %f\n" , Time()-t );
//    }

//    if( Out.set )
//    {
        t = Time() , tree.maxMemoryUsage = 0;
        tree.GetMCIsoSurface( kernelDensityWeights ? GetPointer( *kernelDensityWeights ) : NullPointer< Real >() , solution , isoValue , mesh , true , !pp.NonManifoldFlag , false /*PolygonMesh.set*/ );
//        if( PolygonMesh.set ) DumpOutput("#         Got polygons in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
//        else                  DumpOutput("#        Got triangles in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
        DumpOutput("#        Got triangles in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
        maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );
        DumpOutput( "#             Total Solve: %9.1f (s), %9.1f (MB)\n" , Time()-tt , maxMemoryUsage );

//        if( NoComments.set )
//        {
//            if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , NULL , 0 , iXForm );
//            else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , NULL , 0 , iXForm );
//        }
//        else
//        {
//            if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , comments , commentNum , iXForm );
//            else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , comments , commentNum , iXForm );
//        }
        DumpOutput( "Vertices / Polygons: %d / %d\n" , mesh.outOfCorePointCount()+mesh.inCorePoints.size() , mesh.polygonCount() );
//    }
    FreePointer( solution );


    mesh.resetIterator();
    int vm = mesh.outOfCorePointCount()+mesh.inCorePoints.size();
    int fm = mesh.polygonCount();

    vcg::tri::Allocator<CMeshO>::AddVertices(pm,vm);
//    vcg::tri::Allocator<CMeshO>::AddFaces(pm,fm);

     PlyValueVertex< float > p;
    int i;
    for (i=0; i < int(mesh.inCorePoints.size()); i++){
//      p=mesh.inCorePoints[i];
      pm.vert[i].P()[0] = mesh.inCorePoints[i].point[0];
      pm.vert[i].P()[1] = mesh.inCorePoints[i].point[1];
      pm.vert[i].P()[2] = mesh.inCorePoints[i].point[2];
    }
    for (int ii=0; ii < mesh.outOfCorePointCount(); ii++){
      mesh.nextOutOfCorePoint(p);
      pm.vert[ii+i].P()[0] = p.point[0];
      pm.vert[ii+i].P()[1] = p.point[1];
      pm.vert[ii+i].P()[2] = p.point[2];
    }

    std::vector< CoredVertexIndex > polygon;

    while(mesh.nextPolygon( polygon ))
    {
      assert(polygon.size()==3);
      int indV[3];
      for( int i=0 ; i<int(polygon.size()) ; i++ )
      {
        if( polygon[i].inCore ) indV[i] = polygon[i].idx;
        else                    indV[i]= polygon[i].idx + int( mesh.inCorePoints.size() );
      }
      vcg::tri::Allocator<CMeshO>::AddFace(pm,&pm.vert[indV[0]],&pm.vert[indV[1]],&pm.vert[indV[2]]);
    }
    return 1;
}
bool FilterScreenedPoissonPlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos* cb)
{
    if (filterName == "Screened Poisson Surf. Reconstruction")
    {
      MeshModel *mm =md.mm();
      MeshModel *pm =md.addNewMesh("","Poisson mesh",false);
      PoissonParam<Scalarm> pp;

      pp.MaxDepthVal = env.evalInt("depth");
      Execute<Scalarm,2>(mm->cm,pm->cm,pp);
      pm->UpdateBoxAndNormals();
      return true;
    }
    return false;
}





MESHLAB_PLUGIN_NAME_EXPORTER(FilterScreenedPoissonPlugin)
