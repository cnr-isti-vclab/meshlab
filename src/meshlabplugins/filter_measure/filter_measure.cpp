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
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/stat.h>

#include <vcg/complex/algorithms/update/selection.h>
#include<vcg/complex/append.h>
#include<vcg/simplex/face/pos.h>
#include<vcg/complex/algorithms/bitquad_support.h>
#include<vcg/complex/algorithms/bitquad_optimization.h>
#include "filter_measure.h"

using namespace std;
using namespace vcg;

MeasureTopoTag::MeasureTopoTag(MeshDocument &parent, MeshModel *mm, QString name): TagBase(&parent)
{
	typeName ="Topological Measures";
	filterOwner =name;
	referringMeshes.append(mm->id());
	
	edges =-1;
	boundaryEdges=-1;
	connectComp=-1;
	manifoldness =false;
	genus=-1;
	holes=-1;
	edgeManifNum=-1;
	vertManifNum=-1;
	edgeManifNum=-1;
	faceEdgeManif=-1;
	vertManifNum=-1;
	faceVertManif=-1;
};


// Core Function doing the actual mesh processing.
bool FilterMeasurePlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos * /*cb*/ )
{
	if (filterName == "Compute Topological Measures")
		{
			CMeshO &m=md.mm()->cm;	
			md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);				
			md.mm()->updateDataMask(MeshModel::MM_VERTFACETOPO);				
			tri::Allocator<CMeshO>::CompactFaceVector(m);
			tri::Allocator<CMeshO>::CompactVertexVector(m);
			tri::UpdateTopology<CMeshO>::FaceFace(m);
			tri::UpdateTopology<CMeshO>::VertexFace(m);

			MeasureTopoTag *tag = new MeasureTopoTag(md, md.mm(), filterName );

			int edgeManifNum = tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m,true);
			int faceEdgeManif = tri::UpdateSelection<CMeshO>::FaceCount(m);
			tri::UpdateSelection<CMeshO>::VertexClear(m);
			tri::UpdateSelection<CMeshO>::FaceClear(m);
			tag->edgeManifNum = edgeManifNum;
			tag->faceEdgeManif = faceEdgeManif;

			int vertManifNum = tri::Clean<CMeshO>::CountNonManifoldVertexFF(m,true);
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
			int faceVertManif = tri::UpdateSelection<CMeshO>::FaceCount(m);
			int edgeNum=0,borderNum=0;
			tri::Clean<CMeshO>::CountEdges(m, edgeNum, borderNum);
			int holeNum;
			Log("V: %6i E: %6i F:%6i",m.vn,edgeNum,m.fn);
			int unrefVertNum = tri::Clean<CMeshO>::CountUnreferencedVertex(m);
			Log("Unreferenced Vertices %i",unrefVertNum);
			Log("Boundary Edges %i",borderNum);
			tag->edges = edgeNum;
			tag->boundaryEdges=borderNum;
			tag->vertManifNum= vertManifNum;
			tag->faceEdgeManif = faceEdgeManif;


			int connectedComponentsNum = tri::Clean<CMeshO>::CountConnectedComponents(m);
			Log("Mesh is composed by %i connected component(s)\n",connectedComponentsNum);
			tag->connectComp=connectedComponentsNum;

			if(edgeManifNum==0 && vertManifNum==0){
				Log("Mesh has is two-manifold ");
				tag->manifoldness=true;
			}

			if(edgeManifNum!=0) Log("Mesh has %i non two manifold edges and %i faces are incident on these edges\n",edgeManifNum,faceEdgeManif);

			if(vertManifNum!=0) Log("Mesh has %i non two manifold vertexes and %i faces are incident on these vertices\n",vertManifNum,faceVertManif);

			// For Manifold meshes compute some other stuff
			if(vertManifNum==0 && edgeManifNum==0)
			{
				holeNum = tri::Clean<CMeshO>::CountHoles(m);
				Log("Mesh has %i holes",holeNum);
				tag->holes=holeNum;

				int genus = tri::Clean<CMeshO>::MeshGenus(m.vn-unrefVertNum, edgeNum, m.fn, holeNum, connectedComponentsNum);
				Log("Genus is %i",genus);
				tag->genus=genus;
			}
			else
			{
				Log("Mesh has a undefined number of holes (non 2-manifold mesh)");
				Log("Genus is undefined (non 2-manifold mesh)");
			}

			md.addNewTag(tag);
			return true;
		}

	/************************************************************/
	if (filterName == "Compute Topological Measures for Quad Meshes")
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

      //
      //   i
      //
      //
      //   i+1     i+2
      tri::UpdateFlags<CMeshO>::FaceClearV(m);
      Distribution<float> ad; // angle distributio
      Distribution<float> rd; // ratio distribution

      for(CMeshO::FaceIterator fi=m.face.begin();fi!=m.face.end();++fi)
      {

        // Collect the vertices
        Point3f qv[4];
        for(int i=0;i<3;++i)
        {
         if(!(*fi).IsF(i) && !(*fi).IsF((i+1)%3))
         {
          qv[0]= (*fi).cP0(i);
          qv[1]= (*fi).cP1(i);
          qv[2]= (*fi).cP2(i);
          face::Pos<CFaceO> pp(&*fi,(i+2)%3,fi->V(i));
          assert(pp.F()->IsF(pp.E()));
          pp.FlipF();pp.FlipE();pp.FlipV();
          qv[3]= pp.V()->cP();
          break;
         }

         for(int i=0;i<4;++i)
             ad.Add(fabs(90-math::ToDeg(Angle(qv[(i+0)%4] - qv[(i+1)%4], qv[(i+2)%4] - qv[(i+1)%4]))));
         float edgeLen[4];

         for(int i=0;i<4;++i)
             edgeLen[i]=Distance(qv[(i+0)%4],qv[(i+1)%4]);
         std::sort(edgeLen,edgeLen+4);
         rd.Add(edgeLen[0]/edgeLen[3]);
        }
      }

      Log("Right Angle Discrepancy  Avg %4.3f Min %4.3f Max %4.3f StdDev %4.3f Percentile 0.05 %4.3f percentile 95 %4.3f",
                          ad.Avg(), ad.Min(), ad.Max(),ad.StandardDeviation(),ad.Percentile(0.05),ad.Percentile(0.95));

      Log("Quad Ratio   Avg %4.3f Min %4.3f Max %4.3f", rd.Avg(), rd.Min(), rd.Max());
      return true;
		}
		/************************************************************/ 
	if(filterName == "Compute Geometric Measures")
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
      Log("Mesh Surface is %f", Area);
      Point3f bc=tri::Stat<CMeshO>::ComputeShellBarycenter(m);
      Log("Thin shell barycenter  %9.6f  %9.6f  %9.6f",bc[0],bc[1],bc[2]);

      if(Volume<=0) Log("Mesh is not 'solid', no information on barycenter and inertia tensor.");
      else
      {
        Log("Center of Mass  is %f %f %f", I.CenterOfMass()[0], I.CenterOfMass()[1], I.CenterOfMass()[2]);

        Matrix33f IT;
        I.InertiaTensor(IT);
        Log("Inertia Tensor is :");
        Log("    | %9.6f  %9.6f  %9.6f |",IT[0][0],IT[0][1],IT[0][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",IT[1][0],IT[1][1],IT[1][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",IT[2][0],IT[2][1],IT[2][2]);

        Matrix44f PCA;
        Point4f pcav;
        I.InertiaTensorEigen(PCA,pcav);
        Log("Principal axes are :");
        Log("    | %9.6f  %9.6f  %9.6f |",PCA[0][0],PCA[0][1],PCA[0][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",PCA[1][0],PCA[1][1],PCA[1][2]);
        Log("    | %9.6f  %9.6f  %9.6f |",PCA[2][0],PCA[2][1],PCA[2][2]);

        Log("axis momenta are :");
        Log("    | %9.6f  %9.6f  %9.6f |",pcav[0],pcav[1],pcav[2]);
      }
      return true;
		}
		/************************************************************/
	if((filterName == "Per Vertex Quality Stat") || (filterName == "Per Face Quality Stat") )
		{
			CMeshO &m=md.mm()->cm;
			Distribution<float> DD;
			if(filterName == "Per Vertex Quality Stat")
				tri::Stat<CMeshO>::ComputePerVertexQualityDistribution(m, DD, false);
			else
				tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m, DD, false);

			Log("   Min %f Max %f",DD.Min(),DD.Max());		
			Log("   Avg %f Med %f",DD.Avg(),DD.Percentile(0.5f));		
			Log("   StdDev		%f",DD.StandardDeviation());		
			Log("   Variance  %f",DD.Variance());
			return true;
		}

	if((filterName == "Per Vertex Quality Histogram") || (filterName == "Per Face Quality Histogram") )
		{
			CMeshO &m=md.mm()->cm;
			float RangeMin = env.evalFloat("HistMin");
			float RangeMax = env.evalFloat("HistMax");
			int binNum     = env.evalInt("binNum");

			Histogramf H;
			H.SetRange(RangeMin,RangeMax,binNum);
			if(filterName == "Per Vertex Quality Histogram")
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
			Log("(         -inf..%15.7f) : %4.0f",RangeMin,H.BinCountInd(0));
			for(int i=1;i<=binNum;++i)
			  Log("[%15.7f..%15.7f) : %4.0f",H.BinLowerBound(i),H.BinUpperBound(i),H.BinCountInd(i));
			Log("[%15.7f..             +inf) : %4.0f",RangeMax,H.BinCountInd(binNum+1));
			return true;
        }
    return false;
}

QTreeWidgetItem * FilterMeasurePlugin::tagDump(TagBase * _tag, MeshDocument &/*md*/, MeshModel */*mm*/)
{
	MeasureTopoTag *tag = (MeasureTopoTag *) _tag;
	QTreeWidgetItem *newTag = new QTreeWidgetItem();
	newTag->setText(2, QString::number(tag->id()));	
	newTag->setText(3, tag->typeName);
	QFont font = QFont();
	font.setItalic(true);
	newTag->setFont(3,font);

	QTreeWidgetItem *edges = new QTreeWidgetItem();
	edges->setText(3, QString("Edges"));
	edges->setText(4, QString::number(tag->edges));
	newTag->addChild(edges);

	QTreeWidgetItem *bEdges = new QTreeWidgetItem();
	bEdges->setText(3, QString("Boundary Edges"));
	bEdges->setText(4, QString::number(tag->boundaryEdges));
	newTag->addChild(bEdges);

	QTreeWidgetItem *ccomp = new QTreeWidgetItem();
	ccomp->setText(3, QString("Connected Comp"));
	ccomp->setText(4, QString::number(tag->connectComp));
	newTag->addChild(ccomp);

	QTreeWidgetItem *manif = new QTreeWidgetItem();
	manif->setText(3, QString("Manifold"));
	if(tag->manifoldness)
		manif->setText(4, QString("Yes"));
	else manif->setText(4, QString("No"));
	newTag->addChild(manif);

	QTreeWidgetItem *genus = new QTreeWidgetItem();
	genus->setText(3, QString("Genus"));
	if(tag->vertManifNum==0 && tag->edgeManifNum==0)
		genus->setText(4, QString::number(tag->genus));
	else genus->setText(4, QString("-"));
	newTag->addChild(genus);

	QTreeWidgetItem *holes = new QTreeWidgetItem();
	holes->setText(3, QString("Holes"));
	if(tag->vertManifNum==0 && tag->edgeManifNum==0)
		holes->setText(4, QString::number(tag->holes));
	else holes->setText(4, QString("-"));
	newTag->addChild(holes);


	return newTag;
}

Q_EXPORT_PLUGIN(FilterMeasurePlugin)

//case FP_MEASURE_GAUSSCURV :
//	{
//		CMeshO &m=md.mm()->cm;
//		SimpleTempData<CMeshO::VertContainer, float> TDArea(m.vert,0.0f);
//		SimpleTempData<CMeshO::VertContainer, float> TDAngleSum(m.vert,0);

//		tri::UpdateQuality<CMeshO>::VertexConstant(m,0);
//		float angle[3];
//		CMeshO::FaceIterator fi;
//		for(fi=m.face.begin(); fi!= m.face.end(); ++fi)
//		{
//			angle[0] = math::Abs(Angle(	(*fi).P(1)-(*fi).P(0),(*fi).P(2)-(*fi).P(0) ));
//			angle[1] = math::Abs(Angle(	(*fi).P(0)-(*fi).P(1),(*fi).P(2)-(*fi).P(1) ));
//			angle[2] = M_PI-(angle[0]+angle[1]);

//			float area= DoubleArea(*fi)/6.0f;
//			for(int i=0;i<3;++i)
//			{
//				TDArea[(*fi).V(i)]+=area;
//				TDAngleSum[(*fi).V(i)]+=angle[i];
//			}
//		}
//		CMeshO::VertexIterator vi;
//		float totalSum=0;
//		for(vi=m.vert.begin(); vi!= m.vert.end(); ++vi)
//		{
//			(*vi).Q() = (2.0*M_PI-TDAngleSum[vi]);//*TDArea[vi];
//			//if((*vi).IsS())
//			totalSum += (*vi).Q();
//		}
//		Log("integrated is %f (%f*pi)", totalSum,totalSum/M_PI);
//	} break;
