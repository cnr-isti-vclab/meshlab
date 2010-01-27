#include "RandomFillFilter.h"

#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/inertia.h>

#include <sstream>

using namespace std;
using namespace vcg;

int RandomFillFilter::m_filterType = -1;

RandomFillFilter::RandomFillFilter(){
    MeshSubFilter::m_currentFilterType += 1;
    m_filterType = MeshSubFilter::m_currentFilterType;
}

void RandomFillFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    par.addParam(new RichMesh("container", 0, &md, "Container mesh", "This mesh will act as a container for the filling mesh"));
    par.addParam(new RichMesh("filler", 0, &md, "Filler mesh", "The container mesh will be filled with this mesh"));
    par.addParam(new RichInt("fillCounter", 1, "Fill counter", "The number of object used to fill the container"));
}

bool RandomFillFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(md.size() < 2 || par.getMesh("container") == 0 || par.getMesh("container") == par.getMesh("filler") || par.getInt("fillCounter") < 0)
        return false;

    if(cb != 0) (*cb)(0, "Physics renderization of the scene started...");

    MeshModel* container = par.getMesh("container");
    MeshModel* filler = par.getMesh("filler");
    int fillOffset = md.size();

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);
    m_engine.registerTriMesh(*container, true);

    tri::Inertia<CMeshO> inertia;
    inertia.Compute(container->cm);

    for(int i = 0; i < par.getInt("fillCounter"); i++){
        if(cb != 0) (*cb)(98.f*i/par.getInt("fillCounter"), "Computing...");

        addRandomObject(md, filler, inertia.CenterOfMass(), i);
        m_engine.registerTriMesh(*md.getMesh(fillOffset++));
        for(int j = 0; j < m_stepsPerSecond; j++){
            m_engine.integrate(m_stepSize);
        }
    }
    m_engine.updateTransform();

    if(cb != 0) (*cb)(0, "Physics renderization of the scene completed...");

    return true;
}

void RandomFillFilter::addRandomObject(MeshDocument& md, MeshModel* filler, const vcg::Point3<float>& origin, int meshID){
    ostringstream meshName;
    meshName << "randomMesh" << meshID;
    MeshModel* meshCopy = md.addNewMesh(meshName.str().c_str());
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, filler->cm, false, true);
    meshCopy->cm.Tr = filler->cm.Tr;
    meshCopy->cm.Tr.SetColumn(3, origin);
}
