#include "RandomDropFilter.h"

#include <vcg/complex/trimesh/append.h>

#include <cstdlib>
#include <ctime>

using namespace std;

RandomDropFilter::RandomDropFilter() : m_randomLayer(-1), m_dropRate(-1), m_distance(-1){
}

void RandomDropFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    DynamicMeshSubFilter::initParameterSet(action, md, par);

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }

    par.addParam(new RichEnum("randomLayer", 0, layers, "Random object layer", "Select the layer that contains the object that will spawn randomly"));
    par.addParam(new RichFloat("distance", 1.0f, "Random radius interval", "The object will spawn in a random position contained in the specified radius"));
    par.addParam(new RichInt("dropRate", 2, "Drop rate", "The drop rate"));
}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = (par.getDynamicFloat("timeline") * m_steps) / m_stepsPerSecond;
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

void RandomDropFilter::initialize(MeshDocument& md, RichParameterSet& par){
    DynamicMeshSubFilter::initialize(md, par);

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
        addRandomObject(md);

    for(int i = 0; i < md.size() - randomObjects; i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_randomLayer == i ? false : true);

    m_layersTrans.clear();
    m_layersTrans.resize(md.size());

    for(int i = 0; i < m_layersTrans.size(); i++){
        m_layersTrans[i].reserve(m_steps);
    }

    for(int i = 0; i <= m_steps; i++){
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
}

void RandomDropFilter::addRandomObject(MeshDocument& md){
    MeshModel* meshCopy = md.addNewMesh("randomMesh");
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, md.getMesh(m_randomLayer)->cm, false, true);
    meshCopy->cm.Tr = md.getMesh(m_randomLayer)->cm.Tr;

    float x = meshCopy->cm.Tr.GetColumn3(3).X() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    float y = meshCopy->cm.Tr.GetColumn3(3).Y() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;
    float z = meshCopy->cm.Tr.GetColumn3(3).Z() + m_distance/2.0f - static_cast<float>(rand())/RAND_MAX*m_distance;

    meshCopy->cm.Tr.SetRotateDeg(rand(), vcg::Point3f(x, y, z));
    meshCopy->cm.Tr.SetColumn(3, vcg::Point3f(x, y, z));
    meshCopy->visible = false;
}

bool RandomDropFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = DynamicMeshSubFilter::configurationHasChanged(md, par) ||
                   m_randomLayer != par.getEnum("randomLayer") ||
                   m_distance != par.getFloat("distance") ||
                   m_dropRate != par.getInt("dropRate");

    m_randomLayer = par.getEnum("randomLayer");
    m_distance = par.getFloat("distance");
    m_dropRate = par.getInt("dropRate");

    return changed;
}
