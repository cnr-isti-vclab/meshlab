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

#include <common/interfaces.h>

class ExtraMeshFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
 Q_INTERFACES(MeshFilterInterface)

public:
         /* naming convention :
		 - FP -> Filter Plugin
		 - name of the filter separated by _
	*/
   enum {
     FP_LOOP_SS,
     FP_BUTTERFLY_SS,
     FP_REMOVE_UNREFERENCED_VERTEX,
     FP_REMOVE_DUPLICATED_VERTEX,
     FP_REMOVE_FACES_BY_AREA,
     FP_REMOVE_FACES_BY_EDGE,
     FP_REMOVE_NON_MANIFOLD_FACE,
     FP_REMOVE_NON_MANIFOLD_VERTEX,
     FP_CLUSTERING,
     FP_QUADRIC_SIMPLIFICATION,
     FP_QUADRIC_TEXCOORD_SIMPLIFICATION,
     FP_NORMAL_EXTRAPOLATION,
     FP_COMPUTE_PRINC_CURV_DIR,
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
     FP_CLOSE_HOLES_TRIVIAL,
     FP_CLOSE_HOLES,
     FP_CYLINDER_UNWRAP,
     FP_REFINE_CATMULL,
     FP_REFINE_HALF_CATMULL,
     FP_QUAD_PAIRING,
     FP_FAUX_CREASE,
	 FP_VATTR_SEAM
 } ;



 ExtraMeshFilterPlugin();
 ~ExtraMeshFilterPlugin(){};
 virtual QString filterName(FilterIDType filter) const;
 virtual QString filterInfo(FilterIDType filter) const;

 virtual FilterClass getClass(QAction *);
 virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
 virtual int getRequirements(QAction *);
 virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
 virtual int postCondition(QAction *filter) const;

protected:

 float lastq_QualityThr;
 bool lastq_QualityWeight;
 bool lastq_PreserveBoundary;
 bool lastq_Selected;
 bool lastq_PreserveNormal;
 bool lastq_OptimalPlacement;
 bool lastq_PlanarQuadric;

 float lastqtex_QualityThr;
 float lastqtex_extratw;
};

#endif
