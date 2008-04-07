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


#ifndef GEOMETRYAGINGPLUGIN_H
#define GEOMETRYAGINGPLUGIN_H


#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/refine.h>

#include "edgepred.h"


class GeometryAgingPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

	public:
		enum {FP_ERODE};
	
		GeometryAgingPlugin();
		virtual ~GeometryAgingPlugin();
	
		virtual const QString filterInfo(FilterIDType filter);
		virtual const QString filterName(FilterIDType filter);
		virtual const PluginInfo &pluginInfo();
		virtual const int getRequirements(QAction *) {return (MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);}
		virtual bool autoDialog(QAction *) {return true;}
		virtual void initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &params);
		virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb);
		
	protected:
		enum {SIMPLE=0, LINEAR=1, SINUSOIDAL=2};
		double generateNoiseValue(int style, const CVertexO::ScalarType &x, const CVertexO::ScalarType &y, const CVertexO::ScalarType &z);
};


#endif
