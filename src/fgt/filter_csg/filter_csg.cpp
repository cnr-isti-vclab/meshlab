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

#include <vcg/complex/trimesh/create/extended_marching_cubes.h>
#include <vcg/complex/trimesh/create/marching_cubes.h>

#include <fstream>
#include "gmpfrac.h"
#include "intercept.h"

using namespace std;
using namespace vcg;
using namespace vcg::intercept;

FilterCSG::FilterCSG()
{
    typeList << FP_CSG;

    foreach(FilterIDType tt, types())
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
        return "Constructive Solid Geometry operation filter.<br>"
                "For more details see: <br>"
                "<i>C. Rocchini, P. Cignoni, F. Ganovelli, C. Montani, P. Pingi and R.Scopigno, </i><br>"
                "<b>'Marching Intersections: an Efficient Resampling Algorithm for Surface Management'</b><br>"
                "In Proceedings of Shape Modeling International (SMI) 2001";

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

            typedef float scalar;
            //typedef Intercept<vcg::math::fixed<8,vcg::math::base32>,scalar> intercept;            
            typedef Intercept<fraction,scalar> intercept;
            const scalar d = par.getFloat("Delta");
            const Point3f delta(d, d, d);
            Log(0, "Rasterizing first volume...");
            InterceptVolume<intercept> v = InterceptSet3<intercept>(firstMesh->cm, delta);
            Log(0, "Rasterizing second volume...");
            InterceptVolume<intercept> tmp = InterceptSet3<intercept>(secondMesh->cm, delta);

            MeshModel *mesh;
            switch(par.getEnum("Operator")){
            case CSG_OPERATION_INTERSECTION:
                Log(0, "Intersection...");
                v &= tmp;
                mesh = md.addNewMesh("intersection");
                break;

            case CSG_OPERATION_UNION:
                Log(0, "Union...");
                v |= tmp;
                mesh = md.addNewMesh("union");
                break;

            case CSG_OPERATION_DIFFERENCE:
                Log(0, "Difference...");
                v -= tmp;
                mesh = md.addNewMesh("difference");
                break;

            default:
                assert(0);
                return true;
            }

            Log(0, "Building mesh...");
            typedef vcg::intercept::Walker<CMeshO, intercept> MyWalker;
            typedef vcg::tri::ExtendedMarchingCubes<CMeshO, MyWalker> MyExtendedMarchingCubes;
            typedef vcg::tri::MarchingCubes<CMeshO, MyWalker> MyMarchingCubes;
            MyWalker walker;
            MyMarchingCubes mc(mesh->cm, walker);
            walker.BuildMesh<MyMarchingCubes>(mesh->cm, v, mc);
            Log(0, "Done");

            vcg::tri::UpdateBounding<CMeshO>::Box(mesh->cm);
            vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(mesh->cm);
        }
        return true;

    default:
        assert (0);
    }
    return true;
}


Q_EXPORT_PLUGIN(FilterCSG)
