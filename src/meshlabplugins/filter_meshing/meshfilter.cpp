/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.																											 *
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
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/hole.h>
#include <vcg/complex/trimesh/refine_loop.h>
#include <vcg/complex/trimesh/bitquad_support.h>
#include <vcg/complex/trimesh/bitquad_creation.h>
#include <vcg/complex/trimesh/clustering.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/space/normal_extrapolation.h>
#include "quadric_tex_simp.h"
#include "quadric_simp.h"
#include "../../meshlab/GLLogStream.h"

using namespace std;
using namespace vcg;

void QuadricTexSimplification(CMeshO &m,int  TargetFaceNum, bool Selected, CallBackPos *cb);

ExtraMeshFilterPlugin::ExtraMeshFilterPlugin()
{
  typeList << FP_LOOP_SS<<
    FP_BUTTERFLY_SS<<
    FP_REMOVE_UNREFERENCED_VERTEX<<
    FP_REMOVE_DUPLICATED_VERTEX<<
    FP_REMOVE_FACES_BY_AREA<<
    FP_REMOVE_FACES_BY_EDGE<<
    FP_CLUSTERING<<
    FP_QUADRIC_SIMPLIFICATION<<
	  FP_QUADRIC_TEXCOORD_SIMPLIFICATION<<
    FP_MIDPOINT<<
    FP_REORIENT <<
    FP_INVERT_FACES<<
		FP_REMOVE_NON_MANIFOLD_FACE<<
		FP_REMOVE_NON_MANIFOLD_VERTEX<<
    FP_NORMAL_EXTRAPOLATION<<
		FP_COMPUTE_PRINC_CURV_DIR<<
		FP_CLOSE_HOLES<<
		FP_FREEZE_TRANSFORM<<
		FP_TRANSFORM<<
		FP_CYLINDER_UNWRAP<<
		FP_REFINE_CATMULL;


  FilterIDType tt;

	foreach(tt , types())
		actionList << new QAction(filterName(tt), this);

	//genericELD = new GenericELDialog();
	//genericELD->hide();

	transformDialog = new TransformDialog();
	transformDialog->hide();

	lastq_QualityThr = 0.3f;
	lastq_PreserveBoundary = false;
	lastq_PreserveNormal = false;
	lastq_OptimalPlacement = true;
	lastq_Selected = false;
	lastq_PlanarQuadric = false;
	lastq_QualityWeight =false;

	lastqtex_QualityThr = 0.3f;
	lastqtex_extratw = 0.0;
}

const ExtraMeshFilterPlugin::FilterClass ExtraMeshFilterPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_REMOVE_UNREFERENCED_VERTEX :
    case FP_REMOVE_DUPLICATED_VERTEX :
    case FP_REMOVE_FACES_BY_AREA:
    case FP_REMOVE_FACES_BY_EDGE :
    case FP_REMOVE_NON_MANIFOLD_FACE:
    case FP_REMOVE_NON_MANIFOLD_VERTEX:
      return MeshFilterInterface::Cleaning;
    case FP_BUTTERFLY_SS :
    case FP_LOOP_SS :
    case FP_MIDPOINT :
		case FP_QUADRIC_SIMPLIFICATION :
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION :
		case FP_CLUSTERING :
		case FP_CLOSE_HOLES:
		case FP_REFINE_CATMULL:
         return MeshFilterInterface::Remeshing;
		case FP_NORMAL_EXTRAPOLATION:
		case FP_INVERT_FACES:
		case FP_REORIENT :
		case FP_COMPUTE_PRINC_CURV_DIR:
		case FP_TRANSFORM:
						 return MeshFilterInterface::Normal;
		case FP_FREEZE_TRANSFORM:
							 return FilterClass(MeshFilterInterface::Normal + MeshFilterInterface::Layer);
		case FP_CYLINDER_UNWRAP:
						 return MeshFilterInterface::Smoothing;

    default : assert(0); return MeshFilterInterface::Generic;
  }
}


const QString ExtraMeshFilterPlugin::filterName(FilterIDType filter) const
{
 switch(filter)
  {
		case FP_LOOP_SS :		                  return QString("Loop Subdivision Surfaces");
		case FP_BUTTERFLY_SS :								return QString("Butterfly Subdivision Surfaces");
		case FP_REMOVE_UNREFERENCED_VERTEX :	return QString("Remove Unreferenced Vertex");
		case FP_REMOVE_DUPLICATED_VERTEX :		return QString("Remove Duplicated Vertex");
		case FP_REMOVE_FACES_BY_AREA :     		return QString("Remove Zero Area Faces");
		case FP_REMOVE_FACES_BY_EDGE :				return QString("Remove Faces with edges longer than...");
		case FP_QUADRIC_SIMPLIFICATION :      return QString("Quadric Edge Collapse Decimation");
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION :      return QString("Quadric Edge Collapse Decimation (with texture)");
		case FP_CLUSTERING :	                return QString("Clustering decimation");
		case FP_MIDPOINT :										return QString("Midpoint Subdivision Surfaces");
		case FP_REORIENT :	                  return QString("Re-Orient all faces coherentely");
		case FP_INVERT_FACES:									return QString("Invert Faces Orientation");
		case FP_TRANSFORM:	                	return QString("Apply Transform");
		case FP_FREEZE_TRANSFORM:	            return QString("Freeze Current Matrix");
		case FP_REMOVE_NON_MANIFOLD_FACE:	        return QString("Remove Non Manifold Faces");
		case FP_REMOVE_NON_MANIFOLD_VERTEX:	        return QString("Remove Non Manifold Vertices");
		case FP_NORMAL_EXTRAPOLATION:	        return QString("Compute normals for point sets");
		case FP_COMPUTE_PRINC_CURV_DIR:	        return QString("Compute curvature principal directions");
		case FP_CLOSE_HOLES:	          return QString("Close Holes");
		case FP_CYLINDER_UNWRAP:	     return QString("Geometric Cylindrical Unwrapping");
		case FP_REFINE_CATMULL:				return QString("Catmull-Clark Subdivision Surfaces");



		default: assert(0);
  }
  return QString("error!");

}

ExtraMeshFilterPlugin::~ExtraMeshFilterPlugin() {
//	if(genericELD) delete genericELD;
//	if(transformDialog)		delete transformDialog;
//	for (int i = 0; i < actionList.count() ; i++ ) {
//		delete actionList.at(i);
//	}
}

const QString ExtraMeshFilterPlugin::filterInfo(FilterIDType filterID) const
{
  switch(filterID)
  {
    case FP_LOOP_SS :                   return tr("Apply Loop's Subdivision Surface algorithm. It is an approximate method which subdivide each triangle in four faces. It works for every triangle and has rules for extraordinary vertices");
    case FP_BUTTERFLY_SS :  			      return tr("Apply Butterfly Subdivision Surface algorithm. It is an interpolated method, defined on arbitrary triangular meshes. The scheme is known to be C1 but not C2 on regular meshes");
    case FP_MIDPOINT : 			            return tr("Apply a plain subdivision scheme where every edge is splitted on its midpoint");
    case FP_REMOVE_UNREFERENCED_VERTEX: return tr("Check for every vertex on the mesh if it is referenced by a face and removes it");
    case FP_REMOVE_DUPLICATED_VERTEX : 	return tr("Check for every vertex on the mesh if there are two vertices with same coordinates and removes it");
    case FP_REMOVE_FACES_BY_AREA : 			return tr("Removes null faces (the one with area equal to zero)");
    case FP_REMOVE_FACES_BY_EDGE : 			return tr("Remove from the mesh all triangles whose have an edge with lenght greater or equal than a threshold");
    case FP_REMOVE_NON_MANIFOLD_FACE : 			return tr("Remove non 2-manifold edges by removing some of the faces incident on non manifold edges");
    case FP_REMOVE_NON_MANIFOLD_VERTEX : 		return tr("Remove non 2-manifold vertices, that vertices where the number of faces that can be reached using only face-face connectivity is different from the number of faces actually incident on that vertex.<br>"
																											"Typical example think to two isolated triangles connected by a single vertex building a <i>hourglass</i> shape.");
    case FP_CLUSTERING : 			          return tr("Collapse vertices by creating a three dimensional grid enveloping the mesh and discretizes them based on the cells of this grid");
    case FP_QUADRIC_SIMPLIFICATION: 		return tr("Simplify a mesh using a Quadric based Edge Collapse Strategy, better than clustering but slower");
    case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:return tr("Simplify a textured mesh using a Quadric based Edge Collapse Strategy, better than clustering but slower");
    case FP_REORIENT : 			            return tr("Re-orient in a consistent way all the faces of the mesh");
    case FP_INVERT_FACES : 			        return tr("Invert faces orientation, flip the normal of the mesh");
    case FP_TRANSFORM : 	              return tr("Apply transformation, you can rotate, translate or scale the mesh");
    case FP_FREEZE_TRANSFORM : 	        return tr("Freeze the current transformation matrix into the coords of the vertices of the mesh");
    case FP_NORMAL_EXTRAPOLATION :      return tr("Compute the normals of the vertices of a  mesh without exploiting the triangle connectivity, useful for dataset with no faces");
		case FP_COMPUTE_PRINC_CURV_DIR:			return tr("Compute the principal directions of curvature with several algorithms");
		case FP_CLOSE_HOLES :								return tr("Close holes smaller than a given threshold");
		case FP_CYLINDER_UNWRAP:						return tr("Unwrap the geometry of current mesh along a clylindrical equatorial projection. The cylindrical projection axis is centered on the origin and directed along the vertical <b>Y</b> axis.");
		case FP_REFINE_CATMULL:				return QString("Apply the Catmull-Clark Subdivision Surfaces. Note Position of the new vertces is simply linearly interpolated. If the mesh is triangle based (no faux edges) it generate a quad mesh otherwise it honores it the faux-edge bits");

		default : assert(0);
	}
  return QString();
}

const int ExtraMeshFilterPlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
		case FP_REFINE_CATMULL : return MeshModel::MM_FACEFACETOPO;
    case FP_REMOVE_NON_MANIFOLD_FACE:
    case FP_LOOP_SS :
    case FP_BUTTERFLY_SS :
    case FP_MIDPOINT :
    case FP_CLOSE_HOLES :
           return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;
    case FP_REMOVE_NON_MANIFOLD_VERTEX:
    case FP_REORIENT:             return MeshModel::MM_FACEFACETOPO;
    case FP_REMOVE_UNREFERENCED_VERTEX:
    case FP_REMOVE_DUPLICATED_VERTEX:
    case FP_REMOVE_FACES_BY_AREA:
    case FP_REMOVE_FACES_BY_EDGE:
    case FP_CLUSTERING:
    case FP_TRANSFORM:
    case FP_FREEZE_TRANSFORM:
    case FP_NORMAL_EXTRAPOLATION:
    case FP_INVERT_FACES:
		case FP_CYLINDER_UNWRAP:						
												return 0;
	case FP_COMPUTE_PRINC_CURV_DIR: return	MeshModel::MM_VERTCURVDIR	|
											MeshModel::MM_FACEMARK		|
											MeshModel::MM_VERTFACETOPO	|
											MeshModel::MM_FACEFACETOPO;
    case FP_QUADRIC_SIMPLIFICATION:
	case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:
		return MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_VERTMARK ;
    default: assert(0);
  }
  return 0;
}


// this function builds and intializes with the default values (that can depend on the current mesh or selection)
// the list of parameters that a filter requires.
// return
//		true if has some parameters
//		false is has no params

void ExtraMeshFilterPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
	float maxVal;
	QStringList methods;
	switch(ID(action))
	 {
		case FP_COMPUTE_PRINC_CURV_DIR:
		methods.push_back("Taubin approximation");
		methods.push_back("Principal Component Analysis");
		methods.push_back("Normal Cycles");
		parlst.addParam(new RichEnum("Method", 0, methods, tr("Method:"), tr("Choose a method")));
		parlst.addParam(new RichBool("Autoclean",true,QString("Remove Unreferenced Vertices")));
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
		  parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Affect only selected faces"));
			break;
		case FP_REMOVE_FACES_BY_EDGE:
		  maxVal = m.cm.bbox.Diag();
		  parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the faces with an edge <b>longer</b> than this threshold will be deleted. Useful for removing long skinny faces obtained by bad triangulation of range maps."));
		  parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Affect only selected faces"));
			break;
		case FP_CLUSTERING:
		  maxVal = m.cm.bbox.Diag();
		  parlst.addParam(new RichAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Cell Size", "The size of the cell of the clustering grid. Smaller the cell finer the resulting mesh. For obtaining a very coarse mesh use larger values."));
		  parlst.addParam(new RichBool ("Selected",m.cm.sfn>0,"Affect only selected faces"));
		  break;
		case FP_CYLINDER_UNWRAP:	
			parlst.addParam(new RichFloat("startAngle", 0,"Start angle (deg)", "The starting angle of the unrolling process."));
		  parlst.addParam(new RichFloat("endAngle",360,"End angle (deg)","The ending angle of the unrolling process. Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n"));
			parlst.addParam(new RichFloat("radius", 0,"Projection Radius", "If non zero, this parameter specifies the desired radius of the reference cylinder used for the projection. Changing this parameter affect the <b>X</b> horizontal scaling of the resulting mesh. If zero (default) the average distance of the mesh from the axis is chosen."));
			
		break;
		case FP_NORMAL_EXTRAPOLATION:
			parlst.addParam(new RichInt ("K",(int)10,"Number of neigbors","The number of neighbors used to estimate and propagate normals."));
			break;
			 }
}

// Return true if the specified action has an automatic dialog.
// return false if the action has no parameters or has an self generated dialog.
bool ExtraMeshFilterPlugin::autoDialog(QAction *action)
{
	 switch(ID(action))
	 {
		case FP_QUADRIC_SIMPLIFICATION:
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:
		case FP_COMPUTE_PRINC_CURV_DIR:
		case FP_CLOSE_HOLES:
		case FP_LOOP_SS :
		case FP_BUTTERFLY_SS :
		case FP_MIDPOINT :
		case FP_REMOVE_FACES_BY_EDGE:
		case FP_CLUSTERING:
		case FP_CYLINDER_UNWRAP:	
		case FP_NORMAL_EXTRAPOLATION:
		  return true;
  	 }
  return false;
}
bool ExtraMeshFilterPlugin::customDialog(QAction *action)
{
	 switch(ID(action))
	 {
		 case FP_TRANSFORM:
			 return true;
	 }
  return false;
}

// this function is called to fill the parameter list
// It is called only for filters that have a not empty list of parameters and
// that do not use the autogenerated dialog, but want a personalized dialog.

bool ExtraMeshFilterPlugin::getCustomParameters(QAction *action, QWidget * /*parent*/, MeshModel &m, RichParameterSet & params, MainWindowInterface *mw)
{
 switch(ID(action))
  {
	case FP_TRANSFORM:
      {
        transformDialog->setMesh(&m.cm);
				transformDialog->setMainWindow(mw);
				transformDialog->setAction(action);
				transformDialog->show();
				transformDialog->resize(transformDialog->size());

		    int continueValue = transformDialog->exec();
 		    if (continueValue == QDialog::Rejected)
 			    return false;

      	Matrix44f matrixDlg = transformDialog->getTransformation();
        params.addParam(new RichMatrix44f("Transform",matrixDlg));
				mw->executeFilter(action,params, false);
        break;
	  }
    default :assert(0);
  }
  return true;
}

bool ExtraMeshFilterPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	MeshModel &m=*md.mm();

	if( ID(filter) == FP_LOOP_SS ||
			ID(filter) == FP_BUTTERFLY_SS ||
			ID(filter) == FP_MIDPOINT )
		{
			if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) )
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
			if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
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
      bool selected  = par.getBool("Selected");
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
	if (ID(filter) == (FP_TRANSFORM) ) {
			Matrix44f matrixPar= par.getMatrix44("Transform");
			Log(GLLogStream::FILTER, qPrintable(transformDialog->getLog()) );
			m.cm.Tr=matrixPar;
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

  if (ID(filter) == (FP_NORMAL_EXTRAPOLATION) ) {
    NormalExtrapolation<vector<CVertexO> >::ExtrapolateNormals(m.cm.vert.begin(), m.cm.vert.end(), par.getInt("K"),-1,NormalExtrapolation<vector<CVertexO> >::IsCorrect,  cb);
	}

  if (ID(filter) == (FP_COMPUTE_PRINC_CURV_DIR) ) {

	  if(par.getBool("Autoclean")){
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			Log(GLLogStream::FILTER, "Removed %d unreferenced vertices",delvert);
	  }

	  switch(par.getEnum("Method")){

		  case 0:
			  if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
				errorMessage = "Mesh has some not 2-manifold faces, cannot compute principal curvature directions"; // text
				return false; // can't continue, mesh can't be processed
				}
			  vcg::tri::UpdateCurvature<CMeshO>::PrincipalDirections(m.cm); break;
		  case 1: vcg::tri::UpdateCurvature<CMeshO>::PrincipalDirectionsPCA(m.cm,m.cm.bbox.Diag()/20.0,false); break;
		  case 2:
			  vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
			  vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
			  if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
				errorMessage = "Mesh has some not 2-manifold faces, cannot compute principal curvature directions"; // text
				return false; // can't continue, mesh can't be processed
				}
			  vcg::tri::UpdateCurvature<CMeshO>::PrincipalDirectionsNormalCycles(m.cm); break;
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
		if(ID(filter) == (FP_REFINE_CATMULL))
		{
			tri::BitQuadCreation<CMeshO>::MakePureByRefine(m.cm);
			tri::UpdateNormals<CMeshO>::PerBitQuadFaceNormalized(m.cm);			
			return true;
		}
	 		
	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
