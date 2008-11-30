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


#include <QtGui>
#include <qfiledialog.h>
#include <limits>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <qstring.h>

#include "filter_slice.h"

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component_ocf.h>
#include <vcg/space/point3.h>
#include <vcg/space/box3.h>
#include <vcg/space/index/grid_closest.h>
#include <vcg/complex/intersection.h>
#include <vcg/complex/edgemesh/update/bounding.h>
#include <vcg/complex/trimesh/update/bounding.h>

using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraFilter_SlicePlugin::ExtraFilter_SlicePlugin ()
{
	typeList << FP_PLANE;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
const QString ExtraFilter_SlicePlugin::filterName(FilterIDType filterId)
{
  switch(filterId) {
		case FP_PLANE :  return QString("Cross section plane");
		default : assert(0);
	}
  return QString("error!");
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
const QString ExtraFilter_SlicePlugin::filterInfo(FilterIDType filterId)
{
 switch(filterId) {
		case FP_PLANE :  return QString("Export one or more cross sections of the current mesh relative to one of the XY, YZ or ZX axes in svg format. By default, the cross-section goes through the middle of the object (Cross plane offset == 0).");

		default : assert(0);
	}
  return QString("error!");
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
//void ExtraSamplePlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst)
void ExtraFilter_SlicePlugin::initParameterSet(QAction *filter, MeshModel &m, FilterParameterSet &parlst)
{
  vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
	switch(ID(filter))
	{
    case FP_PLANE :
    {
      QStringList metrics;
      metrics.push_back("XY Axis");
      metrics.push_back("YZ Axis");
      metrics.push_back("ZX Axis");
      parlst.addEnum   ("planeAxis", 0, metrics, tr("Axis"), tr("The Slicing plane will be done perpendicular to the axis"));
      parlst.addFloat  ("planeOffset", 0.0, "Cross plane offset", "Specify an offset of the cross-plane. The offset corresponds to the distance between the center of the object and the point where the plan crosses it. By default (Cross plane offset == 0), the plane crosses the center of the object, so the offset can be positive or negetive");
      // Origin=0, BBox min=1, BBox center=2
      parlst.addEnum   ("relativeTo",0,QStringList()<<"Origin"<<"Bounding box min"<<"Bounding box Center","plane reference","Specify the reference from which the planes are shifted");
      //parlst.addBool   ("absOffset",false,"Absolute offset", "if true the above offset is absolute is relative to the origin of the coordinate system, if false the offset is relative to the center of the bbox.");
      parlst.addAbsPerc("planeDist", 0.0,0,m.cm.bbox.Diag(), "Distance between planes", "Step value between each plane for automatically generating cross-sections. Should be used with the bool selection above.");
      parlst.addInt    ("planeNum", 1, "Number of Planes", "Step value between each plane for automatically generating cross-sections. Should be used with the bool selection above.");
      parlst.addString ("filename", "Slice", "filename","Name of the svg files and of the folder contaning them, it is automatically created in the Sample folder of the Meshlab tree");
      parlst.addBool   ("singleFile", true, "Single SVG","Automatically generate a series of cross-sections along the whole length of the object and store each plane in a separate SVG file. The distance between each plane is given by the step value below");
    }
      break;
    default : assert(0);
  }
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool ExtraFilter_SlicePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &parlst, vcg::CallBackPos *)
{
	switch(ID(filter))
	{
		case FP_PLANE :
		{
			Point3f planeAxis(0,0,0);
			int axisIndex = parlst.getEnum("planeAxis");
			assert(axisIndex >=0 && axisIndex <3);
			planeAxis[axisIndex] = 1.0f;
			float planeDist = parlst.getAbsPerc("planeDist");
			float planeOffset = parlst.getFloat("planeOffset");
			int	planeNum = parlst.getInt("planeNum");
			//bool absOffset = parlst.getBool("absOffset");
      int reference=parlst.getEnum("relativeTo");
			Point3f planeCenter;
			Plane3f slicingPlane;
			pr.numCol=max((int)sqrt(planeNum),2);
			pr.numRow=max(planeNum/pr.numCol,2);
			pr.sizeCm=Point2f(4,4);
			pr.projDir = planeAxis;
			pr.projCenter =  m.cm.bbox.Center();
			pr.scale = 2.0/m.cm.bbox.Diag();
			pr.lineWidthPt=200;
			vector<MyEdgeMesh*> ev;

			for(int i=0;i<planeNum;++i)
			{
				//if(absOffset==false) planeCenter = m.cm.bbox.Center() + planeAxis*planeOffset*(m.cm.bbox.Diag()/2.0);
				//			          else planeCenter = Point3f(0,0,0) + planeAxis*planeOffset;
				switch(reference)
				{
				  case 0: planeCenter = planeAxis*planeOffset;  //origin
            break;
				  case 1: planeCenter = m.cm.bbox.min+planeAxis*planeOffset*(m.cm.bbox.Diag()/2.0);  //bbox min
            break;
				  case 2: planeCenter = m.cm.bbox.Center()+ planeAxis*planeOffset*(m.cm.bbox.Diag()/2.0);  //bbox min
            break;
				}

				planeCenter+=planeAxis*planeDist*i;
				slicingPlane.Init(planeCenter,planeAxis);
				Box3f &bb=m.cm.bbox;
				Log(0,"Slicing a mesh with bb (%5.2f %5.2f %5.2f) -  (%5.2f %5.2f %5.2f)",bb.min[0],bb.min[1],bb.min[2],bb.max[0],bb.max[1],bb.max[2]);
				Log(0,"Crossing at (%5.2f %5.2f %5.2f)",planeCenter[0],planeCenter[1],planeCenter[2]);

				MyEdgeMesh *edgeMesh = new MyEdgeMesh();
				vcg::Intersection<CMeshO, MyEdgeMesh, float>(m.cm, slicingPlane , *edgeMesh);
				vcg::edg::UpdateBounding<MyEdgeMesh>::Box(*edgeMesh);
				ev.push_back(edgeMesh);
			}

			QString fname=parlst.getString("filename");
			if(fname=="")
        fname="Slice.svg";
      if (!fname.endsWith(".svg"))
        fname+=".svg";
			vcg::edg::io::ExporterSVG<MyEdgeMesh>::Save(ev, fname.toStdString().c_str(), pr);
		}
		break;
	}
	return true;
}

const MeshFilterInterface::FilterClass ExtraFilter_SlicePlugin::getClass(QAction *filter)
{
	switch(ID(filter))
	{
		case FP_PLANE : return MeshFilterInterface::Generic;
		default: assert(0);
	}
	return MeshFilterInterface::Generic;
}

bool ExtraFilter_SlicePlugin::autoDialog(QAction *action)
{
  switch(ID(action))
  {
  case  FP_PLANE: return true;
	default: return false;
  }
}

Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
