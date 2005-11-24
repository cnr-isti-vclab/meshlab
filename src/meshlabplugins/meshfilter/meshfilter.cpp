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
//#include <vcg/complex/trimesh/clean.h>

using namespace vcg;

//class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, vert::Coord3f, vert::Normal3f, vert::BitFlags >{};
//class CFaceO    : public FaceSimp2< CVertexO, CEdge, CFaceO, face::InfoOcf, face::FFAdjOcf, face::WedgeTexture2f, face::VertexRef, face::BitFlags, face::Normal3fOcf > {};
//class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};


QStringList ExtraMeshFilterPlugin::filters() const
{ 
	QStringList filterList;
	//filterList << tr("Loop Subdivision Surface");
	filterList << tr("Butterfly Subdivision Surface");
	//	filterList << tr("Remove Unreferenced Vertexes");
	return filterList;
}

bool ExtraMeshFilterPlugin::applyFilter(const QString &filter, MeshModel &m, QWidget *parent) 
{
	//	if(filter == tr("Loop Subdivision Surface") )
	//{
		//  int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	  //QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
	//}
	if(filter == tr("Butterfly Subdivision Surface") )
	{
		vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(m.cm);
		
		vcg::Refine<CMeshO, MidPointButterfly<CMeshO> >(m.cm,vcg::MidPointButterfly<CMeshO>(),0);
		
		//  int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	  //QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
	}
	// 	if(filter == tr("Remove Unreferenced Vertexes"))
// 	{
		//  int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	  //QMessageBox::information(parent, tr("Filter Plugins"), tr("Removed vertices : %1.").arg(delvert));
// 	}
  
	return true;
}

Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
