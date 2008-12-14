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
//#include <stdlib.h>
#include <vcg/complex/intersection.h>
#include <vcg/complex/edgemesh/update/bounding.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <algorithm>


#include "filter_slice_functors.h"
#include <wrap/gl/glu_tesselator.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/space/planar_polygon_tessellation.h>
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
      parlst.addEnum   ("relativeTo",0,QStringList()<<"Bounding box Center"<<"Bounding box min"<<"Origin","plane reference","Specify the reference from which the planes are shifted");
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
bool ExtraFilter_SlicePlugin::applyFilter(QAction *filter, MeshDocument &m, FilterParameterSet &parlst, vcg::CallBackPos *cb)
{
  if (!tri::Clean<CMeshO>::IsTwoManifoldFace(m.mm()->cm) || (tri::Clean<CMeshO>::CountNonManifoldVertexFF(m.mm()->cm,false) != 0))
  {
    Log(0,"Mesh is not two manifold, cannot apply filter");
    return false;
  }
  else
    Log(0,"Mesh is two manifold");

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
			pr.projCenter =  m.mm()->cm.bbox.Center();
			pr.scale = 2.0/m.mm()->cm.bbox.Diag();
			pr.lineWidthPt=200;
			vector<MyEdgeMesh*> ev;
      m.mm()->visible=false;
			for(int i=0;i<planeNum;++i)
			{
				switch(reference)
				{
				  case 2: planeCenter = planeAxis*planeOffset;  //origin
            break;
				  case 1: planeCenter = m.mm()->cm.bbox.min+planeAxis*planeOffset*(m.mm()->cm.bbox.Diag()/2.0);  //bbox min
            break;
				  case 0: planeCenter = m.mm()->cm.bbox.Center()+ planeAxis*planeOffset*(m.mm()->cm.bbox.Diag()/2.0);  //bbox min
            break;
				}

				planeCenter+=planeAxis*planeDist*i;
				slicingPlane.Init(planeCenter,planeAxis);
				//clear the selection flags

        SlicedEdge<CMeshO> slicededge(slicingPlane);
        SlicingFunction<CMeshO> slicingfunc(slicingPlane);
        //after the RefineE call, the mesh will be half vertices selected
        vcg::RefineE<CMeshO, SlicingFunction<CMeshO>, SlicedEdge<CMeshO> >
             (m.mm()->cm, slicingfunc, slicededge, false, cb);
        vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(m.mm()->cm);

        vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(m.mm()->cm,VERTEX_LEFT,VERTEX_LEFT);
        vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.mm()->cm);
        createSlice(m);
        capHole(m);

        vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(m.mm()->cm,VERTEX_RIGHT,VERTEX_RIGHT);
        vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.mm()->cm);
        createSlice(m);
        capHole(m);

        /*
        this is for generating the svd slices

				MyEdgeMesh *edgeMesh = new MyEdgeMesh();
				vcg::Intersection<CMeshO, MyEdgeMesh, float>(m.mm()->cm, slicingPlane , *edgeMesh);

				vcg::edg::UpdateBounding<MyEdgeMesh>::Box(*edgeMesh);

				ev.push_back(edgeMesh);
				*/
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

void ExtraFilter_SlicePlugin::createSlice(MeshDocument& m)
{
  MeshModel *mm= new MeshModel();
  m.meshList.push_back(mm);
  MeshModel *destMesh     = m.meshList.back();	// destination = last
  MeshModel *currentMesh  = m.mm();				// source = current

  tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, m.mm()->cm, true);
  destMesh->fileName = "newmesh.ply";								// mesh name
  tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
  destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
}

void ExtraFilter_SlicePlugin::capHole(MeshDocument& m)
{

  std::vector<Point3f> outline;
  for(int i=0;i<m.mm()->cm.vert.size();i++)
    if (m.mm()->cm.vert[i].Q()==VERTEX_SLICE)
      outline.push_back(m.mm()->cm.vert[i].P());

  // tesselation input: each outline represents a polygon contour
  std::vector< std::vector<Point3f> > outlines;
  outlines.push_back(outline);
  Log(0,"%d points",outline.size());
  // tesselation output (triangles indices)
  std::vector<int> indices;

  // compute triangles indices
  glu_tesselator::tesselate(outlines, indices);

  // unroll input contours points
  std::vector<Point3f> points;

  glu_tesselator::unroll(outlines, points);

  CMeshO::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(m.mm()->cm,indices.size());
  Log(0,"%d new faces",indices.size());
  // create triangles
  for (size_t i=0; i<indices.size(); i+=3,++fi)
  {
    (*&fi)->V0(0)->P()=points[ indices[i+0] ];
    (*&fi)->V1(0)->P()=points[ indices[i+1] ];
    (*&fi)->V2(0)->P()=points[ indices[i+2] ];
  }

  vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.mm()->cm);
}

Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
