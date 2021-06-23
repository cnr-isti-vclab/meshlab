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

#ifndef IO_EXAMPLE_PLUGIN_H
#define IO_EXAMPLE_PLUGIN_H

#include <common/plugins/interfaces/io_plugin.h>

class IOglTFPlugin : public QObject, public IOPlugin
{ 
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IO_PLUGIN_IID)
	Q_INTERFACES(IOPlugin)

public:

	QString pluginName() const;

	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;

	void exportMaskCapability(
			const QString& format,
			int& capability,
			int& defaultBits) const;

	void open(
			const QString& format,
			const QString& fileName,
			MeshModel& m,
			int& mask,
			const RichParameterList& par,
			vcg::CallBackPos* cb = nullptr);

	void save(
			const QString& format,
			const QString& fileName,
			MeshModel& m,
			const int mask,
			const RichParameterList& par,
			vcg::CallBackPos* cb = nullptr);
};

#endif
