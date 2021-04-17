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

#ifndef EXTRAFILTERSPLUGIN_H
#define EXTRAFILTERSPLUGIN_H

#include <common/plugins/interfaces/filter_plugin.h>

class ExtraMeshFilterPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

		enum RefPlane { REF_CENTER,REF_MIN,REF_ORIG};

public:
		/* naming convention :
	- FP -> Filter Plugin
	- name of the filter separated by _
	*/
		enum {
		FP_CLOSE_HOLES_M
	} ;



	ExtraMeshFilterPlugin();
	~ExtraMeshFilterPlugin(){}
	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;

	FilterClass getClass(const QAction*) const;
	void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*parent*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	int postCondition(const QAction *filter) const;
	int getPreConditions(const QAction *filter) const;
	FilterArity filterArity(const QAction *) const {return SINGLE_MESH;}

protected:

	float lastq_QualityThr;
	bool lastq_QualityWeight;
	bool lastq_PreserveBoundary;
	bool lastq_Selected;
	bool lastq_PreserveNormal;
	bool lastq_BoundaryWeight;
	bool lastq_PreserveTopology;
	bool lastq_OptimalPlacement;
	bool lastq_PlanarQuadric;
	float lastq_PlanarWeight;

	float lastqtex_QualityThr;
	float lastqtex_extratw;

	int lastisor_Iterations;
	Scalarm lastisor_MaxSurfDist;
	Scalarm lastisor_FeatureDeg;
	bool lastisor_CheckSurfDist;
	bool lastisor_RemeshingAdaptivity;
	bool lastisor_SelectedOnly;
	bool lastisor_RefineFlag;
	bool lastisor_CollapseFlag;
	bool lastisor_SwapFlag;
	bool lastisor_SmoothFlag;
	bool lastisor_ProjectFlag;

};
#endif
