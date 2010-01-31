#include "RandomDropFilter.h"

#include <vcg/complex/trimesh/append.h>

#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

int RandomDropFilter::m_filterType = -1;

RandomDropFilter::RandomDropFilter() : m_randomMesh(0), m_dropRate(-1), m_distance(-1), m_bboxUnit(false){
    MeshSubFilter::m_currentFilterType += 1;
    m_filterType = MeshSubFilter::m_currentFilterType;
}

void RandomDropFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    DynamicMeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichMesh("randomMesh", 0, &md, "Random mesh", "This mesh will be randomly spawned in the scene"));
    par.addParam(new RichFloat("distance", 1.0f, "Random spawn radius", "The object will spawn in a random position contained in the specified radius"));
    par.addParam(new RichBool("bboxUnit", true, "Use bounding box units", "If true the random spawn radius will be computed in bounding box units"));
    par.addParam(new RichFloat("dropRate", 0.5, "Drop rate (sec)", "The drop rate of the filler mesh in seconds"));
}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(md.size() < 2 || par.getMesh("randomMesh") == 0 || par.getMesh("randomMesh")->fileName.find("randomDropMesh") == 0 || par.getFloat("dropRate") < 0)
        return false;

    if(par.getFloat("dropRate") == 0)
        return true;

    DynamicMeshSubFilter::applyFilter(filter, md, par, cb);

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

void RandomDropFilter::initialize(MeshDocument& md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::initialize(md, par, cb);

    if(cb != 0) (*cb)(0, "Physics pre-renderization of the scene started...");

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear(md);
    m_engine.setGlobalForce(gravity);
    m_engine.setIterations(m_iterations);
    m_engine.setMaxContacts(m_contacts);
    m_engine.setBounciness(m_bounciness);

    srand((unsigned)time(0));
    int randomObjects = m_seconds*m_dropRate;
    m_steps = m_seconds * m_fps;

    foreach(MeshModel *mesh, md.meshList){
        if(mesh->fileName.find("randomDropMesh") == 0)
            md.delMesh(mesh);
    }

    for(int i = 0; i < randomObjects; i++)
        addRandomObject(md, i);

    for(int i = 0; i < md.size() - randomObjects; i++){
        if(m_randomMesh != md.getMesh(i))
            m_engine.registerTriMesh(*md.getMesh(i), true);
    }

    m_layersTrans.clear();
    m_layersTrans.resize(md.size());

    for(int i = 0; i < m_layersTrans.size(); i++){
        m_layersTrans[i].reserve(m_steps);
    }

    for(int i = 0; i <= m_steps; i++){
        if(cb != 0) (*cb)(98.f*i/m_steps, "Computing...");

        int currentRndObject = md.size() - randomObjects + i/(m_fps/m_dropRate);

        if(i != 0 && i % int(m_fps/m_dropRate) == 0)
            m_engine.registerTriMesh(*md.getMesh(currentRndObject - 1), false);

        for(int j = 0; j < md.size(); j++){
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));
        }

        m_engine.integrate(1.0f / m_fps);
    }

    m_files.clear();
    for(int i = 0; i < md.size(); i++)
        m_files.push_back(md.getMesh(i)->fileName);

    if(cb != 0) (*cb)(99, "Physics pre-renderization of the scene completed...");
}

void RandomDropFilter::addRandomObject(MeshDocument& md, int meshID){
    float multiplier = m_bboxUnit ? m_randomMesh->cm.bbox.Diag() : 1.0f;
    float distance = m_distance*multiplier;

    ostringstream meshName;
    meshName << "randomDropMesh" << meshID;
    MeshModel* meshCopy = md.addNewMesh(meshName.str().c_str());
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, m_randomMesh->cm, false, true);
    meshCopy->cm.Tr = m_randomMesh->cm.Tr;

    float x = meshCopy->cm.Tr.GetColumn3(3).X() + distance/2.0f - static_cast<float>(rand())/RAND_MAX*distance;
    float y = meshCopy->cm.Tr.GetColumn3(3).Y() + distance/2.0f - static_cast<float>(rand())/RAND_MAX*distance;
    float z = meshCopy->cm.Tr.GetColumn3(3).Z() + distance/2.0f - static_cast<float>(rand())/RAND_MAX*distance;
    
    meshCopy->cm.Tr.SetColumn(3, vcg::Point3f(x, y, z));
    meshCopy->visible = false;
}

bool RandomDropFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = DynamicMeshSubFilter::configurationHasChanged(md, par) ||
                   m_randomMesh != par.getMesh("randomMesh") ||
                   m_distance != par.getFloat("distance") ||
                   m_dropRate != par.getFloat("dropRate") ||
                   m_bboxUnit != par.getBool("bboxUnit") ||
                   m_currentFilterType != m_filterType;

    m_randomMesh = par.getMesh("randomMesh");
    m_distance = par.getFloat("distance");
    m_dropRate = par.getFloat("dropRate");
    m_bboxUnit = par.getBool("bboxUnit");
    m_currentFilterType = m_filterType;

    return changed;
}
