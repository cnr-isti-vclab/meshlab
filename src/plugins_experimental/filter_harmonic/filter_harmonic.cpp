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

	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterHarmonicPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
	case FP_SCALAR_HARMONIC_FIELD : return QString("Generate Scalar Harmonic Field");
	default : assert(0);
	}
	return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterHarmonicPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId)
	{
	case FP_SCALAR_HARMONIC_FIELD : return QString("Generates a scalar harmonic field over the mesh. Scalar values must be assigned to at least two vertices as Dirichlet boundary conditions. Applying the filter, a discrete Laplace operator generates the harmonic field values for all the mesh vertices. Note that the field values is stored in the quality per vertex attribute of the mesh\n\nFor more details see:\n Kai Xua, Hao Zhang, Daniel Cohen-Or, Yueshan Xionga,'Dynamic Harmonic Fields for Surface Processing'.\nin Computers & Graphics, 2009");
	default : assert(0);
	}
	return QString("Unknown Filter");
}


// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be choosen.
FilterHarmonicPlugin::FilterClass FilterHarmonicPlugin::getClass(QAction * a)
{
	switch(ID(a))
	{
	case FP_SCALAR_HARMONIC_FIELD : return MeshFilterInterface::Remeshing;
	default : assert(0);
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
	switch(ID(action))
	{
	case FP_SCALAR_HARMONIC_FIELD :
		parlst.addParam(new RichPoint3f("point1", m.cm.bbox.min,"Point 1","A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichPoint3f("point2", m.cm.bbox.min,"Point 2","A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichDynamicFloat("value1", 0.0f, 0.0f, 1.0f,"value for the 1st point","Harmonic field value for the vertex."));
		parlst.addParam(new RichDynamicFloat("value2", 1.0f, 0.0f, 1.0f,"value for the 2nd point","Harmonic field value for the vertex."));
		parlst.addParam(new RichBool("colorize", true, "Colorize", "Colorize the mesh to provide an indication of the obtained harmonic field."));
		break;
	default : assert(0);
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterHarmonicPlugin::applyFilter(QAction * action, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb)
{
	switch(ID(action))
	{
	case FP_SCALAR_HARMONIC_FIELD :
	{
		typedef vcg::GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType> VertexGrid;

		typedef double                           FieldScalar;
		// double is fine

		typedef CMeshO::ScalarType               ScalarType;
		typedef CMeshO::CoordType                CoordType;
		typedef CMeshO::VertexType               VertexType;
		typedef CMeshO::FaceType                 FaceType;

		cb(1, "Computing harmonic field...");

		CMeshO & m = md.mm()->cm;

		vcg::tri::Allocator<CMeshO>::CompactFaceVector(m);
		vcg::tri::Allocator<CMeshO>::CompactVertexVector(m);

		md.mm()->updateDataMask(MeshModel::MM_VERTMARK | MeshModel::MM_FACEFLAG);
		vcg::tri::UpdateBounding<CMeshO>::Box(m);

		int n  = m.VN();

		// Get the two vertices with value set
		VertexGrid vg;
		vg.Set(m.vert.begin(), m.vert.end());

		vcg::vertex::PointDistanceFunctor<ScalarType> pd;
		vcg::tri::Tmark<CMeshO, VertexType> mv;
		mv.SetMesh(&m);
		mv.UnMarkAll();
		CoordType  closestP;
		ScalarType minDist = 0;
		VertexType * vp0 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point1"), m.bbox.Diag(), minDist, closestP);
		VertexType * vp1 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point2"), m.bbox.Diag(), minDist, closestP);
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

		// Colorize bands for the 0-1 interval
		if (par.getBool("colorize"))
		{
			md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
			float steps = 20.0f;
			for (size_t i = 0; int(i) < n; ++i)
			{
				bool on = (int)(handle[i]*steps)%2 == 1;
				if (on)
				{
					m.vert[i].C() = vcg::Color4b::ColorRamp(0,2,handle[i]);
				}
				else
				{
					m.vert[i].C() = vcg::Color4b::White;
				}
			}
		}

		cb(100, "Done.");

		return true;
	}
	default : assert(0);
	}
	return false;
}

QString FilterHarmonicPlugin::filterScriptFunctionName( FilterIDType filterID )
{
	switch(filterID)
	{
	case FP_SCALAR_HARMONIC_FIELD : return QString("scalarHarmonicField");
	default : assert(0);
	}
	return QString();
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterHarmonicPlugin)
