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
    par.addParam(new RichEnum("randomLayer", 0, layers, "Random object layer", "Select the layer that contains the object that will spawn randomly"));

    par.addParam(new RichFloat("distance", 1.0f, "Random radius interval", "The object will spawn in a random position contained in the specified radius"));
    par.addParam(new RichFloat("dropRate", 0.005f, "Drop rate", "The drop rate"));
}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    MeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = (par.getDynamicFloat("timeline") * m_steps) / 100;

    for(int i = 0; i < md.size(); i++)
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

    return true;
}

void RandomDropFilter::initialize(MeshDocument& md, RichParameterSet& par){
    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);

    srand((unsigned)time(0));

    for(unsigned int i = 0; i < m_steps*m_seconds*m_dropRate; i++)
        addRandomObject(md);

    for(unsigned int i = 0; i < md.size(); i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_randomLayer == i ? true : false);

    m_layersTrans.clear();
    for(unsigned int i = 0; i < md.size(); i++)
        m_layersTrans.push_back(LayerTransformations());

    m_steps = m_seconds / m_stepSize;
    for(int i = 0; i <= m_steps; i++){
        for(unsigned int j = 0; j < md.size(); j++){
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));
        }
        m_engine.integrate(m_stepSize);
    }
}

void RandomDropFilter::addRandomObject(MeshDocument& md){
    float x = (float) rand()/RAND_MAX * m_distance;
    float y = (float) rand()/RAND_MAX * m_distance;
    float z = (float) rand()/RAND_MAX * m_distance;

    /*MeshModel* meshCopy = md.addNewMesh("rndMesh");
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(md.getMesh(m_randomLayer)->cm, meshCopy->cm, false, true);*/
}

bool RandomDropFilter::configurationHasChanged(RichParameterSet& par){
    bool changed = MeshSubFilter::configurationHasChanged(par) ||
                   m_randomLayer != par.getEnum("randomLayer") ||
                   m_distance != par.getFloat("distance") ||
                   m_dropRate != par.getFloat("dropRate");

    m_randomLayer = par.getEnum("randomLayer");
    m_distance = par.getFloat("distance");
    m_dropRate = par.getFloat("dropRate");

    return changed;
}
