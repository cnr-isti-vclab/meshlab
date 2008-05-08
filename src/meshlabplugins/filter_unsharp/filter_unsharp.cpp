/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

 FilterUnsharp.cpp,v
 Revision 1.1  2007/12/13 00:33:55  cignoni
 New small samples

 Revision 1.1  2007/12/02 07:57:48  cignoni
 Added the new sample filter plugin that removes border faces

 
*****************************************************************************/
#include <Qt>
#include <QtGui>
#include "filter_unsharp.h"

#include <vcg/complex/trimesh/update/quality.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/crease_cut.h>


using namespace vcg;
using namespace std;

FilterUnsharp::FilterUnsharp() 
{
  typeList <<  FP_CREASE_CUT << 
		FP_VERTEX_QUALITY_SMOOTHING<<
		FP_FACE_NORMAL_SMOOTHING<<
		FP_UNSHARP_NORMAL<<
		FP_UNSHARP_GEOMETRY<<
		FP_UNSHARP_COLOR <<
		FP_RECOMPUTE_VERTEX_NORMAL <<
		FP_FACE_NORMAL_NORMALIZE	;
;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

}

FilterUnsharp::~FilterUnsharp() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString FilterUnsharp::filterName(FilterIDType filter) 
{
 switch(filter)
  {
	  case FP_CREASE_CUT :					return QString("Cut mesh along crease");
  	case FP_FACE_NORMAL_NORMALIZE:	    return QString("Normalize Face Normal"); 
  	case FP_FACE_NORMAL_SMOOTHING:	    return QString("Smooth Face Normals"); 
  	case FP_VERTEX_QUALITY_SMOOTHING:	    return QString("Smooth vertex quality"); 
  	case FP_UNSHARP_NORMAL:				return QString("UnSharp Normals"); 
  	case FP_UNSHARP_GEOMETRY:	    return QString("UnSharp Geometry"); 
  	case FP_UNSHARP_COLOR:	      return QString("UnSharp Color"); 
	  case FP_RECOMPUTE_VERTEX_NORMAL: return QString("Recompute Vertex Normals"); 

  	default: assert(0);
  }
  return QString("error!");
}

const QString FilterUnsharp::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
		case FP_CREASE_CUT:						return tr("Cut the mesh along crease edges, duplicating the vertices as necessary."); 
		case FP_FACE_NORMAL_NORMALIZE:	    return tr("Normalize Face Normal Lenghts"); 
		case FP_VERTEX_QUALITY_SMOOTHING:	    return tr("Smooth Face Normals without touching the position of the vertices."); 
		case FP_FACE_NORMAL_SMOOTHING:	    return tr("Smooth Face Normals without touching the position of the vertices."); 
  	case FP_UNSHARP_NORMAL:				return tr("Unsharpen the normals, putting in more evidence normal variations"); 
  	case FP_UNSHARP_GEOMETRY:	    return tr("Unsharpen the color, putting in more evidence normal variations"); 
  	case FP_UNSHARP_COLOR:	    return tr("Unsharpen the normals, putting in more evidence normal variations"); 
		case FP_RECOMPUTE_VERTEX_NORMAL : return tr("Recompute vertex normals as an area weighted average of the adjacent faces");
  	default: assert(0);
  }
  return QString("error!");
}

const FilterUnsharp::FilterClass FilterUnsharp::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_CREASE_CUT :
      return MeshFilterInterface::Generic;     
	case FP_FACE_NORMAL_SMOOTHING:	  
	case FP_VERTEX_QUALITY_SMOOTHING:
	case FP_UNSHARP_NORMAL:				
	case FP_UNSHARP_GEOMETRY:	    
	case FP_UNSHARP_COLOR:	    
	case FP_RECOMPUTE_VERTEX_NORMAL : return 	MeshFilterInterface::Smoothing;
			case FP_FACE_NORMAL_NORMALIZE:	  return MeshFilterInterface::Normal;

    default : return MeshFilterInterface::Generic;
  }
}


const PluginInfo &FilterUnsharp::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr( __DATE__ );
	 ai.Version = tr("0.1");
	 ai.Author = ("Paolo Cignoni");
   return ai;
}

const int FilterUnsharp::getRequirements(QAction *action)
{
  switch(ID(action))
  {
		case FP_UNSHARP_GEOMETRY:	
		case FP_VERTEX_QUALITY_SMOOTHING:
		case FP_UNSHARP_COLOR:	return MeshModel::MM_BORDERFLAG;
    case FP_CREASE_CUT :	return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG;
		case FP_UNSHARP_NORMAL:		return MeshModel::MM_VERTFACETOPO | MeshModel::MM_BORDERFLAG;
		case FP_FACE_NORMAL_SMOOTHING : return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG;
		case FP_RECOMPUTE_VERTEX_NORMAL :
		case FP_FACE_NORMAL_NORMALIZE:	    return 0; 
			
    default: assert(0);
  }
  return 0;
}

bool FilterUnsharp::autoDialog(QAction *action) 
{
	switch(ID(action))
	{
	 case FP_UNSHARP_GEOMETRY:		
	 case FP_UNSHARP_COLOR:	
	 case FP_UNSHARP_NORMAL:		return true;
	}
	return false;
}
void FilterUnsharp::initParameterSet(QAction *action, MeshModel &/*m*/, FilterParameterSet & parlst)
{
	if (ID(action) == FP_UNSHARP_NORMAL) {
		parlst.addBool("recalc", false, tr("Recompute Normals"), tr("Recompute normals from scratch before the unsharp masking"));
		parlst.addFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the weight in the unsharp equation: <br> <i> orig + weight (orig - lowpass)<i><br>"));
		parlst.addInt("iterations", 5, "Iterations", 	tr("number of laplacian face smooth iterations in every run"));
	}
	if (ID(action) == FP_UNSHARP_GEOMETRY || ID(action) == FP_UNSHARP_COLOR) {
		parlst.addFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the weight in the unsharp equation: <br> <i> orig + weight (orig - lowpass)<i><br>"));
		parlst.addInt("iterations", 5, "Iterations", 	tr("number of laplacian smooth iterations in every run"));
	}
}

bool FilterUnsharp::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	switch(ID(filter))
	{
	case FP_CREASE_CUT :
			 tri::CreaseCut(m.cm, math::ToRad(60.0f));
			 m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
			break;
  case FP_FACE_NORMAL_SMOOTHING :
				 FaceNormalSmoothFF(m.cm);
			 break;
  case FP_VERTEX_QUALITY_SMOOTHING :
				 VertexQualitySmooth(m.cm);
			 break;
	case FP_RECOMPUTE_VERTEX_NORMAL : 
			tri::UpdateNormals<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
			break;
	case FP_UNSHARP_NORMAL:			
			{	
				float alpha=par.getFloat("weight");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
				vector<Point3f> normalOrig(m.cm.fn);
				for(int i=0;i<m.cm.fn;++i)
					normalOrig[i]=m.cm.face[i].cN();
				
				for(int i=0;i<smoothIter;++i)
						FaceNormalSmoothFF(m.cm);
				
				for(int i=0;i<m.cm.fn;++i)
					m.cm.face[i].N() = normalOrig[i] + (normalOrig[i] - m.cm.face[i].N())*alpha;
				
			}	break;
	case FP_UNSHARP_GEOMETRY:			
			{	
				float alpha=par.getFloat("weight");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
				vector<Point3f> geomOrig(m.cm.vn);
				for(int i=0;i<m.cm.vn;++i)
					geomOrig[i]=m.cm.vert[i].P();
				
				LaplacianSmooth(m.cm,1);
				
				for(int i=0;i<m.cm.vn;++i)
					m.cm.vert[i].P()=geomOrig[i] + (geomOrig[i] - m.cm.vert[i].P())*alpha;				
					
				tri::UpdateNormals<CMeshO>::PerVertexPerFace(m.cm);
				
			}	break;
	case FP_UNSHARP_COLOR:			
			{	
				float alpha=par.getFloat("weight");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
				vector<Color4f> colorOrig(m.cm.vn);
				for(int i=0;i<m.cm.vn;++i)
					colorOrig[i].Import(m.cm.vert[i].C());
				
				LaplacianSmoothColor(m.cm,smoothIter);
				
				for(int i=0;i<m.cm.vn;++i)
					m.cm.vert[i].C().Import(colorOrig[i] + (colorOrig[i] - Color4f::Construct(m.cm.vert[i].C()))*alpha);				
			}	break;
	case FP_FACE_NORMAL_NORMALIZE :
		tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
			 break;
		
	default : assert(0);
	
	}
	
				
	return true;
}


Q_EXPORT_PLUGIN(FilterUnsharp)
