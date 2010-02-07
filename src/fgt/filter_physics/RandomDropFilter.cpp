#include "RandomDropFilter.h"

#include <vcg/complex/trimesh/append.h>

#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

int RandomDropFilter::m_filterType = -1;

RandomDropFilter::RandomDropFilter() : m_randomMesh(0), m_dropRate(-1), m_distance(-1){
    MeshSubFilter::m_currentFilterType += 1;
    m_filterType = MeshSubFilter::m_currentFilterType;
}

void RandomDropFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    DynamicMeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichMesh("randomMesh", 0, &md, "Random mesh", "This mesh will be randomly spawned in the scene"));
    par.addParam(new RichFloat("dropRate", 0.5, "Drop rate (sec)", "The drop rate of the filler mesh in seconds"));
    par.addParam(new RichAbsPerc("distance",md.bbox().Diag()*0.1,0,md.bbox().Diag(),"Random spawn radius", "The object will spawn in a random position contained in the specified radius"));

}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::applyFilter(filter, md, par, cb);

    if(par.getFloat("dropRate") == 0)
        return true;

    int currentStep  = par.getDynamicFloat("timeline") / 100 * m_steps;
    int randomObjects = m_seconds*m_dropRate;
    int currentRandomObject = md.size() - randomObjects + currentStep/(m_fps/m_dropRate);

    for(int i = 0; i < md.size(); i++){
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

        if(i < currentRandomObject)
            md.getMesh(i)->visible = true;
        else
            md.getMesh(i)->visible = false;
    }

    return true;
}

bool RandomDropFilter::parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par){
    return DynamicMeshSubFilter::parametersAreNotCorrect(md, par) || md.size() < 2 || par.getMesh("randomMesh") == 0 || par.getMesh("randomMesh")->fileName.find("randomDropMesh") == 0 || par.getFloat("dropRate") < 0;
}

void RandomDropFilter::initialize(MeshDocument& md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::initialize(md, par, cb);

    if(cb != 0) (*cb)(0, "Physics pre-renderization of the scene started...");

    float gravity[3] = {0.0f, m_gravity, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);
    m_engine.setIterations(m_iterations);
    m_engine.setMaxContacts(m_contacts);
    m_engine.setBounciness(m_bounciness);
    m_engine.setFriction(m_friction);

    srand((unsigned)time(0));
    int randomObjects = m_seconds*m_dropRate;
    int spawnInterval = m_fps / m_dropRate;
    m_steps = m_seconds * m_fps;

    foreach(MeshModel *mesh, md.meshList){
        if(mesh->fileName.find("randomDropMesh") == 0)
            md.delMesh(mesh);
    }

    int meshes = md.size();
    for(int i = 0; i < md.size(); i++){
        if(m_randomMesh != md.getMesh(i))
            m_engine.registerTriMesh(*md.getMesh(i), true);
    }

    for(int i = 0; i < randomObjects; i++)
        addRandomObject(md, i);

    m_layersTrans.clear();
    m_layersTrans.resize(md.size());

    for(unsigned i = 0; i < m_layersTrans.size(); i++){
        m_layersTrans[i].reserve(m_steps);
    }

    for(int i = 0; i <= m_steps; i++){
        if(cb != 0) (*cb)(98.f*i/m_steps, "Computing...");

        if(i != 0 && i % spawnInterval == 0)
            m_engine.registerTriMesh(*md.getMesh(meshes++), false);

        for(int j = 0; j < md.size(); j++)
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));

        m_engine.integrate(1.0f / m_fps);
    }

    m_files.clear();
    for(int i = 0; i < md.size(); i++)
        m_files.push_back(md.getMesh(i)->fileName);

    if(cb != 0) (*cb)(99, "Physics pre-renderization of the scene completed...");
}

void RandomDropFilter::addRandomObject(MeshDocument& md, int meshID){
    ostringstream meshName;
    meshName << "randomDropMesh" << meshID;
    MeshModel* meshCopy = md.addNewMesh(meshName.str().c_str());
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, m_randomMesh->cm, false, true);
    meshCopy->cm.Tr = m_randomMesh->cm.Tr;

    float x = meshCopy->cm.Tr.GetColumn3(3).X() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    float y = meshCopy->cm.Tr.GetColumn3(3).Y() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    float z = meshCopy->cm.Tr.GetColumn3(3).Z() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    
    meshCopy->cm.Tr.SetColumn(3, vcg::Point3f(x, y, z));
    meshCopy->visible = false;
}

bool RandomDropFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = DynamicMeshSubFilter::configurationHasChanged(md, par) ||
                   m_randomMesh != par.getMesh("randomMesh") ||
                   m_distance != par.getAbsPerc("distance") ||
                   m_dropRate != par.getFloat("dropRate") ||
                   m_currentFilterType != m_filterType;

    m_randomMesh = par.getMesh("randomMesh");
    m_distance = par.getAbsPerc("distance");
    m_dropRate = par.getFloat("dropRate");
    m_currentFilterType = m_filterType;

    return changed;
}
