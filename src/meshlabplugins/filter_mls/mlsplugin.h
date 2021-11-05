/*****************************************************************************
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

#ifndef MLSPLUGIN_H
#define MLSPLUGIN_H

#include <QObject>

#include "mlssurface.h"
#include <common/plugins/interfaces/filter_plugin.h>

class MlsPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum {
		FP_RIMLS_PROJECTION,
		FP_APSS_PROJECTION,
		FP_RIMLS_MCUBE,
		FP_APSS_MCUBE,
		FP_RIMLS_COLORIZE,
		FP_APSS_COLORIZE,
		FP_RADIUS_FROM_DENSITY,
		FP_SELECT_SMALL_COMPONENTS
	};

	MlsPlugin();

	QString     pluginName() const;
	QString     filterName(ActionIDType filter) const;
	QString     pythonFilterName(ActionIDType f) const;
	QString     filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	int         getRequirements(const QAction* action);

	RichParameterList initParameterList(const QAction*, const MeshDocument& md);

	std::map<std::string, QVariant> applyFilter(
		const QAction*           action,
		const RichParameterList& parameters,
		MeshDocument&            md,
		unsigned int&            postConditionMask,
		vcg::CallBackPos*        cb);
	FilterArity filterArity(const QAction*) const { return SINGLE_MESH; }

private:
	void addProjectionParameters(RichParameterList& parlst, const MeshDocument& md);
	void addMlsParameters(RichParameterList& parlst);
	void addApssParameters(RichParameterList& parlst, bool colorize);
	void addRimlsParameters(RichParameterList& parlst);
	void addColorizeParameters(RichParameterList& parlst, bool apss);
	void addMarchingCubesParameters(RichParameterList& parlst);

	void       initMLS(MeshDocument& md);
	MeshModel* getProjectionPointsMesh(MeshDocument& md, const RichParameterList& params);
	GaelMls::MlsSurface<CMeshO>* createMlsRimls(MeshModel* pPoints, const RichParameterList& par);
	GaelMls::MlsSurface<CMeshO>*
		 createMlsApss(MeshModel* pPoints, const RichParameterList& par, bool colorize);
	void computeProjection(
		MeshDocument&                md,
		const RichParameterList&     par,
		GaelMls::MlsSurface<CMeshO>* mls,
		MeshModel*                   pPoints,
		vcg::CallBackPos*            cb);
	void computeColorize(
		MeshDocument&                md,
		const RichParameterList&     par,
		GaelMls::MlsSurface<CMeshO>* mls,
		MeshModel*                   pPoints,
		vcg::CallBackPos*            cb);
	void computeMarchingCubes(
		MeshDocument&                md,
		const RichParameterList&     par,
		GaelMls::MlsSurface<CMeshO>* mls,
		vcg::CallBackPos*            cb);
};

#endif
