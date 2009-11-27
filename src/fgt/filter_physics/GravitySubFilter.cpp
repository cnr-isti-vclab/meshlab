#include "GravitySubFilter.h"

GravitySubFilter::GravitySubFilter() : m_initialized(false), m_stepSize(0.01), m_steps(0), m_prevStep(0), m_currentSceneryLayer(0){
}

void GravitySubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet& par){
    par.addParam(new RichInt("seconds", 1, "Simulation interval in seconds", "Physics simulation interval in seconds"));
    par.addParam(new RichDynamicFloat("timeline", 0, 0, 100, "Timeline %", "Physics simulation is run"));

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }
    par.addParam(new RichEnum("layers", 0, layers, "Scenery layer", "Select the layer that will act as the static scenery"));

    m_initialized = false;
}

bool GravitySubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    initialize(md, par);
    m_engine.updateTransform(); // The transformation matrix got reseted by meshlab

    int currentStep  = (par.getDynamicFloat("timeline") * m_steps) / 100;
    if(currentStep <= m_prevStep)
        return true;

    unsigned int selectedLayer = par.getEnum("layers");
    if(m_currentSceneryLayer != selectedLayer){
        m_engine.setAsRigidBody(*md.getMesh(m_currentSceneryLayer), true);
        m_engine.setAsRigidBody(*md.getMesh(selectedLayer), false);
        m_currentSceneryLayer = selectedLayer;
    }

    for(int i = 0; i < currentStep - m_prevStep; i++)
        m_engine.integrate(m_stepSize);
    m_engine.updateTransform();

    m_prevStep = currentStep;
    return true;
}

void GravitySubFilter::initialize(MeshDocument& md, RichParameterSet& par){
    if(!m_initialized){
        m_engine.clear();

        m_prevStep = 0;
        m_steps = par.getInt("seconds") / m_stepSize;
        m_currentSceneryLayer = par.getEnum("layers");

        for(unsigned int i = 0; i < md.size(); i++)
            m_engine.registerTriMesh(*md.getMesh(i), m_currentSceneryLayer == i ? true : false);

        static float gravity[3] = {0.0f, -9.8f, 0.0f};
        m_engine.setGlobalForce(gravity);
        m_initialized = true;
    }
}
