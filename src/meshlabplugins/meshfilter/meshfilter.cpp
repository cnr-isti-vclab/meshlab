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
// TODO : test directory, need to be moved ...
#include "refine_loop.h"
#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>

using namespace vcg;

//class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, vert::Coord3f, vert::Normal3f, vert::BitFlags >{};
//class CFaceO    : public FaceSimp2< CVertexO, CEdge, CFaceO, face::InfoOcf, face::FFAdjOcf, face::WedgeTexture2f, face::VertexRef, face::BitFlags, face::Normal3fOcf > {};
//class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};


ExtraMeshFilterPlugin::ExtraMeshFilterPlugin() {
	actionList << new QAction("Loop Subdivision Surface", this);
	actionList << new QAction("Butterfly Subdivision Surface", this);
	actionList << new QAction("Remove Unreferenced Vertexes", this);
	actionList << new QAction("Remove Duplicated Vertexes", this);
	actionList << new QAction("Remove Null Faces", this);
}

QList<QAction *> ExtraMeshFilterPlugin::actions() const {
	return actionList;

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
		// TODO : length 0 by default, need a dialog ?
		vcg::RefineOddEvenE<CMeshO, vcg::OddPointLoop<CMeshO>, vcg::EvenPointLoop<CMeshO> >
			(m.cm, OddPointLoop<CMeshO>(), EvenPointLoop<CMeshO>(),0.0f, false, cb);
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
		
		vcg::Refine<CMeshO, MidPointButterfly<CMeshO> >(m.cm,vcg::MidPointButterfly<CMeshO>(),0);
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

	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
