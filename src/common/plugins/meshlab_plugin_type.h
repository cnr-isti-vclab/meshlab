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

#ifndef MESHLAB_PLUGIN_TYPE_H
#define MESHLAB_PLUGIN_TYPE_H

class QString;

class MeshLabPlugin;

class MeshLabPluginType
{
public:
	MeshLabPluginType(const MeshLabPlugin* fpi);

	bool isValid() const;
	bool isDecoratePlugin() const;
	bool isEditPlugin() const;
	bool isFilterPlugin() const;
	bool isIOMeshPlugin() const;
	bool isRenderPlugin() const;

	bool isMultipleTypePlugin() const;
	QString pluginTypeString() const;

private:
	enum MLPType {
		UNKNOWN   = 1<<0, //1
		DECORATE  = 1<<1, //2
		EDIT      = 1<<2, //4
		FILTER    = 1<<3, //8
		IO        = 1<<4, //16
		RENDER    = 1<<5  //32
	};

	int type;
};

#endif // MESHLAB_PLUGIN_TYPE_H
