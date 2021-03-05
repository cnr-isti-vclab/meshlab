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

#ifndef _FILTER_PLYMC_H_
#define _FILTER_PLYMC_H_

#include <common/plugins/interfaces/filter_plugin.h>

class PlyMCPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum {
		FP_PLYMC,
		FP_MC_SIMPLIFY
	} ;

	PlyMCPlugin();

	QString pluginName() const;
	virtual QString filterName(ActionIDType filter) const;
	virtual QString filterInfo(ActionIDType filter) const;
	virtual void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*parent*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	FilterClass getClass(const QAction* a) const;
	FilterPlugin::FilterArity filterArity(const QAction* filter) const;
	int postCondition(const QAction *filter) const;
};

#endif
