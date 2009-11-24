#include "filter_physics.h"

#include <Qt>
#include <QtGui>

using namespace std;
using namespace vcg;

FilterPhysics::FilterPhysics(){
    typeList << FP_PHYSICS;
    
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

const QString FilterPhysics::filterName(FilterIDType filterId) const{
    switch (filterId) {
    case FP_PHYSICS:
        return QString("Physics stuff");
    default:
        assert(0);
        break;
    }
}
const QString FilterPhysics::filterInfo(FilterIDType filterId) const{
    switch (filterId) {
    case FP_PHYSICS:
        return QString("Runs a physics simulation on a set of meshes");
    default:
        assert(0);
        break;
    }
}

void FilterPhysics::initParameterSet(QAction *,MeshModel& m, RichParameterSet & par){
    par.addParam(new RichDynamicFloat("timeline",
                                         0, 0, 100,
                                         "Timeline",
                                         "Physics simulation is run"));
}

bool FilterPhysics::applyFilter(QAction*, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos*){
    /*float timeline  = par.getDynamicFloat("timeline") / 1000.0f;
    if(timeline <= 0)
        return true;*/

    float gravity[3] = {0.0f, -9.8f, 0.0f};

    ODEFacade engine;
    engine.setGlobalForce(gravity);
    engine.registerTriMesh(*md.mm());
    engine.integrate(0.01f);

    return true;
}

const MeshFilterInterface::FilterClass FilterPhysics::getClass(QAction *){
    return MeshFilterInterface::Generic;
}

Q_EXPORT_PLUGIN(FilterPhysics)
