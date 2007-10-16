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
Revision 1.15  2007/10/16 20:26:49  cignoni
changed in order to the new allocator based deleting strategy

Revision 1.14  2007/10/09 11:54:07  cignoni
added delete face and vert

Revision 1.13  2007/10/02 08:13:48  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.12  2007/04/16 10:16:25  cignoni
Added missing info on filtering actions

Revision 1.11  2007/04/16 09:25:30  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing again...

Revision 1.10  2007/03/20 16:23:10  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.9  2007/02/08 23:46:18  pirosu
merged srcpar and par in the GetStdParameters() function

Revision 1.8  2007/02/08 15:59:46  cignoni
Added Border selection filters

Revision 1.7  2006/11/29 00:59:20  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.6  2006/11/27 06:57:21  cignoni
Wrong way of using the __DATE__ preprocessor symbol

Revision 1.5  2006/11/07 09:22:32  cignoni
Wrote correct Help strings, and added required cleardatamask

Revision 1.4  2006/10/16 08:57:16  cignoni
Rewrote for using the new update/selection helper class. Added Selection dilate and erode

Revision 1.3  2006/06/18 21:26:30  cignoni
delete selected face icon

Revision 1.2  2006/06/12 15:18:56  cignoni
Shortcut to delete selected face

Revision 1.1  2006/05/25 04:57:46  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.


****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include "meshselect.h"
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/allocate.h>

using namespace vcg;

const QString SelectionFilterPlugin::filterName(FilterIDType filter) 
{
 switch(filter)
  {
	  case FP_SELECT_ALL :		             return QString("Select All");
	  case FP_SELECT_NONE :		             return QString("Select None");
	  case FP_SELECT_INVERT :		           return QString("Invert Selection");
	  case FP_SELECT_DELETE_FACE :		     return QString("Delete Selected Faces");
	  case FP_SELECT_DELETE_FACEVERT :		 return QString("Delete Selected Faces and Vertices");
	  case FP_SELECT_ERODE :		           return QString("Erode Selection");
	  case FP_SELECT_DILATE :		           return QString("Dilate Selection");
	  case FP_SELECT_BORDER_FACES:		     return QString("Select Border Faces");
  }
  return QString("Unknown filter");
}

SelectionFilterPlugin::SelectionFilterPlugin() 
{
  typeList << 
    FP_SELECT_ALL <<
    FP_SELECT_NONE <<
    FP_SELECT_DELETE_FACE <<
    FP_SELECT_DELETE_FACEVERT <<
    FP_SELECT_ERODE <<
    FP_SELECT_DILATE <<
    FP_SELECT_BORDER_FACES <<
    FP_SELECT_INVERT;
  
  FilterIDType tt;
  
  foreach(tt , types())
    {
      actionList << new QAction(filterName(tt), this);
      if(tt==FP_SELECT_DELETE_FACE){
            actionList.last()->setShortcut(QKeySequence (Qt::Key_Delete));
            actionList.last()->setIcon(QIcon(":/images/delete_face.png"));
      }
      if(tt==FP_SELECT_DELETE_FACEVERT){
            actionList.last()->setShortcut(QKeySequence ("Shift+Del"));
            actionList.last()->setIcon(QIcon(":/images/delete_facevert.png"));
      }
    }	  
}
SelectionFilterPlugin::~SelectionFilterPlugin() 
{
	for (int i = 0; i < actionList.count() ; i++ ) {
		delete actionList.at(i);
	}
}


bool SelectionFilterPlugin::applyFilter(QAction *action, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	CMeshO::FaceIterator fi;
	CMeshO::VertexIterator vi;
	switch(ID(action))
  {
  case FP_SELECT_DELETE_FACE : 
		for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD() && (*fi).IsS() ) tri::Allocator<CMeshO>::DeleteFace(m.cm,*fi);
			m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
    break;
  case FP_SELECT_DELETE_FACEVERT : 
		tri::UpdateSelection<CMeshO>::ClearVertex(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);  
    for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD() && (*fi).IsS() )
					tri::Allocator<CMeshO>::DeleteFace(m.cm,*fi);
		for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
			if(!(*vi).IsD() && (*vi).IsS() )
					tri::Allocator<CMeshO>::DeleteVertex(m.cm,*vi);
			m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
    break;
  case FP_SELECT_ALL    : tri::UpdateSelection<CMeshO>::AllFace(m.cm);     break;
  case FP_SELECT_NONE   : tri::UpdateSelection<CMeshO>::ClearFace(m.cm);   break;
  case FP_SELECT_INVERT : tri::UpdateSelection<CMeshO>::InvertFace(m.cm);  break;
  case FP_SELECT_ERODE  : tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);  
                          tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm); 
  break;
  case FP_SELECT_DILATE : tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);  
                          tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm); 
  break;
  case FP_SELECT_BORDER_FACES: tri::UpdateSelection<CMeshO>::FaceFromBorder(m.cm);  
  break;
  

  default:  assert(0);
  }
  return true;
}

 const QString SelectionFilterPlugin::filterInfo(FilterIDType filterId) 
 {
  switch(filterId)
  {
    case FP_SELECT_DILATE : return tr("Dilate (expand) the current set of selected faces");  
    case FP_SELECT_DELETE_FACE : return tr("Delete the current set of selected faces, vertices that remains unreferenced are not deleted.");  
    case FP_SELECT_DELETE_FACEVERT : return tr("Delete the current set of selected faces and all the vertices surrounded by that faces.");  
    case FP_SELECT_ERODE  : return tr("Erode (reduce) the current set of selected faces");  
    case FP_SELECT_INVERT : return tr("Invert the current set of selected faces");  
    case FP_SELECT_NONE   : return tr("Clear the current set of selected faces");  
    case FP_SELECT_ALL    : return tr("Select all the faces of the current mesh");  
    case FP_SELECT_BORDER_FACES    : return tr("Select all the faces on the boundary");  
  }  
  assert(0);
  return QString();
 }

 const PluginInfo &SelectionFilterPlugin::pluginInfo() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.5");
	 ai.Author = ("Paolo Cignoni");
   return ai;
 } 

const int SelectionFilterPlugin::getRequirements(QAction *action)
{
 switch(ID(action))
  {
   case FP_SELECT_BORDER_FACES:   return  MeshModel::MM_BORDERFLAG;
  }
}

Q_EXPORT_PLUGIN(SelectionFilterPlugin)
