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
Revision 1.64  2006/06/19 05:29:12  cignoni
changed include order for disambiguating gcc specialization template rule

Revision 1.63  2006/06/07 10:25:28  cignoni
init of bbox diag for generic lenght dialog

Revision 1.62  2006/05/25 06:24:14  cignoni
Decimator dialog no more necessary

Revision 1.61  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.60  2006/04/18 06:57:35  zifnab1974
syntax errors for gcc 3.4.5 resolved

Revision 1.59  2006/04/12 15:12:18  cignoni
Added Filter classes (cleaning, meshing etc)

Revision 1.58  2006/02/20 20:52:36  giec
replace refine and detacher dialog whit GnericELDialog

Revision 1.57  2006/02/15 22:13:01  giec
Modify the Detucher function call

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
#include "meshfilter.h"
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/clustering.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include "invert_faces.h"
#include "refine_loop.h"
//#include "decimator.h"

#include "../../meshlab/GLLogStream.h"
#include "../../meshlab/LogStream.h"

using namespace vcg;

ExtraMeshFilterPlugin::ExtraMeshFilterPlugin() 
{
  typeList << FP_LOOP_SS<< 
    FP_BUTTERFLY_SS<< 
    FP_REMOVE_UNREFERENCED_VERTEX<<
    FP_REMOVE_DUPLICATED_VERTEX<< 
    FP_REMOVE_FACES_BY_AREA<<
    FP_REMOVE_FACES_BY_EDGE<<
    FP_LAPLACIAN_SMOOTH<< 
    FP_DECIMATOR<< 
    FP_MIDPOINT<< 
    FP_REORIENT <<
    FP_INVERT_FACES<<
    FP_TRANSFORM;
  
  FilterType tt;
  
  foreach(tt , types())
	    actionList << new QAction(ST(tt), this);

	//refineDialog = new RefineDialog();
	//refineDialog->hide();
//	decimatorDialog = new DecimatorDialog();
//	decimatorDialog->hide();
	
	genericELD = new GenericELDialog();
	genericELD->hide();
	
	transformDialog = new TransformDialog();
	transformDialog->hide();
}

const ExtraMeshFilterPlugin::FilterClass ExtraMeshFilterPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_REMOVE_UNREFERENCED_VERTEX :
    case FP_REMOVE_DUPLICATED_VERTEX :
    case FP_REMOVE_FACES_BY_AREA:
    case FP_REMOVE_FACES_BY_EDGE :
         return MeshFilterInterface::Cleaning; 
    case FP_BUTTERFLY_SS :
    case FP_LOOP_SS :
    case FP_MIDPOINT :
         return MeshFilterInterface::Remeshing; 

    default : return MeshFilterInterface::Generic;
  }
}


const QString ExtraMeshFilterPlugin::ST(FilterType filter) 
{
 switch(filter)
  {
	case FP_LOOP_SS :		                  return QString("Loop Subdivision Surfaces");
	case FP_BUTTERFLY_SS :								return QString("Butterfly Subdivision Surfaces");
	case FP_REMOVE_UNREFERENCED_VERTEX :	return QString("Remove Unreferenced Vertex");
	case FP_REMOVE_DUPLICATED_VERTEX :		return QString("Remove Duplicated Vertex");
	case FP_REMOVE_FACES_BY_AREA :     		return QString("Remove Zero Area Faces");
	case FP_REMOVE_FACES_BY_EDGE :				return QString("Remove Faces with edges longer than...");
  case FP_LAPLACIAN_SMOOTH :						return QString("Laplacian Smooth");
	case FP_DECIMATOR :	                	return QString("Clustering decimation");
	case FP_MIDPOINT :										return QString("Midpoint Subdivision Surfaces");
	case FP_REORIENT :	                  return QString("Re-orient");
	case FP_INVERT_FACES:									return QString("Invert Faces");
	case FP_TRANSFORM:	                	return QString("Apply Transform");
	
	default: assert(0);
  }
  return QString("error!");

}

ExtraMeshFilterPlugin::~ExtraMeshFilterPlugin() {
//	delete refineDialog;
	delete genericELD;
//	delete decimatorDialog;
	if (transformDialog)
		delete transformDialog;
	for (int i = 0; i < actionList.count() ; i++ ) {
		delete actionList.at(i);
	}
}

const ActionInfo &ExtraMeshFilterPlugin::Info(QAction *action)
{
	static ActionInfo ai;
  switch(ID(action))
  {
  case FP_LOOP_SS :
      ai.Help      = tr("Apply Loop's Subdivision Surface algorithm. It is an approximate method which subdivide each triangle in four faces. It works for every triangle and has rules for extraordinary vertices");
      ai.ShortHelp = tr("Apply Loop's Subdivision Surface algorithm");
		     break;
  case FP_BUTTERFLY_SS : 
 			ai.Help = tr("Apply Butterfly Subdivision Surface algorithm. It is an interpolated method, defined on arbitrary triangular meshes. The scheme is known to be C1 but not C2 on regular meshes");
			ai.ShortHelp = tr("Apply Butterfly Subdivision Surface algorithm");
		     break;
  case FP_MIDPOINT : 
			ai.Help = tr("Splits every edge in two");
			ai.ShortHelp = tr("Apply Midpoint's Subdivision Surface algorithm");
		     break;
  case FP_REMOVE_UNREFERENCED_VERTEX : 
			ai.Help = tr("Check for every vertex on the mesh if it is referenced by a face and removes it");
			ai.ShortHelp = tr("Remove Unreferenced Vertexes");
		     break;
  case FP_REMOVE_DUPLICATED_VERTEX : 
			ai.Help = tr("Check for every vertex on the mesh if there are two vertices with same coordinates and removes it");
			ai.ShortHelp = tr("Remove Duplicated Vertexes");
		     break;
  case FP_REMOVE_FACES_BY_AREA : 
			ai.Help = tr("Removes faces with area equal to zero");
			ai.ShortHelp = tr("Remove Null Faces");
		     break;
  case FP_REMOVE_FACES_BY_EDGE : 
			ai.Help = tr("Remove from the mesh all triangles whose have an edge with lenght greater or equal than a threshold");
			ai.ShortHelp = tr("Remove triangle with edge greater than a threshold");
		     break;
  case FP_LAPLACIAN_SMOOTH : 
			ai.Help = tr("For each vertex it calculates the average position with nearest vertex");
			ai.ShortHelp = tr("Smooth the mesh surface");
		     break;
  case FP_DECIMATOR : 
			ai.Help = tr("Collapse vertices by creating a three dimensional grid enveloping the mesh and discretizes them based on the cells of this grid");
			ai.ShortHelp = tr("Simplify the surface eliminating triangle");
		     break;
  case FP_REORIENT : 
			ai.Help = tr("Re-oriented the adjacencies of the face of the mesh");
			ai.ShortHelp = tr("Re-oriented the face");
		     break;
  case FP_INVERT_FACES : 
			ai.Help = tr("Invert faces orentation, flip the normal of the mesh");
			ai.ShortHelp = tr("Invert faces orentation");
		     break;
  case FP_TRANSFORM : 
			ai.Help = tr("Apply transformation, you can rotate, translate or scale the mesh");
			ai.ShortHelp = tr("Apply Transform");
		     break;
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

const int ExtraMeshFilterPlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_LOOP_SS :
    case FP_BUTTERFLY_SS : 
    case FP_MIDPOINT :            return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG;
    case FP_LAPLACIAN_SMOOTH:     return MeshModel::MM_BORDERFLAG;
    case FP_REORIENT:             return MeshModel::MM_FACETOPO;
    case FP_REMOVE_UNREFERENCED_VERTEX:
    case FP_REMOVE_DUPLICATED_VERTEX:
    case FP_REMOVE_FACES_BY_AREA:
    case FP_REMOVE_FACES_BY_EDGE:
    case FP_DECIMATOR:
    case FP_TRANSFORM:
    case FP_INVERT_FACES:         return 0;
    default: assert(0); 
  }
  return 0;
}

bool ExtraMeshFilterPlugin::getParameters(QAction *action, QWidget *, MeshModel &m,FilterParameter &par)
{
 par.clear();
 switch(ID(action))
  {
    case FP_LOOP_SS :
    case FP_BUTTERFLY_SS : 
    case FP_MIDPOINT : 
    case FP_REMOVE_FACES_BY_EDGE:
    case FP_DECIMATOR:
      {
        Histogram<float> histo;
        genericELD->setHistogram(&histo);
        genericELD->setDiagonale(m.cm.bbox.Diag());
        genericELD->setStartingPerc(1.0);
        int continueValue = genericELD->exec();

        //int continueValue = refineDialog->exec();
        if (continueValue == QDialog::Rejected)    return false; // don't continue, user pressed Cancel
        float threshold = genericELD->getThreshold(); // threshold for refinying
        // qDebug( "%f", threshold );
        bool selected = genericELD->getSelected(); // refine only selected faces
        par.addBool("Selected",selected);
        par.addFloat("Threshold",threshold);
        break;
      }
    case FP_TRANSFORM:
      {
        transformDialog->setMesh(&m.cm);
		    int continueValue = transformDialog->exec();
 		    if (continueValue == QDialog::Rejected)
 			    return false;
      	Matrix44f matrix = transformDialog->getTransformation();
        par.addMatrix44("Transform",matrix);
        break;
      } 		
    case FP_REMOVE_FACES_BY_AREA:
    case FP_REMOVE_UNREFERENCED_VERTEX:
    case FP_REMOVE_DUPLICATED_VERTEX:
    case FP_REORIENT:
    case FP_INVERT_FACES:
    case FP_LAPLACIAN_SMOOTH:
       return true; // no parameters
   default :assert(0);
  }
  return true;
}
	
bool ExtraMeshFilterPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos *cb)
{
	if( getClass(filter)==Remeshing)
  {
    if ( ! vcg::tri::Clean<CMeshO>::IsTwoManifoldFace(m.cm) ) {
      QMessageBox::warning(0, QString("Can't continue"), QString("Mesh faces not 2 manifold")); // text
      return false; // can't continue, mesh can't be processed
    }

    bool selected=par.getBool("Selected");	
    float threshold = par.getFloat("Threshold");		

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
  }
	if (ID(filter) == FP_REMOVE_FACES_BY_EDGE ) {
    bool selected  = par.getBool("Selected");	
    float threshold = par.getFloat("Threshold");		
	  if(selected) tri::Clean<CMeshO>::RemoveFaceOutOfRangeEdgeSel<true>(m.cm,0,threshold );
         else    tri::Clean<CMeshO>::RemoveFaceOutOfRangeEdgeSel<false>(m.cm,0,threshold );
	}

  if(filter->text() == ST(FP_REMOVE_FACES_BY_AREA) )
	  {
	    int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
	    if (log) log->Log(GLLogStream::Info, "Removed %d null faces", nullFaces);
	  }

  if(filter->text() == ST(FP_REMOVE_UNREFERENCED_VERTEX) )
	  {
	    int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
	    if (log) log->Log(GLLogStream::Info, "Removed %d unreferenced vertices",delvert);
	  }

	if(filter->text() == ST(FP_REMOVE_DUPLICATED_VERTEX) )
	  {
	    int delvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
	    if (log)
	      log->Log(GLLogStream::Info, "Removed %d duplicated vertices", delvert);
	    if (delvert != 0)
	      vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if(filter->text() == ST(FP_REORIENT) )
	  {
	    bool oriented;
	    bool orientable;
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
      bool selected  = par.getBool("Selected");	
      float threshold = par.getFloat("Threshold");		
      vcg::tri::Clustering<CMeshO, vcg::tri::AverageCell<CMeshO> > Grid;
      Grid.Init(m.cm.bbox,100000,threshold);
      Grid.Add(m.cm);
      Grid.Extract(m.cm);
	    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	  }

	if (filter->text() == ST(FP_INVERT_FACES) ) {
	  InvertFaces<CMeshO>(m.cm);
	}

	if (filter->text() == ST(FP_TRANSFORM) ) {
    Matrix44f matrix= par.getMatrix44("Transform");		

		if (log) {
 			log->Log(GLLogStream::Info,
 							 transformDialog->getLog().toAscii().data());
 		}
 		vcg::tri::UpdatePosition<CMeshO>::Matrix(m.cm, matrix);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
	}


	return true;
}
Q_EXPORT_PLUGIN(ExtraMeshFilterPlugin)
