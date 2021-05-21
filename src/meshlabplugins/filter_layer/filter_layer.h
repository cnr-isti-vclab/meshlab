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

#ifndef FILTER_SPLITTER_H
#define FILTER_SPLITTER_H

#include <QObject>

#include <common/plugins/interfaces/filter_plugin.h>

class FilterLayerPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum {
		FP_FLATTEN,
		FP_MESH_VISIBILITY,
		FP_SPLITSELECTEDFACES,
		FP_SPLITSELECTEDVERTICES,
		FP_SPLITCONNECTED,
		FP_DUPLICATE,
		FP_RENAME_MESH,
		FP_RENAME_RASTER,
		FP_DELETE_MESH,
		FP_DELETE_NON_VISIBLE_MESH,
		FP_DELETE_RASTER,
		FP_DELETE_NON_SELECTED_RASTER,
		FP_SELECTCURRENT,
		FP_EXPORT_CAMERAS,
		FP_IMPORT_CAMERAS
	};

	FilterLayerPlugin();

	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction*) const;
	RichParameterList initParameterList(const QAction*, const MeshDocument &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);    int postCondition(const QAction *filter) const;
	FilterArity filterArity(const QAction*) const;
};

#endif
