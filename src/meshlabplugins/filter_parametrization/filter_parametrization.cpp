/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#include "filter_parametrization.h"

#include <common/utilities/eigen_mesh_conversions.h>

#include <igl/boundary_loop.h>
#include <igl/harmonic.h>
#include <igl/lscm.h>
#include <igl/map_vertices_to_circle.h>

FilterParametrizationPlugin::FilterParametrizationPlugin()
{
	typeList = { FP_HARMONIC_PARAM, FP_LEAST_SQUARES_PARAM};

	for(const ActionIDType& tt : typeList)
		actionList.push_back(new QAction(filterName(tt), this));
}

QString FilterParametrizationPlugin::pluginName() const
{
	return "FilterParametrization";
}

QString FilterParametrizationPlugin::vendor() const
{
	return "CNR-ISTI-VCLab";
}

QString FilterParametrizationPlugin::filterName(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_HARMONIC_PARAM :
		return "Harmonic Parametrization";
	case FP_LEAST_SQUARES_PARAM:
		return "Least Squares Conformal Maps Parametrization";
	default :
		assert(0);
		return "";
	}
}

QString FilterParametrizationPlugin::pythonFilterName(ActionIDType filter) const
{
	switch(filter) {
	case FP_HARMONIC_PARAM :
		return "compute_texcoord_parametrization_harmonic";
	case FP_LEAST_SQUARES_PARAM:
		return "compute_texcoord_parametrization_least_squares_conformal_maps";
	default :
		assert(0);
		return "";
	}
}

QString FilterParametrizationPlugin::filterInfo(ActionIDType filterId) const
{
	QString commonDescription =
		"The resulting parametrization is saved in the per vertex texture coordinates.<br>"
		"The filter uses the original code provided in the "
		"<a href=\"https://libigl.github.io/\">libigl library</a>.<br>";
	switch(filterId) {
	case FP_HARMONIC_PARAM :
		return "Computes a single patch, fixed boundary harmonic parametrization of a mesh. The "
			   "filter requires that the input mesh has a single fixed boundary." +
			   commonDescription;
	case FP_LEAST_SQUARES_PARAM:
		return "Compuites a least squares conformal maps parametrization of a mesh. " +
			   commonDescription;
	default :
		assert(0);
		return "Unknown Filter";
	}
}

FilterParametrizationPlugin::FilterClass FilterParametrizationPlugin::getClass(const QAction *a) const
{
	switch(ID(a)) {
	case FP_HARMONIC_PARAM :
	case FP_LEAST_SQUARES_PARAM:
		return FilterPlugin::Texture;
	default :
		assert(0);
		return FilterPlugin::Generic;
	}
}

FilterPlugin::FilterArity FilterParametrizationPlugin::filterArity(const QAction*) const
{
	return SINGLE_MESH;
}

int FilterParametrizationPlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_VERTCOORD | MeshModel::MM_FACEVERT;
}

int FilterParametrizationPlugin::getRequirements(const QAction*)
{
	return MeshModel::MM_VERTTEXCOORD;
}

int FilterParametrizationPlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_VERTTEXCOORD;
}

RichParameterList FilterParametrizationPlugin::initParameterList(const QAction *action, const MeshModel &)
{
	RichParameterList parlst;
	switch(ID(action)) {
	case FP_HARMONIC_PARAM :
		parlst.addParam(RichInt("harm_function", 1,"N-Harmonic Function", "1 denotes harmonic function, 2 biharmonic, 3 triharmonic, etc."));
		break;
	case FP_LEAST_SQUARES_PARAM:

		break;
	default :
		assert(0);
	}
	return parlst;
}

std::map<std::string, QVariant> FilterParametrizationPlugin::applyFilter(
		const QAction * action,
		const RichParameterList & par,
		MeshDocument &md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos *)
{
	switch(ID(action)) {
	case FP_HARMONIC_PARAM : {
		int f = par.getInt("harm_function");
		if (f < 1)
			throw MLException("Invalid N-Harmonic Function value. Must be >= 1.");

		EigenMatrixX3m v = meshlab::vertexMatrix(md.mm()->cm);
		Eigen::MatrixX3d verts = v.cast<double>();
		Eigen::MatrixX3i faces = meshlab::faceMatrix(md.mm()->cm);

		Eigen::MatrixXd V_uv, bnd_uv;
		Eigen::VectorXi bnd;

		igl::boundary_loop(faces,bnd);
		if (bnd.size() == 0)
			throw MLException(
				"Harmonic Parametrization can be applied only on meshes that have a boundary.");
		igl::map_vertices_to_circle(verts, bnd, bnd_uv);
		igl::harmonic(verts,faces,bnd,bnd_uv,1,V_uv);

		unsigned int i = 0;
		for (auto& v : md.mm()->cm.vert){
			v.T().u() =V_uv(i, 0);
			v.T().v() =V_uv(i, 1);
			i++;
		}

		break;
	}
	case FP_LEAST_SQUARES_PARAM: {
		EigenMatrixX3m v = meshlab::vertexMatrix(md.mm()->cm);
		Eigen::MatrixX3d verts = v.cast<double>();
		Eigen::MatrixX3i faces = meshlab::faceMatrix(md.mm()->cm);
		Eigen::VectorXi bnd, boundaryPoints(2, 1);

		Eigen::MatrixXd V_uv;

		igl::boundary_loop(faces,bnd);
		if (bnd.size() == 0)
			throw MLException(
				"Least Squares Conformal Maps Parametrization can be applied only on meshes that "
				"have a boundary.");

		boundaryPoints(0) = bnd(0);
		boundaryPoints(1) = bnd(bnd.size()/2);

		Eigen::MatrixXd bc(2,2);
		bc<<0,0,1,0;

		// LSCM parametrization
		igl::lscm(verts,faces,boundaryPoints,bc,V_uv);

		unsigned int i = 0;
		for (auto& v : md.mm()->cm.vert){
			v.T().u() =V_uv(i, 0);
			v.T().v() =V_uv(i, 1);
			i++;
		}

		break;
	}
	default :
		wrongActionCalled(action);
	}
	return std::map<std::string, QVariant>();
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterSamplePlugin)
