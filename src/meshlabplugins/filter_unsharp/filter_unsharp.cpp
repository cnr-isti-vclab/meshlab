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
#include "filter_unsharp.h"

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/crease_cut.h>
#include <vcg/complex/algorithms/harmonic.h>

using namespace vcg;
using namespace std;

enum WeightModeParam {WMP_AVG = 0, WMP_AREA, WMP_ANGLE, WMP_AS_DEF};

FilterUnsharp::FilterUnsharp()
{
	typeList = {
		FP_CREASE_CUT,
		FP_LAPLACIAN_SMOOTH,
		FP_HC_LAPLACIAN_SMOOTH,
		FP_SD_LAPLACIAN_SMOOTH,
		FP_TWO_STEP_SMOOTH,
		FP_TAUBIN_SMOOTH,
		FP_DEPTH_SMOOTH,
		FP_DIRECTIONAL_PRESERVATION,
		FP_VERTEX_QUALITY_SMOOTHING,
		FP_FACE_NORMAL_SMOOTHING,
		FP_UNSHARP_NORMAL,
		FP_UNSHARP_GEOMETRY,
		FP_UNSHARP_QUALITY,
		FP_UNSHARP_VERTEX_COLOR,
		FP_RECOMPUTE_VERTEX_NORMAL,
		FP_RECOMPUTE_FACE_NORMAL,
		FP_RECOMPUTE_QUADFACE_NORMAL,
		FP_FACE_NORMAL_NORMALIZE,
		FP_VERTEX_NORMAL_NORMALIZE,
		FP_LINEAR_MORPH,
		FP_SCALAR_HARMONIC_FIELD
	};

	for(ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));

}

FilterUnsharp::~FilterUnsharp() {
}

QString FilterUnsharp::pluginName() const
{
    return "FilterUnsharp";
}

QString FilterUnsharp::filterName(ActionIDType filter) const
{
  switch(filter)
  {
  case FP_LAPLACIAN_SMOOTH :         return QString("Laplacian Smooth");
  case FP_HC_LAPLACIAN_SMOOTH :      return QString("HC Laplacian Smooth");
  case FP_SD_LAPLACIAN_SMOOTH :      return QString("ScaleDependent Laplacian Smooth");
  case FP_TWO_STEP_SMOOTH :          return QString("TwoStep Smooth");
  case FP_TAUBIN_SMOOTH :            return QString("Taubin Smooth");
  case FP_DEPTH_SMOOTH :             return QString("Depth Smooth");
  case FP_DIRECTIONAL_PRESERVATION : return QString("Directional Geom. Preserv.");
  case FP_CREASE_CUT :               return QString("Cut mesh along crease edges");
  case FP_FACE_NORMAL_NORMALIZE:     return QString("Normalize Face Normals");
  case FP_VERTEX_NORMAL_NORMALIZE:   return QString("Normalize Vertex Normals");
  case FP_FACE_NORMAL_SMOOTHING:     return QString("Smooth Face Normals");
  case FP_VERTEX_QUALITY_SMOOTHING:  return QString("Smooth Vertex Quality");
  case FP_UNSHARP_NORMAL:            return QString("UnSharp Mask Normals");
  case FP_UNSHARP_GEOMETRY:          return QString("UnSharp Mask Geometry");
  case FP_UNSHARP_QUALITY:           return QString("UnSharp Mask Quality");
  case FP_UNSHARP_VERTEX_COLOR:      return QString("UnSharp Mask Color");
  case FP_RECOMPUTE_VERTEX_NORMAL:   return QString("Re-Compute Vertex Normals");
  case FP_RECOMPUTE_FACE_NORMAL:     return QString("Re-Compute Face Normals");
  case FP_RECOMPUTE_QUADFACE_NORMAL: return QString("Re-Compute Per-Polygon Face Normals");
  case FP_LINEAR_MORPH :             return QString("Vertex Linear Morphing");
  case FP_SCALAR_HARMONIC_FIELD:     return QString("Generate Scalar Harmonic Field");

  default: assert(0);
  }
  return QString("Error on FilterUnsharp::filterName()");
}

QString FilterUnsharp::filterInfo(ActionIDType filterId) const
{
  switch(filterId)
  {
  case FP_LAPLACIAN_SMOOTH :          return tr("Laplacian smooth. Average each vertex position with weighted positions of neighbour vertices.<br>"
                                                "<b>Laplacian Mesh Processing</b> by <i>Olga Sorkine</i>. EUROGRAPHICS 2005<br>"
                                                "<a href='http://dx.doi.org/10.2312/egst.20051044'>doi:10.2312/egst.20051044</a>");
  case FP_HC_LAPLACIAN_SMOOTH :       return tr("HC Laplacian Smoothing. Extended version of Laplacian Smoothing based on the article: <br>"
                                                "<b>Improved Laplacian Smoothing of Noisy Surface Meshes</b> "
                                                "by <i>Vollmer, Mencl and Müller</i>. EUROGRAPHICS Volume 18 (1999), Number 3, 131-138.<br>"
                                                "<a href='https://doi.org/10.1111/1467-8659.00334'>doi:10.1111/1467-8659.00334</a>");
  case FP_SD_LAPLACIAN_SMOOTH :       return tr("Scale Dependent Laplacian Smoothing, extended version of Laplacian Smoothing based on the Fujiwara extended umbrella operator.<br>"
                                                "<b>Implicit Fairing of Irregular Meshes using Diffusion and Curvature Flow</b> "
                                                "by <i>Desbrun, Meyer, Schroeder and Barr</i>. SIGGRAPH 1999<br>"
                                                "<a href='https://doi.org/10.1145/311535.311576'>doi:10.1145/311535.311576</a>");
  case FP_TWO_STEP_SMOOTH :           return tr("Two Steps Smoothing, a feature preserving/enhancing fairing filter based on two stages:<ol><li>Normal Smoothing, where "
                                                "similar normals are averaged together.<li>Vertex reposition, where vertices are moved to fit on the new normals.</ol>"
                                                "<b>A Comparison of Mesh Smoothing Methods</b> by <i>A. Belyaev and Y. Ohtake</i>. "
                                                "Proc. Israel-Korea Bi-National Conf. Geometric Modeling and Computer Graphics, pp. 83-87, 2003.<br>"
                                                "<a href='https://www.researchgate.net/publication/47861030_A_comparison_of_mesh_smoothing_methods'>publication</a>");
  case FP_TAUBIN_SMOOTH :             return tr("The &lambda;-&mu; Taubin smoothing, combines two steps of low-pass filtering for each iteration. Based on the article:<br>"
                                                "<b>A signal processing approach to fair surface design</b> by <i>Gabriel Taubin</i>, SIGGRAPH 1995<br>"
                                                "<a href='https://doi.org/10.1145/218380.218473'>doi:10.1145/218380.218473</a>");
  case FP_DEPTH_SMOOTH :              return tr("A laplacian smooth that is constrained to move vertices only in one given direction (usually the viewer direction).");
  case FP_DIRECTIONAL_PRESERVATION :  return tr("Store and Blend the current geometry with the result of another previous smoothing processing step. This is useful to "
                                                "limit the influence of any smoothing algorithm along the viewing direction. This is useful to cope with the biased "
                                                "distribution of measuring error in many scanning devices, because TOF scanners usually have very good <i>x,y</i>"
                                                "accuracy but suffer of greater depth errors.");
  case FP_CREASE_CUT:                 return tr("Cut the mesh along crease edges, duplicating the vertices as necessary. Crease (or sharp) edges are defined according "
                                                "to the variation of normal of the adjacent faces.");
  case FP_FACE_NORMAL_NORMALIZE:      return tr("Normalize Face Normal Lengths to unit vectors.");
  case FP_VERTEX_NORMAL_NORMALIZE:    return tr("Normalize Vertex Normal Lengths to unit vectors.");
  case FP_VERTEX_QUALITY_SMOOTHING:	  return tr("Laplacian smooth of the <a href='https://stackoverflow.com/questions/58610746'>quality per vertex</a> values.");
  case FP_FACE_NORMAL_SMOOTHING:      return tr("Laplacian smooth of the face normals, without touching the position of the vertices.");
  case FP_UNSHARP_NORMAL:             return tr("Unsharp mask filtering of the normals per face, putting in more evidence normal variations.<br>"
                                                "<b>A simple normal enhancement technique for interactive non-photorealistic renderings</b> "
                                                "by <i>Cignoni, Scopigno and Tarini</i>, Comput Graph, 29 (1) (2005)<br>"                                                
                                                "<a href='https://doi.org/10.1016/j.cag.2004.11.012'>doi:10.1016/j.cag.2004.11.012</a>");
  case FP_UNSHARP_GEOMETRY:           return tr("Apply Unsharp filter to geometric shape, putting in more evidence ridges and valleys variations.<br>"
                                                "<a href='https://en.wikipedia.org/wiki/Unsharp_masking'>Unsharp Masking</a>");
  case FP_UNSHARP_QUALITY:            return tr("Apply Unsharp filter to values of <a href='https://stackoverflow.com/questions/58610746'>quality per vertex</a>.<br>"
                                                "See <a href='https://en.wikipedia.org/wiki/Unsharp_masking'>Unsharp Masking</a>");
  case FP_UNSHARP_VERTEX_COLOR:       return tr("Apply Unsharp filter to the vertex color, putting in more evidence color variations.<br>"
                                                "See <a href='https://en.wikipedia.org/wiki/Unsharp_masking'>Unsharp Masking</a>");
  case FP_RECOMPUTE_VERTEX_NORMAL:    return tr("Recompute vertex normals according to four different schemes:<br>"
                                                "1) Simple (no weights) average of normals of the incident faces <br>"
                                                "2) Area weighted average of normals of the incident faces <br>"
                                                "3) Angle weighted sum of normals of the incident faces according to the article <b>[1]</b>. "
                                                "Probably this is the best all-purpose choice. It could slightly bias the result for degenerate, fat triangles.<br>"
                                                "4) Weighted sum of normals of the incident faces, as defined by article <b>[2]</b>. "
                                                "The weight for each wedge is the cross product of the two edges over the product of the square of the two edge lengths."
                                                "According to the original article it is perfect only for spherical surface, but it should perform well also in practice.<br>"
                                                "[1]: <b>Computing Vertex Normals from Polygonal Facet</b> by <i>G Thurmer and CA Wuthrich</i>, JGT volume3, num 1. 1998<br>"
                                                "<a href='https://doi.org/10.1080/10867651.1998.10487487'>doi:10.1080/10867651.1998.10487487</a><br>"
                                                "[2]: <b>Weights for Computing Vertex Normals from Facet Normals</b> by <i>Nelson Max</i>, JGT vol4, num 2. 1999<br>"
                                                "<a href='https://doi.org/10.1080/10867651.1999.10487501'>doi:10.1080/10867651.1999.10487501</a>");
  case FP_RECOMPUTE_FACE_NORMAL:      return tr("Recompute face normals as the normal of the plane of the face.<br>"
                                                "See <a href='https://math.stackexchange.com/questions/305642/'>How to find surface normal of a triangle</a>");
  case FP_RECOMPUTE_QUADFACE_NORMAL:  return tr("Recompute face normals as the average of the normals of the triangles that builds a polygon. "
                                                "Useful for showing uniformly shaded quad or polygonal meshes represented using "
                                                "<a href='https://stackoverflow.com/questions/59392193'>faux edges</a>.");
  case FP_LINEAR_MORPH :              return tr("Morph deformation of current mesh towards a target mesh with the same number of vertices and same vertex ordering. "
                                                "Each vertex of the source mesh is linearly interpolated towards the corresponding vertex on the target mesh "
                                                "using the formula:<p align='center'>result<sub><big>i</big></sub> = (1-&lambda;)*source<sub><big>i</big></sub> + &lambda;*target<sub><big>i</big></sub></p>"
                                                "<b>Three-dimensional metamorphosis: a survey</b><br>by <i>F. Lazarus and A. Verroust</i>, Visual Computer, 1998<br>"
                                                "<a href='https://doi.org/10.1007/s003710050149'>doi:10.1007/s003710050149</a>");
  case FP_SCALAR_HARMONIC_FIELD: return QString("Generates a scalar harmonic field over the mesh. Input scalar values must be assigned to two vertices "
                                                "as Dirichlet boundary conditions. Applying the filter, a discrete Laplace operator generates the harmonic "
                                                "field values for all the mesh vertices, which are stored in the "
                                                "<a href='https://stackoverflow.com/questions/58610746'>quality per vertex attribute</a> of the mesh.<br>"
                                                "For more details see:"
                                                "<b>Dynamic Harmonic Fields for Surface Processing</b> by <i>Kai Xua, Hao Zhang, Daniel Cohen-Or, Yueshan Xionga</i>. "
                                                "Computers & Graphics, 2009 <br>"
                                                "<a href='https://doi.org/10.1016/j.cag.2009.03.022'>doi:10.1016/j.cag.2009.03.022</a>");
  default: assert(0);
  }
  return QString("Error on FilterUnsharp::filterInfo()!");
}

 FilterUnsharp::FilterClass FilterUnsharp::getClass(const QAction *a) const
{
  switch(ID(a))
  {
            case FP_CREASE_CUT :
            return FilterPlugin::FilterClass( 	FilterPlugin::Normal | FilterPlugin::Remeshing);
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
                    return 	FilterPlugin::Smoothing;

            case FP_UNSHARP_VERTEX_COLOR:
                    return FilterPlugin::FilterClass( 	FilterPlugin::Smoothing | FilterPlugin::VertexColoring);

            case FP_RECOMPUTE_FACE_NORMAL :
            case FP_RECOMPUTE_QUADFACE_NORMAL :
            case FP_RECOMPUTE_VERTEX_NORMAL :
            case FP_FACE_NORMAL_NORMALIZE:
            case FP_VERTEX_NORMAL_NORMALIZE:
                    return FilterPlugin::Normal;
  case FP_SCALAR_HARMONIC_FIELD: return FilterPlugin::Remeshing;

    default : return FilterPlugin::Generic;
  }
}
int FilterUnsharp::getPreConditions(const QAction *a) const
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


int FilterUnsharp::postCondition(const QAction *a) const
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

 int FilterUnsharp::getRequirements(const QAction *action)
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

void FilterUnsharp::initParameterList(const QAction *action, MeshDocument &md, RichParameterList & parlst)
{
    switch(ID(action))
    {
        case FP_RECOMPUTE_VERTEX_NORMAL :
            parlst.addParam(RichEnum("weightMode", 0, QStringList() << "Simple Average" <<  "By Area" << "By Angle" << "As defined by N. Max",  tr("Weighting Mode:"), ""));
          break;
        case FP_CREASE_CUT :
            parlst.addParam(RichFloat("angleDeg", 90.f, tr("Crease Angle (degree)"), tr("If the angle between the normals of two adjacent faces is <b>larger</b> that this threshold the edge is considered a creased and the mesh is cut along it.")));
            break;
        case FP_UNSHARP_NORMAL:
            parlst.addParam(RichBool("recalc", false, tr("Recompute Normals"), tr("Recompute normals from scratch before the unsharp masking")));
            parlst.addParam(RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)</i><br>")));
            parlst.addParam(RichFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation.<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(RichInt("iterations", 5, "Smooth Iterations", 	tr("number of laplacian face smooth iterations in every run")));
        break;
        case FP_UNSHARP_GEOMETRY:
            parlst.addParam(RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)</i><br>")));
            parlst.addParam(RichFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(RichInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run")));
            break;
        case FP_UNSHARP_VERTEX_COLOR:
            parlst.addParam(RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)</i><br>")));
            parlst.addParam(RichFloat("weightOrig", 1.f, tr("Original Color Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(RichInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run")));
            break;
        case FP_UNSHARP_QUALITY:
            parlst.addParam(RichFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)</i><br>")));
            parlst.addParam(RichFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value.")));
            parlst.addParam(RichInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run")));
            break;
        case FP_TWO_STEP_SMOOTH:
        parlst.addParam(RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
        parlst.addParam(RichFloat("normalThr", (float) 60,"Feature Angle Threshold (deg)", "Specify a threshold angle (0..90) for features that you want to be preserved.<br>Features forming angles LARGER than the specified threshold will be preserved. <br> 0 -> no smoothing <br> 90 -> all faces will be smoothed"));
        parlst.addParam(RichInt  ("stepNormalNum", (int) 20,"Normal Smoothing steps", "Number of iterations of normal smoothing step. The larger the better and (the slower)"));
        parlst.addParam(RichInt  ("stepFitNum",    (int) 20,"Vertex Fitting steps", "Number of iterations of the vertex fitting procedure."));
        parlst.addParam(RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
        break;
        case FP_LAPLACIAN_SMOOTH:
            parlst.addParam(RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
            parlst.addParam(RichBool ("Boundary",true,"1D Boundary Smoothing", "Smooth boundary edges only by themselves (e.g. the polyline forming the boundary of the mesh is independently smoothed). This can reduce the shrinking on the border but can have strange effects on very small boundaries."));
            parlst.addParam(RichBool ("cotangentWeight",true,"Cotangent weighting", "Use cotangent weighting scheme for the averaging of the position. Otherwise the simpler umbrella scheme (1 if the edge is present) is used."));
            parlst.addParam(RichBool("Selected", md.mm()->cm.sfn>0, "Affect only selection", "If checked the filter is performed only on the selected area"));
            break;
        case FP_DEPTH_SMOOTH:
            parlst.addParam(RichInt     ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
            parlst.addParam(RichPoint3f ("viewPoint", Point3f(0,0,0),"Viewpoint", "The position of the view point that is used to get the constraint direction."));
            parlst.addParam(RichAbsPerc ("delta", 1.0, 0, 1.0, "Strength", "How much smoothing is applied: 0 (no smooth) e 1 (full smooth)"));
            parlst.addParam(RichBool    ("Selected",md.mm()->cm.sfn>0,"Affect only selection","If checked the filter is performed only on the selected area"));
            break;
        case FP_DIRECTIONAL_PRESERVATION:
            parlst.addParam(RichEnum("step", 0,
                                    QStringList() << "Store Vertex Position" << "Blend Vertex Position",
                                    tr("Step:"),
                                    tr("The purpose of this filter is to <b>constrain</b> any smoothing algorithm to moving vertices only along a give line of sight.<br> First you should store current vertex position, than after applying  one of the many smoothing algorithms you should re start this filter and blend the original positions with the smoothed results.<br>"
                                       "Given a view point  <i>vp</i> , the smoothed vertex position <i>vs</i> and the original position  <i>v</i>, The new vertex position is computed as the projection of  <i>vs</i> on the line  connecting  <i>v</i>  and <i>vp</i>.")));
            parlst.addParam(RichPoint3f  ("viewPoint", Point3f(0,0,0),"Viewpoint", "The position of the view point that is used to get the constraint direction."));
            parlst.addParam(RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
            break;
        case FP_TAUBIN_SMOOTH:
            parlst.addParam(RichFloat("lambda", (float) 0.5,"Lambda", "The lambda parameter of the Taubin Smoothing algorithm"));
            parlst.addParam(RichFloat("mu", (float) -0.53,"mu", "The mu parameter of the Taubin Smoothing algorithm"));
            parlst.addParam(RichInt  ("stepSmoothNum", (int) 10,"Smoothing steps", "The number of times that the taubin smoothing is iterated. Usually it requires a larger number of iteration than the classical laplacian"));
            parlst.addParam(RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
            break;
        case FP_SD_LAPLACIAN_SMOOTH:
        {
            parlst.addParam(RichInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
            float maxVal = md.mm()->cm.bbox.Diag()/10;
          parlst.addParam(RichAbsPerc("delta",maxVal*0.01,0,maxVal,"delta", ""));
      parlst.addParam(RichBool ("Selected",md.mm()->cm.sfn>0,"Affect only selected faces","If checked the filter is performed only on the selected faces"));
        }
        break;
        case FP_LINEAR_MORPH :
        {
			parlst.addParam(RichMesh ("TargetMesh", md.mm()->id(), &md,"Target Mesh", "The mesh that is the morph target."));
            parlst.addParam(RichDynamicFloat("PercentMorph", 0.0, -150, 250,
            "% Morph", tr("The percent you want to morph towards (or away from) the target. <br>"
            "0 means current mesh <br>"
            "100 means targe mesh <br>"
            "<0 and >100 linearly extrapolate between the two mesh <br>")));
        }
        break;
    case FP_SCALAR_HARMONIC_FIELD:
        parlst.addParam(RichPoint3f("point1",  md.mm()->cm.bbox.min, "Point 1", "A vertex on the mesh that represent one harmonic field boundary condition."));
        parlst.addParam(RichPoint3f("point2",  md.mm()->cm.bbox.max, "Point 2", "A vertex on the mesh that represent one harmonic field boundary condition."));
        parlst.addParam(RichDynamicFloat("value1", 0.0f, 0.0f, 1.0f, "value for the 1st point", "Harmonic field value for the vertex."));
        parlst.addParam(RichDynamicFloat("value2", 1.0f, 0.0f, 1.0f, "value for the 2nd point", "Harmonic field value for the vertex."));
        parlst.addParam(RichBool("colorize", true, "Colorize", "Colorize the mesh to provide an indication of the obtained harmonic field."));
		break;
  }
}

std::map<std::string, QVariant> FilterUnsharp::applyFilter(
		const QAction *filter,
		const RichParameterList & par,
		MeshDocument &md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos * cb)
{
	MeshModel &m=*(md.mm());
	switch(ID(filter))
	{
	case FP_CREASE_CUT :{
		if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm,false) > 0 || tri::Clean<CMeshO>::CountNonManifoldVertexFF(m.cm,false) > 0)
		{
			throw MLException("Mesh has some not 2 manifold faces, this filter require manifoldness");
		}

		Scalarm angleDeg = par.getFloat("angleDeg");
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
		log( "Smoothed %d vertices", Selected ? m.cm.svn : m.cm.vn);
		m.UpdateBoxAndNormals();
	}
		break;
	case FP_DEPTH_SMOOTH :
	{
		int stepSmoothNum = par.getInt("stepSmoothNum");
		bool Selected = par.getBool("Selected");
		if (Selected && m.cm.svn == 0)
			m.cm.svn = tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		Scalarm delta = par.getAbsPerc("delta");
		Point3m viewpoint = par.getPoint3m("viewPoint");
		tri::Smooth<CMeshO>::VertexCoordViewDepth(m.cm, viewpoint, delta, stepSmoothNum, Selected,true);
		log("depth Smoothed %d vertices", Selected ? m.cm.svn : m.cm.vn);
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

			log( "Stored Position %d vertices", m.cm.vn);
			break;
		}
		case 1: // ***** Recovering and Projection Vertex Data *****
		{
			if(!tri::HasPerVertexAttribute(m.cm,AttribName)) 	{
				throw MLException("Failed to retrieve the stored vertex position. First Store than recover.");
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
			log(  "Projected smoothed Position %d vertices", m.cm.vn);
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
		Scalarm delta = par.getAbsPerc("delta");
		tri::Smooth<CMeshO>::VertexCoordScaleDependentLaplacian_Fujiwara(m.cm,stepSmoothNum,delta);
		log( "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);
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
		Scalarm sigma   = cos(math::ToRad(par.getFloat("normalThr")));
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
		Scalarm lambda=par.getFloat("lambda");
		Scalarm mu=par.getFloat("mu");

		size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		tri::Smooth<CMeshO>::VertexCoordTaubin(m.cm,stepSmoothNum,lambda,mu,cnt>0,cb);
		log( "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);
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
		/** ToDo: This filter should NEVER modify per face normals... **/
		int weightMode = par.getEnum("weightMode");
		switch(weightMode)
		{
		case WMP_AVG:
			tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
			tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
			tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
			break;
		case WMP_AREA:
			tri::UpdateNormal<CMeshO>::NormalizePerFaceByArea(m.cm);
			tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
			tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
			break;
		case WMP_ANGLE:
			tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(m.cm);
			tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
			break;
		case WMP_AS_DEF:
			tri::UpdateNormal<CMeshO>::PerVertexNelsonMaxWeighted(m.cm);
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
		Scalarm alpha=par.getFloat("weight");
		Scalarm alphaorig=par.getFloat("weightOrig");
		int smoothIter = par.getInt("iterations");

		tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		vector<Point3m> normalOrig(m.cm.fn);

		//Save original normal per face
		for(int i=0;i<m.cm.fn;++i)
			normalOrig[i]=m.cm.face[i].cN();

		//Laplacian smooth of normal per face
		for(int i=0;i<smoothIter;++i)
			tri::Smooth<CMeshO>::FaceNormalLaplacianFF(m.cm);

		//Unsharp filter normal per face
		for(int i=0;i<m.cm.fn;++i)
			m.cm.face[i].N() = normalOrig[i]*alphaorig + (normalOrig[i] - m.cm.face[i].N())*alpha;

	}	break;
	case FP_UNSHARP_GEOMETRY:
	{
		tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
		tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
		Scalarm alpha=par.getFloat("weight");
		Scalarm alphaorig=par.getFloat("weightOrig");
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
		Scalarm alpha=par.getFloat("weight");
		Scalarm alphaorig=par.getFloat("weightOrig");
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
		Scalarm alpha=par.getFloat("weight");
		Scalarm alphaorig=par.getFloat("weightOrig");
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
		CMeshO &targetMesh = md.getMesh(par.getMeshId("TargetMesh"))->cm;
		CMeshO &sourceMesh = m.cm;

		//if the numbers of vertices don't match up
		if(sourceMesh.vn != targetMesh.vn)
		{
			throw MLException("Number of vertices is not the same so you can't morph between these two meshes.");
		}

		vcg::tri::Allocator<CMeshO>::CompactEveryVector(sourceMesh);
		vcg::tri::Allocator<CMeshO>::CompactEveryVector(targetMesh);
		Scalarm percentage = par.getDynamicFloat("PercentMorph")/100.f;

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
		typedef MESHLAB_SCALAR FieldScalar;
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);

		cb(1, "Computing harmonic field...");

		CMeshO & m = md.mm()->cm;
		vcg::tri::Allocator<CMeshO>::CompactEveryVector(m);
		if (vcg::tri::Clean<CMeshO>::CountConnectedComponents(m) > 1) {
			throw MLException("A mesh composed by a single connected component is required by the filter to properly work.");
		}
		if (vcg::tri::Clean<CMeshO>::CountNonManifoldEdgeFF(md.mm()->cm) > 0) {
			throw MLException("Mesh has some not 2-manifold faces, this filter requires manifoldness");
		}
		if (vcg::tri::Clean<CMeshO>::CountNonManifoldVertexFF(md.mm()->cm) > 0)  {
			throw MLException("Mesh has some not 2-manifold vertices, this filter requires manifoldness");
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
		CVertexO * vp0 = vcg::GridClosest(vg, pd, mv, par.getPoint3m("point1"), m.bbox.Diag(), minDist, closestP);
		CVertexO * vp1 = vcg::GridClosest(vg, pd, mv, par.getPoint3m("point2"), m.bbox.Diag(), minDist, closestP);
		if (vp0 == NULL || vp1 == NULL || vp0 == vp1)
		{
			throw MLException("Error occurred for selected points.");
		}

		vcg::tri::Harmonic<CMeshO, FieldScalar>::ConstraintVec constraints;
		constraints.push_back(vcg::tri::Harmonic<CMeshO, FieldScalar>::Constraint(vp0, FieldScalar(par.getFloat("value1"))));
		constraints.push_back(vcg::tri::Harmonic<CMeshO, FieldScalar>::Constraint(vp1, FieldScalar(par.getFloat("value2"))));

		CMeshO::PerVertexAttributeHandle<FieldScalar> handle = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<FieldScalar>(m, "harmonic");

		bool ok = vcg::tri::Harmonic<CMeshO, FieldScalar>::ComputeScalarField(m, constraints, handle);

		if (!ok)
		{
			throw MLException("An error occurred.");
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
	} break;
	default :
		wrongActionCalled(filter);
	}
	return std::map<std::string, QVariant>();
}

FilterPlugin::FilterArity FilterUnsharp::filterArity(const QAction * filter ) const
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
        return FilterPlugin::SINGLE_MESH;
    case FP_LINEAR_MORPH :	
        return FilterPlugin::FIXED;
    }
    return FilterPlugin::NONE;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterUnsharp)
