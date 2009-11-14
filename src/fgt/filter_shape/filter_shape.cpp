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


using namespace std;
using namespace vcg;

FilterSolidShapes::FilterSolidShapes()
{
	
    typeList << 
    FP_TEXT;
    
	FilterIDType tt;
	foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

const QString FilterSolidShapes::filterName(FilterIDType filterId) const
{
	switch (filterId) {
        case FP_TEXT:
            return QString("Platonic solids");
			break;
		default:
            assert(0); return QString("error");
			break;
	}
}
const QString FilterSolidShapes::filterInfo(FilterIDType filterId) const
{
	switch (filterId) {
        case FP_TEXT:
            return QString("Create platonic solids according to user parameters");
			break;
		default:
            assert(0); return QString("error");
			break;
	}
}

const int FilterSolidShapes::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR;
}

void FilterSolidShapes::initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & par)
{
    par.addParam(new RichString("Param1","Default1","Param1","Explanation of Param1"));
    par.addParam(new RichString("Param2","Default2","Param2","Explanation of Param2"));
}

bool FilterSolidShapes::applyFilter(QAction * /*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
	Log("Creating platonic solid...");
    Log("Param1 = '%s'", qPrintable(par.getString("Param1")));
    Log("Param2 = '%s'", qPrintable(par.getString("Param2")));
	return true;
}

const MeshFilterInterface::FilterClass FilterSolidShapes::getClass(QAction *)
{
    return MeshFilterInterface::MeshCreation;
}


Q_EXPORT_PLUGIN(FilterSolidShapes)
