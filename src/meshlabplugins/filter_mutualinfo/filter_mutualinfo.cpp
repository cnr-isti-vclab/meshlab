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

#include "filter_mutualinfo.h"
#include <common/GLExtensionsManager.h>

#include "alignset.h"

#include "solver.h"
#include "mutual.h"

#include <wrap/gl/shot.h>
#include <wrap/gl/camera.h>

#include <vcg/complex/algorithms/point_sampling.h>

FilterMutualInfoPlugin::FilterMutualInfoPlugin() 
{
	typeList << FP_IMAGE_MUTUALINFO;

	for(FilterIDType tt : types())
		actionList << new QAction(filterName(tt), this);
}

QString FilterMutualInfoPlugin::pluginName() const
{
	return "FilterMutualInfo";
}

QString FilterMutualInfoPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
	case FP_IMAGE_MUTUALINFO:
		return "Image alignment: Mutual Information";
	default :
		assert(0);
		return "";
	}
}

QString FilterMutualInfoPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
	case FP_IMAGE_MUTUALINFO:
		return "Register an image on a 3D model using Mutual Information. This filter is an implementation of Corsini et al. 'Image-to-geometry registration: a mutual information method exploiting illumination-related geometric properties', 2009, <a href=\"http://vcg.isti.cnr.it/Publications/2009/CDPS09/\" target=\"_blank\">Get link</a>";
	default :
		assert(0);
		return "Unknown Filter";
	}
}

FilterMutualInfoPlugin::FilterClass FilterMutualInfoPlugin::getClass(const QAction *a) const
{
	switch(ID(a)) {
	case FP_IMAGE_MUTUALINFO:
		return FilterPluginInterface::Camera;
	default :
		assert(0);
		return FilterPluginInterface::Generic;
	}
}

FilterPluginInterface::FILTER_ARITY FilterMutualInfoPlugin::filterArity(const QAction*) const
{
	return SINGLE_MESH;
}

void FilterMutualInfoPlugin::initParameterList(const QAction *action,MeshDocument & /*md*/, RichParameterList & parlst)
{
	QStringList rendList;
	rendList.push_back("Combined");
	rendList.push_back("Normal map");
	rendList.push_back("Color per vertex");
	rendList.push_back("Specular");
	rendList.push_back("Silhouette");
	rendList.push_back("Specular combined");
	switch(ID(action))	 {
	case FP_IMAGE_MUTUALINFO:
		parlst.addParam(RichEnum("Rendering Mode", 0, rendList, tr("Rendering mode:"), "Rendering modes"));
		parlst.addParam(RichShotf("Shot", vcg::Shotf(), "Starting shot", "If the point of view has been set by hand, it must be retrieved from current trackball"));
		parlst.addParam(RichBool("Estimate Focal", false, "Estimate focal length", "Estimate focal length: if not checked, only extrinsic parameters are estimated"));
		parlst.addParam(RichBool("Fine", true, "Fine Alignment", "Fine alignment: the perturbations applied to reach the alignment are smaller"));
		parlst.addParam(RichInt("NumOfIterations", 100, "Max iterations", "Maximum number of iterations"));
		parlst.addParam(RichFloat("Tolerance", 0.1, "Tolerance", "Threshold to stop convergence"));
		parlst.addParam(RichFloat("ExpectedVariance", 2.0, "Expected Variance", "Expected Variance"));
		parlst.addParam(RichInt("BackgroundWeight", 2, "Background Weight", "Weight of background pixels (1, as all the other pixels; 2, one half of the other pixels etc etc)"));
		break;
	default :
		assert(0);
	}
}

bool FilterMutualInfoPlugin::applyFilter(const QAction *action, MeshDocument &md, unsigned int& /*postConditionMask*/, const RichParameterList & par, vcg::CallBackPos* )
{
	switch(ID(action))	 {
	case FP_IMAGE_MUTUALINFO :
		return imageMutualInfoAlign(
					md,
					par.getEnum("Rendering Mode"), par.getBool("Estimate Focal"),
					par.getBool("Fine"), par.getFloat("ExpectedVariance"),
					par.getFloat("Tolerance"), par.getInt("NumOfIterations"),
					par.getInt("BackgroundWeight"), par.getShotf("Shot"));
		break;
	default :
		assert(0);
		return false;
	}
}

int FilterMutualInfoPlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_NONE;
}

bool FilterMutualInfoPlugin::imageMutualInfoAlign(
		MeshDocument& md,
		int rendmode,
		bool estimateFocal,
		bool fine,
		float expectedVariance,
		float tolerance,
		int numIterations,
		int backGroundWeight,
		vcg::Shotf shot)
{
	Solver solver;
	MutualInfo mutual;
	if (!shot.IsValid()){
		log(GLLogStream::FILTER, "Error: shot not valid. Press 'Get Shot' button before applying!");
		return false;
	}

	if (md.rasterList.size()==0) {
		log(GLLogStream::FILTER, "You need a Raster Model to apply this filter!");
		return false;
	}
	else {
		align.image=&md.rm()->currentPlane->image;
	}

	align.mesh=&md.mm()->cm;
	align.meshid = md.mm()->id();

	solver.optimize_focal = estimateFocal;
	solver.fine_alignment = fine;
	solver.variance = expectedVariance;
	solver.tolerance = tolerance;
	solver.maxiter = numIterations;
	mutual.bweight = backGroundWeight;

	switch(rendmode)
	{
	case 0:
		align.mode=AlignSet::COMBINE;
		break;
	case 1:
		align.mode=AlignSet::NORMALMAP;
		break;
	case 2:
		align.mode=AlignSet::COLOR;
		break;
	case 3:
		align.mode=AlignSet::SPECULAR;
		break;
	case 4:
		align.mode=AlignSet::SILHOUETTE;
		break;
	case 5:
		align.mode=AlignSet::SPECAMB;
		break;
	default:
		align.mode=AlignSet::COMBINE;
		break;
	}

	align.shot = vcg::Shotf::Construct(shot);

	align.shot.Intrinsics.ViewportPx[0]=int((double)align.shot.Intrinsics.ViewportPx[1]*align.image->width()/align.image->height());
	align.shot.Intrinsics.CenterPx[0]=(int)(align.shot.Intrinsics.ViewportPx[0]/2);

	///// Initialize GLContext

	log( "Initialize GL");
	align.setGLContext(glContext);
	glContext->makeCurrent();
	if (initGLMutualInfo() == false)
		return false;

	log( "Done");

	///// Mutual info calculation: every 30 iterations, the mail glarea is updated
	int rounds=(int)(solver.maxiter/30);
	for (int i=0; i<rounds; i++)
	{
		log( "Step %i of %i.", i+1, rounds );

		solver.maxiter=30;

		if (solver.fine_alignment)
			solver.optimize(&align, &mutual, align.shot);
		else
			solver.iterative(&align, &mutual, align.shot);

		md.rm()->shot = Shotm::Construct(align.shot);
		float ratio=(float)md.rm()->currentPlane->image.height()/(float)align.shot.Intrinsics.ViewportPx[1];
		md.rm()->shot.Intrinsics.ViewportPx[0]=md.rm()->currentPlane->image.width();
		md.rm()->shot.Intrinsics.ViewportPx[1]=md.rm()->currentPlane->image.height();
		md.rm()->shot.Intrinsics.PixelSizeMm[1]/=ratio;
		md.rm()->shot.Intrinsics.PixelSizeMm[0]/=ratio;
		md.rm()->shot.Intrinsics.CenterPx[0]=(int)((float)md.rm()->shot.Intrinsics.ViewportPx[0]/2.0);
		md.rm()->shot.Intrinsics.CenterPx[1]=(int)((float)md.rm()->shot.Intrinsics.ViewportPx[1]/2.0);

		QList<int> rl;
		rl << md.rm()->id();

		//md.updateRenderStateRasters(rl,RasterModel::RM_ALL);

		md.documentUpdated();
	}
	this->glContext->doneCurrent();

	return true;
}

bool FilterMutualInfoPlugin::initGLMutualInfo()
{
	log(0, "GL Initialization");
	if (!GLExtensionsManager::initializeGLextensions_notThrowing()) {
		log(0, "GLEW initialization error!");
		return false;
	}

	if (!glewIsSupported("GL_EXT_framebuffer_object")) {
		log(0, "Graphics hardware does not support FBOs");
		return false;
	}
	if (!glewIsSupported("GL_ARB_vertex_shader") || !glewIsSupported("GL_ARB_fragment_shader") ||
			!glewIsSupported("GL_ARB_shader_objects") || !glewIsSupported("GL_ARB_shading_language")) {
		//QMessageBox::warning(this, "Danger, Will Robinson!",
		//                         "Graphics hardware does not fully support Shaders");
	}

	if (!glewIsSupported("GL_ARB_texture_non_power_of_two")) {
		log(0,"Graphics hardware does not support non-power-of-two textures");
		return false;
	}
	if (!glewIsSupported("GL_ARB_vertex_buffer_object")) {
		log(0, "Graphics hardware does not support vertex buffer objects");
		return false;
	}

	glEnable(GL_NORMALIZE);
	glDepthRange (0.0, 1.0);

	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);

	//AlignSet &align = Autoreg::instance().align;
	align.initializeGL();
	align.resize(800);
	//assert(glGetError() == 0);

	log(0, "GL Initialization done");
	return true;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterMutualInfoPlugin)
