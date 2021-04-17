/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
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

#include "meshfilter_M.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/hole.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <vcg/complex/algorithms/bitquad_creation.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/complex/algorithms/attribute_seam.h>
#include <vcg/complex/algorithms/update/curvature.h>
#include <vcg/complex/algorithms/update/curvature_fitting.h>
#include <vcg/complex/algorithms/pointcloud_normal.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/space/fitting3.h>
#include <wrap/gl/glu_tessellator_cap.h>

using namespace std;
using namespace vcg;


ExtraMeshFilterPlugin::ExtraMeshFilterPlugin(void)
{
	typeList = {
		FP_CLOSE_HOLES_M
	};

	for (ActionIDType tt : types())
	{
		actionList.push_back(new QAction(filterName(tt), this));

		if (tt == FP_CLOSE_HOLES_M) {
			//actionList.last()->setShortcut(QKeySequence ("Ctrl+Del"));
			actionList.last()->setIcon(QIcon(":/images/TestBezier.png"));
		}
	}
}

QString ExtraMeshFilterPlugin::pluginName() const
{
	return "FilterMeshing_M";
}

ExtraMeshFilterPlugin::FilterClass ExtraMeshFilterPlugin::getClass(const QAction * a) const
{
	switch (ID(a))
	{
	case FP_CLOSE_HOLES_M                      : return FilterPlugin::Remeshing;

	default                                  : assert(0); return FilterPlugin::Generic;
	}

	return FilterPlugin::Generic;
}

int ExtraMeshFilterPlugin::getPreConditions(const QAction *filter) const
{
	switch (ID(filter))
	{
	case FP_CLOSE_HOLES_M                      : return MeshModel::MM_FACENUMBER;
	}
	return MeshModel::MM_NONE;
}

QString ExtraMeshFilterPlugin::filterName(ActionIDType filter) const
{
	switch (filter)
	{
	case FP_CLOSE_HOLES_M                      : return tr("Close Holes_M");

	default                                  : assert(0);
	}

	return tr("error!");
}

QString ExtraMeshFilterPlugin::filterInfo(ActionIDType filterID) const
{
	switch (filterID)
	{
	case FP_CLOSE_HOLES_M                        : return tr("Close holes smaller than a given threshold");
	
	default                                  : assert(0);
	}

	return QString();
}

// this function builds and initializes with the default values (that can depend on the current mesh or selection)
// the list of parameters that a filter requires.
// return
//		true if has some parameters
//		false is has no params
void ExtraMeshFilterPlugin::initParameterList(const QAction * action, MeshModel & m, RichParameterList & parlst)
{
	float maxVal;
	QStringList curvCalcMethods;
	QStringList curvColorMethods;
	QStringList loopWeightLst;

	switch(ID(action))
	{
	
	case FP_CLOSE_HOLES_M:
		parlst.addParam(RichInt ("MaxHoleSize",(int)30,"Max size to be closed ","The size is expressed as number of edges composing the hole boundary"));
		parlst.addParam(RichBool("Selected",m.cm.sfn>0,"Close holes with selected faces","Only the holes with at least one of the boundary faces selected are closed"));
		parlst.addParam(RichBool("NewFaceSelected",true,"Select the newly created faces","After closing a hole the faces that have been created are left selected. Any previous selection is lost. Useful for example for smoothing the newly created holes."));
		parlst.addParam(RichBool("SelfIntersection",true,"Prevent creation of selfIntersecting faces","When closing an holes it tries to prevent the creation of faces that intersect faces adjacent to the boundary of the hole. It is an heuristic, non intersetcting hole filling can be NP-complete."));
		break;


	default:
		break;
	}
}


void Freeze(MeshModel *m)
{
	tri::UpdatePosition<CMeshO>::Matrix(m->cm, m->cm.Tr,true);
	tri::UpdateBounding<CMeshO>::Box(m->cm);
	m->cm.shot.ApplyRigidTransformation(m->cm.Tr);
	m->cm.Tr.SetIdentity();
}

void ApplyTransform(MeshDocument &md, const Matrix44m &tr, bool toAllFlag, bool freeze,
					bool invertFlag=false, bool composeFlage=true)
{
	if(toAllFlag)
	{
		MeshModel   *m=NULL;
		while ((m=md.nextVisibleMesh(m)))
		{
			if(invertFlag) m->cm.Tr = Inverse(m->cm.Tr);
			if(composeFlage) m->cm.Tr = tr * m->cm.Tr;
			else m->cm.Tr=tr;
			if(freeze) Freeze(m);
		}

		for (int i = 0; i < md.rasterList.size(); i++)
			if (md.rasterList[0]->visible)
				md.rasterList[i]->shot.ApplyRigidTransformation(tr);
	}
	else
	{
		MeshModel   *m=md.mm();
		if(invertFlag) m->cm.Tr = Inverse(m->cm.Tr);
		if(composeFlage) m->cm.Tr = tr * m->cm.Tr;
		else m->cm.Tr=tr;
		if(freeze) Freeze(md.mm());
	}
}


std::map<std::string, QVariant> ExtraMeshFilterPlugin::applyFilter(
		const QAction * filter,
		const RichParameterList & par,
		MeshDocument & md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos * cb)
{
	MeshModel & m = *md.mm();

	switch(ID(filter))
	{

	case FP_CLOSE_HOLES_M:
	{
		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) > 0){
			throw MLException("Mesh has some not 2-manifold edges, filter requires edge manifoldness");
		}

		size_t OriginalSize= m.cm.face.size();
		int MaxHoleSize = par.getInt("MaxHoleSize");
		bool SelectedFlag = par.getBool("Selected");
		bool SelfIntersectionFlag = par.getBool("SelfIntersection");
		bool NewFaceSelectedFlag = par.getBool("NewFaceSelected");
		int holeCnt;
		if( SelfIntersectionFlag )
			holeCnt = tri::Hole<CMeshO>::EarCuttingIntersectionFill<tri::SelfIntersectionEar< CMeshO> >(m.cm,MaxHoleSize,SelectedFlag,cb);
		else
			holeCnt = tri::Hole<CMeshO>::EarCuttingFill<vcg::tri::MinimumWeightEar< CMeshO> >(m.cm,MaxHoleSize,SelectedFlag,cb);
		log("Closed %i holes and added %i new faces",holeCnt,m.cm.fn-OriginalSize);
		assert(tri::Clean<CMeshO>::IsFFAdjacencyConsistent(m.cm));
		m.UpdateBoxAndNormals();

		// hole filling filter does not correctly update the border flags (but the topology is still ok!)
		if(NewFaceSelectedFlag)
		{
			tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
			for(size_t i=OriginalSize;i<m.cm.face.size();++i)
				if(!m.cm.face[i].IsD()) m.cm.face[i].SetS();
		}
	} break;

	
	default:
		wrongActionCalled(filter);
	}
	return std::map<std::string, QVariant>();
}

int ExtraMeshFilterPlugin::postCondition(const QAction * filter) const
{
	switch (ID(filter))
	{
	case FP_CLOSE_HOLES_M : return MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE;

	default                  : return MeshModel::MM_ALL;
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(ExtraMeshFilterPlugin)
