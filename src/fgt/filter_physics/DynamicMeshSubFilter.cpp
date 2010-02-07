#include "DynamicMeshSubFilter.h"

#include <vcg/complex/trimesh/append.h>

DynamicMeshSubFilter::DynamicMeshSubFilter() : m_fps(-1), m_iterations(-1), m_contacts(-1), m_steps(-1), m_seconds(-1), m_bounciness(-1), m_gravity(-1) {
}

void DynamicMeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    MeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichFloat("gravity", -1.f, "Gravity", "Gravity force value"));
    par.addParam(new RichInt("seconds", 10, "Simulation interval (sec)", "Physics simulation interval in seconds"));
    par.addParam(new RichDynamicFloat("timeline", 0, 0, 100, "Timeline %", "Controls the timeline of the simulation"));
}

bool DynamicMeshSubFilter::applyFilter(QAction*, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(parametersAreNotCorrect(md, par))
        return false;

    if(configurationHasChanged(md, par))
        initialize(md, par, cb);

    return true;
}

bool DynamicMeshSubFilter::parametersAreNotCorrect(MeshDocument&, RichParameterSet& par){
    return par.getInt("seconds") < 0 || par.getInt("fps") <= 0 || par.getInt("iterations") <= 0 || par.getInt("contacts") <= 0 || par.getFloat("bounciness") < 0 || par.getFloat("bounciness") > 1;
}

bool DynamicMeshSubFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = m_seconds != par.getInt("seconds");
    changed |= m_fps != par.getInt("fps");
    changed |= m_iterations != par.getInt("iterations");
    changed |= m_contacts != par.getInt("contacts");
    changed |= m_bounciness != par.getFloat("bounciness");
    changed |= m_gravity != par.getFloat("gravity");
    changed |= m_friction != par.getFloat("friction");

    if(unsigned(md.size()) == m_files.size())
        for(unsigned i = 0; i < m_files.size(); i++)
            changed |= m_files.at(i) != md.getMesh(i)->fileName;
    else
        changed = true;

    m_files.clear();
    for(int i = 0; i < md.size(); i++)
        m_files.push_back(md.getMesh(i)->fileName);

    m_seconds = par.getInt("seconds");
    m_fps = par.getInt("fps");
    m_iterations = par.getInt("iterations");
    m_contacts = par.getInt("contacts");
    m_bounciness = par.getFloat("bounciness");
    m_gravity = par.getFloat("gravity");
    m_friction = par.getFloat("friction");
    return changed;
}

void DynamicMeshSubFilter::initialize(MeshDocument& md, RichParameterSet& par, vcg::CallBackPos* cb){
    MeshSubFilter::initialize(md, par, cb);
    m_steps = m_seconds * m_fps;
}
