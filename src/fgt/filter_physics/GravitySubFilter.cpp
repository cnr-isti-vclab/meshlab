#include "GravitySubFilter.h"

GravitySubFilter::GravitySubFilter() : m_currentSceneryLayer(-1){
}

void GravitySubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet& par){
    DynamicMeshSubFilter::initParameterSet(action, md, par);

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }
    par.addParam(new RichEnum("layers", 0, layers, "Scenery layer", "Select the layer that will act as the static scenery"));
}

bool GravitySubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = par.getDynamicFloat("timeline") / 100 * m_steps;

    for(int i = 0; i < md.size(); i++)
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

    return true;
}

bool GravitySubFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = DynamicMeshSubFilter::configurationHasChanged(md, par) || m_currentSceneryLayer != par.getEnum("layers");
    m_currentSceneryLayer = par.getEnum("layers");
    return changed;
}

void GravitySubFilter::initialize(MeshDocument& md, RichParameterSet& par){
    DynamicMeshSubFilter::initialize(md, par);

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);

    for(int i = 0; i < md.size(); i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_currentSceneryLayer == i ? true : false);

    m_layersTrans.clear();
    m_layersTrans.resize(md.size());

    for(int i = 0; i < m_layersTrans.size(); i++){
        m_layersTrans[i].reserve(m_steps);
    }

    for(int i = 0; i <= m_steps; i++){
        for(int j = 0; j < md.size(); j++){
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));
        }
        m_engine.integrate(m_stepSize);
    }
}
