#include "filter_sdf.h"
using namespace std;
using namespace vcg;

SdfPlugin::SdfPlugin() : SingleMeshFilterInterface("Compute SDF"){}
bool SdfPlugin::applyFilter(MeshDocument& md, RichParameterSet&, vcg::CallBackPos*){
    CalculateSDF sdfFilter( &md.mm()->cm );
    sdfFilter.init( CalculateSDF::FACES );
    vector<float>* sdf = sdfFilter.compute();

    // Transfer SDF to face quality, then clear memory
    md.mm()->updateDataMask(MeshModel::MM_FACEQUALITY);
    for(int i=0; i<md.mm()->cm.fn; i++)
        md.mm()->cm.face[i].Q() = (*sdf)[i];
    delete sdf;

    return true;
}

Q_EXPORT_PLUGIN(SdfPlugin)
