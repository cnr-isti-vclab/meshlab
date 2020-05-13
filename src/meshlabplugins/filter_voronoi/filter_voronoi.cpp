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

#include "filter_voronoi.h"
#include <QtScript>

FilterVoronoiPlugin::FilterVoronoiPlugin()
{ 
	typeList
		<< VORONOI_SAMPLING
		<< VOLUME_SAMPLING
		<< VORONOI_SCAFFOLDING
		<< BUILD_SHELL
		<< CROSS_FIELD_CREATION
		<< CROSS_FIELD_SMOOTHING;

	for (FilterIDType tt : types())
		actionList << new QAction(filterName(tt), this);
}

QString FilterVoronoiPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
	case VORONOI_SAMPLING :
		return "Random Vertex Displacement";
	case VOLUME_SAMPLING:
		return "Volumetric Sampling";
	case VORONOI_SCAFFOLDING:
		return "Voronoi Scaffolding";
	case BUILD_SHELL:
		return "Create Solid Wireframe";
	case CROSS_FIELD_CREATION:
		return "Cross Field Creation";
	case CROSS_FIELD_SMOOTHING:
		return "Cross Field Smoothing";
	default :
		assert(0);
		return "";
	}
}


QString FilterVoronoiPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
	case VORONOI_SAMPLING :
		return "Compute a sampling over a mesh and perform a Lloyd relaxation.";
	case VOLUME_SAMPLING:
		return "Compute a volumetric sampling over a watertight mesh.";
	case VORONOI_SCAFFOLDING:
		return "Compute a volumetric sampling over a watertight mesh.";
	case BUILD_SHELL:
		return "";
	case CROSS_FIELD_CREATION:
		return "";
	case CROSS_FIELD_SMOOTHING:
		return "";
	default :
		assert(0);
		return "";
	}
}

FilterVoronoiPlugin::FilterClass FilterVoronoiPlugin::getClass(QAction *a)
{
	switch(ID(a)) {
	case VORONOI_SAMPLING :
		return MeshFilterInterface::Sampling;
	case VOLUME_SAMPLING:
		return MeshFilterInterface::Sampling;
	case VORONOI_SCAFFOLDING:
		return MeshFilterInterface::Sampling;
	case BUILD_SHELL:
		return MeshFilterInterface::Remeshing;
	case CROSS_FIELD_CREATION:
		return MeshFilterInterface::Normal;
	case CROSS_FIELD_SMOOTHING:
		return MeshFilterInterface::Smoothing;
	default :
		assert(0);
		return MeshFilterInterface::Generic;
	}
}

void FilterVoronoiPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst)
{
	 switch(ID(action))	 {
	 case VORONOI_SAMPLING :
		 break;
	 case VOLUME_SAMPLING:
		 break;
	 case VORONOI_SCAFFOLDING:
		 break;
	 case BUILD_SHELL:
		 break;
	 case CROSS_FIELD_CREATION:
		 break;
	 case CROSS_FIELD_SMOOTHING:
		 break;
	 default :
		 assert(0);
	}
}

bool FilterVoronoiPlugin::applyFilter(QAction * /*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{

}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterVoronoiPlugin)
