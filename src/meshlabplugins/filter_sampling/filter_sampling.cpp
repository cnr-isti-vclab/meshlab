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
#include <limits>

#include "filter_sampling.h"

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/resampler.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/algorithms/geodesic.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/algorithms/voronoi_clustering.h>

using namespace vcg;
using namespace std;

/* 
  Sampler Classes (read carefully the following note).

  Sampler Classes and Sampling algorithms are independent. 
  Sampler classes exploits the sample that are generated with various algorithms.
  For example, you can compute Hausdorff distance (that is a sampler) using various 
  sampling strategies (montecarlo, stratified etc).

  For further details see vcg/complex/algorithms/point_sampling.h header file.
*/ 


class BaseSampler
{
	public:
	BaseSampler(CMeshO* _m){m=_m; uvSpaceFlag = false; qualitySampling=false; tex=0;};
	CMeshO *m;
	QImage* tex;
	int texSamplingWidth;
	int texSamplingHeight;
	bool uvSpaceFlag;
	bool qualitySampling;
	
	void AddVert(const CMeshO::VertexType &p) 
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		m->vert.back().ImportData(p);
	}
	
	void AddFace(const CMeshO::FaceType &f, CMeshO::CoordType p) 
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		m->vert.back().P() = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
		m->vert.back().N() = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] + f.V(2)->N()*p[2];

		if (qualitySampling)	
      m->vert.back().Q() = f.V(0)->Q()*p[0] + f.V(1)->Q()*p[1] + f.V(2)->Q()*p[2];
	}
        void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const Point2i &tp, float edgeDist)
	{
                if (edgeDist != .0) return;

		tri::Allocator<CMeshO>::AddVertices(*m,1);

		if(uvSpaceFlag) m->vert.back().P() = Point3f(float(tp[0]),float(tp[1]),0); 
							 else m->vert.back().P() = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
							 
		m->vert.back().N() = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] +f.V(2)->N()*p[2];
		if(tex)
		{
			QRgb val;
                        // Computing normalized texels position
                        int xpos = (int)(tex->width()  * (float(tp[0])/texSamplingWidth)) % tex->width();
                        int ypos = (int)(tex->height() * (1.0- float(tp[1])/texSamplingHeight)) % tex->height();

                        if (xpos < 0) xpos += tex->width();
                        if (ypos < 0) ypos += tex->height();

			val = tex->pixel(xpos,ypos);
			m->vert.back().C().SetRGB(qRed(val),qGreen(val),qBlue(val));
		}
		
	}
}; // end class BaseSampler


/* This sampler is used to perform compute the Hausdorff measuring.
 * It keep internally the spatial indexing structure used to find the closest point 
 * and the partial integration results needed to compute the average and rms error values.
 * Averaged values assume that the samples are equi-distributed (e.g. a good unbiased montecarlo sampling of the surface).
 */
class HausdorffSampler
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
	typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > MetroMeshVertexGrid;
public:
  
  HausdorffSampler(CMeshO* _m=0,CMeshO* _sampleMesh=0, CMeshO* _closestMesh=0 ) :markerFunctor(_m)
	{
		init(_m,_sampleMesh,_closestMesh);
	};

	CMeshO *m;           /// the mesh for which we search the closest points. 
	CMeshO *samplePtMesh;  /// the mesh containing the sample points
	CMeshO *closestPtMesh; /// the mesh containing the corresponding closest points that have been found
	
  MetroMeshVertexGrid   unifGridVert;
  MetroMeshFaceGrid   unifGridFace;

	// Parameters
		double          min_dist;
		double          max_dist;
    double          mean_dist;
    double          RMS_dist;   /// from the wikipedia defintion RMS DIST is sqrt(Sum(distances^2)/n), here we store Sum(distances^2)
    double          volume;
    double          area_S1;
		Histogramf hist;
    // globals parameters driving the samples. 
    int             n_total_samples;
    int             n_samples;
    bool useVertexSampling;
		float dist_upper_bound;  // samples that have a distance beyond this threshold distance are not considered. 
		typedef tri::FaceTmark<CMeshO> MarkerFace;
		MarkerFace markerFunctor;
	
		
		float getMeanDist() const { return mean_dist / n_total_samples; }
		float getMinDist() const { return min_dist ; }
		float getMaxDist() const { return max_dist ; }
		float getRMSDist() const { return sqrt(RMS_dist / n_total_samples); }
	
 	void init(CMeshO *_m,CMeshO* _sampleMesh=0, CMeshO* _closestMesh=0 )
	{
		m=_m;
		samplePtMesh =_sampleMesh;
		closestPtMesh = _closestMesh;
		if(m) 
		{
			tri::UpdateNormals<CMeshO>::PerFaceNormalized(*m);
      if(m->fn==0) useVertexSampling = true;
              else useVertexSampling = false;

      if(useVertexSampling) unifGridVert.Set(m->vert.begin(),m->vert.end());
                      else  unifGridFace.Set(m->face.begin(),m->face.end());
      markerFunctor.SetMesh(m);
			hist.SetRange(0.0, m->bbox.Diag()/100.0, 100);
		}
		min_dist = std::numeric_limits<double>::max();
		max_dist = 0;
		mean_dist =0;
		RMS_dist = 0;
		n_total_samples = 0;
	}

void AddFace(const CMeshO::FaceType &f, CMeshO::CoordType interp) 
{
	Point3f startPt = f.P(0)*interp[0] + f.P(1)*interp[1] +f.P(2)*interp[2]; // point to be sampled
	Point3f startN  = f.V(0)->cN()*interp[0] + f.V(1)->cN()*interp[1] +f.V(2)->cN()*interp[2]; // Normal of the interpolated point
	AddSample(startPt,startN); // point to be sampled);
}

void AddVert(CMeshO::VertexType &p) 
{
	p.Q()=AddSample(p.cP(),p.cN());
}


float AddSample(const CMeshO::CoordType &startPt,const CMeshO::CoordType &startN) 
{	
		// the results
    Point3f       closestPt,      normf, bestq, ip;
		float dist = dist_upper_bound;

    // compute distance between startPt and the mesh S2
		CMeshO::FaceType   *nearestF=0;
    CMeshO::VertexType   *nearestV=0;
    vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
		dist=dist_upper_bound;
    if(useVertexSampling)
      nearestV =  tri::GetClosestVertex<CMeshO,MetroMeshVertexGrid>(*m,unifGridVert,startPt,dist_upper_bound,dist);
      else
       nearestF =  unifGridFace.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);

    // update distance measures
    if(dist == dist_upper_bound)
return dist;

    if(dist > max_dist) max_dist = dist;        // L_inf
    if(dist < min_dist) min_dist = dist;        // L_inf
		
		mean_dist += dist;	        // L_1
    RMS_dist  += dist*dist;     // L_2
    n_total_samples++;

		hist.Add((float)fabs(dist));
		if(samplePtMesh)
		{
			tri::Allocator<CMeshO>::AddVertices(*samplePtMesh,1);
			samplePtMesh->vert.back().P() = startPt;
			samplePtMesh->vert.back().Q() = dist;
			samplePtMesh->vert.back().N() = startN;
		}
		if(closestPtMesh)
		{
			tri::Allocator<CMeshO>::AddVertices(*closestPtMesh,1);
			closestPtMesh->vert.back().P() = closestPt;
			closestPtMesh->vert.back().Q() = dist;
			closestPtMesh->vert.back().N() = startN;
		}
		return dist;
}
}; // end class HausdorffSampler


/* This sampler is used to transfer the detail of a mesh onto another one. 
 * It keep internally the spatial indexing structure used to find the closest point 
 */
class RedetailSampler
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
	typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > VertexMeshGrid;

public:

	RedetailSampler():markerFunctor(0)
	{
		m=0;		
	};
 
	CMeshO *m;           /// the source mesh for which we search the closest points (e.g. the mesh from which we take colors etc). 
	CallBackPos *cb;
	int sampleNum;  // the expected number of samples. Used only for the callback
	int sampleCnt;
	MetroMeshGrid   unifGridFace;
	VertexMeshGrid   unifGridVert;
	bool useVertexSampling;

	// Parameters
		typedef tri::FaceTmark<CMeshO> MarkerFace;
		MarkerFace markerFunctor;
	
	bool coordFlag;
	bool colorFlag;
	bool normalFlag;
	bool qualityFlag;
  bool selectionFlag;
	bool storeDistanceAsQualityFlag;
	float dist_upper_bound;
 	void init(CMeshO *_m, CallBackPos *_cb=0, int targetSz=0)
	{
		coordFlag=false;
		colorFlag=false;
		qualityFlag=false;
    selectionFlag=false;
		storeDistanceAsQualityFlag=false;
		m=_m;
		if(m) 
		{
			tri::UpdateNormals<CMeshO>::PerFaceNormalized(*m);
			if(m->fn==0) useVertexSampling = true;
							else useVertexSampling = false;
							
			if(useVertexSampling) unifGridVert.Set(m->vert.begin(),m->vert.end());
											else  unifGridFace.Set(m->face.begin(),m->face.end());
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
		if(useVertexSampling)
			{
				CMeshO::VertexType   *nearestV=0;
				nearestV =  tri::GetClosestVertex<CMeshO,VertexMeshGrid>(*m,unifGridVert,startPt,dist_upper_bound,dist); //(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);
			if(cb) cb(sampleCnt++*100/sampleNum,"Resampling Vertex attributes");
				if(storeDistanceAsQualityFlag)  p.Q() = dist;
				if(dist == dist_upper_bound) return ;		
				
				if(coordFlag) p.P()=nearestV->P();
				if(colorFlag) p.C() = nearestV->C();
				if(normalFlag) p.N() = nearestV->N();
				if(qualityFlag) p.Q()= nearestV->Q();
        if(selectionFlag) if(nearestV->IsS()) p.SetS();
			}
		else
			{
				CMeshO::FaceType   *nearestF=0;
				vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
				dist=dist_upper_bound;
				if(cb) cb(sampleCnt++*100/sampleNum,"Resampling Vertex attributes");
				nearestF =  unifGridFace.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);
				if(dist == dist_upper_bound) return ;																				

				Point3f interp;
        InterpolationParameters(*nearestF,(*nearestF).cN(),closestPt, interp);
        interp[2]=1.0-interp[1]-interp[0];

				if(coordFlag) p.P()=closestPt;
				if(colorFlag) p.C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),nearestF->V(2)->C(),interp);
				if(normalFlag) p.N() = nearestF->V(0)->N()*interp[0] + nearestF->V(1)->N()*interp[1] + nearestF->V(2)->N()*interp[2];
				if(qualityFlag) p.Q()= nearestF->V(0)->Q()*interp[0] + nearestF->V(1)->Q()*interp[1] + nearestF->V(2)->Q()*interp[2];
        if(selectionFlag) if(nearestF->IsS()) p.SetS();
			}
	}
}; // end class RedetailSampler


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterDocSampling::FilterDocSampling() 
{ 
	typeList 
			<< FP_ELEMENT_SUBSAMPLING 
			<< FP_MONTECARLO_SAMPLING
			<< FP_STRATIFIED_SAMPLING
			<< FP_CLUSTERED_SAMPLING
			<< FP_POISSONDISK_SAMPLING
			<< FP_VARIABLEDISK_SAMPLING
			<< FP_HAUSDORFF_DISTANCE
			<< FP_TEXEL_SAMPLING
			<< FP_VERTEX_RESAMPLING
			<< FP_UNIFORM_MESH_RESAMPLING
			<< FP_VORONOI_CLUSTERING
			<< FP_VORONOI_COLORING
			<< FP_DISK_COLORING
			<< FP_REGULAR_RECURSIVE_SAMPLING
	;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString FilterDocSampling::filterName(FilterIDType filterId) const 
{
  switch(filterId) {
		case FP_ELEMENT_SUBSAMPLING    :  return QString("Mesh Element Subsampling"); 
		case FP_MONTECARLO_SAMPLING :  return QString("Montecarlo Sampling"); 
		case FP_STRATIFIED_SAMPLING :  return QString("Stratified Triangle Sampling"); 
		case FP_CLUSTERED_SAMPLING :  return QString("Clustered vertex Subsampling"); 
		case FP_POISSONDISK_SAMPLING : return QString("Poisson-disk Sampling");
		case FP_VARIABLEDISK_SAMPLING : return QString("Variable density Disk Sampling");
		case FP_HAUSDORFF_DISTANCE  :  return QString("Hausdorff Distance"); 
		case FP_TEXEL_SAMPLING  :  return QString("Texel Sampling"); 
		case FP_VERTEX_RESAMPLING  :  return QString("Vertex Attribute Transfer"); 
		case FP_UNIFORM_MESH_RESAMPLING  :  return QString("Uniform Mesh Resampling"); 
		case FP_VORONOI_CLUSTERING  :  return QString("Voronoi Vertex Clustering"); 
		case FP_VORONOI_COLORING  :  return QString("Voronoi Vertex Coloring"); 
		case FP_DISK_COLORING  :  return QString("Disk Vertex Coloring"); 
		case FP_REGULAR_RECURSIVE_SAMPLING  :  return QString("Regular Recursive Sampling"); 
			
		default : assert(0); return QString("unknown filter!!!!");
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterDocSampling::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_ELEMENT_SUBSAMPLING  :  return QString("Create a new layer populated with a point sampling of the current mesh, At most one sample for each element of the mesh is created. Samples are taking in a uniform way, one for each element (vertex/edge/face); all the elements have the same probabilty of being choosen."); 
		case FP_MONTECARLO_SAMPLING  :  return QString("Create a new layer populated with a point sampling of the current mesh; samples are generated in a randomly uniform way, or with a distribution biased by the per-vertex quality values of the mesh."); 
		case FP_STRATIFIED_SAMPLING  :  return QString("Create a new layer populated with a point sampling of the current mesh; to generate multiple samples inside a triangle each triangle is subdivided according to various <i> stratified</i> strategies. Distribution is often biased by triangle shape."); 
		case FP_CLUSTERED_SAMPLING   :  return QString("Create a new layer populated with a  subsampling of the vertexes of the current mesh; the subsampling is driven by a simple one-per-gridded cell strategy."); 
		case FP_POISSONDISK_SAMPLING :  return QString("Create a new layer populated with a point sampling of the current mesh;"
													   "samples are generated according to a Poisson-disk distribution.  using the algorithm described in:<br>"
													   "<b>'Efficient and Flexible Sampling with Blue Noise Properties of Triangular Meshes'</b><br>"
													   " Massimiliano Corsini, Paolo Cignoni, Roberto Scopigno<br>IEEE TVCG 2012");
		case FP_VARIABLEDISK_SAMPLING:  return QString("Create a new layer populated with a point sampling of the current mesh; samples are generated according to a Poisson-disk distribution"
													   "samples are generated according to a Poisson-disk distribution.  using the algorithm described in:<br>"
													   "<b>Efficient and Flexible Sampling with Blue Noise Properties of Triangular Meshes</b><br>"
													   " Massimiliano Corsini, Paolo Cignoni, Roberto Scopigno<br>IEEE TVCG 2012");
		case FP_HAUSDORFF_DISTANCE   :  return QString("Compute the Hausdorff Distance between two meshes, sampling one of the two and finding foreach sample the closest point over the other mesh."); 
		case FP_TEXEL_SAMPLING       :  return QString("Create a new layer with a point sampling of the current mesh, a sample for each texel of the mesh is generated"); 
		case FP_VERTEX_RESAMPLING    :  return QString("Transfer the choosen per-vertex attributes from one mesh to another. Useful to transfer attributes to different representations of a same object.<br>"
																									"For each vertex of the target mesh the closest point (not vertex!) on the source mesh is computed, and the requested interpolated attributes from that source point are copied into the target vertex.<br>"
																									"The algorithm assumes that the two meshes are reasonably similar and aligned."); 
		case FP_UNIFORM_MESH_RESAMPLING       :  return QString("Create a new mesh that is a resampled version of the current one.<br>"
																									"The resampling is done by building a uniform volumetric representation where each voxel contains the signed distance from the original surface. "
																									"The resampled surface is reconstructed using the <b>marching cube</b> algorithm over this volume."); 
		case FP_VORONOI_CLUSTERING   :  return QString("Apply a clustering algorithm that builds voronoi cells over the mesh starting from random points,"
																									"collapse each voronoi cell to a single vertex, and construct the triangulation according to the clusters adjacency relations.<br>"
																									"Very similar to the technique described in <b>'Approximated Centroidal Voronoi Diagrams for Uniform Polygonal Mesh Coarsening'</b> - Valette Chassery - Eurographics 2004");
		case FP_VORONOI_COLORING   :  return QString("Given a Mesh <b>M</b> and a Pointset <b>P</b>, The filter project each vertex of P over M and color M according to the geodesic distance from these projected points. Projection and coloring are done on a per vertex basis."); 
    case FP_DISK_COLORING   :  return QString("Given a Mesh <b>M</b> and a Pointset <b>P</b>, The filter project each vertex of P over M and color M according to the Euclidean distance from these projected points. Projection and coloring are done on a per vertex basis.");
		case FP_REGULAR_RECURSIVE_SAMPLING   :  return QString("The bbox is recrusively partitioned in a octree style, center of bbox are considered, when the center is nearer to the surface than a given thr it is projected on it. It works also for building ofsetted samples."); 
		default : assert(0); return QString("unknown filter!!!!");

	}
}
 int FilterDocSampling::getRequirements(QAction *action)
{
  switch(ID(action))
  {

    case FP_DISK_COLORING :
    case FP_VORONOI_COLORING : return  MeshModel::MM_VERTFACETOPO  | MeshModel::MM_VERTQUALITY| MeshModel::MM_VERTCOLOR;

		case FP_VORONOI_CLUSTERING : return  MeshModel::MM_VERTFACETOPO;

		case FP_VERTEX_RESAMPLING :
		case FP_UNIFORM_MESH_RESAMPLING:
    case FP_REGULAR_RECURSIVE_SAMPLING:
    case FP_HAUSDORFF_DISTANCE :	return  MeshModel::MM_FACEMARK;
		case FP_ELEMENT_SUBSAMPLING    :   
		case FP_MONTECARLO_SAMPLING :    
		case FP_VARIABLEDISK_SAMPLING :
		case FP_POISSONDISK_SAMPLING :
		case FP_STRATIFIED_SAMPLING :   
    case FP_CLUSTERED_SAMPLING : return 0;
		
		case FP_TEXEL_SAMPLING  :  return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTNORMAL;

    default: assert(0);
  }
  return 0;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterDocSampling::initParameterSet(QAction *action, MeshDocument & md, RichParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_MONTECARLO_SAMPLING :  
 		  parlst.addParam(new RichInt ("SampleNum", md.mm()->cm.vn,
											"Number of samples",
											"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt."));
      parlst.addParam(new RichBool("Weighted",  false,
                     "Quality Weighted Sampling",
                     "Use per vertex quality to drive the vertex sampling. The number of samples falling in each face is proportional to the face area multiplied by the average quality of the face vertices."));
      parlst.addParam(new RichBool("ExactNum",  true,
                     "Exact Sample Num",
                     "If the required total number of samples is not a strict exact requirement we can exploit a different algorithm"
                     "based on the choice of the number of samples inside each triangle by a random Poisson-distributed number with mean equal to the expected number of samples times the area of the triangle over the surface of the whole mesh."));
                      break;
		case FP_STRATIFIED_SAMPLING :  
 		   parlst.addParam(new RichInt ("SampleNum",  std::max(100000,md.mm()->cm.vn),
											"Number of samples",
											"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt."));
			parlst.addParam(new RichEnum("Sampling", 0, 
                  QStringList() << "Similar Triangle" << "Dual Similar Triangle" << "Long Edge Subdiv" << "Sample Edges" << "Sample NonFaux Edges",
									tr("Element to sample:"), 
									tr(	"<b>Similar Triangle</b>: each triangle is subdivided into similar triangles and the internal vertices of these triangles are considered. This sampling leave space around edges and vertices for separate sampling of these entities.<br>"
											"<b>Dual Similar Triangle</b>: each triangle is subdivided into similar triangles and the internal vertices of these triangles are considered.  <br>"
                      "<b>Long Edge Subdiv</b> each triangle is recursively subdivided along the longest edge. <br>"
                      "<b>Sample Edges</b> Only the edges of the mesh are uniformly sampled. <br>"
                      "<b>Sample NonFaux Edges</b> Only the non-faux edges of the mesh are uniformly sampled."
									))); 
											
			 parlst.addParam(new RichBool("Random", false,
											"Random Sampling",
											"if true, for each (virtual) face we draw a random point, otherwise we pick the face midpoint."));
			break;
		case FP_CLUSTERED_SAMPLING :{  
			float maxVal = md.mm()->cm.bbox.Diag();
		  parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Cell Size", "The size of the cell of the clustering grid. Smaller the cell finer the resulting mesh. For obtaining a very coarse mesh use larger values."));

			parlst.addParam(new RichEnum("Sampling", 1, 
									QStringList() << "Average" << "Closest to center", 
									tr("Representative Strataegy:"), 
									tr(	"<b>Average</b>: for each cell we take the average of the sample falling into. The resulting point is a new point.<br>"
                      "<b>Closes to center</b>: for each cell we take the sample that is closest to the center of the cell. Choosen vertices are a subset of the original ones."
									))); 
			parlst.addParam(new RichBool ("Selected", false, "Selected",
											"If true only for the filter is applied only on the selected subset of the mesh."));
											
			}
			break;
		case FP_ELEMENT_SUBSAMPLING :
			parlst.addParam(new RichEnum("Sampling", 0, 
                  QStringList() << "Vertex" << "Edge" << "Face",
									tr("Element to sample:"), 
									tr("Choose what mesh element has to be used for the subsampling. At most one point sample will be added for each one of the chosen elements"))); 
			parlst.addParam(new RichInt("SampleNum", md.mm()->cm.vn/10, "Number of samples", "The desired number of elements that must be chosen. Being a subsampling of the original elements if this number should not be larger than the number of elements of the original mesh."));
			break;
		case FP_POISSONDISK_SAMPLING :

			parlst.addParam(new RichInt("SampleNum", 1000, "Number of samples", "The desired number of samples. The ray of the disk is calculated according to the sampling density."));
			parlst.addParam(new RichAbsPerc("Radius", 0, 0, md.mm()->cm.bbox.Diag(), "Explicit Radius", "If not zero this parameter override the previous parameter to allow exact radius specification"));
      parlst.addParam(new RichInt("MontecarloRate", 20, "MonterCarlo OverSampling", "The over-sampling rate that is used to generate the intial Montecarlo samples (e.g. if this parameter is <i>K</i> means that<i>K</i> x <i>poisson sample</i> points will be used). The generated Poisson-disk samples are a subset of these initial Montecarlo samples. Larger this number slows the process but make it a bit more accurate."));
      parlst.addParam(new RichBool("ApproximateGeodesicDistance", false, "Approximate Geodesic Distance", "If true Poisson Disc distances are computed using an approximate geodesic distance, e.g. an euclidean distance weighted by a function of the difference between the normals of the two points."));
      parlst.addParam(new RichBool("Subsample", false, "Base Mesh Subsampling", "If true the original vertices of the base mesh are used as base set of points. In this case the SampleNum should be obviously much smaller than the original vertex number.<br>Note that this option is very useful in the case you want to subsample a dense point cloud."));
      parlst.addParam(new RichBool("RefineFlag", false, "Refine Existing Samples", "If true the vertices of the below mesh are used as starting vertices, and they will utterly refined by adding more and more points until possible. "));
      parlst.addParam(new RichMesh("RefineMesh", md.mm(),&md, "Samples to be refined", "Used only if the above option is checked. "));
      break;
		case FP_VARIABLEDISK_SAMPLING :
			parlst.addParam(new RichInt("SampleNum", 1000, "Number of samples", "The desired number of samples. The ray of the disk is calculated according to the sampling density."));
			parlst.addParam(new RichAbsPerc("Radius", 0, 0, md.mm()->cm.bbox.Diag(), "Explicit Radius", "If not zero this parameter override the previous parameter to allow exact radius specification"));
			parlst.addParam(new RichFloat("RadiusVariance", 2, "Radius Variance", "The radius of the disk is allowed to vary between r/var and r*var. If this parameter is 1 the sampling is the same of the Poisson Disk Sampling"));
      parlst.addParam(new RichInt("MontecarloRate", 20, "MonterCarlo OverSampling", "The over-sampling rate that is used to generate the intial Montecarlo samples (e.g. if this parameter is x means that x * <i>poisson sample</i> points will be used). The generated Poisson-disk samples are a subset of these initial Montecarlo samples. Larger this number slows the process but make it a bit more accurate."));
			parlst.addParam(new RichBool("Subsample", false, "Base Mesh Subsampling", "If true the original vertices of the base mesh are used as base set of points. In this case the SampleNum should be obviously much smaller than the original vertex number."));
			break;
		case FP_TEXEL_SAMPLING :  
 		  parlst.addParam(new RichInt (	"TextureW", 512, "Texture Width",
											"A sample for each texel is generated, so the desired texture size is need, only samples for the texels falling inside some faces are generated.\n Setting this param to 256 means that you get at most 256x256 = 65536 samples).<br>"
											"If this parameter is 0 the size of the current texture is choosen."));
 		  parlst.addParam(new RichInt (	"TextureH", 512, "Texture Height",
											"A sample for each texel is generated, so the desired texture size is need, only samples for the texels falling inside some faces are generated.\n Setting this param to 256 means that you get at most 256x256 = 65536 samples)"));
 		  parlst.addParam(new RichBool(	"TextureSpace", false, "UV Space Sampling",
											"The generated texel samples have their UV coords as point positions. The resulting point set is has a square domain, the texels/points, even if on a flat domain retain the original vertex normal to help a better perception of the original provenience."));
			parlst.addParam(new RichBool(	"RecoverColor", md.mm()->cm.textures.size()>0, "RecoverColor",
											"The generated point cloud has the current texture color"));
			break;
		case FP_HAUSDORFF_DISTANCE :  
			{
				MeshModel *vertexMesh= md.mm();
				foreach (vertexMesh, md.meshList) 
						if (vertexMesh != md.mm())  break;
		    
				parlst.addParam(new RichMesh ("SampledMesh", md.mm(),&md, "Sampled Mesh",
												"The mesh whose surface is sampled. For each sample we search the closest point on the Target Mesh."));
				parlst.addParam(new RichMesh ("TargetMesh", vertexMesh,&md, "Target Mesh",
												"The mesh that is sampled for the comparison."));
				parlst.addParam(new RichBool ("SaveSample", false, "Save Samples",
												"Save the position and distance of all the used samples on both the two surfaces, creating two new layers with two point clouds representing the used samples."));										
				parlst.addParam(new RichBool ("SampleVert", true, "Sample Vertexes",
												"For the search of maxima it is useful to sample vertices and edges of the mesh with a greater care. "
												"It is quite probably the the farthest points falls along edges or on mesh vertexes, and with uniform montecarlo sampling approaches"
												"the probability of taking a sample over a vertex or an edge is theoretically null.<br>"
												"On the other hand this kind of sampling could make the overall sampling distribution slightly biased and slightly affects the cumulative results."));
				parlst.addParam(new RichBool ("SampleEdge", true, "Sample Edges", "See the above comment."));
				parlst.addParam(new RichBool ("SampleFauxEdge", false, "Sample FauxEdge", "See the above comment."));
				parlst.addParam(new RichBool ("SampleFace", true, "Sample Faces", "See the above comment."));
				parlst.addParam(new RichInt ("SampleNum", md.mm()->cm.vn, "Number of samples",
												"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt."));
				parlst.addParam(new RichAbsPerc("MaxDist", md.mm()->cm.bbox.Diag()/20.0, 0.0f, md.mm()->cm.bbox.Diag(),
												tr("Max Distance"), tr("Sample points for which we do not find anything whithin this distance are rejected and not considered neither for averaging nor for max.")));
			} break;
		case FP_VERTEX_RESAMPLING:
		{
				MeshModel *vertexMesh= md.mm();
				foreach (vertexMesh, md.meshList) 
						if (vertexMesh != md.mm())  break;
						
				parlst.addParam(new RichMesh ("SourceMesh", md.mm(),&md, "Source Mesh",
												"The mesh that contains the source data that we want to transfer."));
				parlst.addParam(new RichMesh ("TargetMesh", vertexMesh,&md, "Target Mesh",
												"The mesh whose vertexes will receive the data from the source."));
				parlst.addParam(new RichBool ("GeomTransfer", false, "Transfer Geometry",
												"if enabled, the position of each vertex of the target mesh will be snapped onto the corresponding closest point on the source mesh"));										
				parlst.addParam(new RichBool ("NormalTransfer", false, "Transfer Normal",
												"if enabled, the normal of each vertex of the target mesh will get the (interpolated) normal of the corresponding closest point on the source mesh"));										
				parlst.addParam(new RichBool ("ColorTransfer", true, "Transfer Color",
												"if enabled, the color of each vertex of the target mesh will become the color of the corresponding closest point on the source mesh"));										
        parlst.addParam(new RichBool ("QualityTransfer", false, "Transfer quality",
                        "if enabled, the quality of each vertex of the target mesh will become the quality of the corresponding closest point on the source mesh"));
        parlst.addParam(new RichBool ("SelectionTransfer", false, "Transfer Selection",
                        "if enabled,  each vertex of the target mesh will be selected if the corresponding closest point on the source mesh falls in a selected face"));
        parlst.addParam(new RichBool ("QualityDistance", false, "Store dist. as quality",
												"if enabled, we store the distance of the transferred value as in the vertex quality"));										
				parlst.addParam(new RichAbsPerc("UpperBound", md.mm()->cm.bbox.Diag()/50.0, 0.0f, md.mm()->cm.bbox.Diag(),
												tr("Max Dist Search"), tr("Sample points for which we do not find anything whithin this distance are rejected and not considered for recovering attributes.")));
		} break; 
		case FP_UNIFORM_MESH_RESAMPLING :
		{
				
			parlst.addParam(new RichAbsPerc("CellSize", md.mm()->cm.bbox.Diag()/50.0, 0.0f, md.mm()->cm.bbox.Diag(),
				tr("Precision"), tr("Size of the cell, the default is 1/50 of the box diag. Smaller cells give better precision at a higher computational cost. Remember that halving the cell size means that you build a volume 8 times larger.")));
			
			parlst.addParam(new RichAbsPerc("Offset", 0.0, -md.mm()->cm.bbox.Diag()/5.0f, md.mm()->cm.bbox.Diag()/5.0f,
												tr("Offset"), tr("Offset of the created surface (i.e. distance of the created surface from the original one).<br>"
																				 "If offset is zero, the created surface passes on the original mesh itself. "
																				 "Values greater than zero mean an external surface, and lower than zero mean an internal surface.<br> "
																				 "In practice this value is the threshold passed to the Marching Cube algorithm to extract the isosurface from the distance field representation.")));
			parlst.addParam(new RichBool ("mergeCloseVert", false, "Clean Vertices",
											"If true the mesh generated by MC will be cleaned by unifying vertices that are almost coincident"));
			parlst.addParam(new RichBool ("discretize", false, "Discretize",
											"If true the position of the intersected edge of the marching cube grid is not computed by linear interpolation, "
											"but it is placed in fixed middle position. As a consequence the resampled object will look severely aliased by a stairstep appearance.<br>"
											"Useful only for simulating the output of 3D printing devices."));

			parlst.addParam(new RichBool ("multisample", false, "Multisample",
											"If true the distance field is more accurately compute by multisampling the volume (7 sample for each voxel). Much slower but less artifacts."));
			parlst.addParam(new RichBool ("absDist", false, "Absolute Distance",
											"If true a <b> not</b> signed distance field is computed. "
											"In this case you have to choose a not zero Offset and a double surface is built around the original surface, inside and outside. "
											"Is useful to convrt thin floating surfaces into <i> solid, thick meshes.</i>. t"));			
		} break; 
		 case FP_VORONOI_CLUSTERING :
		 {
			 parlst.addParam(new RichInt ("SampleNum", md.mm()->cm.vn/100, "Target vertex number",
											"The final number of vertices."));
			 parlst.addParam(new RichInt ("RelaxIter", 1, "Relaxing Iterations",
											"The final number of vertices."));
			 parlst.addParam(new RichInt ("RandSeed", 1, "Random Seed",
											"The final number of vertices."));
			 
		 } break; 
		case FP_VORONOI_COLORING :
		case FP_DISK_COLORING :
		 {
				MeshModel *colorMesh= md.mm();
				foreach (colorMesh, md.meshList) // Search a mesh with some faces..
						if (colorMesh->cm.fn>0)  break;
						
				MeshModel *vertexMesh;
				foreach (vertexMesh, md.meshList) // Search another mesh
						if (vertexMesh != colorMesh)  break;
		    
				parlst.addParam(new RichMesh ("ColoredMesh", colorMesh,&md, "To be Colored Mesh",
												"The mesh whose surface is colored. For each vertex of this mesh we decide the color according the below parameters."));
				parlst.addParam(new RichMesh ("VertexMesh", vertexMesh,&md, "Vertex Mesh",
												"The mesh whose vertexes are used as seed points for the color computation. These seeds point are projected onto the above mesh."));
			if(ID(action) ==	FP_DISK_COLORING) {
					float Diag = md.mm()->cm.bbox.Diag();
					parlst.addParam(new RichDynamicFloat("Radius", Diag/10.0f, 0.0f, Diag/3.0f,  tr("Radius"), 
																"the radius of the spheres centered in the VertexMesh seeds "));
					parlst.addParam(new RichBool("SampleRadius", false, "Use sample radius", "Use the radius that is stored in each sample of the vertex mesh. Useful for displaing the variable disk sampling results"));
          parlst.addParam(new RichBool("ApproximateGeodetic", false, "Use Approximate Geodetic", "Use the Approximate Geodetic Metric instead of the Euclidean distance. Approximate geodetic metric uses the normals between the two points to weight the euclidean distance."));

			} else {
					parlst.addParam(new RichBool ("backward", false, "BackDistance",
													"If true the mesh is colored according the distance from the frontier of the voonoi diagram induced by the VertexMesh seeds."));
			}
		 } break; 
		case FP_REGULAR_RECURSIVE_SAMPLING :
			{
				parlst.addParam(new RichAbsPerc("CellSize", md.mm()->cm.bbox.Diag()/50.0, 0.0f, md.mm()->cm.bbox.Diag(),
				tr("Precision"), tr("Size of the cell, the default is 1/50 of the box diag. Smaller cells give better precision at a higher computational cost. Remember that halving the cell size means that you build a volume 8 times larger.")));
			
				parlst.addParam(new RichAbsPerc("Offset", 0.0, -md.mm()->cm.bbox.Diag()/5.0f, md.mm()->cm.bbox.Diag()/5.0f,
												tr("Offset"), tr("Offset of the created surface (i.e. distance of the created surface from the original one).<br>"
																				 "If offset is zero, the created surface passes on the original mesh itself. "
																				 "Values greater than zero mean an external surface, and lower than zero mean an internal surface.<br> "
																				 "In practice this value is the threshold passed to the Marching Cube algorithm to extract the isosurface from the distance field representation.")));
		 } break; 
   default: break; // do not add any parameter for the other filters
  }
}

bool FilterDocSampling::applyFilter(QAction *action, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(action))
		{
		case FP_ELEMENT_SUBSAMPLING :  
		{
			MeshModel *curMM= md.mm();				
      MeshModel *mm= md.addNewMesh("","Sampled Mesh"); // After Adding a mesh to a MeshDocument the new mesh is the current one
			mm->updateDataMask(curMM);
			
			BaseSampler mps(&(mm->cm));
			
			switch(par.getEnum("Sampling"))
				{
					case 0 :	tri::SurfaceSampling<CMeshO,BaseSampler>::VertexUniform(curMM->cm,mps,par.getInt("SampleNum"));	break;
          case 1 :	tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeUniform(curMM->cm,mps,false);		break;
          case 2 :	tri::SurfaceSampling<CMeshO,BaseSampler>::AllFace(curMM->cm,mps);		break;
				}
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log("Sampling created a new mesh of %i points",md.mm()->cm.vn);		
		}
		break;
		case FP_TEXEL_SAMPLING :  
				{
					MeshModel *curMM= md.mm();	
					if(!tri::HasPerWedgeTexCoord(curMM->cm)) break;
          MeshModel *mm= md.addNewMesh("","Sampled Mesh"); // After Adding a mesh to a MeshDocument the new mesh is the current one
					bool RecoverColor = par.getBool("RecoverColor");
					BaseSampler mps(&(mm->cm));
					mps.texSamplingWidth=par.getInt("TextureW");
					mps.texSamplingHeight=par.getInt("TextureH");
					
					if(RecoverColor && curMM->cm.textures.size()>0)
					{
							mps.tex= new QImage(curMM->cm.textures[0].c_str());
							if(mps.texSamplingWidth==0)  mps.texSamplingWidth  = mps.tex->width();
							if(mps.texSamplingHeight==0) mps.texSamplingHeight = mps.tex->height();
					}
					mps.uvSpaceFlag = par.getBool("TextureSpace");
                                        vcg::tri::UpdateFlags<CMeshO>::FaceClearB(curMM->cm);
					tri::SurfaceSampling<CMeshO,BaseSampler>::Texture(curMM->cm,mps,mps.texSamplingWidth,mps.texSamplingHeight);
					vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
					mm->updateDataMask(MeshModel::MM_VERTNORMAL | MeshModel::MM_VERTCOLOR);
				}
		break;
		case FP_MONTECARLO_SAMPLING :  
		{
			if (md.mm()->cm.fn==0) {
				errorMessage = "This filter requires a mesh with some faces,<br> it does not work on PointSet"; 
				return false; // can't continue, mesh can't be processed
			}
			
			MeshModel *curMM= md.mm();				
      MeshModel *mm= md.addNewMesh("","Montecarlo Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one
			mm->updateDataMask(curMM);

			BaseSampler mps(&(mm->cm));
			if(par.getBool("Weighted")) 
				tri::SurfaceSampling<CMeshO,BaseSampler>::WeightedMontecarlo(curMM->cm,mps,par.getInt("SampleNum"));
      else if(par.getBool("ExactNum")) tri::SurfaceSampling<CMeshO,BaseSampler>::Montecarlo(curMM->cm,mps,par.getInt("SampleNum"));
      else tri::SurfaceSampling<CMeshO,BaseSampler>::MontecarloPoisson(curMM->cm,mps,par.getInt("SampleNum"));
			
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			Log("Sampling created a new mesh of %i points",md.mm()->cm.vn);
		}
			break;
		case FP_STRATIFIED_SAMPLING :  
		{
			if (md.mm()->cm.fn==0) {
				errorMessage = "This filter requires a mesh with some faces,<br> it does not work on PointSet"; 
				return false; // can't continue, mesh can't be processed
			}
			
			MeshModel *curMM= md.mm();				
      MeshModel *mm= md.addNewMesh("","Subdiv Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one
			mm->updateDataMask(curMM);
			int samplingMethod = par.getEnum("Sampling");
			BaseSampler mps(&(mm->cm));
			switch(samplingMethod)
			{
				case 0 :	tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSimilar(curMM->cm,mps,par.getInt("SampleNum"), false ,par.getBool("Random"));
									Log("Similar Sampling created a new mesh of %i points",md.mm()->cm.vn);			
									break;			
				case 1 :	tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSimilar(curMM->cm,mps,par.getInt("SampleNum"), true ,par.getBool("Random"));
									Log("Dual Similar Sampling created a new mesh of %i points",md.mm()->cm.vn);			
									break;			
      case 2 :	tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSubdivision(curMM->cm,mps,par.getInt("SampleNum"), par.getBool("Random"));
                Log("Subdivision Sampling created a new mesh of %i points",md.mm()->cm.vn);
                break;
      case 3 :	tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeUniform(curMM->cm,mps,par.getInt("SampleNum"), true);
                Log("Edge Sampling created a new mesh of %i points",md.mm()->cm.vn);
                break;
      case 4 :	tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeUniform(curMM->cm,mps,par.getInt("SampleNum"), false);
                Log("Non Faux Edge Sampling created a new mesh of %i points",md.mm()->cm.vn);
                break;

			}
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		}
			break;
case FP_CLUSTERED_SAMPLING :  
		{			
			MeshModel *curMM= md.mm();				
      MeshModel *mm= md.addNewMesh("","Cluster Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one
			int samplingMethod = par.getEnum("Sampling");
			float threshold = par.getAbsPerc("Threshold");
			bool selected = par.getBool("Selected");
			if(selected)
			{
					int cnt =tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(curMM->cm);
					Log("Using only %i selected vertexes for choosing",cnt);
			}				
			switch(samplingMethod)
			{
				case 0 :	
						{
							tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > ClusteringGrid;
							ClusteringGrid.Init(curMM->cm.bbox,100000,threshold);
							ClusteringGrid.AddPointSet(curMM->cm,selected);
							ClusteringGrid.ExtractPointSet(mm->cm);
							ClusteringGrid.SelectPointSet(curMM->cm);
							tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(curMM->cm);
							Log("Similar Sampling created a new mesh of %i points",md.mm()->cm.vn);			
						}						
						break;			
				case 1 :	
						{
							vcg::tri::Clustering<CMeshO, vcg::tri::NearestToCenter<CMeshO> > ClusteringGrid;
							ClusteringGrid.Init(curMM->cm.bbox,100000,threshold);
							ClusteringGrid.AddPointSet(curMM->cm,selected);
							ClusteringGrid.SelectPointSet(curMM->cm);
							tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(curMM->cm);
							ClusteringGrid.ExtractPointSet(mm->cm);
							Log("Similar Sampling created a new mesh of %i points",md.mm()->cm.vn);			
						}						
						break;			
			}
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		}
    break;
		case FP_POISSONDISK_SAMPLING :
		case FP_VARIABLEDISK_SAMPLING :
    {
      bool subsampleFlag = par.getBool("Subsample");
      if (md.mm()->cm.fn==0 && subsampleFlag==false)
			{
				errorMessage = "This filter requires a mesh. It does not work on PointSet.";
				return false; // cannot continue
			}

			MeshModel *curMM= md.mm();
      MeshModel *mm= md.addNewMesh("","Poisson-disk Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one
			mm->updateDataMask(curMM);

			float radius = par.getAbsPerc("Radius");
			int sampleNum = par.getInt("SampleNum");
			if(radius==0) radius = tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonDiskRadius(curMM->cm,sampleNum);
						else sampleNum = tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonSampleNum(curMM->cm,radius);

			Log("Computing %i Poisson Samples for an expected radius of %f",sampleNum,radius);
			
			// first of all generate montecarlo samples for fast lookup
			CMeshO *presampledMesh=&(curMM->cm);
			
			CMeshO MontecarloMesh; // this mesh is used only if we need real poisson sampling (and therefore we need to choose points different from the starting mesh vertices)
			
      if(!subsampleFlag)
      {
          QTime tt;tt.start();
          BaseSampler sampler(&MontecarloMesh);
          sampler.qualitySampling =true;
          tri::SurfaceSampling<CMeshO,BaseSampler>::Montecarlo(curMM->cm, sampler, sampleNum*par.getInt("MontecarloRate"));
          MontecarloMesh.bbox = curMM->cm.bbox; // we want the same bounding box
          presampledMesh=&MontecarloMesh;
          Log("Generated %i Montecarlo Samples (%i msec)",MontecarloMesh.vn,tt.elapsed());
      }

      BaseSampler mps(&(mm->cm));

      tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskParam pp;
      if(ID(action)==FP_VARIABLEDISK_SAMPLING)
      {
          pp.adaptiveRadiusFlag=true;
          pp.radiusVariance = par.getFloat("RadiusVariance");
          Log("Variable Density variance is %f, radius can vary from %f to %f",pp.radiusVariance,radius/pp.radiusVariance,radius*pp.radiusVariance);
      }
      else
      {
          if(par.getBool("RefineFlag"))
          {
              pp.preGenFlag=true;
              pp.preGenMesh=&(par.getMesh("RefineMesh")->cm);
          }
          pp.geodesicDistanceFlag=par.getBool("ApproximateGeodesicDistance");
      }
      tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskPruning(curMM->cm, mps, *presampledMesh, radius,pp);
      //tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDisk(curMM->cm, mps, *presampledMesh, radius,pp);

      vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
      Log("Sampling created a new mesh of %i points",md.mm()->cm.vn);
		}
			break;
			
		case FP_HAUSDORFF_DISTANCE : 
			{
			MeshModel* mm0 = par.getMesh("SampledMesh");  // surface where we choose the random samples 
			MeshModel* mm1 = par.getMesh("TargetMesh");   // surface that is sought for the closest point to each sample. 
			bool saveSampleFlag=par.getBool("SaveSample");
			bool sampleVert=par.getBool("SampleVert");
			bool sampleEdge=par.getBool("SampleEdge");
			bool sampleFauxEdge=par.getBool("SampleFauxEdge");
			bool sampleFace=par.getBool("SampleFace");
      float distUpperBound = par.getAbsPerc("MaxDist");

      if(sampleEdge && mm0->cm.fn==0) {
        Log("Disabled edge sampling. Meaningless when sampling point clouds");
        sampleEdge=false;
      }
      if(sampleFace && mm0->cm.fn==0) {
        Log("Disabled face sampling. Meaningless when sampling point clouds");
        sampleFace=false;
      }
			
            mm0->updateDataMask(MeshModel::MM_VERTQUALITY);
            mm1->updateDataMask(MeshModel::MM_VERTQUALITY);
			mm1->updateDataMask(MeshModel::MM_FACEMARK);
			tri::UpdateNormals<CMeshO>::PerFaceNormalized(mm1->cm);

			MeshModel *samplePtMesh =0; 
			MeshModel *closestPtMesh =0; 
		  HausdorffSampler hs;
			if(saveSampleFlag)
				{
          closestPtMesh=md.addNewMesh("","Hausdorff Closest Points"); // After Adding a mesh to a MeshDocument the new mesh is the current one
					closestPtMesh->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
          samplePtMesh=md.addNewMesh("","Hausdorff Sample Point");
					samplePtMesh->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
					hs.init(&(mm1->cm),&(samplePtMesh->cm),&(closestPtMesh->cm));
				}
			else hs.init(&(mm1->cm));
				
			hs.dist_upper_bound = distUpperBound;
			
			qDebug("Sampled  mesh has %7i vert %7i face",mm0->cm.vn,mm0->cm.fn);
			qDebug("Searched mesh has %7i vert %7i face",mm1->cm.vn,mm1->cm.fn);
      qDebug("Max sampling distance %f on a bbox diag of %f",distUpperBound,mm1->cm.bbox.Diag());

			if(sampleVert) 
					tri::SurfaceSampling<CMeshO,HausdorffSampler>::VertexUniform(mm0->cm,hs,par.getInt("SampleNum"));
			if(sampleEdge) 
					tri::SurfaceSampling<CMeshO,HausdorffSampler>::EdgeUniform(mm0->cm,hs,par.getInt("SampleNum"),sampleFauxEdge);
			if(sampleFace)	
					tri::SurfaceSampling<CMeshO,HausdorffSampler>::Montecarlo(mm0->cm,hs,par.getInt("SampleNum"));
				
			Log("Hausdorff Distance computed");						
      Log("     Sampled %i pts (rng: 0) on %s searched closest on %s",hs.n_total_samples,qPrintable(mm0->label()),qPrintable(mm1->label()));
      Log("     min : %f   max %f   mean : %f   RMS : %f",hs.getMinDist(),hs.getMaxDist(),hs.getMeanDist(),hs.getRMSDist());
      float d = mm0->cm.bbox.Diag();
      Log("Values w.r.t. BBox Diag (%f)",d);
      Log("     min : %f   max %f   mean : %f   RMS : %f\n",hs.getMinDist()/d,hs.getMaxDist()/d,hs.getMeanDist()/d,hs.getRMSDist()/d);
			
			
			if(saveSampleFlag)
				{
					tri::UpdateBounding<CMeshO>::Box(samplePtMesh->cm);
					tri::UpdateBounding<CMeshO>::Box(closestPtMesh->cm);
					tri::UpdateColor<CMeshO>::VertexQualityRamp(samplePtMesh->cm);
					tri::UpdateColor<CMeshO>::VertexQualityRamp(closestPtMesh->cm);
				}
			}
			break;
		case	 FP_VERTEX_RESAMPLING :
		{
			MeshModel* srcMesh = par.getMesh("SourceMesh"); // mesh whose attribute are read
			MeshModel* trgMesh = par.getMesh("TargetMesh"); // this whose surface is sought for the closest point to each sample. 
			float upperbound = par.getAbsPerc("UpperBound"); // maximum distance to stop search
			srcMesh->updateDataMask(MeshModel::MM_FACEMARK);
			tri::UpdateNormals<CMeshO>::PerFaceNormalized(srcMesh->cm);

		  RedetailSampler rs;
			rs.init(&(srcMesh->cm),cb,trgMesh->cm.vn);
				
			rs.dist_upper_bound = upperbound;
			rs.colorFlag=par.getBool("ColorTransfer");
			rs.coordFlag=par.getBool("GeomTransfer");
			rs.normalFlag=par.getBool("NormalTransfer");
			rs.qualityFlag=par.getBool("QualityTransfer");
      rs.selectionFlag=par.getBool("SelectionTransfer");
			
			rs.storeDistanceAsQualityFlag=par.getBool("QualityDistance");

      if(!rs.colorFlag && !rs.coordFlag && !rs.qualityFlag  && !rs.normalFlag && !rs.selectionFlag)
			{
				errorMessage = QString("You have to choose at least one attribute to be sampled");
				return false;
			}
			
			if(rs.colorFlag) {
					trgMesh->updateDataMask(MeshModel::MM_VERTCOLOR);
			}
			
			if(rs.qualityFlag){
					trgMesh->updateDataMask(MeshModel::MM_VERTQUALITY);
			}

			qDebug("Source  mesh has %7i vert %7i face",srcMesh->cm.vn,srcMesh->cm.fn);
			qDebug("Target  mesh has %7i vert %7i face",trgMesh->cm.vn,trgMesh->cm.fn);

			tri::SurfaceSampling<CMeshO,RedetailSampler>::VertexUniform(trgMesh->cm,rs,trgMesh->cm.vn);
			
			if(rs.coordFlag) tri::UpdateNormals<CMeshO>::PerFaceNormalized(trgMesh->cm);
			
		} break;
		case FP_UNIFORM_MESH_RESAMPLING :
		{
			if (md.mm()->cm.fn==0) {
				errorMessage = "This filter requires a mesh with some faces,<br> it does not work on PointSet"; 
				return false; // can't continue, mesh can't be processed
			}
			
			float voxelSize = par.getAbsPerc("CellSize");
			float offsetThr = par.getAbsPerc("Offset");
			bool discretizeFlag = par.getBool("discretize");
			bool multiSampleFlag = par.getBool("multisample");
			bool absDistFlag = par.getBool("absDist");
			bool mergeCloseVert = par.getBool("mergeCloseVert");
			
			MeshModel *baseMesh= md.mm();				
      MeshModel *offsetMesh =md.addNewMesh("","Offset mesh");
			baseMesh->updateDataMask(MeshModel::MM_FACEMARK);	
			
			Point3i volumeDim;
			Box3f volumeBox = baseMesh->cm.bbox;
			volumeBox.Offset(volumeBox.Diag()/10.0f+offsetThr);
			BestDim(volumeBox , voxelSize, volumeDim );
			
			Log("Resampling mesh using a volume of %i x %i x %i",volumeDim[0],volumeDim[1],volumeDim[2]);
			Log("     VoxelSize is %f, offset is %f ", voxelSize,offsetThr);
			Log("     Mesh Box is %f %f %f",baseMesh->cm.bbox.DimX(),baseMesh->cm.bbox.DimY(),baseMesh->cm.bbox.DimZ() );
			
			tri::Resampler<CMeshO,CMeshO,float>::Resample(baseMesh->cm, offsetMesh->cm, volumeBox, volumeDim, voxelSize*3.5, offsetThr,discretizeFlag,multiSampleFlag,absDistFlag, cb);
			tri::UpdateBounding<CMeshO>::Box(offsetMesh->cm);
			if(mergeCloseVert)
			{
				float mergeThr =offsetMesh->cm.bbox.Diag()/10000.0f;
				int total = tri::Clean<CMeshO>::MergeCloseVertex(offsetMesh->cm,mergeThr);
				Log("Successfully merged %d vertices with a distance lower than %f", total,mergeThr);
            }
			tri::UpdateNormals<CMeshO>::PerVertexPerFace(offsetMesh->cm);
		} break;
		case FP_VORONOI_CLUSTERING :
		{
			tri::Clean<CMeshO>::RemoveUnreferencedVertex(md.mm()->cm);
			tri::Allocator<CMeshO>::CompactVertexVector(md.mm()->cm);
			tri::Allocator<CMeshO>::CompactFaceVector(md.mm()->cm);
			int sampleNum = par.getInt("SampleNum");
			int relaxIter = par.getInt("RelaxIter");
			int randSeed = par.getInt("RandSeed");
                        // Following two lines commented by Alessandro Giannini
                        // Parameters "SaveSample" and "QualityFlag" not defined in initParameterSet for FP_VORONOI_CLUSTERING
                        //bool saveSampleFlag=par.getBool("SaveSample");
                        //bool qualityFlag = par.getBool("QualityFlag");
			CMeshO *cm = &md.mm()->cm;				
//			MeshModel *clusteredMesh =md.addNewMesh("Offset mesh");
			vector<CMeshO::VertexType *> seedVec;
			md.mm()->updateDataMask(MeshModel::MM_VERTMARK);	
			md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);	
			md.mm()->updateDataMask(MeshModel::MM_VERTQUALITY);	

			tri::ClusteringSampler<CMeshO> vc(&seedVec);
			if(randSeed!=0) tri::SurfaceSampling<CMeshO, tri::ClusteringSampler<CMeshO> >::SamplingRandomGenerator().initialize(randSeed);
			tri::SurfaceSampling<CMeshO, tri::ClusteringSampler<CMeshO> >::VertexUniform(*cm,vc,sampleNum);
			tri::VoronoiProcessing<CMeshO>::VoronoiRelaxing(*cm, seedVec, relaxIter,90,cb);
			
			//VoronoiProcessing<CMeshO>::VoronoiClustering(*cm,clusteredMesh->cm,seedVec);

	//			tri::UpdateBounding<CMeshO>::Box(clusteredMesh->cm);
	//			tri::UpdateNormals<CMeshO>::PerVertexPerFace(clusteredMesh->cm);

		}
		break;
		case FP_VORONOI_COLORING :
		{
			MeshModel* mmM = par.getMesh("ColoredMesh");  // surface where we choose the random samples 
			MeshModel* mmV = par.getMesh("VertexMesh");   // surface that is sought for the closest point to each sample. 
			bool backwardFlag = par.getBool("backward");
			mmM->updateDataMask(MeshModel::MM_VERTFACETOPO);	
			tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmM->cm);
			tri::Allocator<CMeshO>::CompactVertexVector(mmM->cm);
			tri::Allocator<CMeshO>::CompactFaceVector(mmM->cm);
			vector<CMeshO::CoordType> vecP; 
			// Fills the point vector with the position of the Point cloud
			for(CMeshO::VertexIterator vi= mmV->cm.vert.begin(); vi!= mmV->cm.vert.end(); ++vi) if(!(*vi).IsD())
				vecP.push_back((*vi).cP());
			
			vector<CMeshO::VertexPointer> vecV; // points to vertexes of ColoredMesh; 
			tri::VoronoiProcessing<CMeshO>::SeedToVertexConversion	(mmM->cm, vecP, vecV);
			Log("Converted %ui points into %ui vertex ",vecP.size(),vecV.size());
			tri::VoronoiProcessing<CMeshO>::ComputePerVertexSources(mmM->cm,vecV);

			for(uint i=0;i<vecV.size();++i) vecV[i]->C()=Color4b::Red;
		  tri::VoronoiProcessing<CMeshO>::VoronoiColoring(mmM->cm, vecV,backwardFlag);
		} break;

	case FP_DISK_COLORING :
		{
			MeshModel* mmM = par.getMesh("ColoredMesh");  
			MeshModel* mmV = par.getMesh("VertexMesh");   
			typedef vcg::SpatialHashTable<CMeshO::VertexType, float> SampleSHT;
			SampleSHT sht;
	  tri::VertTmark<CMeshO> markerFunctor;
			typedef vcg::vertex::PointDistanceFunctor<float> VDistFunct;
			tri::UpdateColor<CMeshO>::VertexConstant(mmM->cm, Color4b::LightGray);
      tri::UpdateQuality<CMeshO>::VertexConstant(mmM->cm, std::numeric_limits<float>::max());
      bool approximateGeodeticFlag = par.getBool("ApproximateGeodetic");
      bool sampleRadiusFlag = par.getBool("SampleRadius");
      sht.Set(mmM->cm.vert.begin(),mmM->cm.vert.end());
          std::vector<CMeshO::VertexType*> closests;
          float radius = par.getDynamicFloat("Radius");

          for(CMeshO::VertexIterator viv = mmV->cm.vert.begin(); viv!= mmV->cm.vert.end(); ++viv) if(!(*viv).IsD())
          {
            Point3f p = viv->cP();
            if(sampleRadiusFlag) radius = viv->Q();
            Box3f bb(p-Point3f(radius,radius,radius),p+Point3f(radius,radius,radius));
            GridGetInBox(sht, markerFunctor, bb, closests);

            for(size_t i=0; i<closests.size(); ++i)
            {
                float dist;
                if(approximateGeodeticFlag)
                    dist = ApproximateGeodesicDistance(viv->cP(),viv->cN(),closests[i]->cP(),closests[i]->cN());
                else
                    dist = Distance(p,closests[i]->cP());

                if(dist < radius && closests[i]->Q() > dist)
                {
                    closests[i]->Q() = dist;
                    closests[i]->C().lerp(Color4b::White,Color4b::Red,dist/radius);
                }
            }
      }
		} break;
	case FP_REGULAR_RECURSIVE_SAMPLING :
		{
			if (md.mm()->cm.fn==0) {
				errorMessage = "This filter requires a mesh with some faces,<br> it does not work on PointSet"; 
				return false; // can't continue, mesh can't be processed
			}
			float CellSize = par.getAbsPerc("CellSize");
			float offset=par.getAbsPerc("Offset");

			MeshModel *mmM= md.mm();				
      MeshModel *mm= md.addNewMesh("","Recur Samples"); // After Adding a mesh to a MeshDocument the new mesh is the current one

			tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmM->cm);
			tri::Allocator<CMeshO>::CompactVertexVector(mmM->cm);
			tri::Allocator<CMeshO>::CompactFaceVector(mmM->cm);

			tri::UpdateNormals<CMeshO>::PerFaceNormalized(mmM->cm);
			std::vector<Point3f> pvec;
	
			tri::SurfaceSampling<CMeshO,RedetailSampler>::RegularRecursiveOffset(mmM->cm,pvec, offset, CellSize);
			qDebug("Generated %i points",int(pvec.size()));
			
			for(uint i=0;i<pvec.size();++i)
			{
				tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
				mm->cm.vert.back().P() = pvec[i];
			}
			
		}
		break;
		default : assert(0);
		}
	return true;
}
 MeshFilterInterface::FilterClass FilterDocSampling::getClass(QAction *action)
{
	switch(ID(action))
	{
		case FP_VERTEX_RESAMPLING :
		case FP_HAUSDORFF_DISTANCE :
		case FP_ELEMENT_SUBSAMPLING    :
		case FP_MONTECARLO_SAMPLING :
		case FP_STRATIFIED_SAMPLING :
		case FP_CLUSTERED_SAMPLING :
		case FP_POISSONDISK_SAMPLING : 
		case FP_VARIABLEDISK_SAMPLING : 
		case FP_REGULAR_RECURSIVE_SAMPLING : 
		case FP_TEXEL_SAMPLING  :  return FilterDocSampling::Sampling; 
		case FP_VORONOI_CLUSTERING: return FilterDocSampling::Remeshing;
		case FP_UNIFORM_MESH_RESAMPLING: return FilterDocSampling::Remeshing;
		case FP_DISK_COLORING: 
		case FP_VORONOI_COLORING: return MeshFilterInterface::FilterClass(FilterDocSampling::Sampling | FilterDocSampling::VertexColoring);
		default: assert(0);
	}
	return FilterClass(0);
}
int FilterDocSampling::postCondition( QAction* a ) const
{
   switch(ID(a)){
   case FP_VORONOI_COLORING:
   case FP_DISK_COLORING:
     return MeshModel::MM_VERTCOLOR;
   }
   return MeshModel::MM_UNKNOWN;
}

Q_EXPORT_PLUGIN(FilterDocSampling)
