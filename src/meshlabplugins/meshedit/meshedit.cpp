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
Revision 1.2  2006/06/07 08:48:11  cignoni
Added selection modes: clean/Add (ctrl) / Sub (shift)

Revision 1.1  2006/05/25 04:57:46  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.


****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "meshedit.h"
#include <wrap/gl/pick.h>
using namespace vcg;

ExtraMeshEditPlugin::ExtraMeshEditPlugin() {
  isDragging=false;

	actionList << new QAction(QIcon(":/images/select_face.png"),"Select Faces in a region", this);
  QAction *editAction;
  foreach(editAction, actionList)
    editAction->setCheckable(true);
      
}

QList<QAction *> ExtraMeshEditPlugin::actions() const {
	return actionList;
}


 const ActionInfo &ExtraMeshEditPlugin::Info(QAction *action) 
 {
   static ActionInfo ai; 
  
	if( action->text() == tr("Invert Selection") )
		{
			ai.Help = tr("Apply Loop's Subdivision Surface algorithm, it is an approximate method");
			ai.ShortHelp = tr("Apply Loop's Subdivision Surface algorithm");
		}
   return ai;
 }

 const PluginInfo &ExtraMeshEditPlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr("__DATE__");
	 ai.Version = tr("0.5");
	 ai.Author = ("Paolo Cignoni");
   return ai;
 } 
  void ExtraMeshEditPlugin::mousePressEvent    (QAction *, QMouseEvent * event, MeshModel &m, GLArea * gla)
  {
    LastSel.clear();

    if(event->modifiers() == Qt::ControlModifier || 
       event->modifiers() == Qt::ShiftModifier )
      {
        CMeshO::FaceIterator fi;
        for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
            if(!(*fi).IsD() && (*fi).IsS()) 
                    LastSel.push_back(&*fi);        
      }  
      
    selMode=SMClear;
    if(event->modifiers()==Qt::ControlModifier) selMode=SMAdd;
    if(event->modifiers()==Qt::ShiftModifier) selMode=SMSub;

    start=event->pos();
    cur=start;
    return;
  }
  
  void ExtraMeshEditPlugin::mouseMoveEvent     (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
  {
    prev=cur;
    cur=event->pos();
    isDragging = true;
    
    // now the management of the update 
    static int lastMouse=0;
    static int lastRendering=0;
    int curT = clock();
    if(gla->deltaTime < 50 )     gla->update();
    else{
      gla->makeCurrent ();
      glDrawBuffer(GL_FRONT);
      DrawXORRect(gla,true);
      glDrawBuffer(GL_BACK);
      glFlush();
    }
  }
  
  void ExtraMeshEditPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
  {
    gla->update();
    prev=cur;
    cur=event->pos();
  }
  void ExtraMeshEditPlugin::DrawXORRect(GLArea * gla, bool doubleDraw)
  {  
    glMatrixMode(GL_PROJECTION);
	  glPushMatrix();
	  glLoadIdentity();
	  glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
	  glMatrixMode(GL_MODELVIEW);
	  glPushMatrix();
	  glLoadIdentity();
	  glPushAttrib(GL_ENABLE_BIT);
	  glDisable(GL_DEPTH_TEST);
	  glDisable(GL_LIGHTING);
	  glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glColor3f(1,1,1);
    if(doubleDraw)
    {
      glBegin(GL_LINE_LOOP);
        glVertex2f(start.x(),start.y());
        glVertex2f(prev.x(),start.y());
        glVertex2f(prev.x(),prev.y());
        glVertex2f(start.x(),prev.y());
      glEnd();
    }
    glBegin(GL_LINE_LOOP);
      glVertex2f(start.x(),start.y());
      glVertex2f(cur.x(),start.y());
      glVertex2f(cur.x(),cur.y());
      glVertex2f(start.x(),cur.y());
    glEnd();
    glDisable(GL_LOGIC_OP);
    
    // Closing 2D
  	glPopAttrib();
	  glPopMatrix(); // restore modelview
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();
	  glMatrixMode(GL_MODELVIEW);

  }
  void ExtraMeshEditPlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla)
  {
    if(isDragging)
    {
    DrawXORRect(gla,false);
    vector<CMeshO::FacePointer>::iterator fpi;
    // Starting Sel 
    vector<CMeshO::FacePointer> NewSel;  
    QPoint mid=(start+cur)/2;
    mid.setY(gla->curSiz.height()-  mid.y());
    QPoint wid=(start-cur);
    if(wid.x()<0)  wid.setX(-wid.x());
    if(wid.y()<0)  wid.setY(-wid.y());

 /*   CMeshO::FaceIterator fi;
    for(fi=m.cm.face.begin(),fpi=NewSel.begin();fpi!=NewSel.end();++fi) 
      if(!(*fi).IsD()) {
          if(&(*fi)!=*fpi) (*fpi)->ClearS();
        else {
          (*fpi)->SetS();
          ++fpi;
        }
      }

    for(;fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD()) (*fi).ClearS();

*/
   
   CMeshO::FaceIterator fi;
    for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD()) (*fi).ClearS();

    GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, NewSel, wid.x(), wid.y());
    qDebug("Pickface: rect %i %i - %i %i",mid.x(),mid.y(),wid.x(),wid.y());
    qDebug("Pickface: Got  %i on %i",NewSel.size(),m.cm.face.size());
   
    switch(selMode)
    { 
      case SMSub :
      for(fpi=LastSel.begin();fpi!=LastSel.end();++fpi)
          (*fpi)->SetS();
      for(fpi=NewSel.begin();fpi!=NewSel.end();++fpi)
          (*fpi)->ClearS();
      break;
      case SMAdd :
        for(fpi=LastSel.begin();fpi!=LastSel.end();++fpi)
          (*fpi)->SetS(); 
      case SMClear :
        for(fpi=NewSel.begin();fpi!=NewSel.end();++fpi)
          (*fpi)->SetS();
        break;
    }
     isDragging=false;
    }   

  }

void ExtraMeshEditPlugin::StartEdit(QAction * /*mode*/, MeshModel &m, GLArea * /*parent*/)
{
 LastSel.clear();
 CMeshO::FaceIterator fi;
 for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
 if(!(*fi).IsD() && (*fi).IsS() )
      LastSel.push_back(&*fi);

}



Q_EXPORT_PLUGIN(ExtraMeshEditPlugin)
