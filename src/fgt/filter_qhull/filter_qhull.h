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
/****************************************************************************
  History

****************************************************************************/

#ifndef QHULLFILTERSPLUGIN_H
#define QHULLFILTERSPLUGIN_H

#include <QObject>
#include <common/interfaces.h>

class QhullPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:

	/* naming convention :
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/

	enum { 
		FP_QHULL_CONVEX_HULL,  
		FP_QHULL_DELAUNAY_TRIANGULATION,
		FP_QHULL_VORONOI_FILTERING,
		FP_QHULL_ALPHA_COMPLEX_AND_SHAPE,
		FP_QHULL_VISIBLE_POINTS
	} ;

	QhullPlugin();
	~QhullPlugin();
	
	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
  virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
    virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	virtual FilterClass getClass(QAction *);	

};

#endif
