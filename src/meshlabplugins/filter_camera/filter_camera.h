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

#ifndef FILTER_CAMERA_H
#define FILTER_CAMERA_H

#include <QObject>

#include <common/plugins/interfaces/filter_plugin.h>

class FilterCameraPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum { FP_SET_MESH_CAMERA,
		FP_SET_RASTER_CAMERA,
		FP_QUALITY_FROM_CAMERA,
		FP_CAMERA_ROTATE,
		FP_CAMERA_SCALE,
		FP_CAMERA_TRANSLATE,
		FP_CAMERA_TRANSFORM,
        FP_ORIENT_NORMALS_WITH_CAMERAS};

	FilterCameraPlugin();
	QString pluginName() const;
	int getPreConditions(const QAction*) const;
	int postCondition(const QAction* filter) const;
	virtual QString filterName(ActionIDType filter) const;
	virtual QString filterInfo(ActionIDType filter) const;
	virtual FilterClass getClass(const QAction*) const;
	virtual void initParameterList(const QAction*, MeshDocument &/*m*/, RichParameterList & /*parent*/);
	std::map<std::string, QVariant> applyFilter(const QAction* action, const RichParameterList & /*parent*/, MeshDocument &md, unsigned int& postConditionMask, vcg::CallBackPos * cb);
	FILTER_ARITY filterArity(const QAction* act) const;
};

#endif
