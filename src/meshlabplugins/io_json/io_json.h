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
#ifndef JSONIOPLUGIN_H
#define JSONIOPLUGIN_H

#include <common/plugins/interfaces/iomesh_plugin.h>
#include <common/ml_document/mesh_model.h>

class JSONIOPlugin : public QObject, public IOMeshPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IOMESH_PLUGIN_IID)
	Q_INTERFACES(IOMeshPlugin)

public:

	JSONIOPlugin(void);
	virtual ~JSONIOPlugin(void);

	QString pluginName() const;

	std::list<FileFormat> importFormats(void) const;
	QList<FileFormat> exportFormats(void) const;

	void exportMaskCapability(const QString & format, int & capability, int & defaultBits) const;

	bool open(const QString & formatName, const QString & fileName, MeshModel & m, int & mask, const RichParameterList & par, vcg::CallBackPos * cb = 0, QWidget * parent = 0);
	bool save(const QString & formatName, const QString & fileName, MeshModel & m, const int mask, const RichParameterList & par, vcg::CallBackPos * cb = 0, QWidget * parent = 0);
};

#endif // JSONIOPLUGIN_H
