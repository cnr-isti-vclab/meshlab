#include "filter_physics.h"

#include <Qt>
#include <QtGui>

using namespace std;
using namespace vcg;

FilterPhysics::FilterPhysics(){
    typeList << FP_PHYSICS_GRAVITY;
    
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

FilterPhysics::~FilterPhysics(){
}

 QString FilterPhysics::filterName(FilterIDType filterId) const{
    switch (filterId) {
    case FP_PHYSICS_GRAVITY:
        return QString("Physics gravity demo");
    default:
        assert(0);
        break;
    }
}
 QString FilterPhysics::filterInfo(FilterIDType filterId) const{
    switch (filterId) {
    case FP_PHYSICS_GRAVITY:
        return QString("Runs a physics gravity simulation on a set of meshes");
    default:
        assert(0);
        break;
    }
}

void FilterPhysics::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    switch(ID(action)){
    case FP_PHYSICS_GRAVITY:
        m_gravityFilter.initParameterSet(action, md, par);
        break;
    default:
        break;
    }
}

bool FilterPhysics::applyFilter(QAction* action, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    switch(ID(action)){
    case FP_PHYSICS_GRAVITY:
        return m_gravityFilter.applyFilter(action, md, par, cb);
        break;
    default:
        break;
    }

    return true;
}

 MeshFilterInterface::FilterClass FilterPhysics::getClass(QAction *){
    return MeshFilterInterface::Generic;
}

Q_EXPORT_PLUGIN(FilterPhysics)
