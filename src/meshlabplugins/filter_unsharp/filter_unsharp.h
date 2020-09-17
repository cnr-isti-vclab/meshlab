/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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
#ifndef FilterUnsharp_PLUGIN_H
#define FilterUnsharp_PLUGIN_H

#include <QObject>
#include <common/interfaces.h>


class FilterUnsharp : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

		public:
	/* naming convention :
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { 
      FP_CREASE_CUT, 
		FP_LAPLACIAN_SMOOTH, 
		FP_DIRECTIONAL_PRESERVATION, 
		FP_DEPTH_SMOOTH, 
		FP_HC_LAPLACIAN_SMOOTH,
		FP_SD_LAPLACIAN_SMOOTH, 
		FP_TWO_STEP_SMOOTH,
		FP_TAUBIN_SMOOTH,
		FP_FACE_NORMAL_SMOOTHING,
		FP_FACE_NORMAL_NORMALIZE,
		FP_VERTEX_NORMAL_NORMALIZE,
		FP_VERTEX_QUALITY_SMOOTHING,
		FP_UNSHARP_NORMAL,
		FP_UNSHARP_GEOMETRY,
		FP_UNSHARP_QUALITY,
		FP_UNSHARP_VERTEX_COLOR,
		FP_UNSHARP_FACE_COLOR,
		FP_RECOMPUTE_VERTEX_NORMAL,
		FP_RECOMPUTE_FACE_NORMAL,
		FP_RECOMPUTE_QUADFACE_NORMAL,
		FP_LINEAR_MORPH,
        FP_SCALAR_HARMONIC_FIELD
  } ;
	
	/* default values for standard parameters' values of the plugin actions */
	FilterUnsharp();
	~FilterUnsharp();

	QString pluginName() const;
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(const QAction*) const;
	int getRequirements(const QAction*);
	bool applyFilter(const QAction* filter, MeshDocument &md, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
	void initParameterList(QAction *action, MeshDocument &/*m*/, RichParameterList & parlst);
	int postCondition(const QAction* ) const;
	int getPreConditions(const QAction*) const;
	FILTER_ARITY filterArity(const QAction* filter) const;

};


#endif
