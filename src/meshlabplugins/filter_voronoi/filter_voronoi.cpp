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

#include "filter_voronoi.h"
#include <QtScript>

#include<vcg/complex/algorithms/voronoi_processing.h>
#include<vcg/complex/algorithms/update/curvature.h>
#include<vcg/complex/algorithms/update/curvature_fitting.h>
#include<vcg/complex/algorithms/update/quality.h>
#include<vcg/complex/algorithms/smooth.h>
#include<vcg/complex/algorithms/voronoi_volume_sampling.h>
#include<vcg/complex/algorithms/polygon_support.h>

using namespace vcg;

FilterVoronoiPlugin::FilterVoronoiPlugin()
{ 
	typeList
		<< VORONOI_SAMPLING
		<< VOLUME_SAMPLING
		<< VORONOI_SCAFFOLDING
		<< BUILD_SHELL
		<< CROSS_FIELD_CREATION
		<< CROSS_FIELD_SMOOTHING;

	for (FilterIDType tt : types())
		actionList << new QAction(filterName(tt), this);
}

QString FilterVoronoiPlugin::pluginName() const
{
	return "VoronoiTechniques";
}

QString FilterVoronoiPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
	case VORONOI_SAMPLING :
		return "Voronoi Sampling";
	case VOLUME_SAMPLING:
		return "Volumetric Sampling";
	case VORONOI_SCAFFOLDING:
		return "Voronoi Scaffolding";
	case BUILD_SHELL:
		return "Create Solid Wireframe";
	case CROSS_FIELD_CREATION:
		return "Cross Field Creation";
	case CROSS_FIELD_SMOOTHING:
		return "Cross Field Smoothing";
	default :
		assert(0);
		return "";
	}
}


QString FilterVoronoiPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
	case VORONOI_SAMPLING :
		return "Compute a sampling over a mesh and perform a Lloyd relaxation.";
	case VOLUME_SAMPLING:
		return "Compute a volumetric sampling over a watertight mesh.";
	case VORONOI_SCAFFOLDING:
		return "Compute a volumetric sampling over a watertight mesh.";
	case BUILD_SHELL:
		return "";
	case CROSS_FIELD_CREATION:
		return "";
	case CROSS_FIELD_SMOOTHING:
		return "";
	default :
		assert(0);
		return "";
	}
}

FilterVoronoiPlugin::FilterClass FilterVoronoiPlugin::getClass(QAction* a)
{
	switch(ID(a)) {
	case VORONOI_SAMPLING :
	case VOLUME_SAMPLING:
	case VORONOI_SCAFFOLDING:
		return MeshFilterInterface::Sampling;
	case BUILD_SHELL:
		return MeshFilterInterface::Remeshing;
	case CROSS_FIELD_CREATION:
		return MeshFilterInterface::Normal;
	case CROSS_FIELD_SMOOTHING:
		return MeshFilterInterface::Smoothing;
	default :
		assert(0);
		return MeshFilterInterface::Generic;
	}
}

MeshFilterInterface::FILTER_ARITY FilterVoronoiPlugin::filterArity(QAction* a) const
{
	switch(ID(a)) {
	case VORONOI_SAMPLING :
	case VORONOI_SCAFFOLDING :
	case CROSS_FIELD_CREATION :
	case CROSS_FIELD_SMOOTHING :
		return SINGLE_MESH;
	case VOLUME_SAMPLING :
	case BUILD_SHELL :
		return VARIABLE;
	default :
		assert(0);
		return NONE;
	}
}

void FilterVoronoiPlugin::initParameterSet(QAction* action, MeshModel& m, RichParameterSet& par)
{
	 switch(ID(action))	 {
	 case VORONOI_SAMPLING :
		 par.addParam(new RichInt("iterNum", 10, "Iteration", "number of iterations"));
		 par.addParam(new RichInt("sampleNum", 10, "Sample Num.", "Number of samples"));
		 par.addParam(new RichFloat("radiusVariance", 1, "Radius Variance", "The distance metric will vary along the surface between 1/x and x, linearly according to the scalar field specified by the quality."));
		 par.addParam(new RichEnum("colorStrategy", 1, {"None", "Seed_Distance", "Border_Distance", "Region_Area"}, "Color Strategy"));
		 par.addParam(new RichEnum("distanceType", 0, {"Euclidean", "QualityWeighted", "Anisotropic"}, "Distance Type"));
		 par.addParam(new RichBool("preprocessFlag", false, "Preprocessing", ""));
		 par.addParam(new RichInt("refineFactor", 10, "Refinement Factor", "To ensure good convergence the mesh should be more complex than the voronoi partitioning. This number affect how much the mesh is refined according to the required number of samples."));
		 par.addParam(new RichFloat("perturbProbability", 0, "Perturbation Probability", "To ensure good convergence the mesh should be more complex than the voronoi partitioning. This number affect how much the mesh is refined according to the required number of samples."));
		 par.addParam(new RichFloat("perturbAmount", 0.001, "Perturbation Amount", "To ensure good convergence the mesh should be more complex than the voronoi partitioning. This number affect how much the mesh is refined according to the required number of samples."));
		 par.addParam(new RichInt("randomSeed", 0, "Random seed", "To ensure repeatability you can specify the random seed used. If 0 the random seed is tied to the current clock."));
		 par.addParam(new RichEnum("relaxType", 1, {"Geodesic", "SquaredDistance", "Restricted"}, "Relax Type",
									  "At each relaxation step we search for each voronoi region the new position of the seed. "
									  "According to the classical LLoyd relaxation strategy it should have been placed onto the "
									  "barycenter of the region. Over a surface we have two different strategies: <ul>"
									  "<li> Geodesic: the seed is placed onto the vertex that maximize the geodesic distance from the border of the region </li>"
									  "<li> Squared Distance: the seed is placed in the vertex that minimize the squared sum of the distances from all the pints of the region.</li>"
									  "<li> Restricted: the seed is placed in the barycenter of current voronoi region. Even if it is outside the surface. During the relaxation process the seed is free to move off the surface in a continuos way. Re-association to vertex is done at the end..</li>"
									  "</ul>"));
		 break;
	 case VOLUME_SAMPLING:
		 break;
	 case VORONOI_SCAFFOLDING:
		 break;
	 case BUILD_SHELL:
		 break;
	 case CROSS_FIELD_CREATION:
		 break;
	 case CROSS_FIELD_SMOOTHING:
		 break;
	 default :
		 assert(0);
	 }
}

int FilterVoronoiPlugin::getPreConditions(QAction* action) const
{
	switch(ID(action))	 {
	case VORONOI_SAMPLING :
		return MeshModel::MM_NONE;
	case VOLUME_SAMPLING:
		break;
	case VORONOI_SCAFFOLDING:
		break;
	case BUILD_SHELL:
		break;
	case CROSS_FIELD_CREATION:
		break;
	case CROSS_FIELD_SMOOTHING:
		break;
	default :
		assert(0);
		return 0;
	}
}

bool FilterVoronoiPlugin::applyFilter(QAction * action, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(action))	 {
	case VORONOI_SAMPLING :
		return voronoiSampling(
					md, cb,
					par.getInt("iterNum"), par.getInt("sampleNum"), par.getFloat("radiusVariance"),
					par.getEnum("distanceType"), par.getInt("randomSeed"), par.getEnum("relaxType"),
					par.getEnum("colorStrategy"), par.getInt("refineFactor"), par.getFloat("perturbProbability"),
					par.getFloat("perturbAmount"), par.getBool("preprocessFlag"));
	case VOLUME_SAMPLING:
		break;
	case VORONOI_SCAFFOLDING:
		break;
	case BUILD_SHELL:
		break;
	case CROSS_FIELD_CREATION:
		break;
	case CROSS_FIELD_SMOOTHING:
		break;
	default :
		assert(0);
		return false;
	}
}

int FilterVoronoiPlugin::postCondition(QAction* action) const
{
	switch(ID(action))	 {
	case VORONOI_SAMPLING :
		return MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY;
	case VOLUME_SAMPLING:
		break;
	case VORONOI_SCAFFOLDING:
		break;
	case BUILD_SHELL:
		break;
	case CROSS_FIELD_CREATION:
		break;
	case CROSS_FIELD_SMOOTHING:
		break;
	default :
		assert(0);
		return 0;
	}
}

bool FilterVoronoiPlugin::voronoiSampling(
		MeshDocument &md,
		vcg::CallBackPos* cb,
		int iterNum,
		int sampleNum,
		float radiusVariance,
		int distanceType,
		int randomSeed,
		int relaxType,
		int colorStrategy,
		int refineFactor,
		float perturbProbability,
		float perturbAmount,
		bool preprocessingFlag)
{
	MeshModel *om=md.addOrGetMesh("voro", "voro", false);
	MeshModel *poly=md.addOrGetMesh("poly", "poly", false);

	om->updateDataMask(MeshModel::MM_VERTCOLOR);

	MeshModel &m=*md.mm();

	m.updateDataMask(MeshModel::MM_FACEFACETOPO);
	m.updateDataMask(MeshModel::MM_VERTFACETOPO);
	m.updateDataMask(MeshModel::MM_VERTCURV);
	m.updateDataMask(MeshModel::MM_VERTCURVDIR);
	m.updateDataMask(MeshModel::MM_VERTCOLOR);
	tri::UpdateCurvature<CMeshO>::PerVertexBasicRadialCrossField(m.cm);

	//tri::EuclideanDistance<CMeshO> dd;
	vector<CVertexO *> seedVec;
	//tri::ClusteringSampler<CMeshO> cs(seedVec);
	//tri::SurfaceSampling<CMeshO, vcg::tri::ClusteringSampler<CMeshO> >::SamplingRandomGenerator().initialize(randSeed);
	vector<Point3m> pointVec;
	vector<bool> fixedVec;
	CMeshO::ScalarType radius=0;

	tri::PoissonSampling<CMeshO>(m.cm,pointVec,sampleNum,radius,radiusVariance,0,randomSeed);

	tri::VoronoiProcessingParameter vpp;
	vpp.geodesicRelaxFlag = (relaxType==0);
	vpp.colorStrategy = colorStrategy;
	vpp.deleteUnreachedRegionFlag=true;
	vpp.refinementRatio=refineFactor;
	vpp.seedPerturbationAmount = perturbAmount;
	vpp.seedPerturbationProbability = perturbProbability;
	if(preprocessingFlag) {
		tri::VoronoiProcessing<CMeshO>::PreprocessForVoronoi(m.cm,radius,vpp);
	}

	tri::VoronoiProcessing<CMeshO>::SeedToVertexConversion(m.cm,pointVec,seedVec);
	fixedVec.resize(pointVec.size(),false);
	QList<int> meshlist; meshlist << m.id();

	// Uniform Euclidean Distance
	if(distanceType==0)  {
		EuclideanDistance<CMeshO> dd;
		for(int i=0;i<iterNum;++i) {
			cb(100*i/iterNum, "Relaxing...");
			if(relaxType==2) {
				tri::VoronoiProcessing<CMeshO, EuclideanDistance<CMeshO> >::RestrictedVoronoiRelaxing(m.cm, pointVec, fixedVec, 10,vpp);
				tri::VoronoiProcessing<CMeshO>::SeedToVertexConversion(m.cm,pointVec,seedVec);
				tri::VoronoiProcessing<CMeshO>::ComputePerVertexSources(m.cm,seedVec,dd);
			}
			else {
				tri::VoronoiProcessing<CMeshO, EuclideanDistance<CMeshO> >::VoronoiRelaxing(m.cm, seedVec, 1,dd,vpp);
			}
			// md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
			//if (intteruptreq)
			//	return true;
		}
		om->updateDataMask(MeshModel::MM_FACEFACETOPO);
		tri::VoronoiProcessing<CMeshO>::ConvertVoronoiDiagramToMesh(m.cm,om->cm,poly->cm,seedVec, vpp);
	}

	if(distanceType==1) {
		IsotropicDistance<CMeshO> id(m.cm,radiusVariance);
		for(int i=0;i<iterNum;++i) {
			cb(100*i/iterNum, "Relaxing...");
			tri::VoronoiProcessing<CMeshO, IsotropicDistance<CMeshO> >::VoronoiRelaxing(m.cm, seedVec, 1,id,vpp);
			//md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
			//if (intteruptreq)
			//	return true;
		}
		// tri::VoronoiProcessing<CMeshO>::ConvertVoronoiDiagramToMesh(m.cm,om->cm,poly->cm,seedVec, vpp);
	}
	if(distanceType==2) {
		for(int i=0;i<iterNum;++i) {
			cb(100*i/iterNum, "Relaxing...");
			BasicCrossFunctor<CMeshO> bcf(m.cm);
			AnisotropicDistance<CMeshO> ad(m.cm,bcf);
			tri::VoronoiProcessing<CMeshO, AnisotropicDistance<CMeshO> >::VoronoiRelaxing(m.cm, seedVec, 1, ad, vpp);
			//md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
			//if (intteruptreq)
			//	return true;
		}
		//tri::VoronoiProcessing<CMeshO, AnisotropicDistance<CMeshO> >::ConvertVoronoiDiagramToMesh(m.cm,om->cm,seedVec, ad, vpp);
	}

	//md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
	//if (intteruptreq)
	//	return true;

	tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
	for(auto vi =seedVec.begin();vi!=seedVec.end();++vi)
		(*vi)->SetS();

	om->UpdateBoxAndNormals();
	return true;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterVoronoiPlugin)
