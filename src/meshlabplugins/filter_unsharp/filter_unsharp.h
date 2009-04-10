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
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class FilterUnsharp : public QObject, public MeshFilterInterface
{
	Q_OBJECT
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
		FP_RECOMPUTE_VERTEX_NORMAL_WEIGHTED,
		FP_RECOMPUTE_VERTEX_NORMAL_ANGLE,
		FP_RECOMPUTE_FACE_NORMAL,
		FP_LINEAR_MORPH
  } ;
	
	/* default values for standard parameters' values of the plugin actions */
	FilterUnsharp();
	~FilterUnsharp();

	virtual const QString filterName(FilterIDType filter);
	virtual const QString filterInfo(FilterIDType filter);

  virtual bool autoDialog(QAction *);
	virtual const FilterClass getClass(QAction *);
  virtual const int getRequirements(QAction *);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
  virtual void initParameterSet(QAction *action, MeshModel &/*m*/, FilterParameterSet & parlst);

	protected:
	
};


#endif
