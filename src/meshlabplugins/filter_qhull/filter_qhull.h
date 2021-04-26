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
/****************************************************************************
History

****************************************************************************/

#ifndef QHULLFILTERSPLUGIN_H
#define QHULLFILTERSPLUGIN_H

#include <QObject>
#include <common/plugins/interfaces/filter_plugin.h>

class QhullPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:

	/* naming convention :
	- FP -> Filter Plugin
	- name of the plugin separated by _
	*/

	enum {
		FP_QHULL_CONVEX_HULL,
		FP_QHULL_DELAUNAY_TRIANGULATION,
		FP_QHULL_VORONOI_FILTERING,
		FP_QHULL_ALPHA_COMPLEX_AND_SHAPE,
		FP_QHULL_VISIBLE_POINTS
	};

	QhullPlugin();
	~QhullPlugin();

	QString pluginName() const;
	virtual QString filterName(ActionIDType filter) const;
	virtual QString filterInfo(ActionIDType filter) const;
	virtual void initParameterList(const QAction*, const MeshModel &/*m*/, RichParameterList & /*parent*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	virtual FilterClass getClass(const QAction*) const;
	FilterArity filterArity(const QAction *) const {return SINGLE_MESH;}
};

#endif
