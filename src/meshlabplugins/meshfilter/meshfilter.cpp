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
/////////////
#include "../../test/decimator/decimator.h"
////////////
using namespace vcg;






ExtraMeshFilterPlugin::ExtraMeshFilterPlugin() {
	actionList << new QAction("Loop Subdivision Surface", this);
	actionList << new QAction("Butterfly Subdivision Surface", this);
	actionList << new QAction("Remove Unreferenced Vertexes", this);
	actionList << new QAction("Remove Duplicated Vertexes", this);
	actionList << new QAction("Remove Null Faces", this);
	actionList << new QAction("Laplacian Smooth", this);
	/////////////
	actionList << new QAction("Decimator", this);
	////////////
	refineDialog = new RefineDialog();
	refineDialog->hide();
	decimatorDialog = new DecimatorDialog();
	decimatorDialog->hide();
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
//	 ai.Help=tr("Generic Help for an action");
   return ai;
 }

 const PluginInfo &ExtraMeshFilterPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr("__DATE__");
	 ai.Version = tr("0.4");
	 ai.Author = ("Paolo Cignoni, Mario Latronico, Andrea Venturi");
   return ai;
 }
 

bool ExtraMeshFilterPlugin::applyFilter(QAction *filter, MeshModel &m, QWidget *parent, vcg::CallBackPos *cb) 
{
	if(filter->text() == tr("Loop Subdivision Surface") )
		{
			//vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
			//if(!m.cm.face.IsFFAdjacencyEnabled()) m.cm.face.EnableFFAdjacency();
			if(!m.cm.face.IsWedgeTexEnabled()) m.cm.face.EnableWedgeTex();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
			vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
			int continueValue = refineDialog->exec();
			if (continueValue == QDialog::Rejected)
				return false; // don't continue, user pressed Cancel
			
			double threshold = refineDialog->getThreshold();
			bool selected = refineDialog->isSelected();
			// TODO : length 0 by default, need a dialog ?
			vcg::RefineOddEvenE<CMeshO, vcg::OddPointLoop<CMeshO>, vcg::EvenPointLoop<CMeshO> >
				(m.cm, OddPointLoop<CMeshO>(), EvenPointLoop<CMeshO>(),threshold, selected, cb);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);																																			 
		}
	if(filter->text() == tr("Butterfly Subdivision Surface") )
	  {
			//vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
			//if(!m.cm.face.IsFFAdjacencyEnabled()) m.cm.face.EnableFFAdjacency();
			if(!m.cm.face.IsWedgeTexEnabled()) m.cm.face.EnableWedgeTex();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
			vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
			
			int continueValue = refineDialog->exec();
			if (continueValue == QDialog::Rejected)
				return false; // don't continue, user pressed Cancel
			double threshold = refineDialog->getThreshold();
			bool selected = refineDialog->isSelected();
			vcg::Refine<CMeshO,MidPointButterfly<CMeshO> >(m.cm,vcg::MidPointButterfly<CMeshO>(),threshold, selected, cb);
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		}
  if(filter->text() == tr("Remove Unreferenced Vertexes"))
		{
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}
  if(filter->text() == tr("Remove Duplicated Vertexes"))
		{
		  int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
      cb(100,tr("Removed vertices : %1.").arg(delvert).toLocal8Bit());
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}
	if(filter->text() == tr("Remove Null Faces"))
		{
			int delvert=tri::Clean<CMeshO>::RemoveZeroAreaFace(m.cm);
			cb(100,tr("Removed null faces : %1.").arg(delvert).toLocal8Bit());
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}
	if(filter->text() == tr("Laplacian Smooth"))
		{
			LaplacianSmooth(m.cm,1);
			cb(100,tr("smoothed mesh").toLocal8Bit());
			//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
		}
	
 	if(filter->text() == tr("Decimator"))
 		{
			int continueValue = decimatorDialog->exec();
			if (continueValue == QDialog::Rejected)
				return false; // don't continue, user pressed Cancel
			int step = decimatorDialog->getStep();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
 			Decimator<CMeshO>(m.cm,step);
 		}
	
	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
