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

#include "load_save.h"

#include <QElapsedTimer>
#include <QDir>

#include "../globals.h"
#include "../plugins/plugin_manager.h"

#include <exif.h>

namespace meshlab {

/**
 * @brief This function assumes that you already have the followind data:
 * - the plugin that is needed to load the mesh
 * - the number of meshes that will be loaded from the file
 * - the list of MeshModel(s) that will contain the loaded mesh(es)
 * - the open parameters that will be used to load the mesh(es)
 *
 * The function will take care to load the mesh, load textures if needed
 * and make all the clean operations after loading the meshes.
 * If load fails, throws a MLException.
 *
 * @param[i] fileName: the filename
 * @param[i] ioPlugin: the plugin that supports the file format to load
 * @param[i] prePar: the pre open parameters
 * @param[i/o] meshList: the list of meshes that will be loaded from the file
 * @param[o] maskList: masks of loaded components for each loaded mesh
 * @param cb: callback
 * @return the list of texture names that could not be loaded
 */
std::list<std::string> loadMesh(
		const QString& fileName,
		IOPlugin* ioPlugin,
		const RichParameterList& prePar,
		const std::list<MeshModel*>& meshList,
		std::list<int>& maskList,
		vcg::CallBackPos *cb)
{
	std::list<std::string> unloadedTextures;
	QFileInfo fi(fileName);
	QString extension = fi.suffix();

	QDir oldDir = QDir::current();
	QDir::setCurrent(fi.absolutePath());
	ioPlugin->open(extension, fi.fileName(), meshList, maskList, prePar, cb);
	QDir::setCurrent(oldDir.absolutePath());

	auto itmesh = meshList.begin();
	auto itmask = maskList.begin();
	for (unsigned int i = 0; i < meshList.size(); ++i){
		MeshModel* mm = *itmesh;
		int mask = *itmask;

		std::list<std::string> tmp = mm->loadTextures(nullptr, cb);
		unloadedTextures.insert(unloadedTextures.end(), tmp.begin(), tmp.end());

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
	return unloadedTextures;
}

/**
 * @brief loads the given filename and puts the loaded mesh(es) into the
 * given MeshDocument. Returns the list of loaded meshes.
 *
 * If you already know the open parameters that could be used to load the mesh,
 * you can pass a RichParameterList containing them.
 * Note: only parameters of your RPL that are actually required by the plugin
 * will be given as input to the load function.
 * If you don't know any parameter, leave the RichParameterList parameter empty.
 *
 * The function takes care to:
 * - find the plugin that loads the format of the file
 * - create the required MeshModels into the MeshDocument
 * - load the meshes and their textures, with standard parameters
 *
 * if an error occurs, an exception will be thrown, and MeshDocument won't
 * contain new meshes.
 */
std::list<MeshModel*> loadMeshWithStandardParameters(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos *cb,
		RichParameterList prePar)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin* ioPlugin = pm.inputMeshPlugin(extension);

	if (ioPlugin == nullptr)
		throw MLException(
				"Mesh " + filename + " cannot be opened. Your MeshLab version "
				"has not plugin to read " + extension + " file format");

	ioPlugin->setLog(&md.Log);
	RichParameterList openParams =ioPlugin->initPreOpenParameter(extension);

	for (RichParameter& rp : prePar){
		auto it = openParams.findParameter(rp.name());
		if (it != openParams.end()){
			it->setValue(rp.value());
		}
	}
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
		loadMesh(filename, ioPlugin, prePar, meshList, masks, cb);
	}
	catch(const MLException& e){
		for (const MeshModel* mm : meshList)
			md.delMesh(mm->id());
		throw e;
	}

	return meshList;
}


void reloadMesh(
		const QString& filename,
		const std::list<MeshModel*>& meshList,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin* ioPlugin = pm.inputMeshPlugin(extension);

	if (ioPlugin == nullptr) {
		throw MLException(
				"Mesh " + filename + " cannot be opened. Your MeshLab "
				"version has not plugin to read " + extension +
				" file format");
	}


	ioPlugin->setLog(log);
	RichParameterList prePar = ioPlugin->initPreOpenParameter(extension);
	prePar.join(meshlab::defaultGlobalParameterList());

	unsigned int nMeshes = ioPlugin->numberMeshesContainedInFile(extension, filename, prePar);

	if (meshList.size() != nMeshes){
		throw MLException(
			"Cannot reload " + filename + ": expected number layers is "
			"different from the number of meshes contained in th file.");
	}

	std::list<int> masks;
	for (MeshModel* mm : meshList){
		mm->clear();
	}
	loadMesh(filename, ioPlugin, prePar, meshList, masks, cb);
}

void saveMeshWithStandardParameters(
		const QString& fileName,
		MeshModel& m,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(fileName);
	QString extension = fi.suffix().toLower();

	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin* ioPlugin = pm.outputMeshPlugin(extension);
	if (ioPlugin == nullptr) {
		throw MLException(
				"Mesh " + fileName + " cannot be saved. Your MeshLab "
				"version has not plugin to save " + extension +
				" file format");
	}
	ioPlugin->setLog(log);
	int capability=0,defaultBits=0;
	ioPlugin->exportMaskCapability(extension, capability, defaultBits);
	RichParameterList saveParams = ioPlugin->initSaveParameter(extension, m);

	if (defaultBits & vcg::tri::io::Mask::IOM_BITPOLYGONAL)
		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
	ioPlugin->save(extension, fileName, m, defaultBits, saveParams, cb);
	m.setFileName(fileName);
	m.saveTextures(fi.absolutePath(), -1, log, cb);
}

void saveAllMeshes(
		const QString& basePath,
		MeshDocument& md,
		bool onlyVisible,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	PluginManager& pm = meshlab::pluginManagerInstance();

	for (MeshModel& m : md.meshIterator()){
		if (m.isVisible() || !onlyVisible) {
			QString filename, extension;
			if (m.fullName().isEmpty()){
				if (m.label().contains('.')){
					extension = QFileInfo(m.label()).suffix();
					filename = QFileInfo(m.label()).baseName();
				}
				else {
					extension = "ply";
					filename = m.label();
				}
			}
			else {
				QFileInfo fi(m.fullName());
				extension = fi.suffix();
				filename = fi.baseName();
			}
			filename.replace(QRegExp("[" + QRegExp::escape( "\\/:*?\"<>|" ) + "]"),QString("_"));
			IOPlugin* ioPlugin = pm.outputMeshPlugin(extension);
			if (ioPlugin == nullptr){
				std::cerr << "Warning: extension " + extension.toStdString() +
						" not supported. Saving " + filename.toStdString() + ".ply.";
				filename += ".ply";
			}
			else {
				filename += ("." + extension.toLower());
			}
			filename = basePath + "/" + filename;
			saveMeshWithStandardParameters(filename, m, log, cb);
		}
	}
}

QImage loadImage(
		const QString& filename,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.inputImagePlugin(extension);

	if (ioPlugin != nullptr){
		ioPlugin->setLog(log);
		return ioPlugin->openImage(extension, filename, cb);
	}
	else { // fallback: try to load the file using QImage::load
		QImage img(filename);
		if (img.isNull()){ // also QImage::load failed
			throw MLException(
				"Image " + filename + " cannot be opened. Your MeshLab version "
				"has not plugin to read " + extension + " file format.");
		}
		return img;
	}
}

void saveImage(
		const QString& filename,
		const QImage& image,
		int quality,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.outputImagePlugin(extension);

	std::cerr << "Path: " << fi.path().toStdString() << "\n";

	if (!fi.path().isEmpty()){
		if (!QDir(fi.path()).exists()){
			QDir().mkdir(fi.path());
		}
	}

	if (ioPlugin == nullptr)
		throw MLException(
				"Image " + filename + " cannot be saved. Your MeshLab version "
				"has not plugin to save " + extension + " file format.");

	ioPlugin->setLog(log);
	ioPlugin->saveImage(extension, filename, image, quality, cb);
}

void loadRaster(const QString& filename, RasterModel& rm, GLLogStream* log, vcg::CallBackPos* cb)
{
	QImage loadedImage = loadImage(filename, log, cb);
	rm.setLabel(filename);
	rm.addPlane(new RasterPlane(loadedImage, filename, RasterPlane::RGBA));

	// Read the file into a buffer
	FILE *fp = fopen(qUtf8Printable(filename), "rb");
	if (!fp) {
		QString errorMsgFormat = "Exif Parsing: Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
		throw MLException(errorMsgFormat.arg(filename));
	}
	fseek(fp, 0, SEEK_END);
	unsigned long fsize = ftell(fp);
	rewind(fp);
	unsigned char *buf = new unsigned char[fsize];
	if (fread(buf, 1, fsize, fp) != fsize) {
		QString errorMsgFormat = "Exif Parsing: Unable to read the content of the opened file:\n\"%1\"\n\nError details: file %1 is not readable.";
		delete[] buf;
		fclose(fp);
		throw MLException(errorMsgFormat.arg(filename));
	}
	fclose(fp);

	// Parse EXIF
	easyexif::EXIFInfo ImageInfo;
	int code = ImageInfo.parseFrom(buf, fsize);
	delete[] buf;
	if (!code) {
		log->log(GLLogStream::FILTER, "Warning: unable to parse exif for file " + filename);
	}

	if (code && ImageInfo.FocalLengthIn35mm==0.0f)
	{
		rm.shot.Intrinsics.ViewportPx = vcg::Point2i(rm.currentPlane->image.width(), rm.currentPlane->image.height());
		rm.shot.Intrinsics.CenterPx   = Point2m(float(rm.currentPlane->image.width()/2.0), float(rm.currentPlane->image.width()/2.0));
		rm.shot.Intrinsics.PixelSizeMm[0]=36.0f/(float)rm.currentPlane->image.width();
		rm.shot.Intrinsics.PixelSizeMm[1]=rm.shot.Intrinsics.PixelSizeMm[0];
		rm.shot.Intrinsics.FocalMm = 50.0f;
	}
	else
	{
		rm.shot.Intrinsics.ViewportPx = vcg::Point2i(ImageInfo.ImageWidth, ImageInfo.ImageHeight);
		rm.shot.Intrinsics.CenterPx   = Point2m(float(ImageInfo.ImageWidth/2.0), float(ImageInfo.ImageHeight/2.0));
		float ratioFocal=ImageInfo.FocalLength/ImageInfo.FocalLengthIn35mm;
		rm.shot.Intrinsics.PixelSizeMm[0]=(36.0f*ratioFocal)/(float)ImageInfo.ImageWidth;
		rm.shot.Intrinsics.PixelSizeMm[1]=(24.0f*ratioFocal)/(float)ImageInfo.ImageHeight;
		rm.shot.Intrinsics.FocalMm = ImageInfo.FocalLength;
	}
	// End of EXIF reading
}

std::vector<MeshModel*> loadProject(
		const QStringList& filenames,
		IOPlugin* ioPlugin,
		MeshDocument& md,
		std::vector<MLRenderingData>& rendOpt,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(filenames.first());
	QString extension = fi.suffix();

	ioPlugin->setLog(log);
	return ioPlugin->openProject(extension, filenames, md, rendOpt, cb);
}

std::vector<MeshModel*> loadProject(
		const QStringList& filenames,
		MeshDocument& md,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QFileInfo fi(filenames.first());
	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.inputProjectPlugin(extension);

	if (ioPlugin == nullptr)
		throw MLException(
				"Project " + filenames.first() + " cannot be loaded. Your MeshLab version "
				"has not plugin to load " + extension + " file format.");

	std::list<FileFormat> additionalFiles =
			ioPlugin->projectFileRequiresAdditionalFiles(extension, filenames.first());

	if (additionalFiles.size() +1 != (unsigned int)filenames.size()){
		throw MLException(
				"The number of input files given (" + QString::number(filenames.size()) +
				") is different from the expected one (" +
				QString::number(additionalFiles.size() +1));
	}
	std::vector<MLRenderingData> rendOpt;
	return loadProject(filenames, ioPlugin, md, rendOpt, log, cb);
}

std::vector<MeshModel*> loadProject(
		const QString& filename,
		MeshDocument& md,
		GLLogStream* log,
		vcg::CallBackPos* cb)
{
	QStringList fnms;
	fnms.push_back(filename);
	return loadProject(fnms, md, log, cb);
}

void saveProject(
		const QString& filename,
		const MeshDocument& md,
		bool onlyVisibleMeshes,
		std::vector<MLRenderingData> renderData)
{
	QFileInfo fi(filename);
	QString extension = fi.suffix();

	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.outputProjectPlugin(extension);

	if (ioPlugin == nullptr)
		throw MLException(
				"Project " + filename + " cannot be loaded. Your MeshLab version "
				"has not plugin to load " + extension + " file format.");

	if (renderData.size() != 0 && md.meshNumber() != renderData.size()){
		std::cerr << "Warning: renderData vector has different size from "
				"MeshDocument number meshes. Ignoring render data when saving " +
				filename.toStdString() << " project.";
		renderData.clear();
	}

	RichParameterList rpl;
	ioPlugin->saveProject(extension, filename, md, onlyVisibleMeshes, renderData);
}

}
