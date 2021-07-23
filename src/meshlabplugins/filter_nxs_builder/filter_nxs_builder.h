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

#ifndef MESHLAB_FILTER_NXS_BUILDER_H
#define MESHLAB_FILTER_NXS_BUILDER_H

#include <common/plugins/interfaces/filter_plugin.h>

class NxsBuilderPlugin : public QObject, public FilterPlugin
{
	//keep these three lines unchanged
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(NxsBuilderPlugin)

public:
	//enum used to give an ID to every filter implemented in the plugin
	enum FileterIds {
		FP_NXS_BUILDER,
		FP_NXS_COMPRESS
	};

	NxsBuilderPlugin();

	QString pluginName() const;
	QString vendor() const;

	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	int getPreConditions(const QAction *) const;
	int postCondition(const QAction* ) const;
	RichParameterList initParameterList(const QAction*, const MeshModel &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & params,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);

private:
	void vertexDisplacement(
			MeshDocument &md,
			vcg::CallBackPos *cb,
			bool updateNormals,
			Scalarm max_displacement);
};

#endif //MESHLAB_FILTER_NXS_BUILDER_H
