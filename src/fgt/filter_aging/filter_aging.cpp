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

/* VCG includes */
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/math/perlin_noise.h>
#include <vcg/complex/trimesh/clean.h>


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


/* Returns the very short string describing each filtering action */
const QString GeometryAgingPlugin::filterName(FilterIDType filterId) 
{
	switch(filterId) {
		case FP_ERODE:
			return QString("Aging Simulation");
		default:
			assert(0);
	}
}


/* Returns the longer string describing each filtering action */
const QString GeometryAgingPlugin::filterInfo(FilterIDType filterId) 
{
	switch(filterId) {
		case FP_ERODE: 
			return QString("Simulates the aging effects due to small collisions or various chipping events");
		default: 
			assert(0);
	}
}


/* Returns plugin info */
const PluginInfo &GeometryAgingPlugin::pluginInfo() 
{
	static PluginInfo ai;
	ai.Date = tr(__DATE__);
	ai.Version = tr("0.1");
	ai.Author = tr("Marco Fais");
	return ai;
 }


/* Initializes the list of parameters (called by the auto dialog framework) */
void GeometryAgingPlugin::initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &params)
{
	std::pair<float,float> qRange;
	// retrieve mesh quality range
	if(m.cm.HasPerVertexQuality())
		qRange = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
	else
		qRange = std::pair<float,float>(0.0, 0.0);
	
	switch(ID(action)) {
		case FP_ERODE:
			params.addBool("UseQuality", m.cm.HasPerVertexQuality() && qRange.second>qRange.first, 
					"Use per vertex quality values", 
					"Use per vertex quality values to find the areas to erode.\n"
					"Useful after applying ambient occlusion filter.");
			params.addAbsPerc("QualityThreshold", qRange.first+(qRange.second-qRange.first)*2/3, 
					qRange.first, qRange.second,  "Min quality threshold",
					"When you decide to use per vertex quality values, \n"
					"this parameter represents the minimum quality value \n"
					"two vertexes must have to consider the edge they are sharing.");
			params.addFloat("AngleThreshold", 60.0, "Min angle threshold (deg)",
					"If you decide not to use per vertex quality values, \n"
					"the angle between two adjacent faces will be considered. \n"
					"This parameter represents the minimum angle between two adjacent \n"
					"faces to consider the edge they are sharing.");
			params.addAbsPerc("EdgeLenThreshold", m.cm.bbox.Diag()*0.02, 0,m.cm.bbox.Diag()*0.5,
					"Edge len threshold", 
					"The minimum length of an edge. Useful to avoid the creation of too many small faces.");
			params.addAbsPerc("ChipDepth", m.cm.bbox.Diag()*0.05, 0, m.cm.bbox.Diag(),
					"Max chip depth", "The maximum depth of a chip.");
			params.addInt("Octaves", 3, "Fractal Octaves", 
					"The number of octaves that are used in the generation of the \n"
					"fractal noise using Perlin noise; reasonalble values are in the \n"
					"1..8 range. Setting it to 1 means using a simple Perlin Noise.");
			params.addAbsPerc("NoiseFreqScale", m.cm.bbox.Diag()/10, 0, m.cm.bbox.Diag(), "Noise frequency scale",
					"Changes the noise frequency scale: this affects chip dimensions and \n"
					"the distance between chips. The value denotes the average values \n"
					"between two dents. Smaller number means small and frequent chips.");
			params.addFloat("NoiseClamp", 0.5, "Noise clamp threshold [0..1]",
					"All the noise values smaller than this parameter will be \n "
					"considered as 0.");
			params.addFloat("DisplacementSteps", 10, "Displacement steps",
					"The whole displacement process is performed as a sequence of small \n"
					"offsets applyed on each vertex. This parameter represents the number \n"
					"of steps into which the displacement process will be splitted. \n"
					"Useful to avoid the introduction of self intersections. \n"
					"Bigger number means better accuracy.");
			params.addBool("Selected", m.cm.sfn>0, "Affect only selected faces", 
					"The aging procedure will be applied to the selected faces only.");
			break;
		default:
			assert(0);
	}
}


/* The Real Core Function doing the actual mesh processing */
bool GeometryAgingPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb)
{
	bool useQuality = params.getBool("UseQuality");
	
	if(useQuality && !m.cm.HasPerVertexQuality()) {
		errorMessage = QString("This mesh doesn't have per vertex quality informations.");
		return false;
	}
	
	float thresholdValue;
	if(useQuality)
		thresholdValue = params.getAbsPerc("QualityThreshold");
	else {
		thresholdValue = params.getFloat("AngleThreshold");
		while(thresholdValue >= 360.0) thresholdValue -= 360;
		while(thresholdValue < 0.0) thresholdValue += 360;
	}
	float edgeLenTreshold = params.getAbsPerc("EdgeLenThreshold");
	if(edgeLenTreshold == 0.0) edgeLenTreshold = m.cm.bbox.Diag()*0.02;
	float chipDepth = params.getAbsPerc("ChipDepth");
	if(chipDepth == 0.0) chipDepth = m.cm.bbox.Diag()*0.05;
	int octaves = params.getInt("Octaves");
	float noiseScale = params.getAbsPerc("NoiseFreqScale");
	float noiseClamp = params.getFloat("NoiseClamp");
	if(noiseClamp < 0.0) noiseClamp = 0.0;
	if(noiseClamp > 1.0) noiseClamp = 1.0;
	int dispSteps = (int)params.getFloat("DisplacementSteps");
	bool selected = params.getBool("Selected");
	
	// edge predicate
	AgingEdgePred ep = AgingEdgePred((useQuality?AgingEdgePred::QUALITY:AgingEdgePred::ANGLE), 
									 selected, edgeLenTreshold, thresholdValue);
	int fcount = 1;		// face counter (to update progress bar)
	
	switch(ID(filter)) {
		case FP_ERODE:
			// refine needed edges
			refineMesh(m.cm, ep, selected, cb);
			
			// vertexes along selection border will not be displaced 
			if(selected) tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
			
			// clear vertexes V bit (will be used to mark the vertexes as displaced) 
			for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); vi++)
				if(!(*vi).IsD()) (*vi).ClearV();
			
			// displace vertexes
			for(int i=0; i<dispSteps; i++) {
				GridStaticPtr<CFaceO, CMeshO::ScalarType> gM;
				gM.Set(m.cm.face.begin(), m.cm.face.end());
				
				for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); fi++) {
					if(cb) (*cb)(((fcount++/dispSteps)/m.cm.fn)*100, "Aging...");
					if((*fi).IsD()) continue;
					for(int j=0; j<3; j++) {
						if(ep.qaVertTest(face::Pos<CMeshO::FaceType>(&*fi,j))  &&
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

							// mark as visited (displaced)
							(*fi).V(j)->SetV();
						}
					}
				}
				// clear vertexes V bit again
				for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); vi++)
					if(!(*vi).IsD()) (*vi).ClearV();
				
				// update vertex normals
				vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
			}
			
			// readjust selection
			if(selected) tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
			
			// update all normals (face and vertex) before return
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
			
			return true;
		default:
			assert(0);
	}
}


/* Refines the mesh where needed.
 * In two cases we need to perform additional checks to avoid problems.
 * The first one is when we are working on the selection 
 * while the second one is when we are working with an angle predicate. */
void GeometryAgingPlugin::refineMesh(CMeshO &m, AgingEdgePred &ep, bool selection, vcg::CallBackPos *cb)
{
	bool ref = true;
	CMeshO::FaceIterator fi;
	
	// allocate 2 user bits over faces
	ep.allocateBits();
	
	// clear user bits on all faces
	for(fi = m.face.begin(); fi!=m.face.end(); fi++) if(!(*fi).IsD()) {
		(*fi).ClearUserBit(ep.selbit);
		(*fi).ClearUserBit(ep.angbit);
	}
	
	// clear vertexes V bit
	for(CMeshO::VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); vi++)
		if(!(*vi).IsD()) (*vi).ClearV();
	
	while(ref) {
		if(selection) {
			// set selbit bit on selected faces
			for(fi=m.face.begin(); fi!=m.face.end(); fi++)
				if(!(*fi).IsD() && (*fi).IsS()) (*fi).SetUserBit(ep.selbit);
			// dilate selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m);
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
		}
		// mark faces to refine, setting angbit (angle predicate only)
		if(ep.getType()==AgingEdgePred::ANGLE)
			for(fi = m.face.begin(); fi!=m.face.end(); fi++) if(!(*fi).IsD())
				for(int j=0; j<3; j++)
					if(!selection || ((*fi).IsS() && (*fi).FFp(j)->IsS()))
						ep.markFaceAngle(face::Pos<CMeshO::FaceType>(&*fi,j));
		
		ref = RefineE<CMeshO, MidPoint<CMeshO>, AgingEdgePred>(m, MidPoint<CMeshO>(), ep, selection, cb);
		if(ref) vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(m);
		
		if(selection) {
			// erode selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m);
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m);
		}
		
		// clear user bits on all faces
		for(fi=m.face.begin(); fi!=m.face.end(); fi++) if(!(*fi).IsD()) {
			(*fi).ClearUserBit(ep.selbit);
			(*fi).ClearUserBit(ep.angbit);
		}
	}
	
	// delete the 2 user bits previously allocated
	ep.deallocateBits();
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
		trimesh::GetInBoxFace(m, gM, bbox, inBox);
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


Q_EXPORT_PLUGIN(GeometryAgingPlugin)
