#include "MeshSubFilter.h"

int MeshSubFilter::m_currentFilterType = 0;

MeshSubFilter::MeshSubFilter(){
}

void MeshSubFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    par.addParam(new RichInt("fps", 50, "Frames per second", "The number of times per second the physics simulation is updated"));
    par.addParam(new RichInt("iterations", 10, "Physics method iterations", "Number of iterations of the physics iterative method for equation solving"));
    par.addParam(new RichInt("contacts", 20, "Max contacts", "Maximum number of contact points to generate per object pair"));
    par.addParam(new RichFloat("bounciness", 0.1f, "Bounciness", "The amount of bounciness of a collision: 0 means the surfaces are not bouncy at all, 1 is the maximum bounciness"));
}
