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

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/inertia.h>
#include <vcg/complex/trimesh/stat.h>

#include <vcg/complex/trimesh/update/selection.h>
#include<vcg/complex/trimesh/append.h>
#include<vcg/simplex/face/pos.h>
#include<vcg/complex/trimesh/bitquad_support.h>
#include<vcg/complex/trimesh/bitquad_optimization.h>
#include "filter_measure.h"

using namespace std;
using namespace vcg;

// Constructor 
FilterMeasurePlugin::FilterMeasurePlugin() 
{ 
	typeList << 
    FP_MEASURE_TOPO <<
    FP_MEASURE_TOPO_QUAD <<
    FP_MEASURE_GAUSSCURV <<
    FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION <<
    FP_MEASURE_FACE_QUALITY_DISTRIBUTION <<
    FP_MEASURE_VERTEX_QUALITY_HISTOGRAM <<
    FP_MEASURE_FACE_QUALITY_HISTOGRAM <<
    FP_MEASURE_GEOM;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action 
 QString FilterMeasurePlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_MEASURE_TOPO :  return QString("Compute Topological Measures");
    case FP_MEASURE_TOPO_QUAD :  return QString("Compute Topological Measures for Quad Meshes");
    case FP_MEASURE_GEOM :  return QString("Compute Geometric Measures");
    case FP_MEASURE_GAUSSCURV :  return QString("Compute Integral of Gaussian Curvature");
    case FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION :  return QString("Per Vertex Quality Stat");
    case FP_MEASURE_FACE_QUALITY_DISTRIBUTION :  return QString("Per Face Quality Stat");
    case FP_MEASURE_VERTEX_QUALITY_HISTOGRAM :  return QString("Histogram of Quality Per Vertex");
    case FP_MEASURE_FACE_QUALITY_HISTOGRAM :  return QString("Histogram of Quality Per Vertex");
  default : assert(0);
	}
}

// Info() return the longer string describing each filtering action 
 QString FilterMeasurePlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_MEASURE_TOPO :  return QString("Selected faces are moved (or duplicated) in a new layer"); 
		case FP_MEASURE_TOPO_QUAD :  return QString("Selected faces are moved (or duplicated) in a new layer"); 
		case FP_MEASURE_GEOM :  return QString("Create a new layer containing the same model as the current one");
		case FP_MEASURE_GAUSSCURV :  return QString("Compute Integral of Gaussian Curvature");
    case FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION : return QString("Compute some statistical measures (min, max, med, stdev, variance, about the distribution of per vertex quality values");
    case FP_MEASURE_VERTEX_QUALITY_HISTOGRAM : return QString("Compute a histogram with a given number of bin of the per vertex quality");
    case FP_MEASURE_FACE_QUALITY_DISTRIBUTION : return QString("Compute some statistical measures (min, max, med, stdev, variance, about the distribution of per face quality values");
    case FP_MEASURE_FACE_QUALITY_HISTOGRAM : return QString("Compute a histogram with a given number of bin of the per face quality");
    default : assert(0);
	}
}

// This function define the needed parameters for each filter. 
void FilterMeasurePlugin::initParameterSet(QAction *action, MeshDocument &m, RichParameterSet & par)
{
	 switch(ID(action))	 
	 {
   case FP_MEASURE_FACE_QUALITY_HISTOGRAM :
   case FP_MEASURE_VERTEX_QUALITY_HISTOGRAM:
         {
         pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.mm()->cm);
         par.addParam(new RichFloat("minVal",minmax.first,"Min","The value that is used as a lower bound for the set of bins (all the value smaller this one will be put in the first bin)"));
         par.addParam(new RichFloat("maxVal",minmax.second,"Max","The value that is used as a upper bound for the set of bins (all the value over this one will be put in the last bin)"));
         par.addParam(new RichInt("binNum",20,"Number of bins","Number of bins in which the range of values is subdivided"));
        } break;
	}
}

// Core Function doing the actual mesh processing.
bool FilterMeasurePlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos */*cb*/)
{
	CMeshO::FaceIterator fi;

	switch(ID(filter))
  {
		case FP_MEASURE_TOPO : 
			{
				CMeshO &m=md.mm()->cm;	
				md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);				
				md.mm()->updateDataMask(MeshModel::MM_VERTFACETOPO);				
        tri::Allocator<CMeshO>::CompactFaceVector(m);
        tri::Allocator<CMeshO>::CompactVertexVector(m);
        tri::UpdateTopology<CMeshO>::FaceFace(m);
				tri::UpdateTopology<CMeshO>::VertexFace(m);
				
        int edgeManif = tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m,true);
        int faceEdgeManif = tri::UpdateSelection<CMeshO>::CountFace(m);
        tri::UpdateSelection<CMeshO>::ClearVertex(m);
        tri::UpdateSelection<CMeshO>::ClearFace(m);

        int vertManif = tri::Clean<CMeshO>::CountNonManifoldVertexFF(m,true);
        tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
        int faceVertManif = tri::UpdateSelection<CMeshO>::CountFace(m);
				int edgeNum=0,borderNum=0;
				tri::Clean<CMeshO>::CountEdges(m, edgeNum, borderNum);
				int holeNum;
				Log("V: %6i E: %6i F:%6i",m.vn,edgeNum,m.fn);
				Log("Boundary Edges %i",borderNum); 
				
        int connectedComponentsNum = tri::Clean<CMeshO>::CountConnectedComponents(m);
				Log("Mesh is composed by %i connected component(s)",connectedComponentsNum);
				
        if(edgeManif==0 && vertManif==0)
					Log("Mesh has is two-manifold ");
					
        if(edgeManif!=0) Log("Mesh has %i non two manifold edges and %i faces are incident on these edges\n",edgeManif,faceEdgeManif);

        if(vertManif!=0) Log("Mesh has %i non two manifold vertexes and %i faces are incident on these vertices\n",vertManif,faceVertManif);
				
				// For Manifold meshes compute some other stuff
				if(vertManif && edgeManif)
				{
					holeNum = tri::Clean<CMeshO>::CountHoles(m);
					Log("Mesh has %i holes",holeNum);
					
					int genus = tri::Clean<CMeshO>::MeshGenus(m, holeNum, connectedComponentsNum, edgeNum);
					Log("Genus is %i",genus);
				}
			}
		break;
		/************************************************************/ 
		case FP_MEASURE_TOPO_QUAD : 
			{
					CMeshO &m=md.mm()->cm;	
					md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);				
					md.mm()->updateDataMask(MeshModel::MM_FACEQUALITY);				

					if (! tri::Clean<CMeshO>::IsFFAdjacencyConsistent(m)){
											Log("Error: mesh has a not consistent FF adjacency");
											return false;
										}
					if (! tri::Clean<CMeshO>::HasConsistentPerFaceFauxFlag(m)) {
											Log("Warning: mesh has a not consistent FauxEdge tagging");
											return false;
										}
					if (! tri::Clean<CMeshO>::IsBitTriQuadOnly(m)) {
											Log("Warning: IsBitTriQuadOnly");
											//return false;
										}
					//										if (! tri::Clean<CMeshO>::HasConsistentEdges(m)) lastErrorDetected |= NOT_EDGES_CONST;
					int nsinglets= tri::BitQuadOptimization< tri::BitQuad<CMeshO> >::MarkSinglets(m);
					if ( nsinglets  )  {
											Log("Warning: MarkSinglets");
											//return false;
										}
					
					if (! tri::BitQuad<CMeshO>::HasConsistentValencyFlag(m))
					 {
											Log("Warning: HasConsistentValencyFlag");
											//return false;
										}
				
			int nQuads = tri::Clean<CMeshO>::CountBitQuads(m);
			int nTris = tri::Clean<CMeshO>::CountBitTris(m);
			int nPolys = tri::Clean<CMeshO>::CountBitPolygons(m);
			
			Log("Mesh has %i tri %i quad and %i polig",nTris,nQuads,nPolys);

			}
			break;
		/************************************************************/ 
		case FP_MEASURE_GEOM : 
			{
				CMeshO &m=md.mm()->cm;
				tri::Inertia<CMeshO> I;
				I.Compute(m);
				
				tri::UpdateBounding<CMeshO>::Box(m); 
				float Area = tri::Stat<CMeshO>::ComputeMeshArea(m);
				float Volume = I.Mass(); 
				Log("Mesh Bounding Box Size %f %f %f", m.bbox.DimX(), m.bbox.DimY(), m.bbox.DimZ());			
				Log("Mesh Bounding Box Diag %f ", m.bbox.Diag());			
				Log("Mesh Volume  is %f", Volume);			
				Log("Center of Mass  is %f %f %f", I.CenterOfMass()[0], I.CenterOfMass()[1], I.CenterOfMass()[2]);		
				
				
				Matrix33f IT;
				I.InertiaTensor(IT);
				Log("Inertia Tensor is :");		
        Log("    | %9.6f  %9.6f  %9.6f |",IT[0][0],IT[0][1],IT[0][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",IT[1][0],IT[1][1],IT[1][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",IT[2][0],IT[2][1],IT[2][2]);
				
				Log("Mesh Surface is %f", Area);
				
        Matrix44f PCA;
        Point4f pcav;
        I.InertiaTensorEigen(PCA,pcav);
        Log("Principal axes are :");
        Log("    | %9.6f  %9.6f  %9.6f |",PCA[0][0],PCA[0][1],PCA[0][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",PCA[1][0],PCA[1][1],PCA[1][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",PCA[2][0],PCA[2][1],PCA[2][2]);

       // Point3f ax0(PCA[0][0],PCA[0][1],PCA[0][2]);
       // Point3f ax1(PCA[1][0],PCA[1][1],PCA[1][2]);
       // Point3f ax2(PCA[2][0],PCA[2][1],PCA[2][2]);

       // Log("ax0*ax1 %f (len ax0 %f) ",ax0*ax1, Norm(ax0));
       // Log("ax1*ax2 %f (len ax1 %f) ",ax1*ax2, Norm(ax1));
       // Log("ax0*ax2 %f (len ax2 %f) ",ax0*ax2, Norm(ax2));

        Log("axis momenta are :");
        Log("    | %9.6f  %9.6f  %9.6f |",pcav[0],pcav[1],pcav[2]);

			}
		break;
        /************************************************************/
        case FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION :
  case FP_MEASURE_FACE_QUALITY_DISTRIBUTION :
			{
				CMeshO &m=md.mm()->cm;
				Distribution<float> DD;
        if(ID(filter)==FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION)
          tri::Stat<CMeshO>::ComputePerVertexQualityDistribution(m, DD, false);
        else
          tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m, DD, false);
				
				Log("   Min %f Max %f",DD.Min(),DD.Max());		
				Log("   Avg %f Med %f",DD.Avg(),DD.Percentile(0.5f));		
				Log("   StdDev		%f",DD.StandardDeviation());		
				Log("   Variance  %f",DD.Variance());						
			}
		break;

		case FP_MEASURE_GAUSSCURV : 
			{
				CMeshO &m=md.mm()->cm;
				SimpleTempData<CMeshO::VertContainer, float> TDArea(m.vert,0.0f);
				SimpleTempData<CMeshO::VertContainer, float> TDAngleSum(m.vert,0);

				tri::UpdateQuality<CMeshO>::VertexConstant(m,0);
				float angle[3];
				CMeshO::FaceIterator fi;
				for(fi=m.face.begin(); fi!= m.face.end(); ++fi)
					{
						angle[0] = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
						angle[1] = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
						angle[2] = M_PI-(angle[0]+angle[1]);

						float area= DoubleArea(*fi)/6.0f;
							for(int i=0;i<3;++i)
							{
								TDArea[(*fi).V(i)]+=area;
								TDAngleSum[(*fi).V(i)]+=angle[i];
							}
					}
				CMeshO::VertexIterator vi;
				float totalSum=0;
				for(vi=m.vert.begin(); vi!= m.vert.end(); ++vi)
					{
						(*vi).Q() = (2.0*M_PI-TDAngleSum[vi]);//*TDArea[vi];
						//if((*vi).IsS()) 
						totalSum += (*vi).Q(); 
					}
				Log("integrated is %f (%f*pi)", totalSum,totalSum/M_PI);												
			} break;
  case FP_MEASURE_VERTEX_QUALITY_HISTOGRAM:
  case FP_MEASURE_FACE_QUALITY_HISTOGRAM:
            {
            CMeshO &m=md.mm()->cm;
            float RangeMin = par.getFloat("minVal");
            float RangeMax = par.getFloat("maxVal");
            int binNum = par.getInt("binNum");


            Histogramf H;
            H.SetRange(RangeMin,RangeMax,binNum);
            if(ID(filter)==FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION)
            {
              for(CMeshO::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); ++vi)
                if(!(*vi).IsD())
                {
                assert(!math::IsNAN((*vi).Q()) && "You should never try to compute Histogram with Invalid Floating points numbers (NaN)");
                H.Add((*vi).Q());
              }
            }else{
              for(CMeshO::FaceIterator fi = m.face.begin(); fi != m.face.end(); ++fi)
                if(!(*fi).IsD())
                {
                assert(!math::IsNAN((*fi).Q()) && "You should never try to compute Histogram with Invalid Floating points numbers (NaN)");
                H.Add((*fi).Q());
              }
            }
            Log("(         -inf..%15.7f) : %i",RangeMin,H.BinCountInd(0));
            for(int i=1;i<=binNum;++i)
              Log("[%15.7f..%15.7f) : %i",H.BinLowerBound(i),H.BinUpperBound(i),H.BinCountInd(i));

            Log("[%15.7f..             +inf) : %i",RangeMax,H.BinCountInd(binNum+1));
        } break;

          default: assert(0);
  }
	return true;
}

 FilterMeasurePlugin::FilterClass FilterMeasurePlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_MEASURE_GAUSSCURV :
    case FP_MEASURE_GEOM :
    case FP_MEASURE_TOPO :
    case FP_MEASURE_TOPO_QUAD :
  case FP_MEASURE_VERTEX_QUALITY_DISTRIBUTION:
  case FP_MEASURE_VERTEX_QUALITY_HISTOGRAM:
  case FP_MEASURE_FACE_QUALITY_DISTRIBUTION:
  case FP_MEASURE_FACE_QUALITY_HISTOGRAM:
      return MeshFilterInterface::Measure;
		default :  assert(0);
			return MeshFilterInterface::Generic;
  }
}

Q_EXPORT_PLUGIN(FilterMeasurePlugin)
