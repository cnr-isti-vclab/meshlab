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
#include<vcg/complex/algorithms/mesh_to_matrix.h>
#include<vcg/complex/algorithms/bitquad_optimization.h>
#include "filter_measure.h"

using namespace std;
using namespace vcg;


// Core Function doing the actual mesh processing.
bool FilterMeasurePlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos * /*cb*/ )
{
	if (filterName == "Compute Topological Measures")
	{
		CMeshO &m = md.mm()->cm;
		tri::Allocator<CMeshO>::CompactFaceVector(m);
		tri::Allocator<CMeshO>::CompactVertexVector(m);
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
		md.mm()->updateDataMask(MeshModel::MM_VERTFACETOPO);

		int edgeNonManifFFNum = tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m, true);
		int faceEdgeManif = tri::UpdateSelection<CMeshO>::FaceCount(m);
		tri::UpdateSelection<CMeshO>::VertexClear(m);
		tri::UpdateSelection<CMeshO>::FaceClear(m);

		int vertManifNum = tri::Clean<CMeshO>::CountNonManifoldVertexFF(m, true);
		tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
		int faceVertManif = tri::UpdateSelection<CMeshO>::FaceCount(m);
		int edgeNum = 0, edgeBorderNum = 0, edgeNonManifNum = 0;
		tri::Clean<CMeshO>::CountEdgeNum(m, edgeNum, edgeBorderNum, edgeNonManifNum);
		assert(edgeNonManifFFNum == edgeNonManifNum);
		int holeNum;
		Log("V: %6i E: %6i F:%6i", m.vn, edgeNum, m.fn);
		int unrefVertNum = tri::Clean<CMeshO>::CountUnreferencedVertex(m);
		Log("Unreferenced Vertices %i", unrefVertNum);
		Log("Boundary Edges %i", edgeBorderNum);

		int connectedComponentsNum = tri::Clean<CMeshO>::CountConnectedComponents(m);
		Log("Mesh is composed by %i connected component(s)\n", connectedComponentsNum);

		if (edgeNonManifFFNum == 0 && vertManifNum == 0){
			Log("Mesh is two-manifold ");
		}

		if (edgeNonManifFFNum != 0) Log("Mesh has %i non two manifold edges and %i faces are incident on these edges\n", edgeNonManifFFNum, faceEdgeManif);
		if (vertManifNum != 0) Log("Mesh has %i non two manifold vertexes and %i faces are incident on these vertices\n", vertManifNum, faceVertManif);

		// For Manifold meshes compute some other stuff
		if (vertManifNum == 0 && edgeNonManifFFNum == 0)
		{
			holeNum = tri::Clean<CMeshO>::CountHoles(m);
			Log("Mesh has %i holes", holeNum);

			int genus = tri::Clean<CMeshO>::MeshGenus(m.vn - unrefVertNum, edgeNum, m.fn, holeNum, connectedComponentsNum);
			Log("Genus is %i", genus);
		}
		else
		{
			Log("Mesh has a undefined number of holes (non 2-manifold mesh)");
			Log("Genus is undefined (non 2-manifold mesh)");
		}

		return true;
	}

    /************************************************************/
	if (filterName == "Compute Topological Measures for Quad Meshes")
	{
		CMeshO &m = md.mm()->cm;
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
		md.mm()->updateDataMask(MeshModel::MM_FACEQUALITY);

		if (!tri::Clean<CMeshO>::IsFFAdjacencyConsistent(m))
		{
			this->errorMessage = "Error: mesh has a not consistent FF adjacency";
			return false;
		}
		if (!tri::Clean<CMeshO>::HasConsistentPerFaceFauxFlag(m)) {

			this->errorMessage = "QuadMesh problem: mesh has a not consistent FauxEdge tagging";
			return false;
		}

		int nQuads = tri::Clean<CMeshO>::CountBitQuads(m);
		int nTris = tri::Clean<CMeshO>::CountBitTris(m);
		int nPolys = tri::Clean<CMeshO>::CountBitPolygons(m);
		int nLargePolys = tri::Clean<CMeshO>::CountBitLargePolygons(m);
		if (nLargePolys>0) nQuads = 0;

		Log("Mesh has %8i triangles \n", nTris);
		Log("         %8i quads \n", nQuads);
		Log("         %8i polygons \n", nPolys);
		Log("         %8i large polygons (with internal faux vertexes)", nLargePolys);

		if (!tri::Clean<CMeshO>::IsBitTriQuadOnly(m))
		{
			this->errorMessage = "QuadMesh problem: the mesh is not TriQuadOnly";
			return false;
		}

		//
		//   i
		//
		//
		//   i+1     i+2
		tri::UpdateFlags<CMeshO>::FaceClearV(m);
		Distribution<float> AngleD; // angle distribution
		Distribution<float> RatioD; // ratio distribution
		tri::UpdateFlags<CMeshO>::FaceClearV(m);
		for (CMeshO::FaceIterator fi = m.face.begin(); fi != m.face.end(); ++fi)
		if (!fi->IsV())
		{
			fi->SetV();
			// Collect the vertices
			Point3m qv[4];
			bool quadFound = false;
			for (int i = 0; i<3; ++i)
			{
				if ((*fi).IsF(i) && !(*fi).IsF((i + 1) % 3) && !(*fi).IsF((i + 2) % 3))
				{
					qv[0] = fi->V0(i)->P(),
						qv[1] = fi->FFp(i)->V2(fi->FFi(i))->P(),
						qv[2] = fi->V1(i)->P(),
						qv[3] = fi->V2(i)->P();
					quadFound = true;
				}
			}
			assert(quadFound);
			for (int i = 0; i<4; ++i)
				AngleD.Add(fabs(90 - math::ToDeg(Angle(qv[(i + 0) % 4] - qv[(i + 1) % 4], qv[(i + 2) % 4] - qv[(i + 1) % 4]))));
			float edgeLen[4];

			for (int i = 0; i<4; ++i)
				edgeLen[i] = Distance(qv[(i + 0) % 4], qv[(i + 1) % 4]);
			std::sort(edgeLen, edgeLen + 4);
			RatioD.Add(edgeLen[0] / edgeLen[3]);
		}

		Log("Right Angle Discrepancy  Avg %4.3f Min %4.3f Max %4.3f StdDev %4.3f Percentile 0.05 %4.3f percentile 95 %4.3f",
			AngleD.Avg(), AngleD.Min(), AngleD.Max(), AngleD.StandardDeviation(), AngleD.Percentile(0.05), AngleD.Percentile(0.95));

		Log("Quad Ratio   Avg %4.3f Min %4.3f Max %4.3f", RatioD.Avg(), RatioD.Min(), RatioD.Max());
		return true;
	}

	/************************************************************/
	if (filterName == "Compute Geometric Measures")
	{
		CMeshO &m = md.mm()->cm;
		bool watertight = false;
		bool pointcloud = false;

		// the mesh has to be correctly transformed
		if (m.Tr != Matrix44m::Identity())
		{
			Log("BEWARE: Measures are calculated considering the current transformation matrix");
			tri::UpdatePosition<CMeshO>::Matrix(m, m.Tr, true);
		}

		// bounding box
		Log("Mesh Bounding Box Size %f  %f  %f", m.bbox.DimX(), m.bbox.DimY(), m.bbox.DimZ());
		Log("Mesh Bounding Box Diag %f ", m.bbox.Diag());
		Log("Mesh Bounding Box min %f  %f  %f", m.bbox.min[0], m.bbox.min[1], m.bbox.min[2]);
		Log("Mesh Bounding Box max %f  %f  %f", m.bbox.max[0], m.bbox.max[1], m.bbox.max[2]);

		// is pointcloud?
		if ((m.fn == 0) && (m.vn != 0))
			pointcloud = true;

		if (pointcloud)
		{
			// cloud barycenter
			Point3m bc = tri::Stat<CMeshO>::ComputeCloudBarycenter(m, false);
			Log("Pointcloud (vertex) barycenter  %9.6f  %9.6f  %9.6f", bc[0], bc[1], bc[2]);

			// if there is vertex quality, also provide weighted barycenter
			if (tri::HasPerVertexQuality(m))
			{
				bc = tri::Stat<CMeshO>::ComputeCloudBarycenter(m, true);
				Log("Pointcloud (vertex) barycenter, weighted by verytex quality:");
				Log("  %9.6f  %9.6f  %9.6f", bc[0], bc[1], bc[2]);
			}

			// principal axis
			Matrix33m PCA;
			PCA = computePrincipalAxisCloud(m);
			Log("Principal Axes are :");
			Log("    | %9.6f  %9.6f  %9.6f |", PCA[0][0], PCA[0][1], PCA[0][2]);
			Log("    | %9.6f  %9.6f  %9.6f |", PCA[1][0], PCA[1][1], PCA[1][2]);
			Log("    | %9.6f  %9.6f  %9.6f |", PCA[2][0], PCA[2][1], PCA[2][2]);
		}
		else
		{
			// area
			float Area = tri::Stat<CMeshO>::ComputeMeshArea(m);
			Log("Mesh Surface Area is %f", Area);

			// edges
			Distribution<float> eDist;
			tri::Stat<CMeshO>::ComputeFaceEdgeLengthDistribution(m, eDist, false);
			Log("Mesh Total Len of %i Edges is %f Avg Len %f", int(eDist.Cnt()), eDist.Sum(), eDist.Avg());
			tri::Stat<CMeshO>::ComputeFaceEdgeLengthDistribution(m, eDist, true);
			Log("Mesh Total Len of %i Edges is %f Avg Len %f (including faux edges))", int(eDist.Cnt()), eDist.Sum(), eDist.Avg());

			// Thin shell barycenter
			Point3m bc = tri::Stat<CMeshO>::ComputeShellBarycenter(m);
			Log("Thin shell (faces) barycenter:  %9.6f  %9.6f  %9.6f", bc[0], bc[1], bc[2]);

			// cloud barycenter
			bc = tri::Stat<CMeshO>::ComputeCloudBarycenter(m, false);
			Log("Vertices barycenter  %9.6f  %9.6f  %9.6f", bc[0], bc[1], bc[2]);

			// is watertight?
			int edgeNum = 0, edgeBorderNum = 0, edgeNonManifNum = 0;
			tri::Clean<CMeshO>::CountEdgeNum(m, edgeNum, edgeBorderNum, edgeNonManifNum);
			watertight = (edgeBorderNum == 0) && (edgeNonManifNum == 0);
			if (watertight)
			{
				tri::Inertia<CMeshO> I(m);

				// volume
				float Volume = I.Mass();
				Log("Mesh Volume  is %f", Volume);

				// center of mass
				Log("Center of Mass  is %f %f %f", I.CenterOfMass()[0], I.CenterOfMass()[1], I.CenterOfMass()[2]);

				// inertia tensor
				Matrix33m IT;
				I.InertiaTensor(IT);
				Log("Inertia Tensor is :");
				Log("    | %9.6f  %9.6f  %9.6f |", IT[0][0], IT[0][1], IT[0][2]);
				Log("    | %9.6f  %9.6f  %9.6f |", IT[1][0], IT[1][1], IT[1][2]);
				Log("    | %9.6f  %9.6f  %9.6f |", IT[2][0], IT[2][1], IT[2][2]);

				// principal axis
				Matrix33m PCA;
				Point3m pcav;
				I.InertiaTensorEigen(PCA, pcav);
				Log("Principal axes are :");
				Log("    | %9.6f  %9.6f  %9.6f |", PCA[0][0], PCA[0][1], PCA[0][2]);
				Log("    | %9.6f  %9.6f  %9.6f |", PCA[1][0], PCA[1][1], PCA[1][2]);
				Log("    | %9.6f  %9.6f  %9.6f |", PCA[2][0], PCA[2][1], PCA[2][2]);

				Log("axis momenta are :");
				Log("    | %9.6f  %9.6f  %9.6f |", pcav[0], pcav[1], pcav[2]);

			}
			else
			{
				Log("Mesh is not 'watertight', no information on volume, barycenter and inertia tensor.");

				// principal axis
				Matrix33m PCA;
				PCA = computePrincipalAxisCloud(m);
				Log("Principal axes are :");
				Log("    | %9.6f  %9.6f  %9.6f |", PCA[0][0], PCA[0][1], PCA[0][2]);
				Log("    | %9.6f  %9.6f  %9.6f |", PCA[1][0], PCA[1][1], PCA[1][2]);
				Log("    | %9.6f  %9.6f  %9.6f |", PCA[2][0], PCA[2][1], PCA[2][2]);
			}

		}

		// the mesh has to return to its original position
		if (m.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(m, Inverse(m.Tr), true);

		return true;
	}

	/************************************************************/
	if (filterName == "Compute Area/Perimeter of selection")
	{
		CMeshO &m = md.mm()->cm;
		if (m.sfn == 0) // no face selected, fail
		{
			Log("There is no face selection!");
			return false;
		}

		Log("Selection is %i triangles", m.sfn);
		if (m.Tr != Matrix44m::Identity())
			Log("BEWARE: Area and Perimeter are calculated considering the current transformation matrix");

		double fArea = 0.0;
		double sArea=0;
		for (CMeshO::FaceIterator fi = m.face.begin(); fi != m.face.end(); ++fi)
		if (!(*fi).IsD())
		if ((*fi).IsS())
		{
			// this line calculates the doublearea, I took the code from Triangle3.h (DoubleArea) and modified to use transformation
			fArea = (((m.Tr * (*fi).cP(1)) - (m.Tr * (*fi).cP(0))) ^ ((m.Tr * (*fi).cP(2)) - (m.Tr * (*fi).cP(0)))).Norm();
			sArea += fArea / 2.0;
		}
		Log("Selection Surface Area is %f", sArea);

		int ePerimeter = 0;
		double sPerimeter = 0.0;
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
		for (CMeshO::FaceIterator fi = m.face.begin(); fi != m.face.end(); ++fi)
		if (!(*fi).IsD())
		if ((*fi).IsS())
		{
			for (int ei = 0; ei < 3; ei++)
			{
				CMeshO::FacePointer adjf = (*fi).FFp(ei);

				if (adjf == &(*fi) || !(adjf->IsS()))
				{
					ePerimeter += 1;
					sPerimeter += ((m.Tr * (*fi).V(ei)->P()) - (m.Tr * (*fi).V((ei + 1) % 3)->P())).Norm();
				}
			}
		}

		Log("Selection border is %i edges", ePerimeter);
		Log("Perimeter is %f", sPerimeter);

		return true;
	}

	/************************************************************/
	if ((filterName == "Per Vertex Quality Stat") || (filterName == "Per Face Quality Stat"))
	{
		CMeshO &m = md.mm()->cm;
		Distribution<float> DD;
		if (filterName == "Per Vertex Quality Stat")
			tri::Stat<CMeshO>::ComputePerVertexQualityDistribution(m, DD, false);
		else
			tri::Stat<CMeshO>::ComputePerFaceQualityDistribution(m, DD, false);

		Log("   Min %f Max %f", DD.Min(), DD.Max());
		Log("   Avg %f Med %f", DD.Avg(), DD.Percentile(0.5f));
		Log("   StdDev     %f", DD.StandardDeviation());
		Log("   Variance   %f", DD.Variance());
		return true;
	}

	/************************************************************/
	if ((filterName == "Per Vertex Quality Histogram") || (filterName == "Per Face Quality Histogram"))
	{
		CMeshO &m = md.mm()->cm;
		tri::Allocator<CMeshO>::CompactEveryVector(m);
		float RangeMin = env.evalFloat("HistMin");
		float RangeMax = env.evalFloat("HistMax");
		int binNum = env.evalInt("binNum");
		bool areaFlag = env.evalBool("areaWeighted");

		Histogramf H;
		H.SetRange(RangeMin, RangeMax, binNum);
		if (filterName == "Per Vertex Quality Histogram")
		{
			vector<Scalarm> aVec(m.vn, 1.0);
			if (areaFlag)
				tri::MeshToMatrix<CMeshO>::PerVertexArea(m, aVec);

			for (size_t i = 0; i<m.vn; ++i)
				H.Add(m.vert[i].Q(), aVec[i]);
		}
		else{
			vector<Scalarm> aVec(m.fn, 1.0);
			if (areaFlag)
				tri::MeshToMatrix<CMeshO>::PerFaceArea(m, aVec);

			for (size_t i = 0; i<m.fn; ++i)
				H.Add(m.face[i].Q(), aVec[i]);
		}
		if (areaFlag)
		{
			Log("(         -inf..%15.7f) : %15.7f", RangeMin, H.BinCountInd(0));
			for (int i = 1; i <= binNum; ++i)
				Log("[%15.7f..%15.7f) : %15.7f", H.BinLowerBound(i), H.BinUpperBound(i), H.BinCountInd(i));
			Log("[%15.7f..             +inf) : %15.7f", RangeMax, H.BinCountInd(binNum + 1));
		}
		else
		{
			Log("(         -inf..%15.7f) : %4.0f", RangeMin, H.BinCountInd(0));
			for (int i = 1; i <= binNum; ++i)
				Log("[%15.7f..%15.7f) : %4.0f", H.BinLowerBound(i), H.BinUpperBound(i), H.BinCountInd(i));
			Log("[%15.7f..             +inf) : %4.0f", RangeMax, H.BinCountInd(binNum + 1));
		}
		if (filterName == "Per Vertex Quality Histogram")
		{
			vector<Scalarm> aVec(m.vn, 1.0);
			if (areaFlag)
				tri::MeshToMatrix<CMeshO>::PerVertexArea(m, aVec);

			for (size_t i = 0; i<m.vn; ++i)
				H.Add(m.vert[i].Q(), aVec[i]);
		}
		else{
			vector<Scalarm> aVec(m.fn, 1.0);
			if (areaFlag)
				tri::MeshToMatrix<CMeshO>::PerFaceArea(m, aVec);

			for (size_t i = 0; i<m.fn; ++i)
				H.Add(m.face[i].Q(), aVec[i]);
		}
		if (areaFlag)
		{
			Log("(         -inf..%15.7f) : %15.7f", RangeMin, H.BinCountInd(0));
			for (int i = 1; i <= binNum; ++i)
				Log("[%15.7f..%15.7f) : %15.7f", H.BinLowerBound(i), H.BinUpperBound(i), H.BinCountInd(i));
			Log("[%15.7f..             +inf) : %15.7f", RangeMax, H.BinCountInd(binNum + 1));
		}
		else
		{
			Log("(         -inf..%15.7f) : %4.0f", RangeMin, H.BinCountInd(0));
			for (int i = 1; i <= binNum; ++i)
				Log("[%15.7f..%15.7f) : %4.0f", H.BinLowerBound(i), H.BinUpperBound(i), H.BinCountInd(i));
			Log("[%15.7f..             +inf) : %4.0f", RangeMax, H.BinCountInd(binNum + 1));
		}
		return true;
	}

	// should never reach this :)
	return false;
}

// function to calculate principal axis for pointclouds or non-watertight meshes
Matrix33m FilterMeasurePlugin::computePrincipalAxisCloud(CMeshO & m)
{
	Matrix33m cov;
	Point3m bp(0, 0, 0);
	vector<Point3m> PtVec;
	for (CMeshO::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); ++vi)
	if (!(*vi).IsD()) 
	{
		PtVec.push_back((*vi).cP());
		bp += (*vi).cP();
	}

	bp /= m.vn;

	cov.Covariance(PtVec, bp);

	Matrix33m eigenvecMatrix;
	Eigen::Matrix3d em;
	cov.ToEigenMatrix(em);
	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig(em);
	Eigen::Matrix3d c_vec = eig.eigenvectors();
	eigenvecMatrix.FromEigenMatrix(c_vec);

	return eigenvecMatrix;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeasurePlugin)

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
