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
#include "filter_dirt.h"

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

FilterDirt::FilterDirt()
{
	
    typeList << 
    FP_DIRT;
    
	FilterIDType tt;
	foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

const QString FilterDirt::filterName(FilterIDType filterId) const
{
	switch (filterId) {
		case FP_DIRT:
			return QString("Dust Accumulation");
			break;
		default:
            assert(0); return QString("error");
			break;
	}
}
 QString FilterDirt::filterInfo(FilterIDType filterId) const
{
	switch (filterId) {
		case FP_DIRT:
			return QString("Simulate dust accumulation over the mesh");
			break;
		default:
            assert(0); return QString("error");
			break;
	}
}

 int FilterDirt::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR;
}

bool FilterDirt::applyFilter(QAction * /*filter*/, MeshDocument &md, RichParameterSet & /*par*/, vcg::CallBackPos */*cb*/)
{
	return true;
}

 MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *)
{
	return MeshFilterInterface::VertexColoring;
}


Q_EXPORT_PLUGIN(FilterDirt)
