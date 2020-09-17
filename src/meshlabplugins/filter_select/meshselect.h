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

#ifndef FILTER_SELECT_H
#define FILTER_SELECT_H

#include <QObject>
#include <common/interfaces.h>


class SelectionFilterPlugin : public QObject, public MeshFilterInterface
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
    FP_SELECT_ALL,
    FP_SELECT_NONE,
    FP_SELECTBYANGLE,
	FP_SELECT_UGLY,
    FP_SELECT_INVERT,
    FP_SELECT_CONNECTED,
    FP_SELECT_FACE_FROM_VERT,
    FP_SELECT_VERT_FROM_FACE,
    FP_SELECT_DELETE_VERT,
	FP_SELECT_DELETE_ALL_FACE,
    FP_SELECT_DELETE_FACE,
    FP_SELECT_DELETE_FACEVERT,
    FP_SELECT_ERODE,
    FP_SELECT_DILATE,
    FP_SELECT_BORDER,
    FP_SELECT_BY_FACE_QUALITY,
    FP_SELECT_BY_VERT_QUALITY,
    FP_SELECT_BY_RANGE,
    FP_SELECT_BY_COLOR,CP_SELFINTERSECT_SELECT,
    CP_SELECT_TEXBORDER,
    CP_SELECT_NON_MANIFOLD_FACE,
    CP_SELECT_NON_MANIFOLD_VERTEX,
    FP_SELECT_FACES_BY_EDGE,
    FP_SELECT_FOLD_FACE,
    FP_SELECT_OUTLIER
	} ;

	SelectionFilterPlugin();
  //~SelectionFilterPlugin();
    QString pluginName() const;
	virtual QString filterInfo(FilterIDType filter) const;
  virtual QString filterName(FilterIDType filter) const;
  
  virtual FilterClass getClass(const QAction*) const;
  void initParameterSet(QAction *action, MeshModel &m, RichParameterList &parlst);
  int getPreConditions(const QAction*) const;
  int postCondition(const QAction* ) const;
  int getRequirements(const QAction*);
  bool applyFilter(const QAction* filter, MeshDocument &md, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
  FILTER_ARITY filterArity(QAction *) const {return SINGLE_MESH;}
};

#endif
