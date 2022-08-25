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

#ifndef SAMPLEFILTERSPLUGIN_H
#define SAMPLEFILTERSPLUGIN_H

#include <QObject>

#include <common/interfaces.h>

class SlidingCurvaturePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_COMPUTE_PRINC_CURV_DIR_FROM_SLIDING  } ;

	SlidingCurvaturePlugin();
	
	virtual QString filterName(FilterIDType filter);
	virtual QString filterInfo(FilterIDType filter);
	virtual bool autoDialog(QAction *) {return true;}
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	virtual int getRequirements(QAction *);
	const FilterClass getClass(QAction *a);

};

#endif
