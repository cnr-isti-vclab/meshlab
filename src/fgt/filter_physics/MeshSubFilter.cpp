#include "MeshSubFilter.h"

MeshSubFilter::MeshSubFilter() : m_stepSize(0.01), m_steps(-1), m_seconds(-1){
}

void MeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    par.addParam(new RichInt("seconds", 1, "Simulation interval in seconds", "Physics simulation interval in seconds"));
    par.addParam(new RichDynamicFloat("timeline", 0, 0, 100, "Timeline %", "Physics simulation is run"));
}

bool MeshSubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(configurationHasChanged(par))
        initialize(md, par);

    return true;
}

bool MeshSubFilter::configurationHasChanged(RichParameterSet& par){
    bool changed = m_seconds != par.getInt("seconds");
    m_seconds = par.getInt("seconds");
    return changed;
}
