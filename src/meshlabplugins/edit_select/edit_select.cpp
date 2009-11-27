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

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <limits>
#include <meshlab/glarea.h>
#include "edit_select.h"
#include <wrap/gl/pick.h>
#include <vcg/complex/trimesh/update/selection.h>


using namespace std;
using namespace vcg;

ExtraMeshEditPlugin::ExtraMeshEditPlugin(bool ConnectedMode) :connectedMode(ConnectedMode) {
  isDragging=false;      
}

QString ExtraMeshEditPlugin::Info() 
{
	return tr("Interactive selection of faces inside a dragged rectangle in screen space");
}
void ExtraMeshEditPlugin::keyReleaseEvent  (QKeyEvent *, MeshModel &/*m*/, GLArea *gla)
{
	gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));	
}

void ExtraMeshEditPlugin::keyPressEvent    (QKeyEvent *event, MeshModel &/*m*/, GLArea *gla)
{
	
	if(QApplication::keyboardModifiers() ==  Qt::AltModifier)  gla->setCursor(QCursor(QPixmap(":/images/sel_rect_eye.png"),1,1));	
	if(QApplication::keyboardModifiers() ==  Qt::ControlModifier) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus.png"),1,1));	
	if(QApplication::keyboardModifiers() ==  Qt::ShiftModifier) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus.png"),1,1));	
	if(QApplication::keyboardModifiers() == (Qt::AltModifier+Qt::ControlModifier)) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus_eye.png"),1,1));	
	if(QApplication::keyboardModifiers() == (Qt::AltModifier+Qt::ShiftModifier)) 	gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus_eye.png"),1,1));	
}

void ExtraMeshEditPlugin::mousePressEvent(QMouseEvent * event, MeshModel &m, GLArea *)
{
    LastSel.clear();
		
    if((event->modifiers() & Qt::ControlModifier) || 
       (event->modifiers() & Qt::ShiftModifier)  )
      {
        CMeshO::FaceIterator fi;
        for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
            if(!(*fi).IsD() && (*fi).IsS()) 
                    LastSel.push_back(&*fi);        
      }  
      
    selMode=SMClear;
    if(event->modifiers() & Qt::ControlModifier) selMode=SMAdd;
    if(event->modifiers() & Qt::ShiftModifier) selMode=SMSub;
		
    if(event->modifiers() & Qt::AltModifier)		selectFrontFlag=true;
																				else	selectFrontFlag=false;

    start=event->pos();
    cur=start;
    return;
  }
  
  void ExtraMeshEditPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
  {
    prev=cur;
    cur=event->pos();
    isDragging = true;

    // to avoid too frequent rendering 
    if(gla->lastRenderingTime() < 200 )
    {
      gla->update();
    }
    else{
      gla->makeCurrent();
      glDrawBuffer(GL_FRONT);
      DrawXORRect(gla,true);
      glDrawBuffer(GL_BACK);
      glFlush();
    }
  }
  
  void ExtraMeshEditPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
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

  void ExtraMeshEditPlugin::Decorate(MeshModel &m, GLArea * gla)
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

		glPushMatrix();
		glMultMatrix(m.cm.Tr);
		if(selectFrontFlag)	GLPickTri<CMeshO>::PickFaceVisible(mid.x(), mid.y(), m.cm, NewSel, wid.x(), wid.y());
									 else GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, NewSel, wid.x(), wid.y());

    qDebug("Pickface: rect %i %i - %i %i",mid.x(),mid.y(),wid.x(),wid.y());
    qDebug("Pickface: Got  %i on %i",int(NewSel.size()),int(m.cm.face.size()));
		glPopMatrix();
    tri::UpdateSelection<CMeshO>::ClearFace(m.cm);
		switch(selMode)
    { 
      case SMSub :  // Subtract mode : The faces in the rect must be de-selected
				if(connectedMode)
					{
					 	for(fpi=NewSel.begin();fpi!=NewSel.end();++fpi)
								(*fpi)->SetS();
						tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
						
						CMeshO::FaceIterator fi;
						NewSel.clear();
						for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
								if(!(*fi).IsD() && (*fi).IsS()) NewSel.push_back(&*fi);
					}
				// Normal case: simply deselect what has been selected. 
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
					if(connectedMode) tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
        break;
    }
     isDragging=false;
    }   

  }

bool ExtraMeshEditPlugin::StartEdit(MeshModel &m, GLArea *gla )
{
 LastSel.clear();
 CMeshO::FaceIterator fi;
 for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
 if(!(*fi).IsD() && (*fi).IsS() )
      LastSel.push_back(&*fi);
	  
 gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));	

 connect(this, SIGNAL(setSelectionRendering(bool)),gla,SLOT(setSelectionRendering(bool)) );

 setSelectionRendering(true);
 
 if(connectedMode) 
	m.updateDataMask(MeshModel::MM_FACEFACETOPO);
 return true;
}
