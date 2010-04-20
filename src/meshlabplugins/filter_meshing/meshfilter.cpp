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
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/hole.h>
#include <vcg/complex/trimesh/refine_loop.h>
#include <vcg/complex/trimesh/bitquad_support.h>
#include <vcg/complex/trimesh/bitquad_creation.h>
#include <vcg/complex/trimesh/clustering.h>
#include <vcg/complex/trimesh/attribute_seam.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/update/curvature_fitting.h>
#include <vcg/space/normal_extrapolation.h>
#include <vcg/space/fitting3.h>
#include "quadric_tex_simp.h"
#include "quadric_simp.h"

using namespace std;
using namespace vcg;

void QuadricTexSimplification(CMeshO &m,int  TargetFaceNum, bool Selected, CallBackPos *cb);

ExtraMeshFilterPlugin::ExtraMeshFilterPlugin(void)
{
	typeList
		<< FP_LOOP_SS
		<< FP_BUTTERFLY_SS
		<< FP_REMOVE_UNREFERENCED_VERTEX
		<< FP_REMOVE_DUPLICATED_VERTEX
		<< FP_REMOVE_FACES_BY_AREA
		<< FP_REMOVE_FACES_BY_EDGE
		<< FP_CLUSTERING
		<< FP_QUADRIC_SIMPLIFICATION
		<< FP_QUADRIC_TEXCOORD_SIMPLIFICATION
		<< FP_MIDPOINT
		<< FP_REORIENT
		<< FP_FLIP_AND_SWAP
		<< FP_ROTATE
    << FP_ROTATE_FIT
    << FP_PRINCIPAL_AXIS
		<< FP_SCALE
		<< FP_CENTER
		<< FP_INVERT_FACES
		<< FP_REMOVE_NON_MANIFOLD_FACE
		<< FP_REMOVE_NON_MANIFOLD_VERTEX
		<< FP_NORMAL_EXTRAPOLATION
		<< FP_COMPUTE_PRINC_CURV_DIR
		<< FP_CLOSE_HOLES
		<< FP_FREEZE_TRANSFORM
		<< FP_RESET_TRANSFORM
		<< FP_CYLINDER_UNWRAP
		<< FP_REFINE_CATMULL
		<< FP_REFINE_HALF_CATMULL
		<< FP_QUAD_PAIRING
		<< FP_FAUX_CREASE
		<< FP_VATTR_SEAM
	;

	FilterIDType tt;

	foreach(tt, types())
		actionList << new QAction(filterName(tt), this);

	lastq_QualityThr       = 0.3f;
	lastq_PreserveBoundary = false;
	lastq_PreserveNormal   = false;
	lastq_OptimalPlacement = true;
	lastq_Selected         = false;
	lastq_PlanarQuadric    = false;
	lastq_QualityWeight    = false;

	lastqtex_QualityThr    = 0.3f;
	lastqtex_extratw       = 0.0;
}

ExtraMeshFilterPlugin::FilterClass ExtraMeshFilterPlugin::getClass(QAction * a)
{
	switch (ID(a))
	{
		case FP_REMOVE_UNREFERENCED_VERTEX       :
		case FP_REMOVE_DUPLICATED_VERTEX         :
		case FP_REMOVE_FACES_BY_AREA             :
		case FP_REMOVE_FACES_BY_EDGE             :
		case FP_REMOVE_NON_MANIFOLD_FACE         :
		case FP_REMOVE_NON_MANIFOLD_VERTEX       : return MeshFilterInterface::Cleaning;

		case FP_BUTTERFLY_SS                     :
		case FP_LOOP_SS                          :
		case FP_MIDPOINT                         :
		case FP_QUADRIC_SIMPLIFICATION           :
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  :
		case FP_CLUSTERING                       :
		case FP_CLOSE_HOLES                      :
		case FP_REFINE_CATMULL                   :
		case FP_REFINE_HALF_CATMULL              :
		case FP_QUAD_PAIRING                     :
		case FP_FAUX_CREASE                      :
		case FP_VATTR_SEAM                       : return MeshFilterInterface::Remeshing;

    case FP_NORMAL_EXTRAPOLATION             : return MeshFilterInterface::FilterClass( MeshFilterInterface::Normal + MeshFilterInterface::PointSet );

		case FP_INVERT_FACES                     :
		case FP_REORIENT                         :
		case FP_COMPUTE_PRINC_CURV_DIR           :
		case FP_ROTATE                           :
    case FP_ROTATE_FIT                       :
		case FP_CENTER                           :
		case FP_SCALE                            :
    case FP_PRINCIPAL_AXIS                   :
		case FP_FLIP_AND_SWAP                    : return MeshFilterInterface::Normal;

		case FP_FREEZE_TRANSFORM                 :
		case FP_RESET_TRANSFORM                  : return FilterClass(MeshFilterInterface::Normal + MeshFilterInterface::Layer);

		case FP_CYLINDER_UNWRAP                  : return MeshFilterInterface::Smoothing;

		default                                  : assert(0); return MeshFilterInterface::Generic;
	}

	return MeshFilterInterface::Generic;
}

QString ExtraMeshFilterPlugin::filterName(FilterIDType filter) const
{
	switch (filter)
	{
    case FP_LOOP_SS                          : return tr("Subdivision Surfaces: Loop");
    case FP_BUTTERFLY_SS                     : return tr("Subdivision Surfaces: Butterfly Subdivision");
    case FP_MIDPOINT                         : return tr("Subdivision Surfaces: Midpoint");
    case FP_REFINE_CATMULL                   : return tr("Subdivision Surfaces: Catmull-Clark");
    case FP_REMOVE_UNREFERENCED_VERTEX       : return tr("Remove Unreferenced Vertex");
		case FP_REMOVE_DUPLICATED_VERTEX         : return tr("Remove Duplicated Vertex");
		case FP_REMOVE_FACES_BY_AREA             : return tr("Remove Zero Area Faces");
		case FP_REMOVE_FACES_BY_EDGE             : return tr("Remove Faces with edges longer than...");
		case FP_QUADRIC_SIMPLIFICATION           : return tr("Quadric Edge Collapse Decimation");
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  : return tr("Quadric Edge Collapse Decimation (with texture)");
		case FP_CLUSTERING                       : return tr("Clustering decimation");
    case FP_REORIENT                         : return tr("Re-Orient all faces coherentely");
		case FP_INVERT_FACES                     : return tr("Invert Faces Orientation");
    case FP_SCALE                            : return tr("Transform: Scale");
    case FP_CENTER                           : return tr("Transform: Move, Translate, Center");
    case FP_ROTATE                           : return tr("Transform: Rotate");
    case FP_ROTATE_FIT                       : return tr("Transform: Rotate to Fit to a plane");
    case FP_PRINCIPAL_AXIS                   : return tr("Transform: Align to Principal Axis");
    case FP_FLIP_AND_SWAP                    : return tr("Transform: Flip and/or swap axis");
		case FP_FREEZE_TRANSFORM                 : return tr("Freeze Current Matrix");
		case FP_RESET_TRANSFORM                  : return tr("Reset Current Matrix");
		case FP_REMOVE_NON_MANIFOLD_FACE         : return tr("Remove Non Manifold Faces");
		case FP_REMOVE_NON_MANIFOLD_VERTEX       : return tr("Remove Non Manifold Vertices");
		case FP_NORMAL_EXTRAPOLATION             : return tr("Compute normals for point sets");
		case FP_COMPUTE_PRINC_CURV_DIR           : return tr("Compute curvature principal directions");
		case FP_CLOSE_HOLES                      : return tr("Close Holes");
		case FP_CYLINDER_UNWRAP                  : return tr("Geometric Cylindrical Unwrapping");
    case FP_REFINE_HALF_CATMULL              : return tr("Tri to Quad by 4-8 Subdivision");
		case FP_QUAD_PAIRING                     : return tr("Tri to Quad by smart triangle pairing");
		case FP_FAUX_CREASE                      : return tr("Crease Marking with NonFaux Edges");
		case FP_VATTR_SEAM                       : return tr("Vertex Attribute Seam");

		default                                  : assert(0);
	}

	return tr("error!");
}

QString ExtraMeshFilterPlugin::filterInfo(FilterIDType filterID) const
{
	switch (filterID)
	{
		case FP_LOOP_SS                          : return tr("Apply Loop's Subdivision Surface algorithm. It is an approximate method which subdivide each triangle in four faces. It works for every triangle and has rules for extraordinary vertices");
		case FP_BUTTERFLY_SS                     : return tr("Apply Butterfly Subdivision Surface algorithm. It is an interpolated method, defined on arbitrary triangular meshes. The scheme is known to be C1 but not C2 on regular meshes");
		case FP_MIDPOINT                         : return tr("Apply a plain subdivision scheme where every edge is splitted on its midpoint");
		case FP_REMOVE_UNREFERENCED_VERTEX       : return tr("Check for every vertex on the mesh if it is referenced by a face and removes it");
		case FP_REMOVE_DUPLICATED_VERTEX         : return tr("Check for every vertex on the mesh if there are two vertices with same coordinates and removes it");
		case FP_REMOVE_FACES_BY_AREA             : return tr("Removes null faces (the one with area equal to zero)");
		case FP_REMOVE_FACES_BY_EDGE             : return tr("Remove from the mesh all triangles whose have an edge with lenght greater or equal than a threshold");
		case FP_REMOVE_NON_MANIFOLD_FACE         : return tr("Remove non 2-manifold edges by removing some of the faces incident on non manifold edges");
		case FP_REMOVE_NON_MANIFOLD_VERTEX       : return tr("Remove non 2-manifold vertices, that vertices where the number of faces that can be reached using only face-face connectivity is different from the number of faces actually incident on that vertex.<br>"
		                                                     "Typical example think to two isolated triangles connected by a single vertex building a <i>hourglass</i> shape.");
		case FP_CLUSTERING                       : return tr("Collapse vertices by creating a three dimensional grid enveloping the mesh and discretizes them based on the cells of this grid");
		case FP_QUADRIC_SIMPLIFICATION           : return tr("Simplify a mesh using a Quadric based Edge Collapse Strategy, better than clustering but slower");
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  : return tr("Simplify a textured mesh using a Quadric based Edge Collapse Strategy, better than clustering but slower");
		case FP_REORIENT                         : return tr("Re-orient in a consistent way all the faces of the mesh");
		case FP_INVERT_FACES                     : return tr("Invert faces orientation, flip the normal of the mesh");
		case FP_SCALE                            : return tr("Generate a matrix transformation that scale the mesh. The mesh can be also automatically scaled to a unit side box. ");
		case FP_CENTER                           : return tr("Generate a matrix transformation that translate the mesh. The mesh can be translated around one of the axis or a given axis and w.r.t. to the origin or the baricenter, or a given point.");
		case FP_ROTATE                           : return tr("Generate a matrix transformation that rotates the mesh. The mesh can be rotated around one of the axis or a given axis and w.r.t. to the origin or the baricenter, or a given point.");
    case FP_ROTATE_FIT                       : return tr("Generate a matrix transformation that rotates the mesh so that the selected set of points fit well the XY plane.");
    case FP_PRINCIPAL_AXIS                   : return tr("Generate a matrix transformation that rotates the mesh aligning it to its principal axis of inertia. if the mesh is watertight the Intertia tensor is computed assuming the interior of the mesh has a uniform density. In case of an open mesh or a point clouds the inerta tensor is computed assuming each vertex is a constant puntual mass.");
		case FP_FLIP_AND_SWAP                    : return tr("Generate a matrix transformation that flips each one of the axis or swaps a couple of axis. The listed transformations are applied in that order.");
		case FP_RESET_TRANSFORM                  : return tr("Set the current transformation matrix to the Identity. ");
		case FP_FREEZE_TRANSFORM                 : return tr("Freeze the current transformation matrix into the coords of the vertices of the mesh (and set this matrix to the identity). In other words it applies in a definetive way the current matrix to the vertex coords.");
		case FP_NORMAL_EXTRAPOLATION             : return tr("Compute the normals of the vertices of a  mesh without exploiting the triangle connectivity, useful for dataset with no faces");
		case FP_COMPUTE_PRINC_CURV_DIR           : return tr("Compute the principal directions of curvature with several algorithms");
		case FP_CLOSE_HOLES                      : return tr("Close holes smaller than a given threshold");
		case FP_CYLINDER_UNWRAP                  : return tr("Unwrap the geometry of current mesh along a clylindrical equatorial projection. The cylindrical projection axis is centered on the origin and directed along the vertical <b>Y</b> axis.");
		case FP_REFINE_CATMULL                   : return tr("Apply the Catmull-Clark Subdivision Surfaces. Note Position of the new vertces is simply linearly interpolated. If the mesh is triangle based (no faux edges) it generate a quad mesh otherwise it honores it the faux-edge bits");
		case FP_REFINE_HALF_CATMULL              : return tr("Convert a tri mesh into a quad mesh by applying a 4-8 subdivision scheme. It introduces less overhead than the plain Catmull-Clark Subdivision Surfaces (it adds only a single vertex for each triangle instead of four)."
		                                                     "<br> See <b>4–8 Subdivision</b>"
		                                                     "<br> <i>Luiz Velho, Denis Zorin </i>"
                                                             "<br>CAGD, volume 18, Issue 5, Pages 397-427. ");
		case FP_QUAD_PAIRING                     : return tr("Convert a tri mesh into a quad mesh by pairing triangles.");
		case FP_FAUX_CREASE                      : return tr("Mark the crease edges of a mesh as Non-Faux according to edge dihedral angle.");
		case FP_VATTR_SEAM                       : return tr("Make all selected vertex attributes connectivity-independent:<br/>"
		                                                     "vertices are duplicated whenever two or more selected wedge or face attributes do not match.<br/>"
		                                                     "This is particularly useful for GPU-friendly mesh layout, where a single index must be used to access all required vertex attributes.");

		default                                  : assert(0);
	}

	return QString();
}

int ExtraMeshFilterPlugin::getRequirements(QAction * action)
{
	switch(ID(action))
	{
		case FP_FAUX_CREASE                      : return MeshModel::MM_FACEFACETOPO;
		case FP_QUAD_PAIRING                     : return MeshModel::MM_FACEFACETOPO;
		case FP_REFINE_CATMULL                   : return MeshModel::MM_FACEFACETOPO;
		case FP_REFINE_HALF_CATMULL              : return MeshModel::MM_FACEFACETOPO;

		case FP_REMOVE_NON_MANIFOLD_FACE         :
		case FP_LOOP_SS                          :
		case FP_BUTTERFLY_SS                     :
		case FP_MIDPOINT                         :
		case FP_CLOSE_HOLES                      : return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;

		case FP_REMOVE_NON_MANIFOLD_VERTEX       :
		case FP_REORIENT                         : return MeshModel::MM_FACEFACETOPO;

		case FP_REMOVE_UNREFERENCED_VERTEX       :
		case FP_REMOVE_DUPLICATED_VERTEX         :
		case FP_REMOVE_FACES_BY_AREA             :
		case FP_REMOVE_FACES_BY_EDGE             :
		case FP_CLUSTERING                       :
		case FP_ROTATE                           :
    case FP_ROTATE_FIT                       :
    case FP_PRINCIPAL_AXIS                   :
		case FP_CENTER                           :
		case FP_SCALE                            :
		case FP_RESET_TRANSFORM                  :
		case FP_FLIP_AND_SWAP                    :
		case FP_FREEZE_TRANSFORM                 :
		case FP_NORMAL_EXTRAPOLATION             :
		case FP_INVERT_FACES                     :
		case FP_CYLINDER_UNWRAP                  :
		case FP_VATTR_SEAM                       : return 0;

		case FP_COMPUTE_PRINC_CURV_DIR           : return MeshModel::MM_VERTCURVDIR | MeshModel::MM_FACEMARK | MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEFACETOPO;

		case FP_QUADRIC_SIMPLIFICATION           :
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION  : return MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_VERTMARK;

		default                                  : assert(0);
	}

	return 0;
}

// this function builds and intializes with the default values (that can depend on the current mesh or selection)
// the list of parameters that a filter requires.
// return
//		true if has some parameters
//		false is has no params
void ExtraMeshFilterPlugin::initParameterSet(QAction * action, MeshModel & m, RichParameterSet & parlst)
{
	float maxVal;
	QStringList methods;

	switch(ID(action))
	{
		case FP_COMPUTE_PRINC_CURV_DIR:
			methods.push_back("Taubin approximation");
			methods.push_back("Principal Component Analysis");
			methods.push_back("Normal Cycles");
			methods.push_back("Pseudoinverse Quadric Fitting");
			parlst.addParam(new RichEnum("Method", 3, methods, tr("Method:"), tr("Choose a method")));
			parlst.addParam(new RichBool("Autoclean",true,"Remove Unreferenced Vertices","If selected, before starting the filter will remove anyy unreference vertex (for which curvature values are not defined)"));
			break;

		case FP_QUADRIC_SIMPLIFICATION:
			parlst.addParam(new RichInt  ("TargetFaceNum", (m.cm.sfn>0) ? m.cm.sfn/2 : m.cm.fn/2,"Target number of faces", "The desired final number of faces."));
			parlst.addParam(new RichFloat("TargetPerc", 0,"Percentage reduction (0..1)", "If non zero, this parameter specifies the desired final size of the mesh as a percentage of the initial size."));
			parlst.addParam(new RichFloat("QualityThr",lastq_QualityThr,"Quality threshold","Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n"));
			parlst.addParam(new RichBool ("PreserveBoundary",lastq_PreserveBoundary,"Preserve Boundary of the mesh","The simplification process tries not to destroy mesh boundaries"));
			parlst.addParam(new RichBool ("PreserveNormal",lastq_PreserveNormal,"Preserve Normal","Try to avoid face flipping effects and try to preserve the original orientation of the surface"));
			parlst.addParam(new RichBool ("OptimalPlacement",lastq_OptimalPlacement,"Optimal position of simplified vertices","Each collapsed vertex is placed in the position minimizing the quadric error.\n It can fail (creating bad spikes) in case of very flat areas. \nIf disabled edges are collapsed onto one of the two original vertices and the final mesh is composed by a subset of the original vertices. "));
			parlst.addParam(new RichBool ("PlanarQuadric",lastq_PlanarQuadric,"Planar Simplification","Add additional simplification constraints that improves the quality of the simplification of the planar portion of the mesh."));
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
			parlst.addParam(new RichBool ("OptimalPlacement",lastq_OptimalPlacement,"Optimal position of simplified vertices","Each collapsed vertex is placed in the position minimizing the quadric error.\n It can fail (creating bad spikes) in case of very flat areas. \nIf disabled edges are collapsed onto one of the two original vertices and the final mesh is composed by a subset of the original vertices. "));
			parlst.addParam(new RichBool ("PreserveNormal",lastq_PreserveNormal,"Preserve Normal","Try to avoid face flipping effects and try to preserve the original orientation of the surface"));
			parlst.addParam(new RichBool ("PlanarQuadric",lastq_PlanarQuadric,"Planar Simplification","Add additional simplification constraints that improves the quality of the simplification of the planar portion of the mesh."));
			parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Simplify only selected faces","The simplification is applied only to the selected set of faces.\n Take care of the target number of faces!"));
			break;

		case FP_CLOSE_HOLES:
			parlst.addParam(new RichInt ("MaxHoleSize",(int)30,"Max size to be closed ","The size is expressed as number of edges composing the hole boundary"));
			parlst.addParam(new RichBool("Selected",m.cm.sfn>0,"Close holes with selected faces","Only the holes with at least one of the boundary faces selected are closed"));
			parlst.addParam(new RichBool("NewFaceSelected",true,"Select the newly created faces","After closing a hole the faces that have been created are left selected. Any previous selection is lost. Useful for example for smoothing the newly created holes."));
			parlst.addParam(new RichBool("SelfIntersection",true,"Prevent creation of selfIntersecting faces","When closing an holes it tries to prevent the creation of faces that intersect faces adjacent to the boundary of the hole. It is an heuristic, non intersetcting hole filling can be NP-complete."));
			break;

		case FP_LOOP_SS:
		case FP_BUTTERFLY_SS:
		case FP_MIDPOINT:
			maxVal = m.cm.bbox.Diag();
			parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the edges <b>longer</b> than this threshold will be refined.<br>Setting this value to zero will force an uniform refinement."));
			parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Affect only selected faces","If selected the filter affect only the selected faces"));
			break;

		case FP_REMOVE_FACES_BY_EDGE:
			maxVal = m.cm.bbox.Diag();
			parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the faces with an edge <b>longer</b> than this threshold will be deleted. Useful for removing long skinny faces obtained by bad triangulation of range maps."));
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
      parlst.addParam(new RichDynamicFloat("angle",0,-360,360,"Rotation Angle","Angle of rotation (in <b>degree</b>). If snapping is enable this vaule is rounded according to the snap value"));
      parlst.addParam(new RichBool("snapFlag",false,"Snap angle","If selected, before starting the filter will remove anyy unreference vertex (for which curvature values are not defined)"));
      parlst.addParam(new RichPoint3f("customAxis",Point3f(0,0,0),"Custom axis","This rotation axis is used only if the 'custom axis' option is chosen."));
      parlst.addParam(new RichPoint3f("customCenter",Point3f(0,0,0),"Custom center","This rotation center is used only if the 'custom point' option is chosen."));
      parlst.addParam(new RichFloat("snapAngle",30,"Snapping Value","This value is used to snap the rotation angle."));
    }
    break;
    case FP_PRINCIPAL_AXIS:
      {
        parlst.addParam(new RichBool("pointsFlag",m.cm.fn==0,"Use vertex","If selected, only the vertices of the mesh are used to compute the Principal Axis. Mandatory for point clouds or for non water tight meshes"));
      }
     break;
    case FP_CENTER:
			{
				Box3f &bb=m.cm.bbox;
				parlst.addParam(new RichDynamicFloat("axisX",0,-5.0*bb.DimX(),5.0*bb.DimX(),"X Axis","Scaling"));
				parlst.addParam(new RichDynamicFloat("axisY",0,-5.0*bb.DimY(),5.0*bb.DimY(),"Y Axis","Scaling"));
				parlst.addParam(new RichDynamicFloat("axisZ",0,-5.0*bb.DimZ(),5.0*bb.DimZ(),"Z Axis","Scaling"));
				parlst.addParam(new RichBool("centerFlag",false,"translate center of bbox to the origin","If selected, the object is scaled to a box whose sides are at most 1 unit lenght"));
			}
			break;

		case FP_SCALE:
			{
				parlst.addParam(new RichDynamicFloat("axisX",1,0.1,10,"X Axis","Scaling"));
				parlst.addParam(new RichDynamicFloat("axisY",1,0.1,10,"Y Axis","Scaling"));
				parlst.addParam(new RichDynamicFloat("axisZ",1,0.1,10,"Z Axis","Scaling"));
				parlst.addParam(new RichBool("uniformFlag",true,"Uniform Scaling","If selected an uniform scaling (the same for all the three axis) is applied (the X axis value is used)"));
				QStringList scaleCenter;
				scaleCenter.push_back("origin");
				scaleCenter.push_back("barycenter");
				scaleCenter.push_back("custom point");
				parlst.addParam(new RichEnum("scaleCenter", 0, scaleCenter, tr("Center of rotation:"), tr("Choose a method")));
				parlst.addParam(new RichPoint3f("customCenter",Point3f(0,0,0),"Custom center","This rotation center is used only if the 'custom point' option is chosen."));
				parlst.addParam(new RichBool("unitFlag",false,"Scale to Unit bbox","If selected, the object is scaled to a box whose sides are at most 1 unit lenght"));
			}
			break;

		case FP_FAUX_CREASE:
			parlst.addParam(new RichFloat ("AngleDeg",(int)45,"Angle Threshold (deg)","The angle threshold for considering an edge a crease. If the normals between two faces forms an angle larger than the threshold the edge is considered a crease."));
			break;

		case FP_NORMAL_EXTRAPOLATION:
			parlst.addParam(new RichInt ("K",(int)10,"Number of neigbors","The number of neighbors used to estimate and propagate normals."));
			break;

		case FP_VATTR_SEAM:
			{
#if 0
				{
					QStringList positionMethod;
					/* if (true) */ positionMethod.push_back(tr("None"));
					/* if (true) */ positionMethod.push_back(tr("Vertex"));
					/* if (!positionMethod.empty()) */ parlst.addParam(new RichEnum("PositionMode", 0, positionMethod, tr("Position Source:"), tr("Choose a method")));
				}
#endif

				{
					QStringList normalMethod;
					/* if (true)                                    */ normalMethod.push_back(tr("None"));
					/* if (m.hasDataMask(MeshModel::MM_VERTNORMAL)) */ normalMethod.push_back(tr("Vertex"));
					/* if (m.hasDataMask(MeshModel::MM_WEDGNORMAL)) */ normalMethod.push_back(tr("Wedge"));
					/* if (m.hasDataMask(MeshModel::MM_FACENORMAL)) */ normalMethod.push_back(tr("Face"));
					/* if (!normalMethod.empty()) */ parlst.addParam(new RichEnum("NormalMode", 0, normalMethod, tr("Normal Source:"), tr("Choose a method")));
				}

				{
					QStringList colorMethod;
					/* if (true)                                   */ colorMethod.push_back(tr("None"));
					/* if (m.hasDataMask(MeshModel::MM_VERTCOLOR)) */ colorMethod.push_back(tr("Vertex"));
					/* if (m.hasDataMask(MeshModel::MM_WEDGCOLOR)) */ colorMethod.push_back(tr("Wedge"));
					/* if (m.hasDataMask(MeshModel::MM_FACECOLOR)) */ colorMethod.push_back(tr("Face"));
					/* if (!colorMethod.empty()) */ parlst.addParam(new RichEnum("ColorMode", 0, colorMethod, tr("Color Source:"), tr("Choose a method")));
				}

				{
					QStringList texcoordMethod;
					/* if (true)                                      */ texcoordMethod.push_back(tr("None"));
					/* if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)) */ texcoordMethod.push_back(tr("Vertex"));
					/* if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)) */ texcoordMethod.push_back(tr("Wedge"));
					/* if (!texcoordMethod.empty()) */ parlst.addParam(new RichEnum("TexcoordMode", 0, texcoordMethod, tr("Texcoord Source:"), tr("Choose a method")));
				}
			}
			break;

		default:
			break;
	}
}

bool ExtraMeshFilterPlugin::applyFilter(QAction * filter, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb)
{
	MeshModel & m = *md.mm();

	if( ID(filter) == FP_LOOP_SS ||
			ID(filter) == FP_BUTTERFLY_SS ||
			ID(filter) == FP_MIDPOINT )
		{
      if (  tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) > 0)
			{
				errorMessage = "Mesh has some not 2 manifoldfaces, subdivision surfaces require manifoldness"; // text
				return false; // can't continue, mesh can't be processed
			}

			bool  selected  = par.getBool("Selected");
			float threshold = par.getAbsPerc("Threshold");

			switch(ID(filter)) {
				case FP_LOOP_SS :
		tri::RefineOddEven<CMeshO, tri::OddPointLoop<CMeshO>, tri::EvenPointLoop<CMeshO> >
		  (m.cm, tri::OddPointLoop<CMeshO>(), tri::EvenPointLoop<CMeshO>(), threshold, selected, cb);
		break;
				case FP_BUTTERFLY_SS :
					Refine<CMeshO,MidPointButterfly<CMeshO> >
						(m.cm, MidPointButterfly<CMeshO>(), threshold, selected, cb);
					break;
				case FP_MIDPOINT :
					Refine<CMeshO,MidPoint<CMeshO> >
						(m.cm, MidPoint<CMeshO>(&m.cm), threshold, selected, cb);
			}

		 m.clearDataMask(MeshModel::MM_VERTFACETOPO);
		 vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		}
	if (ID(filter) == FP_REMOVE_FACES_BY_EDGE ) {
	bool selected  = par.getBool("Selected");
	float threshold = par.getAbsPerc("Threshold");
		int delFaceNum;
	  if(selected) delFaceNum=tri::Clean<CMeshO>::RemoveFaceOutOfRangeEdgeSel<true>(m.cm,0,threshold );
		 else    delFaceNum=tri::Clean<CMeshO>::RemoveFaceOutOfRangeEdgeSel<false>(m.cm,0,threshold );
	m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
		Log(GLLogStream::FILTER, "Removed %d faces with and edge longer than %f",delFaceNum,threshold);
	}

  if(ID(filter) == (FP_REMOVE_FACES_BY_AREA) )
	  {
		int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
		Log(GLLogStream::FILTER, "Removed %d null faces", nullFaces);
	  m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
	  }

  if(ID(filter) == (FP_REMOVE_UNREFERENCED_VERTEX) )
	  {
		int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
		Log(GLLogStream::FILTER, "Removed %d unreferenced vertices",delvert);
	  }

	if(ID(filter) == (FP_REMOVE_DUPLICATED_VERTEX) )
	  {
		int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
		Log(GLLogStream::FILTER, "Removed %d duplicated vertices", delvert);
		if (delvert != 0)
		  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if(ID(filter) == (FP_REMOVE_NON_MANIFOLD_FACE) )
	  {
		int nonManif=tri::Clean<CMeshO>::RemoveNonManifoldFace(m.cm);

			if(nonManif) Log(GLLogStream::FILTER, "Removed %d Non Manifold Faces", nonManif);
							else Log(GLLogStream::FILTER, "Mesh is two-manifold. Nothing done.", nonManif);

			 m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
	  }

		if(ID(filter) == (FP_REMOVE_NON_MANIFOLD_VERTEX) )
		{
			int nonManif=tri::Clean<CMeshO>::RemoveNonManifoldVertex(m.cm);

			if(nonManif) Log(GLLogStream::FILTER, "Removed %d Non Manifold Vertex", nonManif);
			else Log(GLLogStream::FILTER, "Mesh is two-manifold. Nothing done.", nonManif);

			m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
		}

	if(ID(filter) == (FP_REORIENT) )
	  {
		bool oriented;
		bool orientable;
      if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm)>0 ) {
					errorMessage = "Mesh has some not 2-manifold faces, Orientability requires manifoldness"; // text
					return false; // can't continue, mesh can't be processed
			}

		tri::Clean<CMeshO>::IsOrientedMesh(m.cm, oriented,orientable);
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
			vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(m.cm);

//			m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_BORDERFLAG);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if(ID(filter) == (FP_CLUSTERING))
	  {
	  float threshold = par.getAbsPerc("Threshold");
				vcg::tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > ClusteringGrid;
				ClusteringGrid.Init(m.cm.bbox,100000,threshold);
				ClusteringGrid.AddMesh(m.cm);
				ClusteringGrid.ExtractMesh(m.cm);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
	  }

	if (ID(filter) == (FP_INVERT_FACES) )
	{
	  tri::Clean<CMeshO>::FlipMesh(m.cm);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
	}

	if (ID(filter) == (FP_FREEZE_TRANSFORM) ) {
		tri::UpdatePosition<CMeshO>::Matrix(m.cm, m.cm.Tr);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		tri::UpdateBounding<CMeshO>::Box(m.cm);
		m.cm.Tr.SetIdentity();
	}


	if (ID(filter) == (FP_QUADRIC_SIMPLIFICATION) ) {

		int TargetFaceNum = par.getInt("TargetFaceNum");
		if(par.getFloat("TargetPerc")!=0) TargetFaceNum = m.cm.fn*par.getFloat("TargetPerc");

		tri::MyTriEdgeCollapse::SetDefaultParams();
		tri::TriEdgeCollapseQuadricParameter &pp = tri::MyTriEdgeCollapse::Params();
		pp.QualityThr=lastq_QualityThr =par.getFloat("QualityThr");
		pp.PreserveBoundary=lastq_PreserveBoundary = par.getBool("PreserveBoundary");
		pp.QualityWeight=lastq_QualityWeight = par.getBool("QualityWeight");
		pp.NormalCheck=lastq_PreserveNormal = par.getBool("PreserveNormal");
		pp.OptimalPlacement=lastq_OptimalPlacement = par.getBool("OptimalPlacement");
		pp.QualityQuadric=lastq_PlanarQuadric = par.getBool("PlanarQuadric");
		lastq_Selected = par.getBool("Selected");

		QuadricSimplification(m.cm,TargetFaceNum,lastq_Selected,  cb);

		if(par.getBool("AutoClean"))
		{
			int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
			if(nullFaces) Log(GLLogStream::FILTER, "PostSimplification Cleaning: Removed %d null faces", nullFaces);
			int deldupvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
			if(deldupvert) Log(GLLogStream::FILTER, "PostSimplification Cleaning: Removed %d duplicated vertices", deldupvert);
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			if(delvert) Log(GLLogStream::FILTER, "PostSimplification Cleaning: Removed %d unreferenced vertices",delvert);
			m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
			tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		}

		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		tri::UpdateBounding<CMeshO>::Box(m.cm);
	}


	if (ID(filter) == (FP_QUADRIC_TEXCOORD_SIMPLIFICATION) ) {
		if(!tri::HasPerWedgeTexCoord(m.cm))
		{
			errorMessage="Warning: nothing have been done. Mesh has no Texture.";
			return false;
		}
		if ( ! tri::Clean<CMeshO>::HasConsistentPerWedgeTexCoord(m.cm) ) {
	  errorMessage = "Mesh has some inconsistent tex coords (some faces without texture)"; // text
	  return false; // can't continue, mesh can't be processed
	}

		int TargetFaceNum = par.getInt("TargetFaceNum");
		if(par.getFloat("TargetPerc")!=0) TargetFaceNum = m.cm.fn*par.getFloat("TargetPerc");


		tri::MyTriEdgeCollapseQTex::SetDefaultParams();
		tri::TriEdgeCollapseQuadricTexParameter & pp=tri::MyTriEdgeCollapseQTex::Params();


		lastqtex_QualityThr = pp.QualityThr = par.getFloat("QualityThr");
		lastqtex_extratw = pp.ExtraTCoordWeight = par.getFloat("Extratcoordw");
		lastq_OptimalPlacement = pp.OptimalPlacement = par.getBool("OptimalPlacement");
		lastq_PreserveBoundary = pp.PreserveBoundary = par.getBool("PreserveBoundary");
		lastq_PlanarQuadric  = pp.QualityQuadric = par.getBool("PlanarQuadric");
		lastq_PreserveNormal = pp.NormalCheck = par.getBool("PreserveNormal");

		lastq_Selected = par.getBool("Selected");

		QuadricTexSimplification(m.cm,TargetFaceNum,lastq_Selected, cb);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		tri::UpdateBounding<CMeshO>::Box(m.cm);
	}
	if (ID(filter) == FP_RESET_TRANSFORM ) {
		m.cm.Tr.SetIdentity();
	}
  if (ID(filter) ==  FP_ROTATE_FIT)
    {
      Box3f bbox; //il bbox delle facce selezionate
      std::vector< Point3f > selected_pts; //devo copiare i punti per il piano di fitting

      if(m.cm.svn==0)
      {tri::UpdateSelection<CMeshO>::ClearVertex(m.cm);
        tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
      }
      for(CMeshO::VertexIterator vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
        if(!(*vi).IsD() && (*vi).IsS() ){
        Point3f p = (*vi).P();
        bbox.Add(p);
        selected_pts.push_back(p);
      }
      Log("Using %i vertexes to fit a build plane",int(selected_pts.size()));
      Plane3f plane;

      PlaneFittingPoints(selected_pts,plane); //calcolo il piano di fitting

      Log("New Z axis is %f %f %f",plane.Direction()[0],plane.Direction()[1],plane.Direction()[2]);
      Matrix44f tr1; tr1.SetTranslate(-bbox.Center());
      Matrix44f tr2; tr2.SetTranslate(bbox.Center());
      Point3f rotAxis=Point3f(0,0,1) ^ plane.Direction();
      rotAxis.Normalize();
      float angleRad = Angle(Point3f(0,0,1),plane.Direction());
      Matrix44f rt; rt.SetRotateRad(angleRad,rotAxis);
      m.cm.Tr = rt*tr1;
    }

	if (ID(filter) == FP_ROTATE ) {
		Matrix44f trRot; trRot.SetIdentity();
		Point3f axis;
		Point3f tranVec;
		Matrix44f trTran,trTranInv;

		switch(par.getEnum("rotAxis"))
		{
			case 0: axis=Point3f(1,0,0); break;
			case 1: axis=Point3f(0,1,0);break;
			case 2: axis=Point3f(0,0,1);break;
			case 3: axis=par.getPoint3f("customAxis");break;
		}
		switch(par.getEnum("rotCenter"))
		{
			case 0: tranVec=Point3f(0,0,0); break;
			case 1: tranVec=m.cm.bbox.Center(); break;
			case 2: tranVec=par.getPoint3f("customCenter");break;
		}

		float angleDeg= par.getDynamicFloat("angle");
		float snapAngle = par.getFloat("snapAngle");
		if(par.getBool("snapFlag"))
			angleDeg = floor(angleDeg / snapAngle)*snapAngle;

		trRot.SetRotateDeg(angleDeg,axis);
		trTran.SetTranslate(tranVec);
		trTranInv.SetTranslate(-tranVec);
		m.cm.Tr=trTran*trRot*trTranInv;
	}

  if (ID(filter) == (FP_PRINCIPAL_AXIS) ) {
    if(par.getBool("pointsFlag"))
    {
      Matrix33f cov;
      Point3f bp(0,0,0);
      vector<Point3f> PtVec;
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
      int n;
      Jacobi(cov,eigenvecVector,eigenvecMatrix,n);
      for(int i=0;i<3;i++)
        qDebug("%8.3f %8.3f %8.3f ",eigenvecMatrix[i][0],eigenvecMatrix[i][1],eigenvecMatrix[i][2]);

      qDebug("\n%8.3f %8.3f %8.3f ",eigenvecVector[0],eigenvecVector[1],eigenvecVector[2]);

      Matrix44f trTran; trTran.SetIdentity();
      for(int i=0;i<3;++i)
        for(int j=0;j<3;++j)
          trTran[i][j] = eigenvecMatrix[i][j];
      trTran.transposeInPlace();
      m.cm.Tr=trTran;
    }
    else
    {
      tri::Inertia<CMeshO> I;
      I.Compute(m.cm);

      Matrix44f PCA;
      Point4f pcav;
      I.InertiaTensorEigen(PCA,pcav);
      for(int i=0;i<4;i++)
          qDebug("%8.3f %8.3f %8.3f %8.3f",PCA[i][0],PCA[i][1],PCA[i][2],PCA[i][3]);
      PCA.transposeInPlace();

      for(int i=0;i<4;i++)
          qDebug("%8.3f %8.3f %8.3f %8.3f",PCA[i][0],PCA[i][1],PCA[i][2],PCA[i][3]);
      m.cm.Tr=PCA;
    }
  }
	if (ID(filter) == (FP_CENTER) ) {
		Matrix44f trTran; trTran.SetIdentity();

		float xScale= par.getDynamicFloat("axisX");
		float yScale= par.getDynamicFloat("axisY");
		float zScale= par.getDynamicFloat("axisZ");

		trTran.SetTranslate(xScale,yScale,zScale);
		if(par.getBool("centerFlag"))
			trTran.SetTranslate(-m.cm.bbox.Center());

		m.cm.Tr=trTran;
	}

	if (ID(filter) == (FP_SCALE) ) {
		Matrix44f trScale; trScale.SetIdentity();
		Point3f tranVec;
		Matrix44f trTran,trTranInv;

		float xScale= par.getDynamicFloat("axisX");
		float yScale= par.getDynamicFloat("axisY");
		float zScale= par.getDynamicFloat("axisZ");
		if(par.getBool("uniformFlag"))
			trScale.SetScale(xScale,xScale,xScale);
		else
			trScale.SetScale(xScale,yScale,zScale);
		if(par.getBool("unitFlag"))
		{
			float maxSide= max(m.cm.bbox.DimX(),max(m.cm.bbox.DimY(),m.cm.bbox.DimZ()));
			trScale.SetScale(1.0/maxSide,1.0/maxSide,1.0/maxSide);
		}
		switch(par.getEnum("scaleCenter"))
		{
			case 0: tranVec=Point3f(0,0,0); break;
			case 1: tranVec=m.cm.bbox.Center(); break;
			case 2: tranVec=par.getPoint3f("customCenter");break;
		}

		trTran.SetTranslate(tranVec);
		trTranInv.SetTranslate(-tranVec);
		m.cm.Tr=trTran*trScale*trTranInv;
		//m.cm.Tr=trScale;
	}
	if (ID(filter) == (FP_FLIP_AND_SWAP) ) {

		Matrix44f tr; tr.SetIdentity();
		if(par.getBool("flipX")) { Matrix44f flipM; flipM.SetIdentity(); flipM[0][0]=-1.0f; tr *= flipM; }
		if(par.getBool("flipY")) { Matrix44f flipM; flipM.SetIdentity(); flipM[1][1]=-1.0f; tr *= flipM; }
		if(par.getBool("flipZ")) { Matrix44f flipM; flipM.SetIdentity(); flipM[2][2]=-1.0f; tr *= flipM; }

		if(par.getBool("swapXY")) { Matrix44f swapM; swapM.SetIdentity();
			swapM[0][0]=0.0f; swapM[0][1]=1.0f;
			swapM[1][0]=1.0f; swapM[1][1]=0.0f;
			tr *= swapM; }
		if(par.getBool("swapXZ")) { Matrix44f swapM; swapM.SetIdentity();
			swapM[0][0]=0.0f; swapM[0][2]=1.0f;
			swapM[2][0]=1.0f; swapM[2][2]=0.0f;
			tr *= swapM; }
		if(par.getBool("swapYZ")) { Matrix44f swapM; swapM.SetIdentity();
			swapM[1][1]=0.0f; swapM[1][2]=1.0f;
			swapM[2][1]=1.0f; swapM[2][2]=0.0f;
			tr *= swapM; }
		m.cm.Tr=tr;
	}



  if (ID(filter) == (FP_NORMAL_EXTRAPOLATION) ) {
    tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
    NormalExtrapolation<vector<CVertexO> >::ExtrapolateNormals(m.cm.vert.begin(), m.cm.vert.end(), par.getInt("K"),-1,NormalExtrapolation<vector<CVertexO> >::IsCorrect,  cb);
	}

  if (ID(filter) == (FP_COMPUTE_PRINC_CURV_DIR) ) {

	  if(par.getBool("Autoclean")){
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			Log(GLLogStream::FILTER, "Removed %d unreferenced vertices",delvert);
	  }

	  switch(par.getEnum("Method")){

		  case 0:
        if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0 ) {
				errorMessage = "Mesh has some not 2-manifold faces, cannot compute principal curvature directions"; // text
				return false; // can't continue, mesh can't be processed
				}
			  vcg::tri::UpdateCurvature<CMeshO>::PrincipalDirections(m.cm); break;
		  case 1: vcg::tri::UpdateCurvature<CMeshO>::PrincipalDirectionsPCA(m.cm,m.cm.bbox.Diag()/20.0,false); break;
		  case 2:
			  vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
			  vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
        if ( ! tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0) {
				errorMessage = "Mesh has some not 2-manifold faces, cannot compute principal curvature directions"; // text
				return false; // can't continue, mesh can't be processed
				}
			  vcg::tri::UpdateCurvature<CMeshO>::PrincipalDirectionsNormalCycles(m.cm); break;
		  case 3:
			  vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
        tri::UpdateCurvatureFitting<CMeshO>::computeCurvature(m.cm);
				break;
		  default:assert(0);break;

	  }
  }


  if(ID(filter) == (FP_CLOSE_HOLES))
	  {
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
	  tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);

	  // hole filling filter does not correctly update the border flags (but the topology is still ok!)
	  m.clearDataMask(MeshModel::MM_FACEFLAGBORDER);
	  if(NewFaceSelectedFlag)
	  {
		tri::UpdateSelection<CMeshO>::ClearFace(m.cm);
		for(size_t i=OriginalSize;i<m.cm.face.size();++i)
		  if(!m.cm.face[i].IsD()) m.cm.face[i].SetS();
	  }
	  //tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	  }

  if(ID(filter) == (FP_CYLINDER_UNWRAP))
	  {
		float startAngleDeg = par.getFloat("startAngle");
		float endAngleDeg = par.getFloat("endAngle");
		float radius = par.getFloat("radius");
		// a

		// Number of unrolling. (e.g. if the user set start=-15 end=375 there are two loops)
		int numLoop =	int(1+(endAngleDeg-startAngleDeg)/360.0);

		vector< vector<int> > VertRefLoop(numLoop);
		for(int i=0;i<numLoop;++i)
				VertRefLoop[i].resize(m.cm.vert.size(),-1);

		Log("Computing %i loops from %f to %f",numLoop,startAngleDeg,endAngleDeg);

		MeshModel *um=md.addNewMesh("Unrolled Mesh");
		float avgZ=0;
		CMeshO::VertexIterator vi;
		// First loop duplicate accordingly the vertices.
				for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
				if(!(*vi).IsD())
				{
					Point3f p = (*vi).P();
					float ro,theta,phi;
					p.Y()=0;
					p.ToPolar(ro,theta,phi);
					float thetaDeg = math::ToDeg(theta);
					int loopIndex =0;
					while(thetaDeg<endAngleDeg)
					{
						if(thetaDeg>=startAngleDeg)
							{
								CMeshO::VertexIterator nvi = tri::Allocator<CMeshO>::AddVertices(um->cm,1);
								VertRefLoop[loopIndex][vi-m.cm.vert.begin()] = nvi - um->cm.vert.begin();
								nvi->P().X()=math::ToRad(thetaDeg);
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
							for(int ii1=0;ii1<endIt;ii1++)
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
													nfi->V(0) = &um->cm.vert[i0];
													nfi->V(1) = &um->cm.vert[i1];
													nfi->V(2) = &um->cm.vert[i2];
												}
									}
								}
						loopIndex++;
					}
				}
				tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(um->cm);
				tri::UpdateBounding<CMeshO>::Box(um->cm);

			return true;
		}
  if(ID(filter) == FP_REFINE_HALF_CATMULL)
  {
	  m.updateDataMask(MeshModel::MM_FACEQUALITY);
	  tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
	  tri::UpdateNormals<CMeshO>::PerBitQuadFaceNormalized(m.cm);
	  assert(tri::BitQuadCreation<CMeshO>::IsBitTriQuadConventional(m.cm));
	  m.clearDataMask(MeshModel::MM_FACEFACETOPO);
	  m.updateDataMask(MeshModel::MM_FACEFACETOPO);
	  m.updateDataMask(MeshModel::MM_POLYGONAL);
  }
  if(ID(filter) == FP_REFINE_CATMULL)
  { // in practice it is just a simple double application of the previous step.
	  m.updateDataMask(MeshModel::MM_FACEQUALITY);
	  tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
	  assert(tri::BitQuadCreation<CMeshO>::IsBitTriQuadConventional(m.cm));
	  tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	  tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
	  tri::UpdateNormals<CMeshO>::PerBitQuadFaceNormalized(m.cm);
	  m.clearDataMask(MeshModel::MM_FACEFACETOPO);
  }
  if(ID(filter) == FP_QUAD_PAIRING)
  {
	  m.updateDataMask(MeshModel::MM_FACEQUALITY);

	  tri::BitQuadCreation<CMeshO>::MakeTriEvenBySplit(m.cm);
	  bool ret = tri::BitQuadCreation<CMeshO>::MakePureByFlip(m.cm,100);
	  if(!ret) Log("Warning BitQuadCreation<CMeshO>::MakePureByFlip failed.");
	  tri::UpdateNormals<CMeshO>::PerBitQuadFaceNormalized(m.cm);
	  m.updateDataMask(MeshModel::MM_POLYGONAL);
  }
  if(ID(filter)==FP_FAUX_CREASE)
  {
	float AngleDeg = par.getFloat("AngleDeg");
	tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	tri::UpdateFlags<CMeshO>::FaceFauxCrease(m.cm,math::ToRad(AngleDeg));
  }

	if (ID(filter) == FP_VATTR_SEAM)
	{
		unsigned int vmask = 0;
		vmask |= vcg::tri::AttributeSeam::POSITION_PER_VERTEX;
		/*
		switch (par.getEnum("PositionMode"))
		{
			case 0  : break;
			case 1  : vmask |= vcg::tri::AttributeSeam::POSITION_PER_VERTEX; break;
			default : break;
		}
		*/

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
	}

	return true;
}

int ExtraMeshFilterPlugin::postCondition(QAction * filter) const
{
	switch (ID(filter))
	{
		case FP_FLIP_AND_SWAP    :
		case FP_SCALE            :
		case FP_CENTER           :
		case FP_ROTATE           :
		case FP_RESET_TRANSFORM  : return MeshModel::MM_TRANSFMATRIX;

		default                  : return MeshModel::MM_UNKNOWN;
	}
}

Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
