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


#ifndef GEOMETRYAGINGPLUGIN_H
#define GEOMETRYAGINGPLUGIN_H


#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/space/index/grid_static_ptr.h>

#include "edgepred.h"


class GeometryAgingPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

	public:
		enum {FP_ERODE};
	
		GeometryAgingPlugin();
		virtual ~GeometryAgingPlugin();
	
		virtual const QString filterInfo(FilterIDType filter);
		virtual const QString filterName(FilterIDType filter);

		virtual const int getRequirements(QAction *) {return (MeshModel::MM_FACEMARK | 
				MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_CURV);}
		virtual bool autoDialog(QAction *) {return true;}
		virtual void initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &params);
		virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb);
		
	protected:
		void refineMesh(CMeshO &m, QualityEdgePred &ep, bool selection, vcg::CallBackPos *cb);
		double generateNoiseValue(int Octaves, const CVertexO::CoordType &p);
		bool faceIntersections(CMeshO &m, face::Pos<CMeshO::FaceType> p, GridStaticPtr<CFaceO, CMeshO::ScalarType> &gM);
		void smoothPeaks(CMeshO &m, bool selected, bool updateErosionAttr);
		void computeMeanCurvature(CMeshO &m);
};


#endif
