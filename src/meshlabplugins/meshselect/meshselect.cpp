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

using namespace vcg;

const QString SelectionFilterPlugin::ST(FilterType filter) 
{
 switch(filter)
  {
	  case FP_SELECT_ALL :		             return QString("Select All");
	  case FP_SELECT_NONE :		             return QString("Select None");
	  case FP_SELECT_INVERT :		           return QString("Invert Selection");
	  case FP_SELECT_DELETE :		           return QString("Delete Selected Faces");
	  case FP_SELECT_ERODE :		           return QString("Erode Selection");
	  case FP_SELECT_DILATE :		           return QString("Dilate Selection");
  }
  return QString("Unknown filter");
}

SelectionFilterPlugin::SelectionFilterPlugin() 
{
  typeList << 
    FP_SELECT_ALL <<
    FP_SELECT_NONE <<
    FP_SELECT_DELETE <<
    FP_SELECT_ERODE <<
    FP_SELECT_DILATE <<
    FP_SELECT_INVERT;
  
  FilterType tt;
  
  foreach(tt , types())
    {
      actionList << new QAction(ST(tt), this);
      if(tt==FP_SELECT_DELETE){
            actionList.last()->setShortcut(QKeySequence (Qt::Key_Delete));
            actionList.last()->setIcon(QIcon(":/images/delete_face.png"));
      }
    }	  
}
SelectionFilterPlugin::~SelectionFilterPlugin() 
{
	for (int i = 0; i < actionList.count() ; i++ ) {
		delete actionList.at(i);
	}
}


bool SelectionFilterPlugin::applyFilter(QAction *action, MeshModel &m, FilterParameter & par, vcg::CallBackPos * cb) 
{
 par.clear();
 CMeshO::FaceIterator fi;
 switch(ID(action))
  {
  case FP_SELECT_DELETE : 
    for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD() && (*fi).IsS() )
      {
        (*fi).SetD(); 
        --m.cm.fn;
      }
    break;
  case FP_SELECT_ALL    : tri::UpdateSelection<CMeshO>::AllFace(m.cm);     break;
  case FP_SELECT_NONE   : tri::UpdateSelection<CMeshO>::ClearFace(m.cm);   break;
  case FP_SELECT_INVERT : tri::UpdateSelection<CMeshO>::InvertFace(m.cm);  break;
  case FP_SELECT_ERODE  : tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);  
                          tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm); 
    break;
  case FP_SELECT_DILATE  : tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);  
                          tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm); 
  break;
  

  default:  assert(0);
  }
  return true;
}

 const ActionInfo &SelectionFilterPlugin::Info(QAction *action) 
 {
   static ActionInfo ai; 
  
	if( action->text() == tr("Loop Subdivision Surface") )
		{
			ai.Help = tr("Apply Loop's Subdivision Surface algorithm, it is an approximate method");
			ai.ShortHelp = tr("Apply Loop's Subdivision Surface algorithm");
		}
   return ai;
 }

 const PluginInfo &SelectionFilterPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr("__DATE__");
	 ai.Version = tr("0.5");
	 ai.Author = ("Paolo Cignoni");
   return ai;
 } 
 
Q_EXPORT_PLUGIN(SelectionFilterPlugin)
