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

#ifndef SAMPLEFILTERSPLUGIN_H
#define SAMPLEFILTERSPLUGIN_H

#include <common/plugins/interfaces/filter_plugin.h>

class FilterScreenedPoissonPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)
public:

	enum {
		FP_SCREENED_POISSON
	};

	FilterScreenedPoissonPlugin();
	~FilterScreenedPoissonPlugin();

	QString pluginName() const;
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;

	FilterClass getClass(const QAction* a) const;
	int getRequirements(const QAction* a);

	bool applyFilter(
			const QAction* filter,
			MeshDocument& md,
			std::map<std::string, QVariant>& outputValues,
			unsigned int& postConditionMask,
			const RichParameterList& params,
			vcg::CallBackPos* cb) ;

	void initParameterList(const QAction* a, MeshModel&, RichParameterList& parlist);
	int postCondition(const QAction* filter) const;
	FILTER_ARITY filterArity(const QAction*) const;
};


#endif
