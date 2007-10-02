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
Revision 1.7  2007/10/02 08:13:46  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.6  2007/04/16 09:25:30  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing again...

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
	enum { 
		FP_SELECT_ALL, 
		FP_SELECT_NONE, 
		FP_SELECT_INVERT, 
		FP_SELECT_DELETE, 
		FP_SELECT_ERODE, 
		FP_SELECT_DILATE, 
		FP_SELECT_BORDER_FACES
	} ;

	SelectionFilterPlugin();
	~SelectionFilterPlugin();
	virtual const QString filterInfo(FilterIDType filter);
  virtual const QString filterName(FilterIDType filter);
	virtual const PluginInfo &pluginInfo();
  
  virtual const FilterClass getClass(QAction *) {return MeshFilterInterface::Selection;};
  
	virtual const int getRequirements(QAction *);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
		
	
protected:

//	ActionInfo *ai;

};

#endif
