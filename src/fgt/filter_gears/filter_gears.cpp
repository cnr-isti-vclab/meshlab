/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <QtScript>

#include "filter_gears.h"
#include "spur.h"

#include <vcg/complex/algorithms/create/extrude.h>

// Constructor
FilterGears::FilterGears() {

    typeList << FG_GEAR_SINGLE << FG_GEAR_COUPLE;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

// Menu entries
QString FilterGears::filterName(FilterIDType filterId) const {

    switch(filterId) {
    case FG_GEAR_SINGLE: return QString("Gear");
    case FG_GEAR_COUPLE: return QString("Gear couple");
    default : assert(0);
    }
    return QString();

}

// Description
QString FilterGears::filterInfo(FilterIDType filterId) const {

    QString description;

    switch(filterId) {

    case FG_GEAR_SINGLE:
        description = "Create a single involute spur gear ";
        break;

    case FG_GEAR_COUPLE:
        description = "Create a couple of involute spur gears ";
        break;

    default : assert(0);
    }

    QFile f(":/filter_gears_description.txt");
    if (f.open(QFile::ReadOnly)) {
        QTextStream stream(&f);
        description += stream.readAll();
        f.close();
    } else {
        description += "file not found.";
    }

    return description;
}

// Submenu
FilterGears::FilterClass FilterGears::getClass(QAction *a) {

    switch(ID(a)) {
    case FG_GEAR_SINGLE:
    case FG_GEAR_COUPLE: return MeshFilterInterface::MeshCreation;
    default : assert(0);
    }
    return MeshFilterInterface::Generic;

}

//Set parameters
void FilterGears::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) {

    switch(ID(action))	 {
    case FG_GEAR_SINGLE:
        parlst.addParam(new RichFloat("r", 60.0, "Radius", "Pitch radius of the gear, that is, the radius of the imaginary rolling cylinder that the gear is considered to replace."));
        parlst.addParam(new RichFloat("m", 10.0, "Module", "Ratio between pitch diameter and number of teeth, that is, the length of a tooth measured on the pitch circle."));
        parlst.addParam(new RichFloat("t", 10.0, "Thickness", "Thickness the gear."));
        parlst.addParam(new RichFloat("a", 20.0, "Pressure angle", "It determines the line on which two gears of the same module are always touching. Standard values are 20&deg;, 25&deg; and 14.5&deg;."));
        parlst.addParam(new RichFloat("h", 10.0, "Hole radius", "Radius of the hole, 0 means no hole."));
        break;

    case FG_GEAR_COUPLE:
        parlst.addParam(new RichFloat("i", 180.0, "Interaxis", "Distance between the centres of the two gears."));
        parlst.addParam(new RichFloat("r", 2.0, "Speed ratio", "Ratio between angular velocities."));
        parlst.addParam(new RichFloat("m", 10.0, "Module", "Ratio between pitch diameter and number of teeth, that is, the length of a tooth measured on the pitch circle. The pitch diameter is the diameter of the imaginary rolling cylinder that a gear is considered to replace."));
        parlst.addParam(new RichFloat("t", 10.0, "Thickness", "Thickness of the gears."));
        parlst.addParam(new RichFloat("a", 20.0, "Pressure angle", "It determines the line on which two gears of the same module are always touching. Standard values are 20&deg;, 25&deg; and 14.5&deg;."));
        parlst.addParam(new RichFloat("h", 10.0, "Hole radius", "Radius of the holes, 0 means no holes."));
        break;

    default : assert(0);
    }

}

// The Real Core Function doing the actual mesh processing.
bool FilterGears::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb) {

    MeshModel *g, *s;
    bool res = false;

    if (ID(filter) == FG_GEAR_SINGLE) {

        CMeshO gp;
        res = Spur<CMeshO>::createSingle(*this, this->errorMessage, gp, par.getFloat("r"), par.getFloat("m"), par.getFloat("a"), par.getFloat("h"));
        if (res) {
            g = md.addNewMesh("","gear");
            g->updateDataMask(MeshModel::MM_FACEFACETOPO);
//            Spur<CMeshO>::extrudeandcap(gp, g->cm, par.getFloat("t"));
            tri::Extrude<CMeshO>::ProfileWithCap(gp, g->cm, Point3f(0, 0, -par.getFloat("t")));
            vcg::tri::UpdateBounding<CMeshO>::Box(g->cm);
            vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(g->cm);
        }

    } else if (ID(filter) == FG_GEAR_COUPLE) {

        CMeshO sp, gp;
        res = Spur<CMeshO>::createCouple(*this, this->errorMessage, sp, gp, par.getFloat("i"), par.getFloat("r"), par.getFloat("m"), par.getFloat("a"), par.getFloat("h"));
        if (res){
            g = md.addNewMesh("","gear");
            s = md.addNewMesh("","sprocket");
            s->updateDataMask(MeshModel::MM_FACEFACETOPO);
            g->updateDataMask(MeshModel::MM_FACEFACETOPO);
//            Spur<CMeshO>::extrudeandcap(sp, s->cm, par.getFloat("t"));
//            Spur<CMeshO>::extrudeandcap(gp, g->cm, par.getFloat("t"));
            tri::Extrude<CMeshO>::ProfileWithCap(sp, s->cm, Point3f(0, 0, -par.getFloat("t")));
            tri::Extrude<CMeshO>::ProfileWithCap(gp, g->cm, Point3f(0, 0, -par.getFloat("t")));
            vcg::tri::UpdateBounding<CMeshO>::Box(g->cm);
            vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(g->cm);
            vcg::tri::UpdateBounding<CMeshO>::Box(s->cm);
            vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(s->cm);
        }
    }

    return res;

}

Q_EXPORT_PLUGIN(FilterGears)
