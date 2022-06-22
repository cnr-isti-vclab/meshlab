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
$Log: sampleplugins.h,v $

Revision 0
Created filter for Embree By Paolo Fasano p.fasano99@hotmail.com 

****************************************************************************/

#ifndef FILTEREMBREE_PLUGIN_H
#define FILTEREMBREE_PLUGIN_H

#include <QObject>
#include <common/plugins/interfaces/filter_plugin.h>

class FilterEmbreePlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum { 
		FP_OBSCURANCE, 
		FP_AMBIENT_OCCLUSION,
		FP_SDF, 
		FP_SELECT_VISIBLE_FACES, 
		FP_ANALYZE_NORMALS
		} ;

	FilterEmbreePlugin();
	~FilterEmbreePlugin();

	QString pluginName() const;

	QString filterName(ActionIDType filter) const;
	QString pythonFilterName(ActionIDType f) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	int getPreConditions(const QAction *) const;
	int postCondition(const QAction* ) const;
	RichParameterList initParameterList(const QAction*, const MeshModel &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);

};

#endif
