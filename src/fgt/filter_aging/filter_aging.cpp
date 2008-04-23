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
	// styles list (short descriptions)
	QStringList styles;
	styles.append("Simple     (1 function) ");
	styles.append("Linear     (8 functions)");
	styles.append("Sinusoidal (8 functions)");
	
	switch(ID(action)) {
		case FP_ERODE:
			params.addBool("UseQuality", m.cm.HasPerVertexQuality() && qRange.second>qRange.first, 
					"Use per vertex quality values", 
					"Use per vertex quality values to find the areas to erode.\n" \
					"Useful after applying ambient occlusion filter.");
			params.addAbsPerc("QualityThreshold", qRange.first+(qRange.second-qRange.first)*2/3, 
					qRange.first, qRange.second,  "Min quality threshold", 
					"When you decide to use per vertex quality values, \n" \
					"this parameter represents the minimum quality value \n" \
					"two vertexes must have to consider the edge they are sharing.");
			params.addFloat("AngleThreshold", 60.0, "Min angle threshold (deg)",
					"If you decide not to use per vertex quality values, \n" \
					"the angle between two adjacent faces will be considered.\n" \
					"This parameter represents the minimum angle between two adjacent\n" \
					"faces to consider the edge they are sharing.");
			params.addAbsPerc("EdgeLenThreshold", m.cm.bbox.Diag()*0.02, 0.0, 
					m.cm.bbox.Diag()*0.5,"Edge len threshold", 
					"The minimum length of an edge. Useful to avoid the creation of too many small faces.");
			params.addAbsPerc("ChipDepth", m.cm.bbox.Diag()*0.012, 0.0, m.cm.bbox.Diag()*0.05,
					"Max chip depth", "The maximum depth of a chip.");
			params.addEnum("ChipStyle", 0, styles, "Chip Style", 
					"Mesh erosion style to use. Different styles are defined " \
					"passing different parameters to the noise function \nthat generates displacement values.");
			params.addAbsPerc("NoiseFreqScale", m.cm.bbox.Diag()*0.055, m.cm.bbox.Diag()*0.01,
					m.cm.bbox.Diag()*0.1, "Noise frequency scale",
					"Changes the noise frequency scale. This affects chip dimensions" \
					"and the distance between chips (Linear and Sinusoidal styles only).");
			params.addFloat("NoiseClamp", 0.5, "Noise clamp threshold [0..1]",
					"All the noise values smaller than this parameter will be \n "\
					"considered as 0.");
			params.addFloat("DelIntersMaxIter", 8, "Max iterations to delete self intersections",
					"When eroding the mesh sometimes may happen that a face\n" \
					"intersects another area of the mesh, generating awful \n" \
					"artefacts. To avoid this, the vertexes of that face \n" \
					"will gradually be moved back to their original positions.\n" \
					"This parameter represents the maximum number of iterations\n" \
					"that the plugin can do to try to correct these errors (0 to\n" \
					"ignore the check).");
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
	float chipDepth = params.getAbsPerc("ChipDepth");
	int chipStyle = params.getEnum("ChipStyle");
	float noiseFreq = params.getAbsPerc("NoiseFreqScale");
	float noiseClamp = params.getFloat("NoiseClamp");
	float intersMaxIter = params.getFloat("DelIntersMaxIter");
	bool selected = params.getBool("Selected");
	
	// edge predicate
	AgingEdgePred ep = AgingEdgePred((useQuality?AgingEdgePred::QUALITY:AgingEdgePred::ANGLE), 
									 selected, edgeLenTreshold, thresholdValue);
	
	int fcount = 1;					// face counter (to update progress bar)
	
	typedef std::map<CVertexO*, Point3<CVertexO::ScalarType> > DispMap;
	DispMap displaced;					// displaced vertexes and their displacement offset 
	std::vector<CFaceO*> intersFace;	// self intersecting mesh faces (after displacement)
	
	srand(time(NULL));
	
	switch(ID(filter)) {
		case FP_ERODE:
			// refine needed edges
			refineMesh(m.cm, ep, selected, cb);
			
			// vertexes along selection border will not be displaced 
			if(selected) tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
			
			// clear vertexes V bit (will be used to mark the vertexes as displaced) 
			for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); vi++)
				(*vi).ClearV();
			
			// displace vertexes
			for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); fi++) {
				if(cb) (*cb)((fcount++/m.cm.fn)*100, "Aging..."); 
				if((*fi).IsD()) continue;
				for(int j=0; j<3; j++) {
					if(ep.qaVertTest(face::Pos<CMeshO::FaceType>(&*fi,j))  && 
					  !(*fi).V(j)->IsV() &&
					  (!selected || ((*fi).IsS() && (*fi).FFp(j)->IsS())) ) {
						double noise;							// noise value
						Point3<CVertexO::ScalarType> dispDir;	// displacement direction
						float angleFactor;		// angle multiply factor in [0,1] range
												// (greater angle -> deeper chip)
						
						noise = generateNoiseValue(chipStyle, noiseFreq, (*fi).V(j)->P().X(), (*fi).V(j)->P().Y(), (*fi).V(j)->P().Z());
						// only values bigger than noiseClamp will be considered
						noise = (noise<noiseClamp?0.0:noise);
						
						if((*fi).IsB(j)) {
							dispDir = Point3<CVertexO::ScalarType>((*fi).N().Normalize());
							angleFactor = 0.25;
						}
						else {
							dispDir = Point3<CVertexO::ScalarType>(((*fi).N() + (*fi).FFp(j)->N()).Normalize());
							angleFactor = (sin(vcg::Angle((*fi).N(), (*fi).FFp(j)->N()))/2 + 0.5);
						}
						
						// displacement offset
						Point3<CVertexO::ScalarType> offset = -(dispDir * chipDepth * noise * angleFactor);
						if(displaced.find((*fi).V(j)) == displaced.end())
							displaced.insert(std::pair<CVertexO*, Point3<CVertexO::ScalarType> >((*fi).V(j), (*fi).V(j)->P()));
						(*fi).V(j)->P() += offset;
						// mark as visited (displaced)
						(*fi).V(j)->SetV();
					}
				}
			}
			
			// readjust selection
			if(selected) tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
			
			// avoid mesh self intersections
			if(!displaced.empty()) {
				DispMap::iterator it;
				for(int t=0; t<intersMaxIter; t++) {
					intersFace.clear();
					if(cb) (*cb)((int)((t/intersMaxIter)*100), "Deleting mesh intersections...");
					tri::Clean<CMeshO>::SelfIntersections(m.cm, intersFace);
					if(intersFace.empty()) break;
					for(std::vector<CFaceO *>::iterator fpi=intersFace.begin(); fpi!=intersFace.end(); fpi++) {
						for(int i=0; i<3; i++) {
							it = displaced.find((*fpi)->V(i));
							if(it != displaced.end())
								// gradually move back the vertex to its original position
								(*fpi)->V(i)->P() = ((*fpi)->V(i)->P()+(*it).second)/2.0;
						}
					}
				}
				if(cb) (*cb)(100, "Deleting mesh intersections...");
			}
			
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
			
			// clear vertexes V bit again
			for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); vi++)
				(*vi).ClearV();
			
			return true;
		default:
			assert(0);
	}
}


/* Refines the mesh where needed.
 * In two cases we need to perform additional checks to avoid problems.
 * The first one is when we are working on the selection with a quality predicate 
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
		(*vi).ClearV();
	
	while(ref) {
		if(selection) {
			// set selbit bit on selected faces
			for(fi=m.face.begin(); fi!=m.face.end(); fi++)
				if(!(*fi).IsD() && (*fi).IsS()) (*fi).SetUserBit(ep.selbit);
			// dilate selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m);
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
		}
		// mark faces to refine setting angbit (angle predicate only)
		if(ep.getType()==AgingEdgePred::ANGLE)
			for(fi = m.face.begin(); fi!=m.face.end(); fi++) if(!(*fi).IsD())
				for(int j=0; j<3; j++)
					if(!selection || ((*fi).IsS() && (*fi).FFp(j)->IsS()))
						ep.markFaceAngle(face::Pos<CMeshO::FaceType>(&*fi,j));
		
		ref = RefineE<CMeshO, MidPoint<CMeshO>, AgingEdgePred>(m, MidPoint<CMeshO>(), ep, selection, cb);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m);
		
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
double GeometryAgingPlugin::generateNoiseValue(int style, float freqScale, const CVertexO::ScalarType &x, 
											const CVertexO::ScalarType &y, const CVertexO::ScalarType &z)
{
	double noise = .0;
	
	switch(style) {
		case SIMPLE:
			noise = math::Perlin::Noise(x, y, z);
			break;
		case LINEAR:
		case SINUSOIDAL:
			for(int i=0; i<8; i++) {
				float p = pow(freqScale, i);
				double tmp = math::Perlin::Noise(p*x, p*y, p*z) / p;
				tmp *= (tmp>=0.0?1.0:-1.0);
				noise += tmp;
			}
			break;
	}
	
	if(style == LINEAR)
		noise /= 8;
	else if(style == SINUSOIDAL)
		noise = sin(x + noise);
	
	// no negative values allowed (negative noise generates hills, not chips)
	noise *= (noise>=0.0?1.0:-1.0);
	
	return noise;
}


Q_EXPORT_PLUGIN(GeometryAgingPlugin)
