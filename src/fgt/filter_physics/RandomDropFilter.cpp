#include "RandomDropFilter.h"

#include <vcg/complex/trimesh/append.h>

#include <cstdlib>
#include <ctime>

using namespace std;

RandomDropFilter::RandomDropFilter() : m_randomLayer(-1), m_distance(-1), m_dropRate(-1){
}

void RandomDropFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    MeshSubFilter::initParameterSet(action, md, par);

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }
    par.addParam(new RichEnum("randomLayer", 1, layers, "Random object layer", "Select the layer that contains the object that will spawn randomly"));

    par.addParam(new RichFloat("distance", 1.0f, "Random radius interval", "The object will spawn in a random position contained in the specified radius"));
    par.addParam(new RichInt("dropRate", 2, "Drop rate", "The drop rate"));
}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    MeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = (par.getDynamicFloat("timeline") * m_steps) / 100;
    unsigned int rndObjects = m_seconds/m_dropRate;
    unsigned int currentRndObject = md.size() - rndObjects + currentStep/(100*m_dropRate);

    for(int i = 0; i < md.size(); i++){
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

        if(i < currentRndObject)
            md.getMesh(i)->visible = true;
        else
            md.getMesh(i)->visible = false;
    }

    return true;
}

void RandomDropFilter::initialize(MeshDocument& md, RichParameterSet& par){
    MeshSubFilter::initialize(md, par);

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);

    srand((unsigned)time(0));

    unsigned int rndObjects = m_seconds/m_dropRate;
    for(unsigned int i = 0; i < rndObjects; i++)
        addRandomObject(md);

    for(unsigned int i = 0; i < md.size() - rndObjects; i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_randomLayer == i ? false : true);

    m_layersTrans.clear();
    for(unsigned int i = 0; i < md.size(); i++)
        m_layersTrans.push_back(LayerTransformations());

    m_steps = m_seconds/m_stepSize;
    for(int i = 0; i <= m_steps; i++){
        int currentRndObject = md.size() - rndObjects + i/(100*m_dropRate);

        if(i != 0 && i % (100*m_dropRate) == 0)
            m_engine.registerTriMesh(*md.getMesh(currentRndObject - 1), false);

        for(unsigned int j = 0; j < currentRndObject; j++){
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));
        }

        for(unsigned int j = currentRndObject; j < md.size(); j++){
            m_layersTrans[j].push_back(vcg::Matrix44<float>::Identity());
        }

        m_engine.integrate(m_stepSize);
    }
}

void RandomDropFilter::addRandomObject(MeshDocument& md){
    MeshModel* meshCopy = md.addNewMesh("rndMesh");
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, md.getMesh(m_randomLayer)->cm, false, true);
    meshCopy->cm.Tr = md.getMesh(m_randomLayer)->cm.Tr;

    float x = (float) meshCopy->cm.Tr.GetColumn3(3).X() + m_distance/2.0f - rand()/(RAND_MAX*m_distance);
    float y = (float) meshCopy->cm.Tr.GetColumn3(3).Y() + m_distance/2.0f - rand()/(RAND_MAX*m_distance);
    float z = (float) meshCopy->cm.Tr.GetColumn3(3).Z() + m_distance/2.0f - rand()/(RAND_MAX*m_distance);

    meshCopy->cm.Tr.SetRotateRad(rand(), vcg::Point3<float>(x, y, z));
    meshCopy->cm.Tr.SetTranslate(x, y, z);
    meshCopy->visible = false;
}

bool RandomDropFilter::configurationHasChanged(RichParameterSet& par){
    bool changed = MeshSubFilter::configurationHasChanged(par) ||
                   m_randomLayer != par.getEnum("randomLayer") ||
                   m_distance != par.getFloat("distance") ||
                   m_dropRate != par.getInt("dropRate");

    m_randomLayer = par.getEnum("randomLayer");
    m_distance = par.getFloat("distance");
    m_dropRate = par.getInt("dropRate");

    return changed;
}
