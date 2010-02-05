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


/* Standard includes */
#include <map>
#include <vector>

/* Local includes */
#include "filter_aging.h"
using namespace vcg;
/* VCG includes */
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/math/perlin_noise.h>


/* Constructor */
GeometryAgingPlugin::GeometryAgingPlugin() 
{ 
	typeList << FP_ERODE;
	
	foreach(FilterIDType tt, types())
		actionList << new QAction(filterName(tt), this);
}


/* Destructor */
GeometryAgingPlugin::~GeometryAgingPlugin() 
{ 
}

 MeshFilterInterface::FilterClass GeometryAgingPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_ERODE :           return MeshFilterInterface::Remeshing;
		default :  assert(0);			return MeshFilterInterface::Generic;
  }
}




/* Returns the very short string describing each filtering action */
 QString GeometryAgingPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_ERODE:
			return QString("Mesh aging and chipping simulation");
		default:
			assert(0);
	}
}


/* Returns the longer string describing each filtering action */
 QString GeometryAgingPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_ERODE: 
			return QString("Simulates the aging effects due to small collisions or various chipping events");
		default: 
			assert(0);
	}
}

/* Initializes the list of parameters (called by the auto dialog framework) */
void GeometryAgingPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &params)
{
	if( ID(action) != FP_ERODE)
	{
		assert (0);
		return;
	}
	bool hasQ = m.hasDataMask(MeshModel::MM_VERTQUALITY);
	std::pair<float,float> qRange(0.0, 0.0);;	// mesh quality range
	// retrieve mesh quality range
	if(hasQ) 
	{
		qRange = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
		if(qRange.second <= qRange.first) hasQ=false;
	}
		
		params.addParam(new RichBool("ComputeCurvature", !hasQ, "ReCompute quality from curvature", 
				"Compute per vertex quality values using mesh mean curvature <br>"
				"algorithm. In this way only the areas with higher curvature <br>"
				"will be eroded. If not checked, the quality values already <br>"
				"present over the mesh will be used."));
		params.addParam(new RichBool("SmoothQuality", false, "Smooth vertex quality", 
				"Smooth per vertex quality values. This allows to extend the <br>"
				"area affected by the erosion process."));
		params.addParam(new RichAbsPerc("QualityThreshold", qRange.first+(qRange.second-qRange.first)*0.66,
				qRange.first, qRange.second, "Min quality threshold",
				"Represents the minimum quality value two vertexes must have <br>"
				"to consider the edge they are sharing."));
		params.addParam(new RichAbsPerc("EdgeLenThreshold", m.cm.bbox.Diag()*0.02, 0,m.cm.bbox.Diag()*0.5,
				"Edge len threshold", 
				"The minimum length of an edge. Useful to avoid the creation of too many small faces."));
		params.addParam(new RichAbsPerc("ChipDepth", m.cm.bbox.Diag()*0.05, 0, m.cm.bbox.Diag(),
				"Max chip depth", "The maximum depth of a chip."));
		params.addParam(new RichInt("Octaves", 3, "Fractal Octaves", 
				"The number of octaves that are used in the generation of the <br>"
				"fractal noise using Perlin noise; reasonalble values are in the <br>"
				"1..8 range. Setting it to 1 means using a simple Perlin Noise."));
		params.addParam(new RichAbsPerc("NoiseFreqScale", m.cm.bbox.Diag()/10, 0, m.cm.bbox.Diag(), "Noise frequency scale",
				"Changes the noise frequency scale: this affects chip dimensions and <br>"
				"the distance between chips. The value denotes the average values <br>"
				"between two dents. Smaller number means small and frequent chips."));
		params.addParam(new RichFloat("NoiseClamp", 0.5, "Noise clamp threshold [0..1]",
				"All the noise values smaller than this parameter will be <br> "
				"considered as 0."));
		params.addParam(new RichFloat("DisplacementSteps", 10, "Displacement steps",
				"The whole displacement process is performed as a sequence of small <br>"
				"offsets applyed on each vertex. This parameter represents the number <br>"
				"of steps into which the displacement process will be splitted. <br>"
				"Useful to avoid the introduction of self intersections. <br>"
				"Bigger number means better accuracy."));
		params.addParam(new RichBool("Selected", m.cm.sfn>0, "Affect only selected faces", 
				"The aging procedure will be applied to the selected faces only."));
		params.addParam(new RichBool("StoreDisplacement", false, "Store erosion informations",
				"Select this option if you want to store the erosion informations <br>"
				"over the mesh. A new attribute will be added to each vertex <br>"
				"to contain the displacement offset applied to that vertex."));
}


/* The Real Core Function doing the actual mesh processing */
bool GeometryAgingPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &params, vcg::CallBackPos *cb)
{	
    MeshModel &m=*(md.mm());
    if( ID(filter) != FP_ERODE)
	{
		assert (0);
		return false;
	}

	m.updateDataMask(MeshModel::MM_VERTQUALITY);
	
	bool curvature = params.getBool("ComputeCurvature");
	if(curvature)	computeMeanCurvature(m.cm);
		
	// other plugin parameters
	bool smoothQ = params.getBool("SmoothQuality");
	float qualityTh = params.getAbsPerc("QualityThreshold");
	float edgeLenTh = params.getAbsPerc("EdgeLenThreshold");
	float chipDepth = params.getAbsPerc("ChipDepth");
	int octaves = params.getInt("Octaves");
	float noiseScale = params.getAbsPerc("NoiseFreqScale");
	float noiseClamp = params.getFloat("NoiseClamp");
	int dispSteps = (int)params.getFloat("DisplacementSteps");
	bool selected = params.getBool("Selected");
	bool storeDispl = params.getBool("StoreDisplacement");
	
	// error checking on parameters values
	if(edgeLenTh == 0.0) edgeLenTh = m.cm.bbox.Diag()*0.02;
	if(chipDepth == 0.0) chipDepth = m.cm.bbox.Diag()*0.05;
	noiseClamp = math::Clamp<float>(noiseClamp, 0.0, 1.0);
	
			// quality threshold percentage value
			std::pair<float, float> qRange = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
			float qperc = (qualityTh-qRange.first) / (qRange.second-qRange.first);
			
			// compute mesh quality, if requested
			if(curvature) {
				if(cb) (*cb)(0, "Computing quality values...");
				computeMeanCurvature(m.cm);
			}
			
			// eventually, smooth quality values
			if(smoothQ) tri::Smooth<CMeshO>::VertexQualityLaplacian(m.cm);
			
			// if quality values have been recomputed quality threshold may not 
			// be valid, so we recompute its absolute value using the percentage
			// value chosen by the user
			if(curvature || smoothQ) {
				qRange = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
				qualityTh = qRange.first + (qRange.second-qRange.first) * qperc;
			}
			
			// edge predicate
			QualityEdgePred ep = QualityEdgePred(selected, edgeLenTh, qualityTh);
			
			// refine needed edges
			refineMesh(m.cm, ep, selected, cb);
			
			// if requested, add erosion attribute to vertexes and initialize it
			if(storeDispl) {
				CMeshO::PerVertexAttributeHandle<Point3f> vah = 
						(tri::HasPerVertexAttribute(m.cm, "Erosion") ?
						 tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>(m.cm, "Erosion") :
						 tri::Allocator<CMeshO>::AddPerVertexAttribute<Point3f>(m.cm, std::string("Erosion")));
				for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); vi++)
					vah[vi] = Point3f(0.0, 0.0, 0.0);
			}
			CMeshO::PerVertexAttributeHandle<Point3f> vah = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>(m.cm, "Erosion");
			
			// vertexes along selection border will not be displaced 
			if(selected) tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
			
			// clear vertexes V bit (will be used to mark the vertexes as displaced) 
			tri::UpdateFlags<CMeshO>::VertexClearV(m.cm);
			
			// displace vertexes
			for(int i=0; i<dispSteps; i++) {
				GridStaticPtr<CFaceO, CMeshO::ScalarType> gM;
				gM.Set(m.cm.face.begin(), m.cm.face.end());
				
				if(cb) (*cb)( (i+1)*100/dispSteps, "Aging...");
				
				// blend toghether face normals and recompute vertex normal from these normals 
				// to get smoother offest directions
				tri::Smooth<CMeshO>::FaceNormalLaplacianFF(m.cm, 3); 
				tri::UpdateNormals<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
				tri::UpdateNormals<CMeshO>::NormalizeVertex(m.cm);
				
				for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); fi++) {
					if((*fi).IsD()) continue;
					for(int j=0; j<3; j++) {
						if(ep.qVertTest(face::Pos<CMeshO::FaceType>(&*fi,j))  &&
						   !(*fi).V(j)->IsV() &&		
						   (!selected || ((*fi).IsS() && (*fi).FFp(j)->IsS())) ) {
								double noise;						// noise value
								Point3f dispDir = (*fi).V(j)->N();	// displacement direction
								
								Point3f p = (*fi).V(j)->P() / noiseScale;
								noise = generateNoiseValue(octaves, p);
								// only values bigger than noiseClamp will be considered
								noise = (noise<noiseClamp?0.0:(noise-noiseClamp));
								
								// displacement offset
								Point3f offset = -(dispDir * chipDepth * noise) / dispSteps;
								
								(*fi).V(j)->P() += offset;
								if(faceIntersections(m.cm, face::Pos<CMeshO::FaceType>(&*fi,j), gM))
									(*fi).V(j)->P() -= offset;
								else if(storeDispl)	// store displacement
									vah[(*fi).V(j)] = vah[(*fi).V(j)] + offset;
								
								// mark as visited (displaced)
								(*fi).V(j)->SetV();
							}
					}
				}
				// clear vertexes V bit again
				tri::UpdateFlags<CMeshO>::VertexClearV(m.cm);
			}
			
			// update normals
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
			
			smoothPeaks(m.cm, selected, storeDispl);
			
			// readjust selection
			if(selected) tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
						
			return true;
}


/* Refines the mesh where needed.
 * In one cases we need to perform additional checks to avoid problems:
 * when we are working on the selection. */
void GeometryAgingPlugin::refineMesh(CMeshO &m, QualityEdgePred &ep, bool selection, vcg::CallBackPos *cb)
{
	bool ref = true;
	CMeshO::FaceIterator fi;
	
	// allocate selection user bit over faces
	ep.allocateSelBit();
	
	// clear sel user bit on all faces
	ep.clearSelBit(m);
	
	while(ref) {
		if(selection) {
			// set selbit bit on selected faces
			for(fi=m.face.begin(); fi!=m.face.end(); fi++)
				if(!(*fi).IsD() && (*fi).IsS()) ep.setFaceSelBit(&*fi);
			// dilate selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m);
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
		}
		
		ref = RefineE<CMeshO, MidPoint<CMeshO>, QualityEdgePred>(m, MidPoint<CMeshO>(&m), ep, selection, cb);
		if(ref) vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(m);
		
		if(selection) {
			// erode selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m);
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m);
		}
		
		// clear sel user bit on all faces
		ep.clearSelBit(m);
	}
	
	// delete the 2 user bits previously allocated
	ep.deallocateSelBit();
}


/* Returns a noise value in range [0,1] using the selected style */
double GeometryAgingPlugin::generateNoiseValue(int Octaves, const CVertexO::CoordType &p)
{
	double noise = .0;
	float freq = 1.0;
	for(int i=0; i<Octaves; i++) {
		noise += math::Perlin::Noise(freq*p.X(), freq*p.Y(), freq*p.Z()) / freq;
		freq *= 2;
	}
	// no negative values allowed (negative noise generates hills, not chips)	
	return fabs(noise);
}


/* Tests if the faces incident on the current vertex intersect some other face of the mesh */
bool GeometryAgingPlugin::faceIntersections(CMeshO &m, face::Pos<CMeshO::FaceType> p, GridStaticPtr<CFaceO, CMeshO::ScalarType> &gM)
{
	Box3<CMeshO::ScalarType> bbox;
	std::vector<CFaceO*> inBox;
	face::Pos<CMeshO::FaceType> start(p);
	
	// look for a border face (if any)
	do
		p.NextE();
	while(p != start && !p.IsBorder());
	start = p;
	
	do {
		// test current face intersections
		p.f->GetBBox(bbox);
		tri::GetInBoxFace(m, gM, bbox, inBox);
		std::vector<CFaceO*>::iterator fib;
		for(fib=inBox.begin(); fib!=inBox.end(); fib++)
			if(*fib != p.f)
				if(tri::Clean<CMeshO>::TestIntersection(p.f, *fib))
					return true;
		// move to the next face
		p.NextE();
	} while(p != start && !p.IsBorder());
	
	return false;
}


/* Smooths higher and thinner peaks (edges whose incident faces form an angle
 * greater than 150 degrees) */
void GeometryAgingPlugin::smoothPeaks(CMeshO &m, bool selected, bool updateErosionAttr)
{
	AngleEdgePred aep = AngleEdgePred(150.0);
	GridStaticPtr<CFaceO, CMeshO::ScalarType> gM;
	gM.Set(m.face.begin(), m.face.end());
	CMeshO::PerVertexAttributeHandle<Point3f> vah = 
		vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3f>(m, "Erosion");
	
	for(CMeshO::FaceIterator fi=m.face.begin(); fi!=m.face.end(); fi++) {
		if((*fi).IsD()) continue;
		for(int j=0; j<3; j++) {
			if(aep(face::Pos<CMeshO::FaceType>(&*fi,j)) && !(*fi).V(j)->IsV() &&
			   (!selected || ((*fi).IsS() && (*fi).FFp(j)->IsS())) ) {
					Point3f middlepos = Point3f(((*fi).V2(j)->P() + (*fi).FFp(j)->V2((*fi).FFi(j))->P()) / 2.0);
					Point3f oldpos = (*fi).V(j)->P();
					Point3f dirj = Point3f(((*fi).V(j)->P() - (*fi).V1(j)->P()) / 2.0);
					(*fi).V(j)->P() = middlepos + dirj;
					if(faceIntersections(m, face::Pos<CMeshO::FaceType>(&*fi,j), gM))
						(*fi).V(j)->P() = oldpos;
					else if(updateErosionAttr)		// update stored displacement
						vah[(*fi).V(j)] += ((*fi).V(j)->P() - oldpos);
					(*fi).V(j)->SetV();
			}
		}
	}
	
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m);
	
	// clear vertexes V bit again
	tri::UpdateFlags<CMeshO>::VertexClearV(m);
}


/* Compute per vertex quality values using mean curvature */
void GeometryAgingPlugin::computeMeanCurvature(CMeshO &m) 
{
	int delvert = tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
	if(delvert) Log(GLLogStream::FILTER, "Pre-Curvature Cleaning: Removed %d unreferenced vertices", delvert);
	tri::Allocator<CMeshO>::CompactVertexVector(m);
	tri::UpdateCurvature<CMeshO>::MeanAndGaussian(m);
	tri::UpdateQuality<CMeshO>::VertexFromMeanCurvature(m);
}


Q_EXPORT_PLUGIN(GeometryAgingPlugin)
