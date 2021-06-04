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
#ifndef BASEIOPLUGIN_H
#define BASEIOPLUGIN_H

#include <common/ml_document/mesh_model.h>
#include <common/plugins/interfaces/io_plugin.h>

class BaseMeshIOPlugin : public QObject, public IOPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IO_PLUGIN_IID)
	Q_INTERFACES(IOPlugin)


public:

	BaseMeshIOPlugin();
	QString pluginName() const;

	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;
	std::list<FileFormat> importImageFormats() const;
	std::list<FileFormat> exportImageFormats() const;
	std::list<FileFormat> importProjectFormats() const;
	std::list<FileFormat> exportProjectFormats() const;

	void exportMaskCapability(
			const QString& format,
			int& capability,
			int& defaultBits) const;

	void open(
			const QString& formatName,
			const QString& fileName,
			MeshModel& m,
			int& mask,
			const RichParameterList& par,
			vcg::CallBackPos* cb);

	void save(
			const QString &formatName,
			const QString &fileName,
			MeshModel &m,
			const int mask,
			const RichParameterList& par,
			vcg::CallBackPos* cb);

	QImage openImage(
			const QString& format,
			const QString& fileName,
			vcg::CallBackPos* cb);

	void saveImage(
			const QString& format,
			const QString& fileName,
			const QImage& image,
			int quality,
			vcg::CallBackPos* cb);

	std::list<FileFormat> projectFileRequiresAdditionalFiles(
			const QString& format,
			const QString& filename);

	std::vector<MeshModel*> openProject(
			const QString& format,
			const QStringList& filenames,
			MeshDocument& md,
			std::vector<MLRenderingData>& rendOpt,
			vcg::CallBackPos* cb);

	RichParameterList initSaveProjectParameter(
				const QString& format,
				const MeshDocument& md);

	virtual void saveProject(
			const QString& format,
			const QString& fileName,
			const RichParameterList& params,
			const MeshDocument& md,
			const std::vector<MLRenderingData>& rendOpt,
			vcg::CallBackPos* /*cb*/ = nullptr);

	void initPreOpenParameter(const QString &formatName, RichParameterList &parlst);
	void initSaveParameter(const QString &format, const MeshModel &/*m*/, RichParameterList & par);
};

#endif
