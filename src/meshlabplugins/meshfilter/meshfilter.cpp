/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                  1 * This program is free software; you can redistribute it and/or modify      *   
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
$Log$
Revision 1.44  2006/01/21 14:20:38  mariolatronico
interface work in progress on new features , need implementation

Revision 1.43  2006/01/17 00:55:01  giec
Added detacher filter with it's dialog

Revision 1.42  2006/01/15 19:23:56  mariolatronico
added log for Apply Transform

Revision 1.41  2006/01/15 13:56:48  mariolatronico
added Apply Transform progress, but work in progress. Currently it modifies mesh vertices

Revision 1.40  2006/01/09 15:25:58  mariolatronico
- updated Help and Short help
- on destructor deleted every action

Revision 1.39  2006/01/06 10:58:22  giec
Remuved filter "Color non manifold"

Revision 1.38  2006/01/05 16:46:40  mariolatronico
removed manifold test on invert faces and added update normals on reorient case

Revision 1.37  2006/01/05 16:08:54  mariolatronico
correct typo on comments

Revision 1.36  2006/01/05 15:37:56  mariolatronico
added control for mesh 2-manifold on subdivision algorithms and invert faces

Revision 1.35  2006/01/05 11:06:41  mariolatronico
Added Invert faces filter

Revision 1.34  2006/01/03 23:41:06  cignoni
corrected bug in the invocation of clean::IsOrientedMesh with twice the same boolan var

Revision 1.33  2006/01/03 11:15:37  mariolatronico
ActionInfo ai in Info() must be static because the function return a reference

Revision 1.32  2006/01/02 12:39:41  mariolatronico
used ST function to compare action->text() inside Info function

Revision 1.31  2005/12/30 11:25:09  mariolatronico
removed glArea dependency ... added log variable to log info

Revision 1.30  2005/12/30 10:16:14  giec
Added the Color non manifold filter

Revision 1.29  2005/12/29 13:50:26  mariolatronico
changed or in orValue to compile with gcc

Revision 1.28  2005/12/23 10:02:43  giec
added two filter: Midpoint and  Reoriented

Revision 1.27  2005/12/22 15:19:22  mariolatronico
changed normal cast to dynamic_cast for GLArea

Revision 1.26  2005/12/22 14:16:00  mariolatronico
added log information through glArea->log object

Revision 1.25  2005/12/22 13:32:20  mariolatronico
changed name conventions of plugin, now use ST and enum FilterType

Revision 1.24  2005/12/21 11:02:14  mariolatronico
refactored subdivision surfaces code

Revision 1.23  2005/12/19 15:11:49  mariolatronico
added decimator dialog

Revision 1.22  2005/12/18 15:01:05  mariolatronico
- added slot for threshold refine and "refine only selected vertices"

Revision 1.21  2005/12/17 13:33:19  mariolatronico
added refine dialog (preliminary code). Actually parameters are not used

Revision 1.20  2005/12/13 11:01:57  cignoni
Added callback management in standard refine

Revision 1.19  2005/12/13 09:23:39  mariolatronico
added Information on plugins

Revision 1.18  2005/12/12 22:48:42  cignoni
Added plugin info methods

Revision 1.17  2005/12/09 20:56:16  giec
Added the call to cluster algorithm

Revision 1.16  2005/12/09 18:28:34  mariolatronico
added commented code for Decimator

****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include "refine_loop.h"
#include "meshfilter.h"
#include "detacher.h"
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/position.h>

/////////////
#include "invert_faces.h"
#include "../../test/decimator/decimator.h"
#include "../../meshlab/GLLogStream.h"
#include "../../meshlab/LogStream.h"


//#include "../../meshlab/glarea.h"
////////////

using namespace vcg;
ExtraMeshFilterPlugin::ExtraMeshFilterPlugin() {
	actionList << new QAction(ST(FP_LOOP_SS), this);
	actionList << new QAction(ST(FP_BUTTERFLY_SS), this);

	actionList << new QAction(ST(FP_MIDPOINT), this);

	actionList << new QAction(ST(FP_REMOVE_UNREFERENCED_VERTEX), this);
	actionList << new QAction(ST(FP_REMOVE_DUPLICATED_VERTEX), this);
	actionList << new QAction(ST(FP_REMOVE_NULL_FACES), this);
	actionList << new QAction(ST(FP_LAPLACIAN_SMOOTH), this);

	actionList << new QAction(ST(FP_REORIENT), this);

////
	actionList << new QAction(ST(FP_DETACHER), this);
////
	actionList << new QAction(ST(FP_DECIMATOR), this);
	
	actionList << new QAction(ST(FP_INVERT_FACES), this);
	actionList << new QAction(ST(FP_TRANSFORM), this);
	
	refineDialog = new RefineDialog();
	refineDialog->hide();
	decimatorDialog = new DecimatorDialog();
	decimatorDialog->hide();
	detacherDialog = new DetacherDialog();
	detacherDialog->hide();

}

const QString ExtraMeshFilterPlugin::ST(FilterType filter) {

 switch(filter)
  {
	case FP_LOOP_SS : 
		return QString("Loop Subdivision Surfaces");
	case FP_BUTTERFLY_SS : 
		return QString("Butterfly Subdivision Surfaces");
	case FP_REMOVE_UNREFERENCED_VERTEX : 
		return QString("Remove Unreferenced Vertex");
	case FP_REMOVE_DUPLICATED_VERTEX : 
		return QString("Remove Duplicated Vertex");
	case FP_REMOVE_NULL_FACES : 
		return QString("Remove Null Faces");
	case FP_LAPLACIAN_SMOOTH : 
		return QString("Laplacian Smooth");
	case FP_DECIMATOR : 
		return QString("Decimator");
	case FP_MIDPOINT : 
		return QString("Midpoint Subdivision Surfaces");
	case FP_REORIENT : 
		return QString("Re-oriented");	
	case FP_INVERT_FACES:
		return QString("Invert Faces");
	case FP_TRANSFORM:
		return QString("Apply Transform");
	case FP_DETACHER:
		return QString("Remuve triangle above trashold");

	default: assert(0);
  }
  return QString("error!");

}

ExtraMeshFilterPlugin::~ExtraMeshFilterPlugin() {
	delete refineDialog;
	delete decimatorDialog;
	if (transformDialog)
		delete transformDialog;
	for (int i = 0; i < actionList.count() ; i++ ) {
		delete actionList.at(i);
	}
}

QList<QAction *> ExtraMeshFilterPlugin::actions() const {
	return actionList;
}


const ActionInfo &ExtraMeshFilterPlugin::Info(QAction *action) 
{
	static ActionInfo ai; 
  
	if( action->text() == ST(FP_LOOP_SS) )
		{
			ai.Help = tr("Apply Loop's Subdivision Surface algorithm. It is an approximate method which subdivide each triangle in four faces. It works for every triangle and has rules for extraordinary vertices");
			ai.ShortHelp = tr("Apply Loop's Subdivision Surface algorithm");
		}
	if( action->text() == ST(FP_BUTTERFLY_SS) ) 
	  {
			ai.Help = tr("Apply Butterfly Subdivision Surface algorithm. It is an interpolated method, defined on arbitrary triangular meshes. The scheme is known to be C1 but not C2 on regular meshes");
			ai.ShortHelp = tr("Apply Butterfly Subdivision Surface algorithm");
		}
	if( action->text() == ST(FP_REMOVE_UNREFERENCED_VERTEX) )
		{
			ai.Help = tr("Check for every vertex on the mesh if it is referenced by a face and removes it");
			ai.ShortHelp = tr("Remove Unreferenced Vertexes");
			
		}
  if( action->text() == ST(FP_REMOVE_DUPLICATED_VERTEX) )
		{
			ai.Help = tr("Check for every vertex on the mesh if there are two vertices with same coordinates and removes it");
			ai.ShortHelp = tr("Remove Duplicated Vertexes");
		}
	if(action->text() == ST(FP_REMOVE_NULL_FACES) )
		{
			ai.Help = tr("Removes faces with area equal to zero");
			ai.ShortHelp = tr("Remove Null Faces");
			
		}
	if(action->text() == ST(FP_LAPLACIAN_SMOOTH) )
		{
			ai.Help = tr("For each vertex it calculates the average position with nearest vertex");
			ai.ShortHelp = tr("Smooth the mesh surface");
	
		}
		
 	if(action->text() == ST(FP_DECIMATOR) )
 		{
			ai.Help = tr("Collapse vertices by creating a three dimensional grid enveloping the mesh and discretizes them based on the cells of this grid");
			ai.ShortHelp = tr("Simplify the surface eliminating triangle");
			
 		}
	
	if(action->text() == ST(FP_MIDPOINT) )
 		{
			ai.Help = tr("Splits every edge in two");
			ai.ShortHelp = tr("Apply Midpoint's Subdivision Surface algorithm");
			
 		}
	
	if(action->text() == ST(FP_REORIENT) )
 		{
			ai.Help = tr("Re-oriented the adjacencies of the face of the mesh");
			ai.ShortHelp = tr("Re-oriented the face");
			
 		}
	
	if(action->text() == ST(FP_INVERT_FACES) )
 		{
			ai.Help = tr("Invert faces orentation, flip the normal of the mesh");
			ai.ShortHelp = tr("Invert faces orentation");
 		}
	if(action->text() == ST(FP_TRANSFORM) )
 		{
			ai.Help = tr("Apply transformation, you can rotate, translate or scale the mesh");
			ai.ShortHelp = tr("Apply Transform");
 		}
		if(action->text() == ST(FP_DETACHER) )
 		{
			ai.Help = tr("remove from the mesh all triangles whose have an edge with lenght greater or equal than a trashold");
			ai.ShortHelp = tr("Remove triangle with edge greater than a trashold");
 		}
	
	//	 ai.Help=tr("Generic Help for an action");
   return ai;
}

 const PluginInfo &ExtraMeshFilterPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr("__DATE__");
	 ai.Version = tr("0.5");
	 ai.Author = ("Paolo Cignoni, Mario Latronico, Andrea Venturi");
   return ai;
 }
 

bool ExtraMeshFilterPlugin::applyFilter(QAction *filter, MeshModel &m, QWidget *parent, vcg::CallBackPos *cb) 
{
	double threshold = 0.0;
	bool selected = false;

	if( filter->text().contains(tr("Subdivision Surface")) ) {

	  vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	  // IsTwoManifoldFace needs a FaceFace update topology
	  if ( ! vcg::tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
	    QMessageBox::warning(parent, // parent
				 QString("Can't continue"), // caption
				 QString("Mesh faces not 2 manifold")); // text
	    return false; // can't continue, mesh can't be processed
	  }
	  if(!m.cm.face.IsWedgeTexEnabled()) m.cm.face.EnableWedgeTex();
	  vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	  vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
	  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
	  int continueValue = refineDialog->exec();
	  if (continueValue == QDialog::Rejected)
	    return false; // don't continue, user pressed Cancel
	  double threshold = refineDialog->getThreshold(); // threshold for refinying
	  bool selected = refineDialog->isSelected(); // refine only selected faces
	}
	
	if(filter->text() == ST(FP_LOOP_SS) )
	  {
	    vcg::RefineOddEvenE<CMeshO, vcg::OddPointLoop<CMeshO>, vcg::EvenPointLoop<CMeshO> >
	      (m.cm, OddPointLoop<CMeshO>(), EvenPointLoop<CMeshO>(),threshold, selected, cb);
	    
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);																																			 
	  }
	
	if(filter->text() == ST(FP_BUTTERFLY_SS) )
	  {
	    vcg::Refine<CMeshO,MidPointButterfly<CMeshO> >
	      (m.cm,vcg::MidPointButterfly<CMeshO>(),threshold, selected, cb);
	    
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }
	
	if(filter->text() == ST(FP_MIDPOINT) )
	  {
	    vcg::Refine<CMeshO,MidPoint<CMeshO> >
	      (m.cm,vcg::MidPoint<CMeshO>(),threshold, selected, cb);
	    
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }
	
	if(filter->text() == ST(FP_REMOVE_UNREFERENCED_VERTEX) )
	  {
	    int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	    if (log)
	      log->Log(GLLogStream::Info, "Removed %d unreferenced vertices",delvert);
	    if (delvert != 0)
	      vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	    //QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
	  }
	
	if(filter->text() == ST(FP_REMOVE_DUPLICATED_VERTEX) )
	  {
	    int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
	    if (log)
	      log->Log(GLLogStream::Info, "Removed %d duplicated vertices", delvert);
	    if (delvert != 0)
	      vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }
	
	if(filter->text() == ST(FP_REMOVE_NULL_FACES) ) 
	  {
	    int nullFaces=tri::Clean<CMeshO>::RemoveZeroAreaFace(m.cm);
	    if (log)
	      log->Log(GLLogStream::Info, "Removed %d null faces", nullFaces);
	    if (nullFaces != 0)
	      vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }
	
	if(filter->text() == ST(FP_REORIENT) ) 
	  {
	    bool oriented;
	    bool orientable;
	    vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	    tri::Clean<CMeshO>::IsOrientedMesh(m.cm, oriented,orientable);
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	    
	  }
	
	if(filter->text() == ST(FP_LAPLACIAN_SMOOTH)) 
	  {
	    LaplacianSmooth(m.cm,1);
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }
	
	
 	if(filter->text() == ST(FP_DECIMATOR)) 
	  {
	    int continueValue = decimatorDialog->exec();
	    if (continueValue == QDialog::Rejected)
	      return false; // don't continue, user pressed Cancel
	    int step = decimatorDialog->getStep();
	    vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	    int delvert = Decimator<CMeshO>(m.cm,step);
	    if (log)
	      log->Log(GLLogStream::Info, "Removed %d vertices", delvert);
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if (filter->text() == ST(FP_INVERT_FACES) ) {
	  
	  InvertFaces<CMeshO>(m.cm);
	  // update normal on InvertFaces function
	}

	if (filter->text() == ST(FP_TRANSFORM) ) {
	
		// if (!transformDialog)
// 			transformDialog = new TransformDialog(/*&m.cm*/);

// 		int continueValue = transformDialog->exec(&m.cm);
// 		if (continueValue == QDialog::Rejected)
// 			return false;
// 		Matrix44f matrix = transformDialog->getTransformation();
// 		if (log) {
// 			log->Log(GLLogStream::Info, 
// 							 transformDialog->getLog().toAscii().data());
// 		}
// 		vcg::tri::UpdatePosition<CMeshO>::Matrix(m.cm, matrix);
		// TODO : need update normal
	}

	if (filter->text() == ST(FP_DETACHER) ) {
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);

		int continueValue = detacherDialog->exec();
		if (continueValue == QDialog::Rejected)
			return false; // don't continue, user pressed Cancel
		double threshold = detacherDialog->getThreshold(); // threshold for refinying


		Detacher<CMeshO>(m.cm, threshold);
	}

	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
