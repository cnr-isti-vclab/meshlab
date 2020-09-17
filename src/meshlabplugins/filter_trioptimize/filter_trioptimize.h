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

#ifndef TRIOPTIMIZEFILTERSPLUGIN_H
#define TRIOPTIMIZEFILTERSPLUGIN_H

#include <QObject>
#include <common/interfaces.h>

class TriOptimizePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { 
		// mesh improvement by edge flipping
		FP_CURVATURE_EDGE_FLIP,
		FP_PLANAR_EDGE_FLIP,
		// Laplacian smooth that do not moves vertices far from the surface
		FP_NEAR_LAPLACIAN_SMOOTH
	};

	TriOptimizePlugin();
	
	QString pluginName() const;
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterList & /*parent*/);
	bool applyFilter(const QAction *filter, MeshDocument &md, const RichParameterList &/*parent*/, vcg::CallBackPos * cb) ;
	int getRequirements(QAction *);
	FilterClass getClass(const QAction *);
	int postCondition(const QAction* ) const;
    FILTER_ARITY filterArity(QAction *) const {return SINGLE_MESH;}

};

#endif
