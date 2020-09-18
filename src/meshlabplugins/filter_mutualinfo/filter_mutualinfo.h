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

#ifndef FILTER_MUTUALINFO_H
#define FILTER_MUTUALINFO_H

#include <QObject>

#include <common/interfaces/filter_plugin_interface.h>
#include "alignset.h"

class FilterMutualInfoPlugin : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
	Q_INTERFACES(FilterPluginInterface)

public:

	enum {FP_IMAGE_MUTUALINFO} ;

	FilterMutualInfoPlugin();

	QString pluginName() const;

	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FILTER_ARITY filterArity(const QAction*) const;
	void initParameterList(const QAction*, MeshDocument &, RichParameterList & /*parent*/);
	bool applyFilter(const QAction* filter, MeshDocument &md, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
	int postCondition(const QAction*) const;

private:
	AlignSet align;

	//mutualInfo
	bool imageMutualInfoAlign(
			MeshDocument &md,
			int rendmode,
			bool estimateFocal,
			bool fine,
			float expectedVariance,
			float tolerance,
			int numIterations,
			int backGroundWeight,
			vcg::Shotf shot);

	bool initGLMutualInfo();
};


#endif
