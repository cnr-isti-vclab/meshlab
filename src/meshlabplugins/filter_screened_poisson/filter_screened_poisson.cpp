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

#ifdef WIN32
#include <windows.h>
#include <Psapi.h>
#endif

#include "filter_screened_poisson.h"
#include "poisson_utils.h"

FilterScreenedPoissonPlugin::FilterScreenedPoissonPlugin()
{
	typeList << FP_SCREENED_POISSON;

	for (FilterIDType tt : types()){
		actionList << new QAction(filterName(tt), this);
	}
}

FilterScreenedPoissonPlugin::~FilterScreenedPoissonPlugin()
{
	for (QAction* a : actionList){
		delete a;
	}
}

QString FilterScreenedPoissonPlugin::filterName(FilterIDType filter) const
{
	if (filter == FP_SCREENED_POISSON)
		return "Surface Reconstruction: Screened Poisson";
	else {
		return "Error!";
	}
}

QString FilterScreenedPoissonPlugin::filterInfo(FilterIDType filter) const
{
	if (filter == FP_SCREENED_POISSON)
		return "This surface reconstruction algorithm creates watertight surfaces from oriented point sets.\n"
				"The filter uses the original code of Michael Kazhdan and Matthew Bolitho implementing the algorithm described in the following paper:\n"
				"Michael Kazhdan, Hugues Hoppe,\n"
				"\"Screened Poisson surface reconstruction\"\n"
				"ACM Trans. Graphics, 32(3), 2013\n"
				"WARNING: this filter saves intermediate cache files in the \"working\" folder (last folder used when loading/saving). Be sure you are not working in a READ-ONLY location.\n";
	else {
		return "Error!";
	}
}

MeshFilterInterface::FilterClass FilterScreenedPoissonPlugin::getClass(QAction* a)
{
	if (ID(a) == FP_SCREENED_POISSON){
		return FilterScreenedPoissonPlugin::FilterClass(MeshFilterInterface::Remeshing);
	}
	else {
		assert(0);
		return MeshFilterInterface::Generic;
	}
}

int FilterScreenedPoissonPlugin::getRequirements(QAction* a)
{
	if (ID(a) == FP_SCREENED_POISSON){

	}
	else {
		assert(0);
		return 0;
	}
}

bool FilterScreenedPoissonPlugin::applyFilter(QAction* filter, MeshDocument& md, RichParameterSet& params, vcg::CallBackPos* cb)
{
	/*bool currDirChanged=false;
	QDir currDir = QDir::current();

	if (filter == "Surface Reconstruction: Screened Poisson")
	{
		//check if folder is writable
		QTemporaryFile file("./_tmp_XXXXXX.tmp");
		if (!file.open())
		{
			currDirChanged=true;
			QTemporaryDir tmpdir;
			QDir::setCurrent(tmpdir.path());
			Log("Warning - current folder is not writable. Screened Poisson Merging needs to save intermediate files in the current working folder. Project and meshes must be in a write-enabled folder. Please save your data in a suitable folder before applying.");
			//errorMessage = "current folder is not writable.<br> Screened Poisson Merging needs to save intermediate files in the current working folder.<br> Project and meshes must be in a write-enabled folder.<br> Please save your data in a suitable folder before applying.";
			//return false;
		}

		PoissonParam<Scalarm> pp;
		pp.MaxDepthVal = params.evalInt("depth");
		pp.FullDepthVal = params.evalInt("fullDepth");
		pp.CGDepthVal= params.evalInt("cgDepth");
		pp.ScaleVal = params.evalFloat("scale");
		pp.SamplesPerNodeVal = params.evalFloat("samplesPerNode");
		pp.PointWeightVal = params.evalFloat("pointWeight");
		pp.ItersVal = params.evalInt("iters");
		pp.ConfidenceFlag = params.evalBool("confidence");
		pp.DensityFlag = true;
		pp.CleanFlag = params.evalBool("preClean");

		bool goodNormal=true, goodColor=true;
		if(params.evalBool("visibleLayer") == false)
		{
			PoissonClean(md.mm()->cm, pp.ConfidenceFlag, pp.CleanFlag);
			goodNormal=HasGoodNormal(md.mm()->cm);
			goodColor = md.mm()->hasDataMask(MeshModel::MM_VERTCOLOR);
		}
		else
		{
			MeshModel *_mm=0;
			while(_mm=md.nextVisibleMesh(_mm)) {
				PoissonClean(_mm->cm,  pp.ConfidenceFlag, pp.CleanFlag);
				goodNormal &= HasGoodNormal(_mm->cm);
				goodColor  &= _mm->hasDataMask(MeshModel::MM_VERTCOLOR);
			}
		}

		if(!goodNormal)
		{
			this->errorMessage = "Filter requires correct per vertex normals.<br>"
								 "E.g. it is necessary that your <b>ALL</b> the input vertices have a proper, not-null normal.<br> "
								 "Try enabling the <i>pre-clean<i> option and retry.<br><br>"
								 "To permanently remove this problem:<br>"
								 "If you encounter this error on a triangulated mesh try to use the <i>Remove Unreferenced Vertices</i> filter"
								 "If you encounter this error on a pointcloud try to use the <i>Conditional Vertex Selection</i> filter"
								 "with function '(nx==0.0) && (ny==0.0) && (nz==0.0)', and then <i>delete selected vertices</i>.<br>";
			return false;
		}

		MeshModel *pm =md.addNewMesh("","Poisson mesh",false);
		md.setVisible(pm->id(),false);
		pm->updateDataMask(MeshModel::MM_VERTQUALITY);
		if(goodColor) pm->updateDataMask(MeshModel::MM_VERTCOLOR);

		if(params.evalBool("visibleLayer"))
		{
			Box3m bb;
			MeshModel *_mm=0;
			while(_mm=md.nextVisibleMesh(_mm))
				bb.Add(_mm->cm.Tr,_mm->cm.bbox);

			MeshDocumentPointStream<Scalarm> documentStream(md);
			_Execute<Scalarm,2,BOUNDARY_NEUMANN,PlyColorAndValueVertex<Scalarm> >(&documentStream,bb,pm->cm,pp,cb);
		}
		else
		{
			MeshModelPointStream<Scalarm> meshStream(md.mm()->cm);
			_Execute<Scalarm,2,BOUNDARY_NEUMANN,PlyColorAndValueVertex<Scalarm> >(&meshStream,md.mm()->cm.bbox,pm->cm,pp,cb);
		}
		pm->UpdateBoxAndNormals();
		md.setVisible(pm->id(),true);
		md.setCurrentMesh(pm->id());
		if(currDirChanged) QDir::setCurrent(currDir.path());
		return true;
	}
	return false;*/
}

void FilterScreenedPoissonPlugin::initParameterSet(
		QAction* filter,
		MeshModel& m,
		RichParameterSet& parent)
{
	if (ID(filter) == FP_SCREENED_POISSON) {

	}
}

int FilterScreenedPoissonPlugin::postCondition(QAction* filter) const
{
	if (ID(filter) == FP_SCREENED_POISSON){
		return MeshModel::MM_VERTNUMBER + MeshModel::MM_FACENUMBER;
	}
	else {
		return MeshModel::MM_ALL;
	}
}


MeshFilterInterface::FILTER_ARITY FilterScreenedPoissonPlugin::filterArity(QAction*) const
{
	return VARIABLE;
}

