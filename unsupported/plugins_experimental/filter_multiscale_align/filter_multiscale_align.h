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

#ifndef FILTER_MULTISCALE_ALIGN_H
#define FILTER_MULTISCALE_ALIGN_H
#include <common/interfaces.h>
#include "multiscale_align.h"

class FilterMultiscaleAlign : public QObject, public MeshFilterInterface
{
	Q_OBJECT
		MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

private:
    MultiscaleAlign _msa;
   
public:
	enum { MS_ALIGN } ;

	FilterMultiscaleAlign();

	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(QAction *);
    void initParameterSet(QAction *,MeshDocument & md, RichParameterSet & /*parent*/);
	bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;

    float poissonDiskSampling(MeshDocument &md, MeshModel *refMesh, QString label, float numSamples, bool checkBord=false);
    float selectAllPoints(MeshDocument &md, MeshModel *mesh, QString label);
	
	QString pluginName(void) const { return "FilterMultiscaleAlign"; }
	FILTER_ARITY filterArity(QAction*) const;

	
};

#endif
