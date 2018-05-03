/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
#include <Qt>
#include "filter_unsharp.h"

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/crease_cut.h>
#include <vcg/complex/algorithms/harmonic.h>

using namespace vcg;
using namespace std;

FilterUnsharp::FilterUnsharp()
{
  typeList <<
              FP_CREASE_CUT <<
              FP_LAPLACIAN_SMOOTH<<
              FP_HC_LAPLACIAN_SMOOTH<<
              FP_SD_LAPLACIAN_SMOOTH<<
              FP_TWO_STEP_SMOOTH<<
              FP_TAUBIN_SMOOTH<<
              FP_DEPTH_SMOOTH<<
              FP_DIRECTIONAL_PRESERVATION<<
              FP_VERTEX_QUALITY_SMOOTHING<<
              FP_FACE_NORMAL_SMOOTHING<<
              FP_UNSHARP_NORMAL<<
              FP_UNSHARP_GEOMETRY<<
              FP_UNSHARP_QUALITY <<
              FP_UNSHARP_VERTEX_COLOR <<
              FP_RECOMPUTE_VERTEX_NORMAL <<
              FP_RECOMPUTE_FACE_NORMAL <<
              FP_RECOMPUTE_QUADFACE_NORMAL <<
              FP_FACE_NORMAL_NORMALIZE <<
              FP_VERTEX_NORMAL_NORMALIZE <<
              FP_LINEAR_MORPH <<
              FP_SCALAR_HARMONIC_FIELD;
  ;

  FilterIDType tt;
  foreach(tt , types())
        actionList << new QAction(filterName(tt), this);

}

FilterUnsharp::~FilterUnsharp() {
    for (int i = 0; i < actionList.count() ; i++ )
        delete actionList.at(i);
}

QString FilterUnsharp::filterName(FilterIDType filter) const
{
  switch(filter)
  {
  case FP_LAPLACIAN_SMOOTH :					return QString("Laplacian Smooth");
  case FP_HC_LAPLACIAN_SMOOTH :					return QString("HC Laplacian Smooth");
  case FP_SD_LAPLACIAN_SMOOTH :					return QString("ScaleDependent Laplacian Smooth");
  case FP_TWO_STEP_SMOOTH :	    				return QString("TwoStep Smooth");
  case FP_TAUBIN_SMOOTH :						return QString("Taubin Smooth");
  case FP_DEPTH_SMOOTH :						return QString("Depth Smooth");
  case FP_DIRECTIONAL_PRESERVATION :            return QString("Directional Geom. Preserv.");
  case FP_CREASE_CUT :							return QString("Cut mesh along crease edges");
  case FP_FACE_NORMAL_NORMALIZE:                return QString("Normalize Face Normals");
  case FP_VERTEX_NORMAL_NORMALIZE:		return QString("Normalize Vertex Normals");
  case FP_FACE_NORMAL_SMOOTHING:	  return QString("Smooth Face Normals");
  case FP_VERTEX_QUALITY_SMOOTHING:	return QString("Smooth Vertex Quality");
  case FP_UNSHARP_NORMAL:						return QString("UnSharp Mask Normals");
  case FP_UNSHARP_GEOMETRY:                     return QString("UnSharp Mask Geometry");
  case FP_UNSHARP_QUALITY:					return QString("UnSharp Mask Quality");
  case FP_UNSHARP_VERTEX_COLOR:	    return QString("UnSharp Mask Color");
  case FP_RECOMPUTE_VERTEX_NORMAL:	return QString("Re-Compute Vertex Normals");
  case FP_RECOMPUTE_FACE_NORMAL:            return QString("Re-Compute Face Normals");
  case FP_RECOMPUTE_QUADFACE_NORMAL:		return QString("Re-Compute Per-Polygon Face Normals");
  case FP_LINEAR_MORPH :	return QString("Vertex Linear Morphing");
  case FP_SCALAR_HARMONIC_FIELD: return QString("Generate Scalar Harmonic Field");
    

  default: assert(0);
  }
  return QString("error!");
}

QString FilterUnsharp::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
  case FP_LAPLACIAN_SMOOTH :          return tr("Laplacian smooth of the mesh: for each vertex it calculates the average position with nearest vertex");
  case FP_HC_LAPLACIAN_SMOOTH :       return tr("HC Laplacian Smoothing, extended version of Laplacian Smoothing, based on the paper of Vollmer, Mencl, and Muller");
  case FP_SD_LAPLACIAN_SMOOTH :       return tr("Scale Dependent Laplacian Smoothing, extended version of Laplacian Smoothing, based on the Fujiwara extended umbrella operator");
  case FP_TWO_STEP_SMOOTH :           return tr("Two Step Smoothing, a feature preserving/enhancing fairing filter. It is based on a Normal Smoothing step where similar normals are averaged together and a step where the vertexes are fitted on the new normals. Based on: <br>"
                                                "<i>A. Belyaev and Y. Ohtake</i>, <b>A Comparison of Mesh Smoothing Methods</b>, Proc. Israel-Korea Bi-National Conf. Geometric Modeling and Computer Graphics, pp. 83-87, 2003.");
  case FP_TAUBIN_SMOOTH :             return tr("The &lambda;-&mu; Taubin smoothing, it make two steps of smoothing, forth and back, for each iteration. Based on:<br>"
                                                "Gabriel Taubin,<br><b>A signal processing approach to fair surface design</b><br>Siggraph 1995" );
  case FP_DEPTH_SMOOTH :              return tr("A laplacian smooth that is constrained to move vertices only along the view direction.");
  case FP_DIRECTIONAL_PRESERVATION :  return tr("Store and Blend the current geometry with the result of another previous smoothing processing step. It is useful to limit the influence of any smoothing algorithm along the viewing direction. This is import to cope with the biased distribution of the error in many scanning devices. TOF scanner usually have very good <b>x,y</b> accuracy but suffer of great depth errors.");
  case FP_CREASE_CUT:                 return tr("Cut the mesh along crease edges, duplicating the vertices as necessary. Crease edges are defined according to the variation of normal of the adjacent faces");
  case FP_FACE_NORMAL_NORMALIZE:      return tr("Normalize Face Normal Lenghts");
  case FP_VERTEX_NORMAL_NORMALIZE:    return tr("Normalize Vertex Normal Lenghts");
  case FP_VERTEX_QUALITY_SMOOTHING:	  return tr("Laplacian smooth of the quality values.");
  case FP_FACE_NORMAL_SMOOTHING:      return tr("Smooth Face Normals without touching the position of the vertices.");
  case FP_UNSHARP_NORMAL:             return tr("Unsharp mask filtering of the normals, putting in more evidence normal variations");
  case FP_UNSHARP_GEOMETRY:           return tr("Unsharp mask filtering of geometric shape, putting in more evidence ridges and valleys variations");
  case FP_UNSHARP_QUALITY:            return tr("Unsharp mask filtering of the quality field");
  case FP_UNSHARP_VERTEX_COLOR:       return tr("Unsharp mask filtering of the color, putting in more evidence color edge variations");
  case FP_RECOMPUTE_VERTEX_NORMAL:    return tr("Recompute vertex normals according to four different schemes:<br>"
                                                "1) as a simple average of normals of the incident faces <br>"
                                                "2) as an area weighted average of normals of the incident faces <br>"
                                                "3) as an angle weighted sum of normals of the incident faces according to the paper <i>Computing Vertex Normals from Polygonal Facet</i>, G Thurmer, CA Wuthrich, JGT 1998<br>"
                                                "Probably this is the best all-purpose choice. It could slightly bias the result for degenerate, fat triangles.<br>"
                                                "4) as a weighted sum of normals of the incident faces. Weights are defined according to the paper <i>Weights for Computing Vertex Normals from Facet Normals</i>, Nelson Max, JGT 1999<br>"
                                                "The weight for each wedge is the cross product of the two edge over the product of the square of the two edge lengths."
                                                "According to the original paper it is perfect only for spherical surface, but it should perform well also in practice.");
  case FP_RECOMPUTE_FACE_NORMAL:      return tr("Recompute face normals as the normal of the plane of the face");
  case FP_RECOMPUTE_QUADFACE_NORMAL:  return tr("Recompute face normals as the normal of the average of the normals of the triangles that builds a polygon. Useful for showing shaded quad or polygonal meshes represented using faux edges.");
  case FP_LINEAR_MORPH :              return tr("Morph current mesh towards a target with the same number of vertices. <br> The filter assumes that the two meshes have also the same vertex ordering.");
  case FP_SCALAR_HARMONIC_FIELD: return QString("Generates a scalar harmonic field over the mesh. Scalar values must be assigned to at least two vertices as Dirichlet boundary conditions. Applying the filter, a discrete Laplace operator generates the harmonic field values for all the mesh vertices. Note that the field values is stored in the quality per vertex attribute of the mesh\n\nFor more details see:\n Kai Xua, Hao Zhang, Daniel Cohen-Or, Yueshan Xionga,'Dynamic Harmonic Fields for Surface Processing'.\nin Computers & Graphics, 2009");

  default: assert(0);
  }
  return QString("error!");
}

 FilterUnsharp::FilterClass FilterUnsharp::getClass(QAction *a)
{
  switch(ID(a))
  {
            case FP_CREASE_CUT :
            return MeshFilterInterface::FilterClass( 	MeshFilterInterface::Normal | MeshFilterInterface::Remeshing);
        case FP_SD_LAPLACIAN_SMOOTH:
            case FP_HC_LAPLACIAN_SMOOTH:
            case FP_LAPLACIAN_SMOOTH:
            case FP_TWO_STEP_SMOOTH:
            case FP_TAUBIN_SMOOTH:
            case FP_DEPTH_SMOOTH:
            case FP_DIRECTIONAL_PRESERVATION:
            case FP_FACE_NORMAL_SMOOTHING:
            case FP_VERTEX_QUALITY_SMOOTHING:
            case FP_UNSHARP_NORMAL:
            case FP_UNSHARP_GEOMETRY:
            case FP_UNSHARP_QUALITY:
            case FP_LINEAR_MORPH :
                    return 	MeshFilterInterface::Smoothing;

            case FP_UNSHARP_VERTEX_COLOR:
                    return MeshFilterInterface::FilterClass( 	MeshFilterInterface::Smoothing | MeshFilterInterface::VertexColoring);

            case FP_RECOMPUTE_FACE_NORMAL :
            case FP_RECOMPUTE_QUADFACE_NORMAL :
            case FP_RECOMPUTE_VERTEX_NORMAL :
            case FP_FACE_NORMAL_NORMALIZE:
            case FP_VERTEX_NORMAL_NORMALIZE:
                    return MeshFilterInterface::Normal;
  case FP_SCALAR_HARMONIC_FIELD: return MeshFilterInterface::Remeshing;

    default : return MeshFilterInterface::Generic;
  }
}
int FilterUnsharp::getPreConditions(QAction *a) const
{
  switch(ID(a))
  {
  case FP_VERTEX_QUALITY_SMOOTHING:
  case FP_UNSHARP_QUALITY:
    return MeshModel::MM_FACENUMBER | MeshModel::MM_VERTQUALITY;
  case FP_SD_LAPLACIAN_SMOOTH:
  case FP_HC_LAPLACIAN_SMOOTH:
  case FP_LAPLACIAN_SMOOTH:
  case FP_TWO_STEP_SMOOTH:
  case FP_TAUBIN_SMOOTH:
  case FP_DEPTH_SMOOTH:
  case FP_LINEAR_MORPH :
  case FP_UNSHARP_NORMAL:
  case FP_UNSHARP_GEOMETRY:
  case FP_DIRECTIONAL_PRESERVATION:
  case FP_FACE_NORMAL_SMOOTHING:
  case FP_RECOMPUTE_FACE_NORMAL :
  case FP_RECOMPUTE_QUADFACE_NORMAL :
  case FP_RECOMPUTE_VERTEX_NORMAL :
  case FP_FACE_NORMAL_NORMALIZE:
  case FP_CREASE_CUT:
  case FP_SCALAR_HARMONIC_FIELD:
    return MeshModel::MM_FACENUMBER;
  case FP_UNSHARP_VERTEX_COLOR:
    return MeshModel::MM_FACENUMBER | MeshModel::MM_VERTCOLOR;
    
  case FP_VERTEX_NORMAL_NORMALIZE:	  return MeshModel::MM_NONE;
    
  default : assert(0); return MeshModel::MM_NONE;
  }
}


int FilterUnsharp::postCondition(QAction *a) const
{
	switch(ID(a))
	{
		case FP_SD_LAPLACIAN_SMOOTH:
		case FP_HC_LAPLACIAN_SMOOTH:
		case FP_LAPLACIAN_SMOOTH:
		case FP_TWO_STEP_SMOOTH:
		case FP_TAUBIN_SMOOTH:
		case FP_DEPTH_SMOOTH:
		case FP_LINEAR_MORPH :
		case FP_UNSHARP_NORMAL:
		case FP_UNSHARP_GEOMETRY:  return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL  | MeshModel::MM_FACENORMAL;
		case FP_DIRECTIONAL_PRESERVATION:
		case FP_VERTEX_QUALITY_SMOOTHING:
		case FP_UNSHARP_QUALITY:
		case FP_CREASE_CUT: return MeshModel::MM_ALL;
		case FP_FACE_NORMAL_SMOOTHING:
		case FP_RECOMPUTE_FACE_NORMAL:
		case FP_RECOMPUTE_QUADFACE_NORMAL:
		case FP_FACE_NORMAL_NORMALIZE: return MeshModel::MM_FACENORMAL;
		case FP_RECOMPUTE_VERTEX_NORMAL:
		case FP_VERTEX_NORMAL_NORMALIZE: return MeshModel::MM_VERTNORMAL;
		case FP_UNSHARP_VERTEX_COLOR: return MeshModel::MM_VERTCOLOR;
		case FP_SCALAR_HARMONIC_FIELD: return MeshModel::MM_VERTQUALITY;
		default : assert(0); return MeshModel::MM_ALL;
	}
}

 int FilterUnsharp::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_TWO_STEP_SMOOTH:      return MeshModel::MM_VERTFACETOPO;

    case FP_CREASE_CUT :
    case FP_UNSHARP_NORMAL:
    case FP_RECOMPUTE_QUADFACE_NORMAL :
    case FP_FACE_NORMAL_SMOOTHING : return MeshModel::MM_FACEFACETOPO;

    case FP_RECOMPUTE_FACE_NORMAL :
    case FP_RECOMPUTE_VERTEX_NORMAL :
    case FP_FACE_NORMAL_NORMALIZE:
    case FP_VERTEX_NORMAL_NORMALIZE:
    case FP_DIRECTIONAL_PRESERVATION:
    case FP_LINEAR_MORPH :
    case FP_HC_LAPLACIAN_SMOOTH:
    case FP_SD_LAPLACIAN_SMOOTH:
    case FP_TAUBIN_SMOOTH:
    case FP_DEPTH_SMOOTH:
    case FP_LAPLACIAN_SMOOTH:
    case FP_UNSHARP_GEOMETRY:
    case FP_UNSHARP_QUALITY:
    case FP_VERTEX_QUALITY_SMOOTHING:
	case FP_SCALAR_HARMONIC_FIELD:
	case FP_UNSHARP_VERTEX_COLOR: return MeshModel::MM_NONE;
    default: assert(0);
  }
  return MeshModel::MM_NONE;
}

void FilterUnsharp::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst)
{
    switch(ID(action))
    {
    case FP_RECOMPUTE_VERTEX_NORMAL :
      parlst.addParam(new RichEnum("weightMode", 0,
                                   QStringList() << "None (avg)" <<  "By Angle" << "By Area" << "As defined by N. Max",
                              tr("Weighting Mode:"),
                              tr("")));
          break;
        case FP_CREASE_CUT :
            parlst.addParam(new RichFloat("angleDeg", 90.f, tr("Crease Angle (degree)"), tr("If the angle between the normals of two adjacent faces is <b>larger</b> that this threshold the edge is considered a creased and the mesh is cut along it.")));
            break;
        case FP_UNSHARP_NORMAL:
            parlst.addParam(new RichBool("recalc", false, tr("Recompute Normals"), tr("Recompute normals from scratch before the unsharp masking")));
            parlst.addParam(new RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>")));
            parlst.addParam(new RichFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation.<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(new RichInt("iterations", 5, "Smooth Iterations", 	tr("number of laplacian face smooth iterations in every run")));
        break;
        case FP_UNSHARP_GEOMETRY:
            parlst.addParam(new RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>")));
            parlst.addParam(new RichFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(new RichInt("iterations", 5, "Smooth Iterations", 	tr("number ofiterations of laplacian smooth in every run")));
            break;
        case FP_UNSHARP_VERTEX_COLOR:
            parlst.addParam(new RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>")));
            parlst.addParam(new RichFloat("weightOrig", 1.f, tr("Original Color Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(new RichInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run")));
            break;
        case FP_UNSHARP_QUALITY:
            parlst.addParam(new RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>")));
            parlst.addParam(new RichFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(new RichInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run")));
            break;
        case FP_TWO_STEP_SMOOTH:
        parlst.addParam(new RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
        parlst.addParam(new RichFloat("normalThr", (float) 60,"Feature Angle Threshold (deg)", "Specify a threshold angle (0..90) for features that you want to be preserved.<br>Features forming angles LARGER than the specified threshold will be preserved. <br> 0 -> no smoothing <br> 90 -> all faces will be smoothed"));
        parlst.addParam(new RichInt  ("stepNormalNum", (int) 20,"Normal Smoothing steps", "Number of iterations of normal smoothing step. The larger the better and (the slower)"));
        parlst.addParam(new RichInt  ("stepFitNum",    (int) 20,"Vertex Fitting steps", "Number of iterations of the vertex fitting procedure."));
        parlst.addParam(new RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
        break;
        case FP_LAPLACIAN_SMOOTH:
            parlst.addParam(new RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
            parlst.addParam(new RichBool ("Boundary",true,"1D Boundary Smoothing", "If true the boundary edges are smoothed only by themselves (e.g. the polyline forming the boundary of the mesh is independently smoothed). Can reduce the shrinking on the border but can have strange effects on very small boundaries."));
            parlst.addParam(new RichBool ("cotangentWeight",true,"Cotangent weighting", "If true the cotangente weighting scheme is computed for the averaging of the position. Otherwise (false) the simpler umbrella scheme (1 if the edge is present) is used."));
            parlst.addParam(new RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
            break;
        case FP_DEPTH_SMOOTH:
            parlst.addParam(new RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
            parlst.addParam(new RichPoint3f  ("viewPoint", Point3f(0,0,0),"Viewpoint", "The position of the view point that is used to get the constraint direction."));
            parlst.addParam(new RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
            break;
        case FP_DIRECTIONAL_PRESERVATION:
            parlst.addParam(new RichEnum("step", 0,
                                    QStringList() << "Store Vertex Position" << "Blend Vertex Position",
                                    tr("Step:"),
                                    tr("The purpose of this filter is to <b>constrain</b> any smoothing algorithm to moving vertices only along a give line of sight.<br> First you should store current vertex position, than after applying  one of the many smoothing algorithms you should re start this filter and blend the original positions with the smoothed results.<br>"
                                       "Given a view point  <i>vp</i> , the smoothed vertex position <i>vs</i> and the original position  <i>v</i>, The new vertex position is computed as the projection of  <i>vs</i> on the line  connecting  <i>v</i>  and <i>vp</i>.")));
            parlst.addParam(new RichPoint3f  ("viewPoint", Point3f(0,0,0),"Viewpoint", "The position of the view point that is used to get the constraint direction."));
            parlst.addParam(new RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
            break;
        case FP_TAUBIN_SMOOTH:
            parlst.addParam(new RichFloat("lambda", (float) 0.5,"Lambda", "The lambda parameter of the Taubin Smoothing algorithm"));
            parlst.addParam(new RichFloat("mu", (float) -0.53,"mu", "The mu parameter of the Taubin Smoothing algorithm"));
            parlst.addParam(new RichInt  ("stepSmoothNum", (int) 10,"Smoothing steps", "The number of times that the taubin smoothing is iterated. Usually it requires a larger number of iteration than the classical laplacian"));
            parlst.addParam(new RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
            break;
        case FP_SD_LAPLACIAN_SMOOTH:
        {
            parlst.addParam(new RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
            float maxVal = md.mm()->cm.bbox.Diag()/10;
          parlst.addParam(new RichAbsPerc("delta",maxVal*0.01,0,maxVal,"delta", ""));
      parlst.addParam(new RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
        }
        break;
        case FP_LINEAR_MORPH :
        {
            parlst.addParam(new RichMesh ("TargetMesh", md.mm(), &md,"Target Mesh", "The mesh that is the morph target."));

            parlst.addParam(new RichDynamicFloat("PercentMorph", 0.0, -150, 250,
            "% Morph", "The percent you want to morph toward (or away from) the target. <br>"
            "0 means current mesh <br>"
            "100 means targe mesh <br>"
            "<0 and >100 linearly extrapolate between the two mesh <br>"));
        }
        break;
    case FP_SCALAR_HARMONIC_FIELD:
		parlst.addParam(new RichPoint3f("point1",  md.mm()->cm.bbox.min, "Point 1", "A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichPoint3f("point2",  md.mm()->cm.bbox.max, "Point 2", "A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichDynamicFloat("value1", 0.0f, 0.0f, 1.0f, "value for the 1st point", "Harmonic field value for the vertex."));
		parlst.addParam(new RichDynamicFloat("value2", 1.0f, 0.0f, 1.0f, "value for the 2nd point", "Harmonic field value for the vertex."));
		parlst.addParam(new RichBool("colorize", true, "Colorize", "Colorize the mesh to provide an indication of the obtained harmonic field."));
		break;
  }
}

bool FilterUnsharp::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * cb)
{
    MeshModel &m=*(md.mm());
    switch(ID(filter))
    {
        case FP_CREASE_CUT :{
        if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm,false) > 0 || tri::Clean<CMeshO>::CountNonManifoldVertexFF(m.cm,false) > 0)
        {
          errorMessage = "Mesh has some not 2 manifold faces, this filter require manifoldness";
          return false; // can't continue, mesh can't be processed
        }

                float angleDeg = par.getFloat("angleDeg");
                tri::CreaseCut(m.cm, math::ToRad(angleDeg));
                m.clearDataMask(MeshModel::MM_FACEFACETOPO);
    }
            break;

  case FP_FACE_NORMAL_SMOOTHING :
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                 tri::Smooth<CMeshO>::FaceNormalLaplacianFF(m.cm);
             break;
  case FP_VERTEX_QUALITY_SMOOTHING :
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                 tri::Smooth<CMeshO>::VertexQualityLaplacian(m.cm);
             break;

    case FP_LAPLACIAN_SMOOTH :
      {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
      int stepSmoothNum = par.getInt("stepSmoothNum");
      bool Selected=par.getBool("Selected");
      if(Selected && m.cm.svn==0)
          m.cm.svn=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);

      bool boundarySmooth = par.getBool("Boundary");
      bool cotangentWeight = par.getBool("cotangentWeight");
      if(!boundarySmooth) tri::UpdateFlags<CMeshO>::FaceClearB(m.cm);

      tri::Smooth<CMeshO>::VertexCoordLaplacian(m.cm,stepSmoothNum,Selected,cotangentWeight,cb);
      Log( "Smoothed %d vertices", Selected>0 ? m.cm.svn : m.cm.vn);
      m.UpdateBoxAndNormals();
      }
        break;
    case FP_DEPTH_SMOOTH :
      {
            int stepSmoothNum = par.getInt("stepSmoothNum");
            size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
            //float delta = par.getAbsPerc("delta");
			Point3m viewpoint = par.getPoint3m("viewPoint");
            float alpha = 1;
            tri::Smooth<CMeshO>::VertexCoordViewDepth(m.cm,viewpoint,alpha,stepSmoothNum,true);
            Log( "depth Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);
            m.UpdateBoxAndNormals();
      }
        break;
    case FP_DIRECTIONAL_PRESERVATION:
      {
            const std::string AttribName("SavedVertPosition");
            int stepNum = par.getEnum("step");
			Point3m viewpoint = par.getPoint3m("viewPoint");
            float alpha = 1;

            switch (stepNum) {
                case 0: // ***** Storing Vertex Data *****
                {
                    if(tri::HasPerVertexAttribute(m.cm,AttribName)) 	{
                        vcg::tri::Allocator<CMeshO>::DeletePerVertexAttribute(m.cm,AttribName);
                    }
                    CMeshO::PerVertexAttributeHandle<Point3m> h = tri::Allocator<CMeshO>::AddPerVertexAttribute<Point3m> (m.cm,AttribName);
                    CMeshO::VertexIterator vi;

                    for(vi =m.cm.vert.begin();vi!= m.cm.vert.end();++vi)
                        h[vi] = vi->cP();

                    Log( "Stored Position %d vertices", m.cm.vn);
                    break;
                }
                case 1: // ***** Recovering and Projection Vertex Data *****
                {
                    if(!tri::HasPerVertexAttribute(m.cm,AttribName)) 	{
                        errorMessage = "Failed to retrieve the stored vertex position. First Store than recover.";
                        return false;
                    }
                    CMeshO::PerVertexAttributeHandle<Point3m> h = tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3m> (m.cm,AttribName);

                    CMeshO::VertexIterator vi;
                    for(vi= m.cm.vert.begin();vi!= m.cm.vert.end();++vi)
                    {
                        Point3m d = h[vi] - viewpoint; d.Normalize();
                        float s = d * ( (*vi).cP() - h[vi] );
                        (*vi).P() = h[vi] + d * (s*alpha);
                    }
                    m.UpdateBoxAndNormals();
                    Log(  "Projected smoothed Position %d vertices", m.cm.vn);
                }
                    break;
            }
      }
        break;
    case FP_SD_LAPLACIAN_SMOOTH:
      {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
            int stepSmoothNum = par.getInt("stepSmoothNum");
            size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
            // Small hack
            tri::UpdateFlags<CMeshO>::FaceClearB(m.cm);
            float delta = par.getAbsPerc("delta");
            tri::Smooth<CMeshO>::VertexCoordScaleDependentLaplacian_Fujiwara(m.cm,stepSmoothNum,delta);
            Log( "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);
            m.UpdateBoxAndNormals();
      }
        break;
    case FP_HC_LAPLACIAN_SMOOTH:
      {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
            size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
      tri::Smooth<CMeshO>::VertexCoordLaplacianHC(m.cm,1,cnt>0);
      m.UpdateBoxAndNormals();
      }
        break;
  case FP_TWO_STEP_SMOOTH:
      {
      tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
      tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
      int stepSmoothNum = par.getInt("stepSmoothNum");
      // sigma==0 all is smoothed
      // sigma==1 nothing is smoothed
      float sigma   = cos(math::ToRad(par.getFloat("normalThr")));
      if(sigma<0) sigma=0;

      int stepNormalNum = par.getInt("stepNormalNum");
      int stepFitNum = par.getInt("stepFitNum");
      bool selectedFlag = par.getBool("Selected");
      for(int i=0;i<stepSmoothNum;++i)
      {
        tri::UpdateNormal<CMeshO>::PerFaceNormalized(m.cm);
        tri::Smooth<CMeshO>::VertexCoordPasoDoble(m.cm, stepNormalNum, sigma, stepFitNum,selectedFlag);
      }
      m.UpdateBoxAndNormals();
    }
        break;
    case FP_TAUBIN_SMOOTH :
      {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
            int stepSmoothNum = par.getInt("stepSmoothNum");
            float lambda=par.getFloat("lambda");
            float mu=par.getFloat("mu");

            size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
      tri::Smooth<CMeshO>::VertexCoordTaubin(m.cm,stepSmoothNum,lambda,mu,cnt>0,cb);
            Log( "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);
            m.UpdateBoxAndNormals();
      }
            break;
    case FP_RECOMPUTE_FACE_NORMAL :
            tri::UpdateNormal<CMeshO>::PerFace(m.cm);
            break;
    case FP_RECOMPUTE_QUADFACE_NORMAL :
//            tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(m.cm);
            tri::UpdateNormal<CMeshO>::PerBitPolygonFaceNormalized(m.cm);
            break;
    case FP_RECOMPUTE_VERTEX_NORMAL :
              {
                int weightMode = par.getEnum("weightMode");
                switch(weightMode)
                {
                case 0: tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
                        tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
                        tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
                  break;
                case 1: tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
                        tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
                  break;
                case 2: tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(m.cm);
                        tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
                  break;
                case 3: tri::UpdateNormal<CMeshO>::PerVertexNelsonMaxWeighted(m.cm);
                        tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
                  break;
                default :
                  break;
                }
              }
            break;
    case FP_FACE_NORMAL_NORMALIZE :
            tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
             break;
    case FP_VERTEX_NORMAL_NORMALIZE :
            tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
             break;

    case FP_UNSHARP_NORMAL:
            {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                float alpha=par.getFloat("weight");
                float alphaorig=par.getFloat("weightOrig");
                int smoothIter = par.getInt("iterations");

                tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
                vector<Point3m> normalOrig(m.cm.fn);
                for(int i=0;i<m.cm.fn;++i)
                    normalOrig[i]=m.cm.face[i].cN();

                for(int i=0;i<smoothIter;++i)
                        tri::Smooth<CMeshO>::FaceNormalLaplacianFF(m.cm);

                for(int i=0;i<m.cm.fn;++i)
          m.cm.face[i].N() = normalOrig[i]*alphaorig + (normalOrig[i] - m.cm.face[i].N())*alpha;

            }	break;
    case FP_UNSHARP_GEOMETRY:
            {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                float alpha=par.getFloat("weight");
                float alphaorig=par.getFloat("weightOrig");
                int smoothIter = par.getInt("iterations");

                tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
                vector<Point3m> geomOrig(m.cm.vn);
                for(int i=0;i<m.cm.vn;++i)
                    geomOrig[i]=m.cm.vert[i].P();

                tri::Smooth<CMeshO>::VertexCoordLaplacian(m.cm,smoothIter);

                for(int i=0;i<m.cm.vn;++i)
                    m.cm.vert[i].P()=geomOrig[i]*alphaorig + (geomOrig[i] - m.cm.vert[i].P())*alpha;

                m.UpdateBoxAndNormals();

            }	break;
    case FP_UNSHARP_VERTEX_COLOR:
            {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                float alpha=par.getFloat("weight");
                float alphaorig=par.getFloat("weightOrig");
                int smoothIter = par.getInt("iterations");

                tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
                vector<Color4f> colorOrig(m.cm.vn);
                for(int i=0;i<m.cm.vn;++i)
                    colorOrig[i].Import(m.cm.vert[i].C());

                tri::Smooth<CMeshO>::VertexColorLaplacian(m.cm,smoothIter);
                for(int i=0;i<m.cm.vn;++i)
                    {
                        Color4f colorDelta = colorOrig[i] - Color4f::Construct(m.cm.vert[i].C());
                        Color4f newCol = 	colorOrig[i]*alphaorig + colorDelta*alpha;	 // Unsharp formula
                        Clamp(newCol); // Clamp everything in the 0..1 range
                        m.cm.vert[i].C().Import(newCol);

                        }
            }	break;
    case FP_UNSHARP_QUALITY:
    {
      tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
                float alpha=par.getFloat("weight");
                float alphaorig=par.getFloat("weightOrig");
                int smoothIter = par.getInt("iterations");

                tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
                vector<float> qualityOrig(m.cm.vn);
                for(int i=0;i<m.cm.vn;++i)
                    qualityOrig[i] = m.cm.vert[i].Q();

                tri::Smooth<CMeshO>::VertexQualityLaplacian(m.cm, smoothIter);
                for(int i=0;i<m.cm.vn;++i)
                {
                    float qualityDelta = qualityOrig[i] - m.cm.vert[i].Q();
                    m.cm.vert[i].Q() = 	qualityOrig[i]*alphaorig + qualityDelta*alpha;	 // Unsharp formula
                }
    }	break;

    case FP_LINEAR_MORPH:
    {
        CMeshO &targetMesh = par.getMesh("TargetMesh")->cm;
        CMeshO &sourceMesh = m.cm;

        //if the numbers of vertices dont match up
        if(sourceMesh.vn != targetMesh.vn)
        {
            errorMessage = "Number of vertices is not the same so you cant morph between these two meshes.";
            return false;
        }

        vcg::tri::Allocator<CMeshO>::CompactEveryVector(sourceMesh);
        vcg::tri::Allocator<CMeshO>::CompactEveryVector(targetMesh);
        float percentage = par.getDynamicFloat("PercentMorph")/100.f;

        int i;
        for(i=0;i<targetMesh.vn;++i)
        {
            CMeshO::CoordType &srcP =sourceMesh.vert[i].P();
            CMeshO::CoordType &trgP =targetMesh.vert[i].P();
            srcP = srcP + (trgP-srcP)*percentage;
        }

        m.UpdateBoxAndNormals();
    } break;
    case FP_SCALAR_HARMONIC_FIELD:
    {      
      typedef double FieldScalar;
      md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
      
      cb(1, "Computing harmonic field...");
      
      CMeshO & m = md.mm()->cm;
      vcg::tri::Allocator<CMeshO>::CompactEveryVector(m);      
      if (vcg::tri::Clean<CMeshO>::CountConnectedComponents(m) > 1) {
        this->errorMessage = "A mesh composed by a single connected component is required by the filter to properly work.";
        return false;
      }      
      if (vcg::tri::Clean<CMeshO>::CountNonManifoldEdgeFF(md.mm()->cm) > 0) {
        errorMessage = "Mesh has some not 2-manifold faces, this filter requires manifoldness";
        return false;
      }      
      if (vcg::tri::Clean<CMeshO>::CountNonManifoldVertexFF(md.mm()->cm) > 0)  {
        errorMessage = "Mesh has some not 2-manifold vertices, this filter requires manifoldness";
        return false;
      }
      
      md.mm()->updateDataMask(MeshModel::MM_VERTMARK | MeshModel::MM_FACEMARK | MeshModel::MM_FACEFLAG);
      // Get the two vertices with value set
      vcg::GridStaticPtr<CVertexO, Scalarm> vg;
      vg.Set(m.vert.begin(), m.vert.end());
      
      vcg::vertex::PointDistanceFunctor<Scalarm> pd;
      vcg::tri::Tmark<CMeshO, CVertexO> mv;
      mv.SetMesh(&m);
      mv.UnMarkAll();
      Point3m  closestP;
      Scalarm minDist = 0;
      CVertexO * vp0 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point1"), m.bbox.Diag(), minDist, closestP);
      CVertexO * vp1 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point2"), m.bbox.Diag(), minDist, closestP);
      if (vp0 == NULL || vp1 == NULL || vp0 == vp1)
      {
        this->errorMessage = "Error occurred for selected points.";
        return false;
      }
      
      vcg::tri::Harmonic<CMeshO, FieldScalar>::ConstraintVec constraints;
      constraints.push_back(vcg::tri::Harmonic<CMeshO, FieldScalar>::Constraint(vp0, FieldScalar(par.getFloat("value1"))));
      constraints.push_back(vcg::tri::Harmonic<CMeshO, FieldScalar>::Constraint(vp1, FieldScalar(par.getFloat("value2"))));
      
      CMeshO::PerVertexAttributeHandle<FieldScalar> handle = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<FieldScalar>(m, "harmonic");
      
      bool ok = vcg::tri::Harmonic<CMeshO, FieldScalar>::ComputeScalarField(m, constraints, handle);
      
      if (!ok)
      {
        this->errorMessage += "An error occurred.";
        return false;
      }
      md.mm()->updateDataMask(MeshModel::MM_VERTQUALITY);
      for (auto vi = m.vert.begin(); vi != m.vert.end(); ++vi)
        vi->Q() = handle[vi];
      
      if (par.getBool("colorize"))
      {
        md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
        vcg::tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m);
      }
      
      cb(100, "Done.");
      return true;
    }
    default : assert(0);
    }
    


    return true;
}

MeshFilterInterface::FILTER_ARITY FilterUnsharp::filterArity( QAction * filter ) const
{
    switch(ID(filter))
    {
    case FP_LAPLACIAN_SMOOTH :			
    case FP_HC_LAPLACIAN_SMOOTH :		
    case FP_SD_LAPLACIAN_SMOOTH :		
    case FP_TWO_STEP_SMOOTH :	    	
    case FP_TAUBIN_SMOOTH :				
    case FP_DEPTH_SMOOTH :				
    case FP_DIRECTIONAL_PRESERVATION :  
    case FP_CREASE_CUT :				
    case FP_FACE_NORMAL_NORMALIZE:		
    case FP_VERTEX_NORMAL_NORMALIZE:	
    case FP_FACE_NORMAL_SMOOTHING:	  
    case FP_VERTEX_QUALITY_SMOOTHING:	
    case FP_UNSHARP_NORMAL:				
    case FP_UNSHARP_GEOMETRY:			
    case FP_UNSHARP_QUALITY:			
    case FP_UNSHARP_VERTEX_COLOR:	    
    case FP_RECOMPUTE_VERTEX_NORMAL:	
    case FP_RECOMPUTE_FACE_NORMAL:      
    case FP_RECOMPUTE_QUADFACE_NORMAL:
    case FP_SCALAR_HARMONIC_FIELD:
        return MeshFilterInterface::SINGLE_MESH;
    case FP_LINEAR_MORPH :	
        return MeshFilterInterface::FIXED;
    }
    return MeshFilterInterface::NONE;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterUnsharp)
