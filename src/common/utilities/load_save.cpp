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

	QString warningError = ioPlugin->warningMessageString();

	//saveRecentFileList(fileName);

	auto itmesh = meshList.begin();
	auto itmask = maskList.begin();
	for (unsigned int i = 0; i < meshList.size(); ++i){
		MeshModel* mm = *itmesh;
		int mask = *itmask;

		//if (!(mm->cm.textures.empty()))
		//	updateTexture(mm->id());

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
			ioPlugin->reportWarning(warningError + "\n" + QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum));
			//QMessageBox::warning(this, "MeshLab Warning", QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum) );

		//mm->cm.Tr = mtr;

		//computeRenderingDataOnLoading(mm,isareload, rendOpt);
		++itmesh;
		++itmask;
	}

	//updateLayerDialog();


	//meshDoc()->setBusy(false);

	QDir::setCurrent(origDir); // undo the change of directory before leaving
}


void loadWithStandardParameters(const QString& filename, MeshDocument& md, vcg::CallBackPos *cb)
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


	unsigned int nMeshes = ioPlugin->numberMeshesContainedInFile(extension, filename);
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

	loadMesh(fi.fileName(), ioPlugin, prePar, meshList, masks, cb);
	RichParameterList par;
	ioPlugin->initOpenParameter(extension, meshList, par);
	ioPlugin->applyOpenParameter(extension, meshList, par);
}


void reloadMesh(
		const QString& filename,
		MeshDocument& md,
		MeshModel& mm,
		vcg::CallBackPos* cb)
{
	// reload can affect only meshes loaded from files containing a single mesh,
	// or if the mesh is the first one loaded from that file.
	// if a file contains more than one meshes (idInFile != -1), the reload of
	// all the contained meshes will be performed by the mesh with idInFile == 0
	if (mm.idInFile() < 1) {
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

		unsigned int nMeshes = ioPlugin->numberMeshesContainedInFile(extension, filename);
		std::list<MeshModel*> meshList;

		//simple case: just one meshe expected in the file
		if (mm.idInFile() == -1 && nMeshes == 1) {
			meshList.push_back(&mm);
		}
		else if (mm.idInFile() == 0 && nMeshes > 1) {
			//looking for all the other meshes that should be reloaded from the file
			for (MeshModel* m : md.meshIterator()) {
				if (m->fullName() == mm.fullName() && m->idInFile() >= 0) {
					meshList.push_back(m);
				}
			}
			if (meshList.size() != nMeshes){
				throw MLException(
						"Cannot reload " + filename + ": the number of layers "
						"in this meshlab project associated to this file is "
						"different from the number of meshes contained in the "
						"file.");
			}
		}
		else {
			throw MLException(
					"Cannot reload " + filename + ": expected number layers is "
					"different from the number of meshes contained in th file.");
		}

		std::list<int> masks;
		loadMesh(fi.fileName(), ioPlugin, prePar, meshList, masks, cb);
		RichParameterList par;
		ioPlugin->initOpenParameter(extension, meshList, par);
		ioPlugin->applyOpenParameter(extension, meshList, par);
	}
}

}
