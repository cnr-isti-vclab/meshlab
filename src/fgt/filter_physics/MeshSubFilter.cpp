#include "MeshSubFilter.h"

int MeshSubFilter::m_currentFilterType = 0;

MeshSubFilter::MeshSubFilter(){
}

void MeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    par.addParam(new RichInt("fps", 100, "Frames per second", "The number of times per second the physics simulation is updated"));
    par.addParam(new RichInt("iterations", 20, "Physics method iterations", "The number of iterations of the iterative physics equation solver method"));
    par.addParam(new RichInt("contacts", 20, "Max contacts", "Maximum number of contact points to generate per object pair"));
    par.addParam(new RichFloat("bounciness", 0.1f, "Bounciness", "The amount of bounciness of a collision: 0 means the surfaces are not bouncy at all, 1 is the maximum bounciness"));
    par.addParam(new RichFloat("friction", 10, "Friction", "The coulomb friction coefficient of a collision"));
}

void MeshSubFilter::initialize(MeshDocument& md, RichParameterSet&, vcg::CallBackPos* cb){
    /*if(md.mm()->cm.Tr != vcg::Matrix44f::Identity())
        throw MatrixNotFreezedException();*/
}

void MeshSubFilter::clearLastAppliedFilter(){
    m_currentFilterType = 0;
}
