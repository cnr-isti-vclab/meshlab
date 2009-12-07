#include "GravitySubFilter.h"

GravitySubFilter::GravitySubFilter() : m_currentSceneryLayer(-1){
}

void GravitySubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet& par){
    MeshSubFilter::initParameterSet(action, md, par);

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }
    par.addParam(new RichEnum("layers", 0, layers, "Scenery layer", "Select the layer that will act as the static scenery"));
}

bool GravitySubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    MeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = (par.getDynamicFloat("timeline") * m_steps) / 100;

    for(int i = 0; i < md.size(); i++)
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

    return true;
}

bool GravitySubFilter::configurationHasChanged(RichParameterSet& par){
    bool changed = MeshSubFilter::configurationHasChanged(par) || m_currentSceneryLayer != par.getEnum("layers");
    m_currentSceneryLayer = par.getEnum("layers");
    return changed;
}

void GravitySubFilter::initialize(MeshDocument& md, RichParameterSet& par){
    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);

    for(unsigned int i = 0; i < md.size(); i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_currentSceneryLayer == i ? true : false);

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
