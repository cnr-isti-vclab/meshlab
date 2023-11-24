/*****************************************************************************
 * MeshLab - Alpha Wrap plugin                                       o o     *
 * by PTC, based on CGAL                                           o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2023                                                \/)\/    *
 * PTC                                                             /\/|      *
 * Lex van der Sluijs                                                 |      *
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

#include "filter_mesh_alpha_wrap.h"

#include <common/utilities/eigen_mesh_conversions.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Surface_mesh.h>
#include "CGAL/alpha_wrap_3.h"
#include "qdebug.h"


namespace PMP = CGAL::Polygon_mesh_processing;
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = K::Point_3;
using CgalMesh = CGAL::Surface_mesh<Point_3>;

typedef CgalMesh::Vertex_index vertex_descriptor;
typedef CgalMesh::Face_index face_descriptor;

typedef boost::graph_traits<CgalMesh>::vertex_descriptor boost_vertex_descriptor;

/**
 * @brief
 * Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions.
 * If you want to add icons to your filtering actions you can do here by construction the QActions
 * accordingly
 */
FilterMeshAlphaWrap::FilterMeshAlphaWrap()
{
	typeList = {MESH_ALPHA_WRAP};

	for (const ActionIDType& tt : typeList)
		actionList.push_back(new QAction(filterName(tt), this));
}

QString FilterMeshAlphaWrap::pluginName() const
{
	return "FilterMeshAlphaWrap";
}

QString FilterMeshAlphaWrap::vendor() const
{
    return "PTC";
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterMeshAlphaWrap::filterName(ActionIDType filterId) const
{
	switch (filterId) {
	case MESH_ALPHA_WRAP: return "Alpha Wrap";
	default: assert(0); return QString();
	}
}

QString FilterMeshAlphaWrap::pythonFilterName(ActionIDType f) const
{
	switch (f) {
	case MESH_ALPHA_WRAP: return "generate_alpha_wrap";
	default: assert(0); return QString();
	}
}

/**
 * @brief // Info() must return the longer string describing each filtering action
 * (this string is used in the About plugin dialog)
 * @param filterId: the id of the filter
 * @return an info string of the filter
 */
QString FilterMeshAlphaWrap::filterInfo(ActionIDType filterId) const
{
	QString description =
		"This filter extecutes an Alpha Wrap based on the input mesh. <br>"
		"The filter uses the original code provided in CGAL "
		"<a href=\"https://doc.cgal.org/latest/Alpha_wrap_3/index.html#Chapter_3D_Alpha_wrapping\">3D Alpha Wrapping</a>.<br>"
		"<br>"
		"Alpha: this is the size of the 'ball', specified as the fraction of the length of the largest diagonal of the bounding box. So, if this value is 0.02 then the size of the ball is 2% of the largest diagonal. Note that the run-time and memory consumption will increase with a smaller ball size.<br><br>"
		"Offset: the offset distance that is added to the surface, always larger than 0, as a fraction of the length of the largest diagonal. A value of 0.001 means that the surface will be offset by a thousandth of this length.<br><br>"
		"The implementation is based on the following paper:<br>"
		"Cédric Portaneri, Mael Rouxel-Labbé, Michael Hemmer, David Cohen-Steiner, Pierre Alliez, "
		"<a href=\"https://inria.hal.science/hal-03688637\"><b>\"Alpha Wrapping with an Offset\"</b></a> (2022)<br><br>This plugin is contributed by Lex van der Sluijs at PTC.<br>";
	switch (filterId) {
	case MESH_ALPHA_WRAP: return description;
	default: assert(0); return "Unknown Filter";
	}
}

/**
 * @brief The FilterClass describes in which generic class of filters it fits.
 * This choice affect the submenu in which each filter will be placed
 * More than a single class can be chosen.
 * @param a: the action of the filter
 * @return the class od the filter
 */
FilterMeshAlphaWrap::FilterClass FilterMeshAlphaWrap::getClass(const QAction* a) const
{
	switch (ID(a)) {
	case MESH_ALPHA_WRAP:
		return FilterPlugin::FilterClass(
			FilterPlugin::FilterClass(FilterPlugin::Layer + FilterPlugin::Remeshing));
	default: assert(0); return FilterPlugin::Generic;
	}
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterPlugin::FilterArity FilterMeshAlphaWrap::filterArity(const QAction*) const
{
    return FIXED;
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
// int FilterMeshBooleans::getPreConditions(const QAction*) const
//{
//	return MeshModel::MM_NONE;
//}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
// int FilterMeshBooleans::postCondition(const QAction*) const
//{
//	return MeshModel::MM_VERTCOORD | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL;
//}

/**
 * @brief This function define the needed parameters for each filter. Return true if the filter has
 * some parameters it is called every time, so you can set the default value of parameters according
 * to the mesh For each parameter you need to define,
 * - the name of the parameter,
 * - the default value
 * - the string shown in the dialog
 * - a possibly long string describing the meaning of that parameter (shown as a popup help in the
 * dialog)
 * @param action
 * @param m
 * @param parlst
 */
RichParameterList
FilterMeshAlphaWrap::initParameterList(const QAction* action, const MeshDocument& md)
{
	RichParameterList parlst;
	switch (ID(action)) {
	case MESH_ALPHA_WRAP:
	{
		parlst.addParam(RichFloat(
			"Alpha fraction",
			0.02,
			tr("Alpha: the size of the ball (fraction)"),
			tr("")));
		parlst.addParam(RichFloat(
			"Offset fraction",
			0.001,
			tr("Offset added to the surface (fraction)"),
			tr("")));
	} break;
	default: assert(0);
	}
	return parlst;
}



/**
 * @brief The Real Core Function doing the actual mesh processing.
 * @param action
 * @param md: an object containing all the meshes and rasters of MeshLab
 * @param par: the set of parameters of each filter
 * @param cb: callback object to tell MeshLab the percentage of execution of the filter
 * @return true if the filter has been applied correctly, false otherwise
 */
std::map<std::string, QVariant> FilterMeshAlphaWrap::applyFilter(
	const QAction*           action,
	const RichParameterList& par,
	MeshDocument&            md,
	unsigned int& /*postConditionMask*/,
	vcg::CallBackPos*)
{
	float alpha_fraction = par.getFloat("Alpha fraction");
	float offset_fraction = par.getFloat("Offset fraction");

	CgalMesh inputMesh;
	CgalMesh wrapResult;

	// ------- initialize the CGAL input mesh based on the contents of the MeshDocument ----------
	// 'cm' is 'current mesh' or 'compute mesh' ?
	CMeshO& m = md.mm()->cm;

	// note: coding guided from these pages:
	// http://vcglib.net/platonic_8h_source.html to read from VCGlib mesh
	// https://doc.cgal.org/latest/Surface_mesh/index.html#Chapter_3D_Surface_mesh to write to CGAL mesh

	for (auto vi = m.vert.begin(); vi != m.vert.end(); ++vi){
		vertex_descriptor vertex = inputMesh.add_vertex(K::Point_3((*vi).P().X(), (*vi).P().Y(), (*vi).P().Z()));
	}

	bool meshContainsQuads = false;
	for (auto fi = m.face.begin(); fi != m.face.end(); ++fi)
	{
		vertex_descriptor u( (*fi).V(0)->Index() );
		vertex_descriptor v( (*fi).V(1)->Index() );
		vertex_descriptor w( (*fi).V(2)->Index() );

		//qDebug() << "m.face (*fi).VN() " << (*fi).VN();
		if ( (*fi).VN() == 4)
		{
			meshContainsQuads = true;
			vertex_descriptor x( (*fi).V(3)->Index() );
			face_descriptor face = inputMesh.add_face(u, v, w, x);
		}
		else
			face_descriptor face = inputMesh.add_face(u, v, w);
	}

	CGAL::Bbox_3 bbox = CGAL::Polygon_mesh_processing::bbox(inputMesh);
	const double diag_length = std::sqrt(CGAL::square(bbox.xmax() - bbox.xmin()) +
										 CGAL::square(bbox.ymax() - bbox.ymin()) +
										 CGAL::square(bbox.zmax() - bbox.zmin()));

	switch (ID(action)) {
	case MESH_ALPHA_WRAP:

		{
		if(meshContainsQuads)
		{
			qDebug() << "Input mesh contains quads, triangulating first";
			CGAL::Polygon_mesh_processing::triangulate_faces(inputMesh);
		}

		CGAL::alpha_wrap_3(inputMesh, diag_length * alpha_fraction, diag_length * offset_fraction, wrapResult);

		// making a nice label that has the used parameter doesn't work, since it will be interpreted
		// as a file name and the UI will manipulate the string based on where the dots are.
		// QString newName = QString::asprintf("Alpha Wrap a=%.4f, o=%.4f").arg(alpha_fraction, offset_fraction);
		QString newName = "Alpha wrap";
		MeshModel* mesh = md.addNewMesh("", newName, true);

		EigenMatrixX3m   VR;
		Eigen::MatrixX3i FR;


		VR.resize(wrapResult.num_vertices(), Eigen::NoChange);
		FR.resize(wrapResult.num_faces(), Eigen::NoChange);

		// --------- copy results from wrap into Eigen matrices --------------
		// 'rvi' = result vertex iterator, vi = vertex index
		int vi = 0;
		for(auto rvi = wrapResult.vertices().begin(); rvi != wrapResult.vertices().end(); ++rvi)
		{
			VR(vi, 0) = wrapResult.point(*rvi).x();
			VR(vi, 1) = wrapResult.point(*rvi).y();
			VR(vi, 2) = wrapResult.point(*rvi).z();
			++vi;
		}

		int faceIndex = 0;
		for(auto rfi = wrapResult.faces().begin(); rfi != wrapResult.faces().end(); ++rfi)
		{
			// qDebug() << "vertices around face " << *rfi;
			CGAL::Vertex_around_face_iterator<CgalMesh> vbegin, vend;
			int i = 0;
			for(boost::tie(vbegin, vend) = vertices_around_face(wrapResult.halfedge( *rfi), wrapResult);   vbegin != vend;  ++vbegin)
			{
				// qDebug() << *vbegin << " vbegin->idx() " << vbegin->idx();
				FR(faceIndex, i) = vbegin->idx();
				++i;
			}

			++faceIndex;
		}


		// ------------- then initialize the new mesh using these Eigen matrices ----------
		mesh->cm        = meshlab::meshFromMatrices(VR, FR);
	}
	break;

	default: wrongActionCalled(action);
	}
	return std::map<std::string, QVariant>();
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeshAlphaWrap)
