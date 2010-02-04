#include "filter_physics.h"

#include <Qt>
#include <QtGui>
#include <QMessageBox>

#include <exception>

using namespace std;
using namespace vcg;

FilterPhysics::FilterPhysics(){
    typeList << FP_PHYSICS_GRAVITY;
    typeList << FP_PHYSICS_RNDDROP;
    typeList << FP_PHYSICS_RNDFILL;
    
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

FilterPhysics::~FilterPhysics(){
}

 QString FilterPhysics::filterName(FilterIDType filterId) const{
    switch (filterId) {
    case FP_PHYSICS_GRAVITY:
        return QString("Physics gravity simulation");
    case FP_PHYSICS_RNDDROP:
        return QString("Physics random drop");
    case FP_PHYSICS_RNDFILL:
        return QString("Physics random fill");
    default:
        assert(0);
        break;
    }
}
 QString FilterPhysics::filterInfo(FilterIDType filterId) const{
    switch (filterId) {
    case FP_PHYSICS_GRAVITY:
        return QString("Runs a physics gravity simulation on a set of meshes. Once a static environment mesh has been selected, all other meshes will interact with it dynamically under the influence of gravity. <b>Mesh units should be scaled around 1.0 or you may experience stability problems.</b>");
    case FP_PHYSICS_RNDDROP:
        return QString("Runs a random physics gravity simulation on a set of meshes. The selected random mesh will be spawn on the scene and will interact dynamically under the influence of gravity with all other objects that will be considered as static environment. <b>Mesh units should be scaled around 1.0 or you may experience stability problems.</b>");
    case FP_PHYSICS_RNDFILL:
        return QString("Runs a random physics mesh fill simulation. A mesh will act as a container for a number of copies of a selected filler mesh. <b>Mesh units should be scaled around 1.0 or you may experience stability problems.</b>");
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
    case FP_PHYSICS_RNDDROP:
        m_rndDropFilter.initParameterSet(action, md, par);
        break;
    case FP_PHYSICS_RNDFILL:
        m_rndFillFilter.initParameterSet(action, md, par);
    default:
        break;
    }
}

bool FilterPhysics::applyFilter(QAction* action, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    try{
        bool ret = true;

        switch(ID(action)){
        case FP_PHYSICS_GRAVITY:
            ret = m_gravityFilter.applyFilter(action, md, par, cb);
            break;
        case FP_PHYSICS_RNDDROP:
            ret = m_rndDropFilter.applyFilter(action, md, par, cb);
            break;
        case FP_PHYSICS_RNDFILL:
            ret = m_rndFillFilter.applyFilter(action, md, par, cb);
            break;
        default:
            break;
        }

        if(!ret){
            QMessageBox::critical(0, QString("Error"), QString("Invalid filter configuration: check your parameters"));
            return false;
        }
    }catch(std::exception& ex){
        MeshSubFilter::clearLastAppliedFilter();
        QMessageBox::critical(0, QString("Error"), ex.what());
        return false;
    }

    return true;
}

 MeshFilterInterface::FilterClass FilterPhysics::getClass(QAction *){
    return MeshFilterInterface::Layer;
}

Q_EXPORT_PLUGIN(FilterPhysics)
