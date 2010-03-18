/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef SLIDEPLUGIN_H
#define SLIDEPLUGIN_H

#include "svgpro.h"
#include "slicedialog.h"
#include <common/interfaces.h>
#include <wrap/gui/trackball.h>

#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/edgemesh/base.h>
#include <vcg/simplex/edge/base.h>
#include <vcg/simplex/edge/component.h>
#include <wrap/io_edgemesh/export_svg.h>

typedef CMeshO n_Mesh;

class n_Vertex;
class n_Face;
class n_Edge;

class n_UsedTypes: public vcg::UsedTypes < vcg::Use<n_Vertex>::AsVertexType,
                                          vcg::Use<n_Edge   >::AsEdgeType,
                                          vcg::Use<n_Face  >::AsFaceType >{};

class n_Vertex  : public vcg::Vertex<n_UsedTypes, vcg::vertex::Coord3f, vcg::vertex::BitFlags> {};
class n_Edge    : public vcg::Edge<n_UsedTypes, vcg::edge::VertexRef> {};


class n_EdgeMesh: public vcg::edg::EdgeMesh< std::vector<n_Vertex>, std::vector<n_Edge> > {};

typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
typedef vcg::edg::EdgeMesh<std::vector<n_Vertex>,std::vector<n_Edge> > Edge_Mesh;
typedef vcg::edg::io::SVGProperties SVGProperties;



class ExtraMeshSlidePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	
	ExtraMeshSlidePlugin();
    vcg::Trackball trackball_slice;
	virtual ~ExtraMeshSlidePlugin();
	static const QString Info();
	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	
private:
	TriMeshGrid* mesh_grid;
	n_EdgeMesh* edge_mesh;
	n_Mesh trimesh;
	std::vector< TriMeshGrid::Cell *> intersected_cells;
	std::vector<vcg::Point3f> point_Vector;
	QString fileName, dirName, fileN;
	bool isDragging;
	GLArea * gla;
	MeshModel *m;
  vcg::Box3f b;
	SVGPro *svgpro;
	float edgeMax;
SVGProperties pr;
	dialogslice *dialogsliceobj;
    void DrawPlane(GLArea * gla,MeshModel &m);
	void UpdateVal(SVGPro * sv, SVGProperties * pr);
	bool  activeDefaultTrackball;
    bool disableTransision;

public Q_SLOTS:
	void RestoreDefault();
	void SlotExportButton();
	void upGlA();
};
#endif
