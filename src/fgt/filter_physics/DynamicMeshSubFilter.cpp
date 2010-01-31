#include "DynamicMeshSubFilter.h"

#include <vcg/complex/trimesh/append.h>

DynamicMeshSubFilter::DynamicMeshSubFilter() : m_fps(-1), m_iterations(-1), m_contacts(-1), m_steps(-1), m_seconds(-1), m_bounciness(-1) {
}

void DynamicMeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    MeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichInt("seconds", 10, "Simulation interval (sec)", "Physics simulation interval in seconds"));
    par.addParam(new RichDynamicFloat("timeline", 0, 0, 100, "Timeline %", "Controls the point in timeline of the simulation"));
}

bool DynamicMeshSubFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(par.getInt("seconds") < 0 || par.getInt("fps") <= 0 || par.getInt("iterations") <= 0 || par.getInt("contacts") <= 0 || par.getFloat("bounciness") < 0.f || par.getFloat("bounciness") > 1.f)
        return false;

    if(configurationHasChanged(md, par))
        initialize(md, par, cb);

    return true;
}

bool DynamicMeshSubFilter::configurationHasChanged(MeshDocument& md, RichParameterSet& par){
    bool changed = m_seconds != par.getInt("seconds");
    changed |= m_fps != par.getInt("fps");
    changed |= m_iterations != par.getInt("iterations");
    changed |= m_contacts != par.getInt("contacts");
    changed |= m_bounciness != par.getFloat("bounciness");

    // Does not work because meshlab fails at restoring the original translation matrix in the preview checkbox logic
    /* Dim: the transformation matrices should not change
    for(int i = 0; i < md.size(); i++){
        for(int j = 0; j < 16; j++) std::cout << md.getMesh(i)->cm.Tr[j/4][j%4] <<" ";
        std::cout<<std::endl;
    }
    */

    /*if(md.size() == m_state.size()){
        for(int i = 0; i < m_state.size() && !changed; i++){
            changed |= !compareMesh(md.getMesh(i), m_state[i]);
        }
    }else
        changed = true;

    saveMeshState(md);*/

    // Old correctness testing
    if(md.size() == m_files.size())
        for(int i = 0; i < m_files.size(); i++)
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
    return changed;
}

bool DynamicMeshSubFilter::compareMesh(MeshModel* m1, MeshModel* m2){
    return m1->fileName == m2->fileName && m1->cm.Tr == m2->cm.Tr;
}

void DynamicMeshSubFilter::saveMeshState(MeshDocument& md){
    m_state.clear();
    for(int i = 0; i < md.size(); i++){
        MeshModel* meshCopy = new MeshModel();
        vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, md.getMesh(i)->cm, false, true);
        meshCopy->fileName = md.getMesh(i)->fileName;
        meshCopy->cm.Tr = md.getMesh(i)->cm.Tr;
        m_state.push_back(meshCopy);
    }
}

void DynamicMeshSubFilter::initialize(MeshDocument&, RichParameterSet&, vcg::CallBackPos* cb){
    m_steps = m_seconds * m_fps;
}
