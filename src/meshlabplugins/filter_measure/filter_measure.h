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

#ifndef FILTER_MEASURE_H
#define FILTER_MEASURE_H

#include <common/interfaces.h>

class FilterMeasurePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:
	enum {
			COMPUTE_TOPOLOGICAL_MEASURES,
			COMPUTE_TOPOLOGICAL_MEASURES_QUAD_MESHES,
			COMPUTE_GEOMETRIC_MEASURES,
			COMPUTE_AREA_PERIMETER_SELECTION,
			PER_VERTEX_QUALITY_STAT,
			PER_FACE_QUALITY_STAT,
			PER_VERTEX_QUALITY_HISTOGRAM,
			PER_FACE_QUALITY_HISTOGRAM
		 } ;

	FilterMeasurePlugin();

	QString pluginName() const;

	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(const QAction*) const;
	FILTER_ARITY filterArity(const QAction*) const;
	int getPreConditions(const QAction *action) const;
	void initParameterList(const QAction* , MeshModel& m, RichParameterList& parlst);
	bool applyFilter(const QAction* filter, MeshDocument& md, const RichParameterList& parlst, vcg::CallBackPos*) ;
	int postCondition(const QAction* ) const;

private:
	bool computeTopologicalMeasures(MeshDocument& md);
	bool computeTopologicalMeasuresForQuadMeshes(MeshDocument& md);
	bool computeGeometricMeasures(MeshDocument& md);
	bool computeAreaPerimeterOfSelection(MeshDocument& md);
	bool perVertexQualityStat(MeshDocument& md);
	bool perFaceQualityStat(MeshDocument& md);
	bool perVertexQualityHistogram(MeshDocument& md, float RangeMin, float RangeMax, int binNum, bool areaFlag);
	bool perFaceQualityHostogram(MeshDocument& md, float RangeMin, float RangeMax, int binNum, bool areaFlag);


	Matrix33m computePrincipalAxisCloud(CMeshO & m);

};


#endif
