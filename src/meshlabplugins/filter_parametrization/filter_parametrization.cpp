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
#include <igl/map_vertices_to_circle.h>

FilterParametrizationPlugin::FilterParametrizationPlugin()
{ 
	typeList = { FP_HARMONIC_PARAM};

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
	default :
		assert(0);
		return "";
	}
}

QString FilterParametrizationPlugin::filterInfo(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_HARMONIC_PARAM :
		return "";
	default :
		assert(0);
		return "Unknown Filter";
	}
}

FilterParametrizationPlugin::FilterClass FilterParametrizationPlugin::getClass(const QAction *a) const
{
	switch(ID(a)) {
	case FP_HARMONIC_PARAM :
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

RichParameterList FilterParametrizationPlugin::initParameterList(const QAction *action, const MeshModel &m)
{
	RichParameterList parlst;
	switch(ID(action)) {
	case FP_HARMONIC_PARAM :
		parlst.addParam(RichInt("harm_function", 1,"N-Harmonic Function", "1 denotes harmonic function, 2 biharmonic, 3 triharmonic, etc."));
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
		igl::map_vertices_to_circle(verts,bnd,bnd_uv);
		igl::harmonic(verts,faces,bnd,bnd_uv,1,V_uv);

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
