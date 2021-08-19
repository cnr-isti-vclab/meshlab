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

#ifndef MESHLAB_FILTER_EXAMPLE_PLUGIN_H
#define MESHLAB_FILTER_EXAMPLE_PLUGIN_H

// from meshlab common, include the abstract class file of filter plugins
#include <common/plugins/interfaces/filter_plugin.h>
#include <vcglib/vcg/complex/algorithms/align_pair.h>

#include "./align/icp_align_parameter.h"

/**
 * @brief The FilterExamplePlugin class
 * This is a simple and useless example of a MeshLab filter plugin.
 *
 * You can use this class as a base to start and coding your plugin!
 *
 * You can change the name of the class. Just be sure that the class
 * inherits from the QObject class (needed to make the plugin a Qt plugin)
 * and the FilterPlugin class.
 *
 * Check the cpp file for the explanation of each member function of the class and
 * whether they are mandatory to be implemented.
 */
class FilterIcpPlugin : public QObject, public FilterPlugin
{
	//keep these three lines unchanged
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	//enum used to give an ID to every filter implemented in the plugin
	enum FileterIds {
	    FP_TWO_MESH_ICP,
	    FP_GLOBAL_MESH_ICP
	};

    FilterIcpPlugin();

	QString pluginName() const;
	QString vendor() const;

	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	int getPreConditions(const QAction *) const;
	int postCondition(const QAction* ) const;

	RichParameterList initParameterList(const QAction* action, const MeshDocument &md);

	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & params,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);

private:

    void applyIcpTwoMeshes(MeshDocument &meshDocument, const RichParameterList &par);

    vcg::AlignPair::Param alignParameters;

};

#endif //MESHLAB_FILTER_EXAMPLE_PLUGIN_H
