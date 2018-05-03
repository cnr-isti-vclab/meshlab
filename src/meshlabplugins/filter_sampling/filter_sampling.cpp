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
#include <vcg/complex/algorithms/voronoi_processing.h>

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
  BaseSampler(CMeshO* _m){
    m=_m;
    uvSpaceFlag = false;
    qualitySampling=false;
    perFaceNormal=false;
    tex=0;
  }
  CMeshO *m;
  QImage* tex;
  int texSamplingWidth;
  int texSamplingHeight;
  bool uvSpaceFlag;
  bool qualitySampling;
  bool perFaceNormal;  // default false; if true the sample normal is the face normal, otherwise it is interpolated

  void reset()
  {
    m->Clear();
  }

  void AddVert(const CMeshO::VertexType &p)
  {
    tri::Allocator<CMeshO>::AddVertices(*m,1);
    m->vert.back().ImportData(p);
  }

  void AddFace(const CMeshO::FaceType &f, CMeshO::CoordType p)
  {
    tri::Allocator<CMeshO>::AddVertices(*m,1);
    m->vert.back().P() = f.cP(0)*p[0] + f.cP(1)*p[1] +f.cP(2)*p[2];

    if(perFaceNormal) m->vert.back().N() = f.cN();
       else m->vert.back().N() = f.cV(0)->N()*p[0] + f.cV(1)->N()*p[1] + f.cV(2)->N()*p[2];
    if (qualitySampling)
      m->vert.back().Q() = f.cV(0)->Q()*p[0] + f.cV(1)->Q()*p[1] + f.cV(2)->Q()*p[2];
  }
  void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const Point2i &tp, float edgeDist)
  {
    if (edgeDist != .0) return;

    tri::Allocator<CMeshO>::AddVertices(*m,1);

    if(uvSpaceFlag) m->vert.back().P() = Point3m(float(tp[0]),float(tp[1]),0);
    else m->vert.back().P() = f.cP(0)*p[0] + f.cP(1)*p[1] +f.cP(2)*p[2];

    m->vert.back().N() = f.cV(0)->N()*p[0] + f.cV(1)->N()*p[1] +f.cV(2)->N()*p[2];
    if(tex)
    {
      QRgb val;
      // Computing normalized texels position
      int xpos = (int)(tex->width()  * (float(tp[0])/texSamplingWidth)) % tex->width();
      int ypos = (int)(tex->height() * (1.0- float(tp[1])/texSamplingHeight)) % tex->height();

      if (xpos < 0) xpos += tex->width();
      if (ypos < 0) ypos += tex->height();

      val = tex->pixel(xpos,ypos);
      m->vert.back().C()=Color4b(qRed(val),qGreen(val),qBlue(val),255);
    }

  }
}; // end class BaseSampler



/* This sampler is used to transfer the detail of a mesh onto another one.
 * It keep internally the spatial indexing structure used to find the closest point
 */
class LocalRedetailSampler
{
  typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
  typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > VertexMeshGrid;

public:

  LocalRedetailSampler():m(0) {}

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
    tri::UpdateNormal<CMeshO>::PerFaceNormalized(*m);
    if(m->fn==0) useVertexSampling = true;
    else useVertexSampling = false;

    if(useVertexSampling) unifGridVert.Set(m->vert.begin(),m->vert.end());
    else  unifGridFace.Set(m->face.begin(),m->face.end());
    markerFunctor.SetMesh(m);
    // sampleNum and sampleCnt are used only for the progress callback.
    cb=_cb;
    sampleNum = targetSz;
    sampleCnt = 0;
  }

  // this function is called for each vertex of the target mesh.
  // and retrieve the closest point on the source mesh.
  void AddVert(CMeshO::VertexType &p)
  {
    assert(m);
    // the results
    Point3m       closestPt,      normf, bestq, ip;
    CMeshO::ScalarType dist = dist_upper_bound;
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

      Point3m interp;
      InterpolationParameters(*nearestF,(*nearestF).cN(),closestPt, interp);
      interp[2]=1.0-interp[1]-interp[0];

      if(coordFlag) p.P()=closestPt;
      if(colorFlag) p.C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),nearestF->V(2)->C(),interp);
      if(normalFlag) p.N() = nearestF->V(0)->N()*interp[0] + nearestF->V(1)->N()*interp[1] + nearestF->V(2)->N()*interp[2];
      if(qualityFlag) p.Q()= nearestF->V(0)->Q()*interp[0] + nearestF->V(1)->Q()*interp[1] + nearestF->V(2)->Q()*interp[2];
	  if (selectionFlag)
	  {
		  if (nearestF->IsS()) p.SetS();
		  else if (nearestF->V(0)->IsS() || nearestF->V(1)->IsS() || nearestF->V(2)->IsS()) p.SetS();
	  }
    }
  }
}; // end class RedetailSampler

//--------------------------------------------------------------------
// simple sampler to calculate
// it is very similar to the hausdorff sampler, but more immediate to use
class SimpleDistanceSampler
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
	typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > MetroMeshVertexGrid;

public:

	SimpleDistanceSampler(CMeshO* _m, bool signedDist, double maxd) :markerFunctor(_m)
	{
		m = _m;
		useSigned = signedDist;
		maxDistABS = maxd;
		init();
	}

	CMeshO *m;           /// the reference mesh

	MetroMeshVertexGrid   unifGridVert;
	MetroMeshFaceGrid     unifGridFace;

	bool useVertexSampling;
	CMeshO::ScalarType dist_upper_bound;  // samples that have a distance beyond this threshold distance are not considered.
	tri::FaceTmark<CMeshO> markerFunctor;

	bool useSigned;
	double maxDistABS;

	// distance data
	int  n_total_samples;
	double          min_dist;
	double          max_dist;
	double          mean_dist;
	double          RMS_dist;   /// from the wikipedia defintion RMS DIST is sqrt(Sum(distances^2)/n), here we store Sum(distances^2)

	float getMeanDist() const { return mean_dist / n_total_samples; }
	float getMinDist() const  { return min_dist; }
	float getMaxDist() const  { return max_dist; }
	float getRMSDist() const  { return sqrt(RMS_dist / n_total_samples); }

	void init()
	{
		if (m->fn == 0) // if no faces, we can only use points
		{
			useVertexSampling = true;
			unifGridVert.Set(m->vert.begin(), m->vert.end());
		}
		else 
		{
			useVertexSampling = false;
			unifGridFace.Set(m->face.begin(), m->face.end());
			markerFunctor.SetMesh(m);
		}

		min_dist = std::numeric_limits<double>::max();
		max_dist = std::numeric_limits<double>::min();
		mean_dist = 0;
		RMS_dist = 0;
		n_total_samples = 0;
	}

	void AddVert(CMeshO::VertexType &p)
	{
		p.Q() = AddSample(p.cP(), p.cN());
	}

	float AddSample(const CMeshO::CoordType &startPt, const CMeshO::CoordType &startN)
	{
		// the results
		CMeshO::CoordType closestPt;
		CMeshO::CoordType closestNm;
		CMeshO::ScalarType dist = dist_upper_bound;

		// compute distance between startPt and the mesh S2
		CMeshO::FaceType   *nearestF = 0;
		CMeshO::VertexType *nearestV = 0;
		vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;

		if (useVertexSampling)
		{
			nearestV = tri::GetClosestVertex<CMeshO, MetroMeshVertexGrid>(*m, unifGridVert, startPt, maxDistABS, dist);
			if (nearestV == NULL) return (maxDistABS*2.0);

			closestPt = nearestV->P();
			closestNm = nearestV->N();
		}
		else
		{
			nearestF = unifGridFace.GetClosest(PDistFunct, markerFunctor, startPt, maxDistABS, dist, closestPt);
			if (nearestF == NULL) return (maxDistABS*2.0);

			closestNm = nearestF->N();
		}

		// check sign of distance
		if ((useSigned) && (((startPt - closestPt).Normalize()*(closestNm)) < 0.0))
		{
			dist = -dist;
		}

		if (dist > max_dist) max_dist = dist;
		if (dist < min_dist) min_dist = dist;

		mean_dist += dist;	       
		RMS_dist += dist*dist;     
		n_total_samples++;

		return dist;
	}
}; 

//--------------------------------------------------------------------



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
      << FP_HAUSDORFF_DISTANCE
	  << FP_DISTANCE_REFERENCE
      << FP_TEXEL_SAMPLING
      << FP_VERTEX_RESAMPLING
      << FP_UNIFORM_MESH_RESAMPLING
      << FP_VORONOI_COLORING
      << FP_DISK_COLORING
      << FP_REGULAR_RECURSIVE_SAMPLING
      << FP_POINTCLOUD_SIMPLIFICATION
         ;

  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterDocSampling::filterName(FilterIDType filterId) const
{
  switch(filterId) {
  case FP_ELEMENT_SUBSAMPLING    :  return QString("Mesh Element Sampling");
  case FP_MONTECARLO_SAMPLING :  return QString("Montecarlo Sampling");
  case FP_STRATIFIED_SAMPLING :  return QString("Stratified Triangle Sampling");
  case FP_CLUSTERED_SAMPLING :  return QString("Clustered Vertex Sampling");
  case FP_POISSONDISK_SAMPLING : return QString("Poisson-disk Sampling");
  case FP_HAUSDORFF_DISTANCE  :  return QString("Hausdorff Distance");
  case FP_DISTANCE_REFERENCE:  return QString("Distance from Reference Mesh");
  case FP_TEXEL_SAMPLING  :  return QString("Texel Sampling");
  case FP_VERTEX_RESAMPLING  :  return QString("Vertex Attribute Transfer");
  case FP_UNIFORM_MESH_RESAMPLING  :  return QString("Uniform Mesh Resampling");
  case FP_VORONOI_COLORING  :  return QString("Voronoi Vertex Coloring");
  case FP_DISK_COLORING  :  return QString("Disk Vertex Coloring");
  case FP_REGULAR_RECURSIVE_SAMPLING  :  return QString("Regular Recursive Sampling");
  case FP_POINTCLOUD_SIMPLIFICATION  :  return QString("Point Cloud Simplification");

  default : assert(0); return QString("unknown filter!!!!");
  }
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterDocSampling::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_ELEMENT_SUBSAMPLING        :  return QString("Create a new layer populated with a point sampling of the current mesh; at most one sample for each element of the mesh is created. Samples are taking in a uniform way, one for each element (vertex/edge/face); all the elements have the same probabilty of being choosen.");
		case FP_MONTECARLO_SAMPLING        :  return QString("Create a new layer populated with a point sampling of the current mesh; samples are generated in a randomly uniform way, or with a distribution biased by the per-vertex quality values of the mesh.");
		case FP_STRATIFIED_SAMPLING        :  return QString("Create a new layer populated with a point sampling of the current mesh; to generate multiple samples inside a triangle each triangle is subdivided according to various <i>stratified</i> strategies. Distribution is often biased by triangle shape.");
		case FP_CLUSTERED_SAMPLING         :  return QString("Create a new layer populated with a subsampling of the vertexes of the current mesh; the subsampling is driven by a simple one-per-gridded cell strategy.");
		case FP_POINTCLOUD_SIMPLIFICATION  :  return QString("Create a new layer populated with a simplified version of the current point cloud.");
		case FP_POISSONDISK_SAMPLING       :  return QString("Create a new layer populated with a point sampling of the current mesh;"
													"samples are generated according to a Poisson-disk distribution, using the algorithm described in:<br>"
													"<b>'Efficient and Flexible Sampling with Blue Noise Properties of Triangular Meshes'</b><br>"
													" Massimiliano Corsini, Paolo Cignoni, Roberto Scopigno<br>IEEE TVCG 2012");
		case FP_HAUSDORFF_DISTANCE         :  return QString("Compute the Hausdorff Distance between two layers, sampling one of the two and finding for each sample the closest point over the other mesh.");
		case FP_DISTANCE_REFERENCE         :  return QString("Compute the signed/unsigned (per vertex) distance between a mesh/pointcloud and a reference mesh/pointcloud. Distance is stored in vertex quality.");
		case FP_TEXEL_SAMPLING             :  return QString("Create a new layer with a point sampling of the current mesh, a sample for each texel of the mesh is generated");
		case FP_VERTEX_RESAMPLING          :  return QString("Transfer the choosen per-vertex attributes from one layer to another. Useful to transfer attributes to different representations of a same object.<br>"
													"For each vertex of the target mesh the closest point (not vertex!) on the source mesh is computed, and the requested interpolated attributes from that source point are copied into the target vertex.<br>"
													"The algorithm assumes that the two meshes are reasonably similar and aligned.");
		case FP_UNIFORM_MESH_RESAMPLING    :  return QString("Create a new mesh that is a resampled version of the current one.<br>"
															"The resampling is done by building a uniform volumetric representation where each voxel contains the signed distance from the original surface. "
															"The resampled surface is reconstructed using the <b>marching cube</b> algorithm over this volume.");
		case FP_VORONOI_COLORING           :  return QString("Given a Mesh <b>M</b> and a Pointset <b>P</b>, The filter project each vertex of P over M and color M according to the geodesic distance from these projected points. Projection and coloring are done on a per vertex basis.");
		case FP_DISK_COLORING              :  return QString("Given a Mesh <b>M</b> and a Pointset <b>P</b>, The filter project each vertex of P over M and color M according to the Euclidean distance from these projected points. Projection and coloring are done on a per vertex basis.");
		case FP_REGULAR_RECURSIVE_SAMPLING :  return QString("The bbox is recursively partitioned in a octree style, center of bbox are considered, when the center is nearer to the surface than a given thr it is projected on it. It works also for building ofsetted samples.");
		default : assert(0); return QString("unknown filter!!!!");
  }
}
int FilterDocSampling::getRequirements(QAction *action)
{
  switch(ID(action))
  {

  case FP_DISK_COLORING :
  case FP_VORONOI_COLORING : return  MeshModel::MM_VERTFACETOPO  | MeshModel::MM_VERTQUALITY| MeshModel::MM_VERTCOLOR;
  case FP_VERTEX_RESAMPLING :
  case FP_UNIFORM_MESH_RESAMPLING:
  case FP_REGULAR_RECURSIVE_SAMPLING:
  case FP_HAUSDORFF_DISTANCE :	return  MeshModel::MM_FACEMARK;
  case FP_DISTANCE_REFERENCE :
  case FP_ELEMENT_SUBSAMPLING :
  case FP_MONTECARLO_SAMPLING :
  case FP_POISSONDISK_SAMPLING :
  case FP_POINTCLOUD_SIMPLIFICATION :
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
    parlst.addParam(new RichBool("PerFaceNormal",  false,
                                 "Per-Face Normal",
                                 "If true for each sample we take the normal of the sampled face, otherwise the normal interpolated from the vertex normals."));
    parlst.addParam(new RichFloat("RadiusVariance", 1, "Radius Variance", "The radius of the disk is allowed to vary between r/var and r*var. If this parameter is 1 the sampling is the same of the Poisson Disk Sampling"));
    parlst.addParam(new RichBool("ExactNum",  true,
                                 "Exact Sample Number",
                                 "If the required total number of samples is not a strict exact requirement we can exploit a different algorithm"
                                 "based on the choice of the number of samples inside each triangle by a random Poisson-distributed number with mean equal to the expected number of samples times the area of the triangle over the surface of the whole mesh."));
    parlst.addParam(new RichBool("EdgeSampling",  false,
                                 "Sample CreaseEdge Only",
                                 "Restrict the sampling process to the crease edges only. Useful to sample in a more accurate way the feature edges of a mechanical mesh."));
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
                                 tr("Representative Strategy:"),
                                 tr(	"<b>Average</b>: for each cell we take the average of the sample falling into. The resulting point is a new point.<br>"
                                        "<b>Closest to center</b>: for each cell we take the sample that is closest to the center of the cell. Choosen vertices are a subset of the original ones."
                                        )));
    parlst.addParam(new RichBool ("Selected", false, "Only on Selection",
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

  case FP_POINTCLOUD_SIMPLIFICATION :
    parlst.addParam(new RichInt("SampleNum", 1000, "Number of samples", "The desired number of samples. The ray of the disk is calculated according to the sampling density."));
    parlst.addParam(new RichAbsPerc("Radius", 0, 0, md.mm()->cm.bbox.Diag(), "Explicit Radius", "If not zero this parameter override the previous parameter to allow exact radius specification"));
    parlst.addParam(new RichBool("BestSampleFlag", true, "Best Sample Heuristic", "If true it will use a simple heuristic for choosing the samples. At a small cost (it can slow a bit the process) it usually improve the maximality of the generated sampling. "));
    parlst.addParam(new RichInt("BestSamplePool", 10, "Best Sample Pool Size", "Used only if the Best Sample Flag is true. It control the number of attempt that it makes to get the best sample. It is reasonable that it is smaller than the Montecarlo oversampling factor."));
    parlst.addParam(new RichBool("ExactNumFlag", false, "Exact number of samples", "If requested it will try to do a dicotomic search for the best poisson disk radius that will generate the requested number of samples with a tolerance of the 0.5%. Obviously it takes much longer."));
    break;

  case FP_POISSONDISK_SAMPLING :
    parlst.addParam(new RichInt("SampleNum", 1000, "Number of samples", "The desired number of samples. The ray of the disk is calculated according to the sampling density."));
    parlst.addParam(new RichAbsPerc("Radius", 0, 0, md.mm()->cm.bbox.Diag(), "Explicit Radius", "If not zero this parameter override the previous parameter to allow exact radius specification"));
    parlst.addParam(new RichInt("MontecarloRate", 20, "MonterCarlo OverSampling", "The over-sampling rate that is used to generate the intial Montecarlo samples (e.g. if this parameter is <i>K</i> means that<i>K</i> x <i>poisson sample</i> points will be used). The generated Poisson-disk samples are a subset of these initial Montecarlo samples. Larger this number slows the process but make it a bit more accurate."));
    parlst.addParam(new RichBool("SaveMontecarlo", false, "Save Montecarlo", "If true, it will generate an additional Layer with the montecarlo sampling that was pruned to build the poisson distribution."));
    parlst.addParam(new RichBool("ApproximateGeodesicDistance", false, "Approximate Geodesic Distance", "If true Poisson Disc distances are computed using an approximate geodesic distance, e.g. an euclidean distance weighted by a function of the difference between the normals of the two points."));
    parlst.addParam(new RichBool("Subsample", false, "Base Mesh Subsampling", "If true the original vertices of the base mesh are used as base set of points. In this case the SampleNum should be obviously much smaller than the original vertex number.<br>Note that this option is very useful in the case you want to subsample a dense point cloud."));
    parlst.addParam(new RichBool("RefineFlag", false, "Refine Existing Samples", "If true the vertices of the below mesh are used as starting vertices, and they will utterly refined by adding more and more points until possible. "));
    parlst.addParam(new RichMesh("RefineMesh", md.mm(),&md, "Samples to be refined", "Used only if the above option is checked. "));
    parlst.addParam(new RichBool("BestSampleFlag", true, "Best Sample Heuristic", "If true it will use a simple heuristic for choosing the samples. At a small cost (it can slow a bit the process) it usually improve the maximality of the generated sampling. "));
    parlst.addParam(new RichInt("BestSamplePool", 10, "Best Sample Pool Size", "Used only if the Best Sample Flag is true. It control the number of attempt that it makes to get the best sample. It is reasonable that it is smaller than the Montecarlo oversampling factor."));
    parlst.addParam(new RichBool("ExactNumFlag", false, "Exact number of samples", "If requested it will try to do a dicotomic search for the best poisson disk radius that will generate the requested number of samples with a tolerance of the 0.5%. Obviously it takes much longer."));
    parlst.addParam(new RichFloat("RadiusVariance", 1, "Radius Variance", "The radius of the disk is allowed to vary between r and r*var. If this parameter is 1 the sampling is the same of the Poisson Disk Sampling"));
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

  case FP_HAUSDORFF_DISTANCE:
  {
		MeshModel *vertexMesh = md.mm();
		foreach(vertexMesh, md.meshList)
		if (vertexMesh != md.mm())  break;

		parlst.addParam(new RichMesh("SampledMesh", md.mm(), &md, "Sampled Mesh",
			"The mesh whose surface is sampled. For each sample we search the closest point on the Target Mesh."));
		parlst.addParam(new RichMesh("TargetMesh", vertexMesh, &md, "Target Mesh",
			"The mesh that is sampled for the comparison."));
		parlst.addParam(new RichBool("SaveSample", false, "Save Samples",
			"Save the position and distance of all the used samples on both the two surfaces, creating two new layers with two point clouds representing the used samples."));
		parlst.addParam(new RichBool("SampleVert", true, "Sample Vertexes",
			"For the search of maxima it is useful to sample vertices and edges of the mesh with a greater care. "
			"It is quite probably the the farthest points falls along edges or on mesh vertexes, and with uniform montecarlo sampling approaches"
			"the probability of taking a sample over a vertex or an edge is theoretically null.<br>"
			"On the other hand this kind of sampling could make the overall sampling distribution slightly biased and slightly affects the cumulative results."));
		parlst.addParam(new RichBool("SampleEdge", false, "Sample Edges", "See the above comment."));
		parlst.addParam(new RichBool("SampleFauxEdge", false, "Sample FauxEdge", "See the above comment."));
		parlst.addParam(new RichBool("SampleFace", false, "Sample Faces", "See the above comment."));
		parlst.addParam(new RichInt("SampleNum", md.mm()->cm.vn, "Number of samples",
			"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt."));
		parlst.addParam(new RichAbsPerc("MaxDist", md.mm()->cm.bbox.Diag() / 2.0, 0.0f, md.bbox().Diag(),
			tr("Max Distance"), tr("Sample points for which we do not find anything whithin this distance are rejected and not considered neither for averaging nor for max.")));
  } break;

  case FP_DISTANCE_REFERENCE:
  {
		MeshModel *vertexMesh = md.mm();
		foreach(vertexMesh, md.meshList)
		if (vertexMesh != md.mm())  break;

		parlst.addParam(new RichMesh("MeasureMesh", md.mm(), &md, "Measured Mesh/PointCloud",
			"The Mesh/Pointcloud that is measured, vertex by vertex, computing distance from the REFERENCE mesh/pointcloud."));
		parlst.addParam(new RichMesh("RefMesh", vertexMesh, &md, "Reference Mesh/PointCloud",
			"The Mesh/Pointcloud that is used as a reference, to measure distance from."));

		parlst.addParam(new RichBool("SignedDist", true, "Compute Signed Distance",
			"If TRUE, the distance is signed; if FALSE, it will compute the distance absolute value."));

		parlst.addParam(new RichAbsPerc("MaxDist", md.mm()->cm.bbox.Diag(), 0.0f, md.bbox().Diag(),
			tr("Max Distance [abs]"), tr("Search is interrupted when nothing is found within this distance range [+maxDistance -maxDistance].")));
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
	parlst.addParam(new RichBool ("onSelected", false, "Only on selection",	"If checked, only transfer to selected vertices on TARGET mesh"));

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

    parlst.addParam(new RichBool ("multisample", false, "Multi-sample",
                                  "If true the distance field is more accurately compute by multisampling the volume (7 sample for each voxel). Much slower but less artifacts."));
    parlst.addParam(new RichBool ("absDist", false, "Absolute Distance",
                                  "If true a <b> not</b> signed distance field is computed. "
                                  "In this case you have to choose a not zero Offset and a double surface is built around the original surface, inside and outside. "
                                  "Is useful to convrt thin floating surfaces into <i> solid, thick meshes.</i>. t"));
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
		MeshModel *curMM = md.mm();
		if (par.getInt("SampleNum") == 0) {
			Log("Mesh Element Sampling: Number of Samples is  0, cannot do anything");
			errorMessage = "Number of Samples is  0, cannot do anything";
			return false; // can't continue, mesh can't be processed
		}
		if ((par.getEnum("Sampling")>0) && (curMM->cm.fn == 0)) {
			Log("Mesh Element Sampling: cannot sample on faces/edges, mesh has no faces");
			errorMessage = "Mesh Element Sampling: cannot sample on faces/edges, mesh has no faces";
			return false; // can't continue, mesh can't be processed
		}

		MeshModel *mm= md.addNewMesh("", "Element samples", true); // The new mesh is the current one
		mm->updateDataMask(curMM);

		BaseSampler mps(&(mm->cm));

		switch(par.getEnum("Sampling"))
		{
			case 0 :	tri::SurfaceSampling<CMeshO,BaseSampler>::VertexUniform(curMM->cm,mps,par.getInt("SampleNum"));	break;
			case 1 :	tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeUniform(curMM->cm,mps,par.getInt("SampleNum"),true);		break;
			case 2 :	tri::SurfaceSampling<CMeshO,BaseSampler>::AllFace(curMM->cm,mps);		break;
		}
		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		Log("Mesh Element Sampling created a new mesh of %i points",mm->cm.vn);
	} break;

	case FP_TEXEL_SAMPLING :
	{
		MeshModel *curMM= md.mm();
		if (!tri::HasPerWedgeTexCoord(curMM->cm)) {
			Log("Texel Sampling requires a mesh with Per Wedge UV parametrization");
			errorMessage = "Texel Sampling requires a mesh with Per Wedge UV parametrization";
			return false; // can't continue, mesh can't be processed
		}

		MeshModel *mm= md.addNewMesh("", "Texel samples", true); // The new mesh is the current one
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
		Log("Texel Sampling created a new mesh of %i points", mm->cm.vn);
	} break;

	case FP_MONTECARLO_SAMPLING :
	{
		MeshModel *curMM = md.mm();
		if (curMM->cm.fn == 0) {
			Log("Montecarlo Sampling requires a mesh with faces, it does not work on Point Clouds");
			errorMessage = "Montecarlo Sampling requires a mesh with faces,<br> it does not work on Point Clouds";
			return false; // can't continue, mesh can't be processed
		}
		if (par.getInt("SampleNum") == 0) {
			Log("Montecarlo Sampling: Number of Samples is 0, cannot do anything");
			errorMessage = "Number of Samples is 0, cannot do anything";
			return false; // can't continue, mesh can't be processed
		}
		if (par.getBool("Weighted") && !curMM->hasDataMask(MeshModel::MM_VERTQUALITY)) {
			Log("Montecarlo Sampling: cannot do weighted samplimg, layer has no Vertex Quality value");
			errorMessage = "Cannot do weighted samplimg, layer has no Vertex Quality value";
			return false; // can't continue, mesh can't be processed
		}

		MeshModel *mm= md.addNewMesh("","Montecarlo Samples", true); // The new mesh is the current one
		mm->updateDataMask(curMM);
		BaseSampler mps(&(mm->cm));

    mps.perFaceNormal = par.getBool("PerFaceNormal");

		if(par.getBool("EdgeSampling"))
		{
			tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeMontecarlo(curMM->cm,mps,par.getInt("SampleNum"),false);
		}
		else
		{
			if(par.getBool("Weighted"))
				tri::SurfaceSampling<CMeshO,BaseSampler>::WeightedMontecarlo(curMM->cm,mps,par.getInt("SampleNum"),par.getFloat("RadiusVariance"));
			else if(par.getBool("ExactNum")) 
				tri::SurfaceSampling<CMeshO,BaseSampler>::Montecarlo(curMM->cm,mps,par.getInt("SampleNum"));
			else 
				tri::SurfaceSampling<CMeshO,BaseSampler>::MontecarloPoisson(curMM->cm,mps,par.getInt("SampleNum"));
		}

		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		Log("Sampling created a new mesh of %i points", mm->cm.vn);
	} break;

	case FP_STRATIFIED_SAMPLING :
	{
		MeshModel *curMM = md.mm();
		if (curMM->cm.fn == 0) {
			Log("Stratified Sampling requires a mesh with faces, it does not work on Point Clouds");
			errorMessage = "Stratified Sampling requires a mesh with faces,<br> it does not work on Point Clouds";
			return false; // can't continue, mesh can't be processed
		}
		if (par.getInt("SampleNum") == 0) {
			Log("Stratified Sampling: Number of Samples is  0, cannot do anything");
			errorMessage = "Number of Samples is  0, cannot do anything";
			return false; // can't continue, mesh can't be processed
		}

		MeshModel *mm= md.addNewMesh("","Subdiv Samples", true); // The new mesh is the current one
		mm->updateDataMask(curMM);
		int samplingMethod = par.getEnum("Sampling");
		BaseSampler mps(&(mm->cm));
		switch(samplingMethod)
		{
			case 0:
				tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSimilar(curMM->cm,mps,par.getInt("SampleNum"), false ,par.getBool("Random"));
				Log("Similar Sampling created a new mesh of %i points", mm->cm.vn);
				break;
			case 1:
				tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSimilar(curMM->cm,mps,par.getInt("SampleNum"), true ,par.getBool("Random"));
				Log("Dual Similar Sampling created a new mesh of %i points", mm->cm.vn);
				break;
			case 2:	
				tri::SurfaceSampling<CMeshO,BaseSampler>::FaceSubdivision(curMM->cm,mps,par.getInt("SampleNum"), par.getBool("Random"));
				Log("Subdivision Sampling created a new mesh of %i points", mm->cm.vn);
				break;
			case 3:
				tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeUniform(curMM->cm,mps,par.getInt("SampleNum"), true);
				Log("Edge Sampling created a new mesh of %i points", mm->cm.vn);
				break;
			case 4:	
				tri::SurfaceSampling<CMeshO,BaseSampler>::EdgeUniform(curMM->cm,mps,par.getInt("SampleNum"), false);
				Log("Non Faux Edge Sampling created a new mesh of %i points", mm->cm.vn);
				break;
		}
		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
	} break;

	case FP_CLUSTERED_SAMPLING :
	{
		MeshModel *curMM= md.mm();
		int samplingMethod = par.getEnum("Sampling");
		float threshold = par.getAbsPerc("Threshold");
		bool selected = par.getBool("Selected");

		if (selected && curMM->cm.svn == 0 && curMM->cm.sfn == 0) // if no selection at all, fail
		{
			Log("Clustered Sampling: Cannot apply only on selection: there is no selection");
			errorMessage = "Cannot apply only on selection: there is no selection";
			return false;
		}
		if (selected && (curMM->cm.svn == 0 && curMM->cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
		{
			tri::UpdateSelection<CMeshO>::VertexClear(curMM->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(curMM->cm);
		}
		Log("Using only %i selected vertices", curMM->cm.svn);

		MeshModel *mm= md.addNewMesh("", "Cluster samples", true); // The new mesh is the current one

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
				Log("Similar Sampling created a new mesh of %i points", mm->cm.vn);
			} break;

			case 1 :
			{
				vcg::tri::Clustering<CMeshO, vcg::tri::NearestToCenter<CMeshO> > ClusteringGrid;
				ClusteringGrid.Init(curMM->cm.bbox,100000,threshold);
				ClusteringGrid.AddPointSet(curMM->cm,selected);
				ClusteringGrid.SelectPointSet(curMM->cm);
				tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(curMM->cm);
				ClusteringGrid.ExtractPointSet(mm->cm);
				Log("Similar Sampling created a new mesh of %i points", mm->cm.vn);
			} break;
		}
		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
	} break;

	case FP_POINTCLOUD_SIMPLIFICATION :
	{
		MeshModel *curMM= md.mm();
		CMeshO::ScalarType radius = par.getAbsPerc("Radius");
		int sampleNum = par.getInt("SampleNum");

		if ((radius == 0.0) && (sampleNum == 0)){
			Log("Point Cloud Simplification: Number of Samples AND Radius are both 0, cannot do anything");
			errorMessage = "Number of Samples AND Radius are both 0, cannot do anything";
			return false; // can't continue, mesh can't be processed
		}

		MeshModel *mm= md.addNewMesh("", "Simplified cloud", true); // The new mesh is the current one
		mm->updateDataMask(curMM);
		BaseSampler mps(&(mm->cm));
		tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskParam pp;
		
		if(radius==0) 
			radius = tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonDiskRadius(curMM->cm,sampleNum);
		else 
			sampleNum = tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonSampleNum(curMM->cm,radius);

		if(par.getBool("ExactNumFlag") && radius==0)
			tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskPruningByNumber(mps, curMM->cm, sampleNum, radius,pp,0.005);
		else
			tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskPruning(mps, curMM->cm, radius,pp);

		Log("Point Cloud Simplification created a new mesh of %i points", mm->cm.vn);
		UpdateBounding<CMeshO>::Box(mm->cm);
	} break;

	case FP_POISSONDISK_SAMPLING :
	{
		MeshModel *curMM= md.mm();
		CMeshO::ScalarType radius = par.getAbsPerc("Radius");
		int sampleNum = par.getInt("SampleNum");
		tri::SurfaceSampling<CMeshO, BaseSampler>::PoissonDiskParam pp;
		pp.radiusVariance = par.getFloat("RadiusVariance");
		bool subsampleFlag = par.getBool("Subsample");

		if ((radius == 0.0) && (sampleNum == 0)){
			Log("Poisson disk Sampling: Number of Samples AND Radius are both 0, cannot do anything");
			errorMessage = "Number of Samples AND Radius are both 0, cannot do anything";
			return false; // can't continue, mesh can't be processed
		}

		if (radius == 0)
			radius = tri::SurfaceSampling<CMeshO, BaseSampler>::ComputePoissonDiskRadius(curMM->cm, sampleNum);
		else
			sampleNum = tri::SurfaceSampling<CMeshO, BaseSampler>::ComputePoissonSampleNum(curMM->cm, radius);

		if (pp.radiusVariance != 1.0)
		{
			if (!curMM->hasDataMask(MeshModel::MM_VERTQUALITY)) {
				Log("Poisson disk Sampling: Variable radius requires per-Vertex quality for biasing the distribution");
				errorMessage = "Variable radius requires per-Vertex Quality for biasing the distribution";
				return false; // cannot continue
			}
			pp.adaptiveRadiusFlag = true;
			Log("Variable Density variance is %f, radius can vary from %f to %f", pp.radiusVariance, radius / pp.radiusVariance, radius*pp.radiusVariance);
		}

		if (curMM->cm.fn == 0 && subsampleFlag == false)
		{
			Log("Poisson disk Sampling: Current mesh has no triangles. We cannot create a montecarlo sampling of the surface. Please select the Subsample flag");
			errorMessage = "Current mesh has no triangles. We cannot create a montecarlo sampling of the surface.<br> Please select the Subsample flag";
			return false; // cannot continue
		}

		MeshModel *mm= md.addNewMesh("","Poisson-disk Samples", true); // The new mesh is the current one
		mm->updateDataMask(curMM);

		Log("Computing %i Poisson Samples for an expected radius of %f",sampleNum,radius);

		// first of all generate montecarlo samples for fast lookup
		CMeshO *presampledMesh=0;

		CMeshO MontecarloMesh; // this mesh is used only if we need real poisson sampling (and therefore we need to choose points different from the starting mesh vertices)
		if(subsampleFlag)
			presampledMesh = &(curMM->cm);
		else
		{
			MeshModel *mcm;
			if(par.getBool("SaveMontecarlo"))
			{
				mcm = md.addNewMesh("", "Montecarlo Samples", false); // the new mesh is NOT the current one (byproduct of sampling)
				presampledMesh=&(mcm->cm);
			}
			else
				presampledMesh=&MontecarloMesh;

			QTime tt;tt.start();
			BaseSampler sampler(presampledMesh);
			sampler.qualitySampling=true;
			if(pp.adaptiveRadiusFlag)
				tri::SurfaceSampling<CMeshO,BaseSampler>::WeightedMontecarlo(curMM->cm, sampler, sampleNum*par.getInt("MontecarloRate"),pp.radiusVariance);
			else
				tri::SurfaceSampling<CMeshO,BaseSampler>::Montecarlo(curMM->cm, sampler, sampleNum*par.getInt("MontecarloRate"));
			presampledMesh->bbox = curMM->cm.bbox; // we want the same bounding box
			Log("Generated %i Montecarlo Samples (%i msec)",presampledMesh->vn,tt.elapsed());
		}

		BaseSampler mps(&(mm->cm));
		if(par.getBool("RefineFlag"))
		{
			pp.preGenFlag=true;
			pp.preGenMesh=&(par.getMesh("RefineMesh")->cm);
		}
		pp.geodesicDistanceFlag=par.getBool("ApproximateGeodesicDistance");
		pp.bestSampleChoiceFlag=par.getBool("BestSampleFlag");
		pp.bestSamplePoolSize =par.getInt("BestSamplePool");
		if(par.getBool("ExactNumFlag"))
			tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskPruningByNumber(mps, *presampledMesh, sampleNum, radius,pp,0.005);
		else
			tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskPruning(mps, *presampledMesh, radius,pp);

		//tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDisk(curMM->cm, mps, *presampledMesh, radius,pp);
		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		Point3i &g=pp.pds.gridSize;
		Log("Grid size was %i %i %i (%i allocated on %i)",g[0],g[1],g[2], pp.pds.gridCellNum, g[0]*g[1]*g[2]);
		Log("Poisson Disk Sampling created a new mesh of %i points", mm->cm.vn);
	} break;

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

		if (mm0 == mm1){
			Log("Hausdorff Distance: cannot compute, it is the same mesh");
			errorMessage = "Cannot compute, it is the same mesh";
			return false; // can't continue, mesh can't be processed
		}

		if(sampleEdge && mm0->cm.fn==0) {
			Log("Disabled edge sampling. Meaningless when sampling point clouds");
			sampleEdge=false;
		}
		if(sampleFace && mm0->cm.fn==0) {
			Log("Disabled face sampling. Meaningless when sampling point clouds");
			sampleFace=false;
		}

		// the meshes have to be transformed
		if (mm0->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm0->cm, mm0->cm.Tr, true);
		if (mm1->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm1->cm, mm1->cm.Tr, true);

		mm0->updateDataMask(MeshModel::MM_VERTQUALITY);
		mm1->updateDataMask(MeshModel::MM_VERTQUALITY);
		mm1->updateDataMask(MeshModel::MM_FACEMARK);
		tri::UpdateNormal<CMeshO>::PerFaceNormalized(mm1->cm);

		MeshModel *samplePtMesh =0;
		MeshModel *closestPtMesh =0;
		HausdorffSampler<CMeshO> hs(&(mm1->cm));
		if(saveSampleFlag)
		{
		  closestPtMesh=md.addNewMesh("","Hausdorff Closest Points", false); // the new mesh is NOT the current one (byproduct of measurement)
		  closestPtMesh->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
		  samplePtMesh = md.addNewMesh("", "Hausdorff Sample Point", false); // the new mesh is NOT the current one (byproduct of measurement)
		  samplePtMesh->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
		  hs.init(&(samplePtMesh->cm),&(closestPtMesh->cm));
		}

		hs.dist_upper_bound = distUpperBound;

		qDebug("Sampled  mesh has %7i vert %7i face",mm0->cm.vn,mm0->cm.fn);
		qDebug("Searched mesh has %7i vert %7i face",mm1->cm.vn,mm1->cm.fn);
		qDebug("Max sampling distance %f on a bbox diag of %f",distUpperBound,mm1->cm.bbox.Diag());

		if(sampleVert)
		  tri::SurfaceSampling<CMeshO,HausdorffSampler<CMeshO> >::VertexUniform(mm0->cm,hs,par.getInt("SampleNum"));
		if(sampleEdge)
		  tri::SurfaceSampling<CMeshO,HausdorffSampler<CMeshO> >::EdgeUniform(mm0->cm,hs,par.getInt("SampleNum"),sampleFauxEdge);
		if(sampleFace)
		  tri::SurfaceSampling<CMeshO,HausdorffSampler<CMeshO> >::Montecarlo(mm0->cm,hs,par.getInt("SampleNum"));

		// the meshes have to return to their original position
		if (mm0->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm0->cm, Inverse(mm0->cm.Tr), true);
		if (mm1->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm1->cm, Inverse(mm1->cm.Tr), true);

		Log("Hausdorff Distance computed");
		Log("     Sampled %i pts (rng: 0) on %s searched closest on %s",hs.n_total_samples,qUtf8Printable(mm0->label()),qUtf8Printable(mm1->label()));
		Log("     min : %f   max %f   mean : %f   RMS : %f",hs.getMinDist(),hs.getMaxDist(),hs.getMeanDist(),hs.getRMSDist());
		float d = mm0->cm.bbox.Diag();
		Log("Values w.r.t. BBox Diag (%f)",d);
		Log("     min : %f   max %f   mean : %f   RMS : %f\n",hs.getMinDist()/d,hs.getMaxDist()/d,hs.getMeanDist()/d,hs.getRMSDist()/d);


		if(saveSampleFlag)
		{
		  tri::UpdateBounding<CMeshO>::Box(samplePtMesh->cm);
		  tri::UpdateBounding<CMeshO>::Box(closestPtMesh->cm);

		  tri::UpdateColor<CMeshO>::PerVertexQualityRamp(samplePtMesh->cm);
		  tri::UpdateColor<CMeshO>::PerVertexQualityRamp(closestPtMesh->cm);
		}
	} break;

	case FP_DISTANCE_REFERENCE:
	{
		MeshModel* mm0 = par.getMesh("MeasureMesh");  // this mesh gets measured.
		MeshModel* mm1 = par.getMesh("RefMesh");      // this is the reference mesh
		bool useSigned = par.getBool("SignedDist");
		float maxDistABS = par.getAbsPerc("MaxDist");

		if (mm0 == mm1){
			Log("Distance from Reference: cannot compute, it is the same mesh");
			errorMessage = "Cannot compute, it is the same mesh";
			return false; // can't continue, mesh can't be processed
		}

		// the meshes have to return to their original position
		if (mm0->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm0->cm, mm0->cm.Tr, true);
		if (mm1->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm1->cm, mm1->cm.Tr, true);

		// add quality to vertex of measured mesh
		mm0->updateDataMask(MeshModel::MM_VERTQUALITY);
		// if reference has faces, recompute and normalize normals
		if (mm1->cm.fn > 0)
		{
			tri::UpdateNormal<CMeshO>::PerFaceNormalized(mm1->cm);
			tri::UpdateNormal<CMeshO>::PerVertexNormalized(mm1->cm);
		}
			mm1->updateDataMask(MeshModel::MM_FACEMARK);

		SimpleDistanceSampler ds(&(mm1->cm), useSigned, maxDistABS);

		tri::SurfaceSampling<CMeshO, SimpleDistanceSampler>::AllVertex(mm0->cm, ds);

		// the meshes have to return to their original position
		if (mm0->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm0->cm, Inverse(mm0->cm.Tr), true);
		if (mm1->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(mm1->cm, Inverse(mm1->cm.Tr), true);

		Log("Distance from Reference Mesh computed");
		Log("     Sampled %i vertices on %s searched closest on %s", mm0->cm.vn, qUtf8Printable(mm0->label()), qUtf8Printable(mm1->label()));
		Log("     min : %f   max %f   mean : %f   RMS : %f", ds.getMaxDist(), ds.getMaxDist(), ds.getMeanDist(), ds.getRMSDist());

	} break;

	case FP_VERTEX_RESAMPLING :
	{
		MeshModel* srcMesh = par.getMesh("SourceMesh"); // mesh whose attribute are read
		MeshModel* trgMesh = par.getMesh("TargetMesh"); // this whose surface is sought for the closest point to each sample.
		float upperbound = par.getAbsPerc("UpperBound"); // maximum distance to stop search
		bool onlySelected = par.getBool("onSelected");
		bool colorT = par.getBool("ColorTransfer");
		bool geomT = par.getBool("GeomTransfer");
		bool normalT = par.getBool("NormalTransfer");
		bool qualityT = par.getBool("QualityTransfer");
		bool selectionT = par.getBool("SelectionTransfer");
		bool distquality = par.getBool("QualityDistance");

		if (srcMesh == trgMesh){
			Log("Vertex Attribute Transfer: cannot compute, it is the same mesh");
			errorMessage = "Cannot compute, it is the same mesh";
			return false; // can't continue, mesh can't be processed
		}
		if (!colorT && !geomT && !qualityT && !normalT && !selectionT)
		{
			Log("Vertex Attribute Transfer: you have to choose at least one attribute to be sampled");
			errorMessage = QString("You have to choose at least one attribute to be sampled");
			return false;
		}

		if (onlySelected && trgMesh->cm.svn == 0 && trgMesh->cm.sfn == 0) // if no selection at all, fail
		{
			Log("Vertex Attribute Transfer: Cannot apply only on selection: there is no selection");
			errorMessage = "Cannot apply only on selection: there is no selection";
			return false;
		}
		if (onlySelected && (trgMesh->cm.svn == 0 && trgMesh->cm.sfn > 0)) // if no vert selected, but some faces selected, use their vertices
		{
			tri::UpdateSelection<CMeshO>::VertexClear(trgMesh->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(trgMesh->cm);
		}

		// the meshes have to be transformed
		if (srcMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(srcMesh->cm, srcMesh->cm.Tr, true);
		if (trgMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(trgMesh->cm, trgMesh->cm.Tr, true);
	
		srcMesh->updateDataMask(MeshModel::MM_FACEMARK);
		tri::UpdateNormal<CMeshO>::PerFaceNormalized(srcMesh->cm);
	
		LocalRedetailSampler rs;
		rs.init(&(srcMesh->cm),cb,trgMesh->cm.vn);

		rs.dist_upper_bound = upperbound;
		rs.colorFlag = colorT;
		rs.coordFlag = geomT;
		rs.normalFlag = normalT;
		rs.qualityFlag = qualityT;
		rs.selectionFlag = selectionT;
		rs.storeDistanceAsQualityFlag = distquality;

		if(rs.colorFlag)   trgMesh->updateDataMask(MeshModel::MM_VERTCOLOR);
		if(rs.qualityFlag) trgMesh->updateDataMask(MeshModel::MM_VERTQUALITY);

		qDebug("Source  mesh has %7i vert %7i face",srcMesh->cm.vn,srcMesh->cm.fn);
		qDebug("Target  mesh has %7i vert %7i face",trgMesh->cm.vn,trgMesh->cm.fn);

		tri::SurfaceSampling<CMeshO, LocalRedetailSampler>::VertexUniform(trgMesh->cm, rs, trgMesh->cm.vn, onlySelected);

		if(rs.coordFlag) tri::UpdateNormal<CMeshO>::PerFaceNormalized(trgMesh->cm);

		// the meshes have to return to their original position
		if (srcMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(srcMesh->cm, Inverse(srcMesh->cm.Tr), true);
		if (trgMesh->cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(trgMesh->cm, Inverse(trgMesh->cm.Tr), true);

	} break;

	case FP_UNIFORM_MESH_RESAMPLING :
	{
		if (md.mm()->cm.fn==0) {
			Log("Uniform Mesh Resampling: requires a mesh with faces, it does not work on Point Clouds");
			errorMessage = "Uniform Mesh Resampling requires a mesh with faces,<br> it does not work on Point Clouds";
			return false; // can't continue, mesh can't be processed
		}

		CMeshO::ScalarType voxelSize = par.getAbsPerc("CellSize");
		float offsetThr = par.getAbsPerc("Offset");
		bool discretizeFlag = par.getBool("discretize");
		bool multiSampleFlag = par.getBool("multisample");
		bool absDistFlag = par.getBool("absDist");
		bool mergeCloseVert = par.getBool("mergeCloseVert");

		MeshModel *baseMesh= md.mm();
		MeshModel *offsetMesh = md.addNewMesh("", "Offset mesh", true); // the new mesh is the current one
		baseMesh->updateDataMask(MeshModel::MM_FACEMARK);

		Point3i volumeDim;
		Box3m volumeBox = baseMesh->cm.bbox;
		volumeBox.Offset(volumeBox.Diag()/10.0f+abs(offsetThr));
		BestDim(volumeBox , voxelSize, volumeDim );

		Log("Resampling mesh using a volume of %i x %i x %i",volumeDim[0],volumeDim[1],volumeDim[2]);
		Log("     VoxelSize is %f, offset is %f ", voxelSize,offsetThr);
		Log("     Mesh Box is %f %f %f",baseMesh->cm.bbox.DimX(),baseMesh->cm.bbox.DimY(),baseMesh->cm.bbox.DimZ() );

		tri::Resampler<CMeshO,CMeshO>::Resample(baseMesh->cm, offsetMesh->cm, volumeBox, volumeDim, voxelSize*3.5, offsetThr,discretizeFlag,multiSampleFlag,absDistFlag, cb);
		tri::UpdateBounding<CMeshO>::Box(offsetMesh->cm);
		if(mergeCloseVert)
		{
			float mergeThr =offsetMesh->cm.bbox.Diag()/10000.0f;
			int total = tri::Clean<CMeshO>::MergeCloseVertex(offsetMesh->cm,mergeThr);
			Log("Successfully merged %d vertices with a distance lower than %f", total,mergeThr);
		}
		tri::UpdateNormal<CMeshO>::PerVertexPerFace(offsetMesh->cm);
	} break;

	case FP_VORONOI_COLORING :
	{
		MeshModel* mmM = par.getMesh("ColoredMesh");  // surface where we choose the random samples
		MeshModel* mmV = par.getMesh("VertexMesh");   // surface that is sought for the closest point to each sample.
		bool backwardFlag = par.getBool("backward");

		tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmM->cm);
		tri::Allocator<CMeshO>::CompactVertexVector(mmM->cm);
		tri::Allocator<CMeshO>::CompactFaceVector(mmM->cm);
		mmM->updateDataMask(MeshModel::MM_VERTFACETOPO);
		vector<CMeshO::CoordType> vecP;
		// Fills the point vector with the position of the Point cloud
		for(CMeshO::VertexIterator vi= mmV->cm.vert.begin(); vi!= mmV->cm.vert.end(); ++vi) if(!(*vi).IsD())
			vecP.push_back((*vi).cP());

		vector<CMeshO::VertexPointer> vecV; // points to vertexes of ColoredMesh;
		tri::VoronoiProcessing<CMeshO>::SeedToVertexConversion	(mmM->cm, vecP, vecV);
		Log("Converted %ui points into %ui vertex ",vecP.size(),vecV.size());
		tri::EuclideanDistance<CMeshO> edFunc;
		tri::VoronoiProcessing<CMeshO>::ComputePerVertexSources(mmM->cm,vecV,edFunc);

		for(uint i=0;i<vecV.size();++i) vecV[i]->C()=Color4b::Red;
		tri::VoronoiProcessing<CMeshO>::VoronoiColoring(mmM->cm,backwardFlag);
	} break;

	case FP_DISK_COLORING :
	{
		MeshModel* mmM = par.getMesh("ColoredMesh");
		MeshModel* mmV = par.getMesh("VertexMesh");
		typedef vcg::SpatialHashTable<CMeshO::VertexType, CMeshO::ScalarType> SampleSHT;
		SampleSHT sht;
		tri::EmptyTMark<CMeshO> markerFunctor;
		typedef vcg::vertex::PointDistanceFunctor<float> VDistFunct;
		tri::UpdateColor<CMeshO>::PerVertexConstant(mmM->cm, Color4b::LightGray);
		tri::UpdateQuality<CMeshO>::VertexConstant(mmM->cm, std::numeric_limits<float>::max());
		bool approximateGeodeticFlag = par.getBool("ApproximateGeodetic");
		bool sampleRadiusFlag = par.getBool("SampleRadius");
		sht.Set(mmM->cm.vert.begin(),mmM->cm.vert.end());
		std::vector<CMeshO::VertexType*> closests;
		float radius = par.getDynamicFloat("Radius");

		for(CMeshO::VertexIterator viv = mmV->cm.vert.begin(); viv!= mmV->cm.vert.end(); ++viv) if(!(*viv).IsD())
		{
		  Point3m p = viv->cP();
		  if(sampleRadiusFlag) radius = viv->Q();
		  Box3m bb(p-Point3m(radius,radius,radius),p+Point3m(radius,radius,radius));
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
			Log("Regular Recursive Sampling: requires a mesh with faces, it does not work on Point Clouds");
			errorMessage = "Regular Recursive Sampling requires a mesh with  faces,<br> it does not work on Point Clouds";
			return false; // can't continue, mesh can't be processed
		}
		float CellSize = par.getAbsPerc("CellSize");
		float offset=par.getAbsPerc("Offset");

		MeshModel *mmM= md.mm();
		MeshModel *mm= md.addNewMesh("","Recursive Samples",true); // the new mesh is the current one

		tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmM->cm);
		tri::Allocator<CMeshO>::CompactEveryVector(mmM->cm);

		tri::UpdateNormal<CMeshO>::PerFaceNormalized(mmM->cm);
		std::vector<Point3m> pvec;

		tri::SurfaceSampling<CMeshO,LocalRedetailSampler>::RegularRecursiveOffset(mmM->cm,pvec, offset, CellSize);
		qDebug("Generated %i points",int(pvec.size()));
		tri::BuildMeshFromCoordVector(mm->cm,pvec);
	} break;

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
  case FP_DISTANCE_REFERENCE :
  case FP_ELEMENT_SUBSAMPLING    :
  case FP_MONTECARLO_SAMPLING :
  case FP_STRATIFIED_SAMPLING :
  case FP_CLUSTERED_SAMPLING :
  case FP_POISSONDISK_SAMPLING :
  case FP_REGULAR_RECURSIVE_SAMPLING :
  case FP_TEXEL_SAMPLING  :  return FilterDocSampling::Sampling;
  case FP_UNIFORM_MESH_RESAMPLING: return FilterDocSampling::Remeshing;
  case FP_DISK_COLORING:
  case FP_VORONOI_COLORING: return MeshFilterInterface::FilterClass(FilterDocSampling::Sampling | FilterDocSampling::VertexColoring);
  case FP_POINTCLOUD_SIMPLIFICATION : return MeshFilterInterface::FilterClass(FilterDocSampling::Sampling | FilterDocSampling::PointSet);
  default: assert(0);
  }
  return FilterClass(0);
}
int FilterDocSampling::postCondition( QAction* a ) const
{
	switch(ID(a)){
		case FP_VORONOI_COLORING    :
		case FP_DISK_COLORING       : return MeshModel::MM_VERTCOLOR;

		case FP_ELEMENT_SUBSAMPLING       :
		case FP_MONTECARLO_SAMPLING       :
		case FP_STRATIFIED_SAMPLING       :
		case FP_CLUSTERED_SAMPLING        :
		case FP_POINTCLOUD_SIMPLIFICATION :
		case FP_POISSONDISK_SAMPLING      : 
		case FP_TEXEL_SAMPLING            :			
		case FP_UNIFORM_MESH_RESAMPLING   : return MeshModel::MM_NONE;  // none, because they create a new layer, without affecting old one
  }
  return MeshModel::MM_ALL;
}

MeshFilterInterface::FILTER_ARITY FilterDocSampling::filterArity( QAction * filter ) const
{
    switch(ID(filter))
    {
    case FP_VERTEX_RESAMPLING :
    case FP_ELEMENT_SUBSAMPLING :
    case FP_MONTECARLO_SAMPLING :
    case FP_STRATIFIED_SAMPLING :
    case FP_CLUSTERED_SAMPLING :
    case FP_REGULAR_RECURSIVE_SAMPLING :
    case FP_UNIFORM_MESH_RESAMPLING:
    case FP_POINTCLOUD_SIMPLIFICATION :
        return MeshFilterInterface::SINGLE_MESH;
    case FP_DISTANCE_REFERENCE :
    case FP_HAUSDORFF_DISTANCE :
    case FP_POISSONDISK_SAMPLING :
    case FP_DISK_COLORING :
    case FP_VORONOI_COLORING :
        return MeshFilterInterface::FIXED;
    }
    return MeshFilterInterface::NONE;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterDocSampling)
