/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>
#include "filter_csg.h"

#include "fixed.h"
#include "intercept.h"

using namespace std;
using namespace vcg;
using namespace vcg::intercept;

FilterCSG::FilterCSG()
{
    typeList << FP_CSG;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);
}

QString FilterCSG::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_CSG:
        return "CSG Operation";

    default:
        assert(0);
        return "error";
    }
}

QString FilterCSG::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_CSG:
        return "Constructive Solid Geometry operation filter. Based on ...";

    default:
        assert(0);
        return "error";
    }
}

void FilterCSG::initParameterSet(QAction *action, MeshDocument & md, RichParameterSet & parlst)
{
    switch (ID(action)) {
    case FP_CSG:
        {
            MeshModel *target = md.mm();
            foreach (target, md.meshList)
                if (target != md.mm())
                    break;

            CMeshO::ScalarType mindim = min(md.mm()->cm.bbox.Dim().V(md.mm()->cm.bbox.MinDim()),
                                            target->cm.bbox.Dim().V(target->cm.bbox.MinDim()));

            //TODO: descriptions, tooltips
            parlst.addParam(new RichMesh("FirstMesh", md.mm(), &md, "First Mesh",
                                         "The first operand of the CSG operation"));
            parlst.addParam(new RichMesh("SecondMesh", target, &md, "Second Mesh",
                                         "The second operand of the CSG operation"));
            parlst.addParam(new RichAbsPerc("Delta", mindim / 10.0, 0, mindim,
                                            "Spacing between sampling points"));
            parlst.addParam(new RichEnum("Operator",
                                         0,
                                         QStringList() << "Intersection" << "Union" << "Difference",
                                         "Operator"));
        }
        break;

    default:
        assert(0);
    }
}

bool FilterCSG::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *)
{
    switch(ID(filter)) {
    case FP_CSG:
        {
            MeshModel *firstMesh = par.getMesh("FirstMesh");
            MeshModel *secondMesh = par.getMesh("SecondMesh");
            Log(0, "First BBox: %g %g %g %g %g %g",
                firstMesh->cm.bbox.min.X(), firstMesh->cm.bbox.min.Y(), firstMesh->cm.bbox.min.Z(),
                firstMesh->cm.bbox.max.X(), firstMesh->cm.bbox.max.Y(), firstMesh->cm.bbox.max.Z());
            Log(0, "Operation: %d", par.getEnum("Operator") );
            firstMesh->updateDataMask(MeshModel::MM_FACENORMAL);
            secondMesh->updateDataMask(MeshModel::MM_FACENORMAL);

            typedef CMeshO::ScalarType scalar;
            typedef Intercept<vcg::math::fixed<8,vcg::math::base32>,scalar> intercept;
            const float d = par.getFloat("Delta");
            const Point3f delta(d, d, d);
            InterceptVolume<intercept> v(InterceptSet3<intercept>(firstMesh->cm, delta)),
            tmp(InterceptSet3<intercept>(secondMesh->cm, delta));

            MeshModel *mesh;
            switch(par.getEnum("Operator")){
            case CSG_OPERATION_INTERSECTION:
                v &= tmp;
                mesh = md.addNewMesh("intersection");
                break;

            case CSG_OPERATION_UNION:
                v |= tmp;
                mesh = md.addNewMesh("union");
                break;

            case CSG_OPERATION_DIFFERENCE:
                v -= tmp;
                mesh = md.addNewMesh("difference");
                break;

            default:
                assert(0);
                return true;
            }

        }
        return true;

    default:
        assert (0);
    }
    return true;
}


Q_EXPORT_PLUGIN(FilterCSG)
