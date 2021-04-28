/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#include "load_save.h"

#include <QElapsedTimer>
#include <QDir>

#include "../globals.h"
#include "../plugins/plugin_manager.h"

namespace meshlab {

void loadMesh(const QString& fileName, IOPlugin* ioPlugin, const RichParameterList& prePar, const std::list<MeshModel*>& meshList, std::list<int>& maskList, vcg::CallBackPos *cb)
{
	QFileInfo fi(fileName);
	QString extension = fi.suffix();

	// the original directory path before we switch it
	QString origDir = QDir::current().path();

	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());

	// Adjust the file name after changing the directory
	QString fileNameSansDir = fi.fileName();

	try {
		ioPlugin->open(extension, fileNameSansDir, meshList ,maskList, prePar, cb);
	}
	catch(const MLException& e) {
		QDir::setCurrent(origDir); // undo the change of directory before leaving
		throw e;
	}

	auto itmesh = meshList.begin();
	auto itmask = maskList.begin();
	for (unsigned int i = 0; i < meshList.size(); ++i){
		MeshModel* mm = *itmesh;
		int mask = *itmask;

		// In case of polygonal meshes the normal should be updated accordingly
		if( mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL) {
			mm->updateDataMask(MeshModel::MM_POLYGONAL); // just to be sure. Hopefully it should be done in the plugin...
			int degNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
			if(degNum)
				ioPlugin->log("Warning model contains " + std::to_string(degNum) +" degenerate faces. Removed them.");
			mm->updateDataMask(MeshModel::MM_FACEFACETOPO);
			vcg::tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(mm->cm);
			vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mm->cm);
		} // standard case
		else {
			vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mm->cm);
			if(!( mask & vcg::tri::io::Mask::IOM_VERTNORMAL) )
				vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mm->cm);
		}

		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		if(mm->cm.fn==0 && mm->cm.en==0) {
			if(mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
				mm->updateDataMask(MeshModel::MM_VERTNORMAL);
		}

		if(mm->cm.fn==0 && mm->cm.en>0) {
			if (mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
				mm->updateDataMask(MeshModel::MM_VERTNORMAL);
		}

		//updateMenus();
		int delVertNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateVertex(mm->cm);
		int delFaceNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
		vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
		if(delVertNum>0 || delFaceNum>0 )
			ioPlugin->reportWarning(QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum));

		//computeRenderingDataOnLoading(mm,isareload, rendOpt);
		++itmesh;
		++itmask;
	}
	QDir::setCurrent(origDir); // undo the change of directory before leaving
}


void loadMeshWithStandardParameters(const QString& filename, MeshDocument& md, vcg::CallBackPos *cb)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.inputMeshPlugin(extension);

	if (ioPlugin == nullptr)
		throw MLException(
				"Mesh " + filename + " cannot be opened. Your MeshLab version "
				"has not plugin to read " + extension + " file format");


	RichParameterList prePar;
	ioPlugin->initPreOpenParameter(extension, prePar);
	prePar.join(meshlab::defaultGlobalParameterList());


	unsigned int nMeshes = ioPlugin->numberMeshesContainedInFile(extension, filename, prePar);
	std::list<MeshModel*> meshList;
	for (unsigned int i = 0; i < nMeshes; i++) {
		MeshModel *mm = md.addNewMesh(filename, fi.fileName());
		if (nMeshes != 1) {
			// if the file contains more than one mesh, this id will be
			// != -1
			mm->setIdInFile(i);
		}
		meshList.push_back(mm);
	}

	std::list<int> masks;

	try{
		loadMesh(fi.fileName(), ioPlugin, prePar, meshList, masks, cb);
	}
	catch(const MLException& e){
		for (MeshModel* mm : meshList)
			md.delMesh(mm);
		throw e;
	}
}


void reloadMesh(
		const QString& filename,
		const std::list<MeshModel*>& meshList,
		vcg::CallBackPos* cb)
{

	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.inputMeshPlugin(extension);

	if (ioPlugin == nullptr) {
		throw MLException(
				"Mesh " + filename + " cannot be opened. Your MeshLab "
				"version has not plugin to read " + extension +
				" file format");
	}

	RichParameterList prePar;
	ioPlugin->initPreOpenParameter(extension, prePar);
	prePar.join(meshlab::defaultGlobalParameterList());

	unsigned int nMeshes = ioPlugin->numberMeshesContainedInFile(extension, filename, prePar);

	if (meshList.size() != nMeshes){
		throw MLException(
			"Cannot reload " + filename + ": expected number layers is "
			"different from the number of meshes contained in th file.");
	}

	std::list<int> masks;
	for (MeshModel* mm : meshList){
		mm->Clear();
	}
	loadMesh(filename, ioPlugin, prePar, meshList, masks, cb);
}

void loadRaster(const QString& filename, MeshDocument& md, vcg::CallBackPos* cb)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.inputRasterPlugin(extension);

	if (ioPlugin == nullptr)
		throw MLException(
				"Raster " + filename + " cannot be opened. Your MeshLab version "
				"has not plugin to read " + extension + " file format.");

	RasterModel *rm = md.addNewRaster();
	try {
		ioPlugin->openRaster(extension, filename, *rm, cb);
	}
	catch(const MLException& e){
		md.delRaster(rm);
		throw e;
	}

}

}
