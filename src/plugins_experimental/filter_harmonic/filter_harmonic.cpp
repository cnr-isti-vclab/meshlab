/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2013                                                \/)\/    *
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
#include "filter_harmonic.h"
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/simplex/face/topology.h>
#include <vcg/complex/algorithms/harmonic.h>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
FilterHarmonicPlugin::FilterHarmonicPlugin()
{
	typeList << FP_SCALAR_HARMONIC_FIELD;

	foreach(FilterIDType tt, types())
		actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterHarmonicPlugin::filterName(FilterIDType filterId) const
{
	switch (filterId) {
	case FP_SCALAR_HARMONIC_FIELD: return QString("Generate Scalar Harmonic Field");
	default: assert(0);
	}
	return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterHarmonicPlugin::filterInfo(FilterIDType filterId) const
{
	switch (filterId)
	{
	case FP_SCALAR_HARMONIC_FIELD: return QString("Generates a scalar harmonic field over the mesh. Scalar values must be assigned to at least two vertices as Dirichlet boundary conditions. Applying the filter, a discrete Laplace operator generates the harmonic field values for all the mesh vertices. Note that the field values is stored in the quality per vertex attribute of the mesh\n\nFor more details see:\n Kai Xua, Hao Zhang, Daniel Cohen-Or, Yueshan Xionga,'Dynamic Harmonic Fields for Surface Processing'.\nin Computers & Graphics, 2009");
	default: assert(0);
	}
	return QString("Unknown Filter");
}


// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be choosen.
FilterHarmonicPlugin::FilterClass FilterHarmonicPlugin::getClass(QAction * a)
{
	switch (ID(a))
	{
	case FP_SCALAR_HARMONIC_FIELD: return MeshFilterInterface::Remeshing;
	default: assert(0);
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)

void FilterHarmonicPlugin::initParameterSet(QAction * action, MeshModel & m, RichParameterSet & parlst)
{
	switch (ID(action))
	{
	case FP_SCALAR_HARMONIC_FIELD:
		parlst.addParam(new RichPoint3f("point1", m.cm.bbox.min, "Point 1", "A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichPoint3f("point2", m.cm.bbox.max, "Point 2", "A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichDynamicFloat("value1", 0.0f, 0.0f, 1.0f, "value for the 1st point", "Harmonic field value for the vertex."));
		parlst.addParam(new RichDynamicFloat("value2", 1.0f, 0.0f, 1.0f, "value for the 2nd point", "Harmonic field value for the vertex."));
		parlst.addParam(new RichBool("colorize", true, "Colorize", "Colorize the mesh to provide an indication of the obtained harmonic field."));
		break;
	default: assert(0);
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterHarmonicPlugin::applyFilter(QAction * action, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb)
{
	switch (ID(action))
	{
	case FP_SCALAR_HARMONIC_FIELD:
	{

		typedef double FieldScalar;
		// double is fine

		cb(1, "Computing harmonic field...");

		CMeshO & m = md.mm()->cm;
		vcg::tri::Allocator<CMeshO>::CompactEveryVector(m);

		int numCC = vcg::tri::Clean<CMeshO>::CountConnectedComponents(m);
		if (numCC > 1)
		{
			this->errorMessage = "A mesh composed by a single connected component is required by the filter to properly work.";
			return false;
		}

		if (vcg::tri::Clean<CMeshO>::CountNonManifoldEdgeFF(md.mm()->cm) > 0)
		{
			errorMessage = "Mesh has some not 2-manifold faces, this filter requires manifoldness";
			return false;
		}

		if (vcg::tri::Clean<CMeshO>::CountNonManifoldVertexFF(md.mm()->cm) > 0)
		{
			errorMessage = "Mesh has some not 2-manifold vertices, this filter requires manifoldness";
			return false;
		}

		md.mm()->updateDataMask(MeshModel::MM_VERTMARK | MeshModel::MM_FACEMARK | MeshModel::MM_FACEFLAG);
		// Get the two vertices with value set
		vcg::GridStaticPtr<CVertexO, Scalarm> vg;
		vg.Set(m.vert.begin(), m.vert.end());

		vcg::vertex::PointDistanceFunctor<Scalarm> pd;
		vcg::tri::Tmark<CMeshO, CVertexO> mv;
		mv.SetMesh(&m);
		mv.UnMarkAll();
		Point3m  closestP;
		Scalarm minDist = 0;
		CVertexO * vp0 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point1"), m.bbox.Diag(), minDist, closestP);
		CVertexO * vp1 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point2"), m.bbox.Diag(), minDist, closestP);
		if (vp0 == NULL || vp1 == NULL || vp0 == vp1)
		{
			this->errorMessage = "Error occurred for selected points.";
			return false;
		}

		vcg::tri::Harmonic<CMeshO, FieldScalar>::ConstraintVec constraints;
		constraints.push_back(vcg::tri::Harmonic<CMeshO, FieldScalar>::Constraint(vp0, FieldScalar(par.getFloat("value1"))));
		constraints.push_back(vcg::tri::Harmonic<CMeshO, FieldScalar>::Constraint(vp1, FieldScalar(par.getFloat("value2"))));

		CMeshO::PerVertexAttributeHandle<FieldScalar> handle = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<FieldScalar>(m, "harmonic");

		bool ok = vcg::tri::Harmonic<CMeshO, FieldScalar>::ComputeScalarField(m, constraints, handle);

		if (!ok)
		{
			this->errorMessage += "An error occurred.";
			return false;
		}
		md.mm()->updateDataMask(MeshModel::MM_VERTQUALITY);
		for (auto vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			vi->Q() = handle[vi];

		if (par.getBool("colorize"))
		{
			md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
			vcg::tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m);
		}

		cb(100, "Done.");

		return true;
	}
	default: assert(0);
	}
	return false;
}

QString FilterHarmonicPlugin::filterScriptFunctionName(FilterIDType filterID)
{
	switch (filterID)
	{
	case FP_SCALAR_HARMONIC_FIELD: return QString("scalarHarmonicField");
	default: assert(0);
	}
	return QString();
}

MeshFilterInterface::FILTER_ARITY FilterHarmonicPlugin::filterArity(QAction *act) const
{
	switch (ID(act))
	{
	case FP_SCALAR_HARMONIC_FIELD: return MeshFilterInterface::SINGLE_MESH;
	}
	return MeshFilterInterface::NONE;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterHarmonicPlugin)
