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
#include "filter_text.h"

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

FilterCreateText::FilterCreateText()
{
	
    typeList << 
    FP_TEXT;
    
	FilterIDType tt;
	foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

 QString FilterCreateText::filterName(FilterIDType filterId) const
{
	switch (filterId) {
        case FP_TEXT:
            return QString("String of Text");
			break;
		default:
            assert(0); return QString("error");
			break;
	}
}
 QString FilterCreateText::filterInfo(FilterIDType filterId) const
{
	switch (filterId) {
        case FP_TEXT:
            return QString("Create a 3D model of a text string");
			break;
		default:
            assert(0); return QString("error");
			break;
	}
}

 int FilterCreateText::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR;
}

void FilterCreateText::initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & par)
{
    par.addParam(new RichString("text3d","MeshLab","Text string","The string entered here will be transformed into a 3D model according to the choosen options"));
}

bool FilterCreateText::applyFilter(QAction * /*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
    QString text3d = par.getString("text3d");
    Log("Transforming text '%s'",qPrintable(text3d));
	return true;
}

 MeshFilterInterface::FilterClass FilterCreateText::getClass(QAction *)
{
    return MeshFilterInterface::MeshCreation;
}


Q_EXPORT_PLUGIN(FilterCreateText)
