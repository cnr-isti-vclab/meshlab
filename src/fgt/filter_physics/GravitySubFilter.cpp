#include "GravitySubFilter.h"

int GravitySubFilter::m_filterType = -1;

GravitySubFilter::GravitySubFilter() : m_scenery(0){
    MeshSubFilter::m_currentFilterType += 1;
    m_filterType = MeshSubFilter::m_currentFilterType;
}

void GravitySubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet& par){
    DynamicMeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichMesh("scenery", 0, &md, "Static environment mesh", "This mesh represent the static environment of the simulation"));
}

bool GravitySubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(!DynamicMeshSubFilter::applyFilter(filter, md, par, cb))
        return false;

    int currentStep  = par.getDynamicFloat("timeline") / 100 * m_steps;
    for(int i = 0; i < md.size(); i++) md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];
    return true;
}

bool GravitySubFilter::parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par){
    return DynamicMeshSubFilter::parametersAreNotCorrect(md, par) || md.size() < 2 || par.getMesh("scenery") == 0;
}

bool GravitySubFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = DynamicMeshSubFilter::configurationHasChanged(md, par) || m_scenery != par.getMesh("scenery") || m_currentFilterType != m_filterType;
    m_scenery = par.getMesh("scenery");
    m_currentFilterType = m_filterType;
    return changed;
}

void GravitySubFilter::initialize(MeshDocument& md, RichParameterSet& par, vcg::CallBackPos* cb){
    DynamicMeshSubFilter::initialize(md, par, cb);

    if(cb != 0) (*cb)(0, "Physics pre-renderization of the scene started...");

    float gravity[3] = {0.0f, m_gravity, 0.0f};
    m_engine.clear();
    m_engine.setGlobalForce(gravity);
    m_engine.setIterations(m_iterations);
    m_engine.setMaxContacts(m_contacts);
    m_engine.setBounciness(m_bounciness);
    m_engine.setFriction(m_friction);

    for(int i = 0; i < md.size(); i++)
        m_engine.registerTriMesh(*md.getMesh(i), m_scenery == md.getMesh(i) ? true : false);

    m_layersTrans.clear();
    m_layersTrans.resize(md.size());

    for(unsigned i = 0; i < m_layersTrans.size(); i++){
        m_layersTrans[i].reserve(m_steps);
    }

    for(int i = 0; i <= m_steps; i++){
        if(cb != 0) (*cb)(98.f*i/m_steps, "Computing...");

        for(int j = 0; j < md.size(); j++)
            m_layersTrans[j].push_back(m_engine.getTransformationMatrix(*md.getMesh(j)));

        m_engine.integrate(1.0f / m_fps);
    }

    if(cb != 0) (*cb)(99, "Physics pre-renderization of the scene completed...");
}
