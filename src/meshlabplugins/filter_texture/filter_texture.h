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

#ifndef _FILTER_TEXTURE_H
#define _FILTER_TEXTURE_H

#include <QObject>
#include <QTime>

#include <vcg/complex/trimesh/attribute_seam.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/space/triangle2.h>
#include <common/interfaces.h>

class FilterTexturePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum {
		FP_UV_TO_COLOR,
		FP_UV_WEDGE_TO_VERTEX,
		FP_BASIC_TRIANGLE_MAPPING,
		FP_SET_TEXTURE,
		FP_COLOR_TO_TEXTURE,
		FP_TRANSFER_TO_TEXTURE,
		FP_TEX_TO_VCOLOR_TRANSFER
	};

	FilterTexturePlugin();
	
	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
    virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
    virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb);
	virtual int getRequirements(QAction *);
	virtual int getPreConditions(QAction *) const;
	virtual int postCondition( QAction* ) const;
    FilterClass getClass(QAction *a);

};

#endif
