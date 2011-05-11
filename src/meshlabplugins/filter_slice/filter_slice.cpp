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

#include "filter_slice.h"
#include <vcg/complex/algorithms/intersection.h>
#include <vcg/complex/algorithms/update/topology.h>

#include "filter_slice_functors.h"
#include <wrap/gl/glu_tessellator_cap.h>

using namespace std;
using namespace vcg;

ExtraFilter_SlicePlugin::ExtraFilter_SlicePlugin ()
{
  typeList << FP_SINGLE_PLANE;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

 QString ExtraFilter_SlicePlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_SINGLE_PLANE    :  return QString("Planar Section");
		default : assert(0);
	}
  return QString("error!");
}

 QString ExtraFilter_SlicePlugin::filterInfo(FilterIDType filterId) const
{
 switch(filterId) {
    case FP_SINGLE_PLANE :  return QString("Compute a planar section of a given mesh.");
		default : assert(0);
	}
  return QString("error!");
}

void ExtraFilter_SlicePlugin::initParameterSet(QAction *filter, MeshModel &m, RichParameterSet &parlst)
{
	switch(ID(filter))
	{
    case FP_SINGLE_PLANE:
    {
      QStringList axis = QStringList() <<"X Axis"<<"Y Axis"<<"Z Axis";
      parlst.addParam(new RichEnum   ("planeAxis", 0, axis, tr("Plane perpendicular to"), tr("The Slicing plane will be done perpendicular to the axis")));
      parlst.addParam(new RichSaveFile ("filename","output.svg",QString("svg"),QString("Output File"),QString("Name of the svg files and of the folder containing them, it is automatically created in the Sample folder of the Meshlab tree")));
      parlst.addParam(new RichFloat("length",29,"Dimension on the longer axis (cm)","specify the dimension in cm of the longer axis of the current mesh, this will be the output dimension of the svg"));
      parlst.addParam(new RichPoint3f("customAxis",Point3f(0,1,0),"Custom axis","Specify a custom axis, this is only valid if the above parameter is set to Custom"));
      parlst.addParam(new RichFloat  ("planeOffset", 0.0, "Cross plane offset", "Specify an offset of the cross-plane. The offset corresponds to the distance from the point specified in the plane reference parameter. By default (Cross plane offset == 0)"));
      // BBox min=0, BBox center=1, Origin=2
      parlst.addParam(new RichEnum   ("relativeTo",0,QStringList()<<"Bounding box center"<<"Bounding box min"<<"Origin","plane reference","Specify the reference from which the planes are shifted"));
      }
      break;
    default : assert(0);
  }
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool ExtraFilter_SlicePlugin::applyFilter(QAction *filter, MeshDocument &m, RichParameterSet &parlst, vcg::CallBackPos *cb)
{
	vcg::tri::UpdateBounding<CMeshO>::Box(m.mm()->cm);
	
  switch(ID(filter))
  {
    case FP_SINGLE_PLANE:
      {
        Point3f planeAxis(0,0,0);
        int ind = parlst.getEnum("planeAxis");
        if(ind>=0 && ind<3)   planeAxis[ind] = 1.0f;
        else planeAxis=parlst.getPoint3f("customAxis");

        planeAxis.Normalize();
        float length=parlst.getFloat("length");

        // set common SVG Properties
        float maxdim=m.mm()->cm.bbox.Dim()[m.mm()->cm.bbox.MaxDim()];
        Point3f sizeCm=m.mm()->cm.bbox.Dim()*(length/maxdim);
        // to check for dimensions with custom axis
        if (planeAxis[0]==1) pr.sizeCm=Point2f(sizeCm[1],sizeCm[2]);
        if (planeAxis[1]==1) pr.sizeCm=Point2f(sizeCm[0],sizeCm[2]);
        if (planeAxis[2]==1) pr.sizeCm=Point2f(sizeCm[0],sizeCm[1]);

        Log("sizecm %fx%f",pr.sizeCm[0],pr.sizeCm[1]);

        pr.lineWidthPt=200;
        pr.scale=2/maxdim;
        float planeOffset = parlst.getFloat("planeOffset");
        Point3f planeCenter;
        Plane3f slicingPlane;

        pr.numCol=1;
        pr.numRow=1;
        pr.projDir = planeAxis;
        pr.projCenter =  m.mm()->cm.bbox.Center();

//        vector<MyEdgeMesh*> ev;
        Box3f bbox=m.mm()->cm.bbox;
        MeshModel* base=m.mm();
        MeshModel* orig=m.mm();


        //actual cut of the mesh

        if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(base->cm)>0 || (tri::Clean<CMeshO>::CountNonManifoldVertexFF(base->cm,false) != 0))
        {
          Log("Mesh is not two manifold, cannot apply filter");
          return false;
        }

        switch(RefPlane(parlst.getEnum("relativeTo")))
        {
          case REF_CENTER:  planeCenter = bbox.Center()+ planeAxis*planeOffset*(bbox.Diag()/2.0);      break;
          case REF_MIN:     planeCenter = bbox.min+planeAxis*planeOffset*(bbox.Diag()/2.0);    break;
          case REF_ORIG:    planeCenter = planeAxis*planeOffset;  break;
        }

        //planeCenter+=planeAxis*planeDist ;
        slicingPlane.Init(planeCenter,planeAxis);

        //this is used to generate svg slices
        MeshModel* cap= m.addNewMesh("","EdgeMesh");
        vcg::IntersectionPlaneMesh<CMeshO, CMeshO, float>(orig->cm, slicingPlane, cap->cm );
        tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);

        MeshModel* cap2= m.addNewMesh("","CappedSlice");
        tri::CapEdgeMesh(cap->cm, cap2->cm);
      }
	}
	return true;
}

MeshFilterInterface::FilterClass ExtraFilter_SlicePlugin::getClass(QAction *filter)
{
	switch(ID(filter))
	{
		case FP_SINGLE_PLANE:
		return MeshFilterInterface::Measure;
		default: assert(0);
	}
	return MeshFilterInterface::Generic;
}

 Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
