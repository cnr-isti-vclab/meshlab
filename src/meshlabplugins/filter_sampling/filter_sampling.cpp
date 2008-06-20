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
/****************************************************************************
  History
$Log: samplefilter.cpp,v $
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/space/index/grid_static_ptr.h>
#include "filter_sampling.h"

using namespace vcg;
using namespace std;
// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

SampleFilterDocPlugin::SampleFilterDocPlugin() 
{ 
	typeList 
			<< FP_ELEMENT_SAMPLING 
			<< FP_MONTECARLO_SAMPLING
			<< FP_SIMILAR_SAMPLING
			<< FP_SUBDIV_SAMPLING
			<< FP_HAUSDORFF_DISTANCE
			<< FP_TEXEL_SAMPLING
	;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString SampleFilterDocPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_ELEMENT_SAMPLING    :  return QString("Mesh Element Sampling"); 
		case FP_MONTECARLO_SAMPLING :  return QString("Montecarlo Sampling"); 
		case FP_SIMILAR_SAMPLING :  return QString("Similar Triangle Sampling"); 
		case FP_SUBDIV_SAMPLING :  return QString("Regular Sudiv. Sampling"); 
		case FP_HAUSDORFF_DISTANCE  :  return QString("Hausdorff Distance"); 
		case FP_TEXEL_SAMPLING  :  return QString("Texel Sampling"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString SampleFilterDocPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_ELEMENT_SAMPLING    :  return QString("Create a new layer populated with a point sampling of the current mesh, a sample for each element of the mesh is generated"); 
		case FP_MONTECARLO_SAMPLING :  return QString("Create a new layer populated with a point sampling of the current mesh; samples are generated in a randomly uniform way, or with a distribution biased by the per-vertex quality values of the mesh."); 
		case FP_SIMILAR_SAMPLING		:  return QString("Create a new layer populated with a point sampling of the current mesh; to generate multiple samples inside a triangle it is subdivided into similar triangles and the internal vertices of these triangles are considered. Distribution is biased by the shape of the triangles."); 
		case FP_SUBDIV_SAMPLING			:  return QString("Create a new layer populated with a point sampling of the current mesh; to generate multiple samples inside a triangle"); 
		case FP_HAUSDORFF_DISTANCE  :  return QString("Hausdorff Distance"); 
		case FP_TEXEL_SAMPLING      :  return QString("Create a new layer with a point sampling of the current mesh, a sample for each texel of the mesh is generated"); 
		default : assert(0); 
	}
}

const PluginInfo &SampleFilterDocPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void SampleFilterDocPlugin::initParameterSet(QAction *action, MeshDocument & md, FilterParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_MONTECARLO_SAMPLING :  
 		  parlst.addInt ("SampleNum",
											md.mm()->cm.vn,
											"Number of samples",
											"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt.");
			parlst.addBool("Weighted",
										 false,
										 "Quality Weighted Sampling",
										 "Use per vertex quality to drive the vertex sampling. The number of samples falling in each face is proportional to the face area multiplied by the average quality of the face vertices.");
											break;
		case FP_SIMILAR_SAMPLING :  
		case FP_SUBDIV_SAMPLING :  
 		  parlst.addInt ("SampleNum",
										 std::max(100000,md.mm()->cm.vn),
											"Number of samples",
											"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt.");

			break;
		case FP_ELEMENT_SAMPLING :  
			parlst.addEnum("Sampling", 0, 
									QStringList() << "VertexSampling" << "Edge Sampling" << "Face Sampling", 
									tr("Element to sample:"), 
									tr("Choose what mesh element has to be used for the sampling. A point sample will be added for each one of the chosen elements")); 		
			break;
		case FP_TEXEL_SAMPLING :  
 		  parlst.addInt ("TextureSize",
											256,
											"Texture Size",
											"A sample for each texel is generated, so the desired texture size is need, only samples for the texels falling inside some faces are generated.\n Setting this param to 256 means that you get at most 256x256 = 65536 samples)");
			break;
		case FP_HAUSDORFF_DISTANCE :  
			{
				MeshModel *target= md.mm();
				foreach (target, md.meshList) 
						if (target != md.mm())  break;
		 
			parlst.addMesh ("BaseMesh",
											md.mm(),
											"Base Mesh",
											"The mesh that is used to be compared to.");
			parlst.addMesh ("TargetMesh",
											target,
											"Target Mesh",
											"The mesh that is sampled for the comparison.");
			
		parlst.addBool ("SaveSample",
											false,
											"Save Samples",
											"Save the position and distance of all the used samples on both the two surfaces, creating two new layers with point clouds representing the used samples.");
											
		parlst.addBool ("Symmetric",
											target,
											"Symmetric",
											"Perform the test in both ways (target to base and base to target).");
			
			parlst.addInt ("SampleNum",
											md.mm()->cm.vn,
											"Number of samples",
											"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt.");
											
			parlst.addBool("Weighted",
										 false,
										 "Quality Weighted Sampling",
										 "Use per vertex quality to drive the vertex sampling. The number of samples falling in each face is proportional to the face area multiplied by the average quality of the face vertices.");
											break;
			}
		default : assert(0); 
	}
}

class MyPointSampler
{
	public:
	MyPointSampler(CMeshO* _m){m=_m;};
	CMeshO *m;
	void AddVert(const CMeshO::VertexType &p) 
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		m->vert.back().ImportLocal(p);
	}
	void AddFace(const CMeshO::FaceType &f, CMeshO::CoordType p) 
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		m->vert.back().P() = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
		m->vert.back().N() = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] +f.V(2)->N()*p[2];
	}
}; // end class MyPointSampler

class HausdorffSampler
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
public:
  
	HausdorffSampler(CMeshO* _m=0,CMeshO* _sampleMesh=0, CMeshO* _closestMesh=0 )
	{
		init(_m,_sampleMesh,_closestMesh);
	};

	CMeshO *m;           /// the mesh for which we search the closest points. 
	CMeshO *samplePtMesh;  /// the mesh containing the sample points
	CMeshO *closestPtMesh; /// the mesh containing the corresponding closest points that have been found
	
	MetroMeshGrid   unifGrid;

	// Parameters
		double          max_dist;
    double          mean_dist;
    double          RMS_dist;   /// from the wikipedia defintion RMS DIST is sqrt(Sum(distances^2)/n), here we store Sum(distances^2)
    double          volume;
    double          area_S1;
		Histogramf hist;
    // globals
    int             n_total_samples;
    int             n_samples;
		float dist_upper_bound;
		typedef trimesh::FaceTmark<CMeshO> MarkerFace;
		MarkerFace markerFunctor;
	
		
		float getMeanDist() const { return mean_dist / n_total_samples; }
		float getMaxDist() const { return max_dist ; }
		float getRMSDist() const { return sqrt(RMS_dist / n_total_samples); }
	
 	void init(CMeshO *_m,CMeshO* _sampleMesh=0, CMeshO* _closestMesh=0 )
	{
		m=_m;
		samplePtMesh =_sampleMesh;
		closestPtMesh = _closestMesh;
		if(m) 
		{
			unifGrid.Set(m->face.begin(),m->face.end());
			markerFunctor.SetMesh(m);
			hist.SetRange(0.0, m->bbox.Diag()/100.0, 100);
		}
		max_dist = 0;
		mean_dist =0;
		RMS_dist = 0;  
		n_total_samples = 0;		
	}

void AddFace(const CMeshO::FaceType &f, CMeshO::CoordType interp) 
{
	Point3f startPt = f.P(0)*interp[0] + f.P(1)*interp[1] +f.P(2)*interp[2]; // point to be sampled
	AddSample(startPt);
}

void AddVert(const CMeshO::VertexType &p) 
{
	AddSample(p.cP());
}


void AddSample(const CMeshO::CoordType &startPt) 
{	
		// the results
    Point3f       closestPt,      normf, bestq, ip;
		float dist = dist_upper_bound;

    // compute distance between startPt and the mesh S2
		CMeshO::FaceType   *nearestF=0;
		vcg::face::PointDistanceBaseFunctor PDistFunct;
		dist=dist_upper_bound;
		nearestF =  unifGrid.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);

    // update distance measures
    if(dist == dist_upper_bound)
        return ;

    if(dist > max_dist)
        max_dist = dist;        // L_inf
		
		mean_dist += dist;	        // L_1
    RMS_dist  += dist*dist;     // L_2
    n_total_samples++;

		hist.Add((float)fabs(dist));
		if(samplePtMesh)
		{
			tri::Allocator<CMeshO>::AddVertices(*samplePtMesh,1);
			samplePtMesh->vert.back().P() = startPt;
			samplePtMesh->vert.back().Q() = dist;
		}
		if(closestPtMesh)
		{
			tri::Allocator<CMeshO>::AddVertices(*closestPtMesh,1);
			closestPtMesh->vert.back().P() = closestPt;
			closestPtMesh->vert.back().Q() = dist;
		}
		
}
}; // end class HausdorffSampler


bool SampleFilterDocPlugin::applyFilter(QAction *action, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(action))
		{
		case FP_ELEMENT_SAMPLING :  
		{
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Sampled Mesh"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			MyPointSampler mps(&(mm->cm));
			
			switch(par.getEnum("Sampling"))
				{
					case 0 :	tri::SurfaceSampling<CMeshO,MyPointSampler>::AllVertex(curMM->cm,mps);	break;
					case 1 :	tri::SurfaceSampling<CMeshO,MyPointSampler>::AllEdge(curMM->cm,mps);		break;
					case 2 :	tri::SurfaceSampling<CMeshO,MyPointSampler>::AllFace(curMM->cm,mps);		break;
				}
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);		
		}
		break;
		case FP_TEXEL_SAMPLING :  
				{
					MeshModel *curMM= md.mm();				
					MeshModel *mm= md.addNewMesh("Sampled Mesh"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
					
					vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
				}
		break;
		case FP_MONTECARLO_SAMPLING :  
		{
			
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Montecarlo Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			
			MyPointSampler mps(&(mm->cm));
			if(par.getBool("Weighted")) 
					   tri::SurfaceSampling<CMeshO,MyPointSampler>::WeightedMontecarlo(curMM->cm,mps,par.getInt("SampleNum"));
			else tri::SurfaceSampling<CMeshO,MyPointSampler>::Montecarlo(curMM->cm,mps,par.getInt("SampleNum"));
			
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);						
		}
			break;
		case FP_SUBDIV_SAMPLING :  
		{
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Subdiv Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			
			MyPointSampler mps(&(mm->cm));
			tri::SurfaceSampling<CMeshO,MyPointSampler>::FaceSubdivision(curMM->cm,mps,par.getInt("SampleNum"));
			
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);						
		}
			break;
		case FP_SIMILAR_SAMPLING :  
		{
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Similar Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			
			MyPointSampler mps(&(mm->cm));
			tri::SurfaceSampling<CMeshO,MyPointSampler>::FaceSimilar(curMM->cm,mps,par.getInt("SampleNum"));
			
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);						
		}
			break;
			
		case FP_HAUSDORFF_DISTANCE : 
			{
			MeshModel* mm0 = par.getMesh("BaseMesh");  // this is sampled mesh 
			MeshModel* mm1 = par.getMesh("TargetMesh"); // this whose surface is sought for the closest point to each sample. 
			bool saveSampleFlag=par.getBool("SaveSample");
			bool vertexSamplingFlag;
			bool edgeSamplingFlag;
			bool faceSamplingFlag;

			
			mm1->updateDataMask(MeshModel::MM_FACEMARK);
			tri::UpdateNormals<CMeshO>::PerFaceNormalized(mm1->cm);
			tri::UpdateFlags<CMeshO>::FaceProjection(mm1->cm);

			MeshModel *samplePtMesh =0; 
			MeshModel *closestPtMesh =0; 
		  HausdorffSampler hs;
			if(saveSampleFlag)
				{
					closestPtMesh=md.addNewMesh("Hausdorff Closest Points"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
					samplePtMesh=md.addNewMesh("Hausdorff Sample Point"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
					hs.init(&(mm1->cm),&(samplePtMesh->cm),&(closestPtMesh->cm));
				}
			else hs.init(&(mm1->cm));
				
			hs.dist_upper_bound = mm1->cm.bbox.Diag()/10;
			
			qDebug("Sampled  mesh has %7i vert %7i face",mm0->cm.vn,mm0->cm.fn);
			qDebug("Searched mesh has %7i vert %7i face",mm1->cm.vn,mm1->cm.fn);

			tri::SurfaceSampling<CMeshO,HausdorffSampler>::VertexUniform(mm0->cm,hs,par.getInt("SampleNum"));
			//tri::SurfaceSampling<CMeshO,HausdorffSampler>::Montecarlo(mm0->cm,hs,par.getInt("SampleNum"));
			//tri::SurfaceSampling<CMeshO,HausdorffSampler>::Montecarlo(mm0->cm,hs,par.getInt("SampleNum"));
				
			Log(0,"Hausdorf Distance computed");						
			Log(0,"      max : %f   Sample %i",hs.getMaxDist(),hs.n_total_samples);						
			Log(0,"     mean : %f   RMS : %f",hs.getMeanDist(),hs.getRMSDist());						
			qDebug("Hausdorf Distance computed");						
			qDebug("      max : %f   Sample %i",hs.getMaxDist(),hs.n_total_samples);						
			qDebug("     mean : %f   RMS : %f",hs.getMeanDist(),hs.getRMSDist());						
			
			if(saveSampleFlag)
				{
					tri::UpdateBounding<CMeshO>::Box(samplePtMesh->cm);
					tri::UpdateBounding<CMeshO>::Box(closestPtMesh->cm);
				}
			}
			break;
		default : assert(0);
		}
		
	return true;
}

Q_EXPORT_PLUGIN(SampleFilterDocPlugin)
