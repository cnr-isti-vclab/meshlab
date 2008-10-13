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


#ifndef FILTER_SLICE_H
#define FILTER_SLICE_H

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QList>
#include <QStringList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/math/base.h>
#include <vcg/math/histogram.h>
#include <vcg/space/triangle3.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/simplex/vertex/vertex.h>
#include <vcg/complex/edgemesh/base.h>
#include <vcg/simplex/edge/edge.h>

#include <wrap/io_edgemesh/export_svg.h>

//#include "svgpro.h"


typedef CMeshO n_Mesh;

class n_Edge;
class n_Face;
class n_Vertex  : public vcg::Vertex<float, n_Edge, n_Face> {};
class n_Edge    : public vcg::Edge<n_Edge, n_Vertex> {};


class n_EdgeMesh: public vcg::edge::EdgeMesh< std::vector<n_Vertex>, std::vector<n_Edge> > {};

typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
typedef vcg::edge::EdgeMesh<std::vector<n_Vertex>,std::vector<n_Edge> > Edge_Mesh;
typedef vcg::edge::io::SVGProperties SVGProperties;

class ExtraFilter_SlicePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_PLANE };

	ExtraFilter_SlicePlugin();
	~ExtraFilter_SlicePlugin(){};
	
	virtual const QString filterName(FilterIDType filter);
	virtual const QString filterInfo(FilterIDType filter);
	virtual bool autoDialog(QAction *);
	virtual const FilterClass getClass(QAction *);
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;

private:
	TriMeshGrid *mesh_grid;
	n_EdgeMesh *edge_mesh;
	n_Mesh trimesh;
	std::vector< TriMeshGrid::Cell *> intersected_cells;
	std::vector<vcg::Point3f> point_Vector;
	SVGProperties pr;
	QString fileName, fileN, Number, number, StepNumber, folderN;
	vcg::Box3f b;
	float edgeMax;
};

#endif
