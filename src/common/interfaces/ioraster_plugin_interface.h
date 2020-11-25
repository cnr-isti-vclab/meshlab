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

#ifndef MESHLAB_IORASTER_PLUGIN_INTERFACE_H
#define MESHLAB_IORASTER_PLUGIN_INTERFACE_H

#include "plugin_interface.h"
#include "../utilities/file_format.h"

class IORasterPluginInterface : PluginInterface
{
public:
	IORasterPluginInterface() : PluginInterface() {}
	virtual ~IORasterPluginInterface() {}
	
	virtual QList<Format> exportFormats() const = 0;
	
	virtual bool open(
		const QString& format,
		const QString& filename,
		vcg::CallBackPos* cb = nullptr) = 0;
};

#endif // MESHLAB_IORASTER_PLUGIN_INTERFACE_H
