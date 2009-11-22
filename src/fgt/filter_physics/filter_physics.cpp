#include "filter_physics.h"
#include "ODEFacade.h"

#include <Qt>
#include <QtGui>

using namespace std;
using namespace vcg;

FilterPhysics::FilterPhysics()
{
    typeList << FP_PHYSICS;
    
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

const QString FilterPhysics::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_PHYSICS:
        return QString("Physics stuff");
    default:
        assert(0);
        break;
    }
}
const QString FilterPhysics::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_PHYSICS:
        return QString("Apply physics to a set of meshes");
    default:
        assert(0);
        break;
    }
}

const int FilterPhysics::getRequirements(QAction*)
{	
    return MeshModel::MM_FACEVERT | MeshModel::MM_VERTNORMAL;
}

void FilterPhysics::initParameterSet(QAction *,MeshModel& m, RichParameterSet & par)
{
    //par.addParam(new RichString("text3d","MeshLab","Text string","The string entered here will be transformed into a 3D model according to the choosen options"));
}

bool FilterPhysics::applyFilter(QAction * /*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
    ODEFacade engine;
    engine.registerTriMesh(*md.mm());
}

const MeshFilterInterface::FilterClass FilterPhysics::getClass(QAction *)
{
    return MeshFilterInterface::Generic;
}

Q_EXPORT_PLUGIN(FilterPhysics)
