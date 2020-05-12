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

#include "filter_measure.h"
#include <QtScript>

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterMeasurePlugin::FilterMeasurePlugin()
{ 
	typeList << COMPUTE_TOPOLOGICAL_MEASURES
			 << COMPUTE_TOPOLOGICAL_MEASURES_QUAD_MESHES
			 << COMPUTE_GEOMETRIC_MEASURES
			 << COMPUTE_AREA_PERIMETER_SELECTION
			 << PER_VERTEX_QUALITY_STAT
			 << PER_FACE_QUALITY_STAT
			 << PER_VERTEX_QUALITY_HISTOGRAM
			 << PER_FACE_QUALITY_HISTOGRAM;

	for(FilterIDType tt : types())
		actionList << new QAction(filterName(tt), this);
}

QString FilterMeasurePlugin::filterName(FilterIDType filterId) const
{
	switch (filterId) {
	case COMPUTE_TOPOLOGICAL_MEASURES:
		return "Compute Topological Measures";
		break;
	case COMPUTE_TOPOLOGICAL_MEASURES_QUAD_MESHES:
		return "Compute Topological Measures for Quad Meshes";
		break;
	case COMPUTE_GEOMETRIC_MEASURES:
		return "Compute Geometric Measures";
		break;
	case COMPUTE_AREA_PERIMETER_SELECTION:
		return "Compute Area/Perimeter of selection";
		break;
	case PER_VERTEX_QUALITY_STAT:
		return "Per Vertex Quality Stat";
		break;
	case PER_FACE_QUALITY_STAT:
		return "Per Face Quality Stat";
		break;
	case PER_VERTEX_QUALITY_HISTOGRAM:
		return "Per Vertex Quality Histogram";
		break;
	case PER_FACE_QUALITY_HISTOGRAM:
		return "Per Face Quality Histogram";
		break;
	default:
		assert(0);
		return "";
	}
}

 QString FilterMeasurePlugin::filterInfo(FilterIDType filterId) const
{
	switch (filterId) {
	case COMPUTE_TOPOLOGICAL_MEASURES:
		return "Compute a set of topological measures over a mesh.";
		break;
	case COMPUTE_TOPOLOGICAL_MEASURES_QUAD_MESHES:
		return "Compute a set of topological measures over a quad mesh.";
		break;
	case COMPUTE_GEOMETRIC_MEASURES:
		return "Compute a set of geometric measures of a mesh/pointcloud. Bounding box extents and diagonal, principal axis, thin shell barycenter (mesh only), vertex barycenter and quality-weighted barycenter (pointcloud only), surface area (mesh only), volume (closed mesh) and Inertia tensor Matrix (closed mesh). Open the layer dialog to see the results.";
		break;
	case COMPUTE_AREA_PERIMETER_SELECTION:
		return "Compute area and perimeter of the FACE selection. Open the layer dialog to see the results.";
		break;
	case PER_VERTEX_QUALITY_STAT:
		return "Compute some aggregate statistics over the per vertex quality, like Min, Max, Average, StdDev and Variance.";
		break;
	case PER_FACE_QUALITY_STAT:
		return "Compute some aggregate statistics over the per vertex quality, like Min, Max, Average, StdDev and Variance.";
		break;
	case PER_VERTEX_QUALITY_HISTOGRAM:
		return "Compute an histogram of the values of the per-vertex quality. It can be useful to evaluate the distribution of the quality value over the surface. It can be discrete (e.g. based on vertex count or area weighted).";
		break;
	case PER_FACE_QUALITY_HISTOGRAM:
		return "Compute an histogram of the values of the per-face quality.";
		break;
	default:
		assert(0);
		return "";
	}
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be chosen.
FilterMeasurePlugin::FilterClass FilterMeasurePlugin::getClass(QAction *)
{
    return MeshFilterInterface::Measure;
}

MeshFilterInterface::FILTER_ARITY FilterMeasurePlugin::filterArity(QAction*) const
{
    return SINGLE_MESH;
}

void FilterMeasurePlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst)
{
//	 switch(ID(action))	 {
//		case FP_MOVE_VERTEX :
// 		  parlst.addParam(new RichBool ("UpdateNormals",
//											true,
//											"Recompute normals",
//											"Toggle the recomputation of the normals after the random displacement.\n\n"
//											"If disabled the face normals will remains unchanged resulting in a visually pleasant effect."));
//			parlst.addParam(new RichAbsPerc("Displacement",
//												m.cm.bbox.Diag()/100.0f,0.0f,m.cm.bbox.Diag(),
//												"Max displacement",
//												"The vertex are displaced of a vector whose norm is bounded by this value"));
//											break;
											
//		default : assert(0);
//	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool FilterMeasurePlugin::applyFilter(QAction * /*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
//	CMeshO &m = md.mm()->cm;
//	srand(time(NULL));
//	const float max_displacement =par.getAbsPerc("Displacement");

//	for(unsigned int i = 0; i< m.vert.size(); i++){
//		 // Typical usage of the callback for showing a nice progress bar in the bottom.
//		 // First parameter is a 0..100 number indicating percentage of completion, the second is an info string.
//		  cb(100*i/m.vert.size(), "Randomly Displacing...");

//		Scalarm rndax = (Scalarm(2.0*rand())/RAND_MAX - 1.0 ) *max_displacement;
//		Scalarm rnday = (Scalarm(2.0*rand())/RAND_MAX - 1.0 ) *max_displacement;
//		Scalarm rndaz = (Scalarm(2.0*rand())/RAND_MAX - 1.0 ) *max_displacement;
//		m.vert[i].P() += Point3m(rndax,rnday,rndaz);
//	}
	
//	// Log function dump textual info in the lower part of the MeshLab screen.
//	Log("Successfully displaced %i vertices",m.vn);
	
//	// to access to the parameters of the filter dialog simply use the getXXXX function of the FilterParameter Class
//	if(par.getBool("UpdateNormals"))
//			vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m);
	
//	vcg::tri::UpdateBounding<CMeshO>::Box(m);

//	return true;
}

int FilterMeasurePlugin::postCondition(QAction*) const
{
//	return MeshModel::MM_VERTCOORD | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeasurePlugin)
