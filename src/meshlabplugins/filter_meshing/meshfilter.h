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

#ifndef EXTRAFILTERSPLUGIN_H
#define EXTRAFILTERSPLUGIN_H

#include <common/interfaces/filter_plugin_interface.h>

class ExtraMeshFilterPlugin : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	    MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
		Q_INTERFACES(FilterPluginInterface)

		enum RefPlane { REF_CENTER,REF_MIN,REF_ORIG};

public:
		/* naming convention :
	- FP -> Filter Plugin
	- name of the filter separated by _
	*/
		enum {
		FP_LOOP_SS,
		FP_BUTTERFLY_SS,
		FP_CLUSTERING,
		FP_QUADRIC_SIMPLIFICATION,
		FP_QUADRIC_TEXCOORD_SIMPLIFICATION,
		FP_EXPLICIT_ISOTROPIC_REMESHING,
		FP_NORMAL_EXTRAPOLATION,
		FP_NORMAL_SMOOTH_POINTCLOUD,
		FP_COMPUTE_PRINC_CURV_DIR,
		FP_SLICE_WITH_A_PLANE,
		FP_PERIMETER_POLYLINE,
		FP_MIDPOINT,
		FP_REORIENT ,
		FP_FLIP_AND_SWAP,
		FP_ROTATE,
		FP_ROTATE_FIT,
		FP_SCALE,
		FP_CENTER,
		FP_PRINCIPAL_AXIS,
		FP_INVERT_FACES,
		FP_FREEZE_TRANSFORM,
		FP_RESET_TRANSFORM,
		FP_INVERT_TRANSFORM,
		FP_SET_TRANSFORM_PARAMS,
		FP_SET_TRANSFORM_MATRIX,
		FP_CLOSE_HOLES_TRIVIAL,
		FP_CLOSE_HOLES,
		FP_CYLINDER_UNWRAP,
		FP_REFINE_CATMULL,
		FP_REFINE_HALF_CATMULL,
		FP_QUAD_DOMINANT,
		FP_MAKE_PURE_TRI,
		FP_QUAD_PAIRING,
		FP_FAUX_CREASE,
		FP_FAUX_EXTRACT,
		FP_VATTR_SEAM,
		FP_REFINE_LS3_LOOP
	} ;



	ExtraMeshFilterPlugin();
	~ExtraMeshFilterPlugin(){}
	QString pluginName() const;
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;

	FilterClass getClass(const QAction*) const;
	void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*parent*/);
	bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
	int postCondition(const QAction *filter) const;
	int getPreConditions(const QAction *filter) const;
	FILTER_ARITY filterArity(const QAction *) const {return SINGLE_MESH;}

protected:

	float lastq_QualityThr;
	bool lastq_QualityWeight;
	bool lastq_PreserveBoundary;
	bool lastq_Selected;
	bool lastq_PreserveNormal;
	bool lastq_BoundaryWeight;
	bool lastq_PreserveTopology;
	bool lastq_OptimalPlacement;
	bool lastq_PlanarQuadric;
	float lastq_PlanarWeight;

	float lastqtex_QualityThr;
	float lastqtex_extratw;

	int lastisor_Iterations;
	Scalarm lastisor_MaxSurfDist;
	Scalarm lastisor_FeatureDeg;
	bool lastisor_CheckSurfDist;
	bool lastisor_RemeshingAdaptivity;
	bool lastisor_SelectedOnly;
	bool lastisor_RefineFlag;
	bool lastisor_CollapseFlag;
	bool lastisor_SwapFlag;
	bool lastisor_SmoothFlag;
	bool lastisor_ProjectFlag;

};
#endif
