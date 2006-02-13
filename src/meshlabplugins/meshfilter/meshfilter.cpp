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
$Log$
Revision 1.56  2006/02/13 16:18:51  cignoni
Used the treshold param...

Revision 1.55  2006/02/06 10:18:27  mariolatronico
reverted to old behaviour, removed QEdgeLength

Revision 1.54  2006/02/05 11:22:40  mariolatronico
changed spinbox to QEdgeLength widget

Revision 1.53  2006/02/03 13:46:47  mariolatronico
spell check, remuve -> remove

Revision 1.52  2006/01/31 14:42:27  mariolatronico
fake commit, removed only some history logs

Revision 1.51  2006/01/31 14:37:40  mariolatronico
trashold -> threshold

Revision 1.50  2006/01/30 21:26:53  giec
-changed the voice of the menu from Decimator in Cluster decimator

Revision 1.49  2006/01/26 16:49:50  giec
Bugfix the new signature for decimator function call

Revision 1.48  2006/01/25 21:06:24  giec
Implemented percentile for detucher's dialog

Revision 1.47  2006/01/23 21:47:58  giec
Update detucherdialog with the diagonal percentage spinbox.

Revision 1.46  2006/01/22 16:43:32  mariolatronico
added update bbox and normal after transform dialog

Revision 1.45  2006/01/22 14:11:04  mariolatronico
added scale to unit box, move obj center. Rotate around object and origin are not working actually.

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
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/math/histogram.h>
#include "invert_faces.h"
#include "decimator.h"
#include "../../meshlab/GLLogStream.h"
#include "../../meshlab/LogStream.h"

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
	actionList << new QAction(ST(FP_DETACHER), this);
	actionList << new QAction(ST(FP_DECIMATOR), this);
	actionList << new QAction(ST(FP_INVERT_FACES), this);
	actionList << new QAction(ST(FP_TRANSFORM), this);

	refineDialog = new RefineDialog();
	refineDialog->hide();
	decimatorDialog = new DecimatorDialog();
	decimatorDialog->hide();
	detacherDialog = new DetacherDialog();
	detacherDialog->hide();
	transformDialog = new TransformDialog();
	transformDialog->hide();
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
		return QString("Clustering decimation");
	case FP_MIDPOINT :
		return QString("Midpoint Subdivision Surfaces");
	case FP_REORIENT :
		return QString("Re-oriented");
	case FP_INVERT_FACES:
		return QString("Invert Faces");
	case FP_TRANSFORM:
		return QString("Apply Transform");
	case FP_DETACHER:
		return QString("Remove triangle above threshold");

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
			ai.Help = tr("Remove from the mesh all triangles whose have an edge with lenght greater or equal than a threshold");
			ai.ShortHelp = tr("Remove triangle with edge greater than a threshold");
 		}

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
    vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
//    refineDialog->setDiagonal( m.cm.bbox.Diag() );
	  int continueValue = refineDialog->exec();
	  if (continueValue == QDialog::Rejected)
	    return false; // don't continue, user pressed Cancel
	  threshold = refineDialog->getThreshold(); // threshold for refinying
    qDebug( "%f", threshold );
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
			float diagonale = m.cm.bbox.Diag();

			decimatorDialog->setBboxEdge(m.cm.bbox.min,m.cm.bbox.max);
			//decimatorDialog->setDiagonale(diagonale);

			int continueValue = decimatorDialog->exec();

			if (continueValue == QDialog::Rejected)
	      return false; // don't continue, user pressed Cancel
	    int Xstep = decimatorDialog->getXStep();
			int Ystep = decimatorDialog->getYStep();
			int Zstep = decimatorDialog->getZStep();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	    int delvert = Decimator<CMeshO>(m.cm,Xstep,Ystep,Zstep);
	    if (log)
	      log->Log(GLLogStream::Info, "Removed %d vertices", delvert);
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if (filter->text() == ST(FP_INVERT_FACES) ) {

	  InvertFaces<CMeshO>(m.cm);
	}

	if (filter->text() == ST(FP_TRANSFORM) ) {
		transformDialog->setMesh(&m.cm);
		int continueValue = transformDialog->exec();
 		if (continueValue == QDialog::Rejected)
 			return false;
 		Matrix44f matrix = transformDialog->getTransformation();
 		if (log) {
 			log->Log(GLLogStream::Info,
 							 transformDialog->getLog().toAscii().data());
 		}
 		vcg::tri::UpdatePosition<CMeshO>::Matrix(m.cm, matrix);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
	}

	if (filter->text() == ST(FP_DETACHER) ) {
		float diagonale = m.cm.bbox.Diag();

		Histogram<float> *histo= new Histogram<float>();

		histo->SetRange( 0, diagonale, 10000);


		CMeshO::FaceIterator fi;
		for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
		{
			if(!(*fi).IsD())
			{
				if( !(*fi).V(0)->IsS() && !(*fi).V(1)->IsS()  )
				{
					histo->Add(Distance<float>((*fi).V(0)->P(),(*fi).V(1)->P()));
					(*fi).V(0)->SetS();
					(*fi).V(1)->SetS();
				}
				if( !(*fi).V(1)->IsS() && !(*fi).V(2)->IsS())
				{
					histo->Add(Distance<float>((*fi).V(1)->P(),(*fi).V(2)->P()));
					(*fi).V(2)->SetS();
					(*fi).V(1)->SetS();
				}
				if( !(*fi).V(2)->IsS() && !(*fi).V(0)->IsS())
				{
					histo->Add(Distance<float>((*fi).V(2)->P(),(*fi).V(0)->P()));
					(*fi).V(0)->SetS();
					(*fi).V(2)->SetS();
				}
			}
		}

		CMeshO::VertexIterator vi;
		for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			(*vi).ClearS();

		detacherDialog->setHistogram(histo);
		detacherDialog->setDiagonale(diagonale);
		int continueValue = detacherDialog->exec();

		if (continueValue == QDialog::Rejected)
			return false; // don't continue, user pressed Cancel
		double threshold = detacherDialog->getThreshold(); // threshold for refinying

		Detacher<CMeshO>(m.cm, threshold);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	}

	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
