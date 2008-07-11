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
/****************************************************************************
  History
$Log: meshfilter.cpp,v $
Revision 1.107  2008/04/04 13:22:30  cignoni
Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h, and added a manifoldness check on the reorient tool.

Revision 1.106  2008/03/18 10:33:53  cignoni
added Post-Simplification cleaning filter, improved help

Revision 1.105  2008/03/06 08:25:04  cignoni
updated to the error message reporting style for filters

Revision 1.104  2008/02/12 14:21:39  cignoni
changed the function getParameter into the more meaningful getCustomParameter and added the freeze option

Revision 1.103  2008/02/10 09:32:39  cignoni
added a missing clear border flag in remove non manifold

Revision 1.102  2007/11/21 09:48:03  cignoni
better help and color clustering

Revision 1.101  2007/11/05 23:44:23  cignoni
Added tex simplification check, selection to paso doble and better comments

Revision 1.100  2007/10/16 12:19:25  cignoni
mismatch between float and absperc parameters

Revision 1.99  2007/10/06 23:39:01  cignoni
Updated used defined dialog to the new filter interface.

Revision 1.98  2007/10/02 08:13:44  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.97  2007/07/24 07:20:20  cignoni
Added Freeze transform and improved transformation dialog

Revision 1.96  2007/04/20 10:09:56  cignoni
issue on vertex selection from face selection

Revision 1.95  2007/04/16 10:16:25  cignoni
Added missing info on filtering actions

Revision 1.94  2007/04/16 09:25:29  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing again...


****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/hole.h>
#include <vcg/complex/trimesh/clustering.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/space/normal_extrapolation.h>

#include "refine_loop.h"

#include "../../meshlab/GLLogStream.h"
#include "../../meshlab/LogStream.h"

using namespace std;
using namespace vcg;

void QuadricSimplification(CMeshO &m,int  TargetFaceNum, float QualityThr, bool PreserveBoundary, bool PreserveNormal, bool OptimalPlacement, bool Selected, CallBackPos *cb);
void QuadricTexSimplification(CMeshO &m,int  TargetFaceNum, float QualityThr,float c, bool optimalPlacement,CallBackPos *cb);

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
    FP_REMOVE_NON_MANIFOLD<<
    FP_NORMAL_EXTRAPOLATION<<
    FP_CLOSE_HOLES<<
		FP_FREEZE_TRANSFORM<<
		FP_TRANSFORM;
  
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
    case FP_REMOVE_NON_MANIFOLD:
      return MeshFilterInterface::Cleaning; 
    case FP_BUTTERFLY_SS :
    case FP_LOOP_SS :
    case FP_MIDPOINT :
         return MeshFilterInterface::Remeshing; 
		case FP_NORMAL_EXTRAPOLATION:
		case FP_INVERT_FACES:
		case FP_REORIENT :
				 return MeshFilterInterface::Normal;

    default : return MeshFilterInterface::Generic;
  }
}


const QString ExtraMeshFilterPlugin::filterName(FilterIDType filter) 
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
	case FP_REORIENT :	                  return QString("Re-orient");
	case FP_INVERT_FACES:									return QString("Invert Faces");
	case FP_TRANSFORM:	                	return QString("Apply Transform");
	case FP_FREEZE_TRANSFORM:	            return QString("Freeze Current Matrix");
	case FP_REMOVE_NON_MANIFOLD:	        return QString("Remove Non Manifold Faces");
	case FP_NORMAL_EXTRAPOLATION:	        return QString("Compute normals for point sets");
	case FP_CLOSE_HOLES:	          return QString("Close Holes");
          
    
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

const QString ExtraMeshFilterPlugin::filterInfo(FilterIDType filterID)
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
    case FP_REMOVE_NON_MANIFOLD : 			return tr("Remove non manifold edges by removing some of the faces incident on non manifold edges");  
    case FP_CLUSTERING : 			          return tr("Collapse vertices by creating a three dimensional grid enveloping the mesh and discretizes them based on the cells of this grid");  
    case FP_QUADRIC_SIMPLIFICATION: 		return tr("Simplify a mesh using a Quadric based Edge Collapse Strategy, better than clustering but slower");          
    case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:return tr("Simplify a textured mesh using a Quadric based Edge Collapse Strategy, better than clustering but slower");          
    case FP_REORIENT : 			            return tr("Re-orient in a consistent way all the faces of the mesh");  
    case FP_INVERT_FACES : 			        return tr("Invert faces orientation, flip the normal of the mesh");  
    case FP_TRANSFORM : 	              return tr("Apply transformation, you can rotate, translate or scale the mesh");  
    case FP_FREEZE_TRANSFORM : 	        return tr("Freeze the current transformation matrix into the coords of the vertices of the mesh");  
    case FP_NORMAL_EXTRAPOLATION :      return tr("Compute the normals of the vertices of a  mesh without exploiting the triangle connectivity, useful for dataset with no faces"); 
    case FP_CLOSE_HOLES :         return tr("Close holes smaller than a given threshold"); 
		default : assert(0);
	}
  return QString();
}


const PluginInfo &ExtraMeshFilterPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.5");
	 ai.Author = ("Paolo Cignoni, Mario Latronico, Andrea Venturi, Marco Pirosu");
   return ai;
 }

const int ExtraMeshFilterPlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_REMOVE_NON_MANIFOLD:
    case FP_LOOP_SS :
    case FP_BUTTERFLY_SS : 
    case FP_MIDPOINT :      
    case FP_CLOSE_HOLES :
           return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG;
    case FP_REORIENT:             return MeshModel::MM_FACETOPO;
    case FP_REMOVE_UNREFERENCED_VERTEX:
    case FP_REMOVE_DUPLICATED_VERTEX:
    case FP_REMOVE_FACES_BY_AREA:
    case FP_REMOVE_FACES_BY_EDGE:
    case FP_CLUSTERING:
    case FP_TRANSFORM:
    case FP_FREEZE_TRANSFORM:
    case FP_NORMAL_EXTRAPOLATION:
    case FP_INVERT_FACES:         return 0;
    case FP_QUADRIC_SIMPLIFICATION:
	case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:
		return MeshModel::MM_VERTFACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_VERTMARK ;
    default: assert(0); 
  }
  return 0;
}


// this function builds and intializes with the default values (that can depend on the current mesh or selection) 
// the list of parameters that a filter requires. 
// return 
//		true if has some parameters 
//		false is has no params
void ExtraMeshFilterPlugin::initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	float maxVal;
	
	switch(ID(action))
	 {
		case FP_QUADRIC_SIMPLIFICATION:
		  parlst.addInt  ("TargetFaceNum", (m.cm.sfn>0) ? m.cm.sfn/2 : m.cm.fn/2,"Target number of faces");
		  parlst.addFloat("QualityThr",lastq_QualityThr,"Quality threshold","Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n");
		  parlst.addBool ("PreserveBoundary",lastq_PreserveBoundary,"Preserve Boundary of the mesh","The simplification process tries not to destroy mesh boundaries");
		  parlst.addBool ("PreserveNormal",lastq_PreserveNormal,"Preserve Normal","Try to avoid face flipping effects and try to preserve the original orientation of the surface");
		  parlst.addBool ("OptimalPlacement",lastq_OptimalPlacement,"Optimal position of simplified vertices","Each collapsed vertex is placed in the position minimizing the quadric error.\n It can fail (creating bad spikes) in case of very flat areas. \nIf disabled edges are collapsed onto one of the two original vertices and the final mesh is composed by a subset of the original vertices. ");
		  parlst.addBool ("AutoClean",true,"Post-simplification cleaning","After the simplification an additional set of steps is performed to clean the mesh (unreferenced vertices, bad faces, etc)");
		  parlst.addBool ("Selected",m.cm.sfn>0,"Simplify only selected faces","The simplification is applied only to the selected set of faces.\n Take care of the target number of faces!");
		  break;
		case FP_QUADRIC_TEXCOORD_SIMPLIFICATION:
		  parlst.addInt  ("TargetFaceNum",(int)(m.cm.fn/2),"Target number of faces");
		  parlst.addFloat("QualityThr",lastqtex_QualityThr,"Quality threshold","Quality threshold for penalizing bad shaped faces.<br>The value is in the range [0..1]\n 0 accept any kind of face (no penalties),\n 0.5  penalize faces with quality < 0.5, proportionally to their shape\n");
		  parlst.addFloat("Extratcoordw",lastqtex_extratw,"Texture Weight","Additional weight for each extra Texture Coordinates for every (selected) vertex");
		  parlst.addBool ("OptimalPlacement",lastq_OptimalPlacement,"Optimal position of simplified vertices","Each collapsed vertex is placed in the position minimizing the quadric error.\n It can fail (creating bad spikes) in case of very flat areas. \nIf disabled edges are collapsed onto one of the two original vertices and the final mesh is composed by a subset of the original vertices. ");
		  break;
		case FP_CLOSE_HOLES:
		  parlst.addInt ("MaxHoleSize",(int)30,"Max size to be closed ","The size is expressed as number of edges composing the hole boundary");
		  parlst.addBool("Selected",m.cm.sfn>0,"Close holes with selected faces","Only the holes with at least one of the boundary faces selected are closed");
		  parlst.addBool("NewFaceSelected",true,"Select the newly created faces","After closing a hole the faces that have been created are left selected. Any previous selection is lost. Useful for example for smoothing the newly created holes.");
		  parlst.addBool("SelfIntersection",true,"Prevent creation of selfIntersecting faces","When closing an holes it tries to prevent the creation of faces that intersect faces adjacent to the boundary of the hole. It is an heuristic, non intersetcting hole filling can be NP-complete.");
		  break;
		case FP_LOOP_SS:
		case FP_BUTTERFLY_SS: 
		case FP_MIDPOINT: 
		  maxVal = m.cm.bbox.Diag();
		  parlst.addAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the edges <b>longer</b> than this threshold will be refined.<br>Setting this value to zero will force an uniform refinement.");
		  parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
			break;
		case FP_REMOVE_FACES_BY_EDGE:
		  maxVal = m.cm.bbox.Diag();
		  parlst.addAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the faces with an edge <b>longer</b> than this threshold will be deleted. Useful for removing long skinny faces obtained by bad triangulation of range maps.");
		  parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
			break;
		case FP_CLUSTERING:
		  maxVal = m.cm.bbox.Diag();
		  parlst.addAbsPerc("Threshold",maxVal*0.01,0,maxVal,"Cell Size", "The size of the cell of the clustering grid. Smaller the cell finer the resulting mesh. For obtaining a very coarse mesh use larger values.");
		  parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
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
		case FP_CLOSE_HOLES:
		case FP_LOOP_SS :
		case FP_BUTTERFLY_SS : 
		case FP_MIDPOINT : 
		case FP_REMOVE_FACES_BY_EDGE:
		case FP_CLUSTERING:
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

bool ExtraMeshFilterPlugin::getCustomParameters(QAction *action, QWidget * /*parent*/, MeshModel &m, FilterParameterSet & params, MainWindowInterface *mw)
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
        params.addMatrix44("Transform",matrixDlg);
				mw->executeFilter(action,params);
        break;
      } 		
   default :assert(0);
  }
  return true;
}

bool ExtraMeshFilterPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	//MeshModel &m=*md->mm();

	if( getClass(filter)==Remeshing)
  {
    if ( ! tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
      errorMessage = "Mesh has some not 2 manifoldfaces, subdivision surfaces require manifoldness"; // text
      return false; // can't continue, mesh can't be processed
    }
 
    bool  selected  = par.getBool("Selected");	
    float threshold = par.getAbsPerc("Threshold");		

    switch(ID(filter)) {
      case FP_LOOP_SS :
        RefineOddEvenE<CMeshO, OddPointLoop<CMeshO>, EvenPointLoop<CMeshO> >
          (m.cm, OddPointLoop<CMeshO>(), EvenPointLoop<CMeshO>(), threshold, selected, cb);
        break;
      case FP_BUTTERFLY_SS :
        Refine<CMeshO,MidPointButterfly<CMeshO> >
          (m.cm, MidPointButterfly<CMeshO>(), threshold, selected, cb);
        break;
      case FP_MIDPOINT :
        Refine<CMeshO,MidPoint<CMeshO> >
          (m.cm, MidPoint<CMeshO>(), threshold, selected, cb);
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
    m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
		Log(GLLogStream::Info, "Removed %d faces with and edge longer than %f",delFaceNum,threshold);
	}

  if(ID(filter) == (FP_REMOVE_FACES_BY_AREA) )
	  {
	    int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
	    Log(GLLogStream::Info, "Removed %d null faces", nullFaces);
      m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
	  }

  if(ID(filter) == (FP_REMOVE_UNREFERENCED_VERTEX) )
	  {
	    int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	    Log(GLLogStream::Info, "Removed %d unreferenced vertices",delvert);
	  }

	if(ID(filter) == (FP_REMOVE_DUPLICATED_VERTEX) )
	  {
	    int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
	    Log(GLLogStream::Info, "Removed %d duplicated vertices", delvert);
	    if (delvert != 0)
	      vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if(ID(filter) == (FP_REMOVE_NON_MANIFOLD) )
	  {
	    int nonManif=tri::Clean<CMeshO>::RemoveNonManifoldFace(m.cm);
	    
			if(nonManif) Log(GLLogStream::Info, "Removed %d Non Manifold Faces", nonManif);
							else Log(GLLogStream::Info, "Mesh is two-manifold. Nothing done.", nonManif);
							
			 m.clearDataMask(MeshModel::MM_BORDERFLAG);				
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
			
//			m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

 	if(ID(filter) == (FP_CLUSTERING))
	  {
      bool selected  = par.getBool("Selected");	
      float threshold = par.getAbsPerc("Threshold");		
				vcg::tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > Grid;
				Grid.Init(m.cm.bbox,100000,threshold);
				Grid.Add(m.cm);
				Grid.Extract(m.cm);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
      m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
	  }

	if (ID(filter) == (FP_INVERT_FACES) ) 
	{
	  tri::Clean<CMeshO>::FlipMesh(m.cm);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
    m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
	}

	if (ID(filter) == (FP_FREEZE_TRANSFORM) ) {
		tri::UpdatePosition<CMeshO>::Matrix(m.cm, m.cm.Tr);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		tri::UpdateBounding<CMeshO>::Box(m.cm);
		m.cm.Tr.SetIdentity();
	}
	if (ID(filter) == (FP_TRANSFORM) ) {
			Matrix44f matrixPar= par.getMatrix44("Transform");		
			Log(GLLogStream::Info, qPrintable(transformDialog->getLog()) ); 		
			m.cm.Tr=matrixPar;
	}


	if (ID(filter) == (FP_QUADRIC_SIMPLIFICATION) ) {
  
		int TargetFaceNum = par.getInt("TargetFaceNum");		
		lastq_QualityThr = par.getFloat("QualityThr");
		lastq_PreserveBoundary = par.getBool("PreserveBoundary");
		lastq_PreserveNormal = par.getBool("PreserveNormal");
		lastq_OptimalPlacement = par.getBool("OptimalPlacement");
		lastq_Selected = par.getBool("Selected");

		QuadricSimplification(m.cm,TargetFaceNum,lastq_QualityThr, lastq_PreserveBoundary,lastq_PreserveNormal, lastq_OptimalPlacement,lastq_Selected,  cb);

		if(par.getBool("AutoClean"))
		{
			int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
			if(nullFaces) Log(GLLogStream::Info, "PostSimplification Cleaning: Removed %d null faces", nullFaces);
			int deldupvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
			if(deldupvert) Log(GLLogStream::Info, "PostSimplification Cleaning: Removed %d duplicated vertices", deldupvert);			
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			if(delvert) Log(GLLogStream::Info, "PostSimplification Cleaning: Removed %d unreferenced vertices",delvert);
			m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);			
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
		lastqtex_QualityThr = par.getFloat("QualityThr");
		lastqtex_extratw = par.getFloat("Extratcoordw");
		lastq_OptimalPlacement = par.getBool("OptimalPlacement");

		QuadricTexSimplification(m.cm,TargetFaceNum,lastqtex_QualityThr,lastqtex_extratw,lastq_OptimalPlacement, cb);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		tri::UpdateBounding<CMeshO>::Box(m.cm);
	}

  if (ID(filter) == (FP_NORMAL_EXTRAPOLATION) ) {
    NormalExtrapolation<vector<CVertexO> >::ExtrapolateNormals(m.cm.vert.begin(), m.cm.vert.end(), 10,-1,NormalExtrapolation<vector<CVertexO> >::IsCorrect,  cb);
	}

	if(ID(filter) == (FP_CLOSE_HOLES))
	  {
      size_t OriginalSize= m.cm.face.size();
      int MaxHoleSize = par.getInt("MaxHoleSize");		
	    bool SelectedFlag = par.getBool("Selected");
      bool SelfIntersectionFlag = par.getBool("SelfIntersection");
      bool NewFaceSelectedFlag = par.getBool("NewFaceSelected");
		  
      if( SelfIntersectionFlag ) 
          tri::Hole<CMeshO>::EarCuttingIntersectionFill<tri::SelfIntersectionEar< CMeshO> >(m.cm,MaxHoleSize,SelectedFlag);		
      else 
          tri::Hole<CMeshO>::EarCuttingFill<vcg::tri::MinimumWeightEar< CMeshO> >(m.cm,MaxHoleSize,SelectedFlag,cb);

      assert(tri::Clean<CMeshO>::IsFFAdjacencyConsistent(m.cm));
      tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);	    

      // hole filling filter does not correctly update the border flags (but the topology is still ok!) 
      m.clearDataMask(MeshModel::MM_BORDERFLAG);
      if(NewFaceSelectedFlag)
      {
        tri::UpdateSelection<CMeshO>::ClearFace(m.cm);   
        for(size_t i=OriginalSize;i<m.cm.face.size();++i)
          if(!m.cm.face[i].IsD()) m.cm.face[i].SetS();
      }
      //tri::UpdateTopology<CMeshO>::FaceFace(m.cm);	    
	  }

	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
