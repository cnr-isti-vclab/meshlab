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
/* History
$Log$
Revision 1.5  2007/02/08 15:59:46  cignoni
Added Border selection filters

Revision 1.4  2006/11/29 00:59:20  cignoni
Cleaned plugins interface; changed useless help class into a plain string

****************************************************************************/

#ifndef EDITPLUGIN_H
#define EDITPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class SelectionFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)
		
		public:
	/* naming convention : 
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { FP_SELECT_ALL, FP_SELECT_NONE, FP_SELECT_INVERT, FP_SELECT_DELETE, FP_SELECT_ERODE, FP_SELECT_DILATE, FP_SELECT_BORDER_FACES} ;

	SelectionFilterPlugin();
	~SelectionFilterPlugin();
	virtual const QString Info(QAction *);
	virtual const PluginInfo &Info();
  
  virtual const QString ST(FilterType filter);
  virtual const FilterClass getClass(QAction *) {return MeshFilterInterface::Selection;};
  virtual bool getParameters(QAction *, QWidget *, MeshModel &m, FilterParameter &par){return true;};
  virtual const int getRequirements(QAction *);
  virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameter & /*parent*/, vcg::CallBackPos * cb) ;

protected:

//	ActionInfo *ai;

};

#endif
