#include "filter_sdf.h"
using namespace std;
using namespace vcg;
// #include <vcg/space/index/spatial_hashing.h>
#include <vcg/space/index/aabb_binary_tree/aabb_binary_tree.h>

SdfPlugin::SdfPlugin() : SingleMeshFilterInterface("Compute SDF"){}
bool SdfPlugin::applyFilter(MeshDocument& md, RichParameterSet&, vcg::CallBackPos*){
  MeshModel* mm = md.mm();
  CMeshO& mesh = mm->cm;
  CalculateSDF sdfFilter( &mesh );

  vector<float> sdf;
  sdfFilter.init(CalculateSDF::FACES, sdf);
//  sdfFilter.compute(sdf);

  QTime midnight(0, 0, 0);
  qsrand(midnight.secsTo(QTime::currentTime()));

  // Transfer SDF to face quality, then clear memory
  mm->updateDataMask(MeshModel::MM_FACEQUALITY);
  for(int i=0; i<mesh.fn; i++){
    qDebug("%d: %f", i, sdf[i]);
    mesh.face[i].Q() = qrand() % 100;
    qDebug() << mesh.face[i].Q();
    //sdf[i];
  }

  // ATTEMPT1: Create spatial intersecting structure
  // vcg::SpatialHashTable<CFaceO> spatialHash;
  // spatialHash.Set( mesh.face.begin(), mesh.face.end(), mesh.bbox );
  // spatialHash.Add( &mesh );
  // spatialHash.DoRay()
  // vcg::tri::DoRay< TriMeshType, FaceSpatialIndexing >( m, _g_mesh, r, m.bbox.Diag(), dist );

  // ATTEMPT2: Create spatial intersecting structure
  vcg::AABBBinaryTreeIndex<CFaceO, float, vcg::EmptyClass> sIndex;
  sIndex.Set(mesh.face.begin(), mesh.face.end());

  //--- Tests for backfaces
  vcg::RayTriangleIntersectionFunctor<true> rayIntersector;
  float rayT, maxDist = std::numeric_limits<float>::max();
  Ray3f ray(Point3f(0,0,0), Point3f(1,0,0));
  vcg::EmptyClass a;
  CFaceO* isectFace = sIndex.DoRay(rayIntersector, a, ray, maxDist, rayT);
  qDebug() << tri::Index(mesh, isectFace);
  // isectFace->Q() = 100;

  mm->glw.ccm = GLW::CMPerFace;

  return true;
}

Q_EXPORT_PLUGIN(SdfPlugin)
