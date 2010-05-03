#include "filter_sdf.h"
using namespace std;
using namespace vcg;

SdfPlugin::SdfPlugin() : SingleMeshFilterInterface("Compute SDF"){}
bool SdfPlugin::applyFilter(MeshDocument& md, RichParameterSet&, vcg::CallBackPos* cb){
  MeshModel* mm = md.mm();
  CMeshO& mesh = mm->cm;
  CalculateSDF sdfFilter( &mesh );

  Log("Initializing spatial hashing for fast intersect tests");
  vector<float> sdf;
  sdfFilter.init(CalculateSDF::FACES, sdf);
  Log("Tracing rays in volume");
  sdfFilter.compute(sdf, cb);

  // Transfer SDF to face quality
  mm->updateDataMask(MeshModel::MM_FACEQUALITY);
  for(int i=0; i<mesh.fn; i++)
    mesh.face[i].Q() = sdf[i];
  return true;
}

Q_EXPORT_PLUGIN(SdfPlugin)
