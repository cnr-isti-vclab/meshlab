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
#include "platonic.h"


using namespace std;
using namespace vcg;

FilterSolidShapes::FilterSolidShapes()
{
    typeList << FSS_TEXT;
    
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

const QString FilterSolidShapes::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case FSS_TEXT:
        return QString("Platonic");
        break;
    default:
        assert(0);
        return QString("error");
        break;
    }
}

const QString FilterSolidShapes::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
    case FSS_TEXT:
        return QString("Create platonic solids according to user parameters");
        break;
    default:
        assert(0);
        return QString("error");
        break;
    }
}

const int FilterSolidShapes::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR;
}

void FilterSolidShapes::initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & par)
{
    QStringList list;
    list << "Tetrahedron" << "Hexahedron" << "Octahedron" << "Dodecahedron" << "Icosahedron";

    par.addParam(new RichEnum("Figure", 0, list, "Figure", "Choose a figure"));
    par.addParam(new RichBool("Star", FALSE, "Star?", "Star or minimal triangulation instead"));
}

bool FilterSolidShapes::applyFilter(QAction *filter, MeshModel &m, RichParameterSet & par, vcg::CallBackPos *cb)
{
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
        break;
    case CR_BOX:
        /* CRASSSHHHH
        float sz=par.getFloat("size");
        vcg::Box3f b(vcg::Point3f(1,1,1)*(sz/2),vcg::Point3f(1,1,1)*(-sz/2));
        vcg::tri::Box<CMeshO>(m.cm,b);
        m.updateDataMask(MeshModel::MM_POLYGONAL);
        */
        break;
    }

    //Camera...
    vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm);
    return true;
}

const MeshFilterInterface::FilterClass FilterSolidShapes::getClass(QAction *)
{
    return MeshFilterInterface::MeshCreation;
}


Q_EXPORT_PLUGIN(FilterSolidShapes)
