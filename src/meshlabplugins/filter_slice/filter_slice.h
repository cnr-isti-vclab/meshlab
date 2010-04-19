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
#include <common/interfaces.h>
#include <vcg/complex/used_types.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/edge/base.h>
#include <vcg/complex/edgemesh/base.h>

#include <vcg/simplex/vertex/component.h>

#include <wrap/io_edgemesh/export_svg.h>

#include <vcg/space/plane3.h>
//#include "svgpro.h"



class MyVertex;
class MyEdge;
class MyFace;

class MyUsedTypes: public vcg::UsedTypes < vcg::Use<MyVertex>::AsVertexType,vcg::Use<MyFace>::AsFaceType,vcg::Use<MyEdge>::AsEdgeType>{};

class MyVertex: public vcg::Vertex < MyUsedTypes,vcg::vertex::Coord3f,vcg::vertex::BitFlags,vcg::vertex::VEAdj>{};
class MyFace: public vcg::Face < MyUsedTypes, vcg::face::VertexRef>{};
class MyEdge    : public vcg::Edge <MyUsedTypes,vcg::edge::VertexRef> {};

class MyEdgeMesh: public vcg::edg::EdgeMesh< std::vector<MyVertex>, std::vector<MyEdge> > {};

typedef vcg::edg::io::SVGProperties SVGProperties;

class ExtraFilter_SlicePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_SINGLE_PLANE, FP_PARALLEL_PLANES, FP_RECURSIVE_SLICE };
	enum { CAP_CW, CAP_CCW };
	enum RefPlane { REF_CENTER,REF_MIN,REF_ORIG};
	ExtraFilter_SlicePlugin();
	~ExtraFilter_SlicePlugin(){};

	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
	virtual bool autoDialog(QAction *);
	virtual FilterClass getClass(QAction *);
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	virtual int getRequirements(QAction *){return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_VERTFLAG | MeshModel::MM_VERTMARK | MeshModel::MM_VERTCOORD;}

	static void capHole(MeshModel* orig, MeshModel* dest, int capDir=CAP_CW);
	static void extrude(MeshModel* orig, MeshModel* dest, float eps, vcg::Point3f planeAxis);
private:
	SVGProperties pr;
	void createSlice(MeshModel* orig,MeshModel* dest);
	
	
};

#endif
