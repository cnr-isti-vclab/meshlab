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
/****************************************************************************
  History
$Log$
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

Revision 1.15  2005/12/08 22:46:44  cignoni
Added Laplacian Smooth

Revision 1.14  2005/12/08 13:52:01  mariolatronico
added preliminary version of callback. Now it counts only even point on RefineOddEven

Revision 1.13  2005/12/05 14:51:03  mariolatronico
second action from "Loop" to "Butterfly"

Revision 1.12  2005/12/03 23:46:11  cignoni
Cleaned up a little and added a remove null faces filter

Revision 1.11  2005/12/03 22:50:06  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include "refine_loop.h"
#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/color.h>
/////////////
#include "../../test/decimator/decimator.h"
#include "../../meshlab/GLLogStream.h"
#include "../../meshlab/LogStream.h"
#include "../../meshlab/glarea.h"
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

	actionList << new QAction(ST(FP_DECIMATOR), this);


	refineDialog = new RefineDialog();
	refineDialog->hide();
	decimatorDialog = new DecimatorDialog();
	decimatorDialog->hide();
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


	default: assert(0);
  }
  return QString("error!");

}

ExtraMeshFilterPlugin::~ExtraMeshFilterPlugin() {
	delete refineDialog;
}

QList<QAction *> ExtraMeshFilterPlugin::actions() const {
	return actionList;
}


const ActionInfo &ExtraMeshFilterPlugin::Info(QAction *action) 
{
	ActionInfo ai; 
  
	if( action->text() == tr("Loop Subdivision Surface") )
		{
			ai.Help = tr("Apply Loop's Subdivision Surface algorithm, it is an approximate method");
			ai.ShortHelp = tr("Apply Loop's Subdivision Surface algorithm");
		}
	if( action->text() == tr("Butterfly Subdivision Surface") )
	  {
			ai.Help = tr("Apply Butterfly Subdivision Surface algorithm, it is an interpolated method");
			ai.ShortHelp = tr("Apply Butterfly Subdivision Surface algorithm");
		}
  if( action->text() == tr("Remove Unreferenced Vertexes"))
		{
			ai.Help = tr("Remove Unreferenced Vertexes");
			ai.ShortHelp = tr("Remove Unreferenced Vertexes");
			
		}
  if( action->text() == tr("Remove Duplicated Vertexes"))
		{
			ai.Help = tr("Remove Duplicated Vertexes");
			ai.ShortHelp = tr("Remove Duplicated Vertexes");
		}
	if(action->text() == tr("Remove Null Faces"))
		{
			ai.Help = tr("Remove Null Faces");
			ai.ShortHelp = tr("Remove Null Faces");
			
		}
	if(action->text() == tr("Laplacian Smooth"))
		{
			ai.Help = tr("Laplacian Smooth: Smooth the mesh surface");
			ai.ShortHelp = tr("Smooth the mesh surface");
	
		}
		
 	if(action->text() == tr("Decimator"))
 		{
			ai.Help = tr("Decimator tries to elminate triangles by clustering method by Rossignac");
			ai.ShortHelp = tr("Simplify the surface eliminating triangle");
			
 		}

		 	if(action->text() == tr("Midpoint Subdivision Surfaces"))
 		{
			ai.Help = tr("Apply Midpoint's Subdivision Surface algorithm");
			ai.ShortHelp = tr("Apply Midpoint's Subdivision Surface algorithm");
			
 		}

		 	if(action->text() == tr("Re-oriented"))
 		{
			ai.Help = tr("Re-oriented the  adjacencies of the face of the mesh");
			ai.ShortHelp = tr("Re-oriented the face");
			
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
			
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}

  if(filter->text() == ST(FP_REMOVE_DUPLICATED_VERTEX) )
		{
		  int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
      GLArea *glArea = dynamic_cast<GLArea*>(parent);
			if (glArea) {
				glArea->log.Log(GLLogStream::Info, "Removed %d vertices", delvert);
			}
			//cb(100,tr("Removed vertices : %1.").arg(delvert).toLocal8Bit());
			
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}

	if(filter->text() == ST(FP_REMOVE_NULL_FACES) ) 
		{
			int delvert=tri::Clean<CMeshO>::RemoveZeroAreaFace(m.cm);
      GLArea *glArea = dynamic_cast<GLArea*> (parent);
			if (glArea) {
				glArea->log.Log(GLLogStream::Info, "Removed %d null faces", delvert);
			}
			//			cb(100,tr("Removed null faces : %1.").arg(delvert).toLocal8Bit());
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}

	if(filter->text() == ST(FP_REORIENT) ) 
		{
			bool or = true;
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
			tri::Clean<CMeshO>::IsOrientedMesh(m.cm, or,or);
		}

	if(filter->text() == ST(FP_LAPLACIAN_SMOOTH)) 
		{
			LaplacianSmooth(m.cm,1);
		}
	
 	if(filter->text() == ST(FP_DECIMATOR)) 
 		{
			int continueValue = decimatorDialog->exec();
			if (continueValue == QDialog::Rejected)
				return false; // don't continue, user pressed Cancel
			int step = decimatorDialog->getStep();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
 			int delvert = Decimator<CMeshO>(m.cm,step);
      GLArea *glArea = dynamic_cast<GLArea*> ( parent );
			if (glArea) {
				glArea->log.Log(GLLogStream::Info, "Decimated %d vertices", delvert);
			}

		}
	
	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
