#include "DynamicMeshSubFilter.h"

DynamicMeshSubFilter::DynamicMeshSubFilter() : m_steps(-1), m_seconds(-1) {
}

void DynamicMeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    par.addParam(new RichInt("seconds", 1, "Simulation interval (sec)", "Physics simulation interval in seconds"));
    par.addParam(new RichDynamicFloat("timeline", 0, 0, 100, "Timeline %", "Physics simulation is run"));
}

bool DynamicMeshSubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(par.getInt("seconds") < 0)
        return false;

    if(configurationHasChanged(md, par))
        initialize(md, par, cb);

    return true;
}

bool DynamicMeshSubFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = m_seconds != par.getInt("seconds");
    m_seconds = par.getInt("seconds");
    return changed;
}

void DynamicMeshSubFilter::initialize(MeshDocument&, RichParameterSet&, vcg::CallBackPos* cb){
    m_steps = m_seconds / m_stepSize;
}
