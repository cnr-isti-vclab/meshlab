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
#include <vcg/complex/trimesh/create/resampler.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/space/index/grid_static_ptr.h>
#include "filter_sampling.h"

using namespace vcg;
using namespace std;

/* 
The Samplers; these classes are used to provide the callback used in the sampling process. 
*/ 


class BaseSampler
{
	public:
	BaseSampler(CMeshO* _m){m=_m; uvSpaceFlag = false;};
	CMeshO *m;
	bool uvSpaceFlag;
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
	
	void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const Point2i &tp)
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);

		if(uvSpaceFlag) m->vert.back().P() = Point3f(float(tp[0]),float(tp[1]),0); 
							 else m->vert.back().P() = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
		m->vert.back().N() = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] +f.V(2)->N()*p[2];
	}
}; // end class BaseSampler


/* This sampler is used to perform compute the Hausdorf measuring.
 * It keep internally the spatial inedexing structure used to find the closest point 
 * and the partial integration results needed to compute the average and rms error values.
 */
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
		vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
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


/* This sampler is used to transfer the detail of a mesh onto another one. 
 * the 
 * It keep internally the spatial inedexing structure used to find the closest point 
 * 
 */
class RedetailSampler
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
public:
  
	RedetailSampler()
	{
		m=0;		
	};
 
	CMeshO *m;           /// the source mesh for which we search the closest points (e.g. the mesh from which we take colors etc). 
	CallBackPos *cb;
	int sampleNum;  // the expected number of samples. Used only for the callback
	int sampleCnt;
	MetroMeshGrid   unifGrid;

	// Parameters
		typedef trimesh::FaceTmark<CMeshO> MarkerFace;
		MarkerFace markerFunctor;
	
	bool coordFlag;
	bool colorFlag;
	bool qualityFlag;
	float dist_upper_bound;
 	void init(CMeshO *_m, CallBackPos *_cb=0, int targetSz=0)
	{
		coordFlag=false;
		colorFlag=false;
		qualityFlag=false;
		m=_m;
		if(m) 
		{
			unifGrid.Set(m->face.begin(),m->face.end());
			markerFunctor.SetMesh(m);
		}
		// sampleNum and sampleCnt are used only for the progress callback.
		cb=_cb;
		sampleNum = targetSz; 
		sampleCnt=0;
	}
// this function is called for each vertex of the target mesh.
// and retrieve the closest point on the source mesh.
void AddVert(CMeshO::VertexType &p) 
{
		assert(m);
		// the results
    Point3f       closestPt,      normf, bestq, ip;
		float dist = dist_upper_bound;
		const CMeshO::CoordType &startPt= p.cP();
    // compute distance between startPt and the mesh S2
		CMeshO::FaceType   *nearestF=0;
		vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
		dist=dist_upper_bound;
	  if(cb) cb(sampleCnt++*100/sampleNum,"Resampling Vertex attributes");
		nearestF =  unifGrid.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);
    if(dist == dist_upper_bound) return ;																				

		Point3f interp;
		bool ret = InterpolationParameters(*nearestF,closestPt, interp[0], interp[1], interp[2]);
    assert(ret);
		interp[2]=1.0-interp[1]-interp[0];
																			 
		if(coordFlag) p.P()=closestPt;
		if(colorFlag) p.C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),nearestF->V(2)->C(),interp);
		if(qualityFlag) p.Q()= nearestF->V(0)->Q()*interp[0] + nearestF->V(1)->Q()*interp[1] + nearestF->V(2)->Q()*interp[2];
		}
}; // end class RedetailSampler





// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterDocSampling::FilterDocSampling() 
{ 
	typeList 
			<< FP_ELEMENT_SAMPLING 
			<< FP_MONTECARLO_SAMPLING
			<< FP_SIMILAR_SAMPLING
			<< FP_SUBDIV_SAMPLING
			<< FP_HAUSDORFF_DISTANCE
			<< FP_TEXEL_SAMPLING
			<< FP_VERTEX_RESAMPLING
			<< FP_OFFSET_SURFACE
	;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString FilterDocSampling::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_ELEMENT_SAMPLING    :  return QString("Mesh Element Sampling"); 
		case FP_MONTECARLO_SAMPLING :  return QString("Montecarlo Sampling"); 
		case FP_SIMILAR_SAMPLING :  return QString("Similar Triangle Sampling"); 
		case FP_SUBDIV_SAMPLING :  return QString("Regular Sudiv. Sampling"); 
		case FP_HAUSDORFF_DISTANCE  :  return QString("Hausdorff Distance"); 
		case FP_TEXEL_SAMPLING  :  return QString("Texel Sampling"); 
		case FP_VERTEX_RESAMPLING  :  return QString("Vertex Attribute Transfer"); 
		case FP_OFFSET_SURFACE  :  return QString("Uniform Mesh Resampling"); 
		
		default : assert(0); return QString("unknown filter!!!!");
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString FilterDocSampling::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_ELEMENT_SAMPLING    :  return QString("Create a new layer populated with a point sampling of the current mesh, a sample for each element of the mesh is generated"); 
		case FP_MONTECARLO_SAMPLING :  return QString("Create a new layer populated with a point sampling of the current mesh; samples are generated in a randomly uniform way, or with a distribution biased by the per-vertex quality values of the mesh."); 
		case FP_SIMILAR_SAMPLING		:  return QString("Create a new layer populated with a point sampling of the current mesh; to generate multiple samples inside a triangle it is subdivided into similar triangles and the internal vertices of these triangles are considered. Distribution is biased by the shape of the triangles."); 
		case FP_SUBDIV_SAMPLING			:  return QString("Create a new layer populated with a point sampling of the current mesh; to generate multiple samples inside a triangle"); 
		case FP_HAUSDORFF_DISTANCE  :  return QString("Compute the Hausdorff Distance between two meshes, sampling one of the two and finding foreach sample the closest point over the other mesh."); 
		case FP_TEXEL_SAMPLING      :  return QString("Create a new layer with a point sampling of the current mesh, a sample for each texel of the mesh is generated"); 
		case FP_VERTEX_RESAMPLING		:  return QString("Transfer the choosen per-vertex attributes from one mesh to another. Useful to transfer attributes to different representations of a same object.<br>"
																									"For each vertex of the target mesh the closest point (not vertex!) on the source mesh is computed, and the requested interpolated attributes from that source point are copied into the target vertex.<br>"
																									"The algorithm assumes that the two meshes are reasonably similar and aligned."); 
		case FP_OFFSET_SURFACE			:  return QString("Create a new mesh that is a resampled version of the current one.<br>"
																									"The resampling is done by building a uniform volumetric representation where each voxel contains the signed distance from the original surface. "
																									"The resampled surface is reconstructed using the <b>marching cube</b> algorithm over this volume. "); 
			
		default : assert(0); return QString("unknown filter!!!!");

	}
}
const int FilterDocSampling::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_VERTEX_RESAMPLING :
		case FP_OFFSET_SURFACE:
    case FP_HAUSDORFF_DISTANCE :	return  MeshModel::MM_FACEMARK;
		case FP_ELEMENT_SAMPLING    :   
		case FP_MONTECARLO_SAMPLING :    
		case FP_SIMILAR_SAMPLING :   
		case FP_SUBDIV_SAMPLING :   
		case FP_TEXEL_SAMPLING  :  return 0;

    default: assert(0);
  }
  return 0;
}

const PluginInfo &FilterDocSampling::pluginInfo()
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
void FilterDocSampling::initParameterSet(QAction *action, MeshDocument & md, FilterParameterSet & parlst) 
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
 		  parlst.addInt (	"TextureSize", 256, "Texture Size",
											"A sample for each texel is generated, so the desired texture size is need, only samples for the texels falling inside some faces are generated.\n Setting this param to 256 means that you get at most 256x256 = 65536 samples)");
 		  parlst.addBool(	"TextureSpace", false, "UV Space Sampling",
											"The generated texel samples have their UV coords as point positions. The resulting point set is has a square domain, the texels/points, even if on a flat domain retain the original vertex normal to help a better perception of the original provenience.");
			break;
		case FP_HAUSDORFF_DISTANCE :  
			{
				MeshModel *target= md.mm();
				foreach (target, md.meshList) 
						if (target != md.mm())  break;
		    
				parlst.addMesh ("BaseMesh", md.mm(), "Base Mesh",
												"The mesh that is used to be compared to.");
				parlst.addMesh ("TargetMesh", target, "Target Mesh",
												"The mesh that is sampled for the comparison.");
				parlst.addBool ("SaveSample", false, "Save Samples",
												"Save the position and distance of all the used samples on both the two surfaces, creating two new layers with point clouds representing the used samples.");										
				parlst.addBool ("Symmetric", target, "Symmetric",
												"Perform the test in both ways (target to base and base to target).");
				parlst.addInt ("SampleNum", md.mm()->cm.vn, "Number of samples",
												"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt.");
				parlst.addBool("Weighted", false, "Quality Weighted Sampling",
										 "Use per vertex quality to drive the vertex sampling. The number of samples falling in each face is proportional to the face area multiplied by the average quality of the face vertices.");
			} break;
		case FP_VERTEX_RESAMPLING:
		{
				MeshModel *target= md.mm();
				foreach (target, md.meshList) 
						if (target != md.mm())  break;
						
				parlst.addMesh ("SourceMesh", md.mm(), "Source Mesh",
												"The mesh that contains the source data that we want to transfer.");
				parlst.addMesh ("TargetMesh", target, "Target Mesh",
												"The mesh whose vertexes will receive the data from the source.");
				parlst.addBool ("GeomTransfer", false, "Transfer Geometry",
												"if enabled, the position of each vertex of the target mesh will be snapped onto the corresponding closest point on the source mesh");										
				parlst.addBool ("ColorTransfer", true, "Transfer Color",
												"if enabled, the color of each vertex of the target mesh will become the color of the corresponding closest point on the source mesh");										
				parlst.addBool ("QualityTransfer", false, "Transfer quality",
												"if enabled, the quality of each vertex of the target mesh will become the quality of the corresponding closest point on the source mesh");										
		} break; 
		case FP_OFFSET_SURFACE :
		{
				
			parlst.addAbsPerc("CellSize", md.mm()->cm.bbox.Diag()/50.0, 0.0f, md.mm()->cm.bbox.Diag(),
				tr("Precision"), tr("Size of the cell, the default is 1/50 of the box diag. Smaller cells give better precision at a higher computational cost. Remember that halving the cell size means that you build a volume 8 times larger."));
			
			parlst.addAbsPerc("Offset", 0.0, -md.mm()->cm.bbox.Diag()/5.0f, md.mm()->cm.bbox.Diag()/5.0f,
												tr("Offset"), tr("Offset of the created surface (i.e. distance of the created surface from the original one).<br>"
																				 "If offset is zero, the created surface passes on the original mesh itself. "
																				 "Values greater than zero mean an external surface, and lower than zero mean an internal surface.<br> "
																				 "In practice this value is the threshold passed to the Marching Cube algorithm to extract the isosurface from the distance field representation."));
			
			
		} break; 

		default : assert(0); 
	}
}

bool FilterDocSampling::applyFilter(QAction *action, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(action))
		{
		case FP_ELEMENT_SAMPLING :  
		{
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Sampled Mesh"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			BaseSampler mps(&(mm->cm));
			
			switch(par.getEnum("Sampling"))
				{
					case 0 :	tri::SurfaceSampling<CMeshO,BaseSampler>::AllVertex(curMM->cm,mps);	break;
					case 1 :	tri::SurfaceSampling<CMeshO,BaseSampler>::AllEdge(curMM->cm,mps);		break;
					case 2 :	tri::SurfaceSampling<CMeshO,BaseSampler>::AllFace(curMM->cm,mps);		break;
				}
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);		
		}
		break;
		case FP_TEXEL_SAMPLING :  
				{
					MeshModel *curMM= md.mm();				
					MeshModel *mm= md.addNewMesh("Sampled Mesh"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
					BaseSampler mps(&(mm->cm));
					mps.uvSpaceFlag = par.getBool("TextureSpace");
					tri::SurfaceSampling<CMeshO,BaseSampler>::Texture(curMM->cm,mps,par.getInt("TextureSize"));
					vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
				}
		break;
		case FP_MONTECARLO_SAMPLING :  
		{
			
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Montecarlo Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			
			BaseSampler mps(&(mm->cm));
			if(par.getBool("Weighted")) 
					   tri::SurfaceSampling<CMeshO,BaseSampler>::WeightedMontecarlo(curMM->cm,mps,par.getInt("SampleNum"));
			else tri::SurfaceSampling<CMeshO,BaseSampler>::Montecarlo(curMM->cm,mps,par.getInt("SampleNum"));
			
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);						
		}
			break;
		case FP_SUBDIV_SAMPLING :  
		{
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Subdiv Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			
			BaseSampler mps(&(mm->cm));
			tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSubdivision(curMM->cm,mps,par.getInt("SampleNum"));
			
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);						
		}
			break;
		case FP_SIMILAR_SAMPLING :  
		{
			MeshModel *curMM= md.mm();				
			MeshModel *mm= md.addNewMesh("Similar Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
			
			BaseSampler mps(&(mm->cm));
			tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSimilar(curMM->cm,mps,par.getInt("SampleNum"));
			
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
		case	 FP_VERTEX_RESAMPLING :
		{
			MeshModel* srcMesh = par.getMesh("SourceMesh"); // mesh whose attribute are read
			MeshModel* trgMesh = par.getMesh("TargetMesh"); // this whose surface is sought for the closest point to each sample. 
			
			srcMesh->updateDataMask(MeshModel::MM_FACEMARK);
			tri::UpdateNormals<CMeshO>::PerFaceNormalized(srcMesh->cm);
			tri::UpdateFlags<CMeshO>::FaceProjection(srcMesh->cm);

		  RedetailSampler rs;
			rs.init(&(srcMesh->cm),cb,trgMesh->cm.vn);
				
			rs.dist_upper_bound = trgMesh->cm.bbox.Diag()/50;
			rs.colorFlag=par.getBool("ColorTransfer");
			rs.coordFlag=par.getBool("GeomTransfer");
			rs.qualityFlag=par.getBool("QualityTransfer");

			if(!rs.colorFlag && !rs.coordFlag && !rs.qualityFlag)
			{
				errorMessage = QString("You have to choose at least one attribute to be sampled");
				return false;
			}
			
			if(rs.colorFlag) {
					trgMesh->updateDataMask(MeshModel::MM_VERTCOLOR);
					trgMesh->ioMask|=MeshModel::IOM_VERTCOLOR;
			}
			
			if(rs.qualityFlag){
					trgMesh->updateDataMask(MeshModel::MM_VERTQUALITY);
					trgMesh->ioMask|=MeshModel::IOM_VERTQUALITY;
			}

			qDebug("Source  mesh has %7i vert %7i face",srcMesh->cm.vn,srcMesh->cm.fn);
			qDebug("Target  mesh has %7i vert %7i face",trgMesh->cm.vn,trgMesh->cm.fn);

			tri::SurfaceSampling<CMeshO,RedetailSampler>::VertexUniform(trgMesh->cm,rs,trgMesh->cm.vn);
			
			if(rs.coordFlag) tri::UpdateNormals<CMeshO>::PerFaceNormalized(trgMesh->cm);
			
		} break;
		case FP_OFFSET_SURFACE :
		{
			float voxelSize = par.getAbsPerc("CellSize");
			float offsetThr = par.getAbsPerc("Offset");
			
			MeshModel *baseMesh= md.mm();				
			MeshModel *offsetMesh =md.addNewMesh("Offset mesh"); 			
			baseMesh->updateDataMask(MeshModel::MM_FACEMARK);	
			
			Point3i volumeDim;
			BestDim( baseMesh->cm.bbox, voxelSize, volumeDim );

			tri::Resampler<CMeshO,CMeshO,float>::Resample(baseMesh->cm, offsetMesh->cm, volumeDim, voxelSize*2.5, offsetThr,cb);
			tri::UpdateBounding<CMeshO>::Box(offsetMesh->cm);
			tri::UpdateNormals<CMeshO>::PerVertexPerFace(offsetMesh->cm);
		} break;
		default : assert(0);
		}
	return true;
}

Q_EXPORT_PLUGIN(FilterDocSampling)
