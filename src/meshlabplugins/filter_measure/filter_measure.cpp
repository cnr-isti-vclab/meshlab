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
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/stat.h>

#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/append.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <vcg/complex/algorithms/mesh_to_matrix.h>
#include <vcg/complex/algorithms/bitquad_optimization.h>

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
	switch (ID(action)) {
	case PER_VERTEX_QUALITY_HISTOGRAM:
		parlst.addParam(new RichFloat("HistMin", 0, "Hist Min", "The vertex are displaced of a vector whose norm is bounded by this value"));
		parlst.addParam(new RichFloat("HistMax", 0, "Hist Max", "The vertex are displaced of a vector whose norm is bounded by this value"));
		parlst.addParam(new RichBool("areaWeighted", false, "Area Weighted", "If false, the histogram will report the number of vertices with quality values falling in each bin of the histogram. If true each bin of the histogram will report the approximate area of the mesh with that range of values. Area is computed by assigning to each vertex one third of the area all the incident triangles."));
		parlst.addParam(new RichInt("binNum", 20, "Bin number", "The number of bins of the histogram. E.g. the number of intervals in which the min..max range is subdivided into."));
		break;
	case PER_FACE_QUALITY_HISTOGRAM:
		parlst.addParam(new RichFloat("HistMin", 0, "Hist Min", "The faces are displaced of a vector whose norm is bounded by this value"));
		parlst.addParam(new RichFloat("HistMax", 0, "Hist Max", "The faces are displaced of a vector whose norm is bounded by this value"));
		parlst.addParam(new RichBool("areaWeighted", false, "Area Weighted", "If false, the histogram will report the number of faces with quality values falling in each bin of the histogram. If true each bin of the histogram will report the approximate area of the mesh with that range of values."));
		parlst.addParam(new RichInt("binNum", 20, "Bin number", "The number of bins of the histogram. E.g. the number of intervals in which the min..max range is subdivided into."));
		break;
	default:
		break;
	}
}

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
	return MeshModel::MM_NONE;
}

Matrix33m FilterMeasurePlugin::computePrincipalAxisCloud(CMeshO& m)
{
	Matrix33m cov;
	Point3m bp(0, 0, 0);
	std::vector<Point3m> PtVec;
	for (CMeshO::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); ++vi)
	if (!(*vi).IsD())
	{
		PtVec.push_back((*vi).cP());
		bp += (*vi).cP();
	}

	bp /= m.vn;

	cov.Covariance(PtVec, bp);

	Matrix33m eigenvecMatrix;
	Eigen::Matrix3d em;
	cov.ToEigenMatrix(em);
	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig(em);
	Eigen::Matrix3d c_vec = eig.eigenvectors();
	eigenvecMatrix.FromEigenMatrix(c_vec);

	return eigenvecMatrix;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeasurePlugin)
