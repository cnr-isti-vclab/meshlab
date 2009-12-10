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
#include "filter_shape.h"

#include <vcg/math/base.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/selection.h> 
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/space/triangle3.h>

//VCGLib
#include "platonic.h"
#include "archimedean.h"
#include "knot.h"

using namespace std;
using namespace vcg;

FilterSolidShapes::FilterSolidShapes()
{
    typeList << FSS_PLATONIC << FSS_ARCHIMEDEAN << FSS_MISC;
    
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);
}

 QString FilterSolidShapes::filterName(FilterIDType filterId) const
{
    switch (filterId) {
        case FSS_PLATONIC: return tr("Platonic solids");
        case FSS_ARCHIMEDEAN: return tr("Archimedean solids");
        case FSS_MISC: return tr("Misc solids");
        default: assert(0); return QString("error");
    }
}

 QString FilterSolidShapes::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
        case FSS_PLATONIC:
            return tr("Create platonic solids according to user parameters");
        case FSS_ARCHIMEDEAN:
            return tr("Create archimedean solids according to user parameters");
        case FSS_MISC:
            return tr("Create misc solids according to user parameters");
        default: assert(0); return QString("error");
    }
}

 // What is it??
 int FilterSolidShapes::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR;
}

void FilterSolidShapes::initParameterSet(QAction *action,MeshModel &/*m*/, RichParameterSet & par)
{
    QStringList list;

    switch(ID(action))	 {
        case FSS_PLATONIC:
            list << "Tetrahedron" << "Hexahedron" << "Octahedron" << "Dodecahedron" << "Icosahedron";
            break;
        case FSS_ARCHIMEDEAN:
            list  << "Truncated Tetrahedron" << "Cuboctahedron" << "Truncated Cube"
            << "Truncated Octahedron" << "Rhombicuboctahedron" << "Truncated Cuboctahedron"
            << "Snub Cube" << "Icosidodecahedron" << "Truncated Icosahedron";
            break;
        case FSS_MISC:
            list << "Torus 2-3 (test)" << "Torus 3-8";
            break;
    }

    par.addParam(new RichEnum("Figure", 0, list, "Figure", "Choose a figure"));
    par.addParam(new RichBool("Star", FALSE, "Star?", "Star or minimal triangulation instead"));

}

bool FilterSolidShapes::applyFilter(QAction *filter, MeshModel &m, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
    switch(ID(filter)) {
        case FSS_PLATONIC:
            Log("Creating platonic number %d. STAR=%d", par.getEnum("Figure"), par.getBool("Star"));
            switch(par.getEnum("Figure")) {
                case CR_TETRAHEDRON:
                    vcg::tri::Tetrahedron<CMeshO>(m.cm);
                    break;
                case CR_ICOSAHEDRON:
                    vcg::tri::Icosahedron<CMeshO>(m.cm);
                    break;
                case CR_DODECAHEDRON:
                    vcg::tri::Dodecahedron<CMeshO>(m.cm);
                    m.updateDataMask(MeshModel::MM_POLYGONAL);
                    break;
                case CR_OCTAHEDRON:
                    vcg::tri::Octahedron<CMeshO>(m.cm);
                    break;
                case CR_HEXAHEDRON:
                    vcg::tri::Hexahedron<CMeshO>(m.cm);
                    break;
            }
        break;
        case FSS_ARCHIMEDEAN:
                Log("Creating archimedean number %d. STAR=%d", par.getEnum("Figure"), par.getBool("Star"));
                switch(par.getEnum("Figure")) {
                    case CR_TT:
                        vcg::tri::Truncated_Tetrahedron<CMeshO>(m.cm);
                        break;
                    case CR_COH:
                        vcg::tri::Cuboctahedron<CMeshO>(m.cm);
                        break;
                    case CR_TC:
                        vcg::tri::Truncated_Cube<CMeshO>(m.cm);
                        break;
                    case CR_TO:
                        vcg::tri::Truncated_Octahedron<CMeshO>(m.cm);
                        break;
                    case CR_RCOH:
                        vcg::tri::Rhombicuboctahedron<CMeshO>(m.cm);
                        break;
                    case CR_TCOH:
                        vcg::tri::Truncated_Cuboctahedron<CMeshO>(m.cm);
                        break;
                    case CR_SC:
                        vcg::tri::Snub_Cube<CMeshO>(m.cm);
                        break;
                    case CR_ISDH:
                        vcg::tri::Icosidodecahedron<CMeshO>(m.cm);
                        break;
                    case CR_TIS:
                        vcg::tri::Truncated_Icosahedron<CMeshO>(m.cm);
                        break;
                }
        break;
        case FSS_MISC:
            Log("Creating misc number %d. STAR=%d", par.getEnum("Figure"), par.getBool("Star"));
            switch(par.getEnum("Figure")) {
                case CR_TORUS23:
                    //torus 2-3
                    vcg::tri::Torus_knot<CMeshO>(m.cm, 2, 3, 0.05);
                    break;
                case CR_TORUS38:
                    //torus 2-3
                    vcg::tri::Torus_knot<CMeshO>(m.cm, 3, 8, 0.05);
                    break;

            }
    }

    //Camera...
    vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm);
    return true;
}

 MeshFilterInterface::FilterClass FilterSolidShapes::getClass(QAction *)
{
    return MeshFilterInterface::MeshCreation;
}


Q_EXPORT_PLUGIN(FilterSolidShapes)
