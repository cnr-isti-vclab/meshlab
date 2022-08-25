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

#ifndef FILTER_3DCOFORM_H
#define FILTER_3DCOFORM_H

#include <QObject>

#include <common/interfaces.h>
#include "coform_interface.h"

class Filter3DCoformPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

	CoformGUI* cofGui;
public:
	enum { F3DC_RI_INTEGRATION};

	Filter3DCoformPlugin();
	virtual QString filterName(FilterIDType filterId) const;
	virtual QString filterInfo(FilterIDType filterId) const;
	virtual MeshFilterInterface::FilterClass getClass(QAction *);
	virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *act, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
};

#endif