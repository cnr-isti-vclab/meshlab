#include "GravitySubFilter.h"

GravitySubFilter::GravitySubFilter() : m_initialized(false), m_prevTimeSlice(0), m_currentSceneryLayer(0){
}

void GravitySubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet& par){
    par.addParam(new RichDynamicFloat("timeline", 1, 1, 100, "Timeline", "Physics simulation is run"));

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }
    par.addParam(new RichEnum("layers", 0, layers, "Scenery layer", "Select the layer that will act as the static scenery"));

    m_initialized = false;
}

bool GravitySubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    initialize(md, par);

    unsigned int currentTimeSlice  = par.getDynamicFloat("timeline");

    if(currentTimeSlice <= m_prevTimeSlice)
        return true;

    unsigned int selectedLayer = par.getEnum("layers");
    if(m_currentSceneryLayer != selectedLayer){
        m_engine.setAsRigidBody(*md.getMesh(m_currentSceneryLayer), true);
        m_engine.setAsRigidBody(*md.getMesh(selectedLayer), false);
        m_currentSceneryLayer = selectedLayer;
    }

    for(unsigned int i = 0; i < currentTimeSlice - m_prevTimeSlice; i++)
        m_engine.integrate(0.01);

    m_prevTimeSlice = currentTimeSlice;
    return true;
}

void GravitySubFilter::initialize(MeshDocument& md, RichParameterSet& par){
    if(!m_initialized){
        m_engine.clear();

        m_prevTimeSlice = 0.f;
        m_currentSceneryLayer = par.getEnum("layers");

        for(unsigned int i = 0; i < md.size(); i++)
            m_engine.registerTriMesh(*md.getMesh(i), m_currentSceneryLayer == i ? true : false);

        static float gravity[3] = {0.0f, -9.8f, 0.0f};
        m_engine.setGlobalForce(gravity);
        m_initialized = true;
    }
}
