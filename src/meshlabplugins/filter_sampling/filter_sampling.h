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
$Log: sampleplugins.h,v $
Revision 1.2  2006/11/29 00:59:21  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.1  2006/09/25 09:24:39  e_cerisoli
add sampleplugins

****************************************************************************/

#ifndef FILTERDOCSAMPLINGPLUGIN_H
#define FILTERDOCSAMPLINGPLUGIN_H

#include <QObject>
#include <common/interfaces.h>

class FilterDocSampling : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum {
				FP_ELEMENT_SUBSAMPLING,
				FP_MONTECARLO_SAMPLING,
				FP_REGULAR_RECURSIVE_SAMPLING,
				FP_CLUSTERED_SAMPLING,
				FP_STRATIFIED_SAMPLING,
				FP_HAUSDORFF_DISTANCE,
				FP_TEXEL_SAMPLING,
				FP_VERTEX_RESAMPLING,
				FP_UNIFORM_MESH_RESAMPLING,
				FP_VORONOI_CLUSTERING,
				FP_VORONOI_COLORING,
				FP_DISK_COLORING,
				FP_POISSONDISK_SAMPLING,
				FP_VARIABLEDISK_SAMPLING
			} ;

	FilterDocSampling();
	
	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
  virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	virtual int getRequirements(QAction *action);
    virtual FilterClass getClass(QAction *);
};

#endif
