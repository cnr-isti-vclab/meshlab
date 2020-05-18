/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
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

#include "meshfilter.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/hole.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <vcg/complex/algorithms/bitquad_creation.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/complex/algorithms/attribute_seam.h>
#include <vcg/complex/algorithms/update/curvature.h>
#include <vcg/complex/algorithms/update/curvature_fitting.h>
#include <vcg/complex/algorithms/pointcloud_normal.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/space/fitting3.h>
#include <wrap/gl/glu_tessellator_cap.h>
#include "quadric_simp.h"

using namespace std;
using namespace vcg;


ExtraMeshFilterPlugin::ExtraMeshFilterPlugin(void)
{
	typeList
	    << FP_LOOP_SS
	    << FP_BUTTERFLY_SS
	    << FP_CLUSTERING
	    << FP_QUADRIC_SIMPLIFICATION
	    << FP_QUADRIC_TEXCOORD_SIMPLIFICATION
	    << FP_EXPLICIT_ISOTROPIC_REMESHING
	    << FP_MIDPOINT
	    << FP_REORIENT
	    << FP_FLIP_AND_SWAP
	    << FP_ROTATE
	    << FP_ROTATE_FIT
	    << FP_PRINCIPAL_AXIS
	    << FP_SCALE
	    << FP_CENTER
	    << FP_INVERT_FACES
	    << FP_NORMAL_EXTRAPOLATION
	    << FP_NORMAL_SMOOTH_POINTCLOUD
	    << FP_COMPUTE_PRINC_CURV_DIR
	    << FP_CLOSE_HOLES
	    << FP_FREEZE_TRANSFORM
	    << FP_RESET_TRANSFORM
	    << FP_INVERT_TRANSFORM
	    << FP_SET_TRANSFORM_PARAMS
	    << FP_SET_TRANSFORM_MATRIX
	    << FP_CYLINDER_UNWRAP
	    << FP_REFINE_CATMULL
	    << FP_REFINE_HALF_CATMULL
	    << FP_QUAD_DOMINANT
	    << FP_MAKE_PURE_TRI
	    << FP_QUAD_PAIRING
	    << FP_FAUX_CREASE
	    << FP_FAUX_EXTRACT
	    << FP_VATTR_SEAM
	    << FP_REFINE_LS3_LOOP
	    << FP_SLICE_WITH_A_PLANE
	    << FP_PERIMETER_POLYLINE
	    ;

	FilterIDType tt;

	foreach(tt, types())
		actionList << new QAction(filterName(tt), this);

	tri::TriEdgeCollapseQuadricParameter lpp;
	lastq_QualityThr          = lpp.QualityThr;// 0.3f;
	lastq_PreserveBoundary    = false;
	lastq_PreserveNormal      = false;
	lastq_PreserveTopology    = false;
	lastq_OptimalPlacement    = true;
	lastq_Selected            = false;
	lastq_PlanarQuadric       = false;
	lastq_PlanarWeight        = lpp.QualityQuadricWeight;
	lastq_QualityWeight       = false;
	lastq_BoundaryWeight      = lpp.BoundaryQuadricWeight;
	lastqtex_QualityThr       = 0.3f;
	lastqtex_extratw          = 1.0;

	lastisor_Iterations          = 3;
	lastisor_RemeshingAdaptivity = false;
	lastisor_SelectedOnly        = false;
	lastisor_RefineFlag          = true;
	lastisor_CollapseFlag        = true;
	lastisor_SmoothFlag          = true;
	lastisor_SwapFlag            = true;
	lastisor_ProjectFlag         = true;
	lastisor_FeatureDeg          = 30.0f;
}

ExtraMeshFilterPlugin::FilterClass ExtraMeshFilterPlugin::getClass(QAction * a)
{
	switch (ID(a))
	{
		case FP_BUTTERFLY_SS                     :
		case FP_LOOP_SS                          :
		case FP_MIDPOINT                         :
		case FP_QUADRIC_SIMPLIFICATION           :
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  :
		case FP_EXPLICIT_ISOTROPIC_REMESHING     :
		case FP_CLUSTERING                       :
		case FP_CLOSE_HOLES                      :
		case FP_FAUX_CREASE                      :
		case FP_FAUX_EXTRACT                     :
		case FP_VATTR_SEAM                       :
		case FP_REFINE_LS3_LOOP	                 : return MeshFilterInterface::Remeshing;
		case FP_REFINE_CATMULL                   :
		case FP_REFINE_HALF_CATMULL              :
		case FP_QUAD_DOMINANT                    :
		case FP_MAKE_PURE_TRI                    :
		case FP_QUAD_PAIRING                     : return FilterClass(Remeshing+Polygonal);

		case FP_NORMAL_EXTRAPOLATION             : return FilterClass( Normal + PointSet );
		case FP_NORMAL_SMOOTH_POINTCLOUD         : return FilterClass( Normal + PointSet );

		case FP_INVERT_FACES                     :
		case FP_REORIENT                         :
		case FP_ROTATE                           :
		case FP_ROTATE_FIT                       :
		case FP_CENTER                           :
		case FP_SCALE                            :
		case FP_PRINCIPAL_AXIS                   :
		case FP_FLIP_AND_SWAP                    : return MeshFilterInterface::Normal;

		case FP_COMPUTE_PRINC_CURV_DIR           : return FilterClass( Normal + VertexColoring );

		case FP_FREEZE_TRANSFORM                 :
	case FP_INVERT_TRANSFORM                 :
	case FP_SET_TRANSFORM_PARAMS             :
	case FP_SET_TRANSFORM_MATRIX             :
		case FP_RESET_TRANSFORM                  : return FilterClass(Normal + Layer);

	case FP_PERIMETER_POLYLINE               :
		case FP_SLICE_WITH_A_PLANE               :
	case FP_CYLINDER_UNWRAP                  : return MeshFilterInterface::Measure;

		default                                  : assert(0); return MeshFilterInterface::Generic;
	}

	return MeshFilterInterface::Generic;
}

int ExtraMeshFilterPlugin::getPreCondition(QAction *filter) const
{
	switch (ID(filter))
	{
		case FP_MAKE_PURE_TRI					 : return MeshModel::MM_POLYGONAL;
		case FP_LOOP_SS                          :
		case FP_BUTTERFLY_SS                     :
		case FP_MIDPOINT                         :
		case FP_REFINE_CATMULL                   :
		case FP_QUADRIC_SIMPLIFICATION           :
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  :
		case FP_EXPLICIT_ISOTROPIC_REMESHING     :
		case FP_REORIENT                         :
		case FP_INVERT_FACES                     :
		case FP_COMPUTE_PRINC_CURV_DIR           :
		case FP_CLOSE_HOLES                      :
		case FP_CYLINDER_UNWRAP                  :
		case FP_REFINE_HALF_CATMULL              :
		case FP_QUAD_DOMINANT					 :
		case FP_QUAD_PAIRING                     :
		case FP_FAUX_CREASE                      :
		case FP_FAUX_EXTRACT                     :
		case FP_VATTR_SEAM                       :
		case FP_SLICE_WITH_A_PLANE               :
	case FP_PERIMETER_POLYLINE               :
		case FP_REFINE_LS3_LOOP                  : return MeshModel::MM_FACENUMBER;
		case FP_NORMAL_SMOOTH_POINTCLOUD         : return MeshModel::MM_VERTNORMAL;
		case FP_CLUSTERING                       :
		case FP_SCALE                            :
		case FP_CENTER                           :
		case FP_ROTATE                           :
		case FP_ROTATE_FIT                       :
		case FP_PRINCIPAL_AXIS                   :
		case FP_FLIP_AND_SWAP                    :
		case FP_FREEZE_TRANSFORM                 :
		case FP_RESET_TRANSFORM                  :
	case FP_INVERT_TRANSFORM                 :
	case FP_SET_TRANSFORM_PARAMS             :
	case FP_SET_TRANSFORM_MATRIX             :
		case FP_NORMAL_EXTRAPOLATION             : return MeshModel::MM_NONE;
	}
	return MeshModel::MM_NONE;
}

QString ExtraMeshFilterPlugin::filterName(FilterIDType filter) const
{
	switch (filter)
	{
		case FP_LOOP_SS                          : return tr("Subdivision Surfaces: Loop");
		case FP_BUTTERFLY_SS                     : return tr("Subdivision Surfaces: Butterfly Subdivision");
		case FP_MIDPOINT                         : return tr("Subdivision Surfaces: Midpoint");
		case FP_REFINE_CATMULL                   : return tr("Subdivision Surfaces: Catmull-Clark");
	case FP_QUADRIC_SIMPLIFICATION           : return tr("Simplification: Quadric Edge Collapse Decimation");
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  : return tr("Simplification: Quadric Edge Collapse Decimation (with texture)");
		case FP_EXPLICIT_ISOTROPIC_REMESHING     : return tr("Remeshing: Isotropic Explicit Remeshing");
		case FP_CLUSTERING                       : return tr("Simplification: Clustering Decimation");
		case FP_REORIENT                         : return tr("Re-Orient all faces coherentely");
		case FP_INVERT_FACES                     : return tr("Invert Faces Orientation");
		case FP_SCALE                            : return tr("Transform: Scale, Normalize");
		case FP_CENTER                           : return tr("Transform: Translate, Center, set Origin");
		case FP_ROTATE                           : return tr("Transform: Rotate");
		case FP_ROTATE_FIT                       : return tr("Transform: Rotate to Fit to a plane");
		case FP_PRINCIPAL_AXIS                   : return tr("Transform: Align to Principal Axis");
		case FP_FLIP_AND_SWAP                    : return tr("Transform: Flip and/or swap axis");
	case FP_FREEZE_TRANSFORM                 : return tr("Matrix: Freeze Current Matrix");
		case FP_RESET_TRANSFORM                  : return tr("Matrix: Reset Current Matrix");
	case FP_INVERT_TRANSFORM                 : return tr("Matrix: Invert Current Matrix");
	case FP_SET_TRANSFORM_PARAMS             : return tr("Matrix: Set from translation/rotation/scale");
	case FP_SET_TRANSFORM_MATRIX             : return tr("Matrix: Set/Copy Transformation");
		case FP_NORMAL_EXTRAPOLATION             : return tr("Compute normals for point sets");
		case FP_NORMAL_SMOOTH_POINTCLOUD         : return tr("Smooths normals on a point sets");
		case FP_COMPUTE_PRINC_CURV_DIR           : return tr("Compute curvature principal directions");
		case FP_CLOSE_HOLES                      : return tr("Close Holes");
		case FP_CYLINDER_UNWRAP                  : return tr("Geometric Cylindrical Unwrapping");
		case FP_REFINE_HALF_CATMULL              : return tr("Tri to Quad by 4-8 Subdivision");
		case FP_QUAD_DOMINANT                    : return tr("Turn into Quad-Dominant mesh");
		case FP_MAKE_PURE_TRI                    : return tr("Turn into a Pure-Triangular mesh");
		case FP_QUAD_PAIRING                     : return tr("Tri to Quad by smart triangle pairing");
		case FP_FAUX_CREASE                      : return tr("Select Crease Edges");
		case FP_FAUX_EXTRACT                     : return tr("Build a Polyline from Selected Edges");
		case FP_VATTR_SEAM                       : return tr("Vertex Attribute Seam");
		case FP_REFINE_LS3_LOOP                  : return tr("Subdivision Surfaces: LS3 Loop");
		case FP_SLICE_WITH_A_PLANE               : return tr("Compute Planar Section");
	case FP_PERIMETER_POLYLINE               : return tr("Create Selection Perimeter Polyline");

		default                                  : assert(0);
	}

	return tr("error!");
}

QString ExtraMeshFilterPlugin::filterInfo(FilterIDType filterID) const
{
	switch (filterID)
	{
	case FP_REFINE_LS3_LOOP                    : return tr("Apply LS3 Subdivision Surface algorithm using Loop's weights. This refinement method take normals into account. "
			                                               "<br>See:"
			                                               "<i>Boye', S. Guennebaud, G. & Schlick, C.</i> <br>"
			                                               "<b>Least squares subdivision surfaces</b><br>"
			                                               "Computer Graphics Forum, 2010.<br/><br/>"
			                                               "Alternatives weighting schemes are based on the paper: "
			                                               "<i>Barthe, L. & Kobbelt, L.</i><br>"
			                                               "<b>Subdivision scheme tuning around extraordinary vertices</b><br>"
			                                               "Computer Aided Geometric Design, 2004, 21, 561-583.<br/>"
			                                               "The current implementation of these schemes don't handle vertices of valence > 12");
		case FP_LOOP_SS                            : return tr("Apply Loop's Subdivision Surface algorithm. It is an approximant refinement method and it works for every triangle and has rules for extraordinary vertices.<br>");
		case FP_BUTTERFLY_SS                       : return tr("Apply Butterfly Subdivision Surface algorithm. It is an interpolated refinement method, defined on arbitrary triangular meshes. The scheme is known to be C1 but not C2 on regular meshes<br>");
		case FP_MIDPOINT                           : return tr("Apply a plain subdivision scheme where every edge is split on its midpoint. Useful to uniformly refine a mesh substituting each triangle with four smaller triangles.");
		case FP_REFINE_CATMULL                     : return tr("Apply the Catmull-Clark Subdivision Surfaces. Note that position of the new vertices is simply linearly interpolated. "
                                                           "If the mesh is triangle based (no <a href='https://stackoverflow.com/questions/59392193'>faux edges</a>) it generates a quad mesh, otherwise it honores it the faux-edge bits");
		case FP_REFINE_HALF_CATMULL                : return tr("Convert a tri mesh into a quad mesh by applying a 4-8 subdivision scheme."
			                                               "It introduces less overhead than the plain Catmull-Clark Subdivision Surfaces"
			                                               "(it adds only a single vertex for each triangle instead of four)."
			                                               "<br> See: <br>"
			                                               "<b>4-8 Subdivision</b>"
			                                               "<br> <i>Luiz Velho, Denis Zorin </i>"
			                                               "<br>CAGD, volume 18, Issue 5, Pages 397-427. ");
		case FP_CLUSTERING                         : return tr("Collapse vertices by creating a three dimensional grid enveloping the mesh and discretizes them based on the cells of this grid");
		case FP_QUADRIC_SIMPLIFICATION             : return tr("Simplify a mesh using a Quadric based Edge Collapse Strategy; better than clustering but slower");
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION    : return tr("Simplify a textured mesh using a Quadric based Edge Collapse Strategy preserving UV parametrization; better than clustering but slower");
		case FP_EXPLICIT_ISOTROPIC_REMESHING       : return tr("Perform a explicit remeshing of a triangular mesh, by repeatedly applying edge flip, collapse, relax and refine to improve aspect ratio (triangle quality) and topological regularity.");
		case FP_REORIENT                           : return tr("Re-orient in a consistent way all the faces of the mesh. <br>"
			                                               "The filter visits a mesh face to face, reorienting any unvisited face so that it is coherent "
			                                               "to the already visited faces. If the surface is orientable it will end with a consistent orientation of "
			                                               "all the faces. If the surface is not orientable (e.g. it is non manifold or non orientable like a moebius "
			                                               "strip) the filter will not build a consistent orientation simply because it is not possible. The filter can end up in a consistent orientation that can be exactly the opposite of the expected one; in that case simply invert the whole mesh orientation.");
		case FP_INVERT_FACES                       : return tr("Invert faces orientation, flipping the normals of the mesh. <br>"
			                                               "If requested, it tries to guess the right orientation; "
			                                               "mainly it decide to flip all the faces if the minimum/maximum vertices have not outward point normals for a few directions.<br>"
			                                               "Works well for single component watertight objects.");
		case FP_SCALE                              : return tr("Generate a matrix transformation that scale the mesh. The mesh can be also automatically scaled to a unit side box. ");
		case FP_CENTER                             : return tr("Generate a matrix transformation that translate the mesh. The mesh can be translated around one of the axis or a given axis and w.r.t. to the origin or the baricenter, or a given point.");
		case FP_ROTATE                             : return tr("Generate a matrix transformation that rotates the mesh. The mesh can be rotated around one of the axis or a given axis and w.r.t. to the origin or the baricenter, or a given point.");
		case FP_ROTATE_FIT                         : return tr("Generate a matrix transformation that rotates the mesh so that the selection fits one of the main planes XY YZ ZX. May also translate such that the selection centroid rest on the origin. It reports on the log the average error of the fitting (in mesh units).");
		case FP_PRINCIPAL_AXIS                     : return tr("Generate a matrix transformation that rotates the mesh aligning it to its principal axis of inertia."
			                                               "If the mesh is watertight the Itertia tensor is computed assuming the interior of the mesh has a uniform density."
			                                               "In case of an open mesh or a point clouds the inerta tensor is computed assuming each vertex is a constant puntual mass.");
		case FP_FLIP_AND_SWAP                      : return tr("Generate a matrix transformation that flips each one of the axis or swaps a couple of axis. The listed transformations are applied in that order. This kind of transformation cannot be applied to set of Raster!");
		case FP_RESET_TRANSFORM                    : return tr("Set the current transformation matrix to the Identity. ");
		case FP_FREEZE_TRANSFORM                   : return tr("Freeze the current transformation matrix into the coordinates of the vertices of the mesh (and set this matrix to the identity). In other words it applies in a definetive way the current matrix to the vertex coordinates.");
	case FP_INVERT_TRANSFORM                   : return tr("Invert the current transformation matrix. The current transformation is reversed, becoming its opposite.");
	case FP_SET_TRANSFORM_PARAMS               : return tr("Set the current transformation matrix starting from parameters: [XYZ] translation, [XYZ] Euler angles rotation and [XYZ] scaling.");
	case FP_SET_TRANSFORM_MATRIX               : return tr("Set the current transformation matrix by filling it, or copying from another layer.");
		case FP_NORMAL_EXTRAPOLATION               : return tr("Compute the normals of the vertices of a mesh without exploiting the triangle connectivity, useful for dataset with no faces");
		case FP_NORMAL_SMOOTH_POINTCLOUD           : return tr("Smooth the normals of the vertices of a mesh without exploiting the triangle connectivity, useful for dataset with no faces");
		case FP_COMPUTE_PRINC_CURV_DIR             : return tr("Compute the principal directions of curvature with several algorithms");
		case FP_CLOSE_HOLES                        : return tr("Close holes smaller than a given threshold");
		case FP_CYLINDER_UNWRAP                    : return tr("Unwrap the geometry of current mesh along a clylindrical equatorial projection. The cylindrical projection axis is centered on the origin and directed along the vertical <b>Y</b> axis.");
		case FP_QUAD_PAIRING                       : return tr("Convert a tri-mesh into a quad mesh by pairing triangles.");
		case FP_QUAD_DOMINANT                      : return tr("Convert a tri-mesh into a quad-dominant mesh by pairing suitable triangles.");
		case FP_MAKE_PURE_TRI                      : return tr("Convert into a tri-mesh by splitting any polygonal face.");
		case FP_FAUX_CREASE                        : return tr("It select the crease edges of a mesh according to edge dihedral angle.<br>"
			                                               "Angle between face normal is considered signed according to convexity/concavity."
			                                               "Convex angles are positive and concave are negative.");
		case FP_VATTR_SEAM                         : return tr("Make all selected vertex attributes connectivity-independent:<br/>"
			                                               "vertices are duplicated whenever two or more selected wedge or face attributes do not match.<br/>"
			                                               "This is particularly useful for GPU-friendly mesh layout, where a single index must be used to access all required vertex attributes.");
		case FP_SLICE_WITH_A_PLANE                 : return tr("Compute the polyline representing a planar section (a slice) of a mesh; if the resulting polyline is closed the result is filled and also a triangular mesh representing the section is saved");
	case FP_PERIMETER_POLYLINE                 : return tr("Create a new Layer with the perimeter polyline(s) of the selection borders");
		case FP_FAUX_EXTRACT                       : return tr("Create a new Layer with an edge mesh composed only by the selected edges of the current mesh");

		default                                  : assert(0);
	}

	return QString();
}

// this function builds and initializes with the default values (that can depend on the current mesh or selection)
// the list of parameters that a filter requires.
// return
//		true if has some parameters
//		false is has no params
void ExtraMeshFilterPlugin::initParameterSet(QAction * action, MeshModel & m, RichParameterSet & parlst)
{
	float maxVal;
	QStringList curvCalcMethods;
	QStringList curvColorMethods;
	QStringList loopWeightLst;

	switch(ID(action))
	{
		case FP_COMPUTE_PRINC_CURV_DIR:
			curvCalcMethods.push_back("Taubin approximation");
			curvCalcMethods.push_back("Principal Component Analysis");
			curvCalcMethods.push_back("Normal Cycles");
			curvCalcMethods.push_back("Pseudoinverse Quadric Fitting");
			curvColorMethods << "Mean Curvature"<<"Gaussian Curvature"<<"Min Curvature"<<"Max Curvature" << "Shape Index"<< "CurvedNess" <<"None";
			parlst.addParam(new RichEnum("Method", 3, curvCalcMethods, tr("Method:"), tr("Choose a method")));
			parlst.addParam(new RichEnum("CurvColorMethod", 0, curvColorMethods, tr("Quality/Color Mapping"), QString("Choose the curvature that is mapped into quality and visualized as per vertex color.")));
			parlst.addParam(new RichBool("Autoclean",true,"Remove Unreferenced Vertices","If selected, before starting the filter will remove anyy unreference vertex (for which curvature values are not defined)"));
			break;

		case FP_QUADRIC_SIMPLIFICATION:
			parlst.addParam(new RichInt  ("TargetFaceNum", (m.cm.sfn>0) ? m.cm.sfn/2 : m.cm.fn/2,"Target number of faces", "The desired final number of faces."));
			parlst.addParam(new RichFloat("TargetPerc", 0,"Percentage reduction (0..1)", "If non zero, this parameter specifies the desired final size of the mesh as a percentage of the initial size."));
			parlst.addParam(new RichFloat("QualityThr",lastq_QualityThr,"Quality threshold","Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n"));
			parlst.addParam(new RichBool ("PreserveBoundary",lastq_PreserveBoundary,"Preserve Boundary of the mesh","The simplification process tries to do not affect mesh boundaries during simplification"));
			parlst.addParam(new RichFloat("BoundaryWeight",lastq_BoundaryWeight,"Boundary Preserving Weight","The importance of the boundary during simplification. Default (1.0) means that the boundary has the same importance of the rest. Values greater than 1.0 raise boundary importance and has the effect of removing less vertices on the border. Admitted range of values (0,+inf). "));
			parlst.addParam(new RichBool ("PreserveNormal",lastq_PreserveNormal,"Preserve Normal","Try to avoid face flipping effects and try to preserve the original orientation of the surface"));
			parlst.addParam(new RichBool ("PreserveTopology",lastq_PreserveTopology,"Preserve Topology","Avoid all the collapses that should cause a topology change in the mesh (like closing holes, squeezing handles, etc). If checked the genus of the mesh should stay unchanged."));
			parlst.addParam(new RichBool ("OptimalPlacement",lastq_OptimalPlacement,"Optimal position of simplified vertices","Each collapsed vertex is placed in the position minimizing the quadric error.\n It can fail (creating bad spikes) in case of very flat areas. \nIf disabled edges are collapsed onto one of the two original vertices and the final mesh is composed by a subset of the original vertices. "));
			parlst.addParam(new RichBool ("PlanarQuadric",lastq_PlanarQuadric,"Planar Simplification","Add additional simplification constraints that improves the quality of the simplification of the planar portion of the mesh, as a side effect, more triangles will be preserved in flat areas (allowing better shaped triangles)."));
			parlst.addParam(new RichFloat("PlanarWeight",lastq_PlanarWeight,"Planar Simp. Weight","How much we should try to preserve the triangles in the planar regions. If you lower this value planar areas will be simplified more."));
			parlst.addParam(new RichBool ("QualityWeight",lastq_QualityWeight,"Weighted Simplification","Use the Per-Vertex quality as a weighting factor for the simplification. The weight is used as a error amplification value, so a vertex with a high quality value will not be simplified and a portion of the mesh with low quality values will be aggressively simplified."));
			parlst.addParam(new RichBool ("AutoClean",true,"Post-simplification cleaning","After the simplification an additional set of steps is performed to clean the mesh (unreferenced vertices, bad faces, etc)"));
			parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Simplify only selected faces","The simplification is applied only to the selected set of faces.\n Take care of the target number of faces!"));
			break;

		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:
			parlst.addParam(new RichInt  ("TargetFaceNum", (m.cm.sfn>0) ? m.cm.sfn/2 : m.cm.fn/2,"Target number of faces"));
			parlst.addParam(new RichFloat("TargetPerc", 0,"Percentage reduction (0..1)", "If non zero, this parameter specifies the desired final size of the mesh as a percentage of the initial mesh."));
			parlst.addParam(new RichFloat("QualityThr",lastqtex_QualityThr,"Quality threshold","Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n"));
			parlst.addParam(new RichFloat("Extratcoordw",lastqtex_extratw,"Texture Weight","Additional weight for each extra Texture Coordinates for every (selected) vertex"));
			parlst.addParam(new RichBool ("PreserveBoundary",lastq_PreserveBoundary,"Preserve Boundary of the mesh","The simplification process tries not to destroy mesh boundaries"));
			parlst.addParam(new RichFloat("BoundaryWeight",lastq_BoundaryWeight,"Boundary Preserving Weight","The importance of the boundary during simplification. Default (1.0) means that the boundary has the same importance of the rest. Values greater than 1.0 raise boundary importance and has the effect of removing less vertices on the border. Admitted range of values (0,+inf). "));
			parlst.addParam(new RichBool ("OptimalPlacement",lastq_OptimalPlacement,"Optimal position of simplified vertices","Each collapsed vertex is placed in the position minimizing the quadric error.\n It can fail (creating bad spikes) in case of very flat areas. \nIf disabled edges are collapsed onto one of the two original vertices and the final mesh is composed by a subset of the original vertices. "));
			parlst.addParam(new RichBool ("PreserveNormal",lastq_PreserveNormal,"Preserve Normal","Try to avoid face flipping effects and try to preserve the original orientation of the surface"));
			parlst.addParam(new RichBool ("PlanarQuadric",lastq_PlanarQuadric,"Planar Simplification","Add additional simplification constraints that improves the quality of the simplification of the planar portion of the mesh."));
			parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Simplify only selected faces","The simplification is applied only to the selected set of faces.\n Take care of the target number of faces!"));
			break;

		case FP_EXPLICIT_ISOTROPIC_REMESHING:
			parlst.addParam(new RichInt  ("Iterations", lastisor_Iterations, "Iterations", "Number of iterations of the remeshing operations to repeat on the mesh."));

			parlst.addParam(new RichBool ("Adaptive", lastisor_RemeshingAdaptivity, "Adaptive remeshing", "Toggles adaptive isotropic remeshing." ));
			parlst.addParam(new RichBool ("SelectedOnly", lastisor_SelectedOnly, "Remesh only selected faces", "If checked the remeshing operations will be applied only to the selected faces."));
			maxVal = m.cm.bbox.Diag();
			parlst.addParam(new RichAbsPerc("TargetLen",maxVal*0.01,0,maxVal,"Target Length", "Sets the target length for the remeshed mesh edges."));
			parlst.addParam(new RichFloat  ("FeatureDeg", lastisor_FeatureDeg, "Crease Angle", "Minimum angle between faces of the original to consider the shared edge as a feature to be preserved."));
			parlst.addParam(new RichBool ("CheckSurfDist", lastisor_CheckSurfDist, "Check Surface Distance", "If toggled each local operation must deviate from original mesh by [Max. surface distance]"));
			parlst.addParam(new RichAbsPerc ("MaxSurfDist", maxVal*0.01,0,maxVal, "Max. Surface Distance", "Maximal surface deviation allowed for each local operation"));
			parlst.addParam(new RichBool ("SplitFlag", lastisor_RefineFlag, "Refine Step", "If checked the remeshing operations will include a refine step."));
			parlst.addParam(new RichBool ("CollapseFlag", lastisor_CollapseFlag, "Collapse Step", "If checked the remeshing operations will include a collapse step."));
			parlst.addParam(new RichBool ("SwapFlag", lastisor_SwapFlag, "Edge-Swap Step", "If checked the remeshing operations will include a edge-swap step, aimed at improving the vertex valence of the resulting mesh."));
			parlst.addParam(new RichBool ("SmoothFlag", lastisor_SmoothFlag, "Smooth Step", "If checked the remeshing operations will include a smoothing step, aimed at relaxing the vertex positions in a Laplacian sense."));
			parlst.addParam(new RichBool ("ReprojectFlag", lastisor_ProjectFlag, "Reproject Step", "If checked the remeshing operations will include a step to reproject the mesh vertices on the original surface."));

			break;
		case FP_CLOSE_HOLES:
			parlst.addParam(new RichInt ("MaxHoleSize",(int)30,"Max size to be closed ","The size is expressed as number of edges composing the hole boundary"));
			parlst.addParam(new RichBool("Selected",m.cm.sfn>0,"Close holes with selected faces","Only the holes with at least one of the boundary faces selected are closed"));
			parlst.addParam(new RichBool("NewFaceSelected",true,"Select the newly created faces","After closing a hole the faces that have been created are left selected. Any previous selection is lost. Useful for example for smoothing the newly created holes."));
			parlst.addParam(new RichBool("SelfIntersection",true,"Prevent creation of selfIntersecting faces","When closing an holes it tries to prevent the creation of faces that intersect faces adjacent to the boundary of the hole. It is an heuristic, non intersetcting hole filling can be NP-complete."));
			break;

		case FP_LOOP_SS:
		case FP_REFINE_LS3_LOOP:
			loopWeightLst << "Loop" << "Enhance regularity" << "Enhance continuity";
			parlst.addParam(new RichEnum("LoopWeight", 0, loopWeightLst, "Weighting scheme", "Change the weights used. Allows one to optimize some behaviors over others."));
			// fall through
		case FP_BUTTERFLY_SS:
		case FP_MIDPOINT:
			parlst.addParam(new RichInt("Iterations", 3, "Iterations", "Number of time the model is subdivided."));
			maxVal = m.cm.bbox.Diag();
			parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the edges <b>longer</b> than this threshold will be refined.<br>Setting this value to zero will force an uniform refinement."));
			parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Affect only selected faces","If selected the filter affect only the selected faces"));
			break;


		case FP_CLUSTERING:
			maxVal = m.cm.bbox.Diag();
			parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Cell Size", "The size of the cell of the clustering grid. Smaller the cell finer the resulting mesh. For obtaining a very coarse mesh use larger values."));
			parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Affect only selected faces","If selected the filter affect only the selected faces"));
			break;

		case FP_CYLINDER_UNWRAP:
			parlst.addParam(new RichFloat("startAngle", 0,"Start angle (deg)", "The starting angle of the unrolling process."));
			parlst.addParam(new RichFloat("endAngle",360,"End angle (deg)","The ending angle of the unrolling process. Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n"));
			parlst.addParam(new RichFloat("radius", 0,"Projection Radius", "If non zero, this parameter specifies the desired radius of the reference cylinder used for the projection. Changing this parameter affect the <b>X</b> horizontal scaling of the resulting mesh. If zero (default) the average distance of the mesh from the axis is chosen."));
			break;

		case FP_FLIP_AND_SWAP:
			parlst.addParam(new RichBool ("flipX",false,"Flip X axis","If selected the axis will be swapped (mesh mirrored along the YZ plane"));
			parlst.addParam(new RichBool ("flipY",false,"Flip Y axis","If selected the axis will be swapped (mesh mirrored along the XZ plane"));
			parlst.addParam(new RichBool ("flipZ",false,"Flip Z axis","If selected the axis will be swapped (mesh mirrored along the XY plane"));
			parlst.addParam(new RichBool ("swapXY",false,"Swap X-Y axis","If selected the two axis will be swapped. All the swaps are performed in this order"));
			parlst.addParam(new RichBool ("swapXZ",false,"Swap X-Z axis","If selected the two axis will be swapped. All the swaps are performed in this order"));
			parlst.addParam(new RichBool ("swapYZ",false,"Swap Y-Z axis","If selected the two axis will be swapped. All the swaps are performed in this order"));
			parlst.addParam(new RichBool ("Freeze",true,"Freeze Matrix","The transformation is explicitly applied, and the vertex coordinates are actually changed"));
			parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			break;

		case FP_RESET_TRANSFORM:
			parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			break;

		case FP_FREEZE_TRANSFORM:
			parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			break;

		case FP_INVERT_TRANSFORM:
			parlst.addParam(new RichBool("Freeze", true, "Freeze Matrix", "The transformation is explicitly applied, and the vertex coordinates are actually changed"));
			parlst.addParam(new RichBool("allLayers", false, "Apply to all visible Layers", "If selected the filter will be applied to all visible mesh layers"));
			break;

		case FP_SET_TRANSFORM_MATRIX:
			{
				Matrix44m mat; mat.SetIdentity();
				parlst.addParam(new RichMatrix44f("TransformMatrix", mat, ""));
				parlst.addParam(new RichBool("compose", false, "Compose with current", "If selected, the new matrix will be composed with the current one (matrix=new*old)"));
				parlst.addParam(new RichBool("Freeze", true, "Freeze Matrix", "The transformation is explicitly applied, and the vertex coordinates are actually changed"));
				parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected, the filter will be applied to all visible mesh layers"));
			}
			break;

		case FP_SET_TRANSFORM_PARAMS:
			{
				parlst.addParam(new RichFloat("translationX", 0, "X Translation", "Translation factor on X axis"));
				parlst.addParam(new RichFloat("translationY", 0, "Y Translation", "Translation factor on Y axis"));
				parlst.addParam(new RichFloat("translationZ", 0, "Z Translation", "Translation factor on Z axis"));
				parlst.addParam(new RichFloat("rotationX", 0, "X Rotation", "Rotation angle on X axis"));
				parlst.addParam(new RichFloat("rotationY", 0, "Y Rotation", "Rotation angle on Y axis"));
				parlst.addParam(new RichFloat("rotationZ", 0, "Z Rotation", "Rotation angle on Z axis"));
				parlst.addParam(new RichFloat("scaleX", 1, "X Scale", "Scaling factor on X axis"));
				parlst.addParam(new RichFloat("scaleY", 1, "Y Scale", "Scaling factor on Y axis"));
				parlst.addParam(new RichFloat("scaleZ", 1, "Z Scale", "Scaling factor on Z axis"));
				parlst.addParam(new RichBool("compose", false, "Compose with current", "If selected, the new matrix will be composed with the current one (matrix=new*old)"));
				parlst.addParam(new RichBool("Freeze", true, "Freeze Matrix", "The transformation is explicitly applied, and the vertex coordinates are actually changed"));
				parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			}
			break;

		case FP_ROTATE_FIT:
			{
				QStringList planes;
				planes.push_back("XY plane");
				planes.push_back("YZ plane");
				planes.push_back("ZX plane");
				parlst.addParam(new RichEnum("targetPlane", 0, planes, "Rotate to fit:", "Choose the plane where the selection will fit"));
				QStringList raxis;
				raxis.push_back("any axis");
				raxis.push_back("X axis");
				raxis.push_back("Y axis");
				raxis.push_back("Z axis");
				parlst.addParam(new RichEnum("rotAxis", 0, raxis, "Rotate on:", "Choose on which axis do the rotation: 'any axis' guarantee the best fit of the selection to the plane, only use X,Y or Z it if you want to preserve that specific axis."));
				parlst.addParam(new RichBool("ToOrigin", true, "Move to Origin", "Also apply a translation, such that the centroid of selection rests on the Origin"));
				parlst.addParam(new RichBool("Freeze",true,"Freeze Matrix","The transformation is explicitly applied, and the vertex coordinates are actually changed"));
				parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			}
			break;

		case FP_ROTATE:
			{
				QStringList rotMethod;
				rotMethod.push_back("X axis");
				rotMethod.push_back("Y axis");
				rotMethod.push_back("Z axis");
				rotMethod.push_back("custom axis");
				parlst.addParam(new RichEnum("rotAxis", 0, rotMethod, tr("Rotation on:"), tr("Choose a method")));
				QStringList rotCenter;
				rotCenter.push_back("origin");
				rotCenter.push_back("barycenter");
				rotCenter.push_back("custom point");
				parlst.addParam(new RichEnum("rotCenter", 0, rotCenter, tr("Center of rotation:"), tr("Choose a method")));
				parlst.addParam(new RichDynamicFloat("angle",0,-360,360,"Rotation Angle","Angle of rotation (in <b>degree</b>). If snapping is enabled this value is rounded according to the snap value"));
				parlst.addParam(new RichPoint3f("customAxis",Point3f(0,0,0),"Custom axis","This rotation axis is used only if the 'custom axis' option is chosen."));
				parlst.addParam(new RichPoint3f("customCenter",Point3f(0,0,0),"Custom center","This rotation center is used only if the 'custom point' option is chosen."));
				parlst.addParam(new RichBool("snapFlag", false, "Snap angle", "If selected, before starting the filter will remove any unreferenced vertex (for which curvature values are not defined)"));
				parlst.addParam(new RichFloat("snapAngle",30,"Snapping Value","This value is used to snap the rotation angle (i.e. if the snapping value is 30, 227 becomes 210)."));
				parlst.addParam(new RichBool ("Freeze",true,"Freeze Matrix","The transformation is explicitly applied, and the vertex coordinates are actually changed"));
				parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			}
			break;

		case FP_PRINCIPAL_AXIS:
			parlst.addParam(new RichBool("pointsFlag",true,"Use vertex","If selected, only the vertices of the mesh are used to compute the Principal Axis. Mandatory for point clouds or for non water tight meshes"));
			parlst.addParam(new RichBool ("Freeze",true,"Freeze Matrix","The transformation is explicitly applied, and the vertex coordinates are actually changed"));
			parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			break;

		case FP_CENTER:
			{
				QStringList traslMethod;
				traslMethod.push_back("XYZ translation");
				traslMethod.push_back("Center on Scene BBox");
				traslMethod.push_back("Center on Layer BBox");
				traslMethod.push_back("Set new Origin");
				parlst.addParam(new RichEnum("traslMethod", 0, traslMethod, tr("Transformation:"), tr("[XYZ translation] adds X,Y and Z offset to Layer transformation, [Center on BBox] moves Layer Origin to the Bounding Box center, [Set new Origin] moves Layer Origin to a specific point")));
				Box3m &bb=m.cm.bbox;
				parlst.addParam(new RichDynamicFloat("axisX",0,-5.0*bb.Diag(),5.0*bb.Diag(),"X Axis","when using [XYZ translation], amount of translation along the X axis (in model units)"));
				parlst.addParam(new RichDynamicFloat("axisY",0,-5.0*bb.Diag(),5.0*bb.Diag(),"Y Axis","when using [XYZ translation], amount of translation along the Y axis (in model units)"));
				parlst.addParam(new RichDynamicFloat("axisZ",0,-5.0*bb.Diag(),5.0*bb.Diag(),"Z Axis","when using [XYZ translation], amount of translation along the Z axis (in model units)"));
				parlst.addParam(new RichPoint3f("newOrigin", Point3f(0, 0, 0), "New Origin:", "when using [Set new Origin], this is the location of the new Origin."));
				parlst.addParam(new RichBool ("Freeze",true,"Freeze Matrix","The transformation is explicitly applied, and the vertex coordinates are actually changed"));
				parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			}
			break;

		case FP_SCALE:
			{
				parlst.addParam(new RichFloat("axisX",1,"X Axis","Scaling"));
				parlst.addParam(new RichFloat("axisY",1,"Y Axis","Scaling"));
				parlst.addParam(new RichFloat("axisZ",1,"Z Axis","Scaling"));
				parlst.addParam(new RichBool("uniformFlag",true,"Uniform Scaling","If selected an uniform scaling (the same for all the three axis) is applied (the X axis value is used)"));
				QStringList scaleCenter;
				scaleCenter.push_back("origin");
				scaleCenter.push_back("barycenter");
				scaleCenter.push_back("custom point");
				parlst.addParam(new RichEnum("scaleCenter", 0, scaleCenter, tr("Center of scaling:"), tr("Choose a method")));
				parlst.addParam(new RichPoint3f("customCenter",Point3f(0,0,0),"Custom center","This scaling center is used only if the 'custom point' option is chosen."));
				parlst.addParam(new RichBool("unitFlag",false,"Scale to Unit bbox","If selected, the object is scaled to a box whose sides are at most 1 unit length"));
				parlst.addParam(new RichBool ("Freeze",true,"Freeze Matrix","The transformation is explicitly applied, and the vertex coordinates are actually changed"));
				parlst.addParam(new RichBool ("allLayers",false,"Apply to all visible Layers","If selected the filter will be applied to all visible mesh layers"));
			}
			break;

		case FP_INVERT_FACES:
			parlst.addParam(new RichBool("forceFlip", true, "Force Flip", "If selected, the normals will always be flipped; otherwise, the filter tries to set them outside"));
			parlst.addParam(new RichBool("onlySelected", false, "Flip only selected faces", "If selected, only selected faces will be affected"));
			break;

		case FP_FAUX_CREASE:
			parlst.addParam(new RichFloat ("AngleDegNeg",-45.0f,"Concave Angle Thr. (deg)","Concave Dihedral Angle threshold for considering an edge a crease. If the normals between two faces forms an concave diheadral angle smaller than the threshold the edge is considered a crease."));
			parlst.addParam(new RichFloat ("AngleDegPos", 45.0f,"Convex Angle Thr. (deg)","The angle threshold for considering an edge a crease. If the normals between two faces forms an angle larger than the threshold the edge is considered a crease."));
			break;

		case FP_NORMAL_EXTRAPOLATION:
			parlst.addParam(new RichInt ("K",(int)10,"Neighbour num","The number of neighbors used to estimate normals."));
			parlst.addParam(new RichInt ("smoothIter",0,"Smooth Iteration","The number of smoothing iteration done on the p used to estimate and propagate normals."));
			parlst.addParam(new RichBool("flipFlag",false,"Flip normals w.r.t. viewpoint","If the 'viewpoint' (i.e. scanner position) is known, it can be used to disambiguate normals orientation, so that all the normals will be oriented in the same direction."));
			parlst.addParam(new RichPoint3f("viewPos",m.cm.shot.Extrinsics.Tra(),"Viewpoint Pos.","The viewpoint position can be set by hand (i.e. getting the current viewpoint) or it can be retrieved from mesh camera, if the viewpoint position is stored there."));
			break;

		case FP_NORMAL_SMOOTH_POINTCLOUD:
			parlst.addParam(new RichInt ("K",(int)10,"Number of neighbors","The number of neighbors used to smooth normals."));
			parlst.addParam(new RichBool("useDist",false,"Weight using neighbour distance","If selected, the neighbour normals are waighted according to their distance"));
			break;

		case FP_VATTR_SEAM:
			{
				QStringList normalMethod; normalMethod << "None" << "Vertex" << "Wedge" << "Face";
				parlst.addParam(new RichEnum("NormalMode", 0, normalMethod, tr("Normal Source:"), tr("Choose a method")));
				QStringList colorMethod; colorMethod << "None" << "Vertex" << "Wedge" << "Face";
				parlst.addParam(new RichEnum("ColorMode", 0, colorMethod, tr("Color Source:"), tr("Choose a method")));
				QStringList texcoordMethod;texcoordMethod << "None" << "Vertex" << "Wedge";
				parlst.addParam(new RichEnum("TexcoordMode", 0, texcoordMethod, tr("Texcoord Source:"), tr("Choose a method")));
			}
			break;

		case FP_PERIMETER_POLYLINE:
			break;

		case FP_SLICE_WITH_A_PLANE:
			{
				QStringList axis = QStringList() <<"X Axis"<<"Y Axis"<<"Z Axis"<<"Custom Axis";
				parlst.addParam(new RichEnum   ("planeAxis", 0, axis, tr("Plane perpendicular to"), tr("The Slicing plane will be done perpendicular to the axis")));
				parlst.addParam(new RichPoint3f("customAxis",Point3f(0,1,0),"Custom axis","Specify a custom axis, this is only valid if the above parameter is set to Custom"));
				parlst.addParam(new RichFloat  ("planeOffset", 0.0, "Cross plane offset", "Specify an offset of the cross-plane. The offset corresponds to the distance from the point specified in the plane reference parameter. By default (Cross plane offset == 0)"));
				parlst.addParam(new RichEnum   ("relativeTo",2,QStringList()<<"Bounding box center"<<"Bounding box min"<<"Origin","plane reference","Specify the reference from which the planes are shifted"));
				parlst.addParam(new RichBool("createSectionSurface",false,"Create also section surface","If selected, in addition to a layer with the section polyline, it will be created also a layer with a triangulated version of the section polyline. This only works if the section polyline is closed"));
				parlst.addParam(new RichBool("splitSurfaceWithSection",false,"Create also split surfaces","If selected, it will create two layers with the portion of the mesh under and over the section plane. It requires manifoldness of the mesh."));
			}
			break;

		case FP_QUAD_DOMINANT:
			{
				QStringList opt = QStringList() <<"Fewest triangles"<< "(in between)" <<"Better quad shape";
				parlst.addParam(new RichEnum   ("level", 0, opt, tr("Optimize For:"), tr("Choose any of three different greedy strategies.")));
			}
			break;

		default:
			break;
	}
}


void Freeze(MeshModel *m)
{
  tri::UpdatePosition<CMeshO>::Matrix(m->cm, m->cm.Tr,true);
  tri::UpdateBounding<CMeshO>::Box(m->cm);
  m->cm.shot.ApplyRigidTransformation(m->cm.Tr);
  m->cm.Tr.SetIdentity();
}

void ApplyTransform(MeshDocument &md, const Matrix44m &tr, bool toAllFlag, bool freeze,
                    bool invertFlag=false, bool composeFlage=true)
{
  if(toAllFlag)
  {
	MeshModel   *m=NULL;
	while ((m=md.nextVisibleMesh(m)))
	{
		if(invertFlag) m->cm.Tr = Inverse(m->cm.Tr);
	  if(composeFlage) m->cm.Tr = tr * m->cm.Tr;
	  else m->cm.Tr=tr;
	  if(freeze) Freeze(m);
	}

	for (int i = 0; i < md.rasterList.size(); i++)
		if (md.rasterList[0]->visible)
			md.rasterList[i]->shot.ApplyRigidTransformation(tr);
  }
  else
  {
	MeshModel   *m=md.mm();
	if(invertFlag) m->cm.Tr = Inverse(m->cm.Tr);
	if(composeFlage) m->cm.Tr = tr * m->cm.Tr;
	else m->cm.Tr=tr;
	if(freeze) Freeze(md.mm());
  }
}


bool ExtraMeshFilterPlugin::applyFilter(QAction * filter, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb)
{
MeshModel & m = *md.mm();

switch(ID(filter))
{
	case  FP_LOOP_SS:
	case  FP_BUTTERFLY_SS:
	case  FP_MIDPOINT:
	case  FP_REFINE_LS3_LOOP:
	{
		tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
		m.updateDataMask( MeshModel::MM_FACEFACETOPO);
		tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);

		if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) > 0)
		{
			errorMessage = "Mesh has some not 2 manifoldfaces, subdivision surfaces require manifoldness"; // text
			return false; // can't continue, mesh can't be processed
		}

		bool  selected  = par.getBool("Selected");
		float threshold = par.getAbsPerc("Threshold");
		int iterations = par.getInt("Iterations");

		for(int i=0; i<iterations; ++i)
		{
			m.updateDataMask(MeshModel::MM_VERTFACETOPO);
			switch(ID(filter))
			{
			case FP_LOOP_SS :
				switch(par.getEnum("LoopWeight"))
				{
				case 0:
					tri::RefineOddEven<CMeshO/*, tri::OddPointLoop<CMeshO>, tri::EvenPointLoop<CMeshO>*/ >
					(m.cm, tri::OddPointLoop<CMeshO>(m.cm), tri::EvenPointLoop<CMeshO>(), threshold, selected, cb);
				break;
				case 1:
					tri::RefineOddEven<CMeshO/*,
						tri::OddPointLoopGeneric<CMeshO, Centroid<CMeshO>, RegularLoopWeight<CMeshO::ScalarType> >,
						tri::EvenPointLoopGeneric<CMeshO, Centroid<CMeshO>, RegularLoopWeight<CMeshO::ScalarType> >*/ >
					(m.cm, tri::OddPointLoopGeneric<CMeshO, Centroid<CMeshO>, RegularLoopWeight<CMeshO::ScalarType> >(m.cm),
					tri::EvenPointLoopGeneric<CMeshO, Centroid<CMeshO>, RegularLoopWeight<CMeshO::ScalarType> >(), threshold, selected, cb);
				break;
				case 2:
					tri::RefineOddEven<CMeshO/*,
						tri::OddPointLoopGeneric<CMeshO, Centroid<CMeshO>, ContinuityLoopWeight<CMeshO::ScalarType> >,
						tri::EvenPointLoopGeneric<CMeshO, Centroid<CMeshO>, ContinuityLoopWeight<CMeshO::ScalarType> >*/ >
					(m.cm, tri::OddPointLoopGeneric<CMeshO, Centroid<CMeshO>, ContinuityLoopWeight<CMeshO::ScalarType> >(m.cm),
					tri::EvenPointLoopGeneric<CMeshO, Centroid<CMeshO>, ContinuityLoopWeight<CMeshO::ScalarType> >(), threshold, selected, cb);
				break;
				}
			break;
			case FP_BUTTERFLY_SS :
				Refine<CMeshO,MidPointButterfly<CMeshO> > (m.cm, MidPointButterfly<CMeshO>(m.cm), threshold, selected, cb);
			break;
			case FP_MIDPOINT :
				Refine<CMeshO,MidPoint<CMeshO> > (m.cm, MidPoint<CMeshO>(&m.cm), threshold, selected, cb);
			break;
			case FP_REFINE_LS3_LOOP :
				switch(par.getEnum("LoopWeight"))
				{
				case 0:
					tri::RefineOddEven<CMeshO/*, tri::OddPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double> >, tri::EvenPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double> >*/ >
					(m.cm, tri::OddPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double> >(m.cm), tri::EvenPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double> >(), threshold, selected, cb);
				break;
				case 1:
					tri::RefineOddEven<CMeshO/*,
						tri::OddPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, RegularLoopWeight<double> >,
						tri::EvenPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, RegularLoopWeight<double> >*/ >
					(m.cm, tri::OddPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, RegularLoopWeight<double> >(m.cm),
					tri::EvenPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, RegularLoopWeight<double> >(), threshold, selected, cb);
				break;
				case 2:
					tri::RefineOddEven<CMeshO/*,
						tri::OddPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, ContinuityLoopWeight<double> >,
						tri::EvenPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, ContinuityLoopWeight<double> >*/ >
					(m.cm, tri::OddPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, ContinuityLoopWeight<double> >(m.cm),
					tri::EvenPointLoopGeneric<CMeshO, LS3Projection<CMeshO, double>, ContinuityLoopWeight<double> >(), threshold, selected, cb);
				break;
				}
			break;
			}

			m.clearDataMask(MeshModel::MM_VERTFACETOPO);
		}
		m.UpdateBoxAndNormals();
	    } break;

	case FP_REORIENT:
	{
		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		bool oriented, orientable;
		if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm)>0 )
		{
			errorMessage = "Mesh has some not 2-manifold faces, Orientability requires manifoldness"; // text
			return false; // can't continue, mesh can't be processed
		}
		tri::Clean<CMeshO>::OrientCoherentlyMesh(m.cm, oriented,orientable);
		tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		tri::UpdateTopology<CMeshO>::TestFaceFace(m.cm);
		m.UpdateBoxAndNormals();
	} break;

	case FP_CLUSTERING:
	{
		float threshold = par.getAbsPerc("Threshold");
		vcg::tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > ClusteringGrid;
		ClusteringGrid.Init(m.cm.bbox,100000,threshold);
		if(m.cm.FN() ==0)
			ClusteringGrid.AddPointSet(m.cm);
		else
			ClusteringGrid.AddMesh(m.cm);

		ClusteringGrid.ExtractMesh(m.cm);
		m.UpdateBoxAndNormals();
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
	} break;

	case FP_INVERT_FACES:
	{
		bool flipped=par.getBool("forceFlip");
		bool onlySelected=par.getBool("onlySelected");

		if(flipped)
			tri::Clean<CMeshO>::FlipMesh(m.cm,onlySelected);
		else
			flipped =  tri::Clean<CMeshO>::FlipNormalOutside(m.cm);
		m.UpdateBoxAndNormals();
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
	} break;

	case FP_RESET_TRANSFORM:
		ApplyTransform(md, Matrix44m::Identity(), par.getBool("allLayers"), false, false, false);
	break;

	case FP_FREEZE_TRANSFORM:
		ApplyTransform(md, Matrix44m::Identity(), par.getBool("allLayers"), true, false, true);
	break;

	case FP_INVERT_TRANSFORM:
		ApplyTransform(md, Matrix44m::Identity(), par.getBool("allLayers"), par.getBool("Freeze"), true, true);
	break;

	case FP_SET_TRANSFORM_MATRIX:
		ApplyTransform(md, par.getMatrix44m("TransformMatrix"), par.getBool("allLayers"), par.getBool("Freeze"), false, par.getBool("compose"));
	break;

	case FP_SET_TRANSFORM_PARAMS:
	{
		float tX = par.getFloat("translationX");
		float tY = par.getFloat("translationY");
		float tZ = par.getFloat("translationZ");
		float rX = par.getFloat("rotationX");
		float rY = par.getFloat("rotationY");
		float rZ = par.getFloat("rotationZ");
		float sX = par.getFloat("scaleX");
		float sY = par.getFloat("scaleY");
		float sZ = par.getFloat("scaleZ");

		Matrix44m newTransform = Matrix44m::Identity();
		Matrix44m tt;
		tt.SetTranslate(tX, tY, tZ);
		newTransform = newTransform * tt;

		if ((rX != 0.0) || (rY != 0.0) || (rZ != 0.0))
		{
		  tt.FromEulerAngles(math::ToRad(rX), math::ToRad(rY), math::ToRad(rZ));
		  newTransform = newTransform * tt;
		}

		if ((sX != 0.0) || (sY != 0.0) || (sZ != 0.0))
		{
		  tt.SetScale(sX, sY, sZ);
		  newTransform = newTransform * tt;
		}
		ApplyTransform(md, newTransform, par.getBool("allLayers"), par.getBool("Freeze"), false, par.getBool("compose"));
	}break;



	case FP_QUADRIC_SIMPLIFICATION:
	{
		m.updateDataMask( MeshModel::MM_VERTFACETOPO | MeshModel::MM_VERTMARK);
		tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);

		int TargetFaceNum = par.getInt("TargetFaceNum");
		if(par.getFloat("TargetPerc")!=0) TargetFaceNum = m.cm.fn*par.getFloat("TargetPerc");

		tri::TriEdgeCollapseQuadricParameter pp;
		pp.QualityThr=lastq_QualityThr =par.getFloat("QualityThr");
		pp.PreserveBoundary=lastq_PreserveBoundary = par.getBool("PreserveBoundary");
		pp.BoundaryQuadricWeight = pp.BoundaryQuadricWeight * par.getFloat("BoundaryWeight");
		pp.PreserveTopology=lastq_PreserveTopology = par.getBool("PreserveTopology");
		pp.QualityWeight=lastq_QualityWeight = par.getBool("QualityWeight");
		pp.NormalCheck=lastq_PreserveNormal = par.getBool("PreserveNormal");
		pp.OptimalPlacement=lastq_OptimalPlacement = par.getBool("OptimalPlacement");
		pp.QualityQuadric=lastq_PlanarQuadric = par.getBool("PlanarQuadric");
		pp.QualityQuadricWeight=lastq_PlanarWeight = par.getFloat("PlanarWeight");
		lastq_Selected = par.getBool("Selected");

		QuadricSimplification(m.cm,TargetFaceNum,lastq_Selected,pp,  cb);

		if(par.getBool("AutoClean"))
		{
			int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
			if(nullFaces) Log( "PostSimplification Cleaning: Removed %d null faces", nullFaces);
			int deldupvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
			if(deldupvert) Log( "PostSimplification Cleaning: Removed %d duplicated vertices", deldupvert);
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			if(delvert) Log( "PostSimplification Cleaning: Removed %d unreferenced vertices",delvert);
			m.clearDataMask(MeshModel::MM_FACEFACETOPO );
			tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		}

		m.UpdateBoxAndNormals();
		tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
		tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
		tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);

	} break;

	case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:
	{
		m.updateDataMask( MeshModel::MM_VERTFACETOPO | MeshModel::MM_VERTMARK);
		tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);

		if(!tri::HasPerWedgeTexCoord(m.cm))
		{
			errorMessage="Warning: nothing have been done. Mesh has no Texture.";
			return false;
		}
		if ( ! tri::Clean<CMeshO>::HasConsistentPerWedgeTexCoord(m.cm) ) {
			errorMessage = "Mesh has some inconsistent tex coordinates (some faces without texture)"; // text
			return false; // can't continue, mesh can't be processed
		}

		int TargetFaceNum = par.getInt("TargetFaceNum");
		if(par.getFloat("TargetPerc")!=0) TargetFaceNum = m.cm.fn*par.getFloat("TargetPerc");

		tri::TriEdgeCollapseQuadricTexParameter pp;

		lastqtex_QualityThr = pp.QualityThr = par.getFloat("QualityThr");
		lastqtex_extratw = pp.ExtraTCoordWeight = par.getFloat("Extratcoordw");
		lastq_OptimalPlacement = pp.OptimalPlacement = par.getBool("OptimalPlacement");
		lastq_PreserveBoundary = pp.PreserveBoundary = par.getBool("PreserveBoundary");
		pp.BoundaryWeight = pp.BoundaryWeight * par.getFloat("BoundaryWeight");
		lastq_PlanarQuadric  = pp.QualityQuadric = par.getBool("PlanarQuadric");
		lastq_PreserveNormal = pp.NormalCheck = par.getBool("PreserveNormal");

		lastq_Selected = par.getBool("Selected");

		QuadricTexSimplification(m.cm,TargetFaceNum,lastq_Selected, pp, cb);
		m.UpdateBoxAndNormals();
		tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
		tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
		tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
	} break;
	case FP_EXPLICIT_ISOTROPIC_REMESHING:
	{
		m.updateDataMask( MeshModel::MM_FACEFACETOPO  | MeshModel::MM_VERTFACETOPO | 
                          MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEMARK | 
                          MeshModel::MM_FACEFLAG | MeshModel::MM_VERTMARK );

		tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
		tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
		tri::Allocator<CMeshO>::CompactEveryVector(m.cm);

		m.UpdateBoxAndNormals();

		CMeshO toProjectCopy;

		toProjectCopy.face.EnableMark();

		tri::Append<CMeshO, CMeshO>::MeshCopy(toProjectCopy, m.cm);

		tri::IsotropicRemeshing<CMeshO>::Params params;
		params.SetTargetLen(par.getAbsPerc("TargetLen"));
		params.SetFeatureAngleDeg(par.getFloat("FeatureDeg"));


		params.maxSurfDist  = par.getFloat("MaxSurfDist");

		params.iter         = par.getInt("Iterations");
		params.adapt        = par.getBool("Adaptive");
		params.selectedOnly = par.getBool("SelectedOnly");
		params.splitFlag    = par.getBool("SplitFlag");
		params.collapseFlag = par.getBool("CollapseFlag");
		params.swapFlag     = par.getBool("SwapFlag");
		params.smoothFlag   = par.getBool("SmoothFlag");
		params.projectFlag  = par.getBool("ReprojectFlag");
		params.surfDistCheck= par.getBool("CheckSurfDist");

		lastisor_Iterations          = params.iter;
		lastisor_RemeshingAdaptivity = params.adapt;
		lastisor_SelectedOnly        = params.selectedOnly;
		lastisor_RefineFlag          = params.splitFlag;
		lastisor_CollapseFlag        = params.collapseFlag;
		lastisor_SwapFlag            = params.swapFlag;
		lastisor_SmoothFlag          = params.smoothFlag;
		lastisor_ProjectFlag         = params.projectFlag;
		lastisor_CheckSurfDist       = params.surfDistCheck;

		lastisor_MaxSurfDist= par.getFloat("MaxSurfDist");
		lastisor_FeatureDeg = par.getFloat("FeatureDeg");

		try
		{
			tri::IsotropicRemeshing<CMeshO>::Do(m.cm, toProjectCopy, params, cb);
		}
		catch(vcg::MissingPreconditionException& excp)
		{
			Log(excp.what());
			errorMessage = excp.what();
			return false;
		}
		m.UpdateBoxAndNormals();

//		m.clearDataMask(MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE | MeshModel::MM_FACEFACETOPO  | MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEMARK | MeshModel::MM_FACEFLAG);

	} break;

	case FP_ROTATE_FIT:
	{
		Box3m selBox; //boundingbox of the selected vertices
		std::vector< Point3m > selected_pts; //copy of selected vertices, for plane fitting

		if (m.cm.svn == 0 && m.cm.sfn == 0) // if no selection, fail
		{
			Log("Cannot compute rotation: there is no selection");
			errorMessage = "Cannot compute rotation: there is no selection";
			return false;
		}
		if (m.cm.svn == 0 && m.cm.sfn > 0) // if no vert selected, but some faces selected, use their vertices
		{
			tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
		}

		for (CMeshO::VertexIterator vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD() && (*vi).IsS())
			{
				Point3m p = (*vi).P();
				selBox.Add(p);
				selected_pts.push_back(p);
			}
		Log("Using %i vertices to build a fitting plane", int(selected_pts.size()));
		Plane3m plane;
		FitPlaneToPointSet(selected_pts, plane);
		float errorSum = 0;
		for (size_t i = 0; i < selected_pts.size(); ++i)
			errorSum += fabs(SignedDistancePlanePoint(plane, selected_pts[i]));
		Log("Fitting Plane avg error is %f", errorSum / float(selected_pts.size()));
		Log("Fitting Plane normal is [%f, %f, %f]", plane.Direction().X(), plane.Direction().Y(), plane.Direction().Z());

		Matrix44m tr1; // translation matrix the centroid of selected points
		tr1.SetTranslate(-selBox.Center());

		Point3m targetPlane;
		Point3m rotAxis;
		float angleRad;
		switch (par.getEnum("targetPlane"))
		{
			case 0:
				targetPlane = Point3m(0, 0, 1);
				break;
			case 1:
				targetPlane = Point3m(1, 0, 0);
				break;
			case 2:
				targetPlane = Point3m(0, 1, 0);
				break;
		}
		rotAxis = targetPlane ^ plane.Direction();
		angleRad = Angle(targetPlane, plane.Direction());

		if(par.getEnum("rotAxis")!=0)
		{
			Point3m projDir;
			switch (par.getEnum("rotAxis"))
			{
				case 1:
					rotAxis = -Point3m(1, 0, 0);
					projDir = Point3m(0.0, plane.Direction().Y(), plane.Direction().Z());
					break;
				case 2:
					rotAxis = -Point3m(0, 1, 0);
					projDir = Point3m(plane.Direction().X(), 0.0, plane.Direction().Z());
					break;
				case 3:
					rotAxis = -Point3m(0, 0, 1);
					projDir = Point3m(plane.Direction().X(), plane.Direction().Y(), 0.0);
					break;
			}
			angleRad = Angle(targetPlane, projDir);
			float angleSign = (targetPlane ^ projDir) * rotAxis;
			if (angleSign < 0)
				angleRad = -angleRad;
			else if (angleSign == 0)
				angleRad = 0;
		}

		rotAxis.Normalize();
		Matrix44m rt;
		rt.SetRotateRad(-angleRad, rotAxis);

		Log("Rotation axis is [%f, %f, %f]", rotAxis.X(), rotAxis.Y(), rotAxis.Z());
		Log("Rotation angle is %f", -angleRad);

		Matrix44m transfM;
		if (par.getBool("ToOrigin"))
			transfM = rt*tr1;
		else
			transfM = rt;

		ApplyTransform(md, transfM, par.getBool("allLayers"), par.getBool("Freeze"), false, false);
	} break;

	case FP_ROTATE :
	{
		Matrix44m trRot, trTran, trTranInv, transfM;
		Point3m axis, tranVec;

		switch(par.getEnum("rotAxis"))
		{
			case 0: axis=Point3m(1,0,0); break;
			case 1: axis=Point3m(0,1,0);break;
			case 2: axis=Point3m(0,0,1);break;
			case 3: axis=par.getPoint3m("customAxis");break;
		}
		switch(par.getEnum("rotCenter"))
		{
			case 0: tranVec=Point3m(0,0,0); break;
			case 1: tranVec= m.cm.Tr * m.cm.bbox.Center(); break;
			case 2: tranVec=par.getPoint3m("customCenter");break;
		}

		float angleDeg= par.getDynamicFloat("angle");
		float snapAngle = par.getFloat("snapAngle");
		if(par.getBool("snapFlag"))
		{
			angleDeg = floor(angleDeg / snapAngle)*snapAngle;
			par.setValue("angle", DynamicFloatValue(angleDeg));
		}

		trRot.SetRotateDeg(angleDeg,axis);
		trTran.SetTranslate(tranVec);
		trTranInv.SetTranslate(-tranVec);
		transfM = trTran*trRot*trTranInv;

		ApplyTransform(md,transfM,par.getBool("allLayers"),par.getBool("Freeze"));

	} break;

	case FP_PRINCIPAL_AXIS:
	{
		Matrix44m transfM; transfM.SetIdentity();

		if(par.getBool("pointsFlag"))
		{
			Matrix33m cov;
			Point3m bp(0,0,0);
			vector<Point3m> PtVec;
			for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end();++vi)
				if(!(*vi).IsD()) {
					PtVec.push_back((*vi).cP());
					bp+=(*vi).cP();
				}

			    bp/=m.cm.vn;

				cov.Covariance(PtVec,bp);
				for(int i=0;i<3;i++)
					qDebug("%8.3f %8.3f %8.3f ",cov[i][0],cov[i][1],cov[i][2]);
				qDebug("\n");
				Matrix33f eigenvecMatrix;
				Point3f eigenvecVector;
				Eigen::Matrix3d em;
				cov.ToEigenMatrix(em);
				Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig(em);
				Eigen::Vector3d c_val = eig.eigenvalues();
				Eigen::Matrix3d c_vec = eig.eigenvectors();

				eigenvecMatrix.FromEigenMatrix(c_vec);
				eigenvecVector.FromEigenVector(c_val);

				for(int i=0;i<3;i++)
					qDebug("%8.3f %8.3f %8.3f ",eigenvecMatrix[i][0],eigenvecMatrix[i][1],eigenvecMatrix[i][2]);

				qDebug("\n%8.3f %8.3f %8.3f ",eigenvecVector[0],eigenvecVector[1],eigenvecVector[2]);
				for(int i=0;i<3;++i)
					for(int j=0;j<3;++j)
						transfM[i][j] = eigenvecMatrix[i][j];
				transfM.transposeInPlace();
				if(transfM.Determinant()<0)
					for(int i=0;i<3;++i) transfM[2][i]=-transfM[2][i];

				qDebug("Determinant %f", transfM.Determinant());

		}
		else
		{
			tri::Inertia<CMeshO> I(m.cm);

			Matrix33m PCA;
			Point3m pcav;
			I.InertiaTensorEigen(PCA,pcav);
			for(int i=0;i<3;i++)
				qDebug("%8.3f %8.3f %8.3f",PCA[i][0],PCA[i][1],PCA[i][2]);
			PCA.transposeInPlace();
			qDebug("Determinant %f", PCA.Determinant());

			for(int i=0;i<3;i++)
				qDebug("%8.3f %8.3f %8.3f",PCA[i][0],PCA[i][1],PCA[i][2]);

			for(int i=0;i<3;++i)
				for(int j=0;j<3;++j)
					transfM[i][j] = PCA[i][j];
			if(transfM.Determinant()<0)
				for(int i=0;i<3;++i) transfM[2][i]=-transfM[2][i];

		}

		ApplyTransform(md, transfM, par.getBool("allLayers"), par.getBool("Freeze"), false, false);

	} break;

	case FP_CENTER:
	{
		Matrix44m transfM;
		Point3m translation(0.0, 0.0, 0.0);

		translation.X() = par.getDynamicFloat("axisX");
		translation.Y() = par.getDynamicFloat("axisY");
		translation.Z() = par.getDynamicFloat("axisZ");

		switch (par.getEnum("traslMethod"))
		{
			case 0: break; //we already got it from interface
			case 1: translation = -(m.cm.Tr * md.bbox().Center()); break; // we consider current transformation when finding bbox center
			case 2: translation = -(m.cm.Tr * m.cm.bbox.Center()); break; // we consider current transformation when finding bbox center
			case 3: translation = -par.getPoint3m("newOrigin"); break;
		}

		transfM.SetTranslate(translation);

		ApplyTransform(md,transfM,par.getBool("allLayers"),par.getBool("Freeze"));
	} break;

	case FP_SCALE:
	{
		Matrix44m transfM, scaleTran, trTran, trTranInv;
		Point3m tranVec;
		Box3m  scalebb;
		if(par.getBool("allLayers"))
			scalebb = md.bbox();
		else
			scalebb=md.mm()->cm.trBB();

		float xScale = par.getFloat("axisX");
		float yScale = par.getFloat("axisY");
		float zScale = par.getFloat("axisZ");

		if (par.getBool("uniformFlag"))
			scaleTran.SetScale(xScale, xScale, xScale);
		else
			scaleTran.SetScale(xScale, yScale, zScale);

		if (par.getBool("unitFlag"))
		{
			float maxSide = max(scalebb.DimX(), max(scalebb.DimY(), scalebb.DimZ()));
			scaleTran.SetScale(1.0 / maxSide, 1.0 / maxSide, 1.0 / maxSide);
		}

		switch (par.getEnum("scaleCenter"))
		{
			case 0: tranVec = Point3m(0, 0, 0); break;
			case 1: tranVec = m.cm.Tr * scalebb.Center(); break;
			case 2: tranVec = par.getPoint3m("customCenter"); break;
		}
		trTran.SetTranslate(tranVec);
		trTranInv.SetTranslate(-tranVec);

		transfM = trTran*scaleTran*trTranInv;

		ApplyTransform(md,transfM,par.getBool("allLayers"),par.getBool("Freeze"));
	} break;

	case FP_FLIP_AND_SWAP:
	{
		Matrix44m tr; tr.SetIdentity();
		if(par.getBool("flipX")) { Matrix44m flipM; flipM.SetIdentity(); flipM[0][0]=-1.0f; tr *= flipM; }
		if(par.getBool("flipY")) { Matrix44m flipM; flipM.SetIdentity(); flipM[1][1]=-1.0f; tr *= flipM; }
		if(par.getBool("flipZ")) { Matrix44m flipM; flipM.SetIdentity(); flipM[2][2]=-1.0f; tr *= flipM; }

		if(par.getBool("swapXY")) { Matrix44m swapM; swapM.SetIdentity();
			swapM[0][0]=0.0f; swapM[0][1]=1.0f;
		swapM[1][0]=1.0f; swapM[1][1]=0.0f;
		tr *= swapM; }
		if(par.getBool("swapXZ")) { Matrix44m swapM; swapM.SetIdentity();
			swapM[0][0]=0.0f; swapM[0][2]=1.0f;
		swapM[2][0]=1.0f; swapM[2][2]=0.0f;
		tr *= swapM; }
		if(par.getBool("swapYZ")) { Matrix44m swapM; swapM.SetIdentity();
			swapM[1][1]=0.0f; swapM[1][2]=1.0f;
		swapM[2][1]=1.0f; swapM[2][2]=0.0f;
		tr *= swapM; }

		ApplyTransform(md,tr,par.getBool("allLayers"),par.getBool("Freeze"));
	} break;

	case FP_NORMAL_EXTRAPOLATION :
	{
		tri::PointCloudNormal<CMeshO>::Param p;
		p.fittingAdjNum = par.getInt("K");
		p.smoothingIterNum = par.getInt("smoothIter");
		p.viewPoint = par.getPoint3m("viewPos");
		p.useViewPoint = par.getBool("flipFlag");
		tri::PointCloudNormal<CMeshO>::Compute(m.cm, p,cb);
	} break;

	case FP_NORMAL_SMOOTH_POINTCLOUD :
	{
		tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
		tri::Smooth<CMeshO>::VertexNormalPointCloud(m.cm,par.getInt("K"),1);
	} break;

	case FP_COMPUTE_PRINC_CURV_DIR:
	{
		m.updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEFACETOPO);
		m.updateDataMask(MeshModel::MM_VERTCURV | MeshModel::MM_VERTCURVDIR);
		m.updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
		if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0 ) {
			errorMessage = "Mesh has some not 2-manifold faces, cannot compute principal curvature directions"; // text
			return false; // can't continue, mesh can't be processed
		}
		tri::UpdateNormal<CMeshO>::NormalizePerVertex(m.cm);
		if(par.getBool("Autoclean")){
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			Log( "Removed %d unreferenced vertices",delvert);
		}
		switch(par.getEnum("Method"))
		{
			case 0:	tri::UpdateCurvature<CMeshO>::PrincipalDirections(m.cm); break;
			case 1: tri::UpdateCurvature<CMeshO>::PrincipalDirectionsPCA(m.cm,m.cm.bbox.Diag()/20.0,true,cb); break;
			case 2: tri::UpdateCurvature<CMeshO>::PrincipalDirectionsNormalCycle(m.cm); break;
			case 3: tri::UpdateCurvatureFitting<CMeshO>::computeCurvature(m.cm); break;
			default:assert(0);break;
		}
		switch(par.getEnum("CurvColorMethod"))
		{
			case 0: tri::UpdateQuality<CMeshO>::VertexFromMeanCurvatureDir    (m.cm); break;
			case 1: tri::UpdateQuality<CMeshO>::VertexFromGaussianCurvatureDir(m.cm); break;
			case 2: tri::UpdateQuality<CMeshO>::VertexFromMinCurvatureDir(m.cm); break;
			case 3: tri::UpdateQuality<CMeshO>::VertexFromMaxCurvatureDir(m.cm); break;
			case 4: tri::UpdateQuality<CMeshO>::VertexFromShapeIndexCurvatureDir(m.cm); break;
			case 5: tri::UpdateQuality<CMeshO>::VertexFromCurvednessCurvatureDir(m.cm); break;
			case 6: tri::UpdateQuality<CMeshO>::VertexConstant(m.cm,0); break;
		}

		Histogramf H;
		tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);
		tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm,H.Percentile(0.1f),H.Percentile(0.9f));

		Log( "Curvature Range: %f %f (Used 90 percentile %f %f) ",H.MinV(),H.MaxV(),H.Percentile(0.1f),H.Percentile(0.9f));
	} break;

	case FP_CLOSE_HOLES:
	{
		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) > 0){
			errorMessage = "Mesh has some not 2-manifold edges, filter requires edge manifoldness";
			return false;
		}

		size_t OriginalSize= m.cm.face.size();
		int MaxHoleSize = par.getInt("MaxHoleSize");
		bool SelectedFlag = par.getBool("Selected");
		bool SelfIntersectionFlag = par.getBool("SelfIntersection");
		bool NewFaceSelectedFlag = par.getBool("NewFaceSelected");
		int holeCnt;
		if( SelfIntersectionFlag )
			holeCnt = tri::Hole<CMeshO>::EarCuttingIntersectionFill<tri::SelfIntersectionEar< CMeshO> >(m.cm,MaxHoleSize,SelectedFlag,cb);
		else
			holeCnt = tri::Hole<CMeshO>::EarCuttingFill<vcg::tri::MinimumWeightEar< CMeshO> >(m.cm,MaxHoleSize,SelectedFlag,cb);
		Log("Closed %i holes and added %i new faces",holeCnt,m.cm.fn-OriginalSize);
		assert(tri::Clean<CMeshO>::IsFFAdjacencyConsistent(m.cm));
		m.UpdateBoxAndNormals();

		// hole filling filter does not correctly update the border flags (but the topology is still ok!)
		if(NewFaceSelectedFlag)
		{
			tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
			for(size_t i=OriginalSize;i<m.cm.face.size();++i)
				if(!m.cm.face[i].IsD()) m.cm.face[i].SetS();
		}
	} break;

	case FP_CYLINDER_UNWRAP:
	{
		float startAngleDeg = par.getFloat("startAngle");
		float endAngleDeg = par.getFloat("endAngle");
		float radius = par.getFloat("radius");

		// Number of unrolling. (e.g. if the user set start=-15 end=375 there are two loops)
		int numLoop =	int(1+(endAngleDeg-startAngleDeg)/360.0);

		vector< vector<int> > VertRefLoop(numLoop);
		for(int i=0;i<numLoop;++i)
			VertRefLoop[i].resize(m.cm.vert.size(),-1);

		Log("Computing %i loops from %f to %f",numLoop,startAngleDeg,endAngleDeg);

		MeshModel *um=md.addNewMesh("","Unrolled Mesh");
		um->updateDataMask(&m);
		um->cm.textures = m.cm.textures;
		float avgZ=0;
		CMeshO::VertexIterator vi;
		// First loop duplicate accordingly the vertices.
		for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
			if(!(*vi).IsD())
			{
				Point3m p = (*vi).P();
				CMeshO::ScalarType ro,theta,phi;
				p.Y()=0;
				p.ToPolarRad(ro,theta,phi);
				float thetaDeg = math::ToDeg(theta);
				int loopIndex =0;
				while(thetaDeg<endAngleDeg)
				{
					if(thetaDeg>=startAngleDeg)
					{
						CMeshO::VertexIterator nvi = tri::Allocator<CMeshO>::AddVertices(um->cm,1);
						VertRefLoop[loopIndex][vi-m.cm.vert.begin()] = nvi - um->cm.vert.begin();
						nvi->ImportData(*vi);
						nvi->P().X()=-math::ToRad(thetaDeg);
						nvi->P().Y()=vi->P().Y();
						nvi->P().Z()=ro;
						//nvi->N()=(*vi).N();
						nvi->C()=(*vi).C();
						avgZ += nvi->P().Z();
					}
					thetaDeg+=360;
					loopIndex++;
				}
			}

		    // Now correct the x width with the average radius
		    avgZ = avgZ/um->cm.vert.size();
			if(radius != 0) avgZ = radius; // if the user desire to override that value.

			for(vi=um->cm.vert.begin();vi!=um->cm.vert.end();++vi)
				vi->P().X()*=avgZ;

			// Second Loop Process Faces
			// Note the particolar care to manage the faces that jumps from one side to another.
			CMeshO::FaceIterator fi;
			for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
				if(!(*fi).IsD())
				{
					int loopIndex=0;
					while(loopIndex<numLoop)
					{
						int endIt = min(2,numLoop-loopIndex);
						for(int ii0=0;ii0<endIt;ii0++)
						{
							for(int ii1=0;ii1<endIt;ii1++)
							{
								for(int ii2=0;ii2<endIt;ii2++)
								{
									int i0 = VertRefLoop[loopIndex+ii0][(*fi).V(0)-&m.cm.vert[0]];
									int i1 = VertRefLoop[loopIndex+ii1][(*fi).V(1)-&m.cm.vert[0]];
									int i2 = VertRefLoop[loopIndex+ii2][(*fi).V(2)-&m.cm.vert[0]];
									if(i0>=0 && i1>=0 && i2>=0)
									{
										// skip faces larger than 1/10 of the radius...
										if( (Distance(um->cm.vert[i0].P(),um->cm.vert[i1].P()) < avgZ/10.0) &&
										    (Distance(um->cm.vert[i0].P(),um->cm.vert[i2].P()) < avgZ/10.0) )
										{
											CMeshO::FaceIterator nfi = tri::Allocator<CMeshO>::AddFaces(um->cm,1);
											(*nfi).ImportData(*fi);
											nfi->V(0) = &um->cm.vert[i0];
											nfi->V(1) = &um->cm.vert[i1];
											nfi->V(2) = &um->cm.vert[i2];
										}
									}
								}
							}
						}
						loopIndex++;
					}
				}
		m.UpdateBoxAndNormals();
	} break;

	case FP_REFINE_HALF_CATMULL:
	{
		if (!vcg::tri::BitQuadCreation<CMeshO>::IsTriQuadOnly(m.cm))
		{
			errorMessage = "To be applied filter <i>" + filter->text() + "</i> requires a mesh with only triangular and/or quad faces.";
			return false;
		}
		m.updateDataMask(MeshModel::MM_FACEQUALITY | MeshModel::MM_FACEFACETOPO);
		tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
		tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(m.cm);
		m.clearDataMask( MeshModel::MM_FACEFACETOPO);
		m.updateDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case FP_REFINE_CATMULL :
	{
		if (!vcg::tri::BitQuadCreation<CMeshO>::IsTriQuadOnly(m.cm))
		{
			errorMessage = "To be applied filter <i>" + filter->text() + "</i> requires a mesh with only triangular and/or quad faces.";
			return false;
		}
		// in practice it is just a simple double application of the FP_REFINE_HALF_CATMULL.
		m.updateDataMask(MeshModel::MM_FACEQUALITY | MeshModel::MM_FACEFACETOPO);
		tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
		tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
		tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(m.cm);
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
		m.updateDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case FP_QUAD_PAIRING :
	{
		m.updateDataMask(MeshModel::MM_FACEQUALITY | MeshModel::MM_FACEFACETOPO );
		if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) > 0)
		{
			errorMessage = "Mesh has some not 2 manifoldfaces, filter requires manifoldness"; // text
			return false;
		}
		tri::BitQuadCreation<CMeshO>::MakeTriEvenBySplit(m.cm);
		bool ret = tri::BitQuadCreation<CMeshO>::MakePureByFlip(m.cm,100);
		if(!ret) Log("Warning BitQuadCreation<CMeshO>::MakePureByFlip failed.");
		tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(m.cm);
		m.updateDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case FP_QUAD_DOMINANT:
	{
		m.updateDataMask(MeshModel::MM_FACEQUALITY | MeshModel::MM_FACEFACETOPO );
		int level = par.getEnum("level");
		vcg::tri::BitQuadCreation<CMeshO>::MakeDominant(m.cm,level);
		tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(m.cm);
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
		m.updateDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case FP_MAKE_PURE_TRI:
	{
		vcg::tri::BitQuadCreation<CMeshO>::MakeBitTriOnly(m.cm);
		m.UpdateBoxAndNormals();
		m.clearDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case FP_FAUX_CREASE :
	{
		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		float AngleDegNeg = par.getFloat("AngleDegNeg");
		float AngleDegPos = par.getFloat("AngleDegPos");
//		tri::UpdateFlags<CMeshO>::FaceFauxCrease(m.cm,math::ToRad(AngleDeg));
		tri::UpdateFlags<CMeshO>::FaceEdgeSelSignedCrease(m.cm, math::ToRad(AngleDegNeg), math::ToRad(AngleDegPos));
		m.updateDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case FP_FAUX_EXTRACT :
	{
		//WARNING!!!! the RenderMode(GLW::DMWire) should be useless but...
		MeshModel *em= md.addNewMesh("","EdgeMesh",true/*,RenderMode(GLW::DMWire)*/);
		BuildFromFaceEdgeSel(m.cm,em->cm);
	} break;

	case FP_VATTR_SEAM :
	{
		unsigned int vmask = 0;
		vmask |= vcg::tri::AttributeSeam::POSITION_PER_VERTEX;

		unsigned int nmask = 0;
		switch (par.getEnum("NormalMode"))
		{
			case 0  : break;
			case 1  : if (m.hasDataMask(MeshModel::MM_VERTNORMAL)) nmask |= vcg::tri::AttributeSeam::NORMAL_PER_VERTEX; break;
			case 2  : if (m.hasDataMask(MeshModel::MM_WEDGNORMAL)) nmask |= vcg::tri::AttributeSeam::NORMAL_PER_WEDGE;  break;
			case 3  : if (m.hasDataMask(MeshModel::MM_FACENORMAL)) nmask |= vcg::tri::AttributeSeam::NORMAL_PER_FACE;   break;
			default : break;
		}
		if (nmask != 0) m.updateDataMask(MeshModel::MM_VERTNORMAL);

		unsigned int cmask = 0;
		switch (par.getEnum("ColorMode"))
		{
			case 0  : break;
			case 1  : if (m.hasDataMask(MeshModel::MM_VERTCOLOR)) cmask |= vcg::tri::AttributeSeam::COLOR_PER_VERTEX; break;
			case 2  : if (m.hasDataMask(MeshModel::MM_WEDGCOLOR)) cmask |= vcg::tri::AttributeSeam::COLOR_PER_WEDGE;  break;
			case 3  : if (m.hasDataMask(MeshModel::MM_FACECOLOR)) cmask |= vcg::tri::AttributeSeam::COLOR_PER_FACE;   break;
			default : break;
		}
		if (cmask != 0) m.updateDataMask(MeshModel::MM_VERTCOLOR);

		unsigned int tmask = 0;
		switch (par.getEnum("TexcoordMode"))
		{
			case 0  : break;
			case 1  : if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)) tmask |= vcg::tri::AttributeSeam::TEXCOORD_PER_VERTEX; break;
			case 2  : if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)) tmask |= vcg::tri::AttributeSeam::TEXCOORD_PER_WEDGE;  break;
			default : break;
		}
		if (tmask != 0) m.updateDataMask(MeshModel::MM_VERTTEXCOORD);

		const unsigned int mask = vmask | nmask | cmask | tmask;
		if (mask != 0)
		{
			vcg::tri::AttributeSeam::ASExtract<CMeshO, CMeshO> vExtract(mask);
			vcg::tri::AttributeSeam::ASCompare<CMeshO>         vCompare(mask);
			const bool r = vcg::tri::AttributeSeam::SplitVertex(m.cm, vExtract, vCompare);
			m.clearDataMask(MeshModel::MM_FACEFACETOPO);
			m.clearDataMask(MeshModel::MM_VERTFACETOPO);
			return r;
		}
	} break;

	case FP_PERIMETER_POLYLINE:
	{
		if (m.cm.sfn == 0) // no face selected, fail
		{
			Log("ERROR: There is no face selection!");
			return false;
		}

		Log("Selection is %i triangles", m.cm.sfn);

		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);

		// new layer
		QString newLayerName = QFileInfo(m.shortName()).baseName() + "_perimeter";
		MeshModel* perimeter = md.addNewMesh("", newLayerName, true);
		perimeter->Clear();

		Matrix44m rotM = m.cm.Tr;
		rotM.SetColumn(3, Point3m(0.0, 0.0, 0.0));

		for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
		if (!(*fi).IsD())
		if ((*fi).IsS())
		{
			for (int ei = 0; ei < 3; ei++)
			{
				CMeshO::FacePointer adjf = (*fi).FFp(ei);
				if (adjf == &(*fi) || !(adjf->IsS()))
				{
					CMeshO::VertexIterator nvi;
					vcg::tri::Allocator<CMeshO>::AddEdges(perimeter->cm, 1);
					nvi = vcg::tri::Allocator<CMeshO>::AddVertices(perimeter->cm, 2);
					(*nvi).P() = m.cm.Tr * (*fi).V(ei)->P();
					(*nvi).N() = rotM * (*fi).V(ei)->N();
					perimeter->cm.edge.back().V(0) = &(*nvi);
					nvi++;
					(*nvi).P() = m.cm.Tr * (*fi).V((ei + 1) % 3)->P();
					(*nvi).N() = rotM * (*fi).V((ei + 1) % 3)->N();
					perimeter->cm.edge.back().V(1) = &(*nvi);
				}
			}
		}

		// finishing up the new layer
		tri::Clean<CMeshO>::RemoveDuplicateVertex(perimeter->cm);
		tri::UpdateBounding<CMeshO>::Box(perimeter->cm);
	}break;

	case FP_SLICE_WITH_A_PLANE:
	{
		Point3m planeAxis(0,0,0);
		int ind = par.getEnum("planeAxis");
		if(ind>=0 && ind<3)
			planeAxis[ind] = 1.0f;
		else
			planeAxis=par.getPoint3m("customAxis");

		planeAxis.Normalize();

		float planeOffset = par.getFloat("planeOffset");
		Point3m planeCenter;
		Plane3m slicingPlane;

		Box3m bbox=m.cm.bbox;
		MeshModel* base=&m;
		MeshModel* orig=&m;

		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
		//actual cut of the mesh
		if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(base->cm)>0 || (tri::Clean<CMeshO>::CountNonManifoldVertexFF(base->cm,false) != 0))
		{
			Log("Mesh is not two manifold, cannot apply filter");
			return false;
		}

		// the mesh has to be correctly transformed
		if (m.cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(m.cm, m.cm.Tr, true);

		switch(RefPlane(par.getEnum("relativeTo")))
		{
			case REF_CENTER:  planeCenter = bbox.Center()+ planeAxis*planeOffset*(bbox.Diag()/2.0);      break;
			case REF_MIN:     planeCenter = bbox.min+planeAxis*planeOffset*(bbox.Diag()/2.0);    break;
			case REF_ORIG:    planeCenter = planeAxis*planeOffset;  break;
		}

		//planeCenter+=planeAxis*planeDist ;
		slicingPlane.Init(planeCenter,planeAxis);

		// making up new layer name
		QString sectionName = QFileInfo(base->shortName()).baseName() + "_sect";
		switch(ind)
		{
			case 0:  sectionName.append("_X_");  break;
			case 1:  sectionName.append("_Y_");  break;
			case 2:  sectionName.append("_Z_");  break;
			case 3:  sectionName.append("_custom_");  break;
		}
		sectionName.append(QString::number(planeOffset));

		MeshModel* cap= md.addNewMesh("",sectionName,true);
		vcg::IntersectionPlaneMesh<CMeshO, CMeshO, CMeshO::ScalarType>(orig->cm, slicingPlane, cap->cm );
		tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);
		tri::UpdateBounding<CMeshO>::Box(cap->cm);

		// the mesh has to return to its original position
		if (m.cm.Tr != Matrix44m::Identity())
			tri::UpdatePosition<CMeshO>::Matrix(m.cm, Inverse(m.cm.Tr), true);

		if(par.getBool("createSectionSurface"))
		{
			MeshModel* cap2= md.addNewMesh("",sectionName+"_filled");
			tri::CapEdgeMesh(cap->cm, cap2->cm);
			cap2->UpdateBoxAndNormals();
		}

		if(par.getBool("splitSurfaceWithSection"))
		{
			MeshModel* underM= md.addNewMesh("",sectionName+"_under");
		  underM->updateDataMask(MeshModel::MM_FACEFACETOPO);
		  underM->updateDataMask(MeshModel::MM_VERTQUALITY);

		  tri::Append<CMeshO,CMeshO>::Mesh(underM->cm,orig->cm);
		  tri::UpdateQuality<CMeshO>::VertexFromPlane(underM->cm, slicingPlane);
		  tri::QualityMidPointFunctor<CMeshO> slicingfunc(0.0);
		  tri::QualityEdgePredicate<CMeshO> slicingpred(0.0,0.0);
		  tri::UpdateTopology<CMeshO>::FaceFace(underM->cm);
		  if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(underM->cm) > 0)
		  {
			  Log("Mesh has some not 2 manifoldfaces, splitting surfaces requires manifoldness");
			  md.delMesh(underM);
		  }
		  else
		  {
			tri::RefineE<CMeshO, tri::QualityMidPointFunctor<CMeshO>, tri::QualityEdgePredicate<CMeshO> > (underM->cm, slicingfunc, slicingpred, false);

			tri::UpdateSelection<CMeshO>::VertexFromQualityRange(underM->cm,0,std::numeric_limits<float>::max());
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(underM->cm);
			tri::UpdateSelection<CMeshO>::FaceInvert(underM->cm);

			MeshModel* overM= md.addNewMesh("",sectionName+"_over");
			tri::Append<CMeshO,CMeshO>::Mesh(overM->cm,underM->cm,true);

			tri::UpdateSelection<CMeshO>::VertexClear(underM->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(underM->cm);
			for(auto fi=underM->cm.face.begin();fi!=underM->cm.face.end();++fi)
				if(!(*fi).IsD() && (*fi).IsS())
				tri::Allocator<CMeshO>::DeleteFace(underM->cm,*fi);
			for(auto vi=underM->cm.vert.begin();vi!=underM->cm.vert.end();++vi)
				if(!(*vi).IsD() && (*vi).IsS())
				tri::Allocator<CMeshO>::DeleteVertex(underM->cm,*vi);

			underM->UpdateBoxAndNormals();
			overM->UpdateBoxAndNormals();
		  }
		}

	} break;
}
return true;
}

int ExtraMeshFilterPlugin::postCondition(QAction * filter) const
{
	switch (ID(filter))
	{
		case FP_ROTATE_FIT           :
		case FP_PRINCIPAL_AXIS       :
		case FP_FLIP_AND_SWAP        :
		case FP_SCALE                :
		case FP_CENTER               :
		case FP_ROTATE               :
		case FP_SET_TRANSFORM_PARAMS :
		case FP_SET_TRANSFORM_MATRIX :
		case FP_FREEZE_TRANSFORM     : return MeshModel::MM_TRANSFMATRIX + MeshModel::MM_VERTCOORD + MeshModel::MM_VERTNORMAL + MeshModel::MM_FACENORMAL;
		case FP_RESET_TRANSFORM      :
		case FP_INVERT_TRANSFORM     : return MeshModel::MM_TRANSFMATRIX;
		case FP_NORMAL_EXTRAPOLATION :
		case FP_NORMAL_SMOOTH_POINTCLOUD : return MeshModel::MM_VERTNORMAL;

		case FP_LOOP_SS :
		case FP_BUTTERFLY_SS :
		case FP_CLUSTERING :
		case FP_QUADRIC_SIMPLIFICATION :
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION :
		case FP_EXPLICIT_ISOTROPIC_REMESHING :
		case FP_MIDPOINT :
		case FP_REORIENT :
		case FP_INVERT_FACES :
		case FP_CLOSE_HOLES :
		case FP_REFINE_CATMULL :
		case FP_REFINE_HALF_CATMULL :
		case FP_QUAD_DOMINANT :
		case FP_MAKE_PURE_TRI :
		case FP_QUAD_PAIRING :
		case FP_FAUX_CREASE :
		case FP_FAUX_EXTRACT :
		case FP_VATTR_SEAM :
		case FP_REFINE_LS3_LOOP : return MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE;

		case FP_COMPUTE_PRINC_CURV_DIR : return MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCURV | MeshModel::MM_VERTCURVDIR | MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY;

		case FP_SLICE_WITH_A_PLANE :
		case FP_PERIMETER_POLYLINE :
		case FP_CYLINDER_UNWRAP : return MeshModel::MM_NONE; // they create a new layer

		default                  : return MeshModel::MM_ALL;
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(ExtraMeshFilterPlugin)
