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

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/create/ball_pivoting.h>

#include <vcg/space/normal_extrapolation.h>
using namespace std;
using namespace vcg;

int SnapVertexBorder(CMeshO &m, float threshold,vcg::CallBackPos * cb);

CleanFilter::CleanFilter() 
{
  typeList 
		<< FP_BALL_PIVOTING 
		<< FP_REMOVE_WRT_Q 
		<< FP_REMOVE_ISOLATED_COMPLEXITY 
		<< FP_REMOVE_ISOLATED_DIAMETER 
    << FP_REMOVE_TVERTEX_FLIP
    << FP_REMOVE_TVERTEX_COLLAPSE
    << FP_SNAP_MISMATCHED_BORDER
		<< FP_REMOVE_DUPLICATE_FACE
    << FP_REMOVE_FOLD_FACE
    << FP_REMOVE_NON_MANIF_EDGE
    << FP_REMOVE_NON_MANIF_VERT
		<< FP_MERGE_CLOSE_VERTEX
		<< FP_COMPACT_FACE
		<< FP_COMPACT_VERT;

  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);
  AC(FP_SNAP_MISMATCHED_BORDER)->setShortcut(QKeySequence("ALT+`"));
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
  case FP_BALL_PIVOTING :					return QString("Surface Reconstruction: Ball Pivoting");
  case FP_REMOVE_WRT_Q :            		return QString("Remove Vertices wrt Quality");
  case FP_REMOVE_ISOLATED_DIAMETER   :		return QString("Remove Isolated pieces (wrt Diameter)");
  case FP_REMOVE_ISOLATED_COMPLEXITY :		return QString("Remove Isolated pieces (wrt Face Num.)");
  case FP_REMOVE_TVERTEX_FLIP :             return QString("Remove T-Vertices by Edge Flip");
  case FP_REMOVE_TVERTEX_COLLAPSE :         return QString("Remove T-Vertices by Edge Collapse");
  case FP_SNAP_MISMATCHED_BORDER :          return QString("Snap Mismatched Borders");
  case FP_MERGE_CLOSE_VERTEX :              return QString("Merge Close Vertices");
  case FP_REMOVE_DUPLICATE_FACE:            return QString("Remove Duplicate Faces");
  case FP_REMOVE_FOLD_FACE:                 return QString("Remove Isolated Folded Faces by Edge Flip");
  case FP_REMOVE_NON_MANIF_EDGE:            return QString("Remove Faces from Non Manifold Edges");
  case FP_REMOVE_NON_MANIF_VERT:            return QString("Split Vertexes Incident on Non Manifold Faces");
  case FP_COMPACT_VERT:						return QString("Compact vertices");
  case FP_COMPACT_FACE:						return QString("Compact faces");
  default: assert(0);
  }
  return QString("error!");
}

 QString CleanFilter::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
        case FP_BALL_PIVOTING :	return QString("Given a point cloud with normals it reconstructs a surface using the <b>Ball Pivoting Algorithm</b>."
                                               "Starting with a seed triangle, the BPA algorithm  pivots a ball of the given radius around the already formed edges"
                                               "until it touches another point, forming another triangle. The process continues until all reachable edges have been tried."
                                               "This surface reconstruction algoritm uses the existing points without creating new ones. Works better with uniformly sampled point clouds. "
                                               "If needed first perform a poisson disk subsampling of the point cloud. <br>"
                                               "Bernardini F., Mittleman J., Rushmeier H., Silva C., Taubin G.<br>"
                                               "<b>The ball-pivoting algorithm for surface reconstruction.</b><br>"
                                               "IEEE TVCG 1999");
    case FP_REMOVE_ISOLATED_COMPLEXITY: return QString("Delete isolated connected components composed by a limited number of triangles");
    case FP_REMOVE_ISOLATED_DIAMETER:   return QString("Delete isolated connected components whose diameter is smaller than the specified constant");
    case FP_REMOVE_WRT_Q:               return QString("Delete all the vertices with a quality lower smaller than the specified constant");
    case FP_REMOVE_TVERTEX_COLLAPSE :   return QString("Delete t-vertices from the mesh by collapsing the shortest of the incident edges");
    case FP_REMOVE_TVERTEX_FLIP :       return QString("Delete t-vertices by flipping the opposite edge on the degenerate face if the triangulation quality improves");
    case FP_SNAP_MISMATCHED_BORDER :    return QString("Try to snap together adjacent borders that are slightly mismatched.<br>"
                                                       "This situation can happen on badly triangulated adjacent patches defined by high order surfaces.<br>"
                                                       "For each border vertex the filter snap it onto the closest boundary edge only if it is closest of <i>edge_lenght*threshold</i>. When vertex is snapped the correspoinding face it split and a new vertex is created.");
    case FP_MERGE_CLOSE_VERTEX :        return QString("Merge together all the vertices that are nearer than the speicified threshold. Like a unify duplicated vertices but with some tolerance.");
    case FP_REMOVE_DUPLICATE_FACE :     return QString("Delete all the duplicate faces. Two faces are considered equal if they are composed by the same set of verticies, regardless of the order of the vertices.");
    case FP_REMOVE_FOLD_FACE :          return QString("Delete all the single folded faces. A face is considered folded if its normal is opposite to all the adjacent faces. It is removed by flipping it against the face f adjacent along the edge e such that the vertex opposite to e fall inside f");
    case FP_REMOVE_NON_MANIF_EDGE :     return QString("For each non manifold edge it iteratively deletes the smallest area face until it becomes 2-manifold.");
    case FP_REMOVE_NON_MANIF_VERT :     return QString("Split non manifold vertices until it becomes 2-manifold.");
	case FP_COMPACT_VERT:            return QString("Compact all the vertices that have been deleted and put them to the end of the vector");
	case FP_COMPACT_FACE:            return QString("Compact all the faces that have been deleted and put them to the end of the vector");
    default: assert(0);
  }
  return QString("error!");
}

 CleanFilter::FilterClass CleanFilter::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_REMOVE_WRT_Q :
    case FP_REMOVE_ISOLATED_DIAMETER :
    case FP_REMOVE_ISOLATED_COMPLEXITY :
    case FP_REMOVE_TVERTEX_COLLAPSE :
    case FP_REMOVE_TVERTEX_FLIP :
    case FP_REMOVE_FOLD_FACE :
    case FP_MERGE_CLOSE_VERTEX :
    case FP_REMOVE_DUPLICATE_FACE:
    case FP_SNAP_MISMATCHED_BORDER:
    case FP_REMOVE_NON_MANIF_EDGE:
    case FP_REMOVE_NON_MANIF_VERT:
	case FP_COMPACT_VERT:
	case FP_COMPACT_FACE:
      return MeshFilterInterface::Cleaning;
    case FP_BALL_PIVOTING: 	return MeshFilterInterface::Remeshing;
    default : assert(0);  
	}
}

 int CleanFilter::getRequirements(QAction *action)
{
  switch(ID(action))
  {
	case FP_COMPACT_FACE:
	case FP_COMPACT_VERT:
    case FP_REMOVE_WRT_Q:
    case FP_BALL_PIVOTING: MeshModel::MM_VERTMARK;
    case FP_REMOVE_ISOLATED_COMPLEXITY:
    case FP_REMOVE_ISOLATED_DIAMETER:
      return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEMARK;
    case FP_REMOVE_TVERTEX_COLLAPSE: return MeshModel::MM_VERTMARK;
    case FP_REMOVE_TVERTEX_FLIP: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
    case FP_REMOVE_NON_MANIF_EDGE: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
    case FP_REMOVE_NON_MANIF_VERT: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
    case FP_SNAP_MISMATCHED_BORDER: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK| MeshModel::MM_FACEMARK;
    case FP_REMOVE_FOLD_FACE: return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK;
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
	case FP_MERGE_CLOSE_VERTEX :
		  parlst.addParam(new RichAbsPerc("Threshold",md.mm()->cm.bbox.Diag()/10000.0,0,md.mm()->cm.bbox.Diag()/100.0,"Merging distance","All the vertices that closer than this threshold are merged together. Use very small values, default values is 1/10000 of bounding box diagonal. "));
		  break;
    case FP_SNAP_MISMATCHED_BORDER:
    parlst.addParam(new RichFloat("EdgeDistRatio",1/100.0f,"Edge Distance Ratio", "Collapse edge when the edge / distance ratio is greater than this value. E.g. for default value 1000 two straight border edges are collapsed if the central vertex dist from the straight line composed by the two edges less than a 1/1000 of the sum of the edges lenght. Larger values enforce that only vertexes very close to the line are removed."));
    parlst.addParam(new RichBool("UnifyVertices",true,"UnifyVertices","if true the snap vertices are weld together."));
      break;
    case FP_REMOVE_TVERTEX_COLLAPSE :
    case FP_REMOVE_TVERTEX_FLIP :
       parlst.addParam(new RichFloat(
               "Threshold", 40, "Ratio", "Detects faces where the base/height ratio is lower than this value"));
       parlst.addParam(new RichBool(
               "Repeat", true, "Iterate until convergence", "Iterates the algorithm until it reaches convergence"));
       break;
    case FP_REMOVE_NON_MANIF_VERT :
       parlst.addParam(new RichFloat("VertDispRatio", 0, "Vertex Displacement Ratio", "When a vertex is split it is moved along the average vector going from its position to the baricyenter of the FF connected faces sharing it"));
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
      float Clustering = par.getFloat("Clustering") / 100.0f;
      float CreaseThr = math::ToRad(par.getFloat("CreaseThr"));
      bool DeleteFaces = par.getBool("DeleteFaces");
      if(DeleteFaces) {
        m.cm.fn=0;
        m.cm.face.resize(0);
      }

      int startingFn=m.cm.fn;
      tri::BallPivoting<CMeshO> pivot(m.cm, Radius, Clustering, CreaseThr);
      // the main processing
      pivot.BuildMesh(cb);
      m.clearDataMask(MeshModel::MM_FACEFACETOPO);
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
								 
	  m.clearDataMask(MeshModel::MM_FACEFACETOPO);
			Log("Deleted %i vertices and %i faces with a quality lower than %f", deletedVN,deletedFN,val); 		

	  }break;

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
  case FP_REMOVE_NON_MANIF_EDGE :
  {
      int total = tri::Clean<CMeshO>::RemoveNonManifoldFace(m.cm);
      Log("Successfully removed %d folded faces", total);
  }
  break;
  case FP_REMOVE_NON_MANIF_VERT :
  {
      float threshold = par.getFloat("VertDispRatio");

      int total = tri::Clean<CMeshO>::SplitNonManifoldVertex(m.cm,threshold);
      Log("Successfully split %d non manifold vertices faces", total);
  }
  break;
  case FP_SNAP_MISMATCHED_BORDER :
    {
      float threshold = par.getFloat("EdgeDistRatio");
      int total = SnapVertexBorder(m.cm, threshold,cb);
      Log("Successfully Splitted %d faces to snap", total);
    } break;
	
  case FP_COMPACT_FACE :
	  {
		  vcg::tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		  break;
	  }
  case FP_COMPACT_VERT :
	  {
		  vcg::tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
		  break;
	  }
  default : assert(0); // unknown filter;
	}
	return true;
}


int SnapVertexBorder(CMeshO &m, float threshold, vcg::CallBackPos * cb)
{
  tri::Allocator<CMeshO>::CompactVertexVector(m);
  tri::Allocator<CMeshO>::CompactFaceVector(m);

  tri::UpdateTopology<CMeshO>::FaceFace(m);
  tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m);
  tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m);
  tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m);
  typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
  MetroMeshFaceGrid   unifGridFace;
  typedef tri::FaceTmark<CMeshO> MarkerFace;
  MarkerFace markerFunctor;
  vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
  tri::UpdateFlags<CMeshO>::FaceClearV(m);
  unifGridFace.Set(m.face.begin(),m.face.end());

  markerFunctor.SetMesh(&m);
  int faceFound;
  int K = 20;
  Point3f startPt;
  float maxDist = m.bbox.Diag()/20;
  vector<Point3f> splitVertVec;
  vector<CMeshO::FacePointer> splitFaceVec;
  vector<int> splitEdgeVec;
  for(CMeshO::VertexIterator vi=m.vert.begin();vi!=m.vert.end();++vi)
    if(!(*vi).IsD() && (*vi).IsB())
      {
        int percPos = (tri::Index(m,*vi) *100) / m.vn;
        cb(percPos,"Snapping vertices");
        vector<CMeshO::FacePointer> faceVec;
        vector<float> distVec;
        vector<Point3f> pointVec;
        Point3f u;
        startPt = (*vi).P();
        faceFound = unifGridFace.GetKClosest(PDistFunct,markerFunctor, K, startPt,maxDist, faceVec, distVec, pointVec);

        CMeshO::FacePointer bestFace = 0;
        float localThr, bestDist = std::numeric_limits<float>::max();
        Point3f bestPoint;
        int bestEdge;
//        qDebug("Found %i face for vertex %i",faceFound,vi-m.vert.begin());
        for(int i=0;i<faceFound;++i)
        {
          const float epsilonSmall = 1e-5;
          const float epsilonBig = 1e-2;
          CMeshO::FacePointer fp=faceVec[i];
          InterpolationParameters(*fp,fp->cN(),pointVec[i],u);
//          qDebug(" face %i face for vertex %5.3f %5.3f %5.3f  dist %5.3f  (%c %c %c)",fp-&*m.face.begin(),u[0],u[1],u[2],distVec[i],IsBorder(*fp,0)?'b':' ',IsBorder(*fp,1)?'b':' ',IsBorder(*fp,2)?'b':' ');
          for(int j=0;j<3;++j)
          {
            if(IsBorder(*fp,j) && !fp->IsV())
            {
              if( u[(j+0)%3] > epsilonBig &&
                  u[(j+1)%3] > epsilonBig &&
                  u[(j+2)%3] < epsilonSmall )
              {
                if(distVec[i] < bestDist)
                {
                  bestDist=distVec[i];
                  //bestPoint=pointVec[i];
                  bestPoint=(*vi).cP();
                  bestFace=fp;
                  bestEdge=j;
                }
              }
            }
          }
        } // end for each faceFound

        if(bestFace)
          localThr = threshold*Distance(bestFace->P0(bestEdge),bestFace->P1(bestEdge));
        if(bestDist < localThr && !bestFace->IsV())
        {
          bestFace->SetV();
          (*vi).C()= Color4b::Blue;
          //bestFace->C()=Color4b::LightBlue;
          (*vi).SetS();
          splitVertVec.push_back(bestPoint);
          splitEdgeVec.push_back(bestEdge);
          splitFaceVec.push_back(bestFace);
        }
      }
  tri::Allocator<CMeshO>::PointerUpdater<CMeshO::FacePointer> pu;
  CMeshO::VertexIterator firstVert = tri::Allocator<CMeshO>::AddVertices(m,splitVertVec.size());
  CMeshO::FaceIterator firstface = tri::Allocator<CMeshO>::AddFaces(m,splitVertVec.size(),pu);
//
//             ^                           ^
//           /   \                       / | \          .
//         /       \                   /   |   \        .
//       /           \               /     |     \      .
//     /       fp      \           /       |       \    .
//   /                   \       /    fp   |   ff    \  .
//  V0 ------------------V2     V0 -------fv---------V2
//       i

  for(size_t i=0;i<splitVertVec.size();++i)
    {
      firstVert->P() = splitVertVec[i];
      int eInd = splitEdgeVec[i];
      CMeshO::FacePointer fp = splitFaceVec[i];
      pu.Update(fp);
      firstface->V(0) = &*firstVert;
      firstface->V(1) = fp->V2(eInd);
      firstface->V(2) = fp->V0(eInd);
//      firstface->C()=Color4b::LightBlue;

      fp->V0(eInd) = &*firstVert;

      ++firstface;
      ++firstVert;
    }
  tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m);
  return splitVertVec.size();
  }


//
//             ^-----------------
//           / | \             /
//         /   |   \     fj   /
//       /  i+1|     \ij     /
//     /       |       \    /
//   /    fi   |   fadj  \ /
//  V0 --------V1--------V2
//       i
//

  int  DeleteCollinearBorder(CMeshO &m, float threshold)
  {
    int total=0;
    CMeshO::FaceIterator fi;
    for(fi=m.face.begin();fi!=m.face.end();++fi)
      if(!(*fi).IsD())
        {
          for(int i=0;i<3;++i)
          {
            if(face::IsBorder(*fi,i) && !face::IsBorder(*fi,(i+1)%3))
            {
              CMeshO::VertexPointer V0= (*fi).V0(i);
              CMeshO::VertexPointer V1= (*fi).V1(i);
              CMeshO::VertexPointer V2=0;
              CMeshO::FacePointer fadj = (*fi).FFp((i+1)%3);
              int adjBordInd =  (*fi).FFi((i+1)%3);
              if(fadj->V1(adjBordInd) == V1)
                V2 = fadj->V2(adjBordInd);
              else
                continue; // non coerent face ordering.
              if(face::IsBorder(*fadj,(adjBordInd+1)%3))
              {
                // the colinearity test;
                Point3f pp;
                float dist;
                SegmentPointDistance(Segment3f(V0->cP(),V2->cP()),V1->cP(),pp,dist);
                if(dist* threshold <  Distance(V0->cP(),V2->cP()) )
                {
                  (*fi).V1(i)=V2;
                  if(face::IsBorder(*fadj,(adjBordInd+2)%3))
                  {
                    (*fi).FFp((i+1)%3)=&*fi;
                    (*fi).FFi((i+1)%3)=(i+1)%3;
                  }
                  else
                  {
                    CMeshO::FacePointer fj = fadj->FFp((adjBordInd+2)%3);
                    int ij = fadj->FFi((adjBordInd+2)%3);
                    (*fi).FFp((i+1)%3)= fj;
                    (*fi).FFi((i+1)%3)= ij;
                    fj->FFp(ij)=&*fi;
                    fj->FFi(ij)=(i+1)%3;
                  }
                  tri::Allocator<CMeshO>::DeleteFace(m,*fadj);
                  total++;
                }
              }
            }
          }
        }
	  return total;

}
Q_EXPORT_PLUGIN(CleanFilter)
