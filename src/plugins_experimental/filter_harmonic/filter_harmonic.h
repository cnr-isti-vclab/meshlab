/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2013                                                \/)\/    *
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
#ifndef FILTER_HARMONIC_H
#define FILTER_HARMONIC_H

#include <QObject>
#include <common/interfaces.h>

class QScriptEngine;

class FilterHarmonicPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:
	enum {
		FP_SCALAR_HARMONIC_FIELD
	};

	FilterHarmonicPlugin();

	virtual QString pluginName(void) const { return "FilterHarmonicPlugin"; }
	int getPreConditions(QAction *) const { return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACENUMBER; }
	int getRequirements(QAction *) { return MeshModel::MM_FACEFACETOPO; }
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	void initParameterSet(QAction *, MeshModel & /*m*/, RichParameterSet & /*parent*/);
	bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb);
	int postCondition( QAction* ) const
	{
		return MeshModel::MM_VERTCOLOR
		     | MeshModel::MM_VERTQUALITY
		     | MeshModel::MM_VERTFLAG
		     | MeshModel::MM_FACEVERT;
	}
	FilterClass getClass(QAction *a);
	QString filterScriptFunctionName(FilterIDType filterID);
};

#endif
