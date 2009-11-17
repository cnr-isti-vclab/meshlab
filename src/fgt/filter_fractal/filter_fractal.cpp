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
#include "filter_fractal.h"

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

using namespace std;
using namespace vcg;

FilterFractal::FilterFractal()
{
    typeList << CR_FRACTAL_TERRAIN;
    FilterIDType tt;
    foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

const QString FilterFractal::filterName(FilterIDType filterId) const
{
    switch (filterId) {
        case CR_FRACTAL_TERRAIN:
            return QString("Fractal terrain");
            break;
        default:
            assert(0); return QString("error");
            break;
    }
}

const QString FilterFractal::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
        case CR_FRACTAL_TERRAIN:
            return QString("Generates a fractal terrain");
            break;
        default:
            assert(0); return QString("error");
            break;
    }
}

const int FilterFractal::getRequirements(QAction*/*action*/)
{	
    return MeshModel::MM_NONE;
}

void FilterFractal::initParameterSet(QAction* filter,MeshModel &/*m*/, RichParameterSet & /*parent*/)
{
    return;
}

bool FilterFractal::applyFilter(QAction* filter, MeshModel &/*m*/, RichParameterSet & /* par */, vcg::CallBackPos * /*cb*/)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
            return true;
            break;
    }
    return false;
}

bool FilterFractal::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet & /*par*/, vcg::CallBackPos */*cb*/)
{
    return true;
}

const MeshFilterInterface::FilterClass FilterFractal::getClass(QAction* filter)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
            return MeshFilterInterface::MeshCreation;
        break;
        default: assert(0);
            return MeshFilterInterface::Generic;
    }
}

bool FilterFractal::autoDialog(QAction *)
{
    return true;
}

Q_EXPORT_PLUGIN(FilterFractal)
