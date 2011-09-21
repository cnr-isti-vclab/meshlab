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

#ifndef EXTRACOLORIZEPLUGIN_H
#define EXTRACOLORIZEPLUGIN_H


#include <common/interfaces.h>

class ExtraMeshColorizePlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:

    enum  {
      CP_CLAMP_QUALITY,
      CP_SATURATE_QUALITY,
      CP_MAP_VQUALITY_INTO_COLOR,
      CP_MAP_FQUALITY_INTO_COLOR,
      CP_DISCRETE_CURVATURE,
      CP_TRIANGLE_QUALITY,
      CP_VERTEX_SMOOTH,
      CP_FACE_SMOOTH,
      CP_FACE_TO_VERTEX,
			CP_TEXTURE_TO_VERTEX,
      CP_VERTEX_TO_FACE,
      CP_COLOR_NON_TOPO_COHERENT,
      CP_RANDOM_FACE,
      CP_RANDOM_CONNECTED_COMPONENT
    };

    

    ExtraMeshColorizePlugin();
    ~ExtraMeshColorizePlugin(){}
  
 QString filterName(FilterIDType filter) const;
 QString filterInfo(FilterIDType filterId) const;
	int getPreConditions(QAction *) const;
	int postCondition( QAction* ) const;
	FilterClass getClass(QAction *);
	void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
	bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
};

#endif
