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
		qRange = std::pair<float,float>(0.0, 255.0);
	// styles list (short descriptions)
	QStringList styles;
	styles.append("Simple     (1 function) ");
	styles.append("Linear     (8 functions)");
	styles.append("Sinusoidal (8 functions)");
	
	switch(ID(action)) {
		case FP_ERODE:
			params.addBool("UseQuality", m.cm.HasPerVertexQuality(), "Use per vertex quality values", 
					"Use per vertex quality values to find the areas to erode.\n \
					Useful after applying ambient occlusion filter.");
			params.addAbsPerc("QualityThreshold", qRange.first+(qRange.second-qRange.first)*2/3, 
					qRange.first, qRange.second,  "Quality threshold", 
					"When you decide to use per vertex quality values, \n \
					this parameter represents the minimum quality value \n \
					two vertexes must have to consider the edge they are sharing.");
			params.addAbsPerc("AngleThreshold", 60.0, 10.0, 170.0,  "Angle threshold (deg)", 
					"If you decide not to use per vertex quality values, \n\
					the angle between two adjacent faces will be considered.\n \
					This parameter represents the minimum angle between two adjacent\n \
					faces to consider the edge they are sharing.");
			params.addAbsPerc("EdgeLenThreshold", m.cm.bbox.Diag()*0.02, 0.0, 
					m.cm.bbox.Diag()*0.5,"Edge len threshold", 
					"The minimum length of an edge. Useful to avoid the creation of too many small faces.");
			params.addAbsPerc("ChipDepth", m.cm.bbox.Diag()*0.004, 0.0, m.cm.bbox.Diag()*0.1,
					"Max chip depth", "The maximum depth of a chip.");
			params.addEnum("ChipStyle", 0, styles, "Chip Style", 
					"Mesh erosion style to use. Different styles are defined \
					passing different parameters to the noise function \nthat generates displacement values.");
			params.addAbsPerc("NoiseClamp", 0.0, 0.0, 1.0, "Noise clamp threshold",
					"All the noise values smaller than this parameters will be \n\
					considered as 0.");
			params.addFloat("DelIntersMaxIter", 8, "Max iterations to delete self intersections",
					"When eroding the mesh sometimes may happen that a face\n \
					 intersects another area of the mesh, generating awful \n \
					 artefacts. To avoid this, the vertexes of that face \n \
					 will gradually be moved back to their original positions.\n \
					 This parameter represents the maximum number of iterations\n \
					 that the plugin can do to try to correct these errors (0 to\n \
					 ignore the check).");
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
	
	float thresholdValue = params.getAbsPerc((useQuality?"QualityThreshold":"AngleThreshold"));
	float edgeLenTreshold = params.getAbsPerc("EdgeLenThreshold");
	float chipDepth = params.getAbsPerc("ChipDepth");
	int chipStyle = params.getEnum("ChipStyle");
	float noiseClamp = params.getAbsPerc("NoiseClamp");
	float intersMaxIter = params.getFloat("DelIntersMaxIter");
	bool selected = params.getBool("Selected");
	
	// edge predicate
	AgingEdgePred ep = AgingEdgePred((useQuality?AgingEdgePred::QUALITY:AgingEdgePred::ANGLE), 
									 (selected && useQuality), edgeLenTreshold, thresholdValue);
	
	int fcount = 1;					// face counter (to update progress bar)
	
	typedef std::map<CVertexO*, Point3<CVertexO::ScalarType> > DispMap;
	DispMap displaced;					// displaced vertexes and their displacement offset 
	std::vector<CFaceO*> intersFace;	// self intersecting mesh faces (after displacement)
	
	srand(time(NULL));
	
	switch(ID(filter)) {
		case FP_ERODE:
			// refine needed edges
			RefineMesh(m.cm, ep, selected, cb);
			
			// displace vertexes
			for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); fi++) {
				if(cb) (*cb)((fcount++/m.cm.fn)*100, "Aging..."); 
				if((*fi).IsD()) continue;
				for(int j=0; j<3; j++) {
					if(ep.qaVertTest(face::Pos<CMeshO::FaceType>(&*fi,j))  && 
					  (!selected || ((*fi).IsS() && (*fi).FFp(j)->IsS())) ) { 
						double noise;							// noise value
						Point3<CVertexO::ScalarType> dispDir;	// displacement direction
						float angleFactor;			// angle multiply factor in [0,1] range
													// (greater angle -> deeper chip)
						
						noise = generateNoiseValue(chipStyle, (*fi).V(j)->P().X(), (*fi).V(j)->P().Y(), (*fi).V(j)->P().Z());
						// no negative values allowed (negative noise generates hills, not chips)
						noise *= (noise>=0.0?1.0:-1.0);
						// only values bigger than noiseClamp will be considered
						noise = (noise<noiseClamp?0.0:noise);
						
						if((*fi).IsB(j)) {
							dispDir = Point3<CVertexO::ScalarType>((*fi).N().Normalize());
							angleFactor = 1.0;
						}
						else {
							if(useQuality)
								dispDir = Point3<CVertexO::ScalarType>(((*fi).N() + (*fi).FFp(j)->N()).Normalize());
							else
								dispDir = Point3<CVertexO::ScalarType>((*fi).N().Normalize());
							angleFactor = (sin(vcg::Angle((*fi).N(), (*fi).FFp(j)->N()))/2 + 0.5);
						}
						
						// displacement offset
						Point3<CVertexO::ScalarType> offset = -(dispDir * chipDepth * noise * angleFactor);
						if(displaced.find((*fi).V(j)) == displaced.end())
							displaced.insert(std::pair<CVertexO*, Point3<CVertexO::ScalarType> >((*fi).V(j), (*fi).V(j)->P()));
						(*fi).V(j)->P() += offset;
					}
				}
			}
			
			// avoid mesh self intersection
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
			
			return true;
		default:
			assert(0);
	}
}


/* Refines the mesh where needed.
 * When we work on the selection with a quality predicate we have to do some
 * additional checks to avoid infinite loops: we set the V bit on all the selected 
 * faces, we dilate the selection, we call the refine function, and then we
 * erode the selection to obtain the original selected area. The predicate doesn't
 * allow the refinement of an edge between two selected faces without the V bit 
 * setted. In this way we are able to refine also the esges on the  borders of the 
 * selection (without this operation the refineE function always returns true). */
void GeometryAgingPlugin::RefineMesh(CMeshO &m, AgingEdgePred &ep, bool selection, vcg::CallBackPos *cb)
{
	bool ref = true;
	CMeshO::FaceIterator fi;
	
	// clear V bit on all faces
	if(selection && ep.getType()==AgingEdgePred::QUALITY)
		for(fi = m.face.begin(); fi!=m.face.end(); fi++)
			if(!(*fi).IsD()) (*fi).ClearV();
	while(ref) {
		if(selection && ep.getType()==AgingEdgePred::QUALITY) {
			// set V bit on selected faces
			for(fi = m.face.begin(); fi!=m.face.end(); fi++)
				if(!(*fi).IsD() && (*fi).IsS()) (*fi).SetV();
			// dilate selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m);  
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
		}
		ref = RefineE<CMeshO, MidPoint<CMeshO>, AgingEdgePred>(m, MidPoint<CMeshO>(), ep, selection, cb);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m);
		if(selection && ep.getType()==AgingEdgePred::QUALITY) {
			// erode selection
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m);
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m);
			// clear V bit on all faces
			for(fi = m.face.begin(); fi!=m.face.end(); fi++)
				if(!(*fi).IsD()) (*fi).ClearV();
		}
	}
}


/* Returns a noise value using the selected style */
double GeometryAgingPlugin::generateNoiseValue(int style, const CVertexO::ScalarType &x, 
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
				float p = pow(2.0f, i);
				noise += math::Perlin::Noise(p*x, p*y, p*z) / p;
			}
			break;
	}

	if(style == SINUSOIDAL)
		noise = sin(x + noise);
	
	return noise;
}


Q_EXPORT_PLUGIN(GeometryAgingPlugin)
