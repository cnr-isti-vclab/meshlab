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
#ifndef MESHLAB_IORASTER_BASE_H
#define MESHLAB_IORASTER_BASE_H


#include <common/plugins/interfaces/ioraster_plugin_interface.h>

class IORasterBasePlugin : public QObject, public IORasterPluginInterface
{
	Q_OBJECT
		MESHLAB_PLUGIN_IID_EXPORTER(IORASTER_PLUGIN_INTERFACE_IID)
		Q_INTERFACES(IORasterPluginInterface)


public:

	IORasterBasePlugin();
	QString pluginName() const;
	
	QList<FileFormat> importFormats() const;
	
	bool open(
			const QString& format,
			const QString& filename, 
			RasterModel& rm,
			vcg::CallBackPos* cb = nullptr);
	
private:
	QList<FileFormat> formatList;
};

#endif //MESHLAB_IORASTER_BASE_H
