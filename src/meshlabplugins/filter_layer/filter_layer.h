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

#include <common/plugins/interfaces/filter_plugin_interface.h>

class FilterLayerPlugin : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
	Q_INTERFACES(FilterPluginInterface)

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
    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;
    virtual FilterClass getClass(const QAction*) const;
    virtual void initParameterList(const QAction*, MeshDocument &/*m*/, RichParameterList & /*parent*/);
    virtual bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
    int postCondition(const QAction *filter) const;
    FILTER_ARITY filterArity(const QAction*) const;
};

#endif
