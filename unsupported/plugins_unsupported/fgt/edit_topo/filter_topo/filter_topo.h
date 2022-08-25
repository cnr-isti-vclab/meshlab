/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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

#ifndef FILTER_TOPOPLUGIN_H
#define FILTER_TOPOPLUGIN_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


//**************************************************************
//	class FilterTopoPlugin 
//		Main filter plugin class
//
class FilterTopoPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_RE_TOPO  } ;

	FilterTopoPlugin();
	MeshDocument *doc;
	
	virtual QString filterName(FilterIDType filter);
	virtual QString filterInfo(FilterIDType filter);
	virtual bool autoDialog(QAction *) {return true;}
	virtual void initParameterSet(QAction *,MeshDocument &/*m*/, FilterParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter,  MeshModel &/*m*/, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
};

#endif
