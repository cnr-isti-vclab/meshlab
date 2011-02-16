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

#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include "cleanfilter.h"
#include "align_tools.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/create/ball_pivoting.h>

#include <vcg/space/normal_extrapolation.h>
using namespace std;
using namespace vcg;

CleanFilter::CleanFilter() 
{
  typeList 
		<< FP_BALL_PIVOTING 
		<< FP_REMOVE_WRT_Q 
		<< FP_REMOVE_ISOLATED_COMPLEXITY 
		<< FP_REMOVE_ISOLATED_DIAMETER 
//		<< FP_ALIGN_WITH_PICKED_POINTS
    << FP_SELECTBYANGLE
    << FP_REMOVE_TVERTEX_FLIP
    << FP_REMOVE_TVERTEX_COLLAPSE
		<< FP_REMOVE_DUPLICATE_FACE
    << FP_REMOVE_FOLD_FACE
		<< FP_MERGE_CLOSE_VERTEX;

  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

	maxDiag1=0;
	maxDiag2=-1;
	minCC=25;
	val1=1.0;

}

CleanFilter::~CleanFilter() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

 QString CleanFilter::filterName(FilterIDType filter) const
{
 switch(filter)
  {
 case FP_BALL_PIVOTING :								  return QString("Surface Reconstruction: Ball Pivoting");
	  case FP_REMOVE_WRT_Q :									return QString("Remove vertices wrt quality");
	  case FP_REMOVE_ISOLATED_DIAMETER   :		return QString("Remove isolated pieces (wrt diameter)");
	  case FP_REMOVE_ISOLATED_COMPLEXITY :		return QString("Remove isolated pieces (wrt face num)");
//    case FP_ALIGN_WITH_PICKED_POINTS :	    return QString("Align Mesh using Picked Points");
    case FP_SELECTBYANGLE :                 return QString("Select Faces by view angle");
    case FP_REMOVE_TVERTEX_FLIP :           return QString("Remove T-Vertices by edge flip");
    case FP_REMOVE_TVERTEX_COLLAPSE :       return QString("Remove T-Vertices by edge collapse");
    case FP_MERGE_CLOSE_VERTEX :            return QString("Merge Close Vertices");
    case FP_REMOVE_DUPLICATE_FACE:          return QString("Remove Duplicate Faces");
  case FP_REMOVE_FOLD_FACE:          return QString("Remove Isolated folded face by edge flip");
		default: assert(0);
  }
  return QString("error!");
}

 QString CleanFilter::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
		case FP_BALL_PIVOTING :	return QString("Reconstruct a surface using the <b>Ball Pivoting Algorithm</b> (Bernardini et al. 1999). <br>"
																						 "Starting with a seed triangle, the BPA algorithm  pivots a ball around an edge "
																						 "(i.e. it revolves around the edge while keeping in contact with the edge endpoints) "
																						 "until it touches another point, forming another triangle. The process continues until all reachable edges have been tried."); 
		case FP_REMOVE_ISOLATED_COMPLEXITY:	 return tr("Remove isolated connected components composed by a limited number of triangles"); 
		case FP_REMOVE_ISOLATED_DIAMETER:	 return tr("Remove isolated connected components whose diameter is smaller than the specified constant"); 
		case FP_REMOVE_WRT_Q:	     return tr("Remove all the vertices with a quality lower smaller than the specified constant"); 
//		case FP_ALIGN_WITH_PICKED_POINTS: return tr("Align this mesh with another that has corresponding picked points.");
		case FP_SELECTBYANGLE :  return QString("Select faces according to the angle between their normal and the view direction. It is used in range map processing to select and delete steep faces parallel to viewdirection"); 
    case FP_REMOVE_TVERTEX_COLLAPSE :  return QString("Removes t-vertices from the mesh by collapsing the shortest of the incident edges");
    case FP_REMOVE_TVERTEX_FLIP : return QString("Removes t-vertices by flipping the opposite edge on the degenerate face if the triangulation quality improves");
    case FP_MERGE_CLOSE_VERTEX : return QString("Merge togheter all the vertices that are nearer than the speicified threshold. Like a unify duplicated vertices but with some tolerance.");
    case FP_REMOVE_DUPLICATE_FACE : return QString("Remove all the duplicate faces. Two faces are considered equal if they are composed by the same set of verticies, regardless of the order of the vertices.");
  case FP_REMOVE_FOLD_FACE : return QString("Remove all the single folded faces. A face is considered folded if its normal is opposite to all the adjacent faces. It is removed by flipping it against the face f adjacent along the edge e such that the vertex opposite to e fall inside f");
    default: assert(0);
  }
  return QString("error!");
}

 CleanFilter::FilterClass CleanFilter::getClass(QAction *a)
{
  switch(ID(a))
  {
		case FP_SELECTBYANGLE :
				return MeshFilterInterface::FilterClass(MeshFilterInterface::RangeMap + MeshFilterInterface::Selection);     
    case FP_REMOVE_WRT_Q :
    case FP_REMOVE_ISOLATED_DIAMETER :
    case FP_REMOVE_ISOLATED_COMPLEXITY :
    case FP_REMOVE_TVERTEX_COLLAPSE :
    case FP_REMOVE_TVERTEX_FLIP :
  case FP_REMOVE_FOLD_FACE :
    case FP_MERGE_CLOSE_VERTEX :
    case FP_REMOVE_DUPLICATE_FACE:
      return MeshFilterInterface::Cleaning;     
		case FP_BALL_PIVOTING: 	return MeshFilterInterface::Remeshing;
    //case FP_ALIGN_WITH_PICKED_POINTS: return MeshFilterInterface::RangeMap;
    default : assert(0);  
	}
}

 int CleanFilter::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_REMOVE_WRT_Q:
    case FP_BALL_PIVOTING:	return MeshModel::MM_FACEFLAGBORDER  | MeshModel::MM_VERTMARK;
	  case FP_REMOVE_ISOLATED_COMPLEXITY:
    case FP_REMOVE_ISOLATED_DIAMETER:
        return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_FACEMARK;
    case FP_REMOVE_TVERTEX_COLLAPSE: return MeshModel::MM_VERTMARK;
    case FP_REMOVE_TVERTEX_FLIP: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
  case FP_REMOVE_FOLD_FACE: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
    case FP_SELECTBYANGLE:
//		case FP_ALIGN_WITH_PICKED_POINTS:
		case FP_MERGE_CLOSE_VERTEX:
		case FP_REMOVE_DUPLICATE_FACE:
    	return MeshModel::MM_NONE;
    default: assert(0);
  }
  return 0;
}

void CleanFilter::initParameterSet(QAction *action,MeshDocument &md, RichParameterSet & parlst)
{ 
	pair<float,float> qualityRange;
  switch(ID(action))
  {
    case FP_BALL_PIVOTING :
		  parlst.addParam(new RichAbsPerc("BallRadius",(float)maxDiag1,0,md.mm()->cm.bbox.Diag(),"Pivoting Ball radius (0 autoguess)","The radius of the ball pivoting (rolling) over the set of points. Gaps that are larger than the ball radius will not be filled; similarly the small pits that are smaller than the ball radius will be filled."));
		  parlst.addParam(new RichFloat("Clustering",20.0f,"Clustering radius (% of ball radius)","To avoid the creation of too small triangles, if a vertex is found too close to a previous one, it is clustered/merged with it."));		  
			parlst.addParam(new RichFloat("CreaseThr", 90.0f,"Angle Threshold (degrees)","If we encounter a crease angle that is too large we should stop the ball rolling"));
		  parlst.addParam(new RichBool("DeleteFaces",false,"Delete intial set of faces","if true all the initial faces of the mesh are deleted and the whole surface is rebuilt from scratch, other wise the current faces are used as a starting point. Useful if you run multiple times the algorithm with an incrasing ball radius."));
		  break;
    case FP_REMOVE_ISOLATED_DIAMETER:	 
		  parlst.addParam(new RichAbsPerc("MinComponentDiag",md.mm()->cm.bbox.Diag()/10.0,0,md.mm()->cm.bbox.Diag(),"Enter max diameter of isolated pieces","Delete all the connected components (floating pieces) with a diameter smaller than the specified one"));
		  break;
    case FP_REMOVE_ISOLATED_COMPLEXITY:	 
		  parlst.addParam(new RichInt("MinComponentSize",(int)minCC,"Enter minimum conn. comp size:","Delete all the connected components (floating pieces) composed by a number of triangles smaller than the specified one"));
		  break;
    case FP_REMOVE_WRT_Q:
			qualityRange=tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(md.mm()->cm);
		  parlst.addParam(new RichAbsPerc("MaxQualityThr",(float)val1, qualityRange.first, qualityRange.second,"Delete all vertices with quality under:"));
		  break;
////    case FP_ALIGN_WITH_PICKED_POINTS :
//    	AlignTools::buildParameterSet(md,parlst);
//    	break;
		case FP_SELECTBYANGLE :
			{
				parlst.addParam(new RichDynamicFloat("anglelimit",
															 75.0f, 0.0f, 180.0f, 
												"angle threshold (deg)",
												"faces with normal at higher angle w.r.t. the view direction are selected"));
	 		  parlst.addParam(new RichBool ("usecamera",
												false,
												"Use ViewPoint from Mesh Camera",
												"Uses the ViewPoint from the camera associated to the current mesh\n if there is no camera, an error occurs"));
				parlst.addParam(new RichPoint3f("viewpoint",
												Point3f(0.0f, 0.0f, 0.0f),
												"ViewPoint",
												"if UseCamera is true, this value is ignored"));
			}
			break;    
		case FP_MERGE_CLOSE_VERTEX :
			parlst.addParam(new RichAbsPerc("Threshold",md.mm()->cm.bbox.Diag()/10000.0,0,md.mm()->cm.bbox.Diag()/100.0,"Merging distance","All the vertices that closer than this threshold are merged toghether. Use very small values, default values is 1/10000 of bounding box diagonal. "));
		 break;    

    case FP_REMOVE_TVERTEX_COLLAPSE :
    case FP_REMOVE_TVERTEX_FLIP :
       parlst.addParam(new RichFloat(
               "Threshold", 40, "Ratio", "Detects faces where the base/height ratio is lower than this value"));
       parlst.addParam(new RichBool(
               "Repeat", true, "Iterate until convergence", "Iterates the algorithm until it reaches convergence"));
       break;
  default: break; // do not add any parameter for the other filters
  }
}

bool CleanFilter::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * cb)
{
    MeshModel &m=*(md.mm());
    switch(ID(filter))
  {
	 case FP_BALL_PIVOTING:
	  {
      float Radius = par.getAbsPerc("BallRadius");		
      float Clustering = par.getFloat("Clustering");		      
			float CreaseThr = math::ToRad(par.getFloat("CreaseThr"));
			bool DeleteFaces = par.getBool("DeleteFaces");
      if(DeleteFaces) {
				m.cm.fn=0;
				m.cm.face.resize(0);
      }

			int startingFn=m.cm.fn;			
		  Clustering /= 100.0;
			tri::BallPivoting<CMeshO> pivot(m.cm, Radius, Clustering, CreaseThr); 
      // the main processing
      pivot.BuildMesh(cb);
      m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
			Log("Reconstructed surface. Added %i faces",m.cm.fn-startingFn); 		
	  } break;
    case FP_REMOVE_ISOLATED_DIAMETER:
	  {
      float minCC= par.getAbsPerc("MinComponentDiag");		
      std::pair<int,int> delInfo= tri::Clean<CMeshO>::RemoveSmallConnectedComponentsDiameter(m.cm,minCC);
			Log("Removed %2 connected components out of %1", delInfo.second, delInfo.first); 		
    }break;
    case FP_REMOVE_ISOLATED_COMPLEXITY:
	  {
      float minCC= par.getInt("MinComponentSize");		
      std::pair<int,int> delInfo=tri::Clean<CMeshO>::RemoveSmallConnectedComponentsSize(m.cm,minCC);
			Log("Removed %i connected components out of %i", delInfo.second, delInfo.first); 		
	  }break;
    case FP_REMOVE_WRT_Q:
	  {
			int deletedFN=0;
			int deletedVN=0;
      float val=par.getAbsPerc("MaxQualityThr");		
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
						if(!(*vi).IsD() && (*vi).Q()<val)
						{
							tri::Allocator<CMeshO>::DeleteVertex(m.cm, *vi);
							deletedVN++;
						}
        
      CMeshO::FaceIterator fi;
      for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
				 if((*fi).V(0)->IsD() ||(*fi).V(1)->IsD() ||(*fi).V(2)->IsD() ) 
						 {
								tri::Allocator<CMeshO>::DeleteFace(m.cm, *fi);
								deletedFN++;
						 }
								 
      m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
			Log("Deleted %i vertices and %i faces with a quality lower than %f", deletedVN,deletedFN,val); 		

	  }break;
//    case FP_ALIGN_WITH_PICKED_POINTS:
//	{
//		bool result = AlignTools::setupThenAlign(m, par);
//		if(!result)
//		{
//			Log(GLLogStream::WARNING,"Align failed, make sure you have equal numbers of points.");
//			return false;
//		}
//	}break;

		case FP_SELECTBYANGLE : 
		{
			CMeshO::FaceIterator   fi;
			bool usecam = par.getBool("usecamera");
			Point3f viewpoint = par.getPoint3f("viewpoint");	

			// if usecamera but mesh does not have one
			if( usecam && !m.hasDataMask(MeshModel::MM_CAMERA) ) 
			{
				errorMessage = "Mesh has not a camera that can be used to compute view direction. Please set a view direction."; // text
				return false;
			}
			if(usecam)
			{
				viewpoint = m.cm.shot.GetViewPoint();
			}

			// angle threshold in radians
			float limit = cos( math::ToRad(par.getDynamicFloat("anglelimit")) );
			Point3f viewray;

			for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
				if(!(*fi).IsD())
				{
					viewray = viewpoint - Barycenter(*fi);
					viewray.Normalize();

					if((viewray.dot((*fi).N().Normalize())) < limit)
						fi->SetS();
				}

		}
		break;

    case FP_REMOVE_TVERTEX_COLLAPSE :
    {
        float threshold = par.getFloat("Threshold");
        bool repeat = par.getBool("Repeat");

        int total = tri::Clean<CMeshO>::RemoveTVertexByCollapse(m.cm, threshold, repeat);
        Log("Successfully removed %d t-vertices", total);
    }
        break;
    case FP_REMOVE_TVERTEX_FLIP :
    {
        float threshold = par.getFloat("Threshold");
        bool repeat = par.getBool("Repeat");
        int total = tri::Clean<CMeshO>::RemoveTVertexByFlip(m.cm, threshold, repeat);
        Log("Successfully removed %d t-vertices", total);
    }
    break;
   case FP_MERGE_CLOSE_VERTEX :
    {
        float threshold = par.getAbsPerc("Threshold");
        int total = tri::Clean<CMeshO>::MergeCloseVertex(m.cm, threshold);
        Log("Successfully merged %d vertices", total);
    }
    break;
  case FP_REMOVE_DUPLICATE_FACE :
   {
       int total = tri::Clean<CMeshO>::RemoveDuplicateFace(m.cm);
       Log("Successfully deleted %d duplicated faces", total);
   }
   break;
  case FP_REMOVE_FOLD_FACE:
   {
     m.updateDataMask(MeshModel::MM_FACECOLOR);
     tri::UpdateColor<CMeshO>::FaceConstant(m.cm, Color4b::White);
       int total = tri::Clean<CMeshO>::RemoveFaceFoldByFlip(m.cm);
       tri::UpdateNormals<CMeshO>::PerVertexPerFace(m.cm);
       Log("Successfully flipped %d folded faces", total);
   }
   break;

  default : assert(0); // unknown filter;
	}
	return true;
}


Q_EXPORT_PLUGIN(CleanFilter)
