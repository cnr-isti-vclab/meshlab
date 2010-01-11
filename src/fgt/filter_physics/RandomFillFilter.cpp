#include "RandomFillFilter.h"

#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/inertia.h>

using namespace std;
using namespace vcg;

void RandomFillFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }

    par.addParam(new RichEnum("containerLayer", 0, layers, "Container object layer", "Select the layer that contains the object that will act as a container"));
    par.addParam(new RichEnum("fillLayer", 0, layers, "Fill object layer", "Select the layer that contains the object that will be used to fill"));
    par.addParam(new RichInt("fillCounter", 1, "Fill counter", "The number of object used to fill the container"));
}

bool RandomFillFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(md.size() < 2) return true;

    int containerLayer = par.getEnum("containerLayer");
    int fillLayer = par.getEnum("fillLayer");
    int fillOffset = md.size();

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);
    m_engine.registerTriMesh(*md.getMesh(containerLayer), true);

    tri::Inertia<CMeshO> inertia;
    inertia.Compute(md.getMesh(containerLayer)->cm);

    for(int i = 0; i < par.getInt("fillCounter"); i++){
        addRandomObject(md, fillLayer, inertia.CenterOfMass());
        m_engine.registerTriMesh(*md.getMesh(fillOffset++));
        for(int j = 0; j < m_stepsPerSecond; j++){
            m_engine.integrate(m_stepSize);
        }
    }
    m_engine.updateTransform();

    return true;
}

void RandomFillFilter::addRandomObject(MeshDocument& md, int fillLayer, const vcg::Point3<float>& origin){
    MeshModel* meshCopy = md.addNewMesh("fillMesh");
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, md.getMesh(fillLayer)->cm, false, true);
    meshCopy->cm.Tr = md.getMesh(fillLayer)->cm.Tr;
    meshCopy->cm.Tr.SetColumn(3, origin);
}
