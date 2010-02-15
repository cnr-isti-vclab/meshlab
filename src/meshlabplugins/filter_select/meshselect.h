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
	Q_INTERFACES(MeshFilterInterface)
		
		public:
	/* naming convention : 
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { 
		FP_SELECT_ALL, 
		FP_SELECT_NONE, 
		FP_SELECT_INVERT, 
		FP_SELECT_DELETE_FACE, 
		FP_SELECT_DELETE_FACEVERT, 
		FP_SELECT_ERODE, 
		FP_SELECT_DILATE, 
		FP_SELECT_BORDER_FACES,
		FP_SELECT_BY_QUALITY,
		FP_SELECT_BY_RANGE,
		FP_SELECT_BY_COLOR
	} ;

	SelectionFilterPlugin();
	~SelectionFilterPlugin();
	virtual QString filterInfo(FilterIDType filter) const;
  virtual QString filterName(FilterIDType filter) const;
  
  virtual FilterClass getClass(QAction *) {return MeshFilterInterface::Selection;};
  void initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst);
    virtual int postCondition( QAction* ) const {return MeshModel::MM_VERTFLAGSELECT | MeshModel::MM_FACEFLAGSELECT;};

	virtual int getRequirements(QAction *);
    virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
};

#endif
