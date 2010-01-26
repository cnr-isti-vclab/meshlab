#include "RandomDropFilter.h"

#include <vcg/complex/trimesh/append.h>

#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

RandomDropFilter::RandomDropFilter() : m_randomMesh(0), m_dropRate(-1), m_distance(-1){
}

void RandomDropFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    DynamicMeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichMesh("randomMesh", 0, &md, "Random mesh", "This mesh will be randomly spawned in the scene"));
    par.addParam(new RichFloat("distance", 1.0f, "Random spawn radius", "The object will spawn in a random position contained in the specified radius"));
    par.addParam(new RichInt("dropRate", 2, "Drop rate (sec)", "The drop rate of the filler mesh in seconds"));
}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(md.size() < 2 || par.getMesh("randomMesh") == 0 || par.getInt("dropRate") < 0)
        return false;

    if(par.getInt("dropRate") == 0)
        return true;

    DynamicMeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = par.getDynamicFloat("timeline") / 100 * m_steps;
    int randomObjects = m_seconds/m_dropRate;
    int currentRandomObject = md.size() - randomObjects + currentStep/(m_stepsPerSecond*m_dropRate);

    for(int i = 0; i < md.size(); i++){
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

        if(i < currentRandomObject)
            md.getMesh(i)->visible = true;
        else
            md.getMesh(i)->visible = false;
    }

    return true;
}

void RandomDropFilter::initialize(MeshDocument& md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::initialize(md, par, cb);

    if(cb != 0) (*cb)(0, "Physics pre-renderization of the scene started...");

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);

    srand((unsigned)time(0));
    int randomObjects = m_seconds/m_dropRate;
    m_steps = m_seconds/m_stepSize;

    foreach(MeshModel *mesh, md.meshList){
        if(mesh->fileName.find("randomMesh") == 0)
            md.delMesh(mesh);
    }

    for(int i = 0; i < randomObjects; i++)
        addRandomObject(md, i);

    for(int i = 0; i < md.size() - randomObjects; i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_randomMesh == md.getMesh(i) ? false : true);

    m_layersTrans.clear();
    m_layersTrans.resize(md.size());

    for(int i = 0; i < m_layersTrans.size(); i++){
        m_layersTrans[i].reserve(m_steps);
    }

    for(int i = 0; i <= m_steps; i++){
        if(cb != 0) (*cb)(98.f*i/m_steps, "Computing...");

        int currentRndObject = md.size() - randomObjects + i/(m_stepsPerSecond*m_dropRate);

        if(i != 0 && i % (m_stepsPerSecond*m_dropRate) == 0)
            m_engine.registerTriMesh(*md.getMesh(currentRndObject - 1), false);

        for(int j = 0; j < currentRndObject; j++){
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));
        }

        for(int j = currentRndObject; j < md.size(); j++){
            m_layersTrans[j].push_back(vcg::Matrix44<float>::Identity());
        }

        m_engine.integrate(m_stepSize);
    }

    if(cb != 0) (*cb)(99, "Physics pre-renderization of the scene completed...");
}

void RandomDropFilter::addRandomObject(MeshDocument& md, int meshID){
    ostringstream meshName;
    meshName << "randomMesh" << meshID;
    MeshModel* meshCopy = md.addNewMesh(meshName.str().c_str());
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, m_randomMesh->cm, false, true);
    meshCopy->cm.Tr = m_randomMesh->cm.Tr;

    float x = meshCopy->cm.Tr.GetColumn3(3).X() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    float y = meshCopy->cm.Tr.GetColumn3(3).Y() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    float z = meshCopy->cm.Tr.GetColumn3(3).Z() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;

    meshCopy->cm.Tr.SetRotateDeg(rand(), vcg::Point3f(x, y, z));
    meshCopy->cm.Tr.SetColumn(3, vcg::Point3f(x, y, z));
    meshCopy->visible = false;
}

bool RandomDropFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = DynamicMeshSubFilter::configurationHasChanged(md, par) ||
                   m_randomMesh != par.getMesh("randomMesh") ||
                   m_distance != par.getFloat("distance") ||
                   m_dropRate != par.getInt("dropRate");

    m_randomMesh = par.getMesh("randomMesh");
    m_distance = par.getFloat("distance");
    m_dropRate = par.getInt("dropRate");

    return changed;
}
