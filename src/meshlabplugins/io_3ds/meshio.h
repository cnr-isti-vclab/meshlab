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
#ifndef EXTRAIOPLUGIN_H
#define EXTRAIOPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/plugins/interfaces/iomesh_plugin.h>

class ExtraMeshIOPlugin : public QObject, public IOMeshPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IOMESH_PLUGIN_IID)
	Q_INTERFACES(IOMeshPlugin)

public:
	QString pluginName() const;

	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;


	void exportMaskCapability(const QString& format, int &capability, int &defaultBits) const;

	void initPreOpenParameter(
				const QString& format,
				RichParameterList& parameters);

	bool open(const QString &formatName,
			const QString &fileName,
			MeshModel &m,
			int& mask,
			const RichParameterList &params,
			vcg::CallBackPos *cb=0,
			QWidget* parent=0);

	bool save(
			const QString &formatName,
			const QString &fileName,
			MeshModel &m,
			const int mask,
			const RichParameterList &,
			vcg::CallBackPos *cb=0,
			QWidget *parent= 0);

private:
	const std::array<QString, 1> paramNames = {
		"separate_layers"
	};
	enum paramEnum {
		SEPARATE_LAYERS = 0
	};
};

#endif
