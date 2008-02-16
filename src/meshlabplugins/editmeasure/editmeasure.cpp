/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2008                                                \/)\/    *
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
$Log: editmeasure.cpp,v $
****************************************************************************/
#include <QtGui>

#include <meshlab/glarea.h>
#include "editmeasure.h"

using namespace vcg;

EditMeasurePlugin::EditMeasurePlugin()
 :rubberband(Color4b(255,170,85,255)),was_ready(false)
{
  actionList << new QAction(QIcon(":/images/icon_measure.png"),"Measuring Tool", this);
  foreach(QAction *editAction, actionList)
    editAction->setCheckable(true);
}

QList<QAction *> EditMeasurePlugin::actions() const {
  return actionList;
}

const QString EditMeasurePlugin::Info(QAction *action) 
{
  if( action->text() != tr("Measuring Tool") ) assert (0);
  return tr("Allow to measure distances between points of a model");
}

const PluginInfo &EditMeasurePlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
   ai.Version = tr("1.1");
   ai.Author = ("Paolo Cignoni, Luca Benedetti");
   return ai;
}

void EditMeasurePlugin::mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * gla)
{
  if(was_ready||rubberband.IsReady()){
    rubberband.Reset();
    was_ready=false;
  }
  gla->update();
}

void EditMeasurePlugin::mouseMoveEvent  (QAction *,QMouseEvent * event, MeshModel &, GLArea * gla)
{
  rubberband.Drag(event->pos());
  gla->update();
}

void EditMeasurePlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &, GLArea * gla)
{
  rubberband.Pin(event->pos());
  gla->update();
}

void EditMeasurePlugin::Decorate(QAction *, MeshModel &, GLArea * gla)
{
  rubberband.Render(gla);
  if(rubberband.IsReady()){
    Point3f a,b;
    rubberband.GetPoints(a,b);
    rubberband.RenderLabel(QString("%1").arg(Distance(a,b)),gla);
    if(!was_ready)
      suspendEditToggle();
    was_ready=true;
  }
  assert(!glGetError());
}

void EditMeasurePlugin::StartEdit(QAction *, MeshModel &, GLArea *gla )
{
  gla->setCursor(QCursor(QPixmap(":/images/cur_measure.png"),15,15));	
  connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );
  was_ready = false;
  rubberband.Reset();
}

void EditMeasurePlugin::EndEdit(QAction *, MeshModel &, GLArea *)
{
  was_ready = false;
  rubberband.Reset();
}

Q_EXPORT_PLUGIN(EditMeasurePlugin)
