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

#include <QDir>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include "filter_screened_poisson.h"
#include "poisson_utils.h"

FilterScreenedPoissonPlugin::FilterScreenedPoissonPlugin()
{
	typeList = {FP_SCREENED_POISSON};

	for (ActionIDType tt : types()){
		actionList.push_back(new QAction(filterName(tt), this));
	}
}

FilterScreenedPoissonPlugin::~FilterScreenedPoissonPlugin()
{
}

QString FilterScreenedPoissonPlugin::pluginName() const
{
	return "FilterScreenedPoisson";
}

QString FilterScreenedPoissonPlugin::filterName(ActionIDType filter) const
{
	if (filter == FP_SCREENED_POISSON)
		return "Surface Reconstruction: Screened Poisson";
	else {
		return "Error!";
	}
}

QString FilterScreenedPoissonPlugin::filterInfo(ActionIDType filter) const
{
	if (filter == FP_SCREENED_POISSON)
		return	"This surface reconstruction algorithm creates watertight surfaces "
				"from oriented point sets.<br>"
				"The filter uses the original code of Michael Kazhdan and Matthew Bolitho "
				"implementing the algorithm described in the following paper:<br>"
				"<i>Michael Kazhdan, Hugues Hoppe</i>,<br>"
				"<b>\"Screened Poisson surface reconstruction\"</b><br>";
	else {
		return "Error!";
	}
}

FilterPlugin::FilterClass FilterScreenedPoissonPlugin::getClass(const QAction* a) const
{
	if (ID(a) == FP_SCREENED_POISSON){
		return FilterPlugin::Remeshing;
	}
	else {
		assert(0);
		return FilterPlugin::Generic;
	}
}

int FilterScreenedPoissonPlugin::getRequirements(const QAction* a)
{
	if (ID(a) == FP_SCREENED_POISSON) {
		return MeshModel::MM_NONE;
	}
	else {
		assert(0);
		return 0;
	}
}

std::map<std::string, QVariant> FilterScreenedPoissonPlugin::applyFilter(
		const QAction* filter,
		const RichParameterList& params,
		MeshDocument& md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos* cb)
{
	bool currDirChanged=false;
	QDir currDir = QDir::current();

	if (ID(filter) == FP_SCREENED_POISSON) {
		//Using tmp dir
		QTemporaryDir tmpdir;
		QTemporaryFile file(tmpdir.path());
		if (!file.open()) { //if a file cannot be created in the tmp folder
			log("Warning - tmp folder is not writable.");

			QTemporaryFile file2("./_tmp_XXXXXX.tmp"); //try to create a file in the meshlab folder
			if (!file2.open()){ //if a file cannot be created in the tmp and in the meshlab folder, we cannot run the filter
				log("Warning - current folder is not writable. Screened Poisson Merging needs to save intermediate files in the tmp working folder. Project and meshes must be in a write-enabled folder. Please save your data in a suitable folder before applying.");
				throw MLException("current and tmp folder are not writable.<br> Screened Poisson Merging needs to save intermediate files in the current working folder.<br> Project and meshes must be in a write-enabled folder.<br> Please save your data in a suitable folder before applying.");
			}
		}
		else { //if the tmp folder is writable, we will use it
			currDirChanged=true;
			QDir::setCurrent(tmpdir.path());
		}

		PoissonParam<Scalarm> pp;
		pp.MaxDepthVal = params.getInt("depth");
		pp.FullDepthVal = params.getInt("fullDepth");
		pp.CGDepthVal= params.getInt("cgDepth");
		pp.ScaleVal = params.getFloat("scale");
		pp.SamplesPerNodeVal = params.getFloat("samplesPerNode");
		pp.PointWeightVal = params.getFloat("pointWeight");
		pp.ItersVal = params.getInt("iters");
		pp.ConfidenceFlag = params.getBool("confidence");
		pp.DensityFlag = true;
		pp.CleanFlag = params.getBool("preClean");

		bool goodNormal=true, goodColor=true;
		if(params.getBool("visibleLayer") == false) {
			PoissonClean(md.mm()->cm, pp.ConfidenceFlag, pp.CleanFlag);
			goodNormal=HasGoodNormal(md.mm()->cm);
			goodColor = md.mm()->hasDataMask(MeshModel::MM_VERTCOLOR);
		}
		else {
			MeshModel *_mm=md.nextVisibleMesh();
			while(_mm != nullptr) {
				PoissonClean(_mm->cm,  pp.ConfidenceFlag, pp.CleanFlag);
				goodNormal &= HasGoodNormal(_mm->cm);
				goodColor  &= _mm->hasDataMask(MeshModel::MM_VERTCOLOR);
				_mm=md.nextVisibleMesh(_mm);
			}
		}

		if(!goodNormal) {
			throw MLException("Filter requires correct per vertex normals.<br>"
								 "E.g. it is necessary that your <b>ALL</b> the input vertices have a proper, not-null normal.<br> "
								 "Try enabling the <i>pre-clean<i> option and retry.<br><br>"
								 "To permanently remove this problem:<br>"
								 "If you encounter this error on a triangulated mesh try to use the <i>Remove Unreferenced Vertices</i> filter"
								 "If you encounter this error on a pointcloud try to use the <i>Conditional Vertex Selection</i> filter"
								 "with function '(nx==0.0) && (ny==0.0) && (nz==0.0)', and then <i>delete selected vertices</i>.<br>");
		}

		MeshModel *pm =md.addNewMesh("","Poisson mesh",false);
		md.setVisible(pm->id(),false);
		pm->updateDataMask(MeshModel::MM_VERTQUALITY);
		if(goodColor)
			pm->updateDataMask(MeshModel::MM_VERTCOLOR);

		if(params.getBool("visibleLayer")) {
			Box3m bb;
			MeshModel *_mm=md.nextVisibleMesh();
			while(_mm != nullptr){
				bb.Add(_mm->cm.Tr,_mm->cm.bbox);
				_mm=md.nextVisibleMesh(_mm);
			}

			MeshDocumentPointStream<Scalarm> documentStream(md);
			_Execute<Scalarm,2,BOUNDARY_NEUMANN,PlyColorAndValueVertex<Scalarm> >(&documentStream,bb,pm->cm,pp,cb);
		}
		else {
			MeshModelPointStream<Scalarm> meshStream(md.mm()->cm);
			_Execute<Scalarm,2,BOUNDARY_NEUMANN,PlyColorAndValueVertex<Scalarm> >(&meshStream,md.mm()->cm.bbox,pm->cm,pp,cb);
		}
		pm->UpdateBoxAndNormals();
		md.setVisible(pm->id(),true);
		md.setCurrentMesh(pm->id());
		if(currDirChanged)
			QDir::setCurrent(currDir.path());
	}
	else {
		wrongActionCalled(filter);
	}
	return std::map<std::string, QVariant>();
}

void FilterScreenedPoissonPlugin::initParameterList(
		const QAction* filter,
		MeshModel&,
		RichParameterList& parlist)
{
	if (ID(filter) == FP_SCREENED_POISSON) {
		parlist.addParam(RichBool("visibleLayer", false, "Merge all visible layers", "Enabling this flag means that all the visible layers will be used for providing the points."));
		parlist.addParam(RichInt("depth", 8, "Reconstruction Depth", "This integer is the maximum depth of the tree that will be used for surface reconstruction. Running at depth d corresponds to solving on a voxel grid whose resolution is no larger than 2^d x 2^d x 2^d. Note that since the reconstructor adapts the octree to the sampling density, the specified reconstruction depth is only an upper bound. The default value for this parameter is 8."));
		parlist.addParam(RichInt("fullDepth", 5, "Adaptive Octree Depth", "This integer specifies the depth beyond depth the octree will be adapted. At coarser depths, the octree will be complete, containing all 2^d x 2^d x 2^d nodes. The default value for this parameter is 5."));
		parlist.addParam(RichInt("cgDepth", 0, "Conjugate Gradients Depth", "This integer is the depth up to which a conjugate-gradients solver will be used to solve the linear system. Beyond this depth Gauss-Seidel relaxation will be used. The default value for this parameter is 0."));
		parlist.addParam(RichFloat("scale", 1.1, "Scale Factor", "This floating point value specifies the ratio between the diameter of the cube used for reconstruction and the diameter of the samples' bounding cube. The default value is 1.1."));
		parlist.addParam(RichFloat("samplesPerNode", 1.5, "Minimum Number of Samples", "This floating point value specifies the minimum number of sample points that should fall within an octree node as the octree construction is adapted to sampling density. For noise-free samples, small values in the range [1.0 - 5.0] can be used. For more noisy samples, larger values in the range [15.0 - 20.0] may be needed to provide a smoother, noise-reduced, reconstruction. The default value is 1.5."));
		parlist.addParam(RichFloat("pointWeight", 4, "Interpolation Weight", "This floating point value specifies the importants that interpolation of the point samples is given in the formulation of the screened Poisson equation. The results of the original (unscreened) Poisson Reconstruction can be obtained by setting this value to 0. The default value for this parameter is 4."));
		parlist.addParam(RichInt("iters", 8, "Gauss-Seidel Relaxations", "This integer value specifies the number of Gauss-Seidel relaxations to be performed at each level of the hierarchy. The default value for this parameter is 8."));
		parlist.addParam(RichBool("confidence", false, "Confidence Flag", "Enabling this flag tells the reconstructor to use the quality as confidence information; this is done by scaling the unit normals with the quality values. When the flag is not enabled, all normals are normalized to have unit-length prior to reconstruction."));
		parlist.addParam(RichBool("preClean", false, "Pre-Clean", "Enabling this flag force a cleaning pre-pass on the data removing all unreferenced vertices or vertices with null normals."));
	}
}

int FilterScreenedPoissonPlugin::postCondition(const QAction* filter) const
{
	if (ID(filter) == FP_SCREENED_POISSON){
		return MeshModel::MM_VERTNUMBER + MeshModel::MM_FACENUMBER;
	}
	else {
		return MeshModel::MM_ALL;
	}
}


FilterPlugin::FilterArity FilterScreenedPoissonPlugin::filterArity(const QAction*) const
{
	return VARIABLE;
}

