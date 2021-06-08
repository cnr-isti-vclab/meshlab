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
#ifndef FILTERDOCSAMPLINGPLUGIN_H
#define FILTERDOCSAMPLINGPLUGIN_H

#include <common/plugins/interfaces/filter_plugin.h>

class FilterDocSampling : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum {
		FP_ELEMENT_SUBSAMPLING,
		FP_MONTECARLO_SAMPLING,
		FP_REGULAR_RECURSIVE_SAMPLING,
		FP_CLUSTERED_SAMPLING,
		FP_STRATIFIED_SAMPLING,
		FP_HAUSDORFF_DISTANCE,
		FP_DISTANCE_REFERENCE,
		FP_TEXEL_SAMPLING,
		FP_VERTEX_RESAMPLING,
		FP_UNIFORM_MESH_RESAMPLING,
		FP_VORONOI_COLORING,
		FP_DISK_COLORING,
		FP_POISSONDISK_SAMPLING,
		FP_POINTCLOUD_SIMPLIFICATION
	} ;

	FilterDocSampling();

	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	RichParameterList initParameterList(const QAction*, const MeshDocument &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	int getRequirements(const QAction* action);
	int postCondition(const QAction* ) const;
	FilterClass getClass(const QAction*) const;
	FilterArity filterArity(const QAction* filter) const;
};

#endif
