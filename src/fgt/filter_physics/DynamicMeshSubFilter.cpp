#include "DynamicMeshSubFilter.h"

DynamicMeshSubFilter::DynamicMeshSubFilter() : m_steps(-1), m_seconds(-1), m_stepSize(0.01), m_stepsPerSecond(1.0f/m_stepSize) {
}

void DynamicMeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    par.addParam(new RichInt("seconds", 1, "Simulation interval in seconds", "Physics simulation interval in seconds"));
    par.addParam(new RichDynamicFloat("timeline", 0, 0, 100, "Timeline %", "Physics simulation is run"));
}

bool DynamicMeshSubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(configurationHasChanged(md, par))
        initialize(md, par);

    return true;
}

bool DynamicMeshSubFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = m_seconds != par.getInt("seconds");

    if(md.size() == m_files.size())
        for(int i = 0; i < m_files.size(); i++)
            changed |= m_files.at(i) != md.getMesh(i)->fileName;
    else
        changed = true;

    m_files.clear();
    for(int i = 0; i < md.size(); i++)
        m_files.push_back(md.getMesh(i)->fileName);

    m_seconds = par.getInt("seconds");
    return changed;
}

void DynamicMeshSubFilter::initialize(MeshDocument&, RichParameterSet&){
    m_steps = m_seconds / m_stepSize;
}
