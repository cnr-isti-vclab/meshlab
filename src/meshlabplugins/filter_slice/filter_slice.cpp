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
#include <vcg/complex/algorithms/intersection.h>
#include <algorithm>


#include "filter_slice_functors.h"
#include <wrap/gl/glu_tesselator.h>

#include "kdtree.h"

using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

ExtraFilter_SlicePlugin::ExtraFilter_SlicePlugin ()
{
	typeList << FP_PARALLEL_PLANES
//					 <<FP_RECURSIVE_SLICE
					 <<FP_SINGLE_PLANE;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
 QString ExtraFilter_SlicePlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_PARALLEL_PLANES :  return QString("Cross section parallel planes");
		case FP_RECURSIVE_SLICE :  return QString("Cross section recursive");
    case FP_SINGLE_PLANE    :  return QString("Cross section single plane");
		default : assert(0);
	}
  return QString("error!");
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)

 QString ExtraFilter_SlicePlugin::filterInfo(FilterIDType filterId) const
{
 switch(filterId) {
		case FP_PARALLEL_PLANES :  return QString("Export one or more cross sections of the current mesh relative to one of the XY, YZ or ZX axes in svg format. By default, the cross-section goes through the middle of the object (Cross plane offset == 0).");
    case FP_RECURSIVE_SLICE :  return QString("Create a Sliceform model to cut out and assemble");
		case FP_SINGLE_PLANE :  return QString("Export once cross section of the current mesh relative to an axes in svg format.");
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
void ExtraFilter_SlicePlugin::initParameterSet(QAction *filter, MeshModel &m, RichParameterSet &parlst)
{
	//vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
  vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
	
	//common params
	/*QStringList axis;
	axis<<"X Axis"<<"Y Axis"<<"Z Axis";
	if (ID(filter)!=FP_RECURSIVE_SLICE)
		axis<<"Custom Axis";*/
	//parlst.addParam(new RichEnum   ("planeAxis", 0, axis, tr("Plane perpendicular to"), tr("The Slicing plane will be done perpendicular to the axis")));	
  //parlst.addParam(new RichEnum("units",0,QStringList()<<"cm"<<"in","Units","units in which the objects is measured"));
  parlst.addParam(new RichFloat("length",29,"Dimension on the longer axis (cm)","specify the dimension in cm of the longer axis of the current mesh, this will be the output dimension of the svg"));
	
	QStringList nn=QString(m.fullName()).split("/");
	QString name=nn.last().left(nn.last().lastIndexOf("."));
	if (name=="")
		name="Slice.svg";		
	else if (!name.endsWith(".svg"))
		name+=".svg";
  //parlst.addParam(new RichSaveFile ("filename", name, "svg","filename","Name of the svg files and of the folder contaning them, it is automatically created in the Sample folder of the Meshlab tree"));
	parlst.addParam(new RichSaveFile ("filename",name,QString("svg"),QString("Output File"),QString("Name of the svg files and of the folder containing them, it is automatically created in the Sample folder of the Meshlab tree")));
	/* OLD UNUSED */
	//parlst.addBool   ("absOffset",false,"Absolute offset", "if true the above offset is absolute is relative to the origin of the coordinate system, if false the offset is relative to the center of the bbox.");
  //parlst.addAbsPerc("planeDist", 0.0,0,m.cm.bbox.Diag(), "Distance between planes", "Step value between each plane for automatically generating cross-sections. Should be used with the bool selection above.");
		
	//filter specific params
	switch(ID(filter))
	{
    case FP_PARALLEL_PLANES :
			{
				parlst.addParam(new RichPoint3f("customAxis",Point3f(0,1,0),"Custom axis","Specify a custom axis, this is only valid if the above parameter is set to Custom"));
				parlst.addParam(new RichFloat("planeOffset",0.0,"Cross plane offset","Specify an offset of the cross-plane. The offset corresponds to the distance from the point specified in the plane reference parameter. By default (Cross plane offset == 0)"));
				// BBox min=0, BBox center=1, Origin=2
				parlst.addParam(new RichEnum ("relativeTo",0,QStringList()<<"Bounding box center"<<"Bounding box min"<<"Origin","plane reference","Specify the reference from which the planes are shifted"));				
				parlst.addParam(new RichFloat("eps",0.3,"Medium thickness","Thickness of the medium where the pieces will be cut away"));
				parlst.addParam(new RichInt("planeNum",10,"Number of Planes", "Step value between each plane for automatically generating cross-sections. Should be used with the bool selection above."));
				parlst.addParam(new RichBool("singleFile", true, "Single SVG","Automatically generate a series of cross-sections along the whole length of the object and store each plane in a separate SVG file. The distance between each plane is given by the step value below"));
				parlst.addParam(new RichBool("hideBase",true,"Hide Original Mesh","Hide the Original Mesh"));
				parlst.addParam(new RichBool("hideSlices",true,"Hide Slices","Hide the Generated Slices"));
				parlst.addParam(new RichBool("hidePlanes",false,"Hide Planes","Hide the Generated Slicing Planes"));
				parlst.addParam(new RichBool("capBase",true,"Cap input mesh holes","Eventually cap the holes of the input mesh before applying the filter"));
			}
    break;
    case FP_RECURSIVE_SLICE:
			{
				QStringList axis;
				axis<<"X Axis"<<"Y Axis"<<"Z Axis";
				parlst.addParam(new RichEnum   ("planeAxis", 0, axis, tr("Plane perpendicular to"), tr("The Slicing plane will be done perpendicular to the axis")));
				parlst.addParam(new RichFloat("eps",0.3,"Medium thickness","Thickness of the medium where the pieces will be cut away"));
				parlst.addParam(new RichBool	 ("delfather",true,"delete father", "Delete the father's mesh after slicing it"));
				parlst.addParam(new RichInt		 ("iter",2,"iterations","iterations"));
			}
			break;
		case FP_SINGLE_PLANE:
			{
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
	
	// Get common params
	Point3f planeAxis(0,0,0);
	if (ID(filter) == FP_RECURSIVE_SLICE)
	{
		int ind = parlst.getEnum("planeAxis");
		planeAxis[ind] = 1.0f; 
	}
	else 
		planeAxis=parlst.getPoint3f("customAxis")*(1/parlst.getPoint3f("customAxis").Norm());
	float length=parlst.getFloat("length");
	
	// set common SVG Properties
	float maxdim=m.mm()->cm.bbox.Dim()[m.mm()->cm.bbox.MaxDim()];
	Point3f sizeCm=m.mm()->cm.bbox.Dim()*(length/maxdim);
	// to check for dimensions with custom axis
	if (planeAxis[0]==1)
		pr.sizeCm=Point2f(sizeCm[1],sizeCm[2]);
	else if (planeAxis[1]==1)
		pr.sizeCm=Point2f(sizeCm[0],sizeCm[2]);
	else
		pr.sizeCm=Point2f(sizeCm[0],sizeCm[1]);
	Log("sizecm %fx%f",pr.sizeCm[0],pr.sizeCm[1]);

	pr.lineWidthPt=200;
	pr.scale=2/maxdim;
    //pr.crossHairs=true;

	// get filter specific params and execute filter
	switch(ID(filter))
	{
		case FP_SINGLE_PLANE:
			{			
			float planeOffset = parlst.getFloat("planeOffset");
      Point3f planeCenter;
			Plane3f slicingPlane;

			pr.numCol=1;
			pr.numRow=1;
			pr.projDir = planeAxis;
			pr.projCenter =  m.mm()->cm.bbox.Center();

			vector<MyEdgeMesh*> ev;
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
        case REF_CENTER:  planeCenter = bbox.Center()+ planeAxis*planeOffset*(bbox.Diag()/2.0);  //bbox center
				  break;
        case REF_MIN:     planeCenter = bbox.min+planeAxis*planeOffset*(bbox.Diag()/2.0);  //bbox min
					break;
        case REF_ORIG:    planeCenter = planeAxis*planeOffset;  //origin
				  break;
			}

			//planeCenter+=planeAxis*planeDist;
			slicingPlane.Init(planeCenter,planeAxis);

			//this is used to generate svg slices
			MyEdgeMesh *edgeMesh = new MyEdgeMesh();
			vcg::Intersection<CMeshO, MyEdgeMesh, float>(orig->cm, slicingPlane , *edgeMesh);
			vcg::edg::UpdateBounding<MyEdgeMesh>::Box(*edgeMesh);
			ev.push_back(edgeMesh);

			QString fname=parlst.getSaveFileName("filename");
			if(fname=="")
        fname="Slice.svg";
      if (!fname.endsWith(".svg"))
        fname+=".svg";
			vcg::edg::io::ExporterSVG<MyEdgeMesh>::Save(ev, fname.toStdString().c_str(), pr);


      SlicedEdge<CMeshO> slicededge(slicingPlane);
      SlicingFunction<CMeshO> slicingfunc(slicingPlane);
      //after the RefineE call, the mesh will have vertices with quality 
			//relative to the slicing plane
      vcg::RefineE<CMeshO, SlicingFunction<CMeshO>, SlicedEdge<CMeshO> >
           (base->cm, slicingfunc, slicededge, false, cb);
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(base->cm);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(base->cm);
			
      MeshModel *slice1= m.addNewMesh("","slice");
			//m.meshList.push_back(slice1);
			QString layername;								
			slice1->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
			vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(base->cm,VERTEX_LEFT,VERTEX_LEFT);
      vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(base->cm);
      createSlice(base,slice1);
			vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(slice1->cm);

      MeshModel* cap= m.addNewMesh("","plane");
			//m.meshList.push_back(cap);
			cap->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
			capHole(slice1,cap);
			//delete slice1;
			m.delMesh(slice1);
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(cap->cm);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(cap->cm);
			vcg::tri::UpdateBounding<CMeshO>::Box(cap->cm);
	    
			
		}
		break;
		case FP_PARALLEL_PLANES :
		{
			bool hideBase=parlst.getBool("hideBase");
			bool hideSlices=parlst.getBool("hideSlices");
			bool hidePlanes=parlst.getBool("hidePlanes");
			
			if(parlst.getBool("capBase"))
			{
        MeshModel* cap= m.addNewMesh("","slices");
				capHole(m.mm(),cap);
				tri::Append<CMeshO,CMeshO>::Mesh(m.mm()->cm, cap->cm);
				m.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				tri::UpdateTopology<CMeshO>::FaceFace(m.mm()->cm);
				//delete cap;
				m.delMesh(cap);
			}

			float maxdim=m.mm()->cm.bbox.Dim()[m.mm()->cm.bbox.MaxDim()];
			// eps to use in extrusion, is relative to maxdim
			// eps : length = eps_xtr : maxdim  -> eps_xtr = (eps*maxdim)/length
			// NOTE: change eps with eps_xtr everywhere below
			float eps=parlst.getFloat("eps");
			eps=maxdim*(eps/length);
			
			//float planeDist = parlst.getAbsPerc("planeDist");
			float planeDist=0;
			float planeOffset = parlst.getFloat("planeOffset");
			int	planeNum = parlst.getInt("planeNum");
//			int units=parlst.getEnum("units");
	
			//bool absOffset = parlst.getBool("absOffset");
      RefPlane reference=RefPlane(parlst.getEnum("relativeTo"));
			Point3f planeCenter;
			Plane3f slicingPlane;
			pr.numCol=(int)(max((int)sqrt(planeNum*1.0f),2)+1);
			pr.numRow=(int)(planeNum*1.0f/pr.numCol)+1;
			
			pr.projDir = planeAxis;
			pr.projCenter =  m.mm()->cm.bbox.Center();

			vector<MyEdgeMesh*> ev;
			if (hideBase)
				m.mm()->visible=false;
			Box3f bbox=m.mm()->cm.bbox;
			MeshModel* base=m.mm();
			MeshModel* orig=m.mm();
			for(int i=0;i<planeNum;++i)
			{
        if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(base->cm)>0 || (tri::Clean<CMeshO>::CountNonManifoldVertexFF(base->cm,false) != 0))
				{
					Log("Mesh is not two manifold, cannot apply filter");
					return false;
				}

				QString s;
				s.sprintf("calculating slice %d",i+1);
				cb((i+1)*100.0f/planeNum,s.toStdString().c_str());
				switch(reference)
				{
				  case REF_CENTER:
					  planeCenter = bbox.Center()+ planeAxis*planeOffset*(bbox.Diag()/2.0);  //bbox center
					  if (planeNum!=1)
						  planeDist=(bbox.Dim()*planeAxis)/(2*(planeNum-1));
					  break;
				  case REF_MIN:
						planeCenter = bbox.min+planeAxis*planeOffset*(bbox.Diag()/2.0);  //bbox min
						if (planeNum!=1)
							planeDist=(bbox.Dim()*planeAxis)/(planeNum-1);
						break;
				  case REF_ORIG:
					  planeCenter = planeAxis*planeOffset;  //origin
					  break;
				}

				planeCenter+=planeAxis*planeDist*i;
				slicingPlane.Init(planeCenter,planeAxis);
        SlicedEdge<CMeshO> slicededge(slicingPlane);
        SlicingFunction<CMeshO> slicingfunc(slicingPlane);
        //after the RefineE call, the mesh will have vertices with quality 
				//relative to the slicing plane
        vcg::RefineE<CMeshO, SlicingFunction<CMeshO>, SlicedEdge<CMeshO> >
             (base->cm, slicingfunc, slicededge, false, cb);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(base->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(base->cm);
				
				QString layername;
				layername.sprintf("slice_%d-%d.ply",i,i+1);
        MeshModel *slice1= m.addNewMesh("",qPrintable(layername));
				//m.meshList.push_back(slice1);
				
				slice1->setFileName(layername);								// mesh name
				slice1->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(base->cm,VERTEX_LEFT,VERTEX_LEFT);
        vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(base->cm);
				if (hideSlices)
					slice1->visible=false;

        createSlice(base,slice1);
				vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(slice1->cm);

				layername.sprintf("plane_%d.ply",i+1);
        MeshModel* cap= m.addNewMesh("",qPrintable(layername));
				//m.meshList.push_back(cap);
				
				cap->setFileName(layername);								// mesh name
				cap->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				capHole(slice1,cap);
				if (eps!=0)
				{
					layername.sprintf("plane_%d_extruded.ply",i+1);
          MeshModel* dup= m.addNewMesh("",qPrintable(layername));
					//m.meshList.push_back(dup);
					dup->setFileName(layername);								// mesh name
					dup->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);					
					extrude(&m,cap, dup, eps, planeAxis);
				}
				if (hidePlanes)
					cap->visible=false;
				tri::Append<CMeshO,CMeshO>::Mesh(slice1->cm, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(slice1->cm);
				slice1->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);					
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice1->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(slice1->cm);
				vcg::tri::UpdateBounding<CMeshO>::Box(slice1->cm);

				layername.sprintf("slice_%d-%d.ply",i+1,i+2);
        MeshModel* slice2= m.addNewMesh("",qPrintable(layername));
				//m.meshList.push_back(slice2);
				slice2->setFileName(layername);								// mesh name
				slice2->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(base->cm,VERTEX_RIGHT,VERTEX_RIGHT);
				vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(base->cm);
				createSlice(base,slice2);
				tri::Clean<CMeshO>::FlipMesh(cap->cm);
				tri::Append<CMeshO,CMeshO>::Mesh(slice2->cm, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(slice2->cm);
				slice2->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);					
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice2->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(slice2->cm);
				vcg::tri::UpdateBounding<CMeshO>::Box(slice2->cm);

				if (hideSlices)
					slice2->visible=false;
				if (i!=0)
					m.delMesh(base);
				base=slice2;
	      //this is used to generate svd slices
				MyEdgeMesh *edgeMesh = new MyEdgeMesh();
				vcg::Intersection<CMeshO, MyEdgeMesh, float>(orig->cm, slicingPlane , *edgeMesh);
				vcg::edg::UpdateBounding<MyEdgeMesh>::Box(*edgeMesh);
				ev.push_back(edgeMesh);
			}

			QString fname=parlst.getSaveFileName("filename");
			if(fname=="")
        fname="Slice.svg";
      if (!fname.endsWith(".svg"))
        fname+=".svg";
			vcg::edg::io::ExporterSVG<MyEdgeMesh>::Save(ev, fname.toStdString().c_str(), pr);
		}
		break;
		case FP_RECURSIVE_SLICE:
		{
			float maxdim=m.mm()->cm.bbox.Dim()[m.mm()->cm.bbox.MaxDim()];
			// eps to use in extrusion, is relative to maxdim
			// eps : length = eps_xtr : maxdim  -> eps_xtr = (eps*maxdim)/length
			// NOTE: change eps with eps_xtr everywhere below
			float eps=parlst.getFloat("eps");
			eps=maxdim*(eps/length);
			KDTree<CMeshO> *kdt=new KDTree<CMeshO>(&m,m.mm(), eps, parlst.getEnum("planeAxis"));
			kdt->delfather=parlst.getBool("delfather");
			int iter=parlst.getInt("iter");
			for(int i=0;i<iter;i++)
			{
				kdt->Slice(cb);
			}
			vcg::tri::UpdateBounding<CMeshO>::Box(m.mm()->cm);
    }
    break;
	}
	return true;
}

void ExtraFilter_SlicePlugin::extrude(MeshDocument* /*doc*/,MeshModel* orig, MeshModel* dest, float eps, Point3f planeAxis)
{
  tri::ExtrudeBoundary<CMeshO>(orig->cm,dest->cm,eps,planeAxis);
}
 MeshFilterInterface::FilterClass ExtraFilter_SlicePlugin::getClass(QAction *filter)
{
	switch(ID(filter))
	{
		case FP_SINGLE_PLANE:
		case FP_PARALLEL_PLANES :
		case FP_RECURSIVE_SLICE :
		return MeshFilterInterface::Measure;
		default: assert(0);
	}
	return MeshFilterInterface::Generic;
}

bool ExtraFilter_SlicePlugin::autoDialog(QAction *action)
{
  switch(ID(action))
  {
	case FP_SINGLE_PLANE:
  case  FP_RECURSIVE_SLICE:
  case FP_PARALLEL_PLANES: return true;
	default: return false;
  }
}

void ExtraFilter_SlicePlugin::createSlice(MeshModel* currentMesh, MeshModel* destMesh)
{
  tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(currentMesh->cm);
  tri::Append<CMeshO,CMeshO>::Mesh(destMesh->cm, currentMesh->cm, true);
  tri::UpdateTopology<CMeshO>::FaceFace(destMesh->cm);
  tri::UpdateBounding<CMeshO>::Box(destMesh->cm);						// updates bounding box
  destMesh->cm.Tr = currentMesh->cm.Tr;								// copy transformation
}

void ExtraFilter_SlicePlugin::capHole(MeshModel* currentMesh, MeshModel* destMesh, int capDir)
{

  std::vector< std::vector<Point3f> > outlines;
  std::vector<Point3f> outline;
  vcg::tri::UpdateFlags<CMeshO>::VertexClearV(currentMesh->cm);
  vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(currentMesh->cm);
  int nv=0;
  for(size_t i=0;i<currentMesh->cm.face.size();i++)
  {
    for (int j=0;j<3;j++)
    if (!currentMesh->cm.face[i].IsV() && currentMesh->cm.face[i].IsB(j))
    {
      CFaceO* startB=&(currentMesh->cm.face[i]);
      vcg::face::Pos<CFaceO> p(startB,j);
	    do
      {
				p.V()->SetV();
        outline.push_back(p.V()->P());
        p.NextB();
				nv++;
      }
      while(!p.V()->IsV());
			if (capDir==CAP_CCW)
				std::reverse(outline.begin(),outline.end());

      outlines.push_back(outline);
      outline.clear();
    }
  }
	if (nv<2) return;
  CMeshO::VertexIterator vi=vcg::tri::Allocator<CMeshO>::AddVertices(destMesh->cm,nv);
  for (size_t i=0;i<outlines.size();i++)
  {
    for(size_t j=0;j<outlines[i].size();++j,++vi)
			(&*vi)->P()=outlines[i][j];
  }

  std::vector<int> indices;
  glu_tesselator::tesselate(outlines, indices);
//  Log(0,"%d new faces",indices.size());
  std::vector<Point3f> points;
  glu_tesselator::unroll(outlines, points);
  CMeshO::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(destMesh->cm,nv-2);
  for (size_t i=0; i<indices.size(); i+=3,++fi)
  {
    (*&fi)->V(0)=&destMesh->cm.vert[ indices[i+0] ];
    (*&fi)->V(1)=&destMesh->cm.vert[ indices[i+1] ];
    (*&fi)->V(2)=&destMesh->cm.vert[ indices[i+2] ];
  }
	tri::Clean<CMeshO>::RemoveDuplicateVertex(destMesh->cm);
  //vcg::tri::UpdateTopology<CMeshO>::FaceFace(destMesh->cm);
  vcg::tri::UpdateBounding<CMeshO>::Box(destMesh->cm);
}

Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
