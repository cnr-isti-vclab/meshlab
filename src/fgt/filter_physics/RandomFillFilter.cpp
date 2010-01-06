#include "RandomFillFilter.h"

#include <vcg/complex/trimesh/append.h>

using namespace std;

void RandomFillFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }

    par.addParam(new RichInt("seconds", 1, "Simulation interval in seconds", "Physics simulation interval in seconds"));
    par.addParam(new RichEnum("randomLayer", 0, layers, "Random object layer", "Select the layer that contains the object that will spawn randomly"));
    par.addParam(new RichInt("dropRate", 2, "Drop rate", "The drop rate"));
}

bool RandomFillFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    return true;
}

void RandomFillFilter::addRandomObject(MeshDocument& md){
    MeshModel* meshCopy = md.addNewMesh("randomMesh");
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, md.getMesh(m_randomLayer)->cm, false, true);
    meshCopy->cm.Tr = md.getMesh(m_randomLayer)->cm.Tr;
    meshCopy->visible = false;
}
