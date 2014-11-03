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

#ifdef WIN32
#include <windows.h>
#endif

#include "Src/MarchingCubes.h"
#include "Src/Octree.h"
#include "Src/SparseMatrix.h"
#include "Src/CmdLineParser.h"
#include "Src/PPolynomial.h"
#include "Src/Ply.h"
void DumpOutput( const char* format , ... );
void DumpOutput2( char* str , const char* format , ... );

#include "Src/PointStream.h"

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
  bool DensityFlag;
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
    DensityFlag=false;
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

template< class Real >
class MeshModelPointStream : public PointStream< Real >
{
  CMeshO &_m;
  size_t _curPos;
public:
  MeshModelPointStream(  CMeshO &m):_m(m),_curPos(0)
  {
    vcg::tri::RequireCompactness(m);
  }
  ~MeshModelPointStream( void ){}
  void reset( void ) { _curPos =0;}
  bool nextPoint( Point3D< Real >& p , Point3D< Real >& n )
  {
    if(_curPos>=_m.vn)
      return false;

    p[0] = _m.vert[_curPos].P()[0];
    p[1] = _m.vert[_curPos].P()[1];
    p[2] = _m.vert[_curPos].P()[2];
    n[0] = _m.vert[_curPos].N()[0];
    n[1] = _m.vert[_curPos].N()[1];
    n[2] = _m.vert[_curPos].N()[2];
    ++_curPos;
    return true;
  }
};

template< class Real >
class MeshDocumentPointStream : public PointStream< Real >
{
  MeshDocument &_md;
  MeshModel *_curMesh;
  size_t _curPos;
  size_t _totalSize;
public:
  MeshDocumentPointStream(  MeshDocument &md):_md(md),_curMesh(0),_curPos(0)
  {
    _totalSize=0;
    MeshModel *m=0;
    do
    {
      m=_md.nextVisibleMesh(m);
      if(m!=0)
      {
        vcg::tri::RequireCompactness(m->cm);
        _totalSize+=m->cm.vn;
      }
    } while(m);
    qDebug("TotalSize %i",_totalSize);
  }
  ~MeshDocumentPointStream( void ){}
  void reset( void ) { _curPos =0; _curMesh=0;}
  bool nextPoint( Point3D< Real >& p , Point3D< Real >& n )
  {
    Point3m nn(0,0,0);
//    do
    {
      if((_curMesh==0) || (_curPos >= _curMesh->cm.vn) )
      {
        _curMesh = _md.nextVisibleMesh(_curMesh);
        _curPos = 0;
      }

      if(_curMesh==0)
        return false;
      if(_curPos < _curMesh->cm.vn)
      {
        nn = _curMesh->cm.vert[_curPos].N();
        Point3m tp = _curMesh->cm.Tr * _curMesh->cm.vert[_curPos].P();
        Point4m np = _curMesh->cm.Tr *  Point4m(nn[0],nn[1],nn[2],0);
//        Point3m tp = _curMesh->cm.vert[_curPos].P();
//        Point3m np = nn;
        p[0] = tp[0];
        p[1] = tp[1];
        p[2] = tp[2];
        n[0] = np[0];
        n[1] = np[1];
        n[2] = np[2];
        ++_curPos;
      }
    }
    assert(nn!=Point3m(0,0,0));
    return true;
  }
};


template< class Real>
bool Execute(PointStream< Real > *ps, CMeshO &pm, PoissonParam<Real> &pp, vcg::CallBackPos* cb)
{
  Reset< Real >();
  XForm4x4< Real > xForm=XForm4x4< Real >::Identity();

  cb(1,"Running Screened Poisson Reconstruction\n" );

  double t;
  double tt=Time();
  Real isoValue = 0;

  Octree< Real > tree;
  tree.threads = 1;
  if( pp.MaxSolveDepthVal<0 ) pp.MaxSolveDepthVal = pp.MaxDepthVal;

  //    OctNode< TreeNodeData >::SetAllocator( MEMORY_ALLOCATOR_BLOCK_SIZE );
  OctNode< TreeNodeData >::SetAllocator( 0 );

  //    int kernelDepth = KernelDepth.set ?  KernelDepth.value : Depth.value-2;
  if(pp.KernelDepthVal<0) pp.KernelDepthVal =pp.MaxDepthVal-2;
  if( pp.KernelDepthVal>pp.MaxDepthVal )
    return false;

  cb(10,"Creating Tree");
  double maxMemoryUsage;
  t=Time();
  //     tree.maxMemoryUsage=0;
  typename Octree< Real >::PointInfo* pointInfo = new typename Octree< Real >::PointInfo();
  typename Octree< Real >::NormalInfo* normalInfo = new typename Octree< Real >::NormalInfo();
  std::vector< Real >* kernelDensityWeights = new std::vector< Real >();
  std::vector< Real >* centerWeights = new std::vector< Real >();
  //    int SetTree( char* fileName , int minDepth , int maxDepth , int fullDepth , int splatDepth , Real samplesPerNode ,
  //		Real scaleFactor , bool useConfidence , bool useNormalWeight , Real constraintWeight , int adaptiveExponent ,
  //		PointInfo& pointInfo , NormalInfo& normalInfo , std::vector< Real >& kernelDensityWeights , std::vector< Real >& centerWeights ,
  //		int boundaryType=BSplineElements< Degree >::NONE , XForm4x4< Real > xForm=XForm4x4< Real >::Identity , bool makeComplete=false );

  TreeNodeData::NodeCount=0;
  int pointCount = tree.template SetTree< Scalarm >(  0, pp.MinDepthVal , pp.MaxDepthVal , pp.FullDepthVal , pp.KernelDepthVal , pp.SamplesPerNodeVal ,
                                                      pp.ScaleVal , pp.ConfidenceFlag , pp.NormalWeightsFlag , pp.PointWeightVal , pp.AdaptiveExponentVal ,
                                                      *pointInfo , *normalInfo , *kernelDensityWeights , *centerWeights ,
                                                      ps, pp.BoundaryTypeVal , xForm , pp.CompleteFlag );

  DumpOutput("#             Tree set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
  DumpOutput( "Input Points: %d\n" , pointCount );
  DumpOutput( "Leaves/Nodes: %d/%d\n" , tree.tree.leaves() , tree.tree.nodes() );
  DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage() )/(1<<20) );

  maxMemoryUsage = tree.maxMemoryUsage;
  cb(20,"Settng Constraints");
  t=Time() , tree.maxMemoryUsage=0;
  Pointer( Real ) constraints = tree.SetLaplacianConstraints( *normalInfo );
  delete normalInfo;
  DumpOutput("#      Constraints set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
  DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage())/(1<<20) );
  maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

  cb(70,"Solving Linear system");
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

  cb(80,"Building Isosurface");
  t = Time() , tree.maxMemoryUsage = 0;
  assert(kernelDensityWeights);
  tree.GetMCIsoSurface(  GetPointer( *kernelDensityWeights ) , solution , isoValue , mesh , true , !pp.NonManifoldFlag , false /*PolygonMesh.set*/ );
  DumpOutput("#        Got triangles in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
  maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );
  DumpOutput( "#             Total Solve: %9.1f (s), %9.1f (MB)\n" , Time()-tt , maxMemoryUsage );

  DumpOutput( "Vertices / Polygons: %d / %d\n" , mesh.outOfCorePointCount()+mesh.inCorePoints.size() , mesh.polygonCount() );
  FreePointer( solution );

  cb(90,"Creating Mesh");
  mesh.resetIterator();
  int vm = mesh.outOfCorePointCount()+mesh.inCorePoints.size();

  vcg::tri::Allocator<CMeshO>::AddVertices(pm,vm);


  int i;
  for (i=0; i < int(mesh.inCorePoints.size()); i++){
    pm.vert[i].P()[0] = mesh.inCorePoints[i].point[0];
    pm.vert[i].P()[1] = mesh.inCorePoints[i].point[1];
    pm.vert[i].P()[2] = mesh.inCorePoints[i].point[2];
    pm.vert[i].Q() = mesh.inCorePoints[i].value;
  }
  for (int ii=0; ii < mesh.outOfCorePointCount(); ii++){
    PlyValueVertex< float > p;
    mesh.nextOutOfCorePoint(p);
    pm.vert[ii+i].P()[0] = p.point[0];
    pm.vert[ii+i].P()[1] = p.point[1];
    pm.vert[ii+i].P()[2] = p.point[2];
    pm.vert[ii+i].Q() = p.value;
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
    vcg::tri::Allocator<CMeshO>::AddFace(pm, &pm.vert[indV[0]], &pm.vert[indV[1]], &pm.vert[indV[2]]);
  }
  cb(100,"Done");
  return 1;
}
template <class MeshType>
void PoissonClean(MeshType &m, bool scaleNormal)
{

  if(m.face.size()>0)
    vcg::tri::Clean<MeshType>::RemoveUnreferencedVertex(m);
  vcg::tri::Allocator<MeshType>::CompactEveryVector(m);
  vcg::tri::UpdateNormal<MeshType>::NormalizePerVertex(m);
  if(scaleNormal)
  {
    for(typename MeshType::VertexIterator vi=m.vert.begin();vi!=m.vert.end();++vi)
      vi->N() *= vi->Q();
  }
}

bool FilterScreenedPoissonPlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos* cb)
{
  if (filterName == "Screened Poisson Surface Reconstruction")
  {
    MeshModel *mm =md.mm();
    MeshModel *pm =md.addNewMesh("","Poisson mesh",false);
    md.setVisible(pm->id(),false);

    pm->updateDataMask(MeshModel::MM_VERTQUALITY);
    PoissonParam<Scalarm> pp;

    MeshModelPointStream<Scalarm> meshStream(mm->cm);
    MeshDocumentPointStream<Scalarm> documentStream(md);

    pp.MaxDepthVal = env.evalInt("depth");
    pp.FullDepthVal = env.evalInt("fullDepth");
    pp.CGDepthVal= env.evalInt("cgDepth");
    pp.ScaleVal = env.evalFloat("scale");
    pp.SamplesPerNodeVal = env.evalFloat("samplesPerNode");
    pp.PointWeightVal = env.evalFloat("pointWeight");
    pp.ItersVal = env.evalInt("iters");
    pp.ConfidenceFlag = env.evalBool("confidence");
    pp.NormalWeightsFlag = env.evalBool("nWeights");
    pp.DensityFlag = true;
    if(env.evalBool("visibleLayer"))
    {
      MeshModel *m=0;
      while(m=md.nextVisibleMesh(m))
        PoissonClean(m->cm, (pp.ConfidenceFlag || pp.NormalWeightsFlag));

      Execute<Scalarm>(&documentStream,pm->cm,pp,cb);
    }
    else
    {
      PoissonClean(mm->cm, (pp.ConfidenceFlag || pp.NormalWeightsFlag));
      Execute<Scalarm>(&meshStream,pm->cm,pp,cb);
    }
    pm->UpdateBoxAndNormals();
    md.setVisible(pm->id(),true);

    return true;
  }
  return false;
}





MESHLAB_PLUGIN_NAME_EXPORTER(FilterScreenedPoissonPlugin)
