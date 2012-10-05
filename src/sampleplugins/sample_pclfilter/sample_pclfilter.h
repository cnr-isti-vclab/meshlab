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


#ifndef SAMPLEPCLFILTERPLUGIN_H
#define SAMPLEPCLFILTERPLUGIN_H

#include <QObject>

#include <common/interfaces.h>

class SamplePCLFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_PCL_SAMPLE  } ;

	SamplePCLFilterPlugin();

	QString pluginName(void) const { return "SamplePCLFilterPlugin"; }
	void initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst);
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
    bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
    FilterClass getClass(QAction *a);
};


#endif