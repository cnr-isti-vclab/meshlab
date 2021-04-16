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

#include "meshlab_plugin_type.h"

#include "interfaces/decorate_plugin.h"
#include "interfaces/edit_plugin.h"
#include "interfaces/filter_plugin.h"
#include "interfaces/io_plugin.h"
#include "interfaces/render_plugin.h"

MeshLabPluginType::MeshLabPluginType(const MeshLabPlugin* fpi) : type(0)
{
	//Decorate
	const DecoratePlugin *iDecorator = dynamic_cast<const DecoratePlugin *>(fpi);
	if (iDecorator) {
		type |= DECORATE;
	}
	//EditFactory
	const EditPlugin* efp = dynamic_cast<const EditPlugin *>(fpi);
	if (efp) { //EditFactory Plugin
		type |= EDIT;
	}
	//Filter
	const FilterPlugin *iFilter = dynamic_cast<const FilterPlugin *>(fpi);
	if (iFilter){
		type |= FILTER;
	}
	//IOMesh
	const IOPlugin *iIOMesh = dynamic_cast<const IOPlugin *>(fpi);
	if (iIOMesh) {
		type |= IO;
	}
	
	//Render
	const RenderPlugin *iRender = dynamic_cast<const RenderPlugin *>(fpi);
	if (iRender) {
		type |= RENDER;
	}

	if (type == 0)
		type = UNKNOWN;
}

bool MeshLabPluginType::isValid() const
{
	return !(type & UNKNOWN);
}

bool MeshLabPluginType::isDecoratePlugin() const
{
	return type & DECORATE;
}

bool MeshLabPluginType::isEditPlugin() const
{
	return type & EDIT;
}

bool MeshLabPluginType::isFilterPlugin() const
{
	return type & FILTER;
}

bool MeshLabPluginType::isIOMeshPlugin() const
{
	return type & IO;
}

bool MeshLabPluginType::isRenderPlugin() const
{
	return type & RENDER;
}

bool MeshLabPluginType::isMultipleTypePlugin() const
{
	if (type == UNKNOWN) return false;
	return ((type & (type - 1)) != 0); //true if not power of 2
}

/**
 * @brief Returns a string representing the type of the plugin.
 * Note: 
 * - returns "Unknown" it the type is not valid;
 * - returns a String of the type "Type1|Type2" if the plugin
 *   has multiple types.
 * @return 
 */
QString MeshLabPluginType::pluginTypeString() const
{
	QString type = "";
	if (!isValid())
		return "Unknown";
	
	if (isDecoratePlugin()){
		type += "Decorate";
	}
	if (isEditPlugin()){
		if (!type.isEmpty()) type += "|";
		type += "Edit";
	}
	if (isFilterPlugin()){
		if (!type.isEmpty()) type += "|";
		type += "Filter";
	}
	if (isIOMeshPlugin()){
		if (!type.isEmpty()) type += "|";
		type += "IO";
	}
	if (isRenderPlugin()){
		if (!type.isEmpty()) type += "|";
		type += "Render";
	}
	return type;
}
