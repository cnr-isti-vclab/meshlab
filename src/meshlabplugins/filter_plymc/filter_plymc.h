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

#ifndef _FILTER_PLYMC_H_
#define _FILTER_PLYMC_H_

#include <common/interfaces.h>

class PlyMCPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
    enum { FP_PLYMC  } ;

	PlyMCPlugin();
	
	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
    virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
    virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
    FilterClass getClass(QAction *a);

};

#endif
