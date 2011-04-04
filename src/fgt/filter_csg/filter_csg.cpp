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

#include "filter_csg.h"
#include <vcg/complex/algorithms/create/extended_marching_cubes.h>
#include <vcg/complex/algorithms/create/marching_cubes.h>

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

            parlst.addParam(new RichMesh("FirstMesh", md.mm(), &md, "First Mesh",
                                         "The first operand of the CSG operation"));
            parlst.addParam(new RichMesh("SecondMesh", target, &md, "Second Mesh",
                                         "The second operand of the CSG operation"));
            parlst.addParam(new RichAbsPerc("Delta", mindim / 100.0, 0, mindim,
                                            "Spacing between sampling lines",
                                            "This parameter controls the accuracy of the result and the speed of the computation."
                                            "The time and memory needed to perform the operation usually scale as the reciprocal square of this value."
                                            "For optimal results, this value should be at most half the the smallest feature (i.e. the highest frequency) you want to reproduce."));
            parlst.addParam(new RichInt("SubDelta", 32, "Discretization points per sample interval",
                                        "This is the number of points between the sampling lines to which the vertices can be rounded."
                                        "Increasing this can marginally increase the precision and decrease the speed of the operation."));
            parlst.addParam(new RichEnum("Operator", 0,
                                         QStringList() << "Intersection" << "Union" << "Difference", "Operator",
                                         "Intersection takes the volume shared between the two meshes; "
                                         "Union takes the volume included in at least one of the two meshes; "
                                         "Difference takes the volume included in the first mesh but not in the second one"));
            parlst.addParam(new RichBool("Extended", false, "Extended Marching Cubes",
                                         "Use extended marching cubes for surface reconstruction. "
                                         "It tries to improve the quality of the mesh by reconstructing the sharp features "
                                         "using the information in vertex normals"));
        }
        break;

    default:
        assert(0);
    }
}

bool FilterCSG::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    switch(ID(filter)) {
    case FP_CSG:
        {
            MeshModel *firstMesh = par.getMesh("FirstMesh");
            MeshModel *secondMesh = par.getMesh("SecondMesh");

            firstMesh->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);
            secondMesh->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);
            if (!isValid (firstMesh->cm, this->errorMessage) || !isValid (secondMesh->cm, this->errorMessage))
                return false;

            firstMesh->updateDataMask(MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);
            secondMesh->updateDataMask(MeshModel::MM_FACENORMAL | MeshModel::MM_FACEQUALITY);

            typedef CMeshO::ScalarType scalar;
            typedef Intercept<mpq_class,scalar> intercept;
            const scalar d = par.getFloat("Delta");
            const Point3f delta(d, d, d);
            const int subFreq = par.getInt("SubDelta");
            Log(0, "Rasterizing first volume...");
            InterceptVolume<intercept> v = InterceptSet3<intercept>(firstMesh->cm, delta, subFreq, cb);
            Log(0, "Rasterizing second volume...");
            InterceptVolume<intercept> tmp = InterceptSet3<intercept>(secondMesh->cm, delta, subFreq, cb);

            MeshModel *mesh;
            switch(par.getEnum("Operator")){
            case CSG_OPERATION_INTERSECTION:
                Log(0, "Intersection...");
                v &= tmp;
                mesh = md.addNewMesh("","intersection");
                break;

            case CSG_OPERATION_UNION:
                Log(0, "Union...");
                v |= tmp;
                mesh = md.addNewMesh("","union");
                break;

            case CSG_OPERATION_DIFFERENCE:
                Log(0, "Difference...");
                v -= tmp;
                mesh = md.addNewMesh("","difference");
                break;

            default:
                assert(0);
                return true;
            }

            Log(0, "Building mesh...");
            typedef vcg::intercept::Walker<CMeshO, intercept> MyWalker;
            MyWalker walker;
            if (par.getBool("Extended")) {
                mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);
                typedef vcg::tri::ExtendedMarchingCubes<CMeshO, MyWalker> MyExtendedMarchingCubes;
                MyExtendedMarchingCubes mc(mesh->cm, walker);
                walker.BuildMesh<MyExtendedMarchingCubes>(mesh->cm, v, mc, cb);
            } else {
                typedef vcg::tri::MarchingCubes<CMeshO, MyWalker> MyMarchingCubes;
                MyWalker walker;
                MyMarchingCubes mc(mesh->cm, walker);
                walker.BuildMesh<MyMarchingCubes>(mesh->cm, v, mc, cb);
            }
            Log(0, "Done");

            vcg::tri::UpdateBounding<CMeshO>::Box(mesh->cm);
            vcg::tri::UpdateNormals<CMeshO>::PerFaceFromCurrentVertexNormal(mesh->cm);
        }
        return true;

    default:
        assert (0);
    }
    return true;
}


Q_EXPORT_PLUGIN(FilterCSG)
