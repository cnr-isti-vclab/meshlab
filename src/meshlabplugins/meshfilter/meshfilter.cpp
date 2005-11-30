/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
// TODO : test directory, need to be moved ...
#include <../../test/loop/new_refine.h>
#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>

using namespace vcg;

//class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, vert::Coord3f, vert::Normal3f, vert::BitFlags >{};
//class CFaceO    : public FaceSimp2< CVertexO, CEdge, CFaceO, face::InfoOcf, face::FFAdjOcf, face::WedgeTexture2f, face::VertexRef, face::BitFlags, face::Normal3fOcf > {};
//class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};


QStringList ExtraMeshFilterPlugin::filters() const
{ 
	QStringList filterList;
	filterList << tr("Loop Subdivision Surface");
	filterList << tr("Butterfly Subdivision Surface");
	filterList << tr("Remove Unreferenced Vertexes");
	filterList << tr("Remove Duplicated Vertexes");
	return filterList;
}

bool ExtraMeshFilterPlugin::applyFilter(const QString &filter, MeshModel &m, QWidget *parent, vcg::CallBackPos *cb) 
{
	if(filter == tr("Loop Subdivision Surface") )
	{
		//vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
    //if(!m.cm.face.IsFFAdjacencyEnabled()) m.cm.face.EnableFFAdjacency();
    if(!m.cm.face.IsWedgeTexEnabled()) m.cm.face.EnableWedgeTex();
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
		// TODO : length 0 by default, need a dialog ?
		vcg::RefineOddEvenE<CMeshO, vcg::OddPointLoop<CMeshO>, vcg::EvenPointLoop<CMeshO> >
			(m.cm, OddPointLoop<CMeshO>(), EvenPointLoop<CMeshO>(),0.0f);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);																																			 
	}
	if(filter == tr("Butterfly Subdivision Surface") )
	{
		//vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
    //if(!m.cm.face.IsFFAdjacencyEnabled()) m.cm.face.EnableFFAdjacency();
    if(!m.cm.face.IsWedgeTexEnabled()) m.cm.face.EnableWedgeTex();
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
		
		vcg::Refine<CMeshO, MidPointButterfly<CMeshO> >(m.cm,vcg::MidPointButterfly<CMeshO>(),0);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		
		//  int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	  //QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
	}
  if(filter == tr("Remove Unreferenced Vertexes"))
 	{
  int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
 	}
  if(filter == tr("Remove Duplicated Vertexes"))
 	{
  int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
	//QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
 	}
  
	return true;
}

Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
