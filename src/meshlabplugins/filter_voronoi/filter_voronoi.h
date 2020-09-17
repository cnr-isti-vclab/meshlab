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

#include <common/interfaces.h>

class FilterVoronoiPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

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
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FILTER_ARITY filterArity(const QAction* a) const;
	void initParameterList(const QAction* action, MeshModel& m, RichParameterList& par);
	int getPreConditions(const QAction* action) const;
	bool applyFilter(const QAction* action, MeshDocument& md, const RichParameterList& par, vcg::CallBackPos* cb) ;
	int postCondition(const QAction* ) const;

private:
	bool voronoiSampling(
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
			bool preprocessingFlag);

	bool volumeSampling(
			MeshDocument& md,
			vcg::CallBackPos* cb,
			float sampleSurfRadius,
			int sampleVolNum, bool poissonFiltering,
			float poissonRadius);

	bool voronoiScaffolding(
			MeshDocument& md,
			vcg::CallBackPos* cb,
			float sampleSurfRadius,
			int sampleVolNum,
			int voxelRes,
			float isoThr,
			int smoothStep,
			int relaxStep,
			bool surfFlag,
			int elemType);

	bool createSolidWireframe(
			MeshDocument& md,
			bool edgeCylFlag,
			float edgeCylRadius,
			bool vertCylFlag,
			float vertCylRadius,
			bool vertSphFlag,
			float vertSphRadius,
			bool faceExtFlag,
			float faceExtHeight,
			float faceExtInset,
			bool /*edgeFauxFlag*/,
			int cylinderSideNum);

	bool crossFieldCreation(
			MeshDocument& md,
			int crossType);

	bool crossFieldColoring(MeshDocument& md);

//	bool crossFieldSmoothing(
//			MeshDocument& md,
//			bool preprocessFlag);
};


#endif
