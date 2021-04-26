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

#ifndef FILTER_VORONOI_H
#define FILTER_VORONOI_H

#include <common/plugins/interfaces/filter_plugin.h>

class FilterVoronoiPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum {
		VORONOI_SAMPLING,
		VOLUME_SAMPLING,
		VORONOI_SCAFFOLDING,
		BUILD_SHELL,
		CROSS_FIELD_CREATION,
		CROSS_FIELD_SMOOTHING
		};

	FilterVoronoiPlugin();

	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction* a) const;
	void initParameterList(const QAction* action, const MeshModel& m, RichParameterList& par);
	int getPreConditions(const QAction* action) const;
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	int postCondition(const QAction* ) const;

private:
	void voronoiSampling(
			MeshDocument &md,
			vcg::CallBackPos* cb,
			int iterNum,
			int sampleNum,
			Scalarm radiusVariance,
			int distanceType,
			int randomSeed,
			int relaxType,
			int colorStrategy,
			int refineFactor,
			Scalarm perturbProbability,
			Scalarm perturbAmount,
			bool preprocessingFlag);

	void volumeSampling(
			MeshDocument& md,
			vcg::CallBackPos* cb,
			Scalarm sampleSurfRadius,
			int sampleVolNum, 
			bool poissonFiltering,
			Scalarm poissonRadius);

	void voronoiScaffolding(
			MeshDocument& md,
			vcg::CallBackPos* cb,
			Scalarm sampleSurfRadius,
			int sampleVolNum,
			int voxelRes,
			Scalarm isoThr,
			int smoothStep,
			int relaxStep,
			bool surfFlag,
			int elemType);

	void createSolidWireframe(
			MeshDocument& md,
			bool edgeCylFlag,
			Scalarm edgeCylRadius,
			bool vertCylFlag,
			Scalarm vertCylRadius,
			bool vertSphFlag,
			Scalarm vertSphRadius,
			bool faceExtFlag,
			Scalarm faceExtHeight,
			Scalarm faceExtInset,
			bool /*edgeFauxFlag*/,
			int cylinderSideNum);

	void crossFieldCreation(
			MeshDocument& md,
			int crossType);

	void crossFieldColoring(MeshDocument& md);

//	bool crossFieldSmoothing(
//			MeshDocument& md,
//			bool preprocessFlag);
};


#endif
